#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <Arduino.h>

class Environment
{
public:
    Environment();
    ~Environment();

    uint8_t lightLevel();
    bool lightLevelHasChanged();

    uint8_t temperature();
    bool temperatureHasChanged();

    uint8_t humidity();
    bool humidityHasChanged();

    bool propaneDetected();
    bool propaneHasChanged();

    uint8_t waterLevel();
    bool waterLevelHasChanged();

    void changeUpdateInterval(uint64_t interval);

    void begin();
    void loop();

private:
    uint64_t m_lastReadTime;
    uint64_t m_refreshInterval;

    uint8_t m_lightLevel;
    bool m_lightChanged;

    uint8_t m_tempValue;
    bool m_tempChanged;

    uint8_t m_humidityValue;
    bool m_humidityChanged;

    bool m_propaneDetected;
    bool m_propaneChanged;

    uint8_t m_waterLevel;
    bool m_waterLevelChanged;
};

#endif