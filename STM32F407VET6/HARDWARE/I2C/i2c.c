#include "i2c.h"


/*
*********************************************************************************************************
*	函 数 名: i2c_Delay
*	功能说明: I2C总线位延迟，最快400KHz
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void i2c_Delay(void)
{
	uint8_t i;

	/*　
		可用逻辑分析仪测量I2C通讯时的频率
    工作条件：CPU主频168MHz ，MDK编译环境，1级优化
  
		经测试，循环次数为20~250时都能通讯正常

	*/
	for (i = 0; i < 40; i++);
}


void i2c_init(void)
{
	GPIO_InitTypeDef  	GPIO_InitStructure;
	
	//PB8 PB9初始化设置 使能GPIOB时钟  !!!!
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);			
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_9;		//8号和9号引脚
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;			    	//普通输出模式，
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;					//推挽输出，驱动LED需要电流驱动
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		    	//100MHz
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;				    //上拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);							//初始化GPIOB，把配置的数据写入寄存器						

	//i2c引脚初始化状态，默认为高电平
	SCL =1;
	SDA_W=1;


}

void i2c_sda_mode(GPIOMode_TypeDef iomode)
{
	GPIO_InitTypeDef  	GPIO_InitStructure;
	
	//PB9初始化设置 
	GPIO_InitStructure.GPIO_Pin   =  GPIO_Pin_9;					//9号引脚
	GPIO_InitStructure.GPIO_Mode  = iomode;			    			//输出模式/输入模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;					//推挽输出，驱动LED需要电流驱动
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		    	//100MHz
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;				    //上拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);							//初始化GPIOB，把配置的数据写入寄存器		


}


void i2c_start(void)
{
	//保证SDA引脚为输出模式
	i2c_sda_mode(GPIO_Mode_OUT);
	
	
	SCL =1;
	SDA_W=1;	
	
	i2c_Delay();		//100KHz通信速率，但是不能超过400KHz
	
	SDA_W=0;
	
	i2c_Delay();
	
	SCL =0;				//保持占用I2C总线，允许数据改变
}


void i2c_stop(void)
{
	//保证SDA引脚为输出模式
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
	//保证SDA引脚为输出模式
	i2c_sda_mode(GPIO_Mode_OUT);

	//保证SCL引脚开始的时候为低电平，允许数据的改变
	SCL =0;
	i2c_Delay();
	
	//连续发送8个bit，采用最高有效位优先进行发送
	for(i=0; i<8; i++)  // 地址：B1010 + a2 + a0 +  W   ；数据 8位
	{
		if(txd & (1<<(7-i)))
			SDA_W=1;
		else
			SDA_W=0;
		
		i2c_Delay();
		
		//锁存数据，让从机进行识别
		SCL=1;
		i2c_Delay();
		
		//允许改变数据，从机无视该数据
		SCL=0;
		i2c_Delay();	
	
	}
}

uint8_t i2c_recv_byte(void)
{
	uint32_t i=0;
	uint8_t  rxd=0;
	
	//保证SDA引脚为输出模式
	i2c_sda_mode(GPIO_Mode_IN);

	//保证SCL引脚开始的时候为低电平，允许数据的改变
	SCL =0;
	i2c_Delay();
	
	//连续接收8个bit，采用最高有效位优先进行接收
	for(i=0; i<8; i++)
	{

		
		//delay_us(5);
		
		//锁存数据
		SCL=1;
		i2c_Delay();
		
		if(SDA_R)
			rxd|=1<<(7-i);		
		
		//允许改变数据
		SCL=0;
		i2c_Delay();	
	
	}
	
	return rxd;

}

void i2c_ack(uint8_t ack) // 主机发送出去的ACK
{

	//保证SDA引脚为输出模式
	i2c_sda_mode(GPIO_Mode_OUT);

	//保证SCL引脚开始的时候为低电平，允许数据的改变
	SCL =0;
	i2c_Delay();

	if(ack) SDA_W=1;
	else    SDA_W=0;
	
	i2c_Delay();
	
	//锁存数据，让从机进行识别
	SCL=1;
	i2c_Delay();
	
	//允许改变数据，从机无视该数据
	SCL=0;
	i2c_Delay();	

}

