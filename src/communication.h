#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <Arduino.h>
#include <WiFi.h>
#include <MQTTPubSubClient.h>

#include <vector>
#include <map>

typedef std::function<void(const char *, size_t)> subscription_callback;

class Communication
{
public:
    Communication();
    ~Communication();

    void begin();
    void loop();

    bool isConnected() const;
    bool connectToRemote();

    bool publish(const char *topic, const char *payload, bool retain = false);
    bool publish(const char *topic, uint8_t payload, bool retain = false);

    bool subscribe(const char *topic, uint8_t qos, const subscription_callback &callback);

    WiFiClient m_wifi;
    MQTTPubSubClient m_mqtt;

private:
    const char *mqttTopic(const char *topic) const;

    bool connectToBroker();
    bool subscribeToTopic(const char *topic, uint8_t qos, const subscription_callback &callback);

    const char *m_topicPrefix;
    mutable char m_topicBuffer[60];
    std::map<String, subscription_callback> m_subCallbacks;
};

#endif