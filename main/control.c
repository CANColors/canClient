/* ========================================================================== */
/*                                                                            */
/*   Filename.c                                                               */
/*   (c) 2012 Author                                                          */
/*                                                                            */
/*   Description                                                              */
/*                                                                            */
/* ========================================================================== */
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#include "esp_err.h"
#include "esp_log.h"               
#define CONTROL_TAG "Control"

#include "control.h"       
#include "config.h"
//#include "testClient1.h"
                                 

extern   QueueHandle_t controlQueue;
extern SemaphoreHandle_t can_rx;
extern SemaphoreHandle_t can_tx;
extern SemaphoreHandle_t http_rx;
extern SemaphoreHandle_t http_tx;

void control_task(void *arg)
{
   ControlState cs;
  while(1)
  {
     xQueueReceive(controlQueue, &cs, portMAX_DELAY);
     switch(cs)
     {
          case CONTROL_START:
          {
             vTaskDelay(  HTTP_RECEIVE_TIMESLOT / portTICK_PERIOD_MS);
                xSemaphoreGive(http_rx); //Looking for a command
           ESP_LOGI(CONTROL_TAG, "CONTROL_START:Looking for a command");
              break;
             
          }
          
          case CONTROL_COMMAND:
          {
          
         //  testClientStep1();   //Prepare OBD command
              xSemaphoreGive(can_tx); //Send OBD command
            //if any data on CAN bus 
              xSemaphoreGive(can_rx);
              xSemaphoreGive(http_tx); // send it
               ESP_LOGI(CONTROL_TAG,"CONTROL_COMMAND");
             //    xSemaphoreGive(http_tx); // send it
              break;
          }
          
          case CONTROL_CAN_RESPONSE:
          {
             xSemaphoreGive(http_tx); // send it
               ESP_LOGI(CONTROL_TAG,"CONTROL_CAN_RESPONSE");
              break;
          
          }
          case CONTROL_RESPONSE_TRANSMITTED:
          {
           ESP_LOGI(CONTROL_TAG,"CONTROL_RESPONSE_TRANSMITTED");
            ControlState cs2 = CONTROL_START;    //return to start
            xQueueSend(controlQueue, &cs2, portMAX_DELAY); 
            break;
          }
          case CONTROL_STOP:
          {
          
            break;
          }
     }
  }

}

