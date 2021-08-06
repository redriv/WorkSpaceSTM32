#include "sys.h"
#include "led.h"
#include "key.h"
#include "oled_fun.h"
#include "bmp.h"

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

#define QUEUE_LEN 4 /* 队列的长度，最大可包含多少个消息 */
#define QUEUE_SIZE 4 /* 队列中每个消息大小（字节） */


static TaskHandle_t LED_Task_Handle = NULL;/* LED任务句柄 */
static TaskHandle_t KEY_Task_Handle = NULL;/* KEY任务句柄 */
static TaskHandle_t OLED_Task_Handle = NULL;/* OLED任务句柄 */
static TaskHandle_t AppTaskCreate_Handle = NULL;/* 创建任务句柄 */


static void AppTaskCreate(void);/* 用于创建任务 */

static void BSP_Init(void);/* 用于初始化板载相关资源 */

static void LED_Task(void* pvParameters);/* LED_Task任务实现 */

static void KEY_Task(void* pvParameters);/* KEY_Task任务实现 */

static void OLED_Task(void* pvParameters);/* OLED_Task任务实现 */



BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
QueueHandle_t Test_Queue =NULL;/* 定义一个消息队列句柄 */



//跑马灯实验 -库函数版本
//STM32F4工程模板-库函数版本
//淘宝店铺：http://mcudev.taobao.com

int main(void)
{ 

	
	BaseType_t xReturn1 = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */

	/* 开发板硬件初始化 */
	BSP_Init();

	printf("这是一个基于[野火]-STM32全系列开发板-FreeRTOS固件库例程！\n\n");
	

	/* 创建AppTaskCreate任务 */
	xReturn1 = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* 任务入口函数 */
							(const char*    )"AppTaskCreate",/* 任务名字 */
							(uint16_t       )512,  /* 任务栈大小 */
							(void*          )NULL,/* 任务入口函数参数 */
							(UBaseType_t    )1, /* 任务的优先级 */
							(TaskHandle_t*  )&AppTaskCreate_Handle);/* 任务控制块指针 */ 
	/* 启动任务调度 */           
	if(pdPASS == xReturn1)
	vTaskStartScheduler();   /* 启动任务，开启调度 */
	else
	return -1;  

	while(1);   /* 正常不会执行到这里 */    
}



/***********************************************************************
  * @ 函数名  ： BSP_Init
  * @ 功能说明： 板级外设初始化，所有板子上的初始化均可放在这个函数里面
  * @ 参数    ：   
  * @ 返回值  ： 无
  *********************************************************************/
static void BSP_Init(void)
{
	/*
	 * STM32中断优先级分组为4，即4bit都用来表示抢占优先级，范围为：0~15
	 * 优先级分组只需要分组一次即可，以后如果有其他的任务需要用到中断，
	 * 都统一用这个优先级分组，千万不要再分组，切忌。
	 */
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	
	/* LED 初始化 */
	LED_GPIO_Config();

	/* 串口初始化	*/
	Debug_USART_Config();
	
  
	/* 按键初始化	*/	
	KEY_Init();
	
	
	/* 初始化OLED   */
	i2c_init();	 	// 针对当前AT24C02的初始化  GPIO
	SSD1315_init();	// 初始化液晶屏芯片 SSD1315   
	OLED_Clear();	//清除屏幕		
	show_Desktop() ;//显示桌面
	
}




/***********************************************************************
  * @ 函数名  ： AppTaskCreate
  * @ 功能说明： 为了方便管理，所有的任务创建函数都放在这个函数里面
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  **********************************************************************/
static void AppTaskCreate(void)
{ 

  taskENTER_CRITICAL();           //进入临界区
	
  Test_Queue = xQueueCreate((UBaseType_t)QUEUE_LEN,(UBaseType_t)sizeof(msg));
  if(Test_Queue != NULL)
	printf("创建Test_Queue成功!\r\n");
  
  /* 创建LED_Task任务 */
  xReturn = xTaskCreate((TaskFunction_t )LED_Task, /* 任务入口函数 */
                        (const char*    )"LED_Task",/* 任务名字 */
                        (uint16_t       )512,   /* 任务栈大小 */
                        (void*          )NULL,	/* 任务入口函数参数 */
                        (UBaseType_t    )2,	    /* 任务的优先级 */
                        (TaskHandle_t*  )&LED_Task_Handle);/* 任务控制块指针 */
  if(pdPASS == xReturn)
    printf("创建LED_Task任务成功!\r\n");
  
  xReturn = xTaskCreate((TaskFunction_t )KEY_Task, 
                        (const char*    )"KEY_Task",
                        (uint16_t       )512,  
                        (void*          )NULL,
                        (UBaseType_t    )3, 
                        (TaskHandle_t*  )&KEY_Task_Handle);
  if(pdPASS == xReturn)
    printf("创建KEY_Task任务成功!\r\n");
  
  xReturn = xTaskCreate((TaskFunction_t )OLED_Task,  
                        (const char*    )"OLED_Task",
                        (uint16_t       )512,  
                        (void*          )NULL,
                        (UBaseType_t    )4,
                        (TaskHandle_t*  )&OLED_Task_Handle);
  if(pdPASS == xReturn)
    printf("创建OLED_Task任务成功!\r\n");
  
  
  vTaskDelete(AppTaskCreate_Handle); //删除AppTaskCreate任务
  
  taskEXIT_CRITICAL();            //退出临界区
}

