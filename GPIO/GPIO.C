/********************************** (C) COPYRIGHT *******************************
* File Name          : GPIO.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/09
* Description        : CH549 GPIO��غ���
*******************************************************************************/
#include ".\GPIO\GPIO.H"
#include "./qsport_arch/qsportArch.h"
#pragma  NOAREGS
/*******************************************************************************
* Function Name  : GPIO_Init(UINT8 PORTx,UINT8 PINx,UINT8 MODEx)
* Description    : GPIO�˿ڳ�ʼ������
* Input          : PORTx:0~4
*                  PINx:λ��,ÿ��λ��Ӧ��Port��һ������
*                  MODEx:
*                        0:��������ģʽ������û����������
*                        1:�������ģʽ�����жԳ�������������������������սϴ����
*                        2:��©�����֧�ָ������룬����û����������
*                        3:׼˫��ģʽ(��׼ 8051)����©�����֧�����룬��������������(Ĭ��ģʽ)
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Init(UINT8 PORTx,UINT8 PINx,UINT8 MODEx)
{
    UINT8 Px_DIR_PU,Px_MOD_OC;
    switch(PORTx)                                         //������ʼֵ
    {
    case PORT0:
        Px_MOD_OC = P0_MOD_OC;
        Px_DIR_PU = P0_DIR_PU;
        break;
    case PORT1:
        Px_MOD_OC = P1_MOD_OC;
        Px_DIR_PU = P1_DIR_PU;
        break;
    case PORT2:
        Px_MOD_OC = P2_MOD_OC;
        Px_DIR_PU = P2_DIR_PU;
        break;
    case PORT3:
        Px_MOD_OC = P3_MOD_OC;
        Px_DIR_PU = P3_DIR_PU;
        break;
    case PORT4:
        Px_MOD_OC = P4_MOD_OC;
        Px_DIR_PU = P4_DIR_PU;
        break;
    default :
        break;
    }
    switch(MODEx)
    {
    case MODE0:                                       //��������ģʽ������û����������
        Px_MOD_OC &= ~PINx;
        Px_DIR_PU &= ~PINx;
        break;
    case MODE1:                                       //�������ģʽ�����жԳ�������������������������սϴ����
        Px_MOD_OC &= ~PINx;
        Px_DIR_PU |= PINx;
        break;
    case MODE2:                                       //��©�����֧�ָ������룬����û����������
        Px_MOD_OC |= PINx;
        Px_DIR_PU &= ~PINx;
        break;
    case MODE3:                                       //׼˫��ģʽ(��׼ 8051)����©�����֧�����룬��������������
        Px_MOD_OC |= PINx;
        Px_DIR_PU |= PINx;
        break;
    default :
        break;
    }
    switch(PORTx)                                         //��д
    {
    case PORT0:
        P0_MOD_OC = Px_MOD_OC;
        P0_DIR_PU = Px_DIR_PU;
        break;
    case PORT1:
        P1_MOD_OC = Px_MOD_OC;
        P1_DIR_PU = Px_DIR_PU;
        break;
    case PORT2:
        P2_MOD_OC = Px_MOD_OC;
        P2_DIR_PU = Px_DIR_PU;
        break;
    case PORT3:
        P3_MOD_OC = Px_MOD_OC;
        P3_DIR_PU = Px_DIR_PU;
        break;
    case PORT4:
        P4_MOD_OC = Px_MOD_OC;
        P4_DIR_PU = Px_DIR_PU;
        break;
    default :
        break;
    }
}
/*******************************************************************************
* Function Name  : GPIO_INT_Init
* Description    : ������ RXD1_L��P15_L��P14_L��P03_L��P57_H��P46_L��RXD0_L ��չ���ŵ��ⲿ�ж�
*                  ͬʱ����������C51�� INT1_L��INT0_L ���ⲿ�жϴ���
*                  (RXD1_L��RXD0_L�������ĸ�����ȡ���������Ƿ�ӳ��)
* Input          : IntSrc:��9���ⲿ�жϣ���λ���ʾ�����嶨���GPIO.H
*                  Mode��0����ƽģʽ   1������ģʽ (ע����չ���ŵ��ж�ģʽ��ͳһ���õ�)
*                  NewState��0���رն�Ӧ�ⲿ�ж�ʹ��  1�������Ӧ�ⲿ�ж�
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_INT_Init( UINT16 IntSrc,UINT8 Mode,UINT8 NewState )
{
    /* �жϴ���ģʽ���� */
    if(Mode == INT_EDGE)                                //���ش���ģʽ
    {
        if(IntSrc & 0x7F)                               //������չ�ж�
        {
            GPIO_IE |= bIE_IO_EDGE;
        }
        if(IntSrc&INT_INT0_L)                           //�����ⲿ�ж�0
        {
            IT0 = 1;
        }
        if(IntSrc&INT_INT1_L)                           //�����ⲿ�ж�1
        {
            IT1 = 1;
        }
    }
    else                                                //��ƽ����ģʽ
    {
        if(IntSrc & 0x7F)                               //������չ�ж�
        {
            GPIO_IE &= ~bIE_IO_EDGE;
        }
        if(IntSrc&INT_INT0_L)                           //�����ⲿ�ж�0
        {
            IT0 = 1;
        }
        if(IntSrc&INT_INT1_L)                           //�����ⲿ�ж�1
        {
            IT1 = 1;
        }
    }
    /* �ж�ʹ��״̬ */
    if(NewState == Enable)                              //�����ⲿ�ж�
    {
        GPIO_IE |= ((UINT8)IntSrc&0x7F);
        if(IntSrc&INT_INT0_L)                           //�����ⲿ�ж�0
        {
            EX0 = 1;
        }
        if(IntSrc&INT_INT1_L)                           //�����ⲿ�ж�1
        {
            EX1 = 1;
        }
        if(GPIO_IE&0x7F)
        {
            IE_GPIO = 1;                                //������չGPIO�ж�
        }
        EA = 1;                                         //�������ж�
    }
    else                                                //�رն�Ӧ�ⲿ�ж�
    {
        GPIO_IE &= ~((UINT8)IntSrc&0x7F);
        if(IntSrc&INT_INT0_L)                           //�����ⲿ�ж�0
        {
            EX0 = 0;
        }
        if(IntSrc&INT_INT1_L)                           //�����ⲿ�ж�1
        {
            EX1 = 0;
        }
        if((GPIO_IE&0x7F)== 0)
        {
            IE_GPIO = 0;                                //�ر���չGPIO�ж�
        }
    }
}
/*******************************************************************************
* Function Name  : GPIO_ISR
* Description    : RXD1��P15��P14��P03��P57��P46��RXD0 �����ⲿ�жϷ�����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_EXT_ISR(void) interrupt INT_NO_GPIO
{
    if(AIN11==0)
    {
        mDelaymS(10);
//        printf("P03 Falling\n");
    }

    if(AIN5==0)
    {
        mDelaymS(10);
//        printf("P15 Falling\n");
    }


}
/*******************************************************************************
* Function Name  : GPIO_STD0_ISR
* Description    : INT0(P32) �����ⲿ�жϷ�����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//static UINT8 rotarySwitchIsWork = 0x00;
static UINT8 rotaryLevel = 0x00;//0-100
sbit rotarySwitch2Status = P3^3;
sbit rotarySwitch1Status = P3^2;

#define ROTARY_SWITCH_READ_TATOL_TIME 30
#define MINIMUM_EFFECT_TIME 20
static UINT16 rotarySwitch2StatusReadTotalCount = 0;
static UINT16 judgeInt0FallingEdgeReadTotalCount = 0;
static UINT16 judgeInt0RasingEdgeReadTotalCount = 0;

static UINT16 judgeInt1FallingEdgeReadTotalCount = 0;
static UINT16 judgeInt1RasingEdgeReadTotalCount = 0;

static UINT16 rotarySwitch1StatusReadTotalCount = 0;
static UINT16 rotarySwitch1StatusReadTotalCount_1 = 0;

//sbit TEST_LEDC = P4^0;
static UINT8 isInt0FallingEdge = 0;
static UINT8 isInt0RasingEdge = 0;
static UINT8 isInt1FallingEdge = 0;
static UINT8 isInt1RasingEdge = 0;

static UINT8 clockwiseCondition_1 = 0;
static UINT8 clockwiseCondition_2 = 0;

//UINT8 startComputeInterrupt0LowLevelTimeFlag = 0;
//UINT32 startComputeInterrupt0LowLevelTime = 0;
void GPIO_STD0_ISR(void) interrupt INT_NO_INT0
{
	UINT8 rotarySwitch2StatusReadTotalCountIndex = 0; 
	UINT8 judgeFallingEdgeIndex = 0;
	UINT8 judgeRasingEdgeIndex = 0;
	
	//TEST_LEDC = ~TEST_LEDC;
	
	//juage FallingEdge
	for(judgeFallingEdgeIndex=0; judgeFallingEdgeIndex<ROTARY_SWITCH_READ_TATOL_TIME; judgeFallingEdgeIndex++){
		if((UINT16)(rotarySwitch1Status) == 0){
				judgeInt0FallingEdgeReadTotalCount++;
				
		}	
	}
	if(judgeInt0FallingEdgeReadTotalCount>MINIMUM_EFFECT_TIME && (1 == (UINT16)(rotarySwitch2Status))){
			judgeInt0FallingEdgeReadTotalCount = 0;
			isInt0FallingEdge = 1;
	}

		//judge RasingEdge
	for(judgeRasingEdgeIndex=0; judgeRasingEdgeIndex<ROTARY_SWITCH_READ_TATOL_TIME; judgeRasingEdgeIndex++){
		if((UINT16)(rotarySwitch1Status) == 0){
				judgeInt0RasingEdgeReadTotalCount++;			
		}	
	}	
	if(judgeInt0RasingEdgeReadTotalCount>MINIMUM_EFFECT_TIME && (0 == (UINT16)(rotarySwitch2Status))){
			judgeInt0RasingEdgeReadTotalCount=0;
			isInt0RasingEdge = 1;
	}
	
	if(1 == clockwiseCondition_2 && 1 == isInt0FallingEdge && 1 == isInt0RasingEdge && isInt1FallingEdge && 1 == isInt1RasingEdge){
			isInt1FallingEdge = 0;
			isInt1RasingEdge = 0;
			clockwiseCondition_2 = 0;				
	}		
	
	
	if(1 == isInt0FallingEdge && (UINT16)(rotarySwitch1Status) == 0 && (UINT16)(rotarySwitch2Status) == 1){	
			for(rotarySwitch2StatusReadTotalCountIndex=0; rotarySwitch2StatusReadTotalCountIndex<ROTARY_SWITCH_READ_TATOL_TIME; rotarySwitch2StatusReadTotalCountIndex++){
					if((UINT16)(rotarySwitch2Status) > 0 && (UINT16)(rotarySwitch1Status) == 0){
							rotarySwitch2StatusReadTotalCount++;
					}
			}//remove jitter process
			
			if((UINT16)(rotarySwitch2StatusReadTotalCount) > MINIMUM_EFFECT_TIME && (UINT16)(rotarySwitch1Status) == 0){	
				//TEST_LEDC = ~TEST_LEDC;
				clockwiseCondition_1 = 1;
	
				if(rotaryLevel>=100){						
						rotaryLevel = 100;
				}else{					
						rotaryLevel++;					
				}				
				productEvent(DISPLAY_ROTARYSWITCH_LEVEL_OP,(void *)&rotaryLevel);
				
				//printf("int0 %d\n",(UINT16)rotaryLevel);
			}		
			
			rotarySwitch2StatusReadTotalCount = 0;//clear	
	}
			
	
}
/*******************************************************************************
* Function Name  : GPIO_STD1_ISR
* Description    : INT1(P33) �����ⲿ�жϷ�����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_STD1_ISR(void) interrupt INT_NO_INT1
{
	UINT8 rotarySwitch1StatusReadTotalCountIndex = 0; 
	UINT8 judgeInt1FallingEdgeIndex = 0;
	UINT8 judgeInt1RasingEdgeIndex = 0;

	//juage FallingEdge
	for(judgeInt1FallingEdgeIndex=0; judgeInt1FallingEdgeIndex<ROTARY_SWITCH_READ_TATOL_TIME; judgeInt1FallingEdgeIndex++){
		if((UINT16)(rotarySwitch2Status) == 0){
				judgeInt1FallingEdgeReadTotalCount++;
				
		}	
	}
	if(judgeInt1FallingEdgeReadTotalCount>MINIMUM_EFFECT_TIME && (1 == (UINT16)(rotarySwitch1Status))){
			judgeInt1FallingEdgeReadTotalCount = 0;
			isInt1FallingEdge = 1;
	}	
	
		//judge RasingEdge
	for(judgeInt1RasingEdgeIndex=0; judgeInt1RasingEdgeIndex<ROTARY_SWITCH_READ_TATOL_TIME; judgeInt1RasingEdgeIndex++){
		if((UINT16)(rotarySwitch2Status) == 1){
				judgeInt1RasingEdgeReadTotalCount++;			
		}	
	}	
	if(judgeInt1RasingEdgeReadTotalCount>MINIMUM_EFFECT_TIME && (0 == (UINT16)(rotarySwitch1Status))){
			judgeInt1RasingEdgeReadTotalCount=0;
			isInt1RasingEdge = 1;
	}
	
	if(1 == clockwiseCondition_1 && 1 == isInt0FallingEdge && 1 == isInt0RasingEdge && isInt1FallingEdge && 1 == isInt1RasingEdge){
			isInt0FallingEdge = 0;
			isInt0RasingEdge = 0;
			clockwiseCondition_1 = 0;
			//中断1的上升沿和下降沿需要在中断0里面清除
	}	

	
	if(1 == isInt1FallingEdge && (UINT16)(rotarySwitch1Status) == 1 && (UINT16)(rotarySwitch2Status) == 0){
			for(rotarySwitch1StatusReadTotalCountIndex=0; rotarySwitch1StatusReadTotalCountIndex<ROTARY_SWITCH_READ_TATOL_TIME ;rotarySwitch1StatusReadTotalCountIndex++){
					if((UINT16)(rotarySwitch1Status) > 0 && (UINT16)(rotarySwitch2Status) == 0){
							rotarySwitch1StatusReadTotalCount++;
					}
			}//remove jitter process
			
			if((UINT16)(rotarySwitch1StatusReadTotalCount) > MINIMUM_EFFECT_TIME && (UINT16)(rotarySwitch2Status) == 0){
					clockwiseCondition_2 = 1;
					
				
					if(rotaryLevel>=1){					
							rotaryLevel--;
							productEvent(DISPLAY_ROTARYSWITCH_LEVEL_OP,(void *)&rotaryLevel);
					}		

					//printf("int1 %d\n",(UINT16)rotaryLevel);
			}
			rotarySwitch1StatusReadTotalCount = 0;
	}


}

UINT8 getForceMeasureValue(void)
{
		return rotaryLevel;
}

void clearForceMeasureValue(void)
{
		rotaryLevel = 0;
}


