#ifndef __I2C_H__
#define __I2C_H__

#include "stm32f4xx.h"
#include "stdio.h"

//位带操作,实现51类似的GPIO控制功能
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 

//IO口地址映射

#define GPIOB_ODR_Addr    (GPIOB_BASE+20) //0x40020414 



#define GPIOB_IDR_Addr    (GPIOB_BASE+16) //0x40020410 
 

//IO口操作,只对单一的IO口!
//确保n的值小于16!


#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 



#define SCL		PBout(8)
#define SDA_W	PBout(9)
#define SDA_R	PBin(9) 


void i2c_init(void);
void at24c02_read(uint8_t addr,uint8_t *pbuf,uint8_t len);
void at24c02_write(uint8_t addr,uint8_t *pbuf,uint8_t len);  // 页码 0     pbuf = {1.....1}       写入8个 
uint8_t i2c_wait_ack(void); // 主机等从机发送来ACK
void i2c_send_byte(uint8_t txd) ;
void i2c_start(void);
void i2c_stop(void);
#endif

