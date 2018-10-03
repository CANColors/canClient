/* ========================================================================== */
/*                                                                            */
/*   Filename.c                                                               */
/*   (c) 2012 Author                                                          */
/*                                                                            */
/*   Description                                                              */
/*                                                                            */
/* ========================================================================== */
#include "driver/can.h"

#define ISOTP_MF_BS      0xFF
#define ISOTP_MF_STmin   0x05 

typedef struct {
uint id;
uint32_t timestamp;
can_message_t msg;
}can_msg_timestamped;


void canInit(void);
void testCanDataGenerate(void);
void can_receive_task(void *arg);
void can_transmit_task(void *arg);