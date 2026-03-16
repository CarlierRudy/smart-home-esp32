#ifndef PINS_H_H
#define PINS_H_H

// On board (ESP32)
#define STATUS_LED_PIN         2
#define BUZZER_PIN             17

// Environment sensors
#define LIGHT_SENSOR_PIN       16
#define TEMPERATURE_SENSOR_PIN 4
#define GAS_SENSOR_PIN         33
#define MOTION_SENSOR_PIN      27
#define WATER_SENSOR_PIN       32

// Relays
#define LIGHT_OUTPUT_PIN       25
#define DOOR_LOCK_PIN          26
#define GATE_DOOR_PIN          21

// RFID
#define RFID_SS_PIN            5
#define RFID_RST_PIN           22

#endif // PINS_H