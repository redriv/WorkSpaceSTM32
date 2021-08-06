#include "i2c.h"


/*
*********************************************************************************************************
*	�� �� ��: i2c_Delay
*	����˵��: I2C����λ�ӳ٣����400KHz
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void i2c_Delay(void)
{
	uint8_t i;

	/*��
		�����߼������ǲ���I2CͨѶʱ��Ƶ��
    ����������CPU��Ƶ168MHz ��MDK���뻷����1���Ż�
  
		�����ԣ�ѭ������Ϊ20~250ʱ����ͨѶ����

	*/
	for (i = 0; i < 40; i++);
}


void i2c_init(void)
{
	GPIO_InitTypeDef  	GPIO_InitStructure;
	
	//PB8 PB9��ʼ������ ʹ��GPIOBʱ��  !!!!
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);			
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_9;		//8�ź�9������
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;			    	//��ͨ���ģʽ��
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;					//�������������LED��Ҫ��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		    	//100MHz
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;				    //����
	GPIO_Init(GPIOB, &GPIO_InitStructure);							//��ʼ��GPIOB�������õ�����д��Ĵ���						

	//i2c���ų�ʼ��״̬��Ĭ��Ϊ�ߵ�ƽ
	SCL =1;
	SDA_W=1;


}

void i2c_sda_mode(GPIOMode_TypeDef iomode)
{
	GPIO_InitTypeDef  	GPIO_InitStructure;
	
	//PB9��ʼ������ 
	GPIO_InitStructure.GPIO_Pin   =  GPIO_Pin_9;					//9������
	GPIO_InitStructure.GPIO_Mode  = iomode;			    			//���ģʽ/����ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;					//�������������LED��Ҫ��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		    	//100MHz
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;				    //����
	GPIO_Init(GPIOB, &GPIO_InitStructure);							//��ʼ��GPIOB�������õ�����д��Ĵ���		


}


void i2c_start(void)
{
	//��֤SDA����Ϊ���ģʽ
	i2c_sda_mode(GPIO_Mode_OUT);
	
	
	SCL =1;
	SDA_W=1;	
	
	i2c_Delay();		//100KHzͨ�����ʣ����ǲ��ܳ���400KHz
	
	SDA_W=0;
	
	i2c_Delay();
	
	SCL =0;				//����ռ��I2C���ߣ��������ݸı�
}


void i2c_stop(void)
{
	//��֤SDA����Ϊ���ģʽ
	i2c_sda_mode(GPIO_Mode_OUT);


	SCL =0;
	SDA_W=0;
	
	i2c_Delay();
	
	SCL =1;
	
	i2c_Delay();
	
	SDA_W=1;
	
	i2c_Delay();
}


void i2c_send_byte(uint8_t txd) // 0x1 ---- b0000 0001
{
	uint32_t i=0;
	//��֤SDA����Ϊ���ģʽ
	i2c_sda_mode(GPIO_Mode_OUT);

	//��֤SCL���ſ�ʼ��ʱ��Ϊ�͵�ƽ���������ݵĸı�
	SCL =0;
	i2c_Delay();
	
	//��������8��bit�����������Чλ���Ƚ��з���
	for(i=0; i<8; i++)  // ��ַ��B1010 + a2 + a0 +  W   ������ 8λ
	{
		if(txd & (1<<(7-i)))
			SDA_W=1;
		else
			SDA_W=0;
		
		i2c_Delay();
		
		//�������ݣ��ôӻ�����ʶ��
		SCL=1;
		i2c_Delay();
		
		//����ı����ݣ��ӻ����Ӹ�����
		SCL=0;
		i2c_Delay();	
	
	}
}

uint8_t i2c_recv_byte(void)
{
	uint32_t i=0;
	uint8_t  rxd=0;
	
	//��֤SDA����Ϊ���ģʽ
	i2c_sda_mode(GPIO_Mode_IN);

	//��֤SCL���ſ�ʼ��ʱ��Ϊ�͵�ƽ���������ݵĸı�
	SCL =0;
	i2c_Delay();
	
	//��������8��bit�����������Чλ���Ƚ��н���
	for(i=0; i<8; i++)
	{

		
		//delay_us(5);
		
		//��������
		SCL=1;
		i2c_Delay();
		
		if(SDA_R)
			rxd|=1<<(7-i);		
		
		//����ı�����
		SCL=0;
		i2c_Delay();	
	
	}
	
	return rxd;

}

