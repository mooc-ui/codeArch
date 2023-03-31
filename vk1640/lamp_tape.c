#include "./vk1640/lamp_tape.h"
#include ".\GPIO\GPIO.H"
#include "intrins.h"

#include ".\UART\UART.H"

sbit LAMPTAPE = P2^5;

#define u8                  unsigned char

//#define LED_H 				LAMPTAPE=1
//#define LED_L 				LAMPTAPE=0
 
//send 24bit Data 
//msb format  RGB888 24bit description R7-R0-G7-G0-B7-B0
void send_RGB(u8 R, u8 G, u8 B)
{
	unsigned char i=0;
	EA = 0;//

	for(i=0;i<8;i++){
		if(G & 0x80/*0x80*/){
			LAMPTAPE = 1;
			LAMPTAPE = 1;//640
				
			LAMPTAPE = 0;	//360ns
			//_nop_();
		}else{
			LAMPTAPE = 0;
			LAMPTAPE = 0;
			
			LAMPTAPE = 1;	
			//_nop_();
		}
		G <<= 1;
	}	
	
	for(i=0;i<8;i++){
		if(R & 0x80/*0x80*/){/*send 1*/
			LAMPTAPE = 1;
			LAMPTAPE = 1;
			
			LAMPTAPE = 0;	//360ns
			//_nop_();
		}
		else{
			/*send 0*/
			LAMPTAPE = 0;
			LAMPTAPE = 0;
					
			LAMPTAPE = 1;
			//_nop_();
		}
		R <<= 1;
	}

	for(i=0;i<8;i++){
		if(B & 0x80/*0x80*/){
			LAMPTAPE = 1;
			LAMPTAPE = 1;
					
			LAMPTAPE = 0;	//360ns
		}
		else{
			LAMPTAPE = 0;
			LAMPTAPE = 0;		
			
			LAMPTAPE = 1;
		}
		B <<= 1;
	}
	
	EA = 1;//
}

//void test_send_RGB(u8 R, u8 G, u8 B)
//{
//	unsigned char i=0;
//	unsigned char j=0;
//	EA = 0;//

//	LAMPTAPE = 0;
//	mDelayuS(100);
//	for(j=0; j<LAMP_TAPE_NUMBER; j++){
//			for(i=0;i<8;i++){
//				if((G>>i) & 0x01){
//					LAMPTAPE = 1;
//					//LAMPTAPE = 1;
//						
//					LAMPTAPE = 0;	//360ns
//					//_nop_();
//				}else{
//					LAMPTAPE = 0;
//					//LAMPTAPE = 0;
//					
//					LAMPTAPE = 1;	
//					//_nop_();
//				}
//				//_nop_();
//				//G >>= 1;
//			}	
//			
//			for(i=0;i<8;i++){
//				if((R>>i) & 0x01){/*send 1*/
//					LAMPTAPE = 1;
//					//LAMPTAPE = 1;
//					
//					LAMPTAPE = 0;	//360ns
//					//_nop_();
//				}
//				else{
//					/*send 0*/
//					LAMPTAPE = 0;
//					//LAMPTAPE = 0;
//							
//					LAMPTAPE = 1;
//					//_nop_();
//				}
//				//_nop_();
//				//R >>= 1;
//			}

//			for(i=0;i<8;i++){
//				if((B>>i) & 0x01){
//					LAMPTAPE = 1;
//					//LAMPTAPE = 1;
//							
//					LAMPTAPE = 0;	//360ns
//				}
//				else{
//					LAMPTAPE = 0;
//					//LAMPTAPE = 0;		
//					
//					LAMPTAPE = 1;
//				}
//				//B >>= 1;
//			}	
//	}
//	
//	EA = 1;//
//}


void lampTapeColorSetting(UINT8 R_colr, UINT8 G_colr, UINT8 B_colr)
{
		UINT8 ledNum = 0;
	
		//mDelayuS(100);//RESET Code

		LAMPTAPE = 0;
		mDelayuS(3);
		for(ledNum=0; ledNum<LAMP_TAPE_NUMBER; ledNum++){
				send_RGB(R_colr,G_colr,B_colr);	
		}
		LAMPTAPE = 1;	
}

void lampTapeInit(void)
{
		//GPIO_Init( PORT2,PIN4,MODE1);
		GPIO_Init( PORT2,PIN5,MODE1);
		
		//LAMPTAPE = 1;	
	
		//lampTapeColorSetting(0xff, 0xff, 0xff);//close lamp tape
}

void testLampTapePin(void)
{
	UINT8 ledNum = 0;
	
	while(1){
			LAMPTAPE = 0;
			mDelayuS(3);
			for(ledNum=0; ledNum<LAMP_TAPE_NUMBER; ledNum++){
					//send_RGB(0,0,0);//white color
					send_RGB(0xff,0xff,0xff);//	
			}
			LAMPTAPE = 1;
			mDelaymS(500);
			
			//mDelayuS(100);//RESET Code
			
			LAMPTAPE = 0;
			mDelayuS(3);
			for(ledNum=0; ledNum<LAMP_TAPE_NUMBER; ledNum++){
					send_RGB(0,0xff,0xff);
					//send_RGB(0xff,0xff,0xff);//led off
					//send_RGB(0,0,0xff);//rainbow
					//send_RGB(0x0,0xff,0xff);//red
				
			}
			LAMPTAPE = 1;
			mDelaymS(500);


#if 0
	//test_send_RGB(0,0,0);
	//mDelaymS(500);
	test_send_RGB(0xff,0xff,0xff);
	mDelaymS(1000);
#endif

	}
}


void lampTapeColorControl(lighting_effect_t lightingEffect)
{
	//printf("effect %d \n",(UINT16)lightingEffect);
	switch(lightingEffect)
	{
		case SPORT_LIGHTING_EFFECT_OFF:
			//CH549UART1SendByte(SPORT_LIGHTING_EFFECT_OFF);
			CH549UART3SendByte(SPORT_LIGHTING_EFFECT_OFF);
			break;
		case SPORT_LIGHTING_EFFECT_WHITE:
			//CH549UART1SendByte(SPORT_LIGHTING_EFFECT_WHITE);
			CH549UART3SendByte(SPORT_LIGHTING_EFFECT_WHITE);
			break;
		case SPORT_LIGHTING_EFFECT_BLUE:
			//CH549UART1SendByte(SPORT_LIGHTING_EFFECT_BLUE);
			CH549UART3SendByte(SPORT_LIGHTING_EFFECT_BLUE);
			break;
		case SPORT_LIGHTING_EFFECT_GREEN:
			//CH549UART1SendByte(SPORT_LIGHTING_EFFECT_GREEN);
			CH549UART3SendByte(SPORT_LIGHTING_EFFECT_GREEN);
			break;
		case SPORT_LIGHTING_EFFECT_YELLOW:
			//CH549UART1SendByte(SPORT_LIGHTING_EFFECT_YELLOW);
			CH549UART3SendByte(SPORT_LIGHTING_EFFECT_YELLOW);
			break;
		case SPORT_LIGHTING_EFFECT_RED:
			//CH549UART1SendByte(SPORT_LIGHTING_EFFECT_RED);
			CH549UART3SendByte(SPORT_LIGHTING_EFFECT_RED);
			break;		
		
		default:
			break;
	}
}
