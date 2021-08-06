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

#define QUEUE_LEN 4 /* ���еĳ��ȣ����ɰ������ٸ���Ϣ */
#define QUEUE_SIZE 4 /* ������ÿ����Ϣ��С���ֽڣ� */


static TaskHandle_t LED_Task_Handle = NULL;/* LED������ */
static TaskHandle_t KEY_Task_Handle = NULL;/* KEY������ */
static TaskHandle_t OLED_Task_Handle = NULL;/* OLED������ */
static TaskHandle_t AppTaskCreate_Handle = NULL;/* ���������� */


static void AppTaskCreate(void);/* ���ڴ������� */

static void BSP_Init(void);/* ���ڳ�ʼ�����������Դ */

static void LED_Task(void* pvParameters);/* LED_Task����ʵ�� */

static void KEY_Task(void* pvParameters);/* KEY_Task����ʵ�� */

static void OLED_Task(void* pvParameters);/* OLED_Task����ʵ�� */



BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
QueueHandle_t Test_Queue =NULL;/* ����һ����Ϣ���о�� */



//�����ʵ�� -�⺯���汾
//STM32F4����ģ��-�⺯���汾
//�Ա����̣�http://mcudev.taobao.com

int main(void)
{ 

	
	BaseType_t xReturn1 = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */

	/* ������Ӳ����ʼ�� */
	BSP_Init();

	printf("����һ������[Ұ��]-STM32ȫϵ�п�����-FreeRTOS�̼������̣�\n\n");
	

	/* ����AppTaskCreate���� */
	xReturn1 = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* ������ں��� */
							(const char*    )"AppTaskCreate",/* �������� */
							(uint16_t       )512,  /* ����ջ��С */
							(void*          )NULL,/* ������ں������� */
							(UBaseType_t    )1, /* ��������ȼ� */
							(TaskHandle_t*  )&AppTaskCreate_Handle);/* ������ƿ�ָ�� */ 
	/* ����������� */           
	if(pdPASS == xReturn1)
	vTaskStartScheduler();   /* �������񣬿������� */
	else
	return -1;  

	while(1);   /* ��������ִ�е����� */    
}



/***********************************************************************
  * @ ������  �� BSP_Init
  * @ ����˵���� �弶�����ʼ�������а����ϵĳ�ʼ�����ɷ��������������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  *********************************************************************/
static void BSP_Init(void)
{
	/*
	 * STM32�ж����ȼ�����Ϊ4����4bit��������ʾ��ռ���ȼ�����ΧΪ��0~15
	 * ���ȼ�����ֻ��Ҫ����һ�μ��ɣ��Ժ������������������Ҫ�õ��жϣ�
	 * ��ͳһ��������ȼ����飬ǧ��Ҫ�ٷ��飬�мɡ�
	 */
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	
	/* LED ��ʼ�� */
	LED_GPIO_Config();

	/* ���ڳ�ʼ��	*/
	Debug_USART_Config();
	
  
	/* ������ʼ��	*/	
	KEY_Init();
	
	
	/* ��ʼ��OLED   */
	i2c_init();	 	// ��Ե�ǰAT24C02�ĳ�ʼ��  GPIO
	SSD1315_init();	// ��ʼ��Һ����оƬ SSD1315   
	OLED_Clear();	//�����Ļ		
	show_Desktop() ;//��ʾ����
	
}




/***********************************************************************
  * @ ������  �� AppTaskCreate
  * @ ����˵���� Ϊ�˷���������е����񴴽����������������������
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  **********************************************************************/
static void AppTaskCreate(void)
{ 

  taskENTER_CRITICAL();           //�����ٽ���
	
  Test_Queue = xQueueCreate((UBaseType_t)QUEUE_LEN,(UBaseType_t)sizeof(msg));
  if(Test_Queue != NULL)
	printf("����Test_Queue�ɹ�!\r\n");
  
  /* ����LED_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )LED_Task, /* ������ں��� */
                        (const char*    )"LED_Task",/* �������� */
                        (uint16_t       )512,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )2,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&LED_Task_Handle);/* ������ƿ�ָ�� */
  if(pdPASS == xReturn)
    printf("����LED_Task����ɹ�!\r\n");
  
  xReturn = xTaskCreate((TaskFunction_t )KEY_Task, 
                        (const char*    )"KEY_Task",
                        (uint16_t       )512,  
                        (void*          )NULL,
                        (UBaseType_t    )3, 
                        (TaskHandle_t*  )&KEY_Task_Handle);
  if(pdPASS == xReturn)
    printf("����KEY_Task����ɹ�!\r\n");
  
  xReturn = xTaskCreate((TaskFunction_t )OLED_Task,  
                        (const char*    )"OLED_Task",
                        (uint16_t       )512,  
                        (void*          )NULL,
                        (UBaseType_t    )4,
                        (TaskHandle_t*  )&OLED_Task_Handle);
  if(pdPASS == xReturn)
    printf("����OLED_Task����ɹ�!\r\n");
  
  
  vTaskDelete(AppTaskCreate_Handle); //ɾ��AppTaskCreate����
  
  taskEXIT_CRITICAL();            //�˳��ٽ���
}

