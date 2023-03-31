/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/09
* Description        : CH549 ����1~3 ��ѯ���ж��շ���ʾ��ʵ�����ݻػ�
                      ע�����DEBUG.C
*******************************************************************************/
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
#include ".\UART\UART.H"
#include ".\ADC\ADC.H"
#include ".\Timer\Timer.H"
#include ".\GPIO\GPIO.H"
#include ".\PWM\PWM.H"
#include ".\Timer\Timer.H"
#include ".\vk1640\vk1640_led.h"
#include "./qsport_arch/qsportArch.h"
#include ".\FlashRom\FlashRom.H"

#include "./vk1640/lamp_tape.h"

#pragma  NOAREGS

sbit LED_TEST = P1^3;

volatile UINT8 GetValueFlag = 0;   

char putchar (char c)  {
//  while (!(SIF1&bU1TI));
//  SIF1 |= 1;
//  return (SBUF1 = c);
}


unsigned char Float2Char(float value, unsigned char *array)
{
	UINT16 IntegerPart;
	float DecimalPart;
	UINT8 i = 0;
	UINT8 j = 0;
	char temp;
	//???????
	if(value >= 1) 
	{
		IntegerPart = (UINT16)value;
		DecimalPart = value - IntegerPart;
	}
	else 
	{
		IntegerPart = 0;
		DecimalPart = value - IntegerPart;
	}
	if(IntegerPart==0)
	{
		array[0]='0';
		array[1]='.';
		i=1;
	}
	else
	{
		while(IntegerPart>0)
		{
			array[i]=IntegerPart%10+'0';
			IntegerPart/=10;
			i++;
		}
		i--;
		//fix the result
		for(j=0;j<i;j++)
		{
			temp=array[j];
			array[j]=array[i-j];
			array[i-j]=temp;
		}
		i++;
		array[i] = '.';
	}
	//convert the Decimalpart
	i++;
	array[i++] = (UINT32)(DecimalPart * 10)%10 + '0'; //??????
	array[i++] = (UINT32)(DecimalPart * 100)%10 + '0';
	array[i++] = (UINT32)(DecimalPart * 1000)%10 + '0';
	array[i++] = (UINT32)(DecimalPart * 10000)%10 + '0';
	array[i++] = (UINT32)(DecimalPart * 100000)%10 + '0';	
	array[i++] = (UINT32)(DecimalPart * 1000000)%10 + '0';//??????	
	array[i]   = '\0';
	return i;
}



void adcInit(void)
{
		ADC_ExInit(3);                                                       //ADC��ʼ��,ѡ�����ʱ��
		//ADC_ExInit(0);                                                       //ADC��ʼ��,ѡ�����ʱ��
}

void startChannelSample(UINT8 channelNum)
{	
		ADC_ChSelect(channelNum);
		ADC_StartSample(); 
		while((ADC_CTRL&bADC_IF) == 0)
		{
				;    //��ѯ�ȴ���־��λ
		}
		ADC_CTRL = bADC_IF;                                                //���־
		//printf("channelNum%d=%d  ",(UINT16)channelNum,(UINT16)ADC_DAT);   
		//CH548L_LOG("channelNum%d=%d  ",(UINT16)channelNum,(UINT16)ADC_DAT);                    //���ADC����ֵ				 
}

void timer2Init(void)
{
    CH548L_LOG("T2 Test ...\n");
    mTimer2ClkFsys();                                                          //T2��ʱ��ʱ������  ����FREQ_SYS
    mTimer_x_ModInit(2,0);                                                     //T2 ��ʱ��ģʽ����
    mTimer_x_SetData(2,48000);                                                 //T2��ʱ����ֵ FREQ_SYS=24MHz,2ms�ж�
    mTimer2RunCTL(1);                                                          //T2��ʱ����
    ET2 = 1;                                                                   //T2��ʱ���жϿ���
    EA = 1;
}

