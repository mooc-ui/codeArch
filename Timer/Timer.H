#ifndef __TIMER_H__
#define __TIMER_H__
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
#define TIMER0_RELOAD_VALUE 200
#define  T0_INT   1
//#define  T1_INT   1
//#define  T2_INT   1
#define  T2_CAP   1
/******************* CH549 Timer0��� ************************/
//bTMR_CLKͬʱӰ��Timer0&1&2,ʹ��ʱҪע�� (����ʱʹ�ñ�׼ʱ��)
#define mTimer0Clk12DivFsys( ) (T2MOD &= ~bT0_CLK)                          //��ʱ��,ʱ��=Fsys/12 T0��׼ʱ��
#define mTimer0ClkFsys( )      (T2MOD |= bTMR_CLK | bT0_CLK)                //��ʱ��,ʱ��=Fsys
#define mTimer0Clk4DivFsys( )  (T2MOD &= ~bTMR_CLK;T2MOD |=  bT0_CLK)       //��ʱ��,ʱ��=Fsys/4����ʱ��ͬ������DEBUG.C��mInitSTDIO��
#define mTimer0CountClk( )     (TMOD |= bT0_CT)                             //ѡ�����ڼ�����ģʽ��T0���ţ�P34�����½�����Ч��
#define mTimer0RunCTL( SS )    (TR0 = SS ? START : STOP)                    //CH549 Timer0 ��ʼ(SS=1)/����(SS=0)
/***** CH549 Timer1���(DEBUG.C��mInitSTDIOʹ��T1ģʽ2) *****/
#define mTimer1Clk12DivFsys( ) (T2MOD &= ~bT1_CLK)                          //��ʱ��,ʱ��=Fsys/12  T1��׼ʱ��
#define mTimer1ClkFsys( )      (T2MOD |= bTMR_CLK | bT1_CLK)                //��ʱ��,ʱ��=Fsys
#define mTimer1Clk4DivFsys( )  (T2MOD &= ~bTMR_CLK;T2MOD |=  bT1_CLK)       //��ʱ��,ʱ��=Fsys/4
#define mTimer1CountClk( )     (TMOD |= bT1_CT)                             //ѡ�����ڼ�����ģʽ��T1���ţ�P35�����½�����Ч��
#define mTimer1RunCTL( SS )    (TR1 = SS ? START : STOP)                    //CH549 Timer1 ��ʼ(SS=1)/����(SS=0)
/******************* CH549 Timer2��� ************************/
#define mTimer2Clk12DivFsys( ) {T2MOD &= ~ bT2_CLK;C_T2 = 0;}                  //��ʱ��,ʱ��=Fsys/12 T2��׼ʱ��
#define mTimer2ClkFsys( )      {T2MOD |= (bTMR_CLK | bT2_CLK);C_T2=0;}         //��ʱ��,ʱ��=Fsys
#define mTimer2Clk4DivFsys( )  {T2MOD &= ~bTMR_CLK;T2MOD |=  bT2_CLK;C_T2 = 0;}//��ʱ��,ʱ��=Fsys/4����ʱ��ͬ������DEBUG.C��mInitSTDIO��
#define mTimer2CountClk( )     {C_T2 = 1;}                                     //ѡ�����ڼ�����ģʽ��T2���ţ�P10�����½�����Ч��
#define mTimer2RunCTL( SS )    {TR2 = SS ? START : STOP;}                    //CH549 Timer2 ��ʼ(SS=1)/����(SS=0)
#define mTimer2OutCTL( )       (T2MOD |= T2OE)                               //T2���  Ƶ��TF2/2   
#define CAP1Alter( )           (PIN_FUNC |= bT2_PIN_X;)                      //CAP1��P10 ӳ�䵽P14
#define CAP2Alter( )           (PIN_FUNC |= bT2EX_PIN_X;)                    //CAP2��P11 ӳ��RST
/*******************************************************************************
* Function Name  : mTimer_x_ModInit(UINT8 x ,UINT8 mode)
* Description    : CH549��ʱ������xģʽ����
* Input          : UINT8 mode,Timerģʽѡ��
                   0��ģʽ0��13λ��ʱ����TLn�ĸ�3λ��Ч
                   1��ģʽ1��16λ��ʱ��
                   2��ģʽ2��8λ�Զ���װ��ʱ��
                   3��ģʽ3������8λ��ʱ��  Timer0
                   3��ģʽ3��Timer1ֹͣ
                   UINT8 x ��ʱ��  0 1 2
* Output         : None
* Return         : �ɹ�  SUCCESS
                   ʧ��  FAIL
*******************************************************************************/
UINT8 mTimer_x_ModInit(UINT8 x ,UINT8 mode);
/*******************************************************************************
* Function Name  : mTimer_x_SetData(UINT8 x,UINT16 dat)
* Description    : CH549Timer
* Input          : UINT16 dat;��ʱ����ֵ
                   UINT8 x ��ʱ��  0 1 2
* Output         : None
* Return         : None
*******************************************************************************/
void mTimer_x_SetData(UINT8 x,UINT16 dat);
/*******************************************************************************
* Function Name  : CAP2Init(UINT8 mode)
* Description    : CH549��ʱ������2 T2EX���Ų�׽���ܳ�ʼ����CAP2 P11��
                   UINT8 mode,���ز�׽ģʽѡ��
                   0:T2ex���½��ص���һ���½���
                   1:T2ex�������֮��
                   3:T2ex�������ص���һ��������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAP2Init(UINT8 mode);
/*******************************************************************************
* Function Name  : CAP1Init(UINT8 mode)
* Description    : CH549��ʱ������2 T2���Ų�׽���ܳ�ʼ��T2(CAP1 P10)
                   UINT8 mode,���ز�׽ģʽѡ��
                   0:T2ex���½��ص���һ���½���
                   1:T2ex�������֮��
                   3:T2ex�������ص���һ��������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAP1Init(UINT8 mode);
/*******************************************************************************
* Function Name  : CAP0Init(UINT8 mode)
* Description    : CH549��ʱ������2 CAP0���Ų�׽���ܳ�ʼ��(CAP0 P36)
                   UINT8 mode,���ز�׽ģʽѡ��
                   0:T2ex���½��ص���һ���½���
                   1:T2ex�������֮��
                   3:T2ex�������ص���һ��������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAP0Init(UINT8 mode);
void rotarySpeedMeasureResultClear(void);
UINT32 getSystemSecond(void);

//extern UINT16 Cap0[2] ;
#endif
