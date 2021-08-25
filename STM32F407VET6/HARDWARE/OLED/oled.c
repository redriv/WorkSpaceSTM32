#include "oled.h"
#include "oled_fun.h"



/******************  OLED  start***********************/

/****************************************8*16的点阵************************************/


#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

#define Max_Column	128
#define Max_Row		64

/**********************************************
// IIC Write Command
**********************************************/
void Write_IIC_Command(unsigned char IIC_Command)
{
	i2c_start();
	i2c_send_byte(0x78);            //Slave address,SA0=0
	i2c_wait_ack();	
	i2c_send_byte(0x00);			//write command
	i2c_wait_ack();	
	i2c_send_byte(IIC_Command); 
	i2c_wait_ack();	
	i2c_stop();
}


/**********************************************
// IIC Write Data
**********************************************/
void Write_IIC_Data(unsigned char IIC_Data)
{
		i2c_start();
		i2c_send_byte(0x78);			//D/C#=0; R/W#=0
		i2c_wait_ack();	
		i2c_send_byte(0x40);			//write data  开始传递数据
		i2c_wait_ack();	
		i2c_send_byte(IIC_Data); 
		i2c_wait_ack();	
		i2c_stop();
}


void OLED_WR_Byte( uint8_t val , uint8_t cmd)
{
	if(cmd == 0) // cmd
		Write_IIC_Command(val);
	else		 // data
	  Write_IIC_Data(val);					
		
}

static void oled_Delay(void)
{
	int i;

	/*　
		可用逻辑分析仪测量I2C通讯时的频率
    工作条件：CPU主频168MHz ，MDK编译环境，1级优化
  
		经测试，循环次数为20~250时都能通讯正常

	*/
	for (i = 0; i < 4000; i++);
}
void SSD1315_init(void)
{
	oled_Delay();

	OLED_WR_Byte(0xAE,OLED_CMD);//--display off
	OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  
	OLED_WR_Byte(0xB0,OLED_CMD);//--set page address
	OLED_WR_Byte(0x81,OLED_CMD); // contract control
	OLED_WR_Byte(0xFF,OLED_CMD);//--128   
	OLED_WR_Byte(0xA1,OLED_CMD);//set segment remap 
	OLED_WR_Byte(0xA6,OLED_CMD);//--normal / reverse
	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3F,OLED_CMD);//--1/32 duty
	OLED_WR_Byte(0xC8,OLED_CMD);//Com scan direction
	OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset
	OLED_WR_Byte(0x00,OLED_CMD);//
	
	OLED_WR_Byte(0xD5,OLED_CMD);//set osc division
	OLED_WR_Byte(0x80,OLED_CMD);//
	
	OLED_WR_Byte(0xD8,OLED_CMD);//set area color mode off
	OLED_WR_Byte(0x05,OLED_CMD);//
	
	OLED_WR_Byte(0xD9,OLED_CMD);//Set Pre-Charge Period
	OLED_WR_Byte(0xF1,OLED_CMD);//
	
	OLED_WR_Byte(0xDA,OLED_CMD);//set com pin configuartion
	OLED_WR_Byte(0x12,OLED_CMD);//
	
	OLED_WR_Byte(0xDB,OLED_CMD);//set Vcomh
	OLED_WR_Byte(0x30,OLED_CMD);//
	
	OLED_WR_Byte(0x8D,OLED_CMD);//set charge pump enable
	OLED_WR_Byte(0x14,OLED_CMD);//
	
	OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel
}

//开启OLED显示    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//关闭OLED显示     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}


//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void OLED_Clear(void)  
{  
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		

		for(n=0;n<128;n++)
			OLED_WR_Byte(0,OLED_DATA); 
	} //更新显示
}

//坐标设置
void OLED_Set_Pos(unsigned char x, unsigned char y) 	
{ 	
	OLED_WR_Byte(0xb0+y,OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f),OLED_CMD); 
} 