uint8_t i2c_wait_ack(void) // 主机等从机发送来ACK
{
	uint8_t ack=0;
	
	//保证SDA引脚为输入模式
	i2c_sda_mode(GPIO_Mode_IN);
	
	SCL=1;
	i2c_Delay();
	
	//有应答为低电平，无应答为高电平
	if(SDA_R)		//无应答
	{
		ack=1;
		i2c_stop();
	}
	else			//有应答
		ack=0;

	
	
	SCL =0;			//保持占用I2C总线，允许数据改变
	i2c_Delay();
	
	return ack;
}

void at24c02_write(uint8_t addr,uint8_t *pbuf,uint8_t len)  // 页码 0     pbuf = {1.....1}       写入8个 
{
	uint8_t ack=0;
	
	//发送启动信号
	i2c_start();
	
	//发送寻址地址为0xA0，写访问操作
	i2c_send_byte(0xA0);
	
	//等待应答
	ack = i2c_wait_ack();
	
	if(ack)
	{
		printf("24c02 ack device address fail\r\n");
		return;
	
	}

	printf("24c02 is online\r\n");

	//发送数据存储地址
	i2c_send_byte(addr*8);
	
	//等待应答
	ack = i2c_wait_ack();
	
	if(ack)
	{
		printf("24c02 ack word address fail\r\n");
		return;
	
	}	
	
	printf("24c02 word address ok\r\n");	
	
	while(len--) //逐个自己写入， 写8个字节
	{
		//发送数据
	//	printf("%d \r\n" ,*pbuf);
		i2c_send_byte(*pbuf++);
		
		//等待应答
		ack = i2c_wait_ack();
		
		if(ack)
		{
			printf("24c02 ack send data fail\r\n");
			//i2c_stop();
			return;
		}		
	
	}
	
	//发送停止信号，整个通信过程结束
	i2c_stop();
	
	printf("24c02 write ok\r\n");

}

void at24c02_read(uint8_t addr,uint8_t *pbuf,uint8_t len)
{
	uint8_t ack=0;
	
	//发送启动信号
	i2c_start();
	
	//发送寻址地址为0xA0，写访问操作
	i2c_send_byte(0xA0);
	
	//等待应答
	ack = i2c_wait_ack();
	
	if(ack)
	{
		printf("24c02 ack device address fail\r\n");
		
		return;
	
	}

	printf("24c02 is online\r\n");

	//发送数据存储地址
	i2c_send_byte(addr*8);
	
	//等待应答
	ack = i2c_wait_ack();
	
	if(ack)
	{
		printf("24c02 ack word address 1 fail\r\n");
		
		return;
	
	}	
	
	printf("24c02 word address ok\r\n");	
	
	//重新发送启动信号
	i2c_start();
	
	//发送寻址地址为0xA1，读访问操作
	i2c_send_byte(0xA1);
	
	//等待应答
	ack = i2c_wait_ack();
	
	if(ack)
	{
		printf("24c02 ack device address 2 fail\r\n");
		
		return;
	
	}
	
	len=len-1;
	
	while(len--)
	{
		//接收数据
		*pbuf=i2c_recv_byte();
		
		//printf("[%d]%d \r\n" ,len,*pbuf);
		pbuf++;
		
		//主动发送应答给从机
		i2c_ack(0);
	}
	
	//接收数据
	*pbuf=i2c_recv_byte();
	printf("[%d]%d \r\n" ,len,*pbuf);
	
	//主动发送无应答给从机
	i2c_ack(1);	
	
	//发送停止信号，整个通信过程结束
	i2c_stop();
	
	printf("24c02 read ok\r\n");

}
