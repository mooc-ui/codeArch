#ifndef __SIMULATION_I2C_H_
#define __SIMULATION_I2C_H_

#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"

sbit SCL=P4^4;
sbit SDA=P0^3;
void I2cStart();//start signal
void I2cStop();//end singnal
unsigned char I2cSendByte(unsigned char dat);//发送一字节数据
unsigned char I2cReadByte();//读取一字节数据
void At24c02Write(unsigned char addr,unsigned char dat);//向AT24C02指定地址写入数据
unsigned char At24c02Read(unsigned char addr);//读取AT24C20指定地址内容。



#endif