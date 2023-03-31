#ifndef __CH549_IAP_H__
#define __CH549_IAP_H__
#define        Uart_Sync_Head1      0x57
#define        Uart_Sync_Head2      0xab
/*����ΪIAP���������*/
#define        CMD_IAP_PROM         0x80        // ISP�������
#define        CMD_IAP_ERASE        0x81        // ISP��������
#define        CMD_IAP_VERIFY       0x82        // ISPУ������
#define        CMD_IAP_END          0x83        // ISP������־
/********************************************************************************/
/*
  �����ʽ˵��:
 1, CMD_IAP_END(IAP������־):
    �������ʽ����:
        CMD_IAP_END+1�ֽ����ݳ���+2�ֽ�״̬��������.
    ��������:
        ��
 2, CMD_IAP_VERIFY(IAPУ������):
     �������ʽ����:
         CMD_IAP_VERIFY+1�ֽ����ݳ���+2�ֽ����ݵ�ַ+��������.
    ��������:
        ��ȷ:0x0000
        ����:0x0001
 3, CMD_IAP_PROM(IAP�������):
     �������ʽ����:
         CMD_IAP_PROM+1�ֽ����ݳ���+2�ֽ����ݵ�ַ+��������.ע���������������ݸ�������Ϊ4�ֽڵ�������
     ��������:
        ��ȷ:0x0000
        ����:0x0001
 4, CMD_IAP_ERASE(IAP��������):
     �������ʽ����:
        CMD_IAP_ERASE+1�ֽ����ݳ���+2�ֽڿ�ʼ��ַ��һ��Ϊ0x0000����������ֻ��Ҫ����һ��
    ��������:
        ��ȷ:0x0000
        ����:0x0001
*/
#define     IAP_LEN            64
/* IAP���س���ṹ�� */
typedef union _IAP_CMD
{
    struct
    {
        UINT8    cmd;                   /* ������ 0x81 */
        UINT8    len;                   /* �������ݳ��� */
        UINT8    addr[2];               /* ������ַ */
    } erase;                            /* �������� */
    struct
    {
        UINT8    cmd;                   /* ������ 0x83 */
        UINT8    len;                   /* �������ݳ��� */
        UINT8    status[2];             /* ���ֽ�״̬������ */
    } end;                              /* �������� */
    struct
    {
        UINT8    cmd;                   /* ������ 0x82 */
        UINT8    len;                   /* �������ݳ��� */
        UINT8    addr[2];               /* У���ַ */
        UINT8    buf[IAP_LEN-4];        /* У������ */
    } verify;                           /* У������ */
    struct
    {
        UINT8    cmd;                   /* ������ 0x80 */
        UINT8    len;                   /* �������ݳ��� */
        UINT8    addr[2];               /* ��ַ */
        UINT8    buf[IAP_LEN-4];        /* �������� */
    } program;                          /* ������� */
    struct
    {
        UINT8    buf[IAP_LEN];          /* �������ݰ�*/
    } other;
} iap_cmd1;
#endif