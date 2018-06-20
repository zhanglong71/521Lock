/*******************************************************************************
* File Name          : main.c
* Author             : HuNan HuaRain Science & Technology Co.Ltd, zhanglong
* Date First Issued  : 2012-7-25
* Description        : Adapter
********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_lib.h"														
#include "string.h"
#include "inc/CONST.h"
#include "inc/ctype.h"
#include "inc/global.h"
#include "inc/macro.h"
#include "src/init.h"  
#include "src/l_fingprint.h"
#include "src/arch.h"
//#include "src/bitmapOp.h"
#include "src/charQue.h"   
#include "src/driver.h"	
//#include "src/canTxQue.h"
//#include "src/wdg.h"
#include "main.h"  
/* Local includes ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

//RetStatus CAN_loop(void);

#include "src/func_idle.h"
/** global variable **/

fstack_t g_fstack;
msgq_t  g_msgq;
Timer_t g_timer[TIMER_NUM];		/** Լ��g_timer[0]������sysProcess()�У� g_timer[1]ֻ����sysProcess()�� **/

charBuf_queue_t g_com1TxQue;
charBuf_queue_t g_com2TxQue;

//int g_channel;
actionQueue_t g_actionQueue;
actionQueue_t g_promptQueue;
//actionQueue_t g_promptQueue;
/*********************************************
 * g_flag 
 * g_flag.4 = 1.TO״̬.������������.
 * g_flag.5 = 1.TI״̬.������������.
 * g_flag.6 = 1.finger״̬.ָ��ͷ����ָ����.
 * g_flag.7 = 1.��һ�δ���(����)���ְ���.
 * g_flag.8 = 1.�͵���. ADC�͵���������.
 * g_flag.9 = 1.�͵�ȷ��. 
 * g_flag.10 = 1. ��ָ��. ����1������ָ�Ʋ���ģʽ
 * g_flag.11 = 1. �յ�����Կ���ź�
 * g_flag.12 = 1. �������ü�
 * g_flag.13 = 1. ����busy�ϴε�ƽ
 * g_flag.14 = 1/0. �������ּ�
 *
 *********************************************/
unsigned char g_ucUserId = 0xff;		/** ��ǰ�û�id��Ҳ�ǹ�����id **/
unsigned char g_ucAddUserId = 0xff;		/** ��ӵ�id�ţ�Ҳ�ǹ�����id **/
u32 g_flag;
int g_tick;			/** ��ડ�һ����״̬ת��ʱ���� **/

int	g_iADCref = 0;
int g_iADCCurr = 0;

unsigned int g_subState;		/** ������״̬, ��δ�õ� **/
//unsigned char g_oldKey[10] = {0};
//key_t g_keys;

const char g_magic[MAGIC_SIZE] = {0x48, 0x55, 0x41, 0x52}; 		/** 'H', 'U', 'A', 'R'**/

//unsigned char g_flash[CFLASH_PAGE_SIZE];
flashPage_t g_flash;
/*******************************************************************************
* Function Name  : main
* Description    : Main program
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{
    msg_t msg;
    func_t func;
	int	i;

    RCC_Configuration();	/* System Clocks Configuration */
    NVIC_Configuration();	/* NVIC Configuration */
    GPIO_Configuration();	/* GPIO ports pins Configuration */

	/** ��ʼ����ͨ�ö˿�״̬ **/
	MMasterPower_off();	/** �����ƶϵ磬�ڼ�⵽��ض��������ȷ������ **/	
	MBlackLED_off();	/** ����ƣ�״̬δ֪-�� **/
	MFingerPower_off();	/** ָ��ͷ��״̬δ֪-�� **/
	//MOPGreen_off();		/** ��ɫָʾ�ƣ�״̬δ֪-�� **/
	MOPBlue_off();		/** ��ɫָʾ�ƣ�״̬δ֪-�� **/
	MOPRed_off();		/** ��ɫָʾ�ƣ�״̬δ֪-�� **/
	
	#if	CENABLEVOP			/** ������ʾ��״̬δ֪-??? **/
	VOPDATA_H();
	#else
	VOPDATA_L();
	#endif
	VOPRESET_H();
	
	MLockPWR_off();		/** ��������״̬δ֪-��ֹ **/
	MPKEPower_off();	/** ��Ӧ���磬״̬δ֪-�Ͽ� **/			

    USART_Configuration();    /* Configure the USART1 */
    TIM_Configuration();      /* Configure the TIM */
    EXTI_Configuration();			/** exti **/
	ADC_Configuration();