void timer2CapModeInit(void)
{	
//    P2_MOD_OC &= ~PIN4;//(1<<PIN4);                                                     //CAP2 pull up
//    P2_DIR_PU |= PIN4;//(1<<PIN4);
//    P2_MOD_OC &= ~PIN5;//(1<<PIN5);                                                     //CAP1 pull up
//    P2_DIR_PU |= PIN5;//(1<<PIN5);	
	
    P3_MOD_OC &= ~(1<<PIN6);//cap0 Float input                                                    
    P3_DIR_PU &= ~(1<<PIN6);	
	
    //mTimer2ClkFsys();                                                    //T2��ʱ��ʱ������ FREQ_SYS/12
		mTimer2Clk12DivFsys();
    mTimer_x_SetData(2,0);                                                    //T2 ��ʱ��ģʽ���ò�׽ģʽ
	
		//PIN_FUNC |= 3;//pin alternate

    //CAP2Init(1);                                                              //T2 CAP2(P11)���ã������ز�׽
    //CAP1Init(1);                                                              //T2 CAP1(P10)���ã������ز�׽
		CAP0Init(0);
		
    mTimer2RunCTL(1);                                                         //T2��ʱ����
    ET2 = 1;                                                                  //T2��ʱ���жϿ���
    EA = 1;  	
}

void ch548lUART0Alter(void)
{
		PIN_FUNC |= (1<<4);//������0���õ�P02  P03
}

void pwm2Init(void)
{
    /* ʱ�� Ƶ������ */
    SetPWMClkDiv(4);                                                           //PWMʱ������,FREQ_SYS/4
    SetPWMCycle256Clk();                                                       //PWM���� FREQ_SYS/4/256
		SetPWM2Dat(3);//p23  23.58khz  -80.5%  ����Ч  ���۵͵�ƽռ�ձ� 20/256=7%
	    /* ��ͨ�� */
    PWM_SEL_CHANNEL(0x04, Enable);                  //p23 pwm2���ʹ��		
}

//UINT8 breathAdjust[20] = 

void pwm5Init(void)
{
    /* ʱ�� Ƶ������ */
    //SetPWMClkDiv(4);                                                           //PWMʱ������,FREQ_SYS/4
		SetPWMClkDiv(24000);                                                           //PWMʱ������,FREQ_SYS/4
    SetPWMCycle256Clk();                                                       //PWM���� FREQ_SYS/4/256
		SetPWM5Dat(0);//p23  23.58khz  -80.5%  ����Ч  ���۵͵�ƽռ�ձ� 20/256=7%
	    /* ��ͨ�� */
    //PWM_SEL_CHANNEL(0X01 | 0X02 | 0x04 | 0x08 | 0x10 | 0x20 | 0x40 | 0x80, Enable);                  //p23 pwm2���ʹ��		
		PWM_SEL_CHANNEL(0x20, Enable);
}

void pwm2DutyAdjust(UINT8 duty)
{
		if(duty>=20){
				duty = 20;
		}
		SetPWM2Dat(duty);
}

void timer0Init(void)
{
    mTimer0Clk12DivFsys();                                                     //T0 clock setting FREQ_SYS/12
    mTimer_x_ModInit(0,1);                                                     //T0 mode setting(mode1) 16bit timer
	
		/*note:if you want  modify timer0's Interrupt, you also need modify Interrupt callback function's reload value*/
    mTimer_x_SetData(0,TIMER0_RELOAD_VALUE);                                   //T0 timer value reload 24MHZ 10MS Interrupt
		mTimer0RunCTL(1);                                                          //T0 timer start
    ET0 = 1;                                                                   //T0 timer Interrupt open
    EA = 1;
}

void timer1Init(void)
{
    mTimer1Clk12DivFsys( );                                                    //T1 timer clock setting FREQ_SYS/12
    mTimer_x_ModInit(1,2);                                                     //T1 timer mode setting,mode2 8bit Auto reload
    //mTimer_x_SetData(1,0xC7C8);                                                //T1 timer give value TH1=TL1=256-200=38H;  100us Interrupt
    mTimer_x_SetData(1,0xC7C8);
		mTimer1RunCTL(1);                                                          //T1 timer start
    ET1 = 1;                                                                   //T1 timer Interrupt open
    EA = 1;
}