/**********************************************************************
  * @ ������  �� LED_Task
  * @ ����˵���� Test_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
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

			GPIO_ResetBits(GPIOA,GPIO_Pin_6); //LED0��Ӧ����GPIOA.6���ͣ���  ��ͬLED0=0;
			GPIO_SetBits(GPIOA,GPIO_Pin_7);   //LED1��Ӧ����GPIOA.7���ߣ��� ��ͬLED1=1;			
			vTaskDelay(500);   /* ��ʱ500��tick */

			GPIO_SetBits(GPIOA,GPIO_Pin_6);	  //LED0��Ӧ����GPIOA.6���ߣ���  ��ͬLED0=1;
			GPIO_ResetBits(GPIOA,GPIO_Pin_7); //LED1��Ӧ����GPIOA.7���ͣ��� ��ͬLED1=0;			
			vTaskDelay(500);   /* ��ʱ500��tick */

			LED_msg.data.intdata =count;

			xReturn = xQueueSend( Test_Queue, /*  ��Ϣ���еľ�� */
									&LED_msg, /*  ���͵���Ϣ���� */
										 0 ); /*  �ȴ�ʱ�� 0 */
			if (pdPASS != xReturn)	printf(" ��Ϣ LED_msg  ����ʧ��!\n\n");

		}
}

/**********************************************************************
  * @ ������  �� Test_Task
  * @ ����˵���� Test_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void KEY_Task(void* parameter)
{	
	u8 key;           //�����ֵ
	msg KEY_msg;
	KEY_msg.id = Charmsg;
	while(1)
	{
		key=KEY_Scan(0);		//�õ���ֵ
		switch(key)
		{		
			case KEY0_PRES:	//����LED0��ת
			{					
					vTaskSuspend(LED_Task_Handle);/* ����LED���� */
				
					KEY_msg.data.chrdata ="Task waitting";
					xReturn = xQueueSend( Test_Queue, /*  ��Ϣ���еľ�� */
											&KEY_msg, /*  ���͵���Ϣ���� */
												 0 ); /*  �ȴ�ʱ�� 0 */
					if (pdPASS != xReturn)	printf(" ��Ϣ KEY_msg  ����ʧ��!\n\n");
					break;
				
			}
			case KEY1_PRES:	//����LED1��ת	
			{					
					
					vTaskResume(LED_Task_Handle);/* �ָ�LED���� */	
				
					KEY_msg.data.chrdata ="Task running!";
					xReturn = xQueueSend( Test_Queue, /*  ��Ϣ���еľ�� */
											&KEY_msg, /*  ���͵���Ϣ���� */
												 0 ); /*  �ȴ�ʱ�� 0 */
					if (pdPASS != xReturn)	printf(" ��Ϣ KEY_msg  ����ʧ��!\n\n");
				
				
					break;
			}
			default:
				//printf("ide Test����\n");
				vTaskDelay(200);/* ��ʱ20��tick */
				break;
					
		}
		
	}
}

/**********************************************************************
  * @ ������  �� OLED_Task
  * @ ����˵���� OLED_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void OLED_Task(void* parameter)
{
	
	msg Tsakmsg;	
 
	while(1)
	{
		
		xReturn = xQueueReceive( Test_Queue, /*  ��Ϣ���еľ�� */
								   &Tsakmsg, /*  ���͵���Ϣ���� */
						     portMAX_DELAY); /*  �ȴ�ʱ��  һֱ�� */
		if(pdTRUE!= xReturn) printf(" ���ݽ��ճ���, �������: 0x%lx\n",xReturn);
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


