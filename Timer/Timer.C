/********************************** (C) COPYRIGHT *******************************
* File Name          : Timer.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/21
* Description        : CH549 Time ��ʼ������ʱ������������ֵ��T2��׽���ܿ�������
                       ��ʱ���жϺ���
*******************************************************************************/
#include ".\Timer\Timer.H"
#include ".\bluetooth\bluetoothCommunication.h"
#include "./qsport_arch/qsportArch.h"
#include ".\GPIO\GPIO.H"
#include "./vk1640/lamp_tape.h"

#pragma  NOAREGS
#ifdef  T2_CAP
//UINT16 Cap2[2] = {0};
//UINT16 Cap1[2] = {0};
//UINT16 Cap0[2] = {0};
#endif



/*******************************************************************************
* Function Name  : mTimer_x_ModInit(UINT8 x ,UINT8 mode)
* Description    : CH549��ʱ������xģʽ����
* Input          : UINT8 mode,Timerģʽѡ��
                   0��ģʽ0��13λ��ʱ����TLn�ĸ�3λ��Ч
                   1��ģʽ1��16λ��ʱ��
                   2��ģʽ2��8λ�Զ���װ��ʱ��
                   3��ģʽ3������8λ��ʱ��  Timer0
                   3��ģʽ3��Timer1ֹͣ
* Output         : None
* Return         : �ɹ�  SUCCESS
                   ʧ��  FAIL
*******************************************************************************/
UINT8 mTimer_x_ModInit(UINT8 x ,UINT8 mode)
{
    if(x == 0)
    {
        TMOD = TMOD & 0xf0 | mode;
    }
    else if(x == 1)
    {
        TMOD = TMOD & 0x0f | (mode<<4);
    }
    else if(x == 2)
    {
        RCLK = 0;    //16λ�Զ����ض�ʱ��
        TCLK = 0;
        CP_RL2 = 0;
    }
    else
    {
        return FAIL;
    }
    return SUCCESS;
}
/*******************************************************************************
* Function Name  : mTimer_x_SetData(UINT8 x,UINT16 dat)
* Description    : CH549Timer0 TH0��TL0��ֵ
* Input          : UINT16 dat;��ʱ����ֵ
* Output         : None
* Return         : None
*******************************************************************************/
void mTimer_x_SetData(UINT8 x,UINT16 dat)
{
    UINT16 tmp;
    tmp = 65536 - dat;
    if(x == 0)
    {
        TL0 = tmp & 0xff;
        TH0 = (tmp>>8) & 0xff;
    }
    else if(x == 1)
    {
        TL1 = tmp & 0xff;
        TH1 = (tmp>>8) & 0xff;
    }
    else if(x == 2)
    {
        RCAP2L = TL2 = tmp & 0xff;                                               //16λ�Զ����ض�ʱ��
        RCAP2H = TH2 = (tmp>>8) & 0xff;
    }
}
/*******************************************************************************
* Function Name  : CAP2Init(UINT8 mode)
* Description    : CH549��ʱ������2 T2EX���Ų�׽���ܳ�ʼ����CAP2 P11��
                   UINT8 mode,���ز�׽ģʽѡ��
                   0:T2ex���½��ص���һ���½���
                   1:T2ex�������֮��
                   3:T2ex�������ص���һ��������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAP2Init(UINT8 mode)
{
    RCLK = 0;
    TCLK = 0;
    C_T2  = 0;
    EXEN2 = 1;
    CP_RL2 = 1;                                                                //��T2ex�Ĳ�׽����
    T2MOD |= mode << 2;                                                        //���ز�׽ģʽѡ��
}
/*******************************************************************************
* Function Name  : CAP1Init(UINT8 mode)
* Description    : CH549��ʱ������2 T2���Ų�׽���ܳ�ʼ��T2(CAP1 P10)
                   UINT8 mode,���ز�׽ģʽѡ��
                   0:T2ex���½��ص���һ���½���
                   1:T2ex�������֮��
                   3:T2ex�������ص���һ��������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAP1Init(UINT8 mode)
{
    RCLK = 0;
    TCLK = 0;
    CP_RL2 = 1;
    C_T2 = 0;
    T2MOD = T2MOD & ~T2OE | (mode << 2) | bT2_CAP1_EN;                         //ʹ��T2���Ų�׽����,���ز�׽ģʽѡ��
}
/*******************************************************************************
* Function Name  : CAP0Init(UINT8 mode)
* Description    : CH549��ʱ������2 CAP0���Ų�׽���ܳ�ʼ��(CAP0 P36)
                   UINT8 mode,���ز�׽ģʽѡ��
                   0:T2ex���½��ص���һ���½���
                   1:T2ex�������֮��
                   3:T2ex�������ص���һ��������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAP0Init(UINT8 mode)
{
    RCLK = 0;
    TCLK = 0;
    CP_RL2 = 1;
    C_T2 = 0;
    T2MOD |= mode << 2;                                                        //���ز�׽ģʽѡ��
    T2CON2 = bT2_CAP0_EN;
}
#ifdef T0_INT
/*******************************************************************************
* Function Name  : mTimer0Interrupt()
* Description    : CH549��ʱ������0��ʱ�������жϴ����� 1ms�ж�
*******************************************************************************/
//sbit testPin = P2^0;
static UINT8 cap0IsBegin = 0;
static UINT32 cap0StartTimeCount = 0;
//static UINT8 cap0MeasureIsEnd = 0x01;
static UINT32 systemSecond = 0;
//static UINT8 mcuIsReporData = 0x00; 
static UINT32 lastSpeedMeasureResult = 0;
static UINT32 speedMeasureResult = 0;
static mcu_report_data_t mcuReportData = {0,0,0};
static power_compute_t powerComputeParam = {0,0};

