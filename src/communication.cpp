#include "communication.h"

#include <config.h>

const char *_wifi_ssid = WIFI_SSID;
const char *_wifi_pass = WIFI_PASS;

Communication::Communication()
    : m_topicPrefix(MQTT_TOPIC_PREFIX)
    , m_wifi()
    , m_mqtt()
{
    m_mqtt.setWill(mqttTopic("state"), "disconnected", true, 1);
    m_mqtt.begin(m_wifi);
}

Communication::~Communication()
{
}

void Communication::begin()
{
}

void Communication::loop()
{
    connectToRemote();
    m_mqtt.update();
}

bool Communication::isConnected() const
{
    return false;
}

bool Communication::connectToRemote()
{
    uint32_t timeoutMs = 15000;

    if (WiFi.status() == WL_CONNECTED) {
        if (!m_mqtt.isConnected()) {
            if (!connectToBroker())
                return false;
        }

        return false;
    }

    Serial.printf("Connecting to WiFi: %s ", _wifi_ssid);

    WiFi.mode(WIFI_STA);           // optional - make sure we're in station mode
    WiFi.begin(_wifi_ssid, _wifi_pass);

    unsigned long start = millis();
    bool success = false;

    while (millis() - start < timeoutMs)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            success = true;
            break;
        }
        Serial.print(".");
        delay(400);                 // faster feedback, less blocking
    }

    if (success)
    {
        Serial.println("\nConnected! IP: " + WiFi.localIP().toString());
        // Optional: Serial.printf("RSSI: %d dBm\n", WiFi.RSSI());
        return true;
    }
    else
    {
        Serial.println("\nWiFi connection failed / timeout");
        WiFi.disconnect(true);      // clean up
        return false;
    }
}

bool Communication::publish(const char *topic, const char *payload, bool retain)
{
    if (m_mqtt.isConnected())
        return m_mqtt.publish(mqttTopic(topic), payload, retain);
    else
        return false;
}

bool Communication::publish(const char *topic, uint8_t payload, bool retain)
{
    char data[4];
    sprintf(data, "%d", payload);
    return publish(topic, data, retain);
}


bool Communication::subscribe(const char *topic, uint8_t qos, const subscription_callback &callback)
{
    m_subCallbacks[String(mqttTopic(topic))] = callback;

    if (m_mqtt.isConnected())
        return subscribeToTopic(mqttTopic(topic), qos, callback);
    else
        return false;
}

const char *Communication::mqttTopic(const char *topic) const
{
    sprintf(m_topicBuffer, "%s/%s", m_topicPrefix, topic);
    return m_topicBuffer;
}

bool Communication::connectToBroker()
{
    // MQTT(TCP) connection
    
        Serial.println("Connecting to MQTT(TCP) broker...");
        if (m_wifi.connect(MQTT_BROKER_HOST, MQTT_BROKER_PORT)) {
            Serial.println("Connected to MQTT(TCP) broker");

            Serial.println("Completing MQTT connection...");
            if (m_mqtt.connect(MQTT_CLIENT_ID, "", "")) {
                Serial.println("MQTT connection established !");

                // Resubscribing topics
                std::for_each(m_subCallbacks.begin(), m_subCallbacks.end(), [this](const std::pair<String, subscription_callback> &item) {
                    subscribeToTopic(item.first.c_str(), 0, item.second);
                });

                publish("state", "connected", false);
                return true;
            } else {
                Serial.println("MQTT connection failed");
            }
        } else {
            Serial.println("MQTT(TCP) connection failed");
        }

    return false;
}

bool Communication::subscribeToTopic(const char *topic, uint8_t qos, const subscription_callback &callback)
{
    if (!m_mqtt.isConnected()) {
        return false;
    }

    if (!m_mqtt.subscribe(topic, qos, callback)) {
        Serial.print("subscription to topic ");
        Serial.print(topic);
        Serial.println(" failed");
        return false;
    }

    Serial.print("subscribed to topic ");
    Serial.print(topic);
    return true;
}
