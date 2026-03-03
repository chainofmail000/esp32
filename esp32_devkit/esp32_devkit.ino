/*
  ESP32 DevKit Starter Sketch

  Replace the section marked "YOUR FEATURE" with the code/logic from your shared link.
*/

#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif

const unsigned long BLINK_INTERVAL_MS = 500;
unsigned long lastBlinkMs = 0;
bool ledState = false;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.println();
  Serial.println("ESP32 starter running.");
  Serial.println("Paste your custom feature in loop() section marked YOUR FEATURE.");
}

void loop() {
  // -------- YOUR FEATURE START --------
  // Replace this default behavior with the code from your shared link.
  // -------- YOUR FEATURE END ----------

  // Default alive indicator (safe to remove after you add your feature).
  const unsigned long now = millis();
  if (now - lastBlinkMs >= BLINK_INTERVAL_MS) {
    lastBlinkMs = now;
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState ? HIGH : LOW);
    Serial.println(ledState ? "LED ON" : "LED OFF");
  }
}
