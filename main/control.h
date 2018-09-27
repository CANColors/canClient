/* ========================================================================== */
/*                                                                            */
/*   Filename.c                                                               */
/*   (c) 2012 Author                                                          */
/*                                                                            */
/*   Description                                                              */
/*                                                                            */
/* ========================================================================== */

typedef enum
{
   CONTROL_STOP,
   CONTROL_START,
   CONTROL_COMMAND,
   CONTROL_CAN_RESPONSE,
   CONTROL_RESPONSE_TRANSMITTED
      
}ControlState;


void control_task(void *arg);