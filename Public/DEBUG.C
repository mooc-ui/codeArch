/********************************** (C) COPYRIGHT *******************************
* File Name          : DEBUG.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/09/20
* Description        : CH5XX DEBUG Interface
                     (1)����Ƶ����;
                     (2)��us\ms������ʱ����;										 
                     (3)������0�����ӡ��Ϣ�������ʿɵ�;            				   
                     (4)�����Ź���ʼ���͸�ֵ����;    										 
*******************************************************************************/

#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"

/*******************************************************************************
* Function Name  : CfgFsys( )
* Description    : CH5XXʱ��ѡ������ú���,Ĭ��ʹ���ڲ�����24MHz�����������FREQ_SYS����
                   ����PLL_CFG��CLOCK_CFG���õõ�����ʽ���£�
                   Fsys = (Fosc * ( PLL_CFG & MASK_PLL_MULT ))/(CLOCK_CFG & MASK_SYS_CK_DIV);
                   ����ʱ����Ҫ�Լ�����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/ 
void CfgFsys( )  
{
#if OSC_EN_XT	
		SAFE_MOD = 0x55;
		SAFE_MOD = 0xAA;
    CLOCK_CFG |= bOSC_EN_XT;                          //ʹ���ⲿ����
    CLOCK_CFG &= ~bOSC_EN_INT;                        //�ر��ڲ����� 
#else
		SAFE_MOD = 0x55;
		SAFE_MOD = 0xAA;
    CLOCK_CFG |= bOSC_EN_INT;                        //ʹ���ڲ����� 
    CLOCK_CFG &= ~bOSC_EN_XT;                        //�ر��ⲿ����	
#endif	
		SAFE_MOD = 0x55;
		SAFE_MOD = 0xAA;
#if FREQ_SYS == 48000000	
		CLOCK_CFG = CLOCK_CFG & ~ MASK_SYS_CK_SEL | 0x07;  // 48MHz	
#endif	
#if FREQ_SYS == 32000000	
		CLOCK_CFG = CLOCK_CFG & ~ MASK_SYS_CK_SEL | 0x06;  // 32MHz	
#endif	
#if FREQ_SYS == 24000000		
		CLOCK_CFG = CLOCK_CFG & ~ MASK_SYS_CK_SEL | 0x05;  // 24MHz	
#endif
#if FREQ_SYS == 16000000		
		CLOCK_CFG = CLOCK_CFG & ~ MASK_SYS_CK_SEL | 0x04;  // 16MHz
#endif	
#if FREQ_SYS == 12000000		
		CLOCK_CFG = CLOCK_CFG & ~ MASK_SYS_CK_SEL | 0x03;  // 12MHz	
#endif	
#if FREQ_SYS == 3000000	
		CLOCK_CFG = CLOCK_CFG & ~ MASK_SYS_CK_SEL | 0x02;  // 3MHz	
#endif
#if FREQ_SYS == 750000	
		CLOCK_CFG = CLOCK_CFG & ~ MASK_SYS_CK_SEL | 0x01;  // 750KHz
#endif
#if FREQ_SYS == 187500	
		CLOCK_CFG = CLOCK_CFG & ~ MASK_SYS_CK_SEL | 0x00;  // 187.5KHz	
#endif
		SAFE_MOD = 0x00;
}

/*******************************************************************************
* Function Name  : mDelayus(UNIT16 n)
* Description    : us��ʱ����
* Input          : UNIT16 n
* Output         : None
* Return         : None
*******************************************************************************/ 
void mDelayuS( UINT16 n )  // ��uSΪ��λ��ʱ
{
	while ( n ) {  // total = 12~13 Fsys cycles, 1uS @Fsys=12MHz
		++ SAFE_MOD;  // 2 Fsys cycles, for higher Fsys, add operation here
#ifdef	FREQ_SYS
#if		FREQ_SYS >= 14000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 16000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 18000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 20000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 22000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 24000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 26000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 28000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 30000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 32000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 34000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 36000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 38000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 40000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 42000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 44000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 46000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 48000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 50000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 52000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 54000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 56000000
		++ SAFE_MOD;
#endif
#endif
		-- n;
	}
}

