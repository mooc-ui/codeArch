/********************************** (C) COPYRIGHT *******************************
* File Name          : UART.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/09
* Description        : CH549 ���ڳ�ʼ�����
*******************************************************************************/
#include ".\UART\UART.H"
#include "./qsport_arch/qsportArch.h"
#pragma  NOAREGS
/*******************************************************************************
* Function Name  : CH549UART1Init
* Description    : UART1��ʼ������,����Ĭ������ P26 P27
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH549UART1Init()
{
    SCON1 &= ~bU1SM0;                            //ѡ��8λ����ͨѶ
    SCON1 |= bU1SMOD;                            //����ģʽ
    SCON1 |= bU1REN;                             //ʹ�ܽ���
    SBAUD1 = 0 - FREQ_SYS/16/UART1_BUAD;         //����������
    SIF1 = bU1TI;                                //��շ�����ɱ�־
}
/*******************************************************************************
* Function Name  : CH549UART1Alter()
* Description    : CH549����1����ӳ��,����ӳ�䵽 P16 P17
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH549UART1Alter()
{
    P1_MOD_OC |= (3<<6);                                                   //׼˫��ģʽ
    P1_DIR_PU |= (3<<6);
    PIN_FUNC |= bUART1_PIN_X;                                              //�������Ÿ��ù���
}
/*******************************************************************************
* Function Name  : CH549UART1RcvByte()
* Description    : CH549UART1����һ���ֽ�
* Input          : None
* Output         : None
* Return         : SBUF
*******************************************************************************/
UINT8  CH549UART1RcvByte( )
{
    while((SIF1&bU1RI) == 0)
    {
        ;    //��ѯ����
    }
    SIF1 = bU1RI;                                                          //��������ж�
    return SBUF1;
}
/*******************************************************************************
* Function Name  : CH549UART1SendByte(UINT8 SendDat)
* Description    : CH549UART1����һ���ֽ�
* Input          : UINT8 SendDat��Ҫ���͵�����
* Output         : None
* Return         : None
*******************************************************************************/
void CH549UART1SendByte(UINT8 SendDat)
{
    SBUF1 = SendDat;
    while((SIF1&bU1TI) == 0)
    {
        ;    //��ѯ�ȴ��������
    }
    SIF1 = bU1TI;                                                          //�����������ж�
}
#ifdef UART1_INTERRUPT
/*******************************************************************************
* Function Name  : UART1Interrupt(void)
* Description    : UART1 �жϷ������
*******************************************************************************/
void UART1Interrupt( void ) interrupt INT_NO_UART1 using 1                //����1�жϷ������,ʹ�üĴ�����1
{
    UINT8 dat;
    if(SIF1&bU1RI)
    {
        SIF1 = bU1RI;                                                     //����������ж�
        dat = SBUF1;
        CH549UART1SendByte(dat);
    }
}
#endif

/*******************************************************************************
* Function Name  : CH549UART0SendByte(UINT8 SendDat)
* Description    : CH549UART0����һ���ֽ�
* Input          : UINT8 SendDat��Ҫ���͵�����
* Output         : None
* Return         : None
*******************************************************************************/
void CH549UART0SendByte(UINT8 SendDat)
{
		//ES = 0;
    SBUF = SendDat;
    while(TI == 0)
    {
        ;    //��ѯ�ȴ��������
    }
    TI = 0;                                                          //�����������ж�
		RI = 0;
		
		//ES = 1;
}

/*******************************************************************************
* Function Name  : UART0Interrupt(void)
* Description    : UART0 �жϷ������
*******************************************************************************/
static char Uart0ReceiveData = 0;

void UART0Interrupt( void ) interrupt INT_NO_UART0 using 1                //����0�жϷ������,ʹ�üĴ�����1
{
    UINT8 dat;
		dat = SBUF;
		//printf("enter uart0 int %02x\n",(UINT16)dat);//don't print log

		productEvent(UART_RECEIVE_OP,NULL);
		
    if(RI){
        RI = 0;                                                     //����������ж�			
        Uart0ReceiveData = dat;
    }else{
					TI = 0;	
		}
}

char uart0ReceiveByteData(void)
{
		char ReceiveData;
		ReceiveData = Uart0ReceiveData;
		Uart0ReceiveData = 0;//clear
		return ReceiveData;
}

//#endif