extern UINT16 getBicyclePower(void);

#include ".\PWM\PWM.H"

code UINT16 indexWave[] = {
0, 9, 18, 27, 36, 45, 54, 63, 72, 81, 89, 98,
107, 116, 125, 133, 142, 151, 159, 168, 176,
184, 193, 201, 209, 218, 226, 234, 242, 249,
257, 265, 273, 280, 288, 295, 302, 310, 317,
324, 331, 337, 344, 351, 357, 364, 370, 376,
382, 388, 394, 399, 405, 410, 416, 421, 426,
431, 436, 440, 445, 449, 454, 458, 462, 465,
469, 473, 476, 479, 482, 485, 488, 491, 493,
496, 498, 500, 502, 503, 505, 506, 508, 509,
510, 510, 511, 512, 512, 512, 512, 512, 512,
511, 510, 510, 509, 508, 506, 505, 503, 502,
500, 498, 496, 493, 491, 488, 485, 482, 479,
476, 473, 469, 465, 462, 458, 454, 449, 445,
440, 436, 431, 426, 421, 416, 410, 405, 399,
394, 388, 382, 376, 370, 364, 357, 351, 344,
337, 331, 324, 317, 310, 302, 295, 288, 280,
273, 265, 257, 249, 242, 234, 226, 218, 209,
201, 193, 184, 176, 168, 159, 151, 142, 133,
125, 116, 107, 98, 89, 81, 72, 63, 54, 45, 36,
27, 18, 9, 0
};

sbit LEDR_test = P2^0;

bit  Flag = 0; 	//LED的呼吸方向标志位

unsigned int Counter = 0;					//计数值
unsigned int Compare = 0;				//比较值，占空比的大小
unsigned int Time_Counter = 0;		//延时作用	改变呼吸时长

