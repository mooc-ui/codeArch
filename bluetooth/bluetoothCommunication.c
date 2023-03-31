#include ".\bluetooth\bluetoothCommunication.h"
#include ".\UART\UART.H"
#include ".\vk1640\vk1640_led.h"
#include ".\FlashRom\FlashRom.H"

#define MCU_MAX_SEND_DATA_LEN 32
#define FRAME_HEADER 0xAA
#define FIX_DATA_LEN 4

/*ASCII X=0x58 Q=0x51 0=0x30 2=0x32 5=0x35 7=0x37*/
#define DEVICE_INFORMATION_LENGTH 16
char deviceInformation[DEVICE_INFORMATION_LENGTH] = {0xAA, 0x11, 0x0A, 0x02, 0x58, 0x51, 0x30, 0x32, 0x30, 0x33,
                                        0x30, 0x30, 0x35, 0x30,
                                        0x37, 0x31
                                       };
																			 
enum
{
	waitForStartByte1,
	waitForDataLength,
	waitForMsgID,
	waitForData,
	waitForChksum1,
}qSportRxState;																			 

static UINT8 bluetoothNetState = 0x00;
static UINT8 isInquireBluetoothState = 1;
static qsportdata_t rxPacket;
static qsportdata_t txPacket;

UINT8 bluetoothNetConnectIsSuccess(void)
{
		return bluetoothNetState;
}

static void bluetoothDataPreocess(void)
{
		/*bt net connect status*/		
		if(BT_MODULE_RESPONSE_NONDATA_FRAME_TO_MCU == rxPacket.msgID && Q_ACK == rxPacket.mcuData[0] && 2 == rxPacket.validDataLen){			
				if(0 == rxPacket.mcuData[1]){
						//CH548L_LOG("state 0x%02x",(UINT16)bluetoothNetState);
						bluetoothNetState = 1;//net connect success
				}else{
						//CH548L_LOG("state 0x%02x",(UINT16)bluetoothNetState);
						bluetoothNetState = 0;
				}
		}
}

void MCUReceiveBluetoothData(void)
{
		UINT8 uart0ReceiveData= 0;
		static UINT8 cksum = 0;
		static UINT8 dataIndex = 0;
		static UINT8 rxState = waitForStartByte1;
		
		uart0ReceiveData = (UINT8)uart0ReceiveByteData();
		//printf("rx %d %02x %02x\n",(UINT16)rxState,(UINT16)uart0ReceiveData,(UINT16)cksum);
		CH548L_LOG("rx %d %02x %02x\n",(UINT16)rxState,(UINT16)uart0ReceiveData,(UINT16)cksum);
		switch(rxState)
		{
			case waitForStartByte1:
					if(FRAME_HEADER == uart0ReceiveData){
							cksum += uart0ReceiveData;
							rxState = waitForDataLength;
					}else{
							cksum = 0;
							rxState = waitForStartByte1;//reset state
					}
				break;
			case waitForDataLength:
					if(waitForDataLength == rxState){
							rxPacket.validDataLen = uart0ReceiveData - FIX_DATA_LEN;
							cksum += uart0ReceiveData;
							rxState = waitForMsgID;
					}else{
							rxState = waitForStartByte1;
					}
				break;
			case waitForMsgID:
					if(waitForMsgID == rxState){
							rxPacket.msgID = uart0ReceiveData;
							cksum += uart0ReceiveData;	
							rxState = waitForData;
					}else{
							rxState = waitForStartByte1;
					}
				break;
			case waitForData:
					if(waitForData == rxState){								
							rxPacket.mcuData[dataIndex] = uart0ReceiveData;
							dataIndex++;
							cksum += uart0ReceiveData;
							if(dataIndex == rxPacket.validDataLen){
									dataIndex = 0;
									rxState = waitForChksum1;
							}
					}else{
							rxState = waitForStartByte1;
					}
				break;		
			case waitForChksum1:
				if(cksum == uart0ReceiveData){//check sum success
						bluetoothDataPreocess();
						//CH548L_LOG("success\n");
						dataIndex = 0;
						cksum = 0;
						rxState = waitForStartByte1;
				}													
				break;	
			
			default:
				break;
		}
}

