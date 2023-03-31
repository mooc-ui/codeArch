/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/09
* Description        : CH549 �ⲿͨ�����ڲ��ο���ѹ�Ƚϣ��ⲿͨ�����ⲿͨ���Ƚϣ�
                       �ڲ��ο���ѹ���ⲿͨ���Ƚ�
*******************************************************************************/
#include "CH549.H"
#include "DEBUG.H"
#include "Compare.H"
#pragma  NOAREGS
void main()
{
    CfgFsys( );                                                                  //CH549ʱ��ѡ������
    mDelaymS(20);
    mInitSTDIO( );                                                               //����0��ʼ��
    printf("CMP demo start ...\n");
    CMP_Init();                                                                  //�Ƚ���ͨ����ʼ��
    while(1)
    {
#if (EN_ADC_INT==0)
        if(ADC_CTRL&bCMPO)
        {
            printf("+ > -\n");
        }
        else
        {
            printf("+ < -\n");
        }
        mDelaymS(1000);
#endif
    }
}