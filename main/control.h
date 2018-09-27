/* ========================================================================== */
/*                                                                            */
/*   Filename.c                                                               */
/*   (c) 2012 Author                                                          */
/*                                                                            */
/*   Description                                                              */
/*                                                                            */
/* ========================================================================== */
#define CONTROL_QUEUE_MAX   10

typedef enum
{
   CONTROL_STOP,
   CONTROL_START,
   CONTROL_WAIT_REQUEST,
   CONTROL_CAN_RESPONSE_WAIT,
   CONTROL_SEND_RESPONSE //,
 //  CONTROL_RESPONSE_TIMEOUT,
 //  CONTROL_RESPONSE_TRANSMITTED
      
}ControlState;

typedef enum
{
  EV_START,
  EV_STOP, 
  EV_CAN_RECEIVED,
  EV_CAN_TRANSMITTED,
  EV_NET_TRANSMIT_START,
  EV_NET_TRANSMIT_END,
  EV_NET_TRANSMIT_TIMEOUT,
  EV_NET_RECEIVED
 
}ControlEvents;

void control_task(void *arg);