void uart0Init(void)
{
		SCON &= ~SM0;		////ѡ��8λ����ͨѶ
	
    SCON |= SMOD;                            //����ģʽ
    SCON |= REN;                             //ʹ�ܽ���
    SBAUD1 = 0 - FREQ_SYS/16/UART0_BUAD;         //����������
    TI = 0;                                //��շ�����ɱ�־	
}

void roatryControlPWM(void)
{
		UINT8 PWMLevelTemp = 0;
		UINT8 levelTemp = 0;
	
		levelTemp = RotarySwitchLevel();
		
		PWMLevelTemp = levelTemp/5;/*PWM level adjust range is 0-20. roatrySwitch level is 0-100.*/
		//printf("pwm duty %d",(UINT16)PWMLevelTemp);
		pwm2DutyAdjust(PWMLevelTemp);
}

void displayShowClear(UINT8 clearLinePositon)
{
#ifdef OPEN_NIXIE_TUBE_DISPLAY	
	 if(1 == clearLinePositon){//clear the first show line
			 ledDisplay(test_led_show[0],POSITION_ONE);
			 ledDisplay(test_led_show[0],POSITION_TWO);
			 ledDisplay(test_led_show[0],POSITION_THREE);
	 }
	 if(2 == clearLinePositon){//clear the second show line
			 ledDisplay(test_led_show[0],POSITION_FOUR);
			 ledDisplay(test_led_show[0],POSITION_FIVE);
			 ledDisplay(test_led_show[0],POSITION_SIX);
	 }
#endif	 
}

void InterruptPinInit(void)
{
		PIN_FUNC &= ~bINT0_PIN_X;
		GPIO_INT_Init( (INT_INT0_L|INT_INT1_L),INT_EDGE,Enable); 	
}



void main( )
{
		#define TEST_FLASH_READ_LENGTH 16
		UINT8 readFlashTest[TEST_FLASH_READ_LENGTH];
		UINT8 readFlashTestIsSuccess = 0;
		UINT8 testLampTapeUart1 = 2;
		UINT8 colorIndex = 0;
    CfgFsys( );                                                           
    mDelaymS(20);
	
		pwm5Init();
		PWMInterruptEnable();
		while(1){			
//			mDelaymS(1000);
//			ledRGBColor(1,0,0);
//			mDelaymS(1000);
//			ledRGBColor(0,0,0);			
		}//end	
	

//#if CLOSE_UART0_LOG0//(1 == CLOSE_UART0_LOG)
		/*note:if you want to close uart0's log function.you need annotation all printf function otherwise your Nixie tube will not show normally*/
    CH549UART1Init();
		CH549UART2Init();
		CH549UART3Init();
	
	/*lcd screen uart debug ok*/
//		colorIndex = 0xaa;
//		while(1){			
//			mDelaymS(1000);
//			ledRGBColor(1,1,1);
//			mDelaymS(1000);
//			ledRGBColor(0,0,0);			
//			//CH549UART1SendByte(0x55);
//			//CH549UART2SendByte(0x55);
//			//CH549UART3SendByte(0x55);
//		}//end

//		while(1){
//				for(colorIndex=1; colorIndex<6;colorIndex++){
//					CH549UART3SendByte(colorIndex);
//					mDelaymS(5000);
//				}

//		}
	
//		SCON1 |= (1<<1); 
//		printf("Q300 project start...\n");
//		printf("test batch burner...\n");
//#else 
		//ledDisplayInit();
//#endif	
	
//		readFlashTestIsSuccess = FlashReadBuf(0xF000, readFlashTest, 16);
//		printf("flag 0x%02x addr 0x%04x\n",(UINT16)readFlashTestIsSuccess,(UINT16)&readFlashTest);
//		printf("flash 0x%02x 0x%02x 0x%02x\n",(UINT16)readFlashTest[0], (UINT16)readFlashTest[1],(UINT16)readFlashTest[15]);
	
		//lampTapeInit();
		//testLampTapePin();	
	
	
		//adcInit();	
		ledInit();
	
		//timer0Init();//ms(system time source)
	
		//pwm2Init();
		
	
		timer2CapModeInit();
		
			
#if 1		//uart0 init
		mInitSTDIO(); 
		CH549UART0Alter();//PIN map to RX = p0.2  TX = p0.3 ble
#endif	

		bluetoothModuleInit();//need 1s
				
		InterruptPinInit();//this function if location So the front will appear error
		
//		lampTapeInit();
//		testLampTapePin();
		
		qsportArchInit();
   while(1)
   {	
			consumeEvent();
   }		
}


