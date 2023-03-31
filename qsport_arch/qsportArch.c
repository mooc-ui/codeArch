#include "./qsport_arch/qsportArch.h"
#include ".\ADC\ADC.H"

#include ".\PWM\PWM.H"
#include ".\vk1640\vk1640_led.h"
#include ".\bluetooth\bluetoothCommunication.h"
#include "./vk1640/lamp_tape.h"

//#define OPEN_NIXIE_TUBE_DISPLAY 1

productInfo_t productInfo[MAX_OPERA_NUM] = {0,NULL};

static UINT16 bicyclePowerValue = 0x00;

extern void startChannelSample(UINT8 channelNum);
extern void pwm2DutyAdjust(UINT8 duty);
extern void clearForceMeasureValue(void);

#define RATE_MAX_VALUE 22
#define FORCE_MAX_VALUE 21

code UINT16 powerTable[RATE_MAX_VALUE][FORCE_MAX_VALUE] =   //POWER table
{
//	  0		5		10	15	20	25	30	35	40	45	50	55	60	65	70	75	80	85	90	95	100
    {	0,	0,	0,	0,	0,	0,	0,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	2,	3,	13,	30                             }, //5    0
    {	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	4,	5,	17,	38                             },//10    1
//    {	2,	2,	2,	2,	2,	3,	3,	3,	3,	4,	4,	4,	4,	5,	5,	6,	7,	8,	10,	26,	61                             },//15    2
    {	3,	4,	4,	4,	4,	5,	5,	5,	6,	6,	7,	7,	8,	9,	10,	11,	12,	14,	17,	44,	92                             },//20    3
//    {	5,	5,	5,	6,	6,	7,	7,	8,	9,	9,	10,	11,	12,	14,	15,	16,	18,	20,	25,	50,	132                            },//25    4
    {	6,	7,	8,	8,	9,	10,	10,	11,	12,	13,	14,	16,	17,	19,	21,	23,	26,	29,	35,	71,	215                            },//30    5
//    {	9,	10,	11,	12,	12,	13,	14,	15,	16,	18,	19,	20,	23,	24,	27,	29,	33,	38,	43,	89,	266                            },//35    6
    {	10,	13,	15,	16,	17,	18,	19,	21,	21,	23,	25,	27,	29,	32,	35,	39,	43,	49,	59,	112,	339                          },//40    7
    {	14,	16,	18,	18,	21,	23,	24,	24,	25,	28,	30,	32,	36,	39,	42,	48,	54,	59,	71,	132,	387                          },//45    8
    {	16,	20,	21,	23,	23,	25,	27,	29,	31,	34,	36,	39,	43,	47,	51,	56,	63,	72,	90,	178,	402                          },//50    9
    {	18,	22,	23,	24,	27,	28,	30,	33,	36,	37,	41,	46,	49,	56,	62,	68,	75,	89,	110,	209,	442                        },//55    10
    {	20,	24,	25,	26,	31,	33,	36,	39,	42,	46,	50,	54,	60,	66,	73,	79,	89,	100,	125,	239,	486                      }, //60   11
    {	22,	26,	27,	28,	31,	35,	36,	41,	45,	49,	55,	58,	64,	72,	80,	90,	100,	115,	140,	280,	527                    }, //65   12
    {	24,	28,	29,	32,	37,	40,	42,	45,	50,	55,	62,	67,	75,	82,	91,	99,	111,	130,	160,	300,	562                    }, //70   13
    {	28,	32,	35,	36,	41,	45,	48,	51,	56,	62,	68,	75,	84,	91,	102,	113,	125,	142,	179,	320,	591                }, //75   14
    {	32,	34,	37,	40,	45,	50,	53,	57,	63,	71,	79,	83,	91,	100,	112,	126,	138,	159,	198,	351,	630              }, //80   15
    {	34,	37,	40,	45,	51,	54,	59,	63,	71,	77,	83,	92,	99,	113,	125,	135,	148,	172,	224,	386,	669              }, //85   16
    {	37,	42,	44,	51,	54,	57,	63,	69,	76,	82,	91,	101,	109,	124,	136,	151,	165,	192,	268,	454,	700          }, //90   17
    {	41,	46,	48,	54,	62,	64,	69,	74,	82,	86,	102,	109,	116,	133,	143,	162,	179,	218,	308,	520,	751        }, //95   18
    {	43,	50,	53,	60,	64,	68,	75,	82,	90,	99,	108,	117,	131,	143,	160,	174,	196,	260,	352,	566,	780        }, //100  19
    {	54,	60,	66,	72,	77,	82,	90,	95,	105,	117,	129,	138,	150,	163,	181,	201,	225,	300,	423,	602,	860    }, //110  20
    {	57,	64,	68,	74,	83,	87,	99,	107,	116,	128,	139,	150,	167,	185,	203,	225,	250,	340,	489,	689,	940  }  //120  21
};

