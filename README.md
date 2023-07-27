[![PlatformIO CI](https://github.com/matthewturner/Eventually-StateMachineListener/actions/workflows/platformio.yml/badge.svg)](https://github.com/matthewturner/Eventually-StateMachineListener/actions/workflows/platformio.yml)

# Eventually - State Machine

Built on the [Eventually](https://github.com/matthewturner/Eventually) library, this provides a listener which can act as a non-blocking state machine. This works alongside any other listener you might already have in your sketch.

## Usage

```
EvtManager mgr;
EvtStateMachineListener stateMachine;

void setup()
{
    // register other listeners as normal
    mgr.addListener(new EvtPinListener(...));

    // set the initial state
    stateMachine.transition(IDLE);

    // define the states and transitions
    stateMachine.when(IDLE, (EvtAction)idle, PENDING, STATE_FAILED, 500);
    stateMachine.when(PENDING, (EvtAction)pending, IN_PROGRESS);
    stateMachine.when(IN_PROGRESS, (EvtAction)inProgress, IDLE, STATE_FAILED, 500);

    // optionally define the transition when an interrupt occurs
    stateMachine.whenInterrupted(IDLE, PENDING);

    // register the state machine with
    mgr.addListener(&stateMachine);
}
```

### Ordering of listeners

Currently, the state machine listener will always return true regardless of the individual state methods. Therefore, the state machine should be registered with the manager/context *last* otherwise no other listeners will fire.

## Installing Platform IO

Install command line tools by following the installation instructions for [Windows](https://docs.platformio.org/en/latest/core/installation.html#windows)

## Serial Monitor

Deploy code to your Arduino and run the following command in a terminal:

```powershell
 pio device monitor --eol=CRLF --echo --filter=send_on_enter
```

## Unit Testing

Run the following command after installing Platform IO:

```powershell
pio test -e native
```
