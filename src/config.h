#ifndef CONFIG_H
#define CONFIG_H

// Wifi Informations
#define WIFI_SSID "SMART HOME RTE"
#define WIFI_PASS "smart1234"

// MQTT Settings
#define MQTT_CLIENT_ID    "smart-home-14"
#define MQTT_BROKER_HOST  "broker.emqx.io"
#define MQTT_BROKER_PORT  1883
#define MQTT_TOPIC_PREFIX "smart-14"

// Monitoring
#define ENVIRONMENT_DATA_UPDATE_INTERVAL 3000

// Gas threshold
#define PROPANE_MIN_VALUE 1000
#define PROPANE_MAX_VALUE 3000

// Door lock settings
#define DOOR_UNLOCK_TIMEOUT 3000

#endif // CONFIG_H