static UINT8 isStopSport = 0;
static UINT8 currentSportStatus = 0;
static UINT8 lastSportStatus = 0;

static void displayRotarySwitchLevel(void* pText)
{
		UINT8 showNum = 0;
		UINT8 levelTemp = 0;
		UINT8 PWMLevelTemp = 0;
		levelTemp = *((UINT8*)pText);
		
		/*control pwm*/
		PWMLevelTemp = levelTemp/5;/*PWM level adjust range is 0-20. roatrySwitch level is 0-100.*/
		//printf("level %d pwm duty %d",(UINT16)levelTemp,(UINT16)PWMLevelTemp);
		pwm2DutyAdjust(PWMLevelTemp);

#ifdef OPEN_NIXIE_TUBE_DISPLAY
		/*show*/
		showNum = levelTemp/100%10;
		ledDisplay(test_led_show[showNum],POSITION_ONE);
	
		showNum = levelTemp/10%10;
		ledDisplay(test_led_show[showNum],POSITION_TWO);
	
		showNum = levelTemp/1%10;
		ledDisplay(test_led_show[showNum],POSITION_THREE);
#endif	

}

static MCUReceiveBluetoothDataProcess(void* pText)
{
		MCUReceiveBluetoothData();
}

static MCUSendToBluetoothDataProcess(void* pText)
{
		mcu_report_data_t *reportDataTemp = NULL;
		reportDataTemp = (mcu_report_data_t *)pText;
		if(0 == reportDataTemp->rotarySpeedMeasureData){
				reportDataTemp->rotarySpeedMeasureData = 0;
		}else{
				reportDataTemp->rotarySpeedMeasureData = 1000*60.0f/((reportDataTemp->rotarySpeedMeasureData)*1.0f*2);//Data process
		}
		
		//printf("Rreport %d %d %d \n",(UINT16)reportDataTemp->rotarySpeedMeasureData,(UINT16)reportDataTemp->powerMeasureData,(UINT16)reportDataTemp->forceMeasureData);
	
		mcuReportSportData(reportDataTemp);
}

static void displayRotarySpeed(void* pText)
{
		UINT8 x;
		UINT8X dexdata[20];
		UINT8X len;
	
		UINT8 showSpeedNum = 0;
		UINT32 speedTemp = 0;
		UINT32 speedResultRaw = *((UINT32*)pText);
		//speedTemp = ((float)rotarySpeedMeasureResult())*1.0f*2/60.0f;//uint=min/R
	
		if(speedResultRaw<=0){//DIVIDE zero error
				return;
		}

		//speedTemp = 1000*60.0f/speedResultRaw*1.0f * 2;//uint=R/min
		speedTemp = 1000*60.0f/(speedResultRaw*1.0f*2);//uint=R/min
		
		//printf("RAW %d speed %d\n",(UINT16)speedResultRaw,(UINT16)speedTemp);
		//CH548L_LOG("speed %d\n",(UINT16)speedTemp);
	
	
//		len = Float2Char(speedTemp, dexdata);         
//		printf("speed: "); 		//print Float
//		for( x=0;x<len;x++){
//			SBUF1 = dexdata[x];
//			mDelaymS(5);
//		}	
//		printf("\n");
	
#ifdef OPEN_NIXIE_TUBE_DISPLAY
		
		showSpeedNum = (UINT32)speedTemp/100%10;
		ledDisplay(test_led_show[showSpeedNum],POSITION_FOUR);
	
		showSpeedNum = (UINT32)speedTemp/10%10;
		ledDisplay(test_led_show[showSpeedNum],POSITION_FIVE);
	
		showSpeedNum = (UINT32)speedTemp/1%10;
		ledDisplay(test_led_show[showSpeedNum],POSITION_SIX);	
#endif

}

