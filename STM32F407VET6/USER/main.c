#include "sys.h"
#include "delay.h"
#include "led.h"
#include "key.h"

#include "FreeRTOS.h"
#include "task.h"


static TaskHandle_t AppTaskCreate_Handle = NULL;/* 创建任务句柄 */
static TaskHandle_t Test_Task_Handle = NULL;/* LED任务句柄 */
static TaskHandle_t KEY_Task_Handle = NULL;/* KEY任务句柄 */


static void AppTaskCreate(void);/* 用于创建任务 */
static void Test_Task(void* pvParameters);/* Test_Task任务实现 */
static void KEY_Task(void* pvParameters);/* KEY_Task任务实现 */
static void BSP_Init(void);/* 用于初始化板载相关资源 */


//跑马灯实验 -库函数版本
//STM32F4工程模板-库函数版本
//淘宝店铺：http://mcudev.taobao.com

int main(void)
{ 

	
	BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */

	/* 开发板硬件初始化 */
	BSP_Init();

	printf("这是一个基于[野火]-STM32全系列开发板-FreeRTOS固件库例程！\n\n");
	

	/* 创建AppTaskCreate任务 */
	xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* 任务入口函数 */
							(const char*    )"AppTaskCreate",/* 任务名字 */
							(uint16_t       )512,  /* 任务栈大小 */
							(void*          )NULL,/* 任务入口函数参数 */
							(UBaseType_t    )1, /* 任务的优先级 */
							(TaskHandle_t*  )&AppTaskCreate_Handle);/* 任务控制块指针 */ 
	/* 启动任务调度 */           
	if(pdPASS == xReturn)
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
	//Key_GPIO_Config();
	KEY_Init();
	
	delay_init(168);		  //初始化延时函数
	
	//OLED_Init();				//初始化OLED

}
/***********************************************************************
  * @ 函数名  ： AppTaskCreate
  * @ 功能说明： 为了方便管理，所有的任务创建函数都放在这个函数里面
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  **********************************************************************/
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
  
  taskENTER_CRITICAL();           //进入临界区
  
  /* 创建Test_Task任务 */
  xReturn = xTaskCreate((TaskFunction_t )Test_Task, /* 任务入口函数 */
                        (const char*    )"Test_Task",/* 任务名字 */
                        (uint16_t       )512,   /* 任务栈大小 */
                        (void*          )NULL,	/* 任务入口函数参数 */
                        (UBaseType_t    )2,	    /* 任务的优先级 */
                        (TaskHandle_t*  )&Test_Task_Handle);/* 任务控制块指针 */
  if(pdPASS == xReturn)
    printf("创建Test_Task任务成功!\r\n");
  /* 创建KEY_Task任务 */
  xReturn = xTaskCreate((TaskFunction_t )KEY_Task,  /* 任务入口函数 */
                        (const char*    )"KEY_Task",/* 任务名字 */
                        (uint16_t       )512,  /* 任务栈大小 */
                        (void*          )NULL,/* 任务入口函数参数 */
                        (UBaseType_t    )3, /* 任务的优先级 */
                        (TaskHandle_t*  )&KEY_Task_Handle);/* 任务控制块指针 */ 
  if(pdPASS == xReturn)
    printf("创建KEY_Task任务成功!\r\n");
  
  vTaskDelete(AppTaskCreate_Handle); //删除AppTaskCreate任务
  
  taskEXIT_CRITICAL();            //退出临界区
}

/**********************************************************************
  * @ 函数名  ： Test_Task
  * @ 功能说明： Test_Task任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void Test_Task(void* parameter)
{	
	  while (1)
	  {
		GPIO_ResetBits(GPIOA,GPIO_Pin_6); //LED0对应引脚GPIOA.6拉低，亮  等同LED0=0;
		GPIO_SetBits(GPIOA,GPIO_Pin_7);   //LED1对应引脚GPIOA.7拉高，灭 等同LED1=1;
		printf("Test_Task Running,LED1_ON\r\n");
		vTaskDelay(500);   /* 延时500个tick */
		
		GPIO_SetBits(GPIOA,GPIO_Pin_6);	  //LED0对应引脚GPIOA.6拉高，灭  等同LED0=1;
		GPIO_ResetBits(GPIOA,GPIO_Pin_7); //LED1对应引脚GPIOA.7拉低，亮 等同LED1=0; 
		printf("Test_Task Running,LED1_OFF\r\n");
		vTaskDelay(500);   /* 延时500个tick */
	  }
}

/**********************************************************************
  * @ 函数名  ： Test_Task
  * @ 功能说明： Test_Task任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void KEY_Task(void* parameter)
{	u8 key;           //保存键值
	while(1)
	{
		key=KEY_Scan(0);		//得到键值
		switch(key)
					{				 
			//case WKUP_PRES:	//控制蜂鸣器
			//{
			//	LED0=!LED0;
			//	LED1=!LED1;
			//	printf("ide Test任务！\n");
			//	break;
			//}
			case KEY0_PRES:	//控制LED0翻转
			{
					printf("挂起Test任务！\n");
					vTaskSuspend(Test_Task_Handle);/* 挂起LED任务 */
					printf("挂起Test任务成功！\n");
					break;
			}
			case KEY1_PRES:	//控制LED1翻转	
			{					
					printf("恢复Test任务！\n");
					vTaskResume(Test_Task_Handle);/* 恢复LED任务！ */
					printf("恢复Test任务成功！\n");
					break;
			}
			default:
				//printf("ide Test任务！\n");
				vTaskDelay(200);/* 延时20个tick */
				break;
					
		}
	}
}


