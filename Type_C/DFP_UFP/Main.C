/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/27
* Description        : CH549 Type-Cʹ��
��ģʽ(DFP) ��֧�ּ��Typc-C�豸�Ĳ�Σ�֧���豸������ͷ�����,
              ֧��Ĭ�Ϲ��������1.5A��������Լ�3A�Ĺ����������;
��ģʽ(UFP) ������豸�������������ɼ����������������
                            Ĭ�Ϲ��������1.5A��������Լ�3A�������
DFP (Downstream Facing Port) Host��
UFP (Upstream Facing Port)   Dev��
ע�����DEBUG.C
*******************************************************************************/
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
#include "Type_C.H"
#pragma  NOAREGS
void main( )
{
    UINT16 j = 0;
    CfgFsys( );                                                          //CH549ʱ��ѡ������
    mDelaymS(20);                                                        //�޸���Ƶ�����Լ���ʱ�ȴ�оƬ�����ȶ�
    mInitSTDIO( );                                                       //����0��ʼ��
    printf("Type-C Demo start ...\n");
#ifdef TYPE_C_DFP                                                        //Type-C������������壬֪ͨ�豸������������
    TypeC_DFP_Init(3);                                                     //��������3A���������������
    while(1)
    {
        j = TypeC_DFP_Insert();
        if( j == UCC_DISCONNECT )
        {
            VBUS = 1;                                                          //�ر�VBUS
            printf("UFP disconnect...\n");
        }
        else if( j == UCC1_CONNECT )
        {
            VBUS = 0;                                                          //����VBUS
            printf("UFP CC1 insertion...\n");                                  //�������
        }
        else if( j == UCC2_CONNECT )
        {
            VBUS = 0;                                                          //����VBUS
            printf("UFP CC2 insertion...\n");                                  //�������
        }
        else if( j == UCC_CONNECT )
        {
            VBUS = 0;                                                          //����VBUS
            printf("UFP connect...\n");
        }
        mDelaymS(500);                                                       //��ʱ�����壬ģ�ⵥƬ��ִ����������
    }
#endif
#ifdef TYPE_C_UFP
    TypeC_UPF_PDInit();                                                    //UPF��ʼ��
    while(1)
    {
        j = TypeC_UPF_PDCheck();                                             //��������Ĺ�������
        if(j == UPF_PD_Normal)
        {
            printf("DFP defaultPower...\n");
        }
        if(j == UPF_PD_1A5)
        {
            printf("DFP 1.5A...\n");
        }
        if(j == UPF_PD_3A)
        {
            printf("DFP 3.0...\n");
        }
        if(j == UPD_PD_DISCONNECT)
        {
            printf("disconnect...\n");
        }
        mDelaymS( 500 );                                                     //��ʱ�����壬ģ�ⵥƬ��ִ����������
    }
#endif
}