static void testProcess(void* pText)
{
		static UINT8 toggle = 1;
		UINT32 currentTime = *((UINT32*)pText);
		
	 if(toggle){
		toggle = 1 - toggle;
		//printf("toggle = %d\n",(UINT16)toggle);
		//lampTapeColorSetting(0x33, 0x44, 0x55);
		 ledRGBColor(1,0,0);
	 }else{
		toggle = 1 - toggle;
		//printf("toggle = %d\n",(UINT16)toggle);
		//lampTapeColorSetting(0xff, 0xff, 0xff);
		 ledRGBColor(0,0,0);
	 }

		//lampTapeColorControl(2);
		//printf("----test queue %d\n",(UINT16)currentTime);	
}

static void lightingEffectControl(void* pText)
{
		UINT32 rotarySpeedControlColor;
		lighting_effect_t lightingEffect = SPORT_LIGHTING_EFFECT_OFF;
	
		rotarySpeedControlColor = *(UINT32 *)pText;	
		//printf("color %d \n",(UINT16)rotarySpeedControlColor);
		if(rotarySpeedControlColor < 1){
				lightingEffect = SPORT_LIGHTING_EFFECT_OFF;
		}else if(rotarySpeedControlColor < 40){
				lightingEffect = SPORT_LIGHTING_EFFECT_WHITE;
		}else if(rotarySpeedControlColor < 60){
				lightingEffect = SPORT_LIGHTING_EFFECT_BLUE;
		}else if(rotarySpeedControlColor < 80){
				lightingEffect = SPORT_LIGHTING_EFFECT_GREEN;
		}else if(rotarySpeedControlColor < 100){
				lightingEffect = SPORT_LIGHTING_EFFECT_YELLOW;
		}else{
				lightingEffect = SPORT_LIGHTING_EFFECT_RED;
		}
		//printf("effect %d \n",(UINT16)lightingEffect);
		lampTapeColorControl(lightingEffect);
}

static void lightingEffectControlTest(void* pText)
{
		static UINT32 lightingEffectspeedTemp = 0;
		UINT32 speedResultRaw = *((UINT32*)pText);
		//speedTemp = ((float)rotarySpeedMeasureResult())*1.0f*2/60.0f;//uint=min/R
	
		if(speedResultRaw<=0){
				lightingEffectspeedTemp = 0;
		}else{
				lightingEffectspeedTemp = 1000*60.0f/(speedResultRaw*1.0f*2);//uint=R/min	
		}
		
		//printf("----test queue %d\n",(UINT16)lightingEffectspeedTemp);		
		lightingEffectControl(&lightingEffectspeedTemp);	
}

static void idleLightingEffectProcess(void* pText)
{
		lighting_effect_t lightingEffect = *((lighting_effect_t *)pText);
		//printf("effect exe %d\n",(UINT16)(*((lighting_effect_t *)pText)));
		lampTapeColorControl(lightingEffect);
		//printf("effect exe\n");
}

#define RETAIN_HISTORY_VALUE_MAX 5
//static UINT32 retainHistoryValue[RETAIN_HISTORY_VALUE_MAX] = {0x00,0xff,0x00,0xff,0x00};
static UINT32 retainHistoryValue[RETAIN_HISTORY_VALUE_MAX] = {0x00,0,0x00,0,0x00};
extern void displayShowClear(UINT8 clearLinePositon);

