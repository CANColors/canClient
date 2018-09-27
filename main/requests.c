/* ========================================================================== */
/*                                                                            */
/*   Filename.c                                                               */
/*   (c) 2012 Author                                                          */
/*                                                                            */
/*   Description                                                              */
/*                                                                            */
/* ========================================================================== */

#include <string.h>
#include <esp_system.h>
#include <esp_log.h>
#include <cjson.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "can.h"
#include "http.h"
#include "timestamp.h"
#include "control.h"

extern const QueueHandle_t rxCanQueue;
extern QueueHandle_t txCanQueue;
extern QueueHandle_t controlQueue;

extern uint16_t cntTransaction;



char* request_login()
{

   uint8_t chipid[6];
   esp_efuse_mac_get_default(chipid);
    char buf[50];
    sprintf(buf,"%02x:%02x:%02x:%02x:%02x:%02x",chipid[0], chipid[1], chipid[2], chipid[3], chipid[4], chipid[5]);
  cJSON *root;
	root=cJSON_CreateObject();	
	//cJSON_AddItemToObject(root, "id",  cJSON_CreateIntArray(chipid, 6));
  cJSON_AddStringToObject(root,"id",		buf);
  cJSON_AddStringToObject(root,"status",		"OK");
  cJSON_AddStringToObject(root,"role",		"client");
  
  char* req =  cJSON_Print(root);
  
 // ESP_LOGI("Login request", "cJSON =>  %s", req );
  //post ("login", req );
  
  cJSON_Delete(root);
  //free(req);
  return req;

}


char* request_server_masterrequest(void)
{
  cJSON* msgJson = cJSON_CreateObject();
  cJSON* transaction = cJSON_CreateObject();
  
   UBaseType_t cntMsg = uxQueueMessagesWaiting( rxCanQueue );
  //  ESP_LOGI("SM request", "cntMsg =>  %d", cntMsg );
    if (cntMsg > 0)
    {
      cJSON_AddItemToObject(msgJson, "transaction", transaction);
      cJSON_AddNumberToObject(transaction,"id",	cntTransaction);
      char buf[50];
      sprintf(buf,"%ud", get_timestamp());
      cJSON_AddStringToObject(transaction,"starttime",		buf);
      cJSON_AddNumberToObject(transaction,"waitresponce", 1000	);
      cJSON_AddNumberToObject(transaction,"dataamount", cntMsg	);
  
  
      CJSON_PUBLIC(cJSON *) commands =  cJSON_CreateArray();
  
      can_msg_timestamped msg;
      
      while (cntMsg > 0)
      {
        BaseType_t res =  xQueueReceive(rxCanQueue, &msg, 0);
  //ESP_LOGI("SM cycle", "reading:%d", res );      
        if (res == pdTRUE)
        {
          CJSON_PUBLIC(cJSON *) command =  cJSON_CreateObject();
          cJSON_AddNumberToObject(command, "id",msg.id);
          cJSON_AddStringToObject( command,"type","canpackage");
          sprintf(buf,"%ud", msg.timestamp);
          cJSON_AddStringToObject( command,"timestamp",buf);
   
          cJSON_AddNumberToObject(command, "address",msg.msg.identifier);
          cJSON_AddNumberToObject(command, "datalength",msg.msg.data_length_code);
         
          CJSON_PUBLIC(cJSON *) data =  cJSON_CreateArray();
          
          for(int j=0;j<msg.msg.data_length_code;j++)
          {
            cJSON_AddNumberToObject(data, "data",msg.msg.data[j]);
          }

          cJSON_AddItemToObject(command, "can_data", data);
          
          cJSON_AddBoolToObject(command, "waitforanswer", false);
           cJSON_AddNumberToObject(command, "timeout", 0);
          
          cJSON_AddItemToObject(commands, "command", command);
             
             
          cntMsg = uxQueueMessagesWaiting( rxCanQueue );
     //     ESP_LOGI("SM cycle", "cntMsg =>  %d", cntMsg );                                                                                    
        }
        else
        {
         ESP_LOGI("SM cycle", "wrong read from queue" );     
        //  break;
        }
        
      }
      cJSON_AddItemToObject(msgJson, "commands", commands);
        
    
    }
    char* req =  cJSON_Print(msgJson);
  
 // ESP_LOGI("SMR request", "cJSON =>  %s", req );
 // post ("masterrequest", req );
  
  cJSON_Delete(msgJson);
  return req;
  
}