void i2c_ack(uint8_t ack) // �������ͳ�ȥ��ACK
{

	//��֤SDA����Ϊ���ģʽ
	i2c_sda_mode(GPIO_Mode_OUT);

	//��֤SCL���ſ�ʼ��ʱ��Ϊ�͵�ƽ���������ݵĸı�
	SCL =0;
	i2c_Delay();

	if(ack) SDA_W=1;
	else    SDA_W=0;
	
	i2c_Delay();
	
	//�������ݣ��ôӻ�����ʶ��
	SCL=1;
	i2c_Delay();
	
	//����ı����ݣ��ӻ����Ӹ�����
	SCL=0;
	i2c_Delay();	

}

uint8_t i2c_wait_ack(void) // �����ȴӻ�������ACK
{
	uint8_t ack=0;
	
	//��֤SDA����Ϊ����ģʽ
	i2c_sda_mode(GPIO_Mode_IN);
	
	SCL=1;
	i2c_Delay();
	
	//��Ӧ��Ϊ�͵�ƽ����Ӧ��Ϊ�ߵ�ƽ
	if(SDA_R)		//��Ӧ��
	{
		ack=1;
		i2c_stop();
	}
	else			//��Ӧ��
		ack=0;

	
	
	SCL =0;			//����ռ��I2C���ߣ��������ݸı�
	i2c_Delay();
	
	return ack;
}

void at24c02_write(uint8_t addr,uint8_t *pbuf,uint8_t len)  // ҳ�� 0     pbuf = {1.....1}       д��8�� 
{
	uint8_t ack=0;
	
	//���������ź�
	i2c_start();
	
	//����Ѱַ��ַΪ0xA0��д���ʲ���
	i2c_send_byte(0xA0);
	
	//�ȴ�Ӧ��
	ack = i2c_wait_ack();
	
	if(ack)
	{
		printf("24c02 ack device address fail\r\n");
		return;
	
	}

	printf("24c02 is online\r\n");

	//�������ݴ洢��ַ
	i2c_send_byte(addr*8);
	
	//�ȴ�Ӧ��
	ack = i2c_wait_ack();
	
	if(ack)
	{
		printf("24c02 ack word address fail\r\n");
		return;
	
	}	
	
	printf("24c02 word address ok\r\n");	
	
	while(len--) //����Լ�д�룬 д8���ֽ�
	{
		//��������
	//	printf("%d \r\n" ,*pbuf);
		i2c_send_byte(*pbuf++);
		
		//�ȴ�Ӧ��
		ack = i2c_wait_ack();
		
		if(ack)
		{
			printf("24c02 ack send data fail\r\n");
			//i2c_stop();
			return;
		}		
	
	}
	
	//����ֹͣ�źţ�����ͨ�Ź��̽���
	i2c_stop();
	
	printf("24c02 write ok\r\n");

}

void at24c02_read(uint8_t addr,uint8_t *pbuf,uint8_t len)
{
	uint8_t ack=0;
	
	//���������ź�
	i2c_start();
	
	//����Ѱַ��ַΪ0xA0��д���ʲ���
	i2c_send_byte(0xA0);
	
	//�ȴ�Ӧ��
	ack = i2c_wait_ack();
	
	if(ack)
	{
		printf("24c02 ack device address fail\r\n");
		
		return;
	
	}

	printf("24c02 is online\r\n");

	//�������ݴ洢��ַ
	i2c_send_byte(addr*8);
	
	//�ȴ�Ӧ��
	ack = i2c_wait_ack();
	
	if(ack)
	{
		printf("24c02 ack word address 1 fail\r\n");
		
		return;
	
	}	
	
	printf("24c02 word address ok\r\n");	
	
	//���·��������ź�
	i2c_start();
	
	//����Ѱַ��ַΪ0xA1�������ʲ���
	i2c_send_byte(0xA1);
	
	//�ȴ�Ӧ��
	ack = i2c_wait_ack();
	
	if(ack)
	{
		printf("24c02 ack device address 2 fail\r\n");
		
		return;
	
	}
	
	len=len-1;
	
	while(len--)
	{
		//��������
		*pbuf=i2c_recv_byte();
		
		//printf("[%d]%d \r\n" ,len,*pbuf);
		pbuf++;
		
		//��������Ӧ����ӻ�
		i2c_ack(0);
	}
	
	//��������
	*pbuf=i2c_recv_byte();
	printf("[%d]%d \r\n" ,len,*pbuf);
	
	//����������Ӧ����ӻ�
	i2c_ack(1);	
	
	//����ֹͣ�źţ�����ͨ�Ź��̽���
	i2c_stop();
	
	printf("24c02 read ok\r\n");

}