void mTimer0Interrupt( void ) interrupt INT_NO_TMR0 using 1                    //timer0 Interrupt
{
//			static UINT16 breathTableIndex = 0;
//			static UINT8 toggle = 1;
//			if(breathTableIndex>180){
//				breathTableIndex = 0;
//			}
//			if(indexWave[breathTableIndex++]/2>=255){
//					SetPWM0Dat(255);
//			}else{
//					SetPWM0Dat(indexWave[breathTableIndex++]/2);
//			}
//			
//			
//			
//			if(toggle){
//					toggle = 1 - toggle;
//					ledcOpen();
//			}else{
//					toggle = 1 - toggle;
//					ledcClose();
//			}			
	
	
		
		Counter++;
		Counter%=100;		 //计数值周期为10ms		频率为100Hz
	
		Time_Counter++;
		Time_Counter%=100;		//起延时作用，一次呼吸时间为100us*100*100		2s
	
		if(Time_Counter==0)
		{
			if(Flag==0)
			{
				Compare++;
				if(Compare==100)	//此时占空比最大，开始调换方向
				{
					Flag=~Flag;
				}
			}
			else
			{
				Compare--;
				if(Compare==0)
				{
					Flag=~Flag;
				}
			}
		}
		
		if(Counter<Compare)
		{
			LEDR_test=0;
		}
		else 
		{
			LEDR_test=1;
		}	
	
	
	
//		static UINT16 isReachOneSecond = 0;
//		static UINT16 isReachMCUReporttime = 0;
//		static UINT16 isReachPowerComputeTime = 0;
//		static UINT8 toggle = 1;
//		if(1 == cap0IsBegin%3){//this mean falling edge is coming so i need to start count
//				cap0StartTimeCount++;//uint is 1ms
//		}
//		
//		isReachMCUReporttime++;
//		if(200 == isReachMCUReporttime){
//				//mcuIsReporData = 1;
//				isReachMCUReporttime = 0;
//				
//				mcuReportData.rotarySpeedMeasureData = speedMeasureResult;//RAW Data(this Data need to process)
//				mcuReportData.powerMeasureData = getBicyclePower();//temp use 0xaa
//				mcuReportData.forceMeasureData = getForceMeasureValue();//temp use 0x55
//				productEvent(UART_SEND_OP,(void *)&mcuReportData);
//				
//				
//				productEvent(ADC_SAMPLE,NULL);
//		}	

//		isReachPowerComputeTime++;
//		if(100 == isReachPowerComputeTime){
//				isReachPowerComputeTime = 0;
//			
//				powerComputeParam.rotarySpeedParamData = speedMeasureResult;
//				powerComputeParam.forceParamData = getForceMeasureValue();
//			
//				productEvent(COMPUTE_POWER_OP,(void *)&powerComputeParam);
//		}
//		
//		
//		isReachOneSecond++;	
//		if(1000 == isReachOneSecond){		
//				isReachOneSecond = 0;
//				systemSecond++;//1s
//			
//				productEvent(TEST_OP,(void *)&systemSecond);
//				productEvent(SYSTEM_IDLE_OP,(void *)&mcuReportData);
////				productEvent(LAMP_TAPE_OP,(void *)&speedMeasureResult);
//			
//				if(toggle){
//						toggle = 1 - toggle;
//						ledcOpen();
//				}else{
//						toggle = 1 - toggle;
//						ledcClose();
//				}
//				
//		}
		
		
    mTimer_x_SetData(0,TIMER0_RELOAD_VALUE);                                                 //1ms Interrupt
}

UINT32 getSystemSecond(void)
{
		return systemSecond;
}

#endif
#ifdef T1_INT
/*******************************************************************************
* Function Name  : mTimer1Interrupt()
* Description    : CH549��ʱ������1��ʱ�������жϴ����� 100us�ж�
*******************************************************************************/
void mTimer1Interrupt( void ) interrupt INT_NO_TMR1 using 2                //timer1�жϷ������,ʹ�üĴ�����2
{
    //��ʽ2ʱ��Timer1�Զ���װ
    static UINT16 tmr1 = 0;
    tmr1++;
    if(tmr1 == 2000)                                                       //100us*2000 = 200ms
    {
        tmr1 = 0;
        //SCK = ~SCK;
    }
}
#endif
/*******************************************************************************
* Function Name  : mTimer2Interrupt()
* Description    : CH549��ʱ������0��ʱ�������жϴ�����
*******************************************************************************/

#define 	MINIMIZE_MEASURE_TIME 20

#define ROTARY_LIGHTING_EFFECT_INTEERVAL_0 1
#define ROTARY_LIGHTING_EFFECT_INTEERVAL_1 40 
#define ROTARY_LIGHTING_EFFECT_INTEERVAL_2 60
#define ROTARY_LIGHTING_EFFECT_INTEERVAL_3 80
#define ROTARY_LIGHTING_EFFECT_INTEERVAL_4 100

