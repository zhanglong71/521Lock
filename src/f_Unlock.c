
#include "stm32f10x_lib.h"
#include "string.h"
#include "../inc/CONST.h"
#include "../inc/ctype.h" 
#include "../inc/macro.h" 
#include "../inc/global.h" 
#include "../inc/debug.h"

#include "l_fingprint.h"
#include "func_idle.h"
#include "f_Unlock.h"
#include "arch.h"
#include "charQue.h"
#include "driver.h"	 
#include "main.h"	
 
/*******************************************************************************
 * Description: �����������
 * �͵籨��
 *******************************************************************************/
 
 //#define	CMOTORACT	(4)		/**���ת��ʱ�䳤��**/
 //#define	CMOTORPAUSE (15)	/**�����תǰ�ĵȴ�ʱ�䳤��**/
int f_Unlock(unsigned *pMsg)
{
//	msg_t msg;
    func_t func;
	int iTmp = 0;

//	msg.msgType = ((msg_t *)pMsg)->msgType;
//	msg.msgValue = ((msg_t *)pMsg)->msgValue;

    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_TMR:		/** �͵籨����ʾ��ʱ **/
		g_tick++;
		if(g_tick >= 10)
		{
		GOTO_ALLOFF:
			init_fstack(&g_fstack);
			func.func = func_idle;
    		stackpush(&g_fstack, &func);
    		g_tick = 0;
			SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
			
			/*******************************************************************/
    		iTmp = resetOP_getValue();	/** �ָ�����ֵ��� **/
    		if(iTmp != 0xa0)
    		{
				resetOP_setValue(0xa0);
				flashWrite(g_flash.arrInt);
    		}
    		/*******************************************************************/
			
   	    	MLockPWR_off();
   	    	MPKEPower_off();	/** ��Ӧ��·�ϵ� **/
   			MBlackLED_off();	/** ������� **/
   			MOPBlue_off();		/** ����ָʾ���� **/
   			MOPRed_off();		/** ����ָʾ���� **/
			MFingerPower_off();	/** ָ��ͷ�ϵ� **/
			MMasterPower_off();	/** ���ϵ� **/
		}
		break;
		
	case CMSG_INIT:
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
    	if(g_flag & (1 << 9))	/** ��͵籨�� **/
    	{
    		/** ������ʾ **/
    		promptInit();
    		MOPBlue_setDefaultStatus();		/** �ָ�ָʾ���뱳��һ�� **/
    		//bbbeep();
    		vp_stor(CVOPID_POWERLOW);
    		promptDelay(TIMER_500MS);
    	}
    	else
    	{
    		goto GOTO_ALLOFF;				/** �ޱ���������ֱ�ӽ��� **/
    	}
		break;

	case CPMT_OVER:							/** ����������̽��� **/
    	promptInit();
    	bbbeep();
    	promptDelay(TIMER_500MS);
		break;
			
	default:
		
		break;
	}
    return  0;
}
/*******************************************************************************
 * Description: ��������ϵ�����
 * 
 *******************************************************************************/
int f_UnlockFail(unsigned *pMsg)
{
	func_t func;
	msg_t msg;
   	switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_TMR:
		if(g_tick < 1)
		{
		}
		else 
		{
			init_fstack(&g_fstack);
			func.func = func_idle;
    		stackpush(&g_fstack, &func);
    		
			msg.msgType = CMSG_INIT;
			inq_irq(&g_msgq, &msg);
			g_tick = 0;
    		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
			
			MLockPWR_off();		/** �ȴ� **/
			MBlackLED_off();	/** ������� **/
   			MOPBlue_off();		/** ����ָʾ���� **/
   			MOPRed_off();		/** ����ָʾ���� **/
		    MFingerPower_off();	/** ָ��ͷ�ϵ� **/
			MPKEPower_off();	/** ��Ӧ��·�ϵ� **/
			MMasterPower_off();	/** ���ϵ� **/
		}
		g_tick++;
		break;
		
	case CMSG_INIT:
		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_TMR);
		break;
	
	default:
		break;
	}
    
    return  0;	
}
/////////////////////////////////////////////////////