char* request_server_clientresponse(void)
{
  cJSON* msgJson = cJSON_CreateObject();
  cJSON* transaction = cJSON_CreateObject();
  
   UBaseType_t cntMsg = uxQueueMessagesWaiting( rxCanQueue );
  //  ESP_LOGI("SM request", "cntMsg =>  %d", cntMsg );
    if (cntMsg > 0)
    {
      cJSON_AddItemToObject(msgJson, "transaction", transaction);
      cJSON_AddNumberToObject(transaction,"id",	cntTransaction);
      char buf[50];
      sprintf(buf,"%ud", get_timestamp());
      cJSON_AddStringToObject(transaction,"starttime",		buf);
      cJSON_AddNumberToObject(transaction,"responseId",		4);
      cJSON_AddNumberToObject(transaction,"waitresponce", 1000	);
      cJSON_AddNumberToObject(transaction,"dataamount", cntMsg	);
  
  
      CJSON_PUBLIC(cJSON *) commands =  cJSON_CreateArray();
  
      can_msg_timestamped msg;
      
      while (cntMsg > 0)
      {
        BaseType_t res =  xQueueReceive(rxCanQueue, &msg, 0);
  //ESP_LOGI("SM cycle", "reading:%d", res );      
        if (res == pdTRUE)
        {
          CJSON_PUBLIC(cJSON *) command =  cJSON_CreateObject();
          cJSON_AddNumberToObject(command, "id",msg.id);
          cJSON_AddStringToObject( command,"type","canpackage");
          sprintf(buf,"%ud", msg.timestamp);
          cJSON_AddStringToObject( command,"timestamp",buf);
   
          cJSON_AddNumberToObject(command, "address",msg.msg.identifier);
          cJSON_AddNumberToObject(command, "datalength",msg.msg.data_length_code);
         
          CJSON_PUBLIC(cJSON *) data =  cJSON_CreateArray();
          
          for(int j=0;j<msg.msg.data_length_code;j++)
          {
            cJSON_AddNumberToObject(data, "data",msg.msg.data[j]);
          }

          cJSON_AddItemToObject(command, "can_data", data);
          
          cJSON_AddBoolToObject(command, "waitforanswer", false);
           cJSON_AddNumberToObject(command, "timeout", 0);
          
          cJSON_AddItemToObject(commands, "command", command);
             
             
          cntMsg = uxQueueMessagesWaiting( rxCanQueue );
     //     ESP_LOGI("SM cycle", "cntMsg =>  %d", cntMsg );                                                                                    
        }
        else
        {
         ESP_LOGI("SM cycle", "wrong read from queue" );     
        //  break;
        }
        
      }
      cJSON_AddItemToObject(msgJson, "commands", commands);
        
    
    }
    char* req =  cJSON_Print(msgJson);
  
 // ESP_LOGI("SMR request", "cJSON =>  %s", req );
 // post ("masterrequest", req );
  
  cJSON_Delete(msgJson);
  return req;
  
}