/**********************************************************************
  * @ 函数名  ： LED_Task
  * @ 功能说明： Test_Task任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
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

			GPIO_ResetBits(GPIOA,GPIO_Pin_6); //LED0对应引脚GPIOA.6拉低，亮  等同LED0=0;
			GPIO_SetBits(GPIOA,GPIO_Pin_7);   //LED1对应引脚GPIOA.7拉高，灭 等同LED1=1;			
			vTaskDelay(500);   /* 延时500个tick */

			GPIO_SetBits(GPIOA,GPIO_Pin_6);	  //LED0对应引脚GPIOA.6拉高，灭  等同LED0=1;
			GPIO_ResetBits(GPIOA,GPIO_Pin_7); //LED1对应引脚GPIOA.7拉低，亮 等同LED1=0;			
			vTaskDelay(500);   /* 延时500个tick */

			LED_msg.data.intdata =count;

			xReturn = xQueueSend( Test_Queue, /*  消息队列的句柄 */
									&LED_msg, /*  发送的消息内容 */
										 0 ); /*  等待时间 0 */
			if (pdPASS != xReturn)	printf(" 消息 LED_msg  发送失败!\n\n");

		}
}

/**********************************************************************
  * @ 函数名  ： Test_Task
  * @ 功能说明： Test_Task任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void KEY_Task(void* parameter)
{	
	u8 key;           //保存键值
	msg KEY_msg;
	KEY_msg.id = Charmsg;
	while(1)
	{
		key=KEY_Scan(0);		//得到键值
		switch(key)
		{		
			case KEY0_PRES:	//控制LED0翻转
			{					
					vTaskSuspend(LED_Task_Handle);/* 挂起LED任务 */
				
					KEY_msg.data.chrdata ="Task waitting";
					xReturn = xQueueSend( Test_Queue, /*  消息队列的句柄 */
											&KEY_msg, /*  发送的消息内容 */
												 0 ); /*  等待时间 0 */
					if (pdPASS != xReturn)	printf(" 消息 KEY_msg  发送失败!\n\n");
					break;
				
			}
			case KEY1_PRES:	//控制LED1翻转	
			{					
					
					vTaskResume(LED_Task_Handle);/* 恢复LED任务！ */	
				
					KEY_msg.data.chrdata ="Task running!";
					xReturn = xQueueSend( Test_Queue, /*  消息队列的句柄 */
											&KEY_msg, /*  发送的消息内容 */
												 0 ); /*  等待时间 0 */
					if (pdPASS != xReturn)	printf(" 消息 KEY_msg  发送失败!\n\n");
				
				
					break;
			}
			default:
				//printf("ide Test任务！\n");
				vTaskDelay(200);/* 延时20个tick */
				break;
					
		}
		
	}
}

/**********************************************************************
  * @ 函数名  ： OLED_Task
  * @ 功能说明： OLED_Task任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void OLED_Task(void* parameter)
{
	
	msg Tsakmsg;	
 
	while(1)
	{
		
		xReturn = xQueueReceive( Test_Queue, /*  消息队列的句柄 */
								   &Tsakmsg, /*  发送的消息内容 */
						     portMAX_DELAY); /*  等待时间  一直等 */
		if(pdTRUE!= xReturn) printf(" 数据接收出错, 错误代码: 0x%lx\n",xReturn);
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
				
				default: break;
				
			
			}				
		}
	}

}