static void systemIdleProcess(void* pText)
{
		static UINT32 cuurentTime = 0;
		static UINT32 stopSportTime = 0;
		static UINT8  retainHistoryValueIndex = 0;
		static UINT32 reachEnterStandbyModeTime = 0;
		UINT32 retainHistoryAverageValue = 0;	
		UINT8 isEqualIndex = 0;
		mcu_report_data_t sysGetReportDataTemp;
		static UINT8 isFirstUpdateLightEffect = 1;
		static lighting_effect_t stopSportUpdateLightingEffect = SPORT_LIGHTING_EFFECT_OFF;
	
		//sysGetReportDataTemp = (mcu_report_data_t *)pText;
		memcpy(&sysGetReportDataTemp, (mcu_report_data_t *)pText, sizeof(mcu_report_data_t));
			
	

		if(0 == sysGetReportDataTemp.rotarySpeedMeasureData){
				sysGetReportDataTemp.rotarySpeedMeasureData = 0;  
		}else{
				sysGetReportDataTemp.rotarySpeedMeasureData = 1000*60.0f/((sysGetReportDataTemp.rotarySpeedMeasureData)*1.0f*2);//Data process
		}

		if((getSystemSecond() - cuurentTime)>=1){//every x second save a Data
				cuurentTime = getSystemSecond();
				
				retainHistoryValue[retainHistoryValueIndex%RETAIN_HISTORY_VALUE_MAX] = sysGetReportDataTemp.rotarySpeedMeasureData;
				retainHistoryValueIndex++;
		}	
		
		
		if(retainHistoryValueIndex>=5){retainHistoryValueIndex = 0;}//in case array overflow

		retainHistoryAverageValue = 0;
		for(isEqualIndex=0; isEqualIndex<RETAIN_HISTORY_VALUE_MAX; isEqualIndex++){
				retainHistoryAverageValue += retainHistoryValue[isEqualIndex];
				//printf("store val %d \n",(UINT16)(retainHistoryValue[isEqualIndex]));
		}
		if(RETAIN_HISTORY_VALUE_MAX == isEqualIndex){
				retainHistoryAverageValue /= RETAIN_HISTORY_VALUE_MAX;
				//printf("ave %d ",(UINT16)retainHistoryAverageValue);
		}

		if(retainHistoryAverageValue == retainHistoryValue[0] \
			 && retainHistoryAverageValue == retainHistoryValue[1] \
		   && retainHistoryAverageValue == retainHistoryValue[2] \
			 && retainHistoryAverageValue == retainHistoryValue[3] \
		   && retainHistoryAverageValue == retainHistoryValue[4] \
			){// all array member are equal			
				
				displayShowClear(2);//clear second screen
				
				
				rotarySpeedMeasureResultClear();//if nobody need to clear mcu report Data(rotarySpeedMeasureData)
				bicyclePowerValue = 0x00;
				
				//clearForceMeasureValue();
				//displayShowClear(1);//clear first screen
				
				lastSportStatus = currentSportStatus;
				currentSportStatus = 0;//no sport
				
				isStopSport = 1;
				//stopSportTime = getSystemSecond();
		
				retainHistoryValue[0] = 0x00;
				retainHistoryValue[1] = 0;//0xff;
				retainHistoryValue[2] = 0x00;
				retainHistoryValue[3] = 0;//0xff;
				retainHistoryValue[4] = 0x00;//Restore initialization	
				//printf("nobody\n");
		}else{
				lastSportStatus = currentSportStatus;
				currentSportStatus = 1;//sporting
			
				isStopSport = 0;
				isFirstUpdateLightEffect = 1;//in case update lighting effect when person is sporting
				//printf("sporting\n");
		}	
		
		if(currentSportStatus != lastSportStatus){
				if(1 == currentSportStatus){//start sport
						//
				}else{
						//
				}
				
				if(0 == currentSportStatus){//stop sport
						stopSportUpdateLightingEffect = SPORT_LIGHTING_EFFECT_BLUE;
						productEvent(IDLE_LIGHTING_EFFECT_OP,(void *)&stopSportUpdateLightingEffect);
						stopSportTime = getSystemSecond();//start count(after 2min should close breath lamp tamp)
				}else{
						//
				}				
		}
		//printf("cr %d ls %d t %d\n",(UINT16)currentSportStatus, (UINT16)lastSportStatus,(UINT16)stopSportTime);
		if((0 == currentSportStatus) && (0 == lastSportStatus) && ((getSystemSecond()-stopSportTime)>=120)){
						stopSportTime = getSystemSecond();
						stopSportUpdateLightingEffect = SPORT_LIGHTING_EFFECT_OFF;
						productEvent(IDLE_LIGHTING_EFFECT_OP,(void *)&stopSportUpdateLightingEffect);
		}
				
}

static void ADCSamplePreocess(void* pText)
{
		startChannelSample(0X03);//	 P1.3  5V_ADC
		startChannelSample(0x02);//  p1.2 external adc channel  12v_ADC
		startChannelSample(0x01);//  p1.1 external adc channel  VBUS_ADC=311v
}

