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

//����OLED��ʾ    
void OLED_Display_On(void) ;

//�ر�OLED��ʾ     
void OLED_Display_Off(void) ;

//��������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!	  
void OLED_Clear(void)   ;

//��������
void OLED_Set_Pos(unsigned char x, unsigned char y) ;

//��ʾ����
void OLED_ShowCHinese(u8 x,u8 y,u8 no) ;

//m^n����
u32 oled_pow(u8 m,u8 n) ;

//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~7 page
//mode:0,������ʾ;1,������ʾ				 
//size:ѡ������ 16/12 
//   OLED_ShowChar(   6,   3, '0'  , 16)
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size) ;

//��ָ��λ����ʾһ������,
//x:0~127
//y:0~7 page
//mode:0,������ʾ;1,������ʾ				 
//size:ѡ������ 16/12 
//   OLED_ShowChar(   6,   3, num  , 16)

void OLED_show_numb(u8 x,u8 y,u32 numb,u8 Char_Size);


//��ʾ2������
//x,y :�������	 
//len :���ֵ�λ��
//size:�����С
//mode:ģʽ	0,���ģʽ;1,����ģʽ
//num:��ֵ(0~4294967295);	 		  
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size2) ;


//��ʾһ���ַ��Ŵ� 
//OLED_ShowString(      6,   3,"0.96' OLED TEST",16);
void OLED_ShowString(u8 x,u8 y,u8 *chr          ,u8 Char_Size);


/***********������������ʾ��ʾBMPͼƬ128��64��ʼ������(x,y),x�ķ�Χ0��127��yΪҳ�ķ�Χ0��7*****************/
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]) ;

//��ʾ����
void show_Desktop( void );

#endif

