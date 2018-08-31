/*
* extFsm.c
*/

#include "extFsm.h"
#include <stdio.h>

void	extFsmHandle(ext_fsm_t *fsm)
{
	unsigned char	i;
	unsigned char newState = EXT_STATE_CONTINUE;
	statemachine_t *_state = fsm->states;

	if(fsm->currentEvent == EXT_EVENT_NONE)
		return;

	while(_state->state != EXT_STATE_CONTINUE)
	{
		if( _state->state == fsm->currentState )
		{
			const transition_t *handle = _state->eventHandlers;

			for(i=0; i < _state->size; i++)
			{
				if(fsm->currentEvent == handle->event )
				{
					fsm->currentEvent = EXT_EVENT_NONE;
					newState = (handle->handle)();
					if(newState!= EXT_STATE_CONTINUE && newState != fsm->currentState )
					{
						fsm->currentState = newState;
					}
					return;
				}
				
				handle++;
			}

		}

		_state++;
	}
	

	printf("State Machine Error int state %d, event %d", fsm->currentState, fsm->currentEvent);
	
	return;
}