//    bitmap_clean(&bmpRcvCan);

	g_tick = 0;
	g_flag = 0;
//	g_keys.iCount = 0;
	
		#if	1
	/** ����У��Ͷ�ȡ������ȡĬ��ֵ **/
	//for(i = 0; i < CFLASH_PAGE_SIZE; i++)g_flash.arrChar[i] = 0xff;
	flashCache_init();
	if(memcmp((char *)CFLASHSTARTADDR, g_magic, MAGIC_SIZE) == 0)		/** flash���������� **/
	{	
		u32 *iPtr = (u32 *)CFLASHSTARTADDR;

	 	//flashWrite((u32 *)g_flash.arrInt);
		
		for(i = 0; i < (CFLASH_PAGE_SIZE / 4); i++)g_flash.arrInt[i] = iPtr[i];			/** data valid! read it first **/
	}
	else
	{
		flashWrite((u32 *)g_flash.arrInt);
		memcpy(g_flash.arrChar, g_magic, CFLASH_PAGE_SIZE);						/** data valid! read it first **/
	}
		#endif

    charQueueInit(&g_com1TxQue);
    charQueueInit(&g_com2TxQue);
    actionQueueInit(&g_actionQueue, &(g_timer[1]), CACT_TOUT, CACT_OVER);
    actionQueueInit(&g_promptQueue, &(g_timer[2]), CPMT_TOUT, CPMT_OVER);
    
    init_fstack(&g_fstack);
    init_queue(&g_msgq);
    
    func.func = func_idle;
    stackpush(&g_fstack, &func);

    for(i = 0; i < TIMER_NUM; i++)
    {
        ClrTimer(&g_timer[i]);
    }
    
    SetTimer(&g_timer[0], TIMER_1SEC, CMSG_TMR);

    /** WDG **/
    //wdg_init();
    /**
    * Now all initial ok. then enable interrupt
    **/
	msg.msgType = CMSG_INIT;
    inq(&g_msgq, &msg);
	
    IRQ_enable();
	wdg_init();

    while(1)
    {
	    wdg_feed();
        keyscan();
        fingerCheck();
        noKeyEnter();
        vop_busy();
        DAEMON_USARTx_Send(USART1, &g_com1TxQue);   /** output for debug **/
        DAEMON_USARTx_Send(USART2, &g_com2TxQue);
        actionDoing(&g_actionQueue);
        actionDoing(&g_promptQueue);
        //DAEMON_CAN_Send();
        
        if(outq_irq(&g_msgq, &msg) == FALSE)    /** ����Ϣ��? **/
        {
            continue;
        }
        if(sysProcess(&msg) == TRUE)    /** ��ϵͳ��Ϣ��? **/
        {
            continue;
        }
        if(stacktop(&g_fstack, &func) == FALSE)     /** ��ǰ���ڹ���״̬��? **/
        {
        	/** something wrong happend, Power Down or recover it **/
        	#if	0
    			MMasterPower_off();
    		#else
        		func.func = func_idle;
    			stackpush(&g_fstack, &func);
    			g_tick = 0;
    			
    			SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
				
            	continue;
    		#endif
        }

        func.func((unsigned *)&msg);
    }
}


#ifdef  DEBUG
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