/*
juage lastSpeedMeasureResult and current speedMeasureResult in the same range

m and n is domain of definition
m<a and b<n

*/
//static UINT8 isUpdatelampTampColor(void)
//{	
//		if(  ((lastSpeedMeasureResult<40 && lastSpeedMeasureResult>1) \
//			  &&(speedMeasureResult<40 && speedMeasureResult>1)) \
//			
//			|| ((lastSpeedMeasureResult<60 && lastSpeedMeasureResult>40) \
//			  &&(speedMeasureResult<60 && speedMeasureResult>40)) \	 
//		  
//			|| ((lastSpeedMeasureResult<80 && lastSpeedMeasureResult>60) \
//			  &&(speedMeasureResult<80 && speedMeasureResult>60)) \
//		  
//			|| ((lastSpeedMeasureResult<100 && lastSpeedMeasureResult>80) \
//			  &&(speedMeasureResult<100 && speedMeasureResult>80)) \
//		  
//			|| ((lastSpeedMeasureResult<1 && speedMeasureResult<1)) \

//		){
//				return 1;
//		}else{
//				return 0;
//		}
//}

#define QSPORT_VALUE_DIFF(a,b) (a>b)?(a-b):(b-a) 

// static UINT8 isUpdatelampTampColor(void)
// {	
// 	if(speedMeasureResult < 40){
// 			if(QSPORT_VALUE_DIFF(speedMeasureResult, lastSpeedMeasureResult) >= 40){
// 					return 1;
// 			}else{
// 					return 0;
// 			}			
// 	}else{
// 			//
// 	}
	
// 	if(QSPORT_VALUE_DIFF(speedMeasureResult, lastSpeedMeasureResult) >= 20){
// 			return 1;
// 	}else{
// 			return 0;
// 	}
// }

static UINT8 isUpdatelampTampColor(void)
{	
		if(((lastSpeedMeasureResult<=40 && lastSpeedMeasureResult>1) &&(speedMeasureResult<=40 && speedMeasureResult>1))){
				//printf("step1\n");
				return 0;
		}
		
		if(((lastSpeedMeasureResult<=60 && lastSpeedMeasureResult>40) &&(speedMeasureResult<=60 && speedMeasureResult>40))){
				//printf("step2\n");
				return 0;
		}

		if(((lastSpeedMeasureResult<=80 && lastSpeedMeasureResult>40) &&(speedMeasureResult<=80 && speedMeasureResult>40))){
				//printf("step3\n");
				return 0;
		}

		if(((lastSpeedMeasureResult<=100 && lastSpeedMeasureResult>80) &&(speedMeasureResult<100 && speedMeasureResult>80))){
				//printf("step4\n");
				return 0;
		}		
		
//		if((lastSpeedMeasureResult>100) && (speedMeasureResult>100)){
//				//printf("step5\n");
//				return 0;
//		}
		
		return 1;
}

void mTimer2Interrupt( void ) interrupt INT_NO_TMR2 using 3                //timer2�жϷ������,ʹ�üĴ�����3
{
		
#ifdef  T2_CAP		
    if(T2CON2&bT2_CAP0F)
    {
				cap0IsBegin += 1;
				if(2 == cap0IsBegin%3){	//rising edge mean capture is ended				
						//CH548L_LOG("diff %d \n",(UINT16)(cap0StartTimeCount));
						if(cap0StartTimeCount<MINIMIZE_MEASURE_TIME){						
								cap0IsBegin = 0;
								cap0StartTimeCount = 0;	
                                
								productEvent(DISPLAY_ROTARY_SPEED_OP,(void *)&speedMeasureResult);//send lasttime Data
								return;//Illegal measure(eg:didn't make half a circle)
						}
						lastSpeedMeasureResult = speedMeasureResult;//keep last Data
						speedMeasureResult = cap0StartTimeCount;//NEW Data
						cap0IsBegin = 0;	
						cap0StartTimeCount = 0;
						productEvent(DISPLAY_ROTARY_SPEED_OP,(void *)&speedMeasureResult);
						if(1 == isUpdatelampTampColor()){
								productEvent(LAMP_TAPE_OP,(void *)&speedMeasureResult);
						}
				}

        //Cap0[0] = T2CAP0;
        //Cap0[1] = Cap0[0];			
        T2CON2 &= ~bT2_CAP0F;
    }	
#endif

}

void rotarySpeedMeasureResultClear(void)
{
		speedMeasureResult = 0;
}


