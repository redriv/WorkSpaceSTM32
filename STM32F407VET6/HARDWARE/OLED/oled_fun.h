#ifndef __OLED_fun_H__
#define __OLED_fun_H__


#include "stm32f4xx.h"
#include "i2c.h"
#include "stdio.h"


/**********************************************
// IIC Write Command
***********************************************/
void Write_IIC_Command(unsigned char IIC_Command) ;


/**********************************************
// IIC Write Data
**********************************************/
void Write_IIC_Data(unsigned char IIC_Data) ;

void OLED_WR_Byte( uint8_t val , uint8_t cmd) ;

void SSD1315_init(void) ;

//开启OLED显示    
void OLED_Display_On(void) ;

//关闭OLED显示     
void OLED_Display_Off(void) ;

//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void OLED_Clear(void)   ;

//坐标设置
void OLED_Set_Pos(unsigned char x, unsigned char y) ;

//显示汉字
void OLED_ShowCHinese(u8 x,u8 y,u8 no) ;

//m^n函数
u32 oled_pow(u8 m,u8 n) ;

//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~7 page
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
//   OLED_ShowChar(   6,   3, '0'  , 16)
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size) ;

//在指定位置显示一个数字,
//x:0~127
//y:0~7 page
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
//   OLED_ShowChar(   6,   3, num  , 16)

void OLED_show_numb(u8 x,u8 y,u32 numb,u8 Char_Size);


//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size2) ;


//显示一个字符号串 
//OLED_ShowString(      6,   3,"0.96' OLED TEST",16);
void OLED_ShowString(u8 x,u8 y,u8 *chr          ,u8 Char_Size);


/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]) ;

//显示桌面
void show_Desktop( void );

#endif

