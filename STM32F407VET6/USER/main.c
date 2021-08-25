#include "sys.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "oled_fun.h"
#include "bmp.h"
#include "malloc.h"
#include "sdio_sdcard.h"    

#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOSConfig.h"
#include "queue.h"


typedef union 
	{
		uint32_t intdata;
		uint8_t* chrdata;
	}Data;

typedef struct{
	
	uint8_t  id;	
	Data   data;
} msg;


#define Intmsg  0
#define Charmsg 1
#define QUEUE_LEN 4 
#define QUEUE_SIZE 4


static TaskHandle_t LED_Task_Handle = NULL;/* LED task */
static TaskHandle_t KEY_Task_Handle = NULL;/* KEY task */
static TaskHandle_t OLED_Task_Handle = NULL;/* OLED task */
static TaskHandle_t AppTaskCreate_Handle = NULL;/*Create task */


static void BSP_Init(void);/* BSP_Init */

static void AppTaskCreate(void);/* AppTaskCreate_Task */

static void LED_Task(void* pvParameters);/* LED_Task */

static void KEY_Task(void* pvParameters);/* KEY_Task */

static void OLED_Task(void* pvParameters);/* OLED_Task*/



BaseType_t xReturn = pdPASS;/*pdPASS */
QueueHandle_t Test_Queue =NULL;






int main(void)
{ 

	
	BaseType_t xReturn1 = pdPASS;/* pdPASS */

	/*  */
	BSP_Init();

	printf("River_TASK:<V_1>\n\n");
	

	/* AppTaskCreate */
	xReturn1 = xTaskCreate((TaskFunction_t )AppTaskCreate,  
							(const char*    )"AppTaskCreate",
							(uint16_t       )512,  
							(void*          )NULL,
							(UBaseType_t    )1, 
							(TaskHandle_t*  )&AppTaskCreate_Handle);
	         
	if(pdPASS == xReturn1)
	vTaskStartScheduler();   //调度开始
	else
	return -1;  

	while(1);   
}

void show_sdcard_info(void)
{
	switch(SDCardInfo.CardType)
	{
		case SDIO_STD_CAPACITY_SD_CARD_V1_1:printf("Card Type:SDSC V1.1\r\n");break;
		case SDIO_STD_CAPACITY_SD_CARD_V2_0:printf("Card Type:SDSC V2.0\r\n");break;
		case SDIO_HIGH_CAPACITY_SD_CARD:printf("Card Type:SDHC V2.0\r\n");break;
		case SDIO_MULTIMEDIA_CARD:printf("Card Type:MMC Card\r\n");break;
	}	
  	printf("Card ManufacturerID:%d\r\n",SDCardInfo.SD_cid.ManufacturerID);	//制造商ID
 	printf("Card RCA:%d\r\n",SDCardInfo.RCA);								//卡相对地址
	printf("Card Capacity:%d MB\r\n",(u32)(SDCardInfo.CardCapacity>>20));	//显示容量
 	printf("Card BlockSize:%d\r\n\r\n",SDCardInfo.CardBlockSize);			//显示块大小
}


/***********************************************************************
  * @ 
  *********************************************************************/
static void BSP_Init(void)
{
	/*
	 * 
	 */
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	
	/* LED */
	LED_GPIO_Config();

	/* UART	*/
	Debug_USART_Config();
	
	delay_init(168);  //初始化延时函数
	
	/* KEY*/	
	KEY_Init();
	
	
	/* OLED   */
	i2c_init();	 	//  GPIO
	SSD1315_init();	//  SSD1315   
	OLED_Clear();	//	
	show_Desktop() ;//

	//my_mem_init(SRAMIN);
	//my_mem_init(SRAMCCM);
	
	while(SD_Init())//
	{
		printf("SD Card Error!");
		delay_ms(500);					
		printf("Please Check! ");
		delay_ms(500);
		LED0=!LED0;//DS0
		LED1=!LED1;//DS1
	}
	show_sdcard_info();	//
	
}




/***********************************************************************
 
  **********************************************************************/
