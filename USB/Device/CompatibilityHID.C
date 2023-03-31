/********************************** (C) COPYRIGHT *******************************
* File Name          : CompatibilityHID.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/15
* Description        : CH549ģ��HID�����豸��֧���ж����´���֧�ֿ��ƶ˵����´���֧������ȫ�٣�����
                       ע�����DEBUG.C
*******************************************************************************/
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
#define Fullspeed
#ifdef  Fullspeed
#define THIS_ENDP0_SIZE         64
#else
#define THIS_ENDP0_SIZE         8                                                  //����USB���жϴ��䡢���ƴ�����������Ϊ8
#endif
UINT8X  Ep0Buffer[ THIS_ENDP0_SIZE+2 ] _at_ 0x0000;                                //�˵�0 OUT&IN��������������ż��ַ
UINT8X  Ep2Buffer[ 2*MAX_PACKET_SIZE+4] _at_ THIS_ENDP0_SIZE+2;                    //�˵�2 IN&OUT������,������ż��ַ
UINT8   SetupReq,Ready,UsbConfig;
UINT16  SetupLen;
PUINT8  pDescr;                                                                    //USB���ñ�־
USB_SETUP_REQ   SetupReqBuf;                                                       //�ݴ�Setup��
#define UsbSetupBuf     ((PUSB_SETUP_REQ)Ep0Buffer)
sbit Ep2InKey = P1^5;                                                              //K1����
#pragma  NOAREGS
/*�豸������*/
UINT8C DevDesc[] = {0x12,0x01,0x10,0x01,0x00,0x00,0x00,THIS_ENDP0_SIZE,
                    0x86,0x1A,0xE0,0xE6,0x00,0x00,0x01,0x02,
                    0x00,0x01
                   };
