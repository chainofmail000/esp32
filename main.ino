#include "config.h"
#include "lighting.h"
#include "power.h"
#include "sensor.h"
#include "state_machine.h"
#include "wifi.h"

SensorData gSensorData;
LightingState gLightingState;
SystemState gSystemState = SystemState::ACTIVE;
SemaphoreHandle_t gDataMutex;

void sensorTask(void*) {
  TickType_t lastWake = xTaskGetTickCount();
  while (true) {
    if (xSemaphoreTake(gDataMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
      Sensor::update(gSensorData);
      gSystemState = StateMachine::update(gSensorData, gSystemState);
      xSemaphoreGive(gDataMutex);
    }
    vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(Config::SENSOR_PERIOD_MS));
  }
}

void lightingTask(void*) {
  TickType_t lastWake = xTaskGetTickCount();
  while (true) {
    if (xSemaphoreTake(gDataMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
      if (gSystemState == SystemState::ACTIVE || gSystemState == SystemState::IDLE) {
        Lighting::updateFromSensors(gSensorData, gLightingState);
        if (gSystemState == SystemState::IDLE) {
          gLightingState.brightness = max<uint8_t>(gLightingState.brightness / 3, 8);
        }
      } else {
        gLightingState.target = {0, 0, 0};
      }
      Lighting::render(gLightingState);
      xSemaphoreGive(gDataMutex);
    }
    vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(Config::LIGHTING_PERIOD_MS));
  }
}

void wifiTask(void*) {
  TickType_t lastWake = xTaskGetTickCount();
  while (true) {
    if (xSemaphoreTake(gDataMutex, pdMS_TO_TICKS(20)) == pdTRUE) {
      WiFiControl::handle(gSensorData, gLightingState, gSystemState);
      xSemaphoreGive(gDataMutex);
    }
    vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(Config::WIFI_PERIOD_MS));
  }
}

void powerTask(void*) {
  TickType_t lastWake = xTaskGetTickCount();
  while (true) {
    if (xSemaphoreTake(gDataMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
      Power::handle(gSystemState);
      xSemaphoreGive(gDataMutex);
    }
    vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(Config::POWER_PERIOD_MS));
  }
}

void setup() {
  Serial.begin(115200);
  Sensor::begin();
  Lighting::begin();
  Power::begin();
  WiFiControl::begin();

  gSensorData.lastMotionMs = millis();
  gDataMutex = xSemaphoreCreateMutex();

  xTaskCreatePinnedToCore(sensorTask, "sensor_task", 4096, nullptr, 2, nullptr, 1);
  xTaskCreatePinnedToCore(lightingTask, "lighting_task", 4096, nullptr, 2, nullptr, 1);
  xTaskCreatePinnedToCore(wifiTask, "wifi_task", 6144, nullptr, 1, nullptr, 0);
  xTaskCreatePinnedToCore(powerTask, "power_task", 3072, nullptr, 1, nullptr, 0);
}

void loop() { vTaskDelay(portMAX_DELAY); }