static void AppTaskCreate(void)
{ 

  taskENTER_CRITICAL();           //
	
  Test_Queue = xQueueCreate((UBaseType_t)QUEUE_LEN,(UBaseType_t)sizeof(msg));
  if(Test_Queue != NULL)
	printf("xQueueCreate OK!\r\n");
  
  /* LED_Task */
  xReturn = xTaskCreate((TaskFunction_t )LED_Task, 
                        (const char*    )"LED_Task",
                        (uint16_t       )512,   
                        (void*          )NULL,	
                        (UBaseType_t    )2,	    
                        (TaskHandle_t*  )&LED_Task_Handle);
  if(pdPASS == xReturn)
    printf("LED_Task OK!\r\n");
  
  xReturn = xTaskCreate((TaskFunction_t )KEY_Task, 
                        (const char*    )"KEY_Task",
                        (uint16_t       )512,  
                        (void*          )NULL,
                        (UBaseType_t    )3, 
                        (TaskHandle_t*  )&KEY_Task_Handle);
  if(pdPASS == xReturn)
    printf("KEY_Task OK!\r\n");
  
  xReturn = xTaskCreate((TaskFunction_t )OLED_Task,  
                        (const char*    )"OLED_Task",
                        (uint16_t       )512,  
                        (void*          )NULL,
                        (UBaseType_t    )4,
                        (TaskHandle_t*  )&OLED_Task_Handle);
  if(pdPASS == xReturn)
    printf("OLED_Task OK!\r\n");
  
  
  vTaskDelete(AppTaskCreate_Handle); 
  
  taskEXIT_CRITICAL();            //out
}

/**********************************************************************

 ********************************************************************/
static void LED_Task(void* parameter)
{	  
		int count = 0;
		msg LED_msg;
		LED_msg.id = Intmsg;

		while (1)
		{
			if(count > 99)count = 0;
			else count++;

//			GPIO_ResetBits(GPIOA,GPIO_Pin_6); //LED0
//			GPIO_SetBits(GPIOA,GPIO_Pin_7);   //LED1=1;			
//			vTaskDelay(500);   /*tick */

//			GPIO_SetBits(GPIOA,GPIO_Pin_6);	  //LED0
//			GPIO_ResetBits(GPIOA,GPIO_Pin_7); //LED1;			
			vTaskDelay(500);   /*tick */
			
			LED0=!LED0;//DS0
			LED1=!LED1;//DS1

			LED_msg.data.intdata =count;

			xReturn = xQueueSend( Test_Queue, 
									&LED_msg, 
										 0 ); 
			if (pdPASS != xReturn)	printf("xQueueSend OK!\n\n");

		}
}

/**********************************************************************

  ********************************************************************/
static void KEY_Task(void* parameter)
{	
	u8 key;           //
	msg KEY_msg;
	KEY_msg.id = Charmsg;
	while(1)
	{
		key=KEY_Scan(0);		
		switch(key)
		{		
			case KEY0_PRES:	
			{					
					vTaskSuspend(LED_Task_Handle);
					OLED_ShowNum(90,4,0,2,16);
					KEY_msg.data.chrdata ="Task waitting";
					xReturn = xQueueSend( Test_Queue, 
											&KEY_msg,
												 0 );
					if (pdPASS != xReturn)	printf("xQueueSend OK!\n\n");
					break;
				
			}
			case KEY1_PRES:	
			{					
					
					vTaskResume(LED_Task_Handle);
					OLED_ShowNum(90,4,1,2,16);
					KEY_msg.data.chrdata ="Task running!";
					xReturn = xQueueSend( Test_Queue, 
											&KEY_msg, 
												 0 );
					if (pdPASS != xReturn)	printf(" xQueueSend OK!!\n\n");
				
				
					break;
			}
			default:					
					vTaskDelay(200);
					break;
					
		}
		
	}
}

/**********************************************************************
 
  ********************************************************************/
static void OLED_Task(void* parameter)
{
	
	msg Tsakmsg;	
 
	while(1)
	{
		
		xReturn = xQueueReceive( Test_Queue, 
								   &Tsakmsg, 
						     portMAX_DELAY);
		if(pdTRUE!= xReturn) printf(" xQueueReceive error 0x%lx\n",xReturn);
		else
		{
			switch (Tsakmsg.id)
			{
				
				case Intmsg :
				{
					//printf("[LED_TASK]:%d",Tsakmsg.data.intdata);
					OLED_ShowNum(90,2,Tsakmsg.data.intdata,2,16);
					break;				
				}
				case Charmsg :
				{
					//printf("[KEY_TASK]:%s",Tsakmsg.data.chrdata);
					OLED_ShowString(0,6,(u8 *)Tsakmsg.data.chrdata,16);
					break;				
				}
				
				default: 
					break;
			}				
		}
	}

}