//void main( )
//{
//		showNumber_t num = NUMBER_ONE;
//		UINT8 testRegMode = 0XAA;
//		UINT8 testRegDir = 0;
//		UINT8 testReceiveData = 0;
//		UINT16 testCap0Value = 0;
//		UINT8 initBluetoothModuleCount = 10;
//    CfgFsys( );                                                           
//    mDelaymS(20);

////#if CLOSE_UART0_LOG0//(1 == CLOSE_UART0_LOG)
//		/*note:if you want to close uart0's log function.you need annotation all printf function otherwise your Nixie tube will not show normally*/
//    CH549UART1Init();                                                
//		SCON1 |= (1<<1); 
//		printf("Q300 project start...\n");
////#else 
//		//ledDisplayInit();
////#endif	

//	
//		adcInit();	
//		ledInit();
//	

//		timer0Init();//ms
//	
//		pwm2Init();
//		//pwm2DutyAdjust(10);//test pwm duty adjust
//	
//	
//		timer2CapModeInit();
//		
//			
//#if 1		//uart0 init
//		mInitSTDIO(); 
//		CH549UART0Alter();//PIN map to RX = p0.2  TX = p0.3 ble
//#endif	



//		bluetoothModuleInit();//need 1s

//		//printf("test bool 0x%02x \n",(UINT16)uart0ReceiveDataIsSuccess(testReceiveDataArray));
//		//printf("test cksum 0x%02x \n",(UINT16)((testRegMode+testRegMode)%256));
//		
//		//EA=1; 
//		ES = 1;//enable uart0 Interrupt
//			
//		InterruptPinInit();//this function if location So the front will appear error
//		
//    while(1)
//    {	
//			
//#if 0	//100MS timer sample/time
//				startChannelSample(0X03);//	 P1.3  5V_ADC
//				startChannelSample(0x02);//  p1.2 external adc channel  12v_ADC
//				startChannelSample(0x01);//  p1.1 external adc channel  VBUS_ADC=311v
//#endif
//	
//#if 1
//			if(isPrintLog)
//			{ 
//					isPrintLog = 0;
//					//printf("-- %02x\n",(UINT16)uart0ReceiveByteData());
//					MCUReceiveBluetoothData();
//			}
//#endif	


//			if(0x01 == judgeRotarySwitchIsWork()){
//					clearRotarySwitchIsWorkFlag();
//					rotaryLevelShow();
//					roatryControlPWM();
//			}
//			
//			if(0 == isPersonSport()){//if nobody sport the line2 led will show 000				
//					displayShowClear(2);
//					rotarySpeedMeasureResultClear();//what time clear it
//			}
//			if(0x01 == speedMeasureIsSuccess()){
//					speedMeasureIsSuccessFlagClear();
//					rotarySpeedShow();
//			}
//					
//			if(0x01 == mcuIsReachReportTime()){
//					mcuReportSportData();
//					mcuIsReachReportTimeClear();
//			}
//			
//				//LED_TEST = ~LED_TEST;
//				//mDelaymS(1000);				
//    }
//}

