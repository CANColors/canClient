/* ========================================================================== */
/*                                                                            */
/*   Filename.c                                                               */
/*   (c) 2012 Author                                                          */
/*                                                                            */
/*   Description                                                              */
/*                                                                            */
/* ========================================================================== */


#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/can.h"
 
 #include "can.h"
 
extern SemaphoreHandle_t can_rx;
extern SemaphoreHandle_t can_tx;
extern SemaphoreHandle_t http_rx;
extern SemaphoreHandle_t http_tx;

extern QueueHandle_t rxCanQueue;
extern QueueHandle_t txCanQueue;
 
 
void testClientStep1(void)
{
  can_msg_timestamped msgt;

    msgt.id = 15;
    msgt.timestamp = 01;
    msgt.msg.identifier = 0x7DF;
    msgt.msg.data_length_code = 8;
    msgt.msg.data[0]=0x01;
    msgt.msg.data[1]=0x00;
    msgt.msg.data[2]=0x00;
    msgt.msg.data[3]=0x00;
    msgt.msg.data[4]=0x00;
    msgt.msg.data[5]=0x00;
    msgt.msg.data[6]=0x00;
    msgt.msg.data[7]=0x00;
    
     xQueueSend(txCanQueue, &msgt, portMAX_DELAY);
     xSemaphoreGive(can_tx);
     xSemaphoreGive(http_rx);

}