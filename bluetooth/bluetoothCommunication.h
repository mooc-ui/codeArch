#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include ".\Public\CH549.H"


#define QSPORT_MAX_DATA_SIZE 32
/*communication Data structure*/
typedef struct
{
	UINT8 msgID;
	UINT8 validDataLen;//receive valid Data from bluetooth module(not include FRAME_HEADER/datalength/checksum )
	UINT16 reserve;
	UINT8 mcuData[QSPORT_MAX_DATA_SIZE];
}qsportdata_t;

typedef enum {
/*mcu send to BT module*/
//Data frame	
	MCU_SEND_DATA_FRAME_TO_BT_MODULE = 0x00,
	BT_MODULE_RESPONSE_DATA_FRAME_TO_MCU = 0x00,
//NonData frame
	MCU_SEND_NONDATA_FRAME_TO_BT_MODULE = 0x0A,
	BT_MODULE_RESPONSE_NONDATA_FRAME_TO_MCU = 0x0A,
	
	
/*BT module send to mcu*/
//Data frame	
	BT_MODULE_SEND_DATA_FRAME_TO_MCU = 0x80,
	MCU_RESPONSE_DATA_FRAME_TO_BT_MODULE = 0x80,
//NonData frame	
	BT_MODULE_SEND_NONDATA_FRAME_TO_MCU = 0x8A,
	MCU_RESPONSE_NONDATA_FRAME_TO_BT_MODULE = 0x8A,
	
}qcmd_t;

typedef enum{
/*down*/
		Q_GET = 0x00,
		Q_AURH = 0x03,
		Q_SET = 0X01,
		Q_NOTIFY = 0XE1,
	
/*up*/	
		Q_REPORT = 0X80,
		Q_IDSYNC = 0x02,
	
/*general*/	
		Q_ACK = 0xff,
	
}qmethod_t;

typedef struct mcu_report_data{
	UINT32 rotarySpeedMeasureData;
	UINT16 powerMeasureData;
	UINT8  forceMeasureData;
}mcu_report_data_t;

typedef struct power_compute{
	UINT32 rotarySpeedParamData;
	UINT8  forceParamData;
}power_compute_t;

void MCUSendDataToBluetooth(qsportdata_t *packet);
void bluetoothModuleInit(void);
void mcuReportSportData(mcu_report_data_t *reportData);

void testBt(void);

#endif