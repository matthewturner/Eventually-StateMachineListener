#include <Eventually.h>
#include <EventuallyStateMachine.h>

#include <Arduino.h>

const byte IDLE = 0;
const byte PENDING = 1;
const byte IN_PROGRESS = 2;

EvtManager mgr;
EvtStateMachineListener stateMachine;

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        ;

    pinMode(13, OUTPUT);

    // set the initial state
    stateMachine.transition(IDLE);

    // define states and transitions
    stateMachine.when(IDLE, (EvtAction)idle, PENDING, STATE_FAILED, 5000);
    stateMachine.when(PENDING, (EvtAction)pending, IN_PROGRESS);
    stateMachine.when(IN_PROGRESS, (EvtAction)inProgress, IDLE, STATE_FAILED, 2000);
    stateMachine.whenInterrupted(IDLE, PENDING);

    // register the listener
    mgr.addListener(&stateMachine);

    pinMode(2, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(2), wakeUp, FALLING);

    Serial.println("Setup complete, continuing...");
}

USE_EVENTUALLY_LOOP(mgr)

void wakeUp()
{
    stateMachine.onInterrupt();
}

bool idle()
{
    Serial.println("Idling...");
    digitalWrite(13, LOW);
    return true;
}

bool pending()
{
    Serial.println("Pending...");
    return true;
}

bool inProgress()
{
    Serial.println("In progress...");
    digitalWrite(13, HIGH);
    return true;
}