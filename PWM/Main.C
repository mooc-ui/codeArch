/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/23
* Description        : CH549 PWM��ʼ����ռ�ձ����ã�PWMĬ�ϵ�ƽ����
*                      ֧���жϷ�ʽ�޸�PWMռ�ձ�
*                      ����           ����
*                      P25            PWM0
*                      P24            PWM1
*                      P23            PWM2
*                      P22            PWM3
*                      P21            PWM4
*                      P20            PWM5
*                      P26            PWM6
*                      P27            PWM7
                       ע�����DEBUG.C
*******************************************************************************/
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
#include ".\PWM\PWM.H"
#pragma  NOAREGS
void main( )
{
    CfgFsys( );                                                                //CH549ʱ��ѡ������
    mDelaymS(5);                                                               //����ʱ�Ӻ󣬽�����ʱ�ȶ�ʱ��
    mInitSTDIO( );                                                             //����0��ʼ��
    printf("PWM Demo start ...\n");
    /* ʱ�� Ƶ������ */
    SetPWMClkDiv(4);                                                           //PWMʱ������,FREQ_SYS/4
    SetPWMCycle256Clk();                                                       //PWM���� FREQ_SYS/4/256
    /* PWM0 PWM1�����Ч��������(��ѡ) */
    PWM0OutPolarLowAct();
    PWM1OutPolarHighAct();
    /* ��ʼռ�ձ����� */
    SetPWM0Dat(10);
    SetPWM1Dat(25);
    SetPWM2Dat(50);
    SetPWM3Dat(60);
    SetPWM4Dat(64);
    SetPWM5Dat(175);
    SetPWM6Dat(200);
    SetPWM7Dat(250);
    /* ����ͨ�� */
    PWM_SEL_CHANNEL( CH0|CH1|CH2|CH3|CH4|CH5|CH6|CH7,Enable );                 //���ʹ��
#ifdef PWM_INTERRUPT
    PWMInterruptEnable();
#endif
    while(1)
    {
    }
}