static void computeBicyclePower(void* pText)
{
		UINT8 forceIndex=0;
		UINT8 rotarySpeedIndex=0;
	
		power_compute_t computeBicyclePowerDataTemp;
	
		//sysGetReportDataTemp = (mcu_report_data_t *)pText;
		memcpy(&computeBicyclePowerDataTemp, (power_compute_t *)pText, sizeof(power_compute_t));	
	
		if(0 == computeBicyclePowerDataTemp.rotarySpeedParamData){
				rotarySpeedIndex = 0;
		}else{
				rotarySpeedIndex = 1000*60.0f/((computeBicyclePowerDataTemp.rotarySpeedParamData)*1.0f*2);//Data process (uint:R/min)
		}	
	
		//printf("i %d j %d",(UINT16)rotarySpeedIndex,(UINT16)forceIndex);
	
		
		rotarySpeedIndex = rotarySpeedIndex/5;//index powerTable
		forceIndex = computeBicyclePowerDataTemp.forceParamData/5+1;//index powerTable
	
		if(rotarySpeedIndex >= RATE_MAX_VALUE){
				rotarySpeedIndex = RATE_MAX_VALUE-1;
		}		
	
		if(forceIndex >= FORCE_MAX_VALUE){
				forceIndex = FORCE_MAX_VALUE-1;
		}	
	
		//printf("i %d j %d",(UINT16)rotarySpeedIndex,(UINT16)forceIndex);
		bicyclePowerValue = powerTable[rotarySpeedIndex][forceIndex];
}

UINT16 getBicyclePower(void)
{
		return bicyclePowerValue;
}

void qsportArchInit(void)
{
		UINT8 initproductInfoIndex = 0;
	
		for(initproductInfoIndex=0; initproductInfoIndex<MAX_OPERA_NUM; initproductInfoIndex++){
				productInfo[initproductInfoIndex].loopId = 0;
				productInfo[initproductInfoIndex].paramMember = NULL;
		}
}

void productEvent(loopId_t lpid, void * param)
{
	if(lpid <= NONE_OP || lpid >= MAX_OPERA_NUM){
			//#error "please input correct param"
			return;
	}
	productInfo[lpid].loopId = 1;
	productInfo[lpid].paramMember = param;
}
	
void consumeEvent(void)
{
	UINT8 productInfoIndex = 0;
	for(productInfoIndex=0; productInfoIndex<MAX_OPERA_NUM; productInfoIndex++){
			if(1 == productInfo[productInfoIndex].loopId){
						switch(productInfoIndex)
						{
								case UART_SEND_OP:
										MCUSendToBluetoothDataProcess(productInfo[productInfoIndex].paramMember);
								break;
							
								case UART_RECEIVE_OP:
										MCUReceiveBluetoothDataProcess(productInfo[productInfoIndex].paramMember);
								break;
								
								case DISPLAY_ROTARY_SPEED_OP:
										displayRotarySpeed(productInfo[productInfoIndex].paramMember);
								break;
								
								case ADC_SAMPLE:
										ADCSamplePreocess(NULL);				
									break;
								
								case DISPLAY_ROTARYSWITCH_LEVEL_OP:
										displayRotarySwitchLevel(productInfo[productInfoIndex].paramMember);//
								break;
								
								case SYSTEM_IDLE_OP:
										systemIdleProcess(productInfo[productInfoIndex].paramMember);
									break;
								case COMPUTE_POWER_OP:
										computeBicyclePower(productInfo[productInfoIndex].paramMember);
									break;
								
								case TEST_OP:
										testProcess(productInfo[productInfoIndex].paramMember);								
									break;
								
								case LAMP_TAPE_OP:
										lightingEffectControlTest(productInfo[productInfoIndex].paramMember);
								
								
										//lightingEffectControl(productInfo[productInfoIndex].paramMember);
									break;
								case IDLE_LIGHTING_EFFECT_OP:
										idleLightingEffectProcess(productInfo[productInfoIndex].paramMember);
									break;
							
								default:			
								break;			
						}
					productInfo[productInfoIndex].loopId = 0;//clear
					productInfo[productInfoIndex].paramMember = NULL;//clear							
			}
	}
}