/*******************************************************************************
* Function Name  : mDelayms(UNIT16 n)
* Description    : ms��ʱ����
* Input          : UNIT16 n
* Output         : None
* Return         : None
*******************************************************************************/
void mDelaymS( UINT16 n )                                                  // ��mSΪ��λ��ʱ
{
	while ( n ) 
	{
		mDelayuS( 1000 );
		-- n;
	}
}                                         

/*******************************************************************************
* Function Name  : CH549UART0Alter()
* Description    : CH549����0����ӳ��,����ӳ�䵽P0.2(R)��P0.3(T)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH549UART0Alter()
{
	P0_MOD_OC |= (3<<2);                                                   //׼˫��ģʽ
	P0_DIR_PU |= (3<<2);
	PIN_FUNC |= bUART0_PIN_X;                                              //�������Ÿ��ù���
}

/*******************************************************************************
* Function Name  : mInitSTDIO()
* Description    : CH559����0��ʼ��,Ĭ��ʹ��T1��UART0�Ĳ����ʷ�����,Ҳ����ʹ��T2
                   ��Ϊ�����ʷ�����
* Input          : None
* Output         : None+
* Return         : None
*******************************************************************************/
void mInitSTDIO( )
{
    UINT32 x;
    UINT8 x2; 

    SM0 = 0;
    SM1 = 1;
    SM2 = 0;                                                                   //����0ʹ��ģʽ1
                                                                               //ʹ��Timer1��Ϊ�����ʷ�����
    RCLK = 0;                                                                  //UART0����ʱ��
    TCLK = 0;                                                                  //UART0����ʱ��
    PCON |= SMOD;
    x = 10 * FREQ_SYS / UART0BUAD / 16;                                        //���������Ƶ��ע��x��ֵ��Ҫ���                            
    x2 = x % 10;
    x /= 10;
    if ( x2 >= 5 ) x ++;                                                       //��������

    TMOD = TMOD & ~ bT1_GATE & ~ bT1_CT & ~ MASK_T1_MOD | bT1_M1;              //0X20��Timer1��Ϊ8λ�Զ����ض�ʱ��
    T2MOD = T2MOD | bTMR_CLK | bT1_CLK;                                        //Timer1ʱ��ѡ��
    TH1 = 0-x;                                                                 //12MHz����,buad/12Ϊʵ�������ò�����
    TR1 = 1;                                                                   //������ʱ��1
    TI = 1;//
		//TI = 0;//
    REN = 1;                                                                   //����0����ʹ��
		
		ES = 1;//enable uart0 interrupt
		//EA=1; //open all interrupt
}

 /*******************************************************************************
 * Function Name  : CH549SoftReset()
 * Description    : CH549����λ
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void CH549SoftReset( )
{
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG |= bSW_RESET;
}
/*******************************************************************************
* Function Name  : CH549WDTModeSelect(UINT8 mode)
* Description    : CH549���Ź�ģʽѡ��
*                  8λ��������������ڣ��룩�� (131072/FREQ_SYS)*��256-WDOG_COUNT��
* Input          : UINT8 mode 
                   0  timer
                   1  watchDog
* Output         : None
* Return         : None
*******************************************************************************/
void CH549WDTModeSelect(UINT8 mode)
{
   SAFE_MOD = 0x55;
   SAFE_MOD = 0xaa;                                                             //���밲ȫģʽ
   if(mode){
     GLOBAL_CFG |= bWDOG_EN;                                                    //�������Ź���λ
   }
   else GLOBAL_CFG &= ~bWDOG_EN;	                                              //�������Ź�������Ϊ��ʱ��
   SAFE_MOD = 0x00;                                                             //�˳���ȫģʽ
   WDOG_COUNT = 0;                                                              //���Ź�����ֵ
}

/*******************************************************************************
* Function Name  : CH549WDTFeed(UINT8 tim)
* Description    : CH549���Ź���ʱʱ������
* Input          : UINT8 tim ���Ź���λʱ������
                   00H(12MHz)=2.8s     00H(24MHz)=1.4s
                   80H(12MHz)=1.4s     80H(24MHz)=0.7s
* Output         : None
* Return         : None
*******************************************************************************/
void CH549WDTFeed(UINT8 tim)
{
   WDOG_COUNT = tim;                                                             //���Ź���������ֵ	
}


