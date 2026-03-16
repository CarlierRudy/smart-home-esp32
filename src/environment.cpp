#include "environment.h"

#include <pins.h>
#include <config.h>

#include <DHT.h>

DHT TempSensor(TEMPERATURE_SENSOR_PIN, DHT11);

Environment::Environment()
    : m_lastReadTime(0)
    , m_refreshInterval(ENVIRONMENT_DATA_UPDATE_INTERVAL)
    , m_lightLevel(0)
    , m_lightChanged(true)
    , m_tempValue(0)
    , m_tempChanged(true)
    , m_humidityValue(0)
    , m_humidityChanged(true)
    , m_propaneDetected(false)
    , m_propaneChanged(true)
    , m_waterLevel(0)
    , m_waterLevelChanged(true)
{
}

Environment::~Environment()
{
}

uint8_t Environment::lightLevel()
{
    m_lightChanged = false;
    return m_lightLevel;
}

bool Environment::lightLevelHasChanged()
{
    return m_lightChanged;
}

uint8_t Environment::temperature()
{
    m_tempChanged = false;
    return m_tempValue;
}

bool Environment::temperatureHasChanged()
{
    return m_tempChanged;
}

uint8_t Environment::humidity()
{
    m_humidityChanged = false;
    return m_humidityValue;
}

bool Environment::humidityHasChanged()
{
    return m_humidityChanged;
}

bool Environment::propaneDetected()
{
    m_propaneChanged = false;
    return m_propaneDetected;
}

bool Environment::propaneHasChanged()
{
    return m_propaneChanged;
}

uint8_t Environment::waterLevel()
{
    m_waterLevelChanged = false;
    return m_waterLevel;
}

bool Environment::waterLevelHasChanged()
{
    return m_waterLevelChanged;
}

void Environment::changeUpdateInterval(uint64_t interval)
{
    m_refreshInterval = (interval > ENVIRONMENT_DATA_UPDATE_INTERVAL ? interval : ENVIRONMENT_DATA_UPDATE_INTERVAL);

    Serial.print("Environmental data update interval: ");
    Serial.print(interval);
    Serial.println(" ms");
}

void Environment::begin()
{
    pinMode(LIGHT_SENSOR_PIN, INPUT);
    TempSensor.begin();
    pinMode(GAS_SENSOR_PIN, INPUT);
    pinMode(MOTION_SENSOR_PIN, INPUT);
    pinMode(WATER_SENSOR_PIN, INPUT);
}

void Environment::loop()
{
    if (m_lastReadTime > 0 && millis() - m_lastReadTime < m_refreshInterval)
        return;

    int lightReading = digitalRead(LIGHT_SENSOR_PIN);
    uint8_t lightLevel = (lightReading == HIGH ? 100 : 0);
    if (m_lightLevel != lightLevel) {
        m_lightLevel = lightLevel;
        m_lightChanged = true;
    }

    uint8_t temp = TempSensor.readTemperature(false);
    if (m_tempValue != temp) {
        m_tempValue = temp;
        m_tempChanged = true;
    }

    uint8_t humidity = TempSensor.readHumidity();
    if (m_humidityValue != humidity) {
        m_humidityValue = humidity;
        m_humidityChanged = true;
    }

    uint16_t gasReading = analogRead(GAS_SENSOR_PIN);
    bool propaneDetected = (gasReading >= PROPANE_MIN_VALUE && gasReading >= PROPANE_MAX_VALUE);
    if (m_propaneDetected != propaneDetected) {
        m_propaneDetected = propaneDetected;
        m_propaneChanged = m_propaneChanged;
    }

    uint16_t waterLevelReading = analogRead(WATER_SENSOR_PIN);
    uint8_t waterLevel = map(waterLevelReading, 0, 4095, 0, 100);
    if (m_waterLevel != waterLevel) {
        m_waterLevel = waterLevel;
        m_waterLevelChanged = true;
    }

    m_lastReadTime = millis();
}