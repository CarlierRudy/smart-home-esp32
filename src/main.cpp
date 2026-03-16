#include <Arduino.h>
#include <SPI.h>

#include "communication.h"
#include "environment.h"
#include "controller.h"

Communication Com;
Environment Env;
Controller Control(&Com);

void setup()
{
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Booting...");

  SPI.begin();

  Com.begin();
  Env.begin();
  Control.begin();

  Com.subscribe("env/update", 1, [](const char *payload, size_t len) {
    uint64_t interval = atoi(payload) * 1000;
    Env.changeUpdateInterval(interval);
  });
}

void loop()
{
  // Environmental data

  if (Env.lightLevelHasChanged())
    Com.publish("light/level", Env.lightLevel());

  if (Env.temperatureHasChanged())
    Com.publish("temperature", Env.temperature(), true);

  if (Env.humidityHasChanged())
    Com.publish("humidity", Env.humidity(), true);

  if (Env.waterLevelHasChanged())
    Com.publish("water/level", Env.waterLevel(), true);

  // Routine processing
  Com.loop();
  Env.loop();
  Control.loop();

  // Break time
  delay(1000);
}