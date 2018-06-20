
#include "stm32f10x_lib.h"
#include "string.h"
#include "../inc/CONST.h"
#include "../inc/ctype.h" 
#include "../inc/macro.h" 
#include "../inc/global.h" 
#include "../inc/debug.h"

#include "func_idle.h"
#include "f_Unlock.h"
#include "arch.h"
#include "charQue.h"
#include "driver.h"	 
#include "main.h"	
 
/*******************************************************************************
 * no key enter
 * ��Կ�׽���
 *******************************************************************************/
int f_nokeyEnter(unsigned *pMsg)
{
//	msg_t msg;
    func_t func;

//	msg.msgType = ((msg_t *)pMsg)->msgType;
//	msg.msgValue = ((msg_t *)pMsg)->msgValue;

    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_TMR:		/** �͵籨����ʾ��ʱ **/
		g_tick++;
		if(g_tick > 10)
		{
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
    		g_tick = 0;
			SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
			
   	    	MLockPWR_off();
   	    	MPKEPower_off();	/** ��Ӧ��·�ϵ� **/
   			MBlackLED_off();	/** ������� **/
			MFingerPower_off();	/** ָ��ͷ�ϵ� **/
			MMasterPower_off();	/** ���ϵ� **/
		}
		break;
		
	case CMSG_INIT:
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
    	promptDelay(TIMER_100MS);	/** ���ڴ˼�����ʾ���������Ҫ **/
		break;

	case CPMT_OVER:					/** ��ʾ������������ **/
    	promptInit();
    	lockAction();				/** ���� **/
		break;
			
	case CACT_OVER:					/** ����������̽��� **/
    	actionInit();
    	init_fstack(&g_fstack);
		func.func = f_Unlock;
    	stackpush(&g_fstack, &func);
    	g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		break;
		
	default:
		break;
	}
    return  0;
}

/////////////////////////////////////////////////////
/*******************************************************************************
 * ң�ز���
 *******************************************************************************/
int f_remote(unsigned *pMsg)
{
    func_t func;
    
    switch(((msg_t *)pMsg)->msgType)
	{	
	case CMSG_TMR:		/** �͵籨����ʾ��ʱ **/
		g_tick++;
		if(g_tick > 10)
		{
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
    		g_tick = 0;
			SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		break;
		
	case CMSG_INIT:
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
    	beep();
    	promptDelay(TIMER_200MS);	/** ���ڴ˼�����ʾ���������Ҫ(��ΪNY3P087��,����TIMER_100MS������������ԭ���� **/
		break;

	case CPMT_OVER:					/** ��ʾ������������ **/
    	promptInit();
    	lockAction();				/** ���� **/
		break;
			
	case CACT_OVER:					/** ����������̽��� **/
    	actionInit();
    	init_fstack(&g_fstack);
		func.func = f_Unlock;
    	stackpush(&g_fstack, &func);
    	g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		break;
			
	default:
		break;
	}
	return 0;
}
