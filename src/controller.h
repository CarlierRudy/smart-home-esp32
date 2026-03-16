#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <Arduino.h>

class Communication;

class Controller
{
public:
    Controller(Communication *comm);
    ~Controller();

    bool isLightOn() const;
    void switchLight(bool on);

    bool isDoorLocked() const;
    void lockDoor();
    void unlockDoor();

    bool isGateOpen() const;
    void openGate();
    void closeGate();

    void begin();
    void loop();

private:
    bool m_statusLedOn;
    uint64_t m_statusBlinkTime;

    bool m_lightOn;

    bool m_doorLocked;
    uint64_t m_doorUnlockTime;

    bool m_gateOpened;

    Communication *m_comm;
};

#endif