/*******************************************************************************
* Function Name  : CH549UART2Init
* Description    : UART2��ʼ������,����Ĭ������ P04 P05
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH549UART2Init()
{
    SCON2 &= ~bU2SM0;                            //ѡ��8λ����ͨѶ
    SCON2 |= bU2SMOD;                            //����ģʽ
    SCON2 |= bU2REN;                             //ʹ�ܽ���
    SBAUD2 = 0 - FREQ_SYS/16/UART2_BUAD;         //����������
    SIF2 = bU2TI;                                //��շ�����ɱ�־
#ifdef UART2_INTERRUPT                           //�����ж�ʹ��
    SCON2 |= bU2IE;                              //����UART2�жϣ��ر�ADC�ж�
    IE_UART2 = 1;
    EA = 1;
#endif
}
/*******************************************************************************
* Function Name  : CH549UART2RcvByte()
* Description    : CH549UART2����һ���ֽ�
* Input          : None
* Output         : None
* Return         : SBUF
*******************************************************************************/
UINT8 CH549UART2RcvByte( )
{
    while((SIF2&bU2RI) == 0)
    {
        ;    //��ѯ����
    }
    SIF2 = bU2RI;                                                          //��������ж�
    return SBUF2;
}
/*******************************************************************************
* Function Name  : CH549UART2SendByte(UINT8 SendDat)
* Description    : CH549UART2����һ���ֽ�
* Input          : UINT8 SendDat��Ҫ���͵�����
* Output         : None
* Return         : None
*******************************************************************************/
void CH549UART2SendByte(UINT8 SendDat)
{
    SBUF2 = SendDat;
    while((SIF2&bU2TI) == 0)
    {
        ;    //��ѯ�ȴ��������
    }
    SIF2 = bU2TI;                                                          //�����������ж�
}
#define UART2_INTERRUPT

#ifdef UART2_INTERRUPT
/*******************************************************************************
* Function Name  : UART1Interrupt(void)
* Description    : UART1 �жϷ������
*******************************************************************************/
void UART2Interrupt( void ) interrupt INT_NO_UART2 using 1                //����2�жϷ������,ʹ�üĴ�����1
{
    UINT8 dat;
    if(SIF2&bU2RI)
    {
        SIF2 = bU2RI;                                                     //����������ж�
        dat = SBUF2;
        CH549UART2SendByte(dat);
    }
}
#endif
/*******************************************************************************
* Function Name  : CH549UART3Init
* Description    : UART3��ʼ������,����Ĭ������ P06 P07
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH549UART3Init()
{
    SCON3 &= ~bU3SM0;                            //ѡ��8λ����ͨѶ
    SCON3 |= bU3SMOD;                            //����ģʽ
    SCON3 |= bU3REN;                             //ʹ�ܽ���
    SBAUD3 = 0 - FREQ_SYS/16/UART3_BUAD;         //����������
    SIF3 = bU3TI;                                //��շ�����ɱ�־
#ifdef UART3_INTERRUPT                           //�����ж�ʹ��
    SCON3 |= bU3IE;                              //����UART3�жϣ��ر�PWM�ж�
    IE_UART3 = 1;
    EA = 1;
#endif
}
/*******************************************************************************
* Function Name  : CH549UART3RcvByte()
* Description    : CH549UART3����һ���ֽ�
* Input          : None
* Output         : None
* Return         : SBUF
*******************************************************************************/
UINT8 CH549UART3RcvByte( )
{
    while((SIF3&bU3RI) == 0)
    {
        ;    //��ѯ����
    }
    SIF3 = bU3RI;                                                          //��������ж�
    return SBUF3;
}
/*******************************************************************************
* Function Name  : CH549UART3SendByte(UINT8 SendDat)
* Description    : CH549UART3����һ���ֽ�
* Input          : UINT8 SendDat��Ҫ���͵�����
* Output         : None
* Return         : None
*******************************************************************************/
void CH549UART3SendByte(UINT8 SendDat)
{
    SBUF3 = SendDat;
    while((SIF3&bU3TI) == 0)
    {
        ;    //��ѯ�ȴ��������
    }
    SIF3 = bU3TI;                                                          //�����������ж�
}
#ifdef UART3_INTERRUPT
/*******************************************************************************
* Function Name  : UART1Interrupt(void)
* Description    : UART1 �жϷ������
*******************************************************************************/
void UART3Interrupt( void ) interrupt INT_NO_UART3 using 1                //����3�жϷ������,ʹ�üĴ�����1
{
    UINT8 dat;
    if(SIF3&bU3RI)
    {
        SIF3 = bU3RI;                                                     //����������ж�
        dat = SBUF3;
        CH549UART3SendByte(dat);
    }
}
#endif