void request_can_send (CJSON_PUBLIC(cJSON *)jdata)
{
  
    cJSON * trans = cJSON_GetObjectItem(jdata,"transaction");
     int readed =  cJSON_GetObjectItem(trans,"readed")->valueint;
     // ESP_LOGI("CAN_SEND", "readed:%d", readed);
     if (readed == 1)
     {
              
    char* json = cJSON_Print(jdata);
  
  ESP_LOGI("CAN_SEND", "========================");
    ESP_LOGI("CAN_SEND", "print:%s", json);
    free (json);                                      
  cJSON *commands = cJSON_GetObjectItem(jdata,"commands");
  // ESP_LOGI("CAN_SEND", "JSON parsed"); 
  // ESP_LOGI("CAN_SEND", "is=%d", cJSON_IsArray(commands));
  if (cJSON_IsArray(commands) == 1)
  {
   // ESP_LOGI("CAN_SEND", "Array"); 
    int len =  cJSON_GetArraySize(commands);
   // ESP_LOGI("CAN_SEND", " Array size %d", len); 
    for (int i=0; i< len;i++)
    {
      cJSON *command = cJSON_GetArrayItem(commands, i);
   //   char* commandType = cJSON_GetObjectItem(command,"type")->valuestring;
   //   ESP_LOGI("CAN_SEND", "command type:%s", commandType); 
   //   if (strcmp(commandType, "canpackage"))
      {
        can_msg_timestamped canM;
        canM.id =  cJSON_GetObjectItem(command,"id")->valueint;
   //      ESP_LOGI("CAN_SEND", "ide:%d", canM.id); 
        //canM.timestamp = //��������������� ������ � �����
        canM.msg.identifier =  cJSON_GetObjectItem(command,"address")->valueint;
        canM.msg.data_length_code = cJSON_GetObjectItem(command,"datalength")->valueint;
          cJSON *data = cJSON_GetObjectItem(command, "can_data");
   //            ESP_LOGI("CAN_SEND", " can_data is array=%d", cJSON_IsArray(data));
          if (cJSON_IsArray(data) == 1)
          {
             int data_len =  cJSON_GetArraySize(data);
   //          ESP_LOGI("CAN_SEND", " Array size %d", data_len);
             for (int j=0;j<data_len; j++)
             {
                 canM.msg.data[j] =  cJSON_GetArrayItem(data, j)->valueint;
             }
          }
           ESP_LOGI("HTTP RX", "=> address: %d, dl: %d, data[0]:%d", canM.msg.identifier, canM.msg.data_length_code, canM.msg.data[0]);
           
           
    /*        canM.msg.identifier =  0x07DF;
           canM.msg.data[0]=01;
           canM.msg.data[1]=00;
           canM.msg.data[2]=00;
           canM.msg.data[3]=00;
           canM.msg.data[4]=00;
           canM.msg.data[5]=00;
           canM.msg.data[6]=00;
           canM.msg.data[7]=01;*/
          xQueueSend(txCanQueue, &canM, portMAX_DELAY);
      }
      
    }
  }
  
    ControlState cs2 = CONTROL_COMMAND;    //proceed command
   xQueueSend(controlQueue, &cs2, portMAX_DELAY);
   }
   else  
   {
    ControlState cs2 = CONTROL_START;    //return to start
   xQueueSend(controlQueue, &cs2, portMAX_DELAY);
   }
}


char* request_heartbeat(void)
{
   uint8_t chipid[6];
   esp_efuse_mac_get_default(chipid);
    char buf[50];
    sprintf(buf,"%02x:%02x:%02x:%02x:%02x:%02x",chipid[0], chipid[1], chipid[2], chipid[3], chipid[4], chipid[5]);
  cJSON *root;
	root=cJSON_CreateObject();	
	//cJSON_AddItemToObject(root, "id",  cJSON_CreateIntArray(chipid, 6));
  cJSON_AddStringToObject(root,"id",		buf);
  cJSON_AddStringToObject(root,"status",		"OK");
  cJSON_AddStringToObject(root,"role",		"client");
  cJSON_AddStringToObject(root,"type","heartbeat");
  cJSON_AddNumberToObject(root,"lasttransaction", 5);
  char* req =  cJSON_Print(root);
  
 // ESP_LOGI("Login request", "cJSON =>  %s", req );
  return req;

}

char* request_response(void)
{
   
  cJSON *root;
	root=cJSON_CreateObject();	
	//cJSON_AddItemToObject(root, "id",  cJSON_CreateIntArray(chipid, 6));
  cJSON_AddStringToObject(root,"message",	"***");
  char* req =  cJSON_Print(root);
  
 // ESP_LOGI("Response request", "cJSON =>  %s", req );
  return req;

}