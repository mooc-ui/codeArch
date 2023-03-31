/********************************** (C) COPYRIGHT *******************************
* File Name          : CompositeKM.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/15
* Description        : CH549ģ��USB���󸴺��豸,֧��������,֧�ֻ���
                       ��ʾ�������򵥲�����������ֵ���ο� HID USAGE TABLEЭ���ĵ�
                       ����0�����ַ���
                       ��L����������
                       ��R��: ����Ҽ�
                       ��A��: ���̡�A����
                       ��Q��: ���̡�Caps����
                    �����ַ�������˯��״̬��,�豸Զ�̻���������ע���豸һ�����Թ���,��Ϊ�������߿���USB��Ҳ����磩
*******************************************************************************/
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
//#define Fullspeed
#ifdef  Fullspeed
#define THIS_ENDP0_SIZE         64
#else
#define THIS_ENDP0_SIZE         8                                              //����USB���жϴ��䡢���ƴ�����������Ϊ8
#endif
#define EP1_IN_SIZE             8                                              //���̶˵����ݰ���С
#define EP2_IN_SIZE             4                                              //���˵����ݰ���С
UINT8X  Ep0Buffer[ THIS_ENDP0_SIZE+2 ] _at_ 0x0000;                            //�˵�0 OUT&IN��������������ż��ַ
UINT8X  Ep1Buffer[ EP1_IN_SIZE+2 ]     _at_ THIS_ENDP0_SIZE+2;                 //�˵�1 IN������,������ż��ַ
UINT8X  Ep2Buffer[ EP2_IN_SIZE+2 ]     _at_ THIS_ENDP0_SIZE+EP1_IN_SIZE+4;     //�˵�2 IN������,������ż��ַ
UINT8   SetupReq,Ready,UsbConfig;
UINT16  SetupLen;
PUINT8  pDescr;                                                                //USB���ñ�־
USB_SETUP_REQ   SetupReqBuf;                                                   //�ݴ�Setup��
#define UsbSetupBuf     ((PUSB_SETUP_REQ)Ep0Buffer)
#pragma  NOAREGS
/*�豸������*/
UINT8C DevDesc[] = { 0x12,0x01,0x10,0x01,0x00,0x00,0x00,THIS_ENDP0_SIZE,
                     0x86,0x1a,0xe1,0xe6,0x00,0x01,0x01,0x02,
                     0x00,0x01
                   };
