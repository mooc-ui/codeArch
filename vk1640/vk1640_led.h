#ifndef  __VK1640_LED_H__
#define  __VK1640_LED_H__

#include "CH549.h"

/*Data cmd setting*/
#define DATA_READ_WRITE_MODE 0x40
#define AIUTO_ADDRESS_INCREASE 0x40 
#define FIX_ADDRESS 0x44 

typedef enum {
	LED_OFF = 0X00,
	
//{0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x3f};	
	NUMBER_ONE = 0x06,
	NUMBER_TWO = 0x5b,
	NUMBER_THREE = 0x4f,
	NUMBER_FOUR = 0x66,
	NUMBER_FIVE = 0x6d,
	NUMBER_SIX = 0x7d,
	NUMBER_SEVEN = 0x07,
	NUMBER_EIGHT = 0x7f,
	NUMBER_NINE = 0x6f,
	NUMBER_ZERO = 0x3f,

//{0x86,0xdb,0xcf,0xe6,0xed,0xfd,0x87,0xff,0xef,0xbf};	
	NUMBER_ONE_POINT = 0x86,
	NUMBER_TWO_POINT = 0xdb,
	NUMBER_THREE_POINT = 0xcf,
	NUMBER_FOUR_POINT = 0xe6,
	NUMBER_FIVE_POINT = 0xed,
	NUMBER_SIX_POINT = 0xfd,
	NUMBER_SEVEN_POINT = 0x87,
	NUMBER_EIGHT_POINT = 0xff,
	NUMBER_NINE_POINT = 0xef,
	NUMBER_ZERO_POINT = 0xbf,
	
}showNumber_t;

typedef enum {
	POSITION_ONE = 0xC0,
	POSITION_TWO = 0XC1,
	POSITION_THREE = 0XC2,
	POSITION_FOUR = 0XC3,
	POSITION_FIVE = 0XC4,
	POSITION_SIX = 0XC5,
	
	POSITION_MAX = 0XFF,
}showPosition_t;

extern const UINT8 test_led_show[10];


void testNixieTube(void);
//void vk1640PinInit(void);
//void vk1640ClearAll(void);
void ledDisplay(showNumber_t num, showPosition_t showPosition);
void ledDisplayInit(void);


/*LED*/
void ledInit(void);
void ledcOpen(void);
void ledcClose(void);
void ledRGBColor(UINT8 r, UINT8 g, UINT8 b);

#endif
