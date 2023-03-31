#ifndef  __LAMP_TAPE_H__
#define  __LAMP_TAPE_H__

#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"

#define LAMP_TAPE_NUMBER 15


//#define SPORT_LIGHTING_EFFECT_OFF 0
//#define SPORT_LIGHTING_EFFECT_WHITE 1    //R+G+B
//#define SPORT_LIGHTING_EFFECT_BLUE 2    //B
//#define SPORT_LIGHTING_EFFECT_GREEN 3   //G
//#define SPORT_LIGHTING_EFFECT_YELLOW 4   //G+R
//#define SPORT_LIGHTING_EFFECT_RED 5    //R

typedef enum{
		SPORT_LIGHTING_EFFECT_OFF = 0,
		SPORT_LIGHTING_EFFECT_WHITE = 1,
		SPORT_LIGHTING_EFFECT_BLUE = 2,
		SPORT_LIGHTING_EFFECT_GREEN = 3,
		SPORT_LIGHTING_EFFECT_YELLOW = 4,
		SPORT_LIGHTING_EFFECT_RED = 5,
	
		SPORT_LIGHTING_EFFECT_MAX,
}lighting_effect_t;


void lampTapeInit(void);
void lampTapeColorSetting(UINT8 R_colr, UINT8 G_colr, UINT8 B_colr);
void testLampTapePin(void);

void lampTapeColorControl(lighting_effect_t lightingEffect);

#endif