void MCUSendDataToBluetooth(qsportdata_t *packet)
{
		UINT8 arrayOverflowWarning = 10;
		UINT8 index=0;
		UINT8 SendBufferIndex = 0;
		UINT8 payloadIndex = 0;
		UINT8 checkSum = 0;
		UINT8 sendBuffer[MCU_MAX_SEND_DATA_LEN] = {0};
		
		if(packet->validDataLen >= (MCU_MAX_SEND_DATA_LEN-FIX_DATA_LEN)){
				while(arrayOverflowWarning--){
						ledRGBColor(1,0,1);
						mDelaymS(500);
						ledRGBColor(0,0,0);
						mDelaymS(500);
				}
				return;
		}//array overflow process
		
		checkSum = FRAME_HEADER + (packet->validDataLen+FIX_DATA_LEN) + packet->msgID;
		
		sendBuffer[SendBufferIndex++] = FRAME_HEADER;
		sendBuffer[SendBufferIndex++] = packet->validDataLen+FIX_DATA_LEN;
		sendBuffer[SendBufferIndex++] = packet->msgID;
		
		for(payloadIndex=0; payloadIndex<packet->validDataLen; payloadIndex++){
				sendBuffer[SendBufferIndex++] = packet->mcuData[payloadIndex];
				checkSum += packet->mcuData[payloadIndex];
		}
		
		sendBuffer[SendBufferIndex++] = checkSum;
	
		
		ES = 0;
		//printf("send Data ");
		for(index=0; index<(packet->validDataLen+FIX_DATA_LEN); index++){
				//printf("%02x ",(UINT16)sendBuffer[index]);
				CH549UART0SendByte(sendBuffer[index]);
		}	
		//printf("\n");
		ES = 1;
}

/*sn coding scheme*/
/*
---------------------------------------------------------------------------------------------
|byte0 | byte1 | byte2  byte3 | byte4  byte5 | byte6  byte7  byte8  byte9 | byte10 | byte11 |
---------------------------------------------------------------------------------------------
| X    |  Q    | 							|							 |														|				|					|
---------------------------------------------------------------------------------------------
*/
#define SN_DATA_LENGTH 16
#define SN_DATA_STORAGE_LOCATION 0xF000
void bluetoothModuleInit(void)
{
		UINT8 Index = 0;
		UINT16 trySendbtNameCount = 10;
		UINT8 readProductionSN[SN_DATA_LENGTH];
		//UINT8 dataArray[13] = {0x02, 0x58, 0x51, 0x30, 0x32, 0x30, 0x33, 0x30, 0x30, 0x35, 0x30, 0x37, 0x31};//Q200 ascii(only test)
		//UINT8 dataArray[13] = {0x02, 0x58, 0x51, 0x00, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x07, 0x01};
		UINT8 dataArray[13] = {0x02, 0x58, 0x51, 0x30, 0x32, 0x31, 0x31, 0x30, 0x30, 0x30, 0x31, 0x37, 0x31};//need send ascii
		
		memset(readProductionSN, 0x30, SN_DATA_LENGTH);
		FlashReadBuf(SN_DATA_STORAGE_LOCATION, readProductionSN, SN_DATA_LENGTH);
		//printf("flash 0x%02x 0x%02x 0x%02x 0x%02x\n",(UINT16)readProductionSN[6], (UINT16)readProductionSN[7],(UINT16)readProductionSN[8],(UINT16)readProductionSN[9]);
		if(0xff != readProductionSN[6] && 0xff != readProductionSN[7] && 0xff != readProductionSN[8] && 0xff != readProductionSN[9]){
				dataArray[7] = readProductionSN[6];
				dataArray[8] = readProductionSN[7];
				dataArray[9] = readProductionSN[8];
				dataArray[10] = readProductionSN[9];
		}
		
		txPacket.msgID = 0x0A;
		txPacket.validDataLen = 13;
		
		for(Index=0; Index<txPacket.validDataLen; Index++){
				txPacket.mcuData[Index] = dataArray[Index];
		}

		while(trySendbtNameCount--){
				mDelaymS(50);
				MCUSendDataToBluetooth(&txPacket);
				//printf("test bt init\n");
		}		
			
}