UINT8C CfgDesc[] =
{
    0x09,0x02,0x29,0x00,0x01,0x01,0x04,0xA0,0x23,               //����������
    0x09,0x04,0x00,0x00,0x02,0x03,0x00,0x00,0x05,               //�ӿ�������
    0x09,0x21,0x00,0x01,0x00,0x01,0x22,0x22,0x00,               //HID��������
#ifdef  Fullspeed
    0x07,0x05,0x82,0x03,THIS_ENDP0_SIZE,0x00,0x01,              //�˵�������(ȫ�ټ��ʱ��ĳ�1ms)
    0x07,0x05,0x02,0x03,THIS_ENDP0_SIZE,0x00,0x01,              //�˵�������
#else
    0x07,0x05,0x82,0x03,THIS_ENDP0_SIZE,0x00,0x0A,              //�˵�������(���ټ��ʱ����С10ms)
    0x07,0x05,0x02,0x03,THIS_ENDP0_SIZE,0x00,0x0A,              //�˵�������
#endif
};
/*�ַ��������� ��*/
/*HID�౨��������*/
UINT8C HIDRepDesc[ ] =
{
    0x06, 0x00,0xff,
    0x09, 0x01,
    0xa1, 0x01,                                                   //���Ͽ�ʼ
    0x09, 0x02,                                                   //Usage Page  �÷�
    0x15, 0x00,                                                   //Logical  Minimun
    0x26, 0x00,0xff,                                              //Logical  Maximun
    0x75, 0x08,                                                   //Report Size
    0x95, THIS_ENDP0_SIZE,                                        //Report Counet
    0x81, 0x06,                                                   //Input
    0x09, 0x02,                                                   //Usage Page  �÷�
    0x15, 0x00,                                                   //Logical  Minimun
    0x26, 0x00,0xff,                                              //Logical  Maximun
    0x75, 0x08,                                                   //Report Size
    0x95, THIS_ENDP0_SIZE,                                        //Report Counet
    0x91, 0x06,                                                   //Output
    0xC0
};
// ����������
UINT8C  MyLangDescr[] = { 0x04, 0x03, 0x09, 0x04 };
// ������Ϣ
UINT8C  MyManuInfo[] = { 0x0E, 0x03, 'w', 0, 'c', 0, 'h', 0, '.', 0, 'c', 0, 'n', 0 };
// ��Ʒ��Ϣ
UINT8C  MyProdInfo[] = { 0x0C, 0x03, 'C', 0, 'H', 0, '5', 0, '4', 0, '9', 0 };                                //�ַ���������
UINT8X UserEp2Buf[64];                                            //�û����ݶ���
UINT8 Endp2Busy = 0;
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
    UEP2_T_LEN = 0;                                                            //Ԥʹ�÷��ͳ���һ��Ҫ���
    UEP2_DMA = Ep2Buffer;                                                      //�˵�2���ݴ����ַ
    UEP2_3_MOD |= bUEP2_TX_EN | bUEP2_RX_EN;                                   //�˵�2���ͽ���ʹ��
    UEP2_3_MOD &= ~bUEP2_BUF_MOD;                                              //�˵�2�շ���64�ֽڻ�����
    UEP2_CTRL = bUEP_AUTO_TOG | UEP_T_RES_NAK | UEP_R_RES_ACK;                 //�˵�2�Զ���תͬ����־λ��IN���񷵻�NAK��OUT����ACK
    UEP0_DMA = Ep0Buffer;                                                      //�˵�0���ݴ����ַ
    UEP4_1_MOD &= ~(bUEP4_RX_EN | bUEP4_TX_EN);                                //�˵�0��64�ֽ��շ�������
    UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;                                 //OUT���񷵻�ACK��IN���񷵻�NAK
    USB_DEV_AD = 0x00;
    USB_CTRL |= bUC_DEV_PU_EN | bUC_INT_BUSY | bUC_DMA_EN;                     // ����USB�豸��DMA�����ж��ڼ��жϱ�־δ���ǰ�Զ�����NAK
    UDEV_CTRL |= bUD_PORT_EN;                                                  // ����USB�˿�
    USB_INT_FG = 0xFF;                                                         // ���жϱ�־
    USB_INT_EN = bUIE_SUSPEND | bUIE_TRANSFER | bUIE_BUS_RST;
    IE_USB = 1;
}
/*******************************************************************************
* Function Name  : Enp2BlukIn()
* Description    : USB�豸ģʽ�˵�2�������ϴ�
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Enp2BlukIn( UINT8 *buf,UINT8 len)
{
    memcpy( Ep2Buffer+MAX_PACKET_SIZE, buf, len);                              //�����ϴ�����
    UEP2_T_LEN = len;                                                          //�ϴ���������
    UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;                  //������ʱ�ϴ����ݲ�Ӧ��ACK
}
/*******************************************************************************
* Function Name  : DeviceInterrupt()
* Description    : CH559USB�жϴ�������
*******************************************************************************/
void  DeviceInterrupt( void ) interrupt INT_NO_USB using 1                     //USB�жϷ������,ʹ�üĴ�����1
{
    UINT8 i;
    UINT16 len;
    if(UIF_TRANSFER)                                                            //USB������ɱ�־
    {
        switch (USB_INT_ST & (MASK_UIS_TOKEN | MASK_UIS_ENDP))
        {
        case UIS_TOKEN_IN | 2:                                                  //endpoint 2# �˵������ϴ�
            UEP2_T_LEN = 0;                                                     //Ԥʹ�÷��ͳ���һ��Ҫ���
//            UEP1_CTRL ^= bUEP_T_TOG;                                          //����������Զ���ת����Ҫ�ֶ���ת
            Endp2Busy = 0 ;
            UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;           //Ĭ��Ӧ��NAK
            break;
        case UIS_TOKEN_OUT | 2:                                                 //endpoint 2# �˵������´�
            if ( U_TOG_OK )                                                     // ��ͬ�������ݰ�������
            {
                len = USB_RX_LEN;                                               //�������ݳ��ȣ����ݴ�Ep2Buffer�׵�ַ��ʼ���
                for ( i = 0; i < len; i ++ )
                {
                    Ep2Buffer[MAX_PACKET_SIZE+i] = Ep2Buffer[i] ^ 0xFF;         // OUT����ȡ����IN�ɼ������֤
                }
                UEP2_T_LEN = len;
                UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;       // �����ϴ�
            }
            break;
        case UIS_TOKEN_SETUP | 0:                                               //SETUP����
            UEP0_CTRL = UEP0_CTRL & (~MASK_UEP_T_RES )| UEP_T_RES_NAK;          //Ԥ��NAK,��ֹstall֮�󲻼�ʱ�����Ӧ��ʽ
            len = USB_RX_LEN;
            if(len == (sizeof(USB_SETUP_REQ)))
            {
                SetupLen = ((UINT16)UsbSetupBuf->wLengthH<<8) + UsbSetupBuf->wLengthL;
                len = 0;                                                         // Ĭ��Ϊ�ɹ������ϴ�0����
                SetupReq = UsbSetupBuf->bRequest;
                if ( ( UsbSetupBuf->bRequestType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD )/*HID������*/
                {
                    switch( SetupReq )
                    {
                        Ready = 1;                                              //HID������һ�����usbö����ɵı�־
                    case 0x01:                                                //GetReport
                        pDescr = UserEp2Buf;                                    //���ƶ˵��ϴ����
                        if(SetupLen >= THIS_ENDP0_SIZE)                         //���ڶ˵�0��С����Ҫ���⴦��
                        {
                            len = THIS_ENDP0_SIZE;
                        }
                        else
                        {
                            len = SetupLen;
                        }
                        break;
                    case 0x02:                                                   //GetIdle
                        break;
                    case 0x03:                                                   //GetProtocol
                        break;
                    case 0x09:                                                   //SetReport
                        break;
                    case 0x0A:                                                   //SetIdle
                        break;
                    case 0x0B:                                                   //SetProtocol
                        break;
                    default:
                        len = 0xFFFF;                                                    /*���֧��*/
                        break;
                    }
                    if( SetupLen > len )
                    {
                        SetupLen = len;    //�����ܳ���
                    }
                    len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen;   //���δ��䳤��
                    memcpy(Ep0Buffer,pDescr,len);                                     //�����ϴ�����
                    SetupLen -= len;
                    pDescr += len;
                }
                else                                                             //��׼����
                {
                    switch(SetupReq)                                             //������
                    {
                    case USB_GET_DESCRIPTOR:
                        switch(UsbSetupBuf->wValueH)
                        {
                        case 1:                                                  //�豸������
                            pDescr = DevDesc;                                    //���豸�������͵�Ҫ���͵Ļ�����
                            len = sizeof(DevDesc);
                            break;
                        case 2:                                                  //����������
                            pDescr = CfgDesc;                                    //���豸�������͵�Ҫ���͵Ļ�����
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
                                len = 0xFFFF;                                // ��֧�ֵ��ַ���������
                                break;
                            }
                            break;
                        case 0x22:                                               //����������
                            pDescr = HIDRepDesc;                                 //����׼���ϴ�
                            len = sizeof(HIDRepDesc);
                            break;
                        default:
                            len = 0xFFFF;                                        //��֧�ֵ�������߳���
                            break;
                        }
                        if ( SetupLen > len )
                        {
                            SetupLen = len;    //�����ܳ���
                        }
                        len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen;//���δ��䳤��
                        memcpy(Ep0Buffer,pDescr,len);                            //�����ϴ�����
                        SetupLen -= len;
                        pDescr += len;
                        break;
                    case USB_SET_ADDRESS:
                        SetupLen = UsbSetupBuf->wValueL;                         //�ݴ�USB�豸��ַ
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
                            Ready = 1;                                            //set config����һ�����usbö����ɵı�־
                        }
                        break;
                    case 0x0A:
                        break;
                    case USB_CLEAR_FEATURE:                                      //Clear Feature
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
                            case 0x02:
                                UEP2_CTRL = UEP2_CTRL & ~ ( bUEP_R_TOG | MASK_UEP_R_RES ) | UEP_R_RES_ACK;
                                break;
                            default:
                                len = 0xFFFF;                                     // ��֧�ֵĶ˵�
                                break;
                            }
                        }
                        else
                        {
                            len = 0xFFFF;                                         // ���Ƕ˵㲻֧��
                        }
                        break;
                    case USB_SET_FEATURE:                                         /* Set Feature */
                        if( ( UsbSetupBuf->bRequestType & 0x1F ) == 0x00 )        /* �����豸 */
                        {
                            if( ( ( ( UINT16 )UsbSetupBuf->wValueH << 8 ) | UsbSetupBuf->wValueL ) == 0x01 )
                            {
                                if( CfgDesc[ 7 ] & 0x20 )
                                {
                                    /* ���û���ʹ�ܱ�־ */
                                }
                                else
                                {
                                    len = 0xFFFF;                                  /* ����ʧ�� */
                                }
                            }
                            else
                            {
                                len = 0xFFFF;                                      /* ����ʧ�� */
                            }
                        }
                        else if( ( UsbSetupBuf->bRequestType & 0x1F ) == 0x02 )    /* ���ö˵� */
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
                                    len = 0xFFFF;                                   /* ����ʧ�� */
                                    break;
                                }
                            }
                            else
                            {
                                len = 0xFFFF;                                       /* ����ʧ�� */
                            }
                        }
                        else
                        {
                            len = 0xFFFF;                                           /* ����ʧ�� */
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
                        len = 0xFFFF;                                                //����ʧ��
                        break;
                    }
                }
            }
            else
            {
                len = 0xFFFF;                                                        //�����ȴ���
            }
            if(len == 0xFFFF)
            {
                SetupReq = 0xFF;
                UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL;//STALL
            }
            else if(len <= THIS_ENDP0_SIZE)                                         //�ϴ����ݻ���״̬�׶η���0���Ȱ�
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
        case UIS_TOKEN_IN | 0:                                                      //endpoint0 IN
            switch(SetupReq)
            {
            case USB_GET_DESCRIPTOR:
            case HID_GET_REPORT:
                len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen;     //���δ��䳤��
                memcpy( Ep0Buffer, pDescr, len );                                   //�����ϴ�����
                SetupLen -= len;
                pDescr += len;
                UEP0_T_LEN = len;
                UEP0_CTRL ^= bUEP_T_TOG;                                            //ͬ����־λ��ת
                break;
            case USB_SET_ADDRESS:
                USB_DEV_AD = USB_DEV_AD & bUDA_GP_BIT | SetupLen;
                UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
                break;
            default:
                UEP0_T_LEN = 0;                                                      //״̬�׶�����жϻ�����ǿ���ϴ�0�������ݰ��������ƴ���
                UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
                break;
            }
            break;
        case UIS_TOKEN_OUT | 0:  // endpoint0 OUT
            len = USB_RX_LEN;
            UEP0_CTRL ^= bUEP_R_TOG;                                                //ͬ����־λ��ת
            break;
        default:
            break;
        }
        UIF_TRANSFER = 0;                                                           //д0����ж�
    }
    else if(UIF_BUS_RST)                                                            //�豸ģʽUSB���߸�λ�ж�
    {
        UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
        UEP2_CTRL = bUEP_AUTO_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;
        USB_DEV_AD = 0x00;
        UIF_SUSPEND = 0;
        UIF_TRANSFER = 0;
        Endp2Busy = 0;
        Ready = 0;
        UIF_BUS_RST = 0;                                                            //���жϱ�־
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
//             WAKE_CTRL = bWA  K_BY_USB | bWAK_RXD0_LO;                              //USB����RXD0���ź�ʱ�ɱ�����
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
void main()
{
    UINT8 i;
    CfgFsys( );                                                            //CH549ʱ��ѡ������
    mDelaymS(20);                                                          //�޸���Ƶ�ȴ��ڲ������ȶ�,�ؼ�
    mInitSTDIO( );                                                         //����0��ʼ��
#ifdef DE_PRINTF
    printf("start ...\n");
#endif
    for(i=0; i<64; i++)                                                    //׼����ʾ����
    {
        UserEp2Buf[i] = i;
    }
    USBDeviceInit();                                                       //USB�豸ģʽ��ʼ��
    EA = 1;                                                                //������Ƭ���ж�
    while(1)
    {
        if(Ready && (Ep2InKey==0))
        {
            while( Endp2Busy )
            {
                ;    //���æ����һ������û�д���ȥ������ȴ���
            }
            Endp2Busy = 1;                                                 //����Ϊæ״̬
            Enp2BlukIn( UserEp2Buf,THIS_ENDP0_SIZE );
        }
        mDelaymS( 100 );                                                   //ģ�ⵥƬ����������
    }
}