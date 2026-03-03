#include "wifi.h"

#include <WebServer.h>
#include <WiFi.h>

namespace {
WebServer server(80);
bool autoMode = true;

const char* kSsid = "YOUR_SSID";
const char* kPass = "YOUR_PASSWORD";

String stateToString(SystemState state) {
  switch (state) {
    case SystemState::ACTIVE:
      return "ACTIVE";
    case SystemState::IDLE:
      return "IDLE";
    case SystemState::SLEEP:
      return "SLEEP";
  }
  return "UNKNOWN";
}

void setupRoutes(SensorData* sensorData, LightingState* lighting, SystemState* systemState) {
  server.on("/", [sensorData, lighting, systemState]() {
    String html = "<html><body><h2>Adaptive Presence Light</h2>";
    html += "<p>Distance: " + String(sensorData->distanceCm, 1) + " cm</p>";
    html += "<p>State: " + stateToString(*systemState) + "</p>";
    html += "<p>Mode: " + String(autoMode ? "AUTO" : "MANUAL") + "</p>";
    html += "<p><a href='/auto?enabled=1'>Auto On</a> | <a href='/auto?enabled=0'>Auto Off</a></p>";
    html += "<p><a href='/color?r=255&g=64&b=0'>Warm</a> | ";
    html += "<a href='/color?r=0&g=128&b=255'>Cool</a></p>";
    html += "</body></html>";
    server.send(200, "text/html", html);
  });

  server.on("/auto", [lighting]() {
    autoMode = server.arg("enabled") != "0";
    if (autoMode) {
      lighting->manualOverride = false;
    }
    server.send(200, "text/plain", autoMode ? "AUTO" : "MANUAL");
  });

  server.on("/color", [lighting]() {
    autoMode = false;
    lighting->manualOverride = true;
    lighting->target.r = static_cast<uint8_t>(server.arg("r").toInt());
    lighting->target.g = static_cast<uint8_t>(server.arg("g").toInt());
    lighting->target.b = static_cast<uint8_t>(server.arg("b").toInt());
    server.send(200, "application/json",
                "{\"ok\":true,\"manual\":true,\"r\":" + String(lighting->target.r) +
                    ",\"g\":" + String(lighting->target.g) + ",\"b\":" +
                    String(lighting->target.b) + "}");
  });

  server.begin();
}
}  // namespace

namespace WiFiControl {
void begin() {
  if (!Config::ENABLE_WIFI) {
    return;
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(kSsid, kPass);
  uint32_t startMs = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startMs < 10000) {
    delay(250);
  }
}

void handle(const SensorData& data, LightingState& light, SystemState state) {
  if (!Config::ENABLE_WIFI) {
    return;
  }

  static bool initialized = false;
  static SensorData sharedData{};
  static LightingState sharedLight{};
  static SystemState sharedState = SystemState::ACTIVE;

  sharedData = data;
  sharedLight = light;
  sharedState = state;

  if (!initialized && WiFi.status() == WL_CONNECTED) {
    setupRoutes(&sharedData, &sharedLight, &sharedState);
    initialized = true;
  }

  if (initialized) {
    server.handleClient();
    if (sharedLight.manualOverride != light.manualOverride ||
        sharedLight.target.r != light.target.r || sharedLight.target.g != light.target.g ||
        sharedLight.target.b != light.target.b) {
      light.manualOverride = sharedLight.manualOverride;
      light.target = sharedLight.target;
    }
  }
}
}  // namespace WiFiControl