/*

*/


void mcuReportSportData(mcu_report_data_t *reportData)
{
		UINT8 Index = 0;
		//          rate(2byte)   power(2byte)     force(1byte)   frequence
		//			 80 04020020        040100A7         0203a8         0207a8
		
		//format:   attrid(Data class) typeid Data
		//eg: 0x80, 0x04,0x02,(0x00,0x20),    0x04,0x01,(0x00,0xA7),    0x02,0x03,(0xa8)
		UINT8 dataArray[12] = {0x80, 0x04,0x02,0x00,0x20,    0x04,0x01,0x00,0xA7,    0x02,0x03,0xa8};//report all value success	

		//printf("report 0x%02x %d\n",(UINT16)rotarySpeedMeasureResult(),(UINT16)rotarySpeedMeasureResult());
//		dataArray[3] = (UINT8)((rotarySpeedMeasureResult() & 0xff00)>>8);//rate HIGH
//		dataArray[4] = (UINT8)(rotarySpeedMeasureResult() & 0xff);//rate LOW
		
//				printf("report %d %d %d \n",(UINT16)reportData->rotarySpeedMeasureData,(UINT16)reportData->powerMeasureData,(UINT16)reportData->forceMeasureData);
				dataArray[3] = (reportData->rotarySpeedMeasureData & 0xff00) >> 8;//rate HIGH
				dataArray[4] = (UINT8)( reportData->rotarySpeedMeasureData & 0xff);//rate LOW		

				dataArray[7] = (reportData->powerMeasureData & 0xff00) >> 8;//power HIGH
				dataArray[8] = (UINT8)((reportData->powerMeasureData) & 0xff);//power LOW			
		
				dataArray[11] = reportData->forceMeasureData;//force

		
		/*
		dataArray[3] = (UINT8)((reportData->rotarySpeedMeasureData) & 0xff00)>>8);//rate HIGH
		dataArray[4] = (UINT8)((reportData->rotarySpeedMeasureData) & 0xff);//rate LOW

		dataArray[7] = (UINT8)((reportData->powerMeasureData) & 0xff00)>>8);;//power HIGH
		dataArray[8] = (UINT8)((reportData->powerMeasureData) & 0xff);;//power LOW	

		dataArray[11] = reportData->forceMeasureData;//force
		*/
		
		txPacket.msgID = 0x00;
		txPacket.validDataLen = 12;//note:if modify report Data you need modify there otherwise wil appear error

		
		for(Index=0; Index<txPacket.validDataLen; Index++){
				txPacket.mcuData[Index] = dataArray[Index];
		}

		MCUSendDataToBluetooth(&txPacket);
}


//02   method
//58 51 30 32 33 34 35 36 37 38 39 30 Data
void testBt(void)
{
		UINT8 Index = 0;
#if 1	//test
		UINT8 dataArray[13] = {0x02, 0x58, 0x51, 0x30, 0x32, 0x30, 0x33, 0x30, 0x30, 0x35, 0x30, 0x37, 0x31};
		txPacket.msgID = 0x0A;
		txPacket.validDataLen = 13;
#endif	
		
		

#if 0	//send device's SN
		// method = 0x02(QIDSYNC) sn = [(58 51)  (00 02)  (01 01)  (00 00 00 01)  (07) (01)]
		UINT8 dataArray[13] = {0x02, 0x58, 0x51, 0x00, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x07, 0x01};
		txPacket.msgID = 0x0A;
		txPacket.validDataLen = 13;
		isInquireBluetoothState  = 1;
#endif	
		
		for(Index=0; Index<txPacket.validDataLen; Index++){
				txPacket.mcuData[Index] = dataArray[Index];
		}


		MCUSendDataToBluetooth(&txPacket);	
}



