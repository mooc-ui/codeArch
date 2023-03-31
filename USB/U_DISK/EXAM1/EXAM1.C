/********************************** (C) COPYRIGHT *******************************
* File Name          : EXAM1.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/15
* Description        :
 CH549 C���Ե�U���ļ��ֽڶ�дʾ�������ļ�ָ��ƫ�ƣ��޸��ļ����ԣ�ɾ���ļ��Ȳ���
 ֧��: FAT12/FAT16/FAT32
 ע����� CH549UFI.LIB/USBHOST.C/DEBUG.C
*******************************************************************************/
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
#include ".\USB\USB_LIB\USBHOST.H"
#define DISK_BASE_BUF_LEN       512 /* Ĭ�ϵĴ������ݻ�������СΪ512�ֽ�(����ѡ��Ϊ2048����4096��֧��ĳЩ��������U��),Ϊ0���ֹ�ڱ��ļ��ж��建��������Ӧ�ó�����pDISK_BASE_BUF��ָ�� */
#define FOR_ROOT_UDISK_ONLY      1// ֻ����DP/DM�˿ڵ�U���ļ�����(ʹ���ӳ����CH549UFI/X),��֧��HUB��U�̲���
//����Ҫ����LIB���ļ�
//#define NO_DEFAULT_ACCESS_SECTOR      1       /* ��ֹĬ�ϵĴ���������д�ӳ���,���������б�д�ĳ�������� */
//#define NO_DEFAULT_DISK_CONNECT       1       /* ��ֹĬ�ϵļ����������ӳ���,���������б�д�ĳ�������� */
//#define NO_DEFAULT_FILE_ENUMER        1       /* ��ֹĬ�ϵ��ļ���ö�ٻص�����,���������б�д�ĳ�������� */
#include ".\USB\USB_LIB\CH549UFI.H"
#include ".\USB\USB_LIB\CH549UFI.C"
// ���ӳ��򷵻�״̬��
#define ERR_SUCCESS         0x00    // �����ɹ�
#define ERR_USB_CONNECT     0x15    /* ��⵽USB�豸�����¼�,�Ѿ����� */
#define ERR_USB_DISCON      0x16    /* ��⵽USB�豸�Ͽ��¼�,�Ѿ��Ͽ� */
#define ERR_USB_BUF_OVER    0x17    /* USB��������������������̫�໺������� */
#define ERR_USB_DISK_ERR    0x1F    /* USB�洢������ʧ��,�ڳ�ʼ��ʱ������USB�洢����֧��,�ڶ�д�����п����Ǵ����𻵻����Ѿ��Ͽ� */
#define ERR_USB_TRANSFER    0x20    /* NAK/STALL�ȸ����������0x20~0x2F */
#define ERR_USB_UNSUPPORT   0xFB
#define ERR_USB_UNKNOWN     0xFE
#define SetUsbSpeed( x )
// ��ȡ�豸������
UINT8C  SetupGetDevDescr[] = { USB_REQ_TYP_IN, USB_GET_DESCRIPTOR, 0x00, USB_DESCR_TYP_DEVICE, 0x00, 0x00, sizeof( USB_DEV_DESCR ), 0x00 };
// ��ȡ����������
UINT8C  SetupGetCfgDescr[] = { USB_REQ_TYP_IN, USB_GET_DESCRIPTOR, 0x00, USB_DESCR_TYP_CONFIG, 0x00, 0x00, 0x04, 0x00 };
// ����USB��ַ
UINT8C  SetupSetUsbAddr[] = { USB_REQ_TYP_OUT, USB_SET_ADDRESS, USB_DEVICE_ADDR, 0x00, 0x00, 0x00, 0x00, 0x00 };
// ����USB����
UINT8C  SetupSetUsbConfig[] = { USB_REQ_TYP_OUT, USB_SET_CONFIGURATION, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
// ����˵�STALL
UINT8C  SetupClrEndpStall[] = { USB_REQ_TYP_OUT | USB_REQ_RECIP_ENDP, USB_CLEAR_FEATURE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
UINT8X  UsbDevEndp0Size;    /* USB�豸�Ķ˵�0�������ߴ� */
//USB�豸�����Ϣ��,CH549���֧��2���豸
#define ROOT_DEV_DISCONNECT     0
#define ROOT_DEV_CONNECTED      1
#define ROOT_DEV_FAILED         2
#define ROOT_DEV_SUCCESS        3
UINT8X  RxBuffer[ MAX_PACKET_SIZE ] _at_ 0x0000 ;  // IN, must even address
UINT8X  TxBuffer[ MAX_PACKET_SIZE ] _at_ 0x0040 ;  // OUT, must even address
#define pSetupReq   ((PXUSB_SETUP_REQ)TxBuffer)
bit     FoundNewDev;
UINT8X  buf[100];                                  //���ȿ��Ը���Ӧ���Լ�ָ��
#pragma NOAREGS
/* ������״̬,�����������ʾ������벢ͣ�� */
void mStopIfError( UINT8 iError )
{
    if ( iError == ERR_SUCCESS )
    {
        return;    /* �����ɹ� */
    }
    printf( "Error: %02X\n", (UINT16)iError );  /* ��ʾ���� */
    /* ���������,Ӧ�÷����������Լ�CH554DiskStatus״̬,�������CH549DiskReady��ѯ��ǰU���Ƿ�����,���U���ѶϿ���ô�����µȴ�U�̲����ٲ���,
       ���������Ĵ�������:
       1������һ��CH549DiskReady,�ɹ����������,����Open,Read/Write��
       2�����CH549DiskReady���ɹ�,��ôǿ�н���ͷ��ʼ����(�ȴ�U�����ӣ�CH554DiskReady��) */
    while ( 1 )
    {
//      LED_TMP=0;  /* LED��˸ */
//      mDelaymS( 100 );
//      LED_TMP=1;
//      mDelaymS( 100 );
    }
}
void main( )
{
    UINT8   s, c,i;
    UINT16  TotalCount;
    CfgFsys();
    mDelaymS(20);                                                              //�޸���Ƶ���Լ���ʱ�ȴ���Ƶ�ȶ�
    mInitSTDIO( );                                                            //��ʼ������0Ϊ���ü����ͨ�����ڼ����ʾ����
    printf("start ...\n");
    InitUSB_Host( );
    CH549LibInit( );                                                          //��ʼ��CH549�������֧��U���ļ�
    FoundNewDev = 0;
    while ( 1 )
    {
        s = ERR_SUCCESS;
        if ( UIF_DETECT )                                                   // �����USB��������ж�����
        {
            UIF_DETECT = 0;                                                 // �������жϱ�־
            s = AnalyzeRootHub( );                                          // ����ROOT-HUB״̬
            if ( s == ERR_USB_CONNECT )
            {
                FoundNewDev = 1;
            }
        }
        if ( FoundNewDev || s == ERR_USB_CONNECT )                         // ���µ�USB�豸����
        {
            FoundNewDev = 0;
            mDelaymS( 200 );                                               // ����USB�豸�ղ�����δ�ȶ�,�ʵȴ�USB�豸���ٺ���,������ζ���
            s = InitRootDevice( );                                         // ��ʼ��USB�豸
            if ( s == ERR_SUCCESS )
            {
                // U�̲������̣�USB���߸�λ��U�����ӡ���ȡ�豸������������USB��ַ����ѡ�Ļ�ȡ������������֮�󵽴�˴�����CH549�ӳ���������ɺ�������
                CH549DiskStatus = DISK_USB_ADDR;
                for ( i = 0; i != 10; i ++ )
                {
                    printf( "Wait DiskReady\n" );
                    s = CH549DiskReady( );                                 //�ȴ�U��׼����
                    if ( s == ERR_SUCCESS )
                    {
                        break;
                    }
                    else
                    {
                        printf("%02x\n",(UINT16)s);
                    }
                    mDelaymS( 50 );
                }
                if ( CH549DiskStatus >= DISK_MOUNTED )
                {
//                  /* ���ļ� */
//                  strcpy( mCmdParam.Open.mPathName, "/C51/CH549HFT.C" );     //���ý�Ҫ�������ļ�·�����ļ���/C51/CH549HFT.C
//                  s = CH549FileOpen( );                                      //���ļ�
//                  if ( s == ERR_MISS_DIR || s == ERR_MISS_FILE ) {           //û���ҵ��ļ�
//                      printf( "û���ҵ��ļ�\n" );
//                  }
//                  else
//                  {                                                          //�ҵ��ļ����߳���
//                      TotalCount = 100;                                      //����׼����ȡ�ܳ���100�ֽ�
//                      printf( "������ǰ%d���ַ���:\n",TotalCount );
//                      while ( TotalCount ) {                                 //����ļ��Ƚϴ�,һ�ζ�����,�����ٵ���CH549ByteRead������ȡ,�ļ�ָ���Զ�����ƶ�
//                           if ( TotalCount > (MAX_PATH_LEN-1) ) c = MAX_PATH_LEN-1;/* ʣ�����ݽ϶�,���Ƶ��ζ�д�ĳ��Ȳ��ܳ��� sizeof( mCmdParam.Other.mBuffer ) */
//                           else c = TotalCount;                                 /* ���ʣ����ֽ��� */
//                           mCmdParam.ByteRead.mByteCount = c;                   /* ���������ʮ�ֽ����� */
//                           mCmdParam.ByteRead.mByteBuffer= &buf[0];
//                           s = CH549ByteRead( );                                /* ���ֽ�Ϊ��λ��ȡ���ݿ�,���ζ�д�ĳ��Ȳ��ܳ���MAX_BYTE_IO,�ڶ��ε���ʱ���Ÿղŵ����� */
//                           TotalCount -= mCmdParam.ByteRead.mByteCount;         /* ����,��ȥ��ǰʵ���Ѿ��������ַ��� */
//                           for ( i=0; i!=mCmdParam.ByteRead.mByteCount; i++ ) printf( "%C", mCmdParam.ByteRead.mByteBuffer[i] );  /* ��ʾ�������ַ� */
//                           if ( mCmdParam.ByteRead.mByteCount < c ) {           /* ʵ�ʶ������ַ�������Ҫ��������ַ���,˵���Ѿ����ļ��Ľ�β */
//                                printf( "\n" );
//                                printf( "�ļ��Ѿ�����\n" );
//                                break;
//                           }
//                      }
//                      printf( "Close\n" );
//                      i = CH549FileClose( );                                    /* �ر��ļ� */
//                      mStopIfError( i );
//                  }
//                  /*���ϣ����ָ��λ�ÿ�ʼ��д,�����ƶ��ļ�ָ�� */
//                  mCmdParam.ByteLocate.mByteOffset = 608;  //�����ļ���ǰ608���ֽڿ�ʼ��д
//                  CH549ByteLocate( );
//                  mCmdParam.ByteRead.mByteCount = 5;  //��ȡ5���ֽ�
//                  mCmdParam.ByteRead.mByteBuffer= &buf[0];
//                  CH549ByteRead( );   //ֱ�Ӷ�ȡ�ļ��ĵ�608���ֽڵ�612���ֽ�����,ǰ608���ֽڱ�����
//                  //���ϣ�������������ӵ�ԭ�ļ���β��,�����ƶ��ļ�ָ��
//                  CH549FileOpen( );
//                  mCmdParam.ByteLocate.mByteOffset = 0xffffffff;  //�Ƶ��ļ���β��
//                  CH549ByteLocate( );
//                  mCmdParam.ByteWrite.mByteCount = 13;  //д��13���ֽڵ�����
//                  CH549ByteWrite( );   //��ԭ�ļ��ĺ�����������,�¼ӵ�13���ֽڽ���ԭ�ļ���β������
//                  mCmdParam.ByteWrite.mByteCount = 2;  //д��2���ֽڵ�����
//                  CH549ByteWrite( );   //������ԭ�ļ��ĺ�����������
//                  mCmdParam.ByteWrite.mByteCount = 0;  //д��0���ֽڵ�����,ʵ���ϸò�������֪ͨ���������ļ�����
//                  CH549ByteWrite( );   //д��0�ֽڵ�����,�����Զ������ļ��ĳ���,�����ļ���������15,�����������,��ôִ��CH554FileCloseʱҲ���Զ������ļ�����
                    //�����ļ���ʾ
                    printf( "Create\n" );
                    strcpy( mCmdParam.Create.mPathName, "/NEWFILE.TXT" );          /* ���ļ���,�ڸ�Ŀ¼��,�����ļ��� */
                    s = CH549FileCreate( );                                        /* �½��ļ�����,����ļ��Ѿ���������ɾ�������½� */
                    mStopIfError( s );
                    printf( "ByteWrite\n" );
                    //ʵ��Ӧ���ж�д���ݳ��ȺͶ��建���������Ƿ������������ڻ�������������Ҫ���д��
                    i = sprintf( buf,"Note: \xd\xa������������ֽ�Ϊ��λ����U���ļ���д,549����ʾ���ܡ�\xd\xa");  /*��ʾ */
                    for(c=0; c<10; c++)
                    {
                        mCmdParam.ByteWrite.mByteCount = i;                          /* ָ������д����ֽ��� */
                        mCmdParam.ByteWrite.mByteBuffer = buf;                       /* ָ�򻺳��� */
                        s = CH549ByteWrite( );                                       /* ���ֽ�Ϊ��λ���ļ�д������ */
                        mStopIfError( s );
                        printf("�ɹ�д�� %02X��\n",(UINT16)c);
                    }
                    //��ʾ�޸��ļ�����
//                  printf( "Modify\n" );
//                  mCmdParam.Modify.mFileAttr = 0xff;   //�������: �µ��ļ�����,Ϊ0FFH���޸�
//                  mCmdParam.Modify.mFileTime = 0xffff;   //�������: �µ��ļ�ʱ��,Ϊ0FFFFH���޸�,ʹ���½��ļ�������Ĭ��ʱ��
//                  mCmdParam.Modify.mFileDate = MAKE_FILE_DATE( 2015, 5, 18 );  //�������: �µ��ļ�����: 2015.05.18
//                  mCmdParam.Modify.mFileSize = 0xffffffff;  // �������: �µ��ļ�����,���ֽ�Ϊ��λд�ļ�Ӧ���ɳ����ر��ļ�ʱ�Զ����³���,���Դ˴����޸�
//                  i = CH549FileModify( );   //�޸ĵ�ǰ�ļ�����Ϣ,�޸�����
//                  mStopIfError( i );
                    printf( "Close\n" );
                    mCmdParam.Close.mUpdateLen = 1;                                  /* �Զ������ļ�����,���ֽ�Ϊ��λд�ļ�,�����ó����ر��ļ��Ա��Զ������ļ����� */
                    i = CH549FileClose( );
                    mStopIfError( i );
//                  /* ɾ��ĳ�ļ� */
//                  printf( "Erase\n" );
//                  strcpy( mCmdParam.Create.mPathName, "/OLD" );  //����ɾ�����ļ���,�ڸ�Ŀ¼��
//                  i = CH549FileErase( );  //ɾ���ļ����ر�
//                  if ( i != ERR_SUCCESS ) printf( "Error: %02X\n", (UINT16)i );  //��ʾ����
                }
            }
        }
        mDelaymS( 100 );  // ģ�ⵥƬ����������
        SetUsbSpeed( 1 );  // Ĭ��Ϊȫ��
    }
}