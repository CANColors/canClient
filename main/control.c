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
#include <esp_log.h>

#include "control.h"
#include "config.h"
#include "mqtt.h"



extern SemaphoreHandle_t can_rx;
extern SemaphoreHandle_t can_tx;
//extern SemaphoreHandle_t net_rx;
//extern SemaphoreHandle_t net_tx;
extern SemaphoreHandle_t obd_tx_wait;

static const char *CONTROL_TAG="CONTROL";
ControlState cs = CONTROL_START;
ControlState prevState = CONTROL_START;
QueueHandle_t controlEvents;

static void changeState(ControlState newState);   

static void controlStartState(ControlEvents ev);
static void controlWaitRequestState(ControlEvents ev);
static void controlCAN_Response_WaitState(ControlEvents ev);
static void controlSendResponseState(ControlEvents ev);


/*****************************************************************************/
/*        CONTROL_TASK                                                       */
/*****************************************************************************/ 
void control_task(void *arg)
{
   ControlEvents ev;
   
   controlEvents = xQueueCreate( CONTROL_QUEUE_MAX, sizeof( ControlEvents) );
   ControlEvents cs2 = EV_START;    //return to start
   xQueueSend(controlEvents, &cs2, portMAX_DELAY);
   
  while(1)
  {
     xQueueReceive(controlEvents, &ev, portMAX_DELAY);
  //    ESP_LOGI(CONTROL_TAG, "CONTROl state: %d ,event %d", cs,ev );
     switch(cs)
     {
          case CONTROL_START:
          {
              ESP_LOGI(CONTROL_TAG, "CONTROL_START" );
              controlStartState(ev);
              break;
          }
          
          case  CONTROL_WAIT_REQUEST:
          {
              ESP_LOGI(CONTROL_TAG, " CONTROL_WAIT_REQUEST" );
               controlWaitRequestState(ev);
              break;
          }
          
          case CONTROL_CAN_RESPONSE_WAIT:
          {
              ESP_LOGI(CONTROL_TAG, "CONTROL_CAN_RESPONSE_WAIT" );
              controlCAN_Response_WaitState(ev);
              break;
          }  
                   
          case CONTROL_SEND_RESPONSE:
          {
            ESP_LOGI(CONTROL_TAG, "CONTROL_SEND_RESPONSE" );
            controlSendResponseState(ev);
               break;
          
          }
          
          case CONTROL_STOP: { break; }
       //   case CONTROL_RESPONSE_TIMEOUT: { break; }
          default:break;
     }
  }

}


/*****************************************************************************/
/*                  START STATE                                              */
/*****************************************************************************/ 
void controlStartState(ControlEvents ev)
{
 //  ESP_LOGI(CONTROL_TAG, " controlStartState =>%d", ev );
  switch (ev)
  {
   case EV_START:
   {  
      receptionStateSet(TRUE); 
      changeState ( CONTROL_WAIT_REQUEST);
    break;
   }
   case EV_STOP: { break; }
   case EV_CAN_RECEIVED: { break;}
   case EV_CAN_TRANSMITTED:{break;}
   case EV_NET_TRANSMIT_START:{break;}
   case EV_NET_TRANSMIT_END: {break;}
   case EV_NET_TRANSMIT_TIMEOUT:{break;}
   case EV_NET_RECEIVED:{break;}
  
   default: break;
  }
}

/*****************************************************************************/
/*                WAIT REQUEST                                               */
/*****************************************************************************/ 
void controlWaitRequestState(ControlEvents ev)
{

//ESP_LOGI(CONTROL_TAG, "controlWaitRequestState" );
  switch (ev)
  {
   case EV_START:{break;}
   case EV_STOP: { break; }
   case EV_CAN_RECEIVED: { break;}
   case EV_CAN_TRANSMITTED:{break;}
   case EV_NET_TRANSMIT_START:{break;}
   case EV_NET_TRANSMIT_END: {break;}
   case EV_NET_TRANSMIT_TIMEOUT:{break;}
   case EV_NET_RECEIVED:
   {
      xSemaphoreGive(can_tx);
      xSemaphoreGive(can_rx);
      changeState (CONTROL_CAN_RESPONSE_WAIT);
    break;
    }
    
   default: break;
  }
}

/*****************************************************************************/
/*         CAN RESPONSE WAIT                                                 */
/*****************************************************************************/ 
void controlCAN_Response_WaitState(ControlEvents ev)
{
  switch (ev)
  {
   case EV_START:{break;}
   case EV_STOP: { break; }
   case EV_CAN_RECEIVED:
    {
        transmitionStateSet(TRUE);
        changeState (CONTROL_SEND_RESPONSE);
       break;
    }
   case EV_CAN_TRANSMITTED:{break;}
   case EV_NET_TRANSMIT_START:{break;}
   case EV_NET_TRANSMIT_END: {break;}
   case EV_NET_TRANSMIT_TIMEOUT:{break;}
   case EV_NET_RECEIVED:{break;}
   default: break;
  }
}


/*****************************************************************************/
/*         SEND RESPONSE                                                     */
/*****************************************************************************/ 
void controlSendResponseState(ControlEvents ev)
{
   // ESP_LOGI(CONTROL_TAG, " controlSendResponseState" );
  switch (ev)
  {
   case EV_START:{break;}
   case EV_STOP: { break; }
   case EV_CAN_RECEIVED: { break;}
   case EV_CAN_TRANSMITTED:{break;}
   case EV_NET_TRANSMIT_START:{break;}
   case EV_NET_TRANSMIT_END: 
   {
 //   ESP_LOGI(CONTROL_TAG, " EV_NET_TRANSMIT_END" );
        changeState (CONTROL_START);
  //       ESP_LOGI(CONTROL_TAG, " state:%d", cs );
        ControlEvents cs2 = EV_START;    //return to start
        xQueueSend(controlEvents, &cs2, portMAX_DELAY);
      break;
   }
   case EV_NET_TRANSMIT_TIMEOUT:{break;}
   case EV_NET_RECEIVED:{break;}
   default: break;
  }
}

/*****************************************************************************/
/*         change state                                                      */
/*****************************************************************************/
void changeState(ControlState newState)
{
  prevState = cs;
  cs = newState;

}

/*****************************************************************************/
/*         SEND RESPONSE                                                     */
/*****************************************************************************/
/*
void controlState(ControlEvents ev)
{
  switch (ev)
  {
   case EV_START:{break;}
   case EV_STOP: { break; }
   case EV_CAN_RECEIVED: { break;}
   case EV_CAN_TRANSMITTED:{break;}
   case EV_NET_TRANSMIT_START:{break;}
   case EV_NET_TRANSMIT_END: {break;}
   case EV_NET_TRANSMIT_TIMEOUT:{break;}
   case EV_NET_RECEIVED:{break;}
   default: break;
  }
}
 */
 