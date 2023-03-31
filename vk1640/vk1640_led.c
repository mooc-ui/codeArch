#include ".\vk1640\vk1640_led.h"
#include "DEBUG.h"
#include ".\GPIO\GPIO.H"

const UINT8 test_led_show[10] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f}; //0-9
//const UINT8 test_led_show[10] = {0xbf,0x86,0xdb,0xcf,0xe6,0xed,0xfd,0x87,0xff,0xef};//0. - 9.


//sbit LED_TEST = P1^3;
sbit LEDC = P4^0;
sbit LEDR = P2^0;
sbit LEDG = P2^1;
sbit LEDB = P2^2;

sbit VK1640_SCK = P2^6;//P0^6;//modify
sbit VK1640_SDA = P2^7;//P0^7;//modify

static void delay()
{
		mDelayuS(5);
}

void vk1640PinInit(void)
{
	GPIO_Init( PORT2,PIN6,MODE1);//modify 
	GPIO_Init( PORT2,PIN7,MODE1);//modify	
	
//	GPIO_Init( PORT0,PIN6,MODE1);//modify 
//	GPIO_Init( PORT0,PIN7,MODE1);//modify
}

static void start_1640()
{
    VK1640_SCK=0;
    delay();
    VK1640_SDA=1;
    delay();
    VK1640_SCK=1;
    delay();   
    VK1640_SDA=0;
    delay();
    VK1640_SCK=0;
    delay();
}

static void stop_1640()
{
    VK1640_SCK=0;
    delay();
    VK1640_SDA=0;
    delay();
    VK1640_SCK=1;
    delay();    
    VK1640_SDA=1;
    delay(); 
    VK1640_SCK=0;    
}

static void send_data_1640(unsigned char x)
{
    unsigned char i;
    for(i=0;i<8;i++)
    {
        VK1640_SCK=0;
        delay();
        if(x&0X01)VK1640_SDA=1;
        else VK1640_SDA=0;
        delay();
        VK1640_SCK=1;
        delay();
        x>>=1;
    }
    VK1640_SCK=0;
}

void vk1640ClearAll(void)
{
    unsigned char i;
    start_1640();
    send_data_1640(0X40);//地址自动+1
    stop_1640();
    
    start_1640();
    send_data_1640(0XC0);//从00地址开始
    for(i=0;i<16;i++)  
    {
        send_data_1640(0x00);
    }
    stop_1640();

    start_1640();
    send_data_1640(0X8A);//灰度显示
    stop_1640();    
}


void ledDisplay(showNumber_t num, showPosition_t showPosition)
{
		//vk1640ClearAll();//clear ram
	
    start_1640();
    send_data_1640(FIX_ADDRESS);//固定地址
    stop_1640();
	
    start_1640();
		send_data_1640(showPosition);//show address
		send_data_1640(num);//show number	
    stop_1640();		
	
    start_1640();
    send_data_1640(0X8f);//open show
    stop_1640(); 		
}

void ledDisplayInit(void)
{
		//UINT8 powerOnAnimation = 0;
		UINT8 testPosition=0XC0;
		UINT8 testNumber=0;
		vk1640PinInit();
		vk1640ClearAll();//close all leds
	
		for(testPosition=0XC0; testPosition<0XC6; testPosition++){
				ledDisplay(NUMBER_ZERO, testPosition);	
				//mDelaymS(1000);	
		}
}

void ledInit(void)
{
	GPIO_Init(PORT4,PIN0,MODE1); 
	GPIO_Init(PORT2,PIN0,MODE1);
	GPIO_Init(PORT2,PIN1,MODE1);
	GPIO_Init(PORT2,PIN2,MODE1);
	
	LEDC = 0;
}

void ledcOpen(void)
{
		LEDC = 1;
}

void ledcClose(void)
{
		LEDC = 0;
}

void ledRGBColor(UINT8 r, UINT8 g, UINT8 b)
{
		LEDR = r;
		LEDG = g;
		LEDB = b;
}

void testNixieTube(void)
{
#if 0	
	UINT8 testPosition=0XC0;
	UINT8 testNumber=0;
	vk1640PinInit();
	for(testPosition=0XC0; testPosition<0XC6; testPosition++){
		for(testNumber=0; testNumber<10; testNumber++){
				ledDisplay(test_led_show[testNumber], testPosition);
				mDelaymS(1000);	
		}
	}
#endif
	
#if 1
	UINT8 testCount = 0;
	vk1640PinInit();
	vk1640ClearAll();//close all leds
	for(testCount=0; testCount<10; testCount++){
			ledDisplay(NUMBER_ONE, POSITION_ONE);
			ledDisplay(NUMBER_TWO, POSITION_TWO);
			ledDisplay(NUMBER_THREE, POSITION_THREE);
		
			ledDisplay(NUMBER_ONE_POINT, POSITION_FOUR);
			ledDisplay(NUMBER_TWO_POINT, POSITION_FIVE);
			ledDisplay(NUMBER_THREE_POINT, POSITION_SIX);		
			mDelaymS(1000);	
			ledDisplay(LED_OFF, POSITION_ONE);
			ledDisplay(LED_OFF, POSITION_TWO);	
			ledDisplay(LED_OFF, POSITION_THREE);
		
			ledDisplay(LED_OFF, POSITION_FOUR);
			ledDisplay(LED_OFF, POSITION_FIVE);	
			ledDisplay(LED_OFF, POSITION_SIX);	//led_display	
			mDelaymS(1000);	
	}	
#endif		
}