//显示汉字
void OLED_ShowCHinese(u8 x,u8 y,u8 no)
{      			    
	u8 t,adder=0;
	OLED_Set_Pos(x,y);	
    for(t=0;t<16;t++)
	{
				OLED_WR_Byte(Hzk[2*no][t],OLED_DATA);
				adder+=1;
    }	
	OLED_Set_Pos(x,y+1);	
    for(t=0;t<16;t++)
	{	
				OLED_WR_Byte(Hzk[2*no+1][t],OLED_DATA);
				adder+=1;
    }					
}

//m^n函数
u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}			

	


//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~7 page
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
//   OLED_ShowChar(   6,   3, '0'  , 16)
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size)
{      	
	unsigned char c=0,i=0;	
		c=chr -' ';//得到偏移后的值			
		if(x>Max_Column-1)
		{ 
			x=0;y=y+2; 
		}
		if(Char_Size ==16)
		{
			OLED_Set_Pos(x,y);	
			for(i=0;i<8;i++) // 一个字符中的前8行，每行是1Byte (8bit)像素点都显示出来
				OLED_WR_Byte(F8X16[c*16+i],OLED_DATA);  // F8X16 字模数据
			
			
			OLED_Set_Pos(x,y+1); // 以1page(8行)为单位  +1 也就是往下移动1page
			
			for(i=0;i<8;i++)// 一个字符中的后8*8个像素点都显示出来
				OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA);
		}
}

//在指定位置显示一个数字,
//x:0~127
//y:0~7 page
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
//   OLED_ShowChar(   6,   3, num  , 16)

void OLED_show_numb(u8 x,u8 y,u32 numb,u8 Char_Size)
{
	u32 i,j,k,m;
	u8 tmp = 9;
	if(numb > 3000) printf("data error");
	 i = numb/1000;      //千位
	 j = numb/100 % 10 ; //百位
	 k = numb/10  % 10 ; //十位
	 m = numb % 10 ;     //个位
	
	switch (i)
	{
		case 0: {OLED_ShowChar( x, y , '0' ,Char_Size ) ;break ;}
		case 1: {OLED_ShowChar( x, y , '1' ,Char_Size ) ;break ;}
		case 2: {OLED_ShowChar( x, y , '2' ,Char_Size ) ;break ;}
		case 3: {OLED_ShowChar( x, y , '3' ,Char_Size ) ;break ;}
		case 4: {OLED_ShowChar( x, y , '4' ,Char_Size ) ;break ;}
		case 5: {OLED_ShowChar( x, y , '5' ,Char_Size ) ;break ;}
		case 6: {OLED_ShowChar( x, y , '6' ,Char_Size ) ;break ;}
		case 7: {OLED_ShowChar( x, y , '7' ,Char_Size ) ;break ;}
		case 8: {OLED_ShowChar( x, y , '8' ,Char_Size ) ;break ;}
		case 9: {OLED_ShowChar( x, y , '9' ,Char_Size ) ;break ;}
	}

	x+=tmp;
	switch (j)
	{
		case 0: {OLED_ShowChar( x, y , '0' ,Char_Size ) ;break ;}
		case 1: {OLED_ShowChar( x, y , '1' ,Char_Size ) ;break ;}
		case 2: {OLED_ShowChar( x, y , '2' ,Char_Size ) ;break ;}
		case 3: {OLED_ShowChar( x, y , '3' ,Char_Size ) ;break ;}
		case 4: {OLED_ShowChar( x, y , '4' ,Char_Size ) ;break ;}
		case 5: {OLED_ShowChar( x, y , '5' ,Char_Size ) ;break ;}
		case 6: {OLED_ShowChar( x, y , '6' ,Char_Size ) ;break ;}
		case 7: {OLED_ShowChar( x, y , '7' ,Char_Size ) ;break ;}
		case 8: {OLED_ShowChar( x, y , '8' ,Char_Size ) ;break ;}
		case 9: {OLED_ShowChar( x, y , '9' ,Char_Size ) ;break ;}
	}	
	x+=tmp;
	switch (k)
	{
		case 0: {OLED_ShowChar( x, y , '0' ,Char_Size ) ;break ;}
		case 1: {OLED_ShowChar( x, y , '1' ,Char_Size ) ;break ;}
		case 2: {OLED_ShowChar( x, y , '2' ,Char_Size ) ;break ;}
		case 3: {OLED_ShowChar( x, y , '3' ,Char_Size ) ;break ;}
		case 4: {OLED_ShowChar( x, y , '4' ,Char_Size ) ;break ;}
		case 5: {OLED_ShowChar( x, y , '5' ,Char_Size ) ;break ;}
		case 6: {OLED_ShowChar( x, y , '6' ,Char_Size ) ;break ;}
		case 7: {OLED_ShowChar( x, y , '7' ,Char_Size ) ;break ;}
		case 8: {OLED_ShowChar( x, y , '8' ,Char_Size ) ;break ;}
		case 9: {OLED_ShowChar( x, y , '9' ,Char_Size ) ;break ;}
	}	
	x+=tmp;
	switch (m)
	{
		case 0: {OLED_ShowChar( x, y , '0' ,Char_Size ) ;break ;}
		case 1: {OLED_ShowChar( x, y , '1' ,Char_Size ) ;break ;}
		case 2: {OLED_ShowChar( x, y , '2' ,Char_Size ) ;break ;}
		case 3: {OLED_ShowChar( x, y , '3' ,Char_Size ) ;break ;}
		case 4: {OLED_ShowChar( x, y , '4' ,Char_Size ) ;break ;}
		case 5: {OLED_ShowChar( x, y , '5' ,Char_Size ) ;break ;}
		case 6: {OLED_ShowChar( x, y , '6' ,Char_Size ) ;break ;}
		case 7: {OLED_ShowChar( x, y , '7' ,Char_Size ) ;break ;}
		case 8: {OLED_ShowChar( x, y , '8' ,Char_Size ) ;break ;}
		case 9: {OLED_ShowChar( x, y , '9' ,Char_Size ) ;break ;}
	}	
	



}
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size2)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size2/2)*t,y,' ',size2);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size2/2)*t,y,temp+'0',size2); 
	}
} 




