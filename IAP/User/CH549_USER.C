/********************************** (C) COPYRIGHT ******************************
* File Name          : CH549_USER.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/24
* Description        : �û�������ʾ�û��������У���P46����͵�ƽʱ��������ת��IAP
*                      �������������û�����������
                       D2����
                       ע�����DEBUG.C
*******************************************************************************/
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
#pragma NOAREGS
#define IAP_StartAddr     0xE000                            //IAP�����ŵ���ʼ��ַ���õ�ַ����Ҫ��ʵ�ʵ�IAP��ַС4�ֽ�
sbit    EnableIAP  =      P4^6;                             //IAP��ת�������
sbit    D2 = P2^2;
typedef void (*Function)( void );
Function Jump2IAPAddr;
/*******************************************************************************
* Function Name  : main
* Description    : ������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void main( void )
{
    UINT32 i=0;
    CfgFsys();
    mDelaymS(5);
    mInitSTDIO( );
    printf("User Demo start...\n");
    Jump2IAPAddr = (Function)IAP_StartAddr;                //����ָ�븳ֵ,ָ��IAP�����ַ
    while(1)
    {
        if( EnableIAP == 0 )                               //���P46�����Ƿ�Ϊ��
        {
            printf("Jump to IAP..\n");
            Jump2IAPAddr( );                               //��ת��IAP������
        }
        i++;
        if(i== 200000)
        {
            printf("User APP Wait..\n");
            D2 = ~D2;
            i = 0;
        }
    }
}