/*�ַ���������*/
UINT8C  MyLangDescr[] = { 0x04, 0x03, 0x09, 0x04 };                                    // ����������
UINT8C  MyManuInfo[] = { 0x0E, 0x03, 'w', 0, 'c', 0, 'h', 0, '.', 0, 'c', 0, 'n', 0 }; // ������Ϣ
UINT8C  MyProdInfo[] = { 0x0C, 0x03, 'C', 0, 'H', 0, '5', 0, '4', 0, '9', 0 };         // ��Ʒ��Ϣ
/*HID�౨��������*/
UINT8C KeyRepDesc[] =
{
    0x05,0x01,0x09,0x06,0xA1,0x01,0x05,0x07,
    0x19,0xe0,0x29,0xe7,0x15,0x00,0x25,0x01,
    0x75,0x01,0x95,0x08,0x81,0x02,0x95,0x01,
    0x75,0x08,0x81,0x01,0x95,0x03,0x75,0x01,
    0x05,0x08,0x19,0x01,0x29,0x03,0x91,0x02,
    0x95,0x05,0x75,0x01,0x91,0x01,0x95,0x06,
    0x75,0x08,0x26,0xff,0x00,0x05,0x07,0x19,
    0x00,0x29,0x91,0x81,0x00,0xC0
};
UINT8C MouseRepDesc[] =
{
    0x05,0x01,0x09,0x02,0xA1,0x01,0x09,0x01,
    0xA1,0x00,0x05,0x09,0x19,0x01,0x29,0x03,
    0x15,0x00,0x25,0x01,0x75,0x01,0x95,0x03,
    0x81,0x02,0x75,0x05,0x95,0x01,0x81,0x01,
    0x05,0x01,0x09,0x30,0x09,0x31,0x09,0x38,
    0x15,0x81,0x25,0x7f,0x75,0x08,0x95,0x03,
    0x81,0x06,0xC0,0xC0
};
/*����������*/
UINT8C CfgDesc[] =
{
    0x09,0x02,0x3b,0x00,0x02,0x01,0x00,0xA0,0x32,                    //����������

    0x09,0x04,0x00,0x00,0x01,0x03,0x01,0x01,0x00,                    //�ӿ�������,����
    0x09,0x21,0x11,0x01,0x00,0x01,0x22,sizeof(KeyRepDesc)&0xFF,sizeof(KeyRepDesc)>>8,//HID��������
    0x07,0x05,0x81,0x03,EP1_IN_SIZE,0x00,0x0a,                       //�˵�������

    0x09,0x04,0x01,0x00,0x01,0x03,0x01,0x02,0x00,                    //�ӿ�������,���
    0x09,0x21,0x10,0x01,0x00,0x01,0x22,sizeof(MouseRepDesc)&0xFF,sizeof(MouseRepDesc)>>8,//HID��������
    0x07,0x05,0x82,0x03,EP2_IN_SIZE,0x00,0x0a                        //�˵�������
};
/*��������*/
UINT8 HIDKey[EP1_IN_SIZE];
/*�������*/
UINT8 HIDMouse[EP2_IN_SIZE];
UINT8 Endp1Busy = 0;                                                 //������ɿ��Ʊ�־λ
UINT8 Endp2Busy = 0;
UINT8 WakeUpEnFlag = 0;                                              //Զ�̻���ʹ�ܱ�־
/*******************************************************************************
* Function Name  : CH554USBDevWakeup()
* Description    : CH554�豸ģʽ��������������K�ź�
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH554USBDevWakeup( )
{
#ifdef Fullspeed
    UDEV_CTRL |= bUD_LOW_SPEED;
    mDelaymS(2);
    UDEV_CTRL &= ~bUD_LOW_SPEED;
#else
    UDEV_CTRL &= ~bUD_LOW_SPEED;
    mDelaymS(2);
    UDEV_CTRL |= bUD_LOW_SPEED;
#endif
}
/*******************************************************************************
* Function Name  : USBDeviceInit()
* Description    : USB�豸ģʽ����,�豸ģʽ�������շ��˵����ã��жϿ���
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USBDeviceInit()
{
    IE_USB = 0;
    USB_CTRL = 0x00;                                                           // ���趨USB�豸ģʽ
    UDEV_CTRL = bUD_PD_DIS;                                                    // ��ֹDP/DM��������
#ifndef Fullspeed
    UDEV_CTRL |= bUD_LOW_SPEED;                                                //ѡ�����1.5Mģʽ
    USB_CTRL |= bUC_LOW_SPEED;
#else
    UDEV_CTRL &= ~bUD_LOW_SPEED;                                               //ѡ��ȫ��12Mģʽ��Ĭ�Ϸ�ʽ
    USB_CTRL &= ~bUC_LOW_SPEED;
#endif
    UEP1_T_LEN = 0;                                                            //Ԥʹ�÷��ͳ���һ��Ҫ���
    UEP2_T_LEN = 0;                                                            //Ԥʹ�÷��ͳ���һ��Ҫ���
    UEP2_DMA = Ep2Buffer;                                                      //�˵�2���ݴ����ַ
    UEP2_3_MOD = UEP2_3_MOD & ~bUEP2_BUF_MOD | bUEP2_TX_EN;                    //�˵�2����ʹ�� 64�ֽڻ�����
    UEP2_CTRL = bUEP_AUTO_TOG | UEP_T_RES_NAK;                                 //�˵�2�Զ���תͬ����־λ��IN���񷵻�NAK
    UEP0_DMA = Ep0Buffer;                                                      //�˵�0���ݴ����ַ
    UEP4_1_MOD &= ~(bUEP4_RX_EN | bUEP4_TX_EN);                                //�˵�0��64�ֽ��շ�������
    UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;                                 //OUT���񷵻�ACK��IN���񷵻�NAK
    UEP1_DMA = Ep1Buffer;                                                      //�˵�1���ݴ����ַ
    UEP4_1_MOD = UEP4_1_MOD & ~bUEP1_BUF_MOD | bUEP1_TX_EN;                    //�˵�1����ʹ�� 64�ֽڻ�����
    UEP1_CTRL = bUEP_AUTO_TOG | UEP_T_RES_NAK;                                 //�˵�1�Զ���תͬ����־λ��IN���񷵻�NAK
    USB_DEV_AD = 0x00;
    USB_CTRL |= bUC_DEV_PU_EN | bUC_INT_BUSY | bUC_DMA_EN;                     // ����USB�豸��DMA�����ж��ڼ��жϱ�־δ���ǰ�Զ�����NAK
    UDEV_CTRL |= bUD_PORT_EN;                                                  // ����USB�˿�
    USB_INT_FG = 0xFF;                                                         // ���жϱ�־
    USB_INT_EN = bUIE_SUSPEND | bUIE_TRANSFER | bUIE_BUS_RST;
    IE_USB = 1;
}
/*******************************************************************************
* Function Name  : Enp1IntIn
* Description    : USB�豸ģʽ�˵�1���ж��ϴ�
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Enp1IntIn( UINT8 *buf,UINT8 len )
{
    memcpy( Ep1Buffer, buf, len );                                            //�����ϴ�����
    UEP1_T_LEN = len;                                                         //�ϴ����ݳ���
    UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;                 //������ʱ�ϴ����ݲ�Ӧ��ACK
}
/*******************************************************************************
* Function Name  : Enp2IntIn()
* Description    : USB�豸ģʽ�˵�2���ж��ϴ�
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Enp2IntIn( UINT8 *buf,UINT8 len )
{
    memcpy( Ep2Buffer, buf, len);                                              //�����ϴ�����
    UEP2_T_LEN = len;                                                          //�ϴ����ݳ���
    UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;                  //������ʱ�ϴ����ݲ�Ӧ��ACK
}
/*******************************************************************************
* Function Name  : DeviceInterrupt()
* Description    : CH559USB�жϴ�������
*******************************************************************************/
void    DeviceInterrupt( void ) interrupt INT_NO_USB using 1                    //USB�жϷ������,ʹ�üĴ�����1
{
    UINT16 len = 0;
    if(UIF_TRANSFER)                                                            //USB������ɱ�־
    {
        switch (USB_INT_ST & (MASK_UIS_TOKEN | MASK_UIS_ENDP))
        {
        case UIS_TOKEN_IN | 2:                                                  //endpoint 2# �ж϶˵��ϴ�
            UEP2_T_LEN = 0;                                                     //Ԥʹ�÷��ͳ���һ��Ҫ���
//            UEP1_CTRL ^= bUEP_T_TOG;                                          //����������Զ���ת����Ҫ�ֶ���ת
            Endp2Busy = 0;
            UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;           //Ĭ��Ӧ��NAK
            break;
        case UIS_TOKEN_IN | 1:                                                  //endpoint 1# �ж϶˵��ϴ�
            UEP1_T_LEN = 0;                                                     //Ԥʹ�÷��ͳ���һ��Ҫ���
//            UEP2_CTRL ^= bUEP_T_TOG;                                          //����������Զ���ת����Ҫ�ֶ���ת
            Endp1Busy = 0;
            UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;           //Ĭ��Ӧ��NAK
            break;
        case UIS_TOKEN_SETUP | 0:                                               //SETUP����
            UEP0_CTRL = UEP0_CTRL & (~MASK_UEP_T_RES )| UEP_T_RES_NAK;          //Ԥ��NAK,��ֹstall֮�󲻼�ʱ�����Ӧ��ʽ
            len = USB_RX_LEN;
            if(len == (sizeof(USB_SETUP_REQ)))
            {
                SetupLen = ((UINT16)UsbSetupBuf->wLengthH<<8) + UsbSetupBuf->wLengthL;
                len = 0;                                                        // Ĭ��Ϊ�ɹ������ϴ�0����
                SetupReq = UsbSetupBuf->bRequest;
                if ( ( UsbSetupBuf->bRequestType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD )/* HID������ */
                {
                    switch( SetupReq )
                    {
                    case 0x01://GetReport
                        break;
                    case 0x02://GetIdle
                        break;
                    case 0x03://GetProtocol
                        break;
                    case 0x09://SetReport
                        break;
                    case 0x0A://SetIdle
                        break;
                    case 0x0B://SetProtocol
                        break;
                    default:
                        len = 0xFFFF;                                  /*���֧��*/
                        break;
                    }
                }
                else
                {
                    //��׼����
                    switch(SetupReq)                                        //������
                    {
                    case USB_GET_DESCRIPTOR:
                        switch(UsbSetupBuf->wValueH)
                        {
                        case 1:                                             //�豸������
                            pDescr = DevDesc;                               //���豸�������͵�Ҫ���͵Ļ�����
                            len = sizeof(DevDesc);
                            break;
                        case 2:                                             //����������
                            pDescr = CfgDesc;                               //���豸�������͵�Ҫ���͵Ļ�����
                            len = sizeof(CfgDesc);
                            break;
                        case 3:
                            switch( UsbSetupBuf->wValueL )
                            {
                            case 1:
                                pDescr = (PUINT8)( &MyManuInfo[0] );
                                len = sizeof( MyManuInfo );
                                break;
                            case 2:
                                pDescr = (PUINT8)( &MyProdInfo[0] );
                                len = sizeof( MyProdInfo );
                                break;
                            case 0:
                                pDescr = (PUINT8)( &MyLangDescr[0] );
                                len = sizeof( MyLangDescr );
                                break;
                            default:
                                len = 0xFFFF;                           // ��֧�ֵ��ַ���������
                                break;
                            }
                            break;
                        case 0x22:                                          //����������
                            if(UsbSetupBuf->wIndexL == 0)                   //�ӿ�0����������
                            {
                                pDescr = KeyRepDesc;                        //����׼���ϴ�
                                len = sizeof(KeyRepDesc);
                            }
                            else if(UsbSetupBuf->wIndexL == 1)              //�ӿ�1����������
                            {
                                pDescr = MouseRepDesc;                      //����׼���ϴ�
                                len = sizeof(MouseRepDesc);
                            }
                            else
                            {
                                len = 0xFFFF;                                 //������ֻ��2���ӿڣ���仰����������ִ��
                            }
                            break;
                        default:
                            len = 0xFFFF;                                     //��֧�ֵ�������߳���
                            break;
                        }
                        if ( SetupLen > len )
                        {
                            SetupLen = len;    //�����ܳ���
                        }
                        len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen; //���δ��䳤��
                        memcpy(Ep0Buffer,pDescr,len);                        //�����ϴ�����
                        SetupLen -= len;
                        pDescr += len;
                        break;
                    case USB_SET_ADDRESS:
                        SetupLen = UsbSetupBuf->wValueL;                     //�ݴ�USB�豸��ַ
                        break;
                    case USB_GET_CONFIGURATION:
                        Ep0Buffer[0] = UsbConfig;
                        if ( SetupLen >= 1 )
                        {
                            len = 1;
                        }
                        break;
                    case USB_SET_CONFIGURATION:
                        UsbConfig = UsbSetupBuf->wValueL;
                        if(UsbConfig)
                        {
#ifdef DE_PRINTF
                            printf("SET CONFIG.\n");
#endif
                            Ready = 1;                                                   //set config����һ�����usbö����ɵı�־
                        }
                        break;
                    case 0x0A:
                        break;
                    case USB_CLEAR_FEATURE:                                              //Clear Feature
                        if ( ( UsbSetupBuf->bRequestType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )// �˵�
                        {
                            switch( UsbSetupBuf->wIndexL )
                            {
                            case 0x82:
                                UEP2_CTRL = UEP2_CTRL & ~ ( bUEP_T_TOG | MASK_UEP_T_RES ) | UEP_T_RES_NAK;
                                break;
                            case 0x81:
                                UEP1_CTRL = UEP1_CTRL & ~ ( bUEP_T_TOG | MASK_UEP_T_RES ) | UEP_T_RES_NAK;
                                break;
                            case 0x01:
                                UEP1_CTRL = UEP1_CTRL & ~ ( bUEP_R_TOG | MASK_UEP_R_RES ) | UEP_R_RES_ACK;
                                break;
                            default:
                                len = 0xFFFF;                                            // ��֧�ֵĶ˵�
                                break;
                            }
                        }
                        if ( ( UsbSetupBuf->bRequestType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_DEVICE )// �豸
                        {
                            WakeUpEnFlag = 0;
                            printf("Wake up\n");
                            break;
                        }
                        else
                        {
                            len = 0xFFFF;                                                // ���Ƕ˵㲻֧��
                        }
                        break;
                    case USB_SET_FEATURE:                                               /* Set Feature */
                        if( ( UsbSetupBuf->bRequestType & 0x1F ) == 0x00 )              /* �����豸 */
                        {
                            if( ( ( ( UINT16 )UsbSetupBuf->wValueH << 8 ) | UsbSetupBuf->wValueL ) == 0x01 )
                            {
                                if( CfgDesc[ 7 ] & 0x20 )
                                {
                                    WakeUpEnFlag = 1;                                   /* ���û���ʹ�ܱ�־ */
                                    printf("Enable Remote Wakeup.\n");
                                }
                                else
                                {
                                    len = 0xFFFF;                                        /* ����ʧ�� */
                                }
                            }
                            else
                            {
                                len = 0xFFFF;                                            /* ����ʧ�� */
                            }
                        }
                        else if( ( UsbSetupBuf->bRequestType & 0x1F ) == 0x02 )        /* ���ö˵� */
                        {
                            if( ( ( ( UINT16 )UsbSetupBuf->wValueH << 8 ) | UsbSetupBuf->wValueL ) == 0x00 )
                            {
                                switch( ( ( UINT16 )UsbSetupBuf->wIndexH << 8 ) | UsbSetupBuf->wIndexL )
                                {
                                case 0x82:
                                    UEP2_CTRL = UEP2_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;/* ���ö˵�2 IN STALL */
                                    break;
                                case 0x02:
                                    UEP2_CTRL = UEP2_CTRL & (~bUEP_R_TOG) | UEP_R_RES_STALL;/* ���ö˵�2 OUT Stall */
                                    break;
                                case 0x81:
                                    UEP1_CTRL = UEP1_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;/* ���ö˵�1 IN STALL */
                                    break;
                                default:
                                    len = 0xFFFF;                               //����ʧ��
                                    break;
                                }
                            }
                            else
                            {
                                len = 0xFFFF;                                   //����ʧ��
                            }
                        }
                        else
                        {
                            len = 0xFFFF;                                      //����ʧ��
                        }
                        break;
                    case USB_GET_STATUS:
                        Ep0Buffer[0] = 0x00;
                        Ep0Buffer[1] = 0x00;
                        if ( SetupLen >= 2 )
                        {
                            len = 2;
                        }
                        else
                        {
                            len = SetupLen;
                        }
                        break;
                    default:
                        len = 0xFFFF;                                           //����ʧ��
                        break;
                    }
                }
            }
            else
            {
                len = 0xFFFF;                                                   //�����ȴ���
            }
            if(len == 0xFFFF)
            {
                SetupReq = 0xFF;
                UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL;//STALL
            }
            else if(len)                                                //�ϴ����ݻ���״̬�׶η���0���Ȱ�
            {
                UEP0_T_LEN = len;
                UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;//Ĭ�����ݰ���DATA1������Ӧ��ACK
            }
            else
            {
                UEP0_T_LEN = 0;  //��Ȼ��δ��״̬�׶Σ�������ǰԤ���ϴ�0�������ݰ��Է�������ǰ����״̬�׶�
                UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;//Ĭ�����ݰ���DATA1,����Ӧ��ACK
            }
            break;
        case UIS_TOKEN_IN | 0:                                               //endpoint0 IN
            switch(SetupReq)
            {
            case USB_GET_DESCRIPTOR:
                len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen;    //���δ��䳤��
                memcpy( Ep0Buffer, pDescr, len );                            //�����ϴ�����
                SetupLen -= len;
                pDescr += len;
                UEP0_T_LEN = len;
                UEP0_CTRL ^= bUEP_T_TOG;                                     //ͬ����־λ��ת
                break;
            case USB_SET_ADDRESS:
                USB_DEV_AD = USB_DEV_AD & bUDA_GP_BIT | SetupLen;
                UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
                break;
            default:
                UEP0_T_LEN = 0;                                              //״̬�׶�����жϻ�����ǿ���ϴ�0�������ݰ��������ƴ���
                UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
                break;
            }
            break;
        case UIS_TOKEN_OUT | 0:  // endpoint0 OUT
            len = USB_RX_LEN;
            if(SetupReq == 0x09)
            {
                if(Ep0Buffer[0])
                {
                    printf("Light on Num Lock LED!\n");
                }
                else if(Ep0Buffer[0] == 0)
                {
                    printf("Light off Num Lock LED!\n");
                }
            }
            UEP0_CTRL ^= bUEP_R_TOG;                                      //ͬ����־λ��ת
            break;
        default:
            break;
        }
        UIF_TRANSFER = 0;                                                 //д0����ж�
    }
    else if(UIF_BUS_RST)                                                  //�豸ģʽUSB���߸�λ�ж�
    {
        UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
        UEP1_CTRL = bUEP_AUTO_TOG | UEP_T_RES_NAK;
        UEP2_CTRL = bUEP_AUTO_TOG | UEP_T_RES_NAK;
        USB_DEV_AD = 0x00;
        UIF_SUSPEND = 0;
        UIF_TRANSFER = 0;
        Ready = 0;
        UIF_BUS_RST = 0;                                                 //���жϱ�־
    }
    else if (UIF_SUSPEND)                                                           //USB���߹���/�������
    {
        UIF_SUSPEND = 0;
        if ( USB_MIS_ST & bUMS_SUSPEND )                                            //����
        {
#ifdef DE_PRINTF
            printf( "z" );                                                          //˯��״̬
#endif
//             while ( XBUS_AUX & bUART0_TX )
//             {
//                 ;    //�ȴ��������
//             }
//             SAFE_MOD = 0x55;
//             SAFE_MOD = 0xAA;
//             WAKE_CTRL = bWAK_BY_USB | bWAK_RXD0_LO;                              //USB����RXD0���ź�ʱ�ɱ�����
//             PCON |= PD;                                                          //˯��
//             SAFE_MOD = 0x55;
//             SAFE_MOD = 0xAA;
//             WAKE_CTRL = 0x00;
        }
        else                                                                        // ����
        {
#ifdef DE_PRINTF
            printf( "w" );
#endif
        }
    }
    else {                                                                          //������ж�,�����ܷ��������
        USB_INT_FG = 0xFF;                                                          //���жϱ�־
#ifdef DE_PRINTF
        printf("UnknownInt  \n");
#endif
    }
}
void HIDValueHandle()
{
    UINT8 i;
    i = getkey( );
    printf( "%c", (UINT8)i );
    if( WakeUpEnFlag )                                                   //����������
    {
        CH554USBDevWakeup();                                             //��������
    }
    else
    {
        switch(i)
        {
//��������ϴ�ʾ��
        case 'L':                                                        //���
            HIDMouse[0] = 0x01;
            while( Endp2Busy )
            {
                ;    //���æ����һ������û�д���ȥ������ȴ���
            }
            Endp2Busy = 1;                                               //����Ϊæ״̬
            Enp2IntIn(HIDMouse,sizeof(HIDMouse));
            HIDMouse[0] = 0;                                             //̧��
            while( Endp2Busy )
            {
                ;    //���æ����һ������û�д���ȥ������ȴ���
            }
            Endp2Busy = 1;                                               //����Ϊæ״̬
            Enp2IntIn(HIDMouse,sizeof(HIDMouse));
            break;
        case 'R':                                                        //�Ҽ�
            HIDMouse[0] = 0x02;
            while( Endp2Busy )
            {
                ;    //���æ����һ������û�д���ȥ������ȴ���
            }
            Endp2Busy = 1;                                               //����Ϊæ״̬
            Enp2IntIn(HIDMouse,sizeof(HIDMouse));
            HIDMouse[0] = 0;                                             //̧��
            while( Endp2Busy )
            {
                ;    //���æ����һ������û�д���ȥ������ȴ���
            }
            Endp2Busy = 1;                                               //����Ϊæ״̬
            Enp2IntIn(HIDMouse,sizeof(HIDMouse));
            break;
//���������ϴ�ʾ��
        case 'A':                                                         //A��
            HIDKey[2] = 0x04;                                             //������ʼ
            while( Endp1Busy )
            {
                ;    //���æ����һ������û�д���ȥ������ȴ���
            }
            Endp1Busy = 1;                                               //����Ϊæ״̬
            Enp1IntIn(HIDKey,sizeof(HIDKey));
            HIDKey[2] = 0;                                                //��������
            while( Endp1Busy )
            {
                ;    //���æ����һ������û�д���ȥ������ȴ���
            }
            Endp1Busy = 1;                                               //����Ϊæ״̬
            Enp1IntIn(HIDKey,sizeof(HIDKey));
            break;
        case 'Q':                                                         //CAP��
            HIDKey[2] = 0x39;
            while( Endp1Busy )
            {
                ;    //���æ����һ������û�д���ȥ������ȴ���
            }
            Endp1Busy = 1;                                               //����Ϊæ״̬
            Enp1IntIn(HIDKey,sizeof(HIDKey));
            HIDKey[2] = 0;                                                //��������
            while( Endp1Busy )
            {
                ;    //���æ����һ������û�д���ȥ������ȴ���
            }
            Endp1Busy = 1;                                               //����Ϊæ״̬
            Enp1IntIn(HIDKey,sizeof(HIDKey));
            break;
        default:                                                          //����
            break;
        }
    }
}
void main()
{
    CfgFsys( );                                                           //CH549ʱ��ѡ������
    mDelaymS(20);                                                         //�޸���Ƶ�ȴ��ڲ������ȶ�,�ؼ�
    mInitSTDIO( );                                                        //����0��ʼ��
    printf("KM Device start ...\n");
    USBDeviceInit();                                                      //USB�豸ģʽ��ʼ��
    EA = 1;                                                               //������Ƭ���ж�
    memset(HIDKey,0,sizeof(HIDKey));                                      //��ջ�����
    memset(HIDMouse,0,sizeof(HIDMouse));
    while(1)
    {
        if(Ready)
        {
            HIDValueHandle();                                             //����0,�����ͣ��getkey�����ȴ�����һ���ַ�
        }
    }
}