#ifndef  __QSPORT_ARCH_H__
#define  __QSPORT_ARCH_H__

#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"

#define OPEN_NIXIE_TUBE_DISPLAY	

typedef enum
{
	NONE_OP	= 0,
	IDLE_LIGHTING_EFFECT_OP,
	SYSTEM_IDLE_OP,
	UART_SEND_OP,
	UART_RECEIVE_OP,
	LAMP_TAPE_OP,
	DISPLAY_ROTARY_SPEED_OP,	
	DISPLAY_ROTARYSWITCH_LEVEL_OP,
	ADC_SAMPLE,
	COMPUTE_POWER_OP,
		
	TEST_OP,
		
	MAX_OPERA_NUM,
}loopId_t;

typedef struct {
	UINT8 loopId;//bool
	void *paramMember;
}productInfo_t, *productInfoHandle_t;

extern productInfo_t productInfo[MAX_OPERA_NUM];

void qsportArchInit(void);
void productEvent(loopId_t lpid, void * param);
void consumeEvent(void);

#endif