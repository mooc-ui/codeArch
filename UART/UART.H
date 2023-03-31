#ifndef __UART_H__
#define __UART_H__
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"

#define UART0_MAX_RECEIVE_NUM_BYTE 30

#define   UART0_BUAD           115200               //����0����������

/* UART1��� */
                       //�����Ƿ����1�ж�
#define   UART1_BUAD           115200               //����1����������
/* UART2��� */
#define   UART2_INTERRUPT                           //�����Ƿ����2�ж�
#define   UART2_BUAD           9600//115200               //����2����������
/* UART3��� */
//#define   UART3_INTERRUPT                           //�����Ƿ����3�ж�
#define   UART3_BUAD           115200               //����2����������
/* �ⲿ�����Ӻ��� */
extern void  CH549UART1Init();                      //UART1��ʼ������
extern void  CH549UART1Alter();                     //����1����ӳ��
extern UINT8 CH549UART1RcvByte( );                  //UART1����һ���ֽ�
extern void  CH549UART1SendByte(UINT8 SendDat);     //UART1����һ���ֽ�
extern void  CH549UART2Init();                      //UART2��ʼ������
extern UINT8 CH549UART2RcvByte( );                  //UART2����һ���ֽ�
extern void  CH549UART2SendByte(UINT8 SendDat);     //UART2����һ���ֽ�
extern void  CH549UART3Init();                      //UART3��ʼ������
extern UINT8 CH549UART3RcvByte( );                  //UART3����һ���ֽ�
extern void  CH549UART3SendByte(UINT8 SendDat);     //UART3����һ���ֽ�

char uart0ReceiveByteData(void);
void clearUart0ReceiveData(void);
BOOL uart0ReceiveDataIsSuccess(char *dataArray);

void CH549UART0SendByte(UINT8 SendDat);



#endif
