#!/usr/bin/env python3
"""Download an ESP32 Arduino project from GitHub and prepare an IDE-ready sketch folder."""

from __future__ import annotations

import argparse
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

EXCLUDE_DIRS = {".git", ".github", ".vscode", ".idea", "build", ".pio", "venv", ".venv"}


def run(cmd: list[str], cwd: Path | None = None) -> None:
    result = subprocess.run(cmd, cwd=str(cwd) if cwd else None, capture_output=True, text=True)
    if result.returncode != 0:
        raise RuntimeError(f"Command failed: {' '.join(cmd)}\n{result.stderr.strip()}")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Clone a GitHub Arduino/ESP32 project and normalize it for Arduino IDE upload."
    )
    parser.add_argument("repo", help="GitHub clone URL, e.g. https://github.com/user/repo.git")
    parser.add_argument("--ref", default="main", help="Branch or tag to clone (default: main)")
    parser.add_argument(
        "--project-name",
        default=None,
        help="Output sketch folder name; defaults to repo name",
    )
    parser.add_argument(
        "--subdir",
        default=None,
        help="Optional subdirectory inside the cloned repo that contains the Arduino project",
    )
    parser.add_argument(
        "--output-dir",
        default="downloads",
        help="Parent output directory (default: downloads)",
    )
    parser.add_argument(
        "--force",
        action="store_true",
        help="Overwrite existing output folder",
    )
    return parser.parse_args()


def repo_name_from_url(url: str) -> str:
    name = url.rstrip("/").split("/")[-1]
    if name.endswith(".git"):
        name = name[:-4]
    return name


def find_primary_ino(project_root: Path) -> Path:
    ino_files = [p for p in project_root.rglob("*.ino") if ".git" not in p.parts and ".github" not in p.parts]
    if not ino_files:
        raise FileNotFoundError("No .ino file found in cloned project")

    repo_name = project_root.name.lower()
    for candidate in ino_files:
        if candidate.name.lower() == f"{repo_name}.ino":
            return candidate
    for candidate in ino_files:
        if candidate.name.lower() == "main.ino":
            return candidate
    return sorted(ino_files)[0]


def copy_project_tree(src_root: Path, dst_root: Path) -> None:
    for item in src_root.iterdir():
        if item.name in EXCLUDE_DIRS:
            continue
        target = dst_root / item.name
        if item.is_dir():
            shutil.copytree(item, target)
        else:
            shutil.copy2(item, target)


def write_upload_notes(target_root: Path, sketch_name: str) -> None:
    notes = f"""# Upload notes for {sketch_name}

1. Open Arduino IDE.
2. Install **ESP32 by Espressif Systems** in Boards Manager.
3. Open the folder `{target_root}` as a sketch.
4. Select your board (for example: **ESP32 Dev Module**) and COM port.
5. Click **Upload**.

If WiFi is used in the project, update credentials in source before uploading.
"""
    (target_root / "UPLOAD_NOTES.md").write_text(notes)


def main() -> int:
    args = parse_args()
    repo_name = repo_name_from_url(args.repo)
    sketch_name = args.project_name or repo_name

    output_parent = Path(args.output_dir)
    output_parent.mkdir(parents=True, exist_ok=True)
    output_path = output_parent / sketch_name

    if output_path.exists():
        if not args.force:
            print(f"Output folder already exists: {output_path}. Use --force to overwrite.")
            return 2
        shutil.rmtree(output_path)

    with tempfile.TemporaryDirectory(prefix="gh-esp32-") as tmp:
        tmp_path = Path(tmp)
        clone_path = tmp_path / repo_name

        clone_cmd = ["git", "clone", "--depth", "1", "--branch", args.ref, args.repo, str(clone_path)]
        try:
            run(clone_cmd)
        except RuntimeError:
            fallback_cmd = ["git", "clone", "--depth", "1", args.repo, str(clone_path)]
            run(fallback_cmd)

        source_root = clone_path / args.subdir if args.subdir else clone_path
        if not source_root.exists():
            raise FileNotFoundError(f"Subdirectory not found in repository: {args.subdir}")

        primary_ino = find_primary_ino(source_root)
        project_root = primary_ino.parent

        output_path.mkdir(parents=True, exist_ok=True)
        copy_project_tree(project_root, output_path)

        copied_ino = output_path / primary_ino.name
        target_ino = output_path / f"{sketch_name}.ino"
        if copied_ino.exists() and copied_ino != target_ino:
            copied_ino.rename(target_ino)
        elif not target_ino.exists():
            ino_files = list(output_path.glob("*.ino"))
            if ino_files:
                ino_files[0].rename(target_ino)

        write_upload_notes(output_path, sketch_name)

    print(f"Prepared IDE-ready project at: {output_path}")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except Exception as exc:  # keep entry-point errors user-friendly
        print(f"ERROR: {exc}", file=sys.stderr)
        raise SystemExit(1)
