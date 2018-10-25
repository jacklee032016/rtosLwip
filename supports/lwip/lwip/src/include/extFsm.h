/*
* 
*/
#ifndef	__EXT_FSM_H__
#define	__EXT_FSM_H__

#define		WITH_VERSION_STATE			0

#include		"extSysParams.h"

#pragma		pack(1)

typedef	enum
{
	EXT_MEDIA_EVENT_CONNECT = EXT_SYS_EVENT_MAX+1, 		/* SDI connect from FPGA */
	EXT_MEDIA_EVENT_DISCONNECT, 						/* SDI disconnect from FPGA */
	EXT_MEDIA_EVENT_ACK, 								/* recv ACK after send set_param  */
	EXT_MEDIA_EVENT_NACK, 							/* recv NACK, eg 811 refuse this message after send set_param  */
	EXT_MEDIA_EVENT_TIMEOUT, 							/* timeout after send set_param */


}EXT_MEDIA_EVENT_T;



typedef	enum
{
	EXT_MEDIA_STATE_DISCONNECT = EXT_STATE_CONTINUE+1,
	EXT_MEDIA_STATE_CONNECT
}EXT_MEDIA_STATE_T;


struct _transition_t
{
	unsigned char			event;
	unsigned char 		(*handle)(void *);
};

typedef	struct _transition_t	transition_t;

struct	_transition_table_t
{
	unsigned char				state;
	
	unsigned char				size;
	const transition_t			*eventHandlers;
	
	void	(*enter_handle)(void *);
};

typedef	struct _transition_table_t	statemachine_t;

struct _fsm_t
{
	unsigned char 			currentState;
	unsigned char				currentEvent;
	
	void						*arg;
	
	const statemachine_t		*states;
	
};


typedef	struct _fsm_t				ext_fsm_t;


void	extFsmHandle(ext_fsm_t *fsm);


#endif

