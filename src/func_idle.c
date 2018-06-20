
#include "stm32f10x_lib.h"
#include "string.h"
#include "../inc/CONST.h"
#include "../inc/ctype.h" 
#include "../inc/macro.h" 
#include "../inc/global.h" 
#include "../inc/debug.h"
#include "arch.h"
#include "driver.h"
#include "l_fingprint.h"
#include "func_idle.h"
#include "f_Unlock.h"
#include "f_finger.h"		  
#include "f_nokey.h"			  
#include "f_key.h"	
#include "charQue.h"	 
#include "main.h"	
 
/*******************************************************************************
 * local function
 *******************************************************************************/
int func_idle(unsigned *pMsg)
{
	func_t func;
	//msg_t msg;
//	int i;

//	static unsigned char  ucTmp = 0;
	charData_t charData;

	//msg.msgType = ((msg_t *)pMsg)->msgType;
	//msg.msgValue = ((msg_t *)pMsg)->msgValue;

    switch(((msg_t *)pMsg)->msgType) 
    //switch(msg.msgType)
	{
	case CACT_OVER:
		actionInit();
		break;

	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10)	/** ���������ȴ�ʱ��, ���Ǽ��ָ��ģ��Ϊ�յ�����µ������תʱ�� **/
		{
			MPKEPower_off();			/** ����Կ�׹��ܶϵ� **/
			MBlackLED_off();			/** ������� **/
			MFingerPower_off();			/** ָ��ͷ�ϵ� **/
			MOPBlue_off();
			MOPRed_off();
			MMasterPower_off();			/** ������IC�ϵ� **/
			
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_TMR);
		}
		
		charData.ucVal = g_tick;
		charQueueIn_irq(&g_com1TxQue, &charData); 
		//charQueueIn_irq(&g_com2TxQue, &charData);

		#if	0
			AD_value = ADC_GetConversionValue(ADC1);
			charData.ucVal = AD_value & 0xff;
			charQueueIn_irq(&g_com1TxQue, &charData); 
			charQueueIn_irq(&g_com2TxQue, &charData);
			charData.ucVal = (AD_value >> 8) & 0xff;
			charQueueIn_irq(&g_com1TxQue, &charData); 
			charQueueIn_irq(&g_com2TxQue, &charData);
		#endif
		break;
		
	case CMSG_INIT:		/** nothing to be done **/
		g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		break;
	case CNOKEY_ENTER:
		#if	0
		/** ������Կ�׿���״̬ **/
			init_fstack(&g_fstack);
    		func.func = f_nokeyEnter;
    		stackpush(&g_fstack, &func);
    		
			g_tick = 0;
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
    	#else
    		/** ң�ؽ��� **/
    		MMasterPower_on();			/** ������IC���� **/
    		init_fstack(&g_fstack);
    		func.func = f_remote;
    		stackpush(&g_fstack, &func);
    	
			g_tick = 0;
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
    	#endif
		break;
		
	case CMSG_BACK:
		MMasterPower_on();			/** ������IC���� **/
		MBlackLED_on();			/** ���� **/
		MOPBlue_on();			/** �뱳��һ���ָʾ�� **/
		
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
    	
    	if(fingerTab_isEmpty()) {			/** ָ��ģ��գ�ֱ�ӿ��� **/
			init_fstack(&g_fstack);
			func.func = f_keyFirstTouch;
    		stackpush(&g_fstack, &func);
		} else {
			if(MFP_isInductionPower()) {	/** ָ��ͷ�и�Ӧ�ϵ繦��ʱ�������������뿪��״̬ **/
				init_fstack(&g_fstack);
				func.func = f_key;
    			stackpush(&g_fstack, &func);
    		} else {
    			/**  ָ��ͷû�и�Ӧ�ϵ繦��ʱ�����ְ������Ƚ���ָ�ƿ���״̬����ָ�ƿ���״̬������а������²Ž������뿪��״̬  **/
    			goto CAddrFinger_on;
    		}
		}
		break;	
		
	case CFIGER_ON:		/** ��⵽ָ�ư��� **/
	  CAddrFinger_on:
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		
    	if(fingerTab_isEmpty())	{ /** ָ��ģ��գ�ֱ�ӿ��� **/
			MMasterPower_on();			/** ������IC���� **/
			init_fstack(&g_fstack);
			func.func = f_keyFirstTouch;
    		stackpush(&g_fstack, &func);
		} else {
			MMasterPower_on();			/** ������IC���� **/
			MFingerPower_on();			/** ��ָ��ͷ���� **/
        	
			/** ����ָ�Ʊȶ�״̬ **/
			init_fstack(&g_fstack);
    		func.func = f_finger;
    		stackpush(&g_fstack, &func);
    	}
		break;

	case CMSG_DKEY:							/** ��⵽�������� **/
		switch(((msg_t *)pMsg)->msgValue)
		{	
		case CKEY_GOOUT:				/** �����ڰ��֣�ֱ�ӿ��� **/
			{
				MMasterPower_on();			/** ������IC���� **/
				#if	1
    			SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
				g_subState = 0;
				init_fstack(&g_fstack);
				func.func = f_KEYToOut;
    			stackpush(&g_fstack, &func);
				#endif
			}
    		break;
    		
		case CKEY_SET:				/** �������� **/
			{
				MMasterPower_on();			/** ������IC���� **/
				#if	1
    			SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
				g_subState = 0;
				init_fstack(&g_fstack);
				func.func = f_KEYSET;
    			stackpush(&g_fstack, &func);
				#endif
			}
    		break;
		
		case CKEY_GOIN:					/** ��������֣�������Կ�� **/
			{
				if(fingerTab_isEmpty())	/** ָ��ģ��գ�ֱ�ӽ��뿪������ **/
				{
					#if	1
					MMasterPower_on();
					lockAction();
    				SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
					#endif
				}
				else
				{
					MMasterPower_on();		/** ������IC���� **/
					MPKEPower_on();			/** ����Կ�׹��ܹ��� **/
					g_tick = 0;					/** ͬʱ��ʼ���� **/
    				SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
				}
			}
			break;
			
			#if	0
		case CKEY_1:			/** ����ָ�Ʋ���ģʽ **/
		case CKEY_2:			/** �����������ģʽ **/
		case CKEY_3:	
		case CKEY_4:	
		case CKEY_5:	
		case CKEY_6:	
		case CKEY_7:	
		case CKEY_8:	
		case CKEY_9:	
		case CKEY_asterisk:		/** *-asterisk **/
		case CKEY_0:		
		case CKEY_pound:		/** #-pound **/
			{
				actionInit();
				promptInit();
				beep();
				MBlackLED_on();				/** ������� **/
				MMasterPower_on();			/** ������IC���� **/
				g_tick = 0;					/** ͬʱ��ʼ���� **/
    			SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
				
			}	
			break;
			#endif
			
		default:					/** ���ܵĴ��� **/
			break;
		}
		break;

	default:
		/** δ֪ **/
			
		break;
	}  

    return  0;
}
/////////////////////////////////////////////////////

