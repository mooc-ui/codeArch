/********************************** (C) COPYRIGHT *******************************
* File Name          : ADC.C
* Author             : WCH
* Version            : V1.0
* Date               : 2019/07/22
* Description        : CH549 ADC����ʱ�����ã�ADCͨ�����ú������¶ȼ���ʼ��
*******************************************************************************/
#include "ADC.H"
#pragma  NOAREGS
/*******************************************************************************
* Function Name  : ADC_ExInit
* Description    : ADC �ⲿͨ����ʼ������
* Input          : UINT8 AdcClk
                   0 :512 Fosc cycles for each ADC
                   1 :256 Fosc cycles for each ADC
                   2 :128 Fosc cycles for each ADC
                   3 :64  Fosc cycles for each ADC
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_ExInit( UINT8 AdcClk )
{
    UINT8 dat;
    ADC_CFG |= bADC_EN;                              //����ADCģ���Դ
    ADC_CFG = (ADC_CFG & ~(bADC_CLK0 | bADC_CLK1))|AdcClk;//ѡ��ADC�ο�ʱ��
    ADC_CFG |= bADC_AIN_EN;                          //�����ⲿͨ��
    dat = ADC_DAT;                                   //�ն�
    ADC_CTRL = bADC_IF;                              //���ADCת����ɱ�־��д1����
#if EN_ADC_INT
     SCON2 &= ~bU2IE;                                  //��UART2�жϵ�ַ���ã����ж���2ѡ1
    IE_ADC = 1;                                      //����ADC�ж�ʹ��
    EA = 1;                                          //�������ж�ʹ��
#endif
}
/*******************************************************************************
* Function Name  : ADC_ChSelect
* Description    : ADC �ⲿͨ��ѡ��
* Input          : UINT8 ch
                   ch, λ�� 0~15bit�ֱ��Ӧ�ⲿ16ͨ��
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_ChSelect( UINT8 ch )
{
    ADC_CHAN = (ADC_CHAN & ~MASK_ADC_CHAN) | ch;
    if(ch<=7)                                  //P10~P17��������,����ÿ�ζ�����
    {
        P1_MOD_OC &= ~(1<<ch);                       //��������
        P1_DIR_PU &= ~(1<<ch);				
							
    }
    if(ch>7 && ch<=0x0f)                                  //P00~P07�������ã�����ÿ�ζ�����
    {
        P0_MOD_OC &= ~(1<<(ch-8));                  //��������
        P0_DIR_PU &= ~(1<<(ch-8));
			
    }
}
/*******************************************************************************
* Function Name  : ADC_InTSInit
* Description    : ADC �ڲ�ͨ���¶�ͨ����ʼ��
                   �����ڲ��¶ȼ�⣬��ر������ⲿ����ͨ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_InTSInit(void)
{
    ADC_CFG |= bADC_EN;                              //����ADCģ���Դ
    ADC_CFG = ADC_CFG & ~(bADC_CLK0 | bADC_CLK1);    //ѡ��ADC�ο�ʱ�� 750KHz
    ADC_CFG &= ~bADC_AIN_EN;                         //�ر��ⲿͨ��
    //ADC_CHAN = ADC_CHAN &~MASK_ADC_I_CH | (3<<4);    //�ڲ�ͨ��3Ϊ�¶ȼ��ͨ��
		ADC_CHAN = ADC_CHAN &~MASK_ADC_I_CH | (1<<4);    //�ڲ�ͨ��3Ϊ�¶ȼ��ͨ��
    ADC_CTRL |= bADC_IF;                             //���ADCת����ɱ�־��д1����
#if EN_ADC_INT
     SCON2 &= ~bU2IE;                                  //��UART2�жϵ�ַ���ã����ж���2ѡ1
    IE_ADC = 1;                                      //����ADC�ж�ʹ��
    EA = 1;                                          //�������ж�ʹ��
#endif
}

void ADCInternalChannelSelcet(UINT8 internalChannelSelect)
{
		ADC_CHAN = ADC_CHAN &~MASK_ADC_I_CH | (internalChannelSelect<<4);  
}

#if EN_ADC_INT
/*******************************************************************************
* Function Name  : ADC_ISR( )
* Description    : ADC �жϷ�����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_ISR(void)  interrupt INT_NO_ADC
{
    if(ADC_CTRL&bADC_IF)
    {
        ADC_CTRL = bADC_IF;                          //���ADCת������жϱ�־
        GetValueFlag = 1;			
    }	
    ADC_ChSelect(CH0);                               //ѡ����һ����Ҫ�ɼ���ͨ��	
}
#endif