//显示一个字符号串 
//OLED_ShowString( 6,   3,"0.96' OLED TEST",16);
void OLED_ShowString(u8 x,u8 y,u8 *chr ,u8 Char_Size)
{
	unsigned char j=0;
	while (chr[j]!='\0')
	{		
		OLED_ShowChar(x,y,chr[j],Char_Size);// OLED_ShowChar(6, 3, '0' , 16)
		x+=8; // 目前字库中每个字符占8*16
		if(x>120)
		{ 
			x=0;
			y+=2; //换行
		}
		j++;
	}
}

/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{ 	
 unsigned int j=0;
 unsigned char x,y;
  
  if(y1%8==0) y=y1/8;      
  else y=y1/8+1;
	for(y=y0;y<y1;y++)
	{
		OLED_Set_Pos(x0,y);
    for(x=x0;x<x1;x++)
	    {      

	    	OLED_WR_Byte(BMP[j++],OLED_DATA);	    	
	    }
	}
} 

/******************  OLED  end ***********************/


//显示桌面
void show_Desktop( void )
{
//				OLED_ShowCHinese(0, 0,0);//中
//				OLED_ShowCHinese(18,0,1);//景
//				OLED_ShowCHinese(36,0,2);//园
//				OLED_ShowCHinese(54,0,3);//电
//				OLED_ShowCHinese(72,0,4);//子
//				OLED_ShowCHinese(90,0,5);//科
//				OLED_ShowCHinese(108,0,6);//技
	
				OLED_ShowString(0,0,"[FreeRTOS_Test]",16);					
				OLED_ShowString(0,2,"[LED_TASK]:",16);
				OLED_ShowString(0,4,"[KEY_TASK]:",16);  
				OLED_ShowNum(90,4,1,2,16);
				OLED_ShowString(0,6,"Task running!",16);
					
				


}

