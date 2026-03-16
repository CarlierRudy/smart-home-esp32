#include "controller.h"

#include <pins.h>
#include <config.h>
#include <communication.h>

#include <Arduino.h>
#include <MFRC522.h>
#include <ESP32Servo.h>

MFRC522 RFIDModule;
Servo GateMotor;

Controller::Controller(Communication *comm)
    : m_statusLedOn(false)
    , m_statusBlinkTime(0)
    , m_lightOn(false)
    , m_doorLocked(true)
    , m_doorUnlockTime(0)
    , m_gateOpened(false)
    , m_comm(comm)
{
}

Controller::~Controller()
{
}

bool Controller::isLightOn() const
{
    return m_lightOn;
}

void Controller::switchLight(bool on)
{
    digitalWrite(LIGHT_OUTPUT_PIN, on ? HIGH : LOW);
    m_lightOn = on;

    m_comm->publish("light/state", on ? "true" : "false");
    Serial.print("Light turned ");
    Serial.println(on ? "on" : "off");
}

bool Controller::isDoorLocked() const
{
    return m_doorLocked;
}

void Controller::lockDoor()
{
    digitalWrite(DOOR_LOCK_PIN, LOW);
    m_doorLocked = true;
    m_doorUnlockTime = 0;

    Serial.println("Door locked !");
    m_comm->publish("door/locked", "true");
}

void Controller::unlockDoor()
{
    digitalWrite(DOOR_LOCK_PIN, HIGH);
    m_doorLocked = false;
    m_doorUnlockTime = millis();

    Serial.println("Door unlocked !");
    m_comm->publish("door/locked", "false");
}

bool Controller::isGateOpen() const
{
    return m_gateOpened;
}

void Controller::openGate()
{
    GateMotor.write(90);
    m_gateOpened = true;

    Serial.println("Gate opened !");
    m_comm->publish("gate/opened", "true");
}

void Controller::closeGate()
{
    GateMotor.write(0);
    m_gateOpened = false;

    Serial.println("Gate closed !");
    m_comm->publish("gate/opened", "false");
}

void Controller::begin()
{
    pinMode(STATUS_LED_PIN, OUTPUT);
    digitalWrite(STATUS_LED_PIN, HIGH);

    pinMode(LIGHT_OUTPUT_PIN, OUTPUT);
    pinMode(DOOR_LOCK_PIN, OUTPUT);

    RFIDModule.PCD_Init();
    Serial.print("RFID module firmware version: ");
    RFIDModule.PCD_DumpVersionToSerial();

    GateMotor.attach(GATE_DOOR_PIN);
    GateMotor.write(0);

    m_comm->subscribe("light/switch", 0, [this](const char *data, size_t len) {
        if (strncmp("true", data, len) == 0)
            switchLight(true);
        else if (strncmp("false", data, len) == 0)
            switchLight(false);
    });

    m_comm->subscribe("door/lock", 0, [this](const char *data, size_t len) {
        if (strncmp("true", data, len) == 0)
            lockDoor();
        else
            unlockDoor();
    });

    m_comm->subscribe("gate/open", 0, [this](const char *data, size_t len) {
        if (strncmp("true", data, len) == 0)
            openGate();
        else if (strncmp("false", data, len) == 0)
            closeGate();
    });
}

void Controller::loop()
{
    if (millis() - m_statusBlinkTime > 1000) {
        m_statusLedOn = !m_statusLedOn;
        digitalWrite(STATUS_LED_PIN, m_statusLedOn ? HIGH : LOW);
        m_statusBlinkTime = millis();
    }

    if (!m_doorLocked && millis() - m_doorUnlockTime >= DOOR_UNLOCK_TIMEOUT) {
        lockDoor();
    }

    if (RFIDModule.PICC_IsNewCardPresent()) {
        Serial.println("Reading RFID card...");
        if (RFIDModule.PICC_ReadCardSerial()) {
            String uid;
            for (byte i = 0; i < RFIDModule.uid.size; i++)
                uid += RFIDModule.uid.uidByte[i];

            Serial.print("Card UID: ");
            Serial.println(uid);

            m_comm->publish("rfid/uid", uid.c_str());

            RFIDModule.PICC_HaltA();
            RFIDModule.PCD_StopCrypto1();
        }
    }   
}
