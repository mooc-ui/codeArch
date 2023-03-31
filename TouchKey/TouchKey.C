/********************************** (C) COPYRIGHT *******************************
* File Name          : TouchKey.C
* Author             : WCH
* Version            : V1.0
* Date               : 2019/07/22
* Description        : CH549 TouchKey��������
*                      CH0~CH15 �ֱ��Ӧ���� P1.0~P1.7  P0.0~P0.7
*******************************************************************************/
#include ".\TouchKey\TouchKey.H"
#pragma  NOAREGS

UINT16 IntCurValue = 0;                              //�жϲɼ����ĵ�ǰֵ
UINT16 KeyBuf[16][KEY_BUF_LEN];
/*******************************************************************************
* Function Name  : TouchKey_Init
* Description    : ����������ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TouchKey_Init( void )
{
                                                     //Touch����ͨ������Ϊ��������
    P0_MOD_OC &= 0xF0;                               //P00 P01 P02 P03��������
    P0_DIR_PU &= 0xF0;
    ADC_CFG |= (bADC_EN|bADC_AIN_EN);                //����ADCģ���Դ,ѡ���ⲿͨ��
    ADC_CFG = ADC_CFG & ~(bADC_CLK0 | bADC_CLK1);    //ѡ��ADC�ο�ʱ��
    ADC_CHAN = (3<<4);                               //Ĭ��ѡ���ⲿͨ��0
    ADC_CTRL = bADC_IF;                              //���ADCת����ɱ�־��д1����
#if EN_ADC_INT
    SCON2 &= ~bU2IE;                                  //��UART2�жϵ�ַ���ã����ж���2ѡ1
    IE_ADC = 1;                                      //����ADC�ж�ʹ��
    EA = 1;                                          //�������ж�ʹ��
#endif
}

/*******************************************************************************
* Function Name  : UpDataBuf
* Description    : ȥ���ƽ��ֵ�����˲�
				   ���´�������,���Ƴ�����newdat��������β,�����С KEY_BUF_LEN
*                  ����ȥֵ��ȡƽ������
* Input          : buf,newdat
* Output         : None
* Return         : None
*******************************************************************************/
UINT16X Temp_Buf[KEY_BUF_LEN];                                      //ר�û�����
UINT16 Buf_UpData_Filter(UINT16 *buf,UINT16 newdat)
{
	UINT8 i,j,k;
	UINT16 temp;
	UINT32 sum;


	//���¶���
	for(i=1;i!=KEY_BUF_LEN;i++)
	{
		buf[i-1] = buf[i];
	}
	buf[i-1] = newdat;

	
	//����
	memcpy(Temp_Buf,buf,sizeof(Temp_Buf));
	k = KEY_BUF_LEN-1;
	for( i = 0;i != k; i++)
	{
		for( j = i + 1;j != KEY_BUF_LEN; j++)
		{
			if(Temp_Buf[i] < Temp_Buf[j])
			{
				temp = Temp_Buf[i];
				Temp_Buf[i] = Temp_Buf[j];
				Temp_Buf[j] = temp;
			}
		}
	}
	
	//��ֵ��ȡƽ��
	sum = 0;
	k = KEY_BUF_LEN - KEY_BUF_LOST;
	for(i=KEY_BUF_LOST;i != k;i++)
	{
		sum+=Temp_Buf[i];
	}
	return (sum / (KEY_BUF_LEN - 2*KEY_BUF_LOST));
}


/*******************************************************************************
* Function Name  : TouchKey_Init
* Description    : ����������ʼ��
* Input          : ͨ����ѡ��ch:0~15,�ֱ��ӦP10~P17��P00~P07
                   ���������� cpw:0~127
                   cpw�� �ⲿ�����������ݡ�VDD��ѹ����Ƶ���߾�����
                   ���㹫ʽ��count=(Ckey+Cint)*0.7VDD/ITKEY/(2/Fsys)
                   TKEY_CTRL=count > 127 ? 127 : count ������CintΪ15pF,ITKEYΪ50u��
                   �򻯹�ʽ��cpw = ��Ckey+15��*0.35*VDD*Fsys/50
                   cpw = cpw>127?127:cpw
* Output         : None
* Return         : ���ش�������ѹ
*******************************************************************************/
UINT16 TouchKeySelect( UINT8 ch,UINT8 cpw )
{
    ADC_CHAN = ADC_CHAN & (~MASK_ADC_CHAN) | ch;     //�ⲿͨ��ѡ��
    //���ݽϴ�ʱ����������IO�ͣ�Ȼ��ָ���������ʵ���ֹ��ŵ磬��0.2us
    TKEY_CTRL = cpw;                                 //�������������ã�����7λ��Ч��ͬʱ���bADC_IF������һ��TouchKey��
    while(ADC_CTRL&bTKEY_ACT);
	IntCurValue = (ADC_DAT&0xFFF);
	TKEY_CTRL = 0;
	IntCurValue = Buf_UpData_Filter(&KeyBuf[ch][0],IntCurValue);
    return (IntCurValue);
}

UINT16 Default_TouchKey( UINT8 ch,UINT8 cpw )
{
    ADC_CHAN = ADC_CHAN & (~MASK_ADC_CHAN) | ch;     //�ⲿͨ��ѡ��
    //���ݽϴ�ʱ����������IO�ͣ�Ȼ��ָ���������ʵ���ֹ��ŵ磬��0.2us
    TKEY_CTRL = cpw;                                 //�������������ã�����7λ��Ч��ͬʱ���bADC_IF������һ��TouchKey��
    while(ADC_CTRL&bTKEY_ACT);
	IntCurValue = (ADC_DAT&0xFFF);
	TKEY_CTRL = 0;
    return (IntCurValue);
}