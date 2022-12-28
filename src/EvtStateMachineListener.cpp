#include "EvtStateMachineListener.h"

EvtStateMachineListener::EvtStateMachineListener()
{
}

void EvtStateMachineListener::when(byte targetState, EvtAction action,
                                   byte successState, byte failureState,
                                   uint32_t transitionDelay)
{
    StateAction s;
    s.action = action;
    s.state = targetState;
    s.successState = successState;
    s.failureState = failureState;
    s.transitionDelay = transitionDelay;

    _stateActions[targetState] = s;
}

bool EvtStateMachineListener::performTriggerAction(EvtContext *ctx)
{
    byte currentState = _state;
    StateAction s = _stateActions[currentState];

    if (_actionExecuted)
    {
        if (!hasPassedTransitionDelay(s.transitionDelay))
        {
            // keep in this state and repeat
            return true;
        }

        if (s.successState == NO_TRANSITION)
        {
            // keep in this state and repeat action
            setTransitionTime(systemTime());
            _actionExecuted = false;
            return true;
        }

        transition(s.successState);
        return true;
    }

    bool result = (*s.action)(this, ctx);
    _actionExecuted = true;
    if (result)
    {
        if (s.transitionDelay == 0)
        {
            _actionExecuted = false;
            if (s.successState == NO_TRANSITION)
            {
                return true;
            }
            if (_state != s.state)
            {
                // state was externally set so don't
                // auto-transition
                return true;
            }
            transition(s.successState);
            return true;
        }
        // keep in this state and repeat
        return true;
    }

    transition(s.failureState);
    return true;
}

bool EvtStateMachineListener::hasPassedTransitionDelay(uint32_t transitionDelay)
{
    uint64_t durationInCurrentState = systemTime() - transitionTime();
    return durationInCurrentState > transitionDelay;
}

void EvtStateMachineListener::setTransitionTime(uint64_t timeInMs)
{
    _transitionTime = timeInMs;
}

void EvtStateMachineListener::onInterrupt()
{
    if (currentState() == _interruptHandler.guardState)
    {
        transition(_interruptHandler.targetState);
    }
}

void EvtStateMachineListener::whenInterrupted(byte guardState, byte targetState)
{
    _interruptHandler.guardState = guardState;
    _interruptHandler.targetState = targetState;
}

bool StateMachineListener::isEventTriggered()
{
    if (!EvtListener::isEventTriggered())
    {
        return false;
    }

    return _state != STATE_FAILED;
}

void EvtStateMachineListener::transition(byte newState)
{
    if (_state != newState)
    {
        _state = newState;
        _transitionTime = systemTime();
        _actionExecuted = false;
    }
}

byte EvtStateMachineListener::currentState()
{
    return _state;
}

uint64_t EvtStateMachineListener::transitionTime()
{
    return _transitionTime;
}

uint64_t EvtStateMachineListener::systemTime()
{
    return millis();
}

void EvtStateMachineListener::setupListener()
{
}