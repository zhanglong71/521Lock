
#include "stm32f10x_lib.h"
#include "string.h"
#include "../inc/CONST.h"
#include "../inc/ctype.h" 
#include "../inc/macro.h" 
#include "../inc/global.h" 
#include "../inc/debug.h"
#include "arch.h"		  
#include "charQue.h"	  
#include "driver.h"	 
#include "main.h"	
#include "func_idle.h"
#include "f_Unlock.h"	  
#include "f_finger.h"		  
#include "f_key.h"			  
#include "f_test.h"	
#include "l_fingprint.h"
 
/*******************************************************************************
 * f_key
 * ֻ����һ��beep��ʾ������ɺ������һ������
 * ����а������£�����������һ������
 *******************************************************************************/
int f_key(unsigned *pMsg)
{
	msg_t msg;
	func_t func;

	msg_init(&msg);
	msg.msgType = ((msg_t *)pMsg)->msgType;
	msg.msgValue = ((msg_t *)pMsg)->msgValue;
	
    switch(msg.msgType)
	{
	case CMSG_INIT:
		if(getpassword_errno() >= CMAX_PSERR)		//��������б仯
    	{
    		init_fstack(&g_fstack);
			func.func = f_keyFuncInvalid;
    		stackpush(&g_fstack, &func);
    		
			g_tick = 0;
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
    	}
		else
		{
			promptDelay(TIMER_100MS);
    		vp_stor(CVOPID_PLEASEINPUTPSWORD);
    	
			g_tick = 0;
    		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
    	}
		break;
			
	case CPMT_OVER:
		promptInit();
		
		init_fstack(&g_fstack);
		func.func = f_keyInput;
    	stackpush(&g_fstack, &func);
    
    	g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		break;
		
	case CMSG_DKEY:					/** ��⵽�������� **/
		promptInit();
		switch(msg.msgValue)
		{
		case CKEY_GOIN:			/** ��������֣��޹�������� **/
			break;
			
		case CKEY_GOOUT:			/** �����ڰ���, �����������ѡ��״̬ **/
			init_fstack(&g_fstack);
			func.func = f_KEYToOut;
    		stackpush(&g_fstack, &func);
			//beep();				/** ԭ���������ڰ���û��beep�����������ǹ���״̬����Ҫ��ʾ������Ч�� **/
			g_tick = 0;
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
			break;
		
		case CKEY_1:			/** �������뿪��ģʽ **/
		case CKEY_2:	
		case CKEY_3:	
		case CKEY_4:	
		case CKEY_5:	
		case CKEY_6:	
		case CKEY_7:	
		case CKEY_8:	
		case CKEY_9:	
		case CKEY_asterisk:	
		case CKEY_0:	
		case CKEY_pound:
			{
				/** �������ְ������������뿪������ **/
				init_fstack(&g_fstack);
				func.func = f_keyInput;
    			stackpush(&g_fstack, &func);
    			
    			inq_irq(&g_msgq, &msg);		/** ����һ��״̬������ʱ��ֵ�ط�һ�� **/
    		}
			break;
		default:
			break;
		}
		break;
			
	case CMSG_TMR:
		g_tick++;		
	default:
		break;
	}
    
    return  0;
}

/*******************************************************************************
 * f_keyFuncInvalid
 * ���뿪������(������)��Ч
 * ֻ��Ӧbeep��ʾ��,ֻ����ʾ��Ӱ��״̬ת��
 *******************************************************************************/
int f_keyFuncInvalid(unsigned *pMsg)
{
	func_t func;

    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_INIT:
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
    	
		promptInit();
    	beep();
    	vp_stor(CVOPID_PSWORD);
    	vp_stor(CVOPID_INVALID);
    	
    	bbbeep();
		ledWarn(TIMER_100MS);		/** ������˸ **/
		break;

	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10)
		{
			goto GOTO_FUNCOVER;
		}
		break;
		
	case CPMT_OVER:
	GOTO_FUNCOVER:
		promptInit();
		//MOPBlue_on();			/** ��˸֮��ָ����� **/		
		MOPBlue_setDefaultStatus();	/** �ָ�ָʾ���뱳��һ�� **/
		
		init_fstack(&g_fstack);
		func.func = f_Unlock;
    	stackpush(&g_fstack, &func);
    	
    	g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		break;
					
	default:
		break;
	}
    return  0;
}


/*******************************************************************************
 * �������
 * �鰴��(3,4)�ж���������������¼�뻹������ɾ��״̬
 *******************************************************************************/
func_t g_next_func;
#if	0
int f_keyAdminSelect(unsigned *pMsg)
{
//	int i = 0;
	msg_t msg;
	func_t func;
	
	msg_init(&msg);
	msg.msgType = ((msg_t *)pMsg)->msgType;
	msg.msgValue = ((msg_t *)pMsg)->msgValue;

    switch(msg.msgType)
	{	
	case CMSG_INIT:
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		beep();
		break;
		
	case CPMT_OVER:
		promptInit();
		break;
	
	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10)				/** ѡ����̳�ʱ�˳� **/
		{
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
    		
			g_tick = 0;
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		break;	
	case CMSG_DKEY:					/** ��⵽�������� **/
		{
			switch(msg.msgValue)
			{
			case CKEY_GOIN:				/** ��������֣��޹�������� **/
				break;
				
			case CKEY_GOOUT:
				/** �����ڰ���, �˳� **/
				init_fstack(&g_fstack);
				func.func = f_Unlock;
    			stackpush(&g_fstack, &func);
				g_tick = 0;
    			SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
				break;
				
			case CKEY_3:
				g_next_func.func  = f_keyAdminRgstPSWord;
				init_fstack(&g_fstack);
				func.func = f_keyAdminAuthen;
    			stackpush(&g_fstack, &func);
				g_tick = 0;
    			SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
				break;
				
			case CKEY_4:
				g_next_func.func  = f_keyAdminDergstPSWord;
				init_fstack(&g_fstack);
				func.func = f_keyAdminAuthen;
    			stackpush(&g_fstack, &func);
				g_tick = 0;
    			SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
				break;
				
			case CKEY_1:		/** ������������� **/
			case CKEY_2:
			case CKEY_5:	
			case CKEY_6:	
			case CKEY_7:	
			case CKEY_8:	
			case CKEY_9:	
			case CKEY_asterisk:		/** *-asterisk **/
			case CKEY_0:		
			case CKEY_pound:		/** #-pound **/
				break;
			default:
				break;
			}
		}
		break;
		
	default:
		break;
	}
    
    return  0;
}
#endif

/** ����Ա�����֤ **/
int	iSearchCount = 0;
int f_keyAdminAuthen(unsigned *pMsg)
{
	msg_t msg;
	func_t func;
//	unsigned char ucResp; 
//	unsigned char ucArr[12]; 
//	unsigned char ucRet;
	
	msg_init(&msg);
	msg.msgType = ((msg_t *)pMsg)->msgType;
	msg.msgValue = ((msg_t *)pMsg)->msgValue;

    switch(msg.msgType)
	{	
	case CMSG_INIT:
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
    	MFingerPower_on();	/** ָ��ͷͨ�� **/
    	
    	beep();
    	vp_stor(CVOPID_COMPARISON);
    	vp_stor(CVOPID_MANAGE);
    	vp_stor(CVOPID_FINGER);
    	vp_stor(CVOPID_PRESSFINGER);
		beep();
		promptDelay(TIMER_1SEC);
		break;
		 
	case CPMT_OVER:		/** ��ʾ�����������Կ�ʼ������֤ **/
		//ClrTimer_irq(&g_timer[1]);
		promptInit();
		FP_Query(0, 0);
		break;
	
	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10)
		{
			init_fstack(&g_fstack);
			func.func = f_keyUnlockFail;
    		stackpush(&g_fstack, &func);
    		
			g_tick = 0;						/** ��ֹ���ܵĲ���;�У���ʱ�˳� **/
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		break;
		
	case CMSG_FGQUERY:
		/** Ȩ��ͨ��,��ʼ¼�롢ɾ������ **/
		MFingerPower_off();				/** ָ��ͷ�ϵ� **/
		g_tick = 0;						/** ��ֹ���ܵĲ���;�У���ʱ�˳� **/
    	SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
    	
		init_fstack(&g_fstack);
    	stackpush(&g_fstack, &g_next_func);
		break;
	
	case CMSG_FGOPFAIL:
		iSearchCount++;
		if(iSearchCount < 3)
		{
			FP_Query(0, 0);	/** �ȶ�ʧ�ܣ����·���ָ�Ʊȶ����� **/
		}
		else
		{
			init_fstack(&g_fstack);
			func.func = f_keyUnlockFail;
    		stackpush(&g_fstack, &func);
    		
			g_tick = 0;						/** ��ֹ���ܵĲ���;�У���ʱ�˳� **/
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		break;
	
	default:
		break;
	}
	return	0;		
}

int f_keyAdminDergstPSWord(unsigned *pMsg)
{
	msg_t msg;
	func_t func;
	
	msg_init(&msg);
	msg.msgType = ((msg_t *)pMsg)->msgType;
	msg.msgValue = ((msg_t *)pMsg)->msgValue;

	
    switch(msg.msgType)
	{	 	
	case CMSG_INIT:
		g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		password_del();
		flashWrite(g_flash.arrInt);
		
		promptInit();
		bbeep();
    	vp_stor(CVOPID_COMPARISON);
    	vp_stor(CVOPID_PASSED);		
    			
		//vp_stor(CVOPID_PSWORD);
		promptDelay(TIMER_300MS);
		vp_stor(CVOPID_DEL);
		vp_stor(CVOPID_SUCESS);
		bbeep();
		ledPrompt(TIMER_100MS);
		break;

	case CMSG_TMR:
		g_tick++;
		break;
		
	case CPMT_OVER:
		//ClrTimer_irq(&g_timer[1]);
		promptInit();
		init_fstack(&g_fstack);
		func.func = f_Unlock;
    	stackpush(&g_fstack, &func);
    	
		g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		break;
			
	default:
		break;
	}		
	return	0;
}
/*******************************************************************************
 * ע������
 *******************************************************************************/
int f_keyAdminRgstPSWord(unsigned *pMsg)
{
//	int i = 0;
	msg_t msg;
	func_t func;
	//static unsigned char password[10];
	static unsigned char password[18];
	static int iCount = 0;
//	charData_t charData;
	
	msg_init(&msg);
	msg.msgType = ((msg_t *)pMsg)->msgType;
	msg.msgValue = ((msg_t *)pMsg)->msgValue;

    switch(msg.msgType)
	{	 
	case CMSG_INIT:
		g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		
		promptInit();
		bbeep();
    	vp_stor(CVOPID_COMPARISON);
    	vp_stor(CVOPID_PASSED);				
		vp_stor(CVOPID_PLEASEINPUTPSWORD);
		lbeep();
		ledPrompt(TIMER_100MS);
		break;
		
	case CPMT_OVER:
		promptInit();
		
		//MOPBlue_on();			/** ��˸֮��ָ����� **/		
		MOPBlue_setDefaultStatus();	/** �ָ�ָʾ���뱳��һ�� **/
		break;
	
	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 30)				/** ����������̳�ʱ�˳� **/
		{
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
    		
			g_tick = 0;
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		break;
		
	case CMSG_DKEY:						/** ��⵽�������� **/
		{
			switch(msg.msgValue)
			{
			case CKEY_GOIN:				/** ��������֣��޹�������� **/
			//case CTIKEY:
				break;
				
			case CKEY_GOOUT:
			//case CTOKEY:
				/** �����ڰ���, �˳� **/
				init_fstack(&g_fstack);
				func.func = f_Unlock;
    			stackpush(&g_fstack, &func);
				g_tick = 0;
    			SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
				break;
			
			case CKEY_1:		/** ������������� **/
			case CKEY_2:
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
				/** ����λ **/
				#if	1
				password[iCount] = msg.msgValue;	
				iCount++;
				#endif
				
				if(iCount >= 16) {
					init_fstack(&g_fstack);
				   	func.func = f_keyAdminRgstPSWordDone;
    				stackpush(&g_fstack, &func);
					g_tick = 0;
    				SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
					
					if((1 != psword_isFull()) && psword_inputConfirm(password) && password_add(password)) {
						promptInit();
						beep();
						vp_stor(CVOPID_EROLL);
						vp_stor(CVOPID_SUCESS);
						bbeep();
						ledPrompt(TIMER_100MS);
					} else {
						promptInit();
						beep();
						vp_stor(CVOPID_EROLL);
						vp_stor(CVOPID_FAILED);
						bbbeep();	/*** ���ʧ�ܻ򸲸���ǰ����������� ***/
						ledWarn(TIMER_100MS);
					}
					flashWrite(g_flash.arrInt);
				} else if(iCount == 8) {
					beep();
					vp_stor(CVOPID_PLEASEEROLLAGAIN);
				} else {
					beep();
				}
				break;
				
			default:
				break;
			}
		}
		break;
		
	default:
		break;
	}
    
    return  0;
}

int f_keyAdminRgstPSWordDone(unsigned *pMsg)
{
	msg_t msg;
	func_t func;
	
	msg_init(&msg);
	msg.msgType = ((msg_t *)pMsg)->msgType;
	msg.msgValue = ((msg_t *)pMsg)->msgValue;

	switch(msg.msgType)
	{
	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10)			/** ��ʱ�˳� **/
		{
			goto GOTO_FUNCOVER;
		}
		break;
		
	case CPMT_OVER:
		GOTO_FUNCOVER:
			
		promptInit();
		init_fstack(&g_fstack);
		func.func = f_Unlock;
    	stackpush(&g_fstack, &func);
    		
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);		
		break;
	
	default:
		break;
	}	
	return	0;
}

/*******************************************************************************
 * f_keyPasswordError
 * �������(����ͳ��)
 * ֻ��Ӧbeep��ʾ��,ֻ����ʾ��Ӱ��״̬ת��
 *******************************************************************************/
 static unsigned char ucErrCount = 0;	//��Ŀǰ�ۼ����������������
int f_keyPasswordError(unsigned *pMsg)
{
	func_t func; 
	static unsigned char ucErrCount_bak = 0;
	
    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_INIT:
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
    	
		promptInit();
    	beep();
    	vp_stor(CVOPID_COMPARISON);
    	vp_stor(CVOPID_FAILED);
    	
    	bbbeep();
		ledWarn(TIMER_100MS);		/** ������˸ **/
			
		break;

	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10)
		{
			init_fstack(&g_fstack);
			func.func = f_keyUnlockFail;
    		stackpush(&g_fstack, &func);
    		
			g_tick = 0;
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		break;
		
	case CPMT_OVER:
		promptInit();
		MOPBlue_setDefaultStatus();	/** �ָ�ָʾ���뱳��һ�� **/
		
		if(ucErrCount_bak == 0)		//��һ�ν���
    	{
    		ucErrCount_bak = getpassword_errno();
    		ucErrCount =  getpassword_errno();
    	}
    	ucErrCount++;
    	ucErrCount_bak = ucErrCount;
    	
    	if(ucErrCount >= CMAX_PSERR)
    	{
    		if(getpassword_errno() != ucErrCount)		//��������б仯
    		{
				setpassword_errno(ucErrCount);
				flashWrite(g_flash.arrInt);
			}
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
    	}
    	else
    	{
    		init_fstack(&g_fstack);
			func.func = f_keyInput;
    		stackpush(&g_fstack, &func);
    		
    		vp_stor(CVOPID_PLEASEINPUTPSWORD);
    	}
		
		
		
    	g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
    	
		break;
					
	default:
		break;
	}
    return  0;
}

/*******************************************************************************
 * ���뿪��״̬
 * ��ʱ����������ʱ�˳�
 *******************************************************************************/
int f_keyInput(unsigned *pMsg)
{
//	int i;
	msg_t msg;
	func_t func;
	static unsigned char password[10];
	static int iCount = 0;	   
	
//	static unsigned char ucErrCount_bak = 0;
//	charData_t charData;

	msg.msgType = ((msg_t *)pMsg)->msgType;
	msg.msgValue = ((msg_t *)pMsg)->msgValue;

    switch(((msg_t *)pMsg)->msgType) 
	{
		
	case CMSG_INIT:
    	g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
    	iCount = 0;
		break; 
		
	case CMSG_TMR:
		g_tick++;
		if(g_tick > 10)			/** ��ʱ�˳� **/
		{
			if(getpassword_errno() != ucErrCount)		//��������б仯
    		{
				setpassword_errno(ucErrCount);
				flashWrite(g_flash.arrInt);
			}
			
			init_fstack(&g_fstack);
			func.func = f_keyUnlockFail;
    		stackpush(&g_fstack, &func);
    		
			g_tick = 0;
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		break;
		
	case CPMT_OVER:
		promptInit();
		break;
		
	case CMSG_DKEY:					/** ��⵽�������� **/
		promptInit();
		#if	0	/** debug **/
			charData.ucVal = msg.msgValue & 0xff;
			charQueueIn_irq(&g_com1TxQue, &charData); 				
		#endif
		
		switch(msg.msgValue)
		{
		case CKEY_GOOUT:
		case CKEY_SET:
			
			/** �����İ�����������а������ڰ��֣������ü����� **/
			init_fstack(&g_fstack);
			func.func = f_KEYToOut;
    		stackpush(&g_fstack, &func);
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
			break;
			
		case CKEY_GOIN:				/** ��������֣��޹����������ֱ������֮ **/
			break;
		
		case CKEY_1:		/** ��������ģʽ **/
		case CKEY_2:	
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
			#if	1
			password[iCount] = msg.msgValue;	
			iCount++;
			#endif
			
			if(iCount >= 8)
			{
				if(password_Query(password) == 1)
				{
					init_fstack(&g_fstack);
			   		func.func = f_keyUnlockDoing;
    				stackpush(&g_fstack, &func);
    				SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
    				
    				ucErrCount = 0;
    				if(getpassword_errno() != 0)		//��������б仯
    				{
						setpassword_errno(0);
						flashWrite(g_flash.arrInt);
					}
				}
				else	/** û��ͨ������ȶ� **/
				{
					init_fstack(&g_fstack);
			   		func.func = f_keyPasswordError;
    				stackpush(&g_fstack, &func);
    				
					g_tick = 0;
    				SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
    				
				}
			}
			else
			{	
				beep();
			}
			break;
			
		default:
			break;
		}
		break;
		
	default:
		break;
	}
    
    return  0;
}

/*******************************************************************************
 * f_keyUnlockDoing
 * ��������(�ȿ�ʼһ��beep+led Flash���ٿ���)
 * ֻ�п�������Ӱ��״̬ת��
 *******************************************************************************/
int f_keyUnlockDoing(unsigned *pMsg)
{
	//msg_t msg;
	func_t func;

	//msg.msgType = ((msg_t *)pMsg)->msgType;
	//msg.msgValue = ((msg_t *)pMsg)->msgValue;

    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_INIT:
   		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		promptInit();
		beep();
    	vp_stor(CVOPID_COMPARISON);
    	vp_stor(CVOPID_PASSED);
    	vp_stor(CVOPID_OK);
    	
    	ledPrompt(TIMER_100MS);		/** �ɹ���ʾ��˸ **/	
    	lockAction();
    	
    	if(getpassword_errno() != 0)		//���������Ҫ����
    	{
			setpassword_errno(0);
			flashWrite(g_flash.arrInt);
		}
    	
		break;
			
	case CACT_OVER:
		actionInit();
		
		init_fstack(&g_fstack);
		func.func = f_keyInputDone;
    	stackpush(&g_fstack, &func);
    	SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		break;
		
	case CPMT_OVER:
		promptInit();
		
		//MOPBlue_on();			/** ��˸֮��ָ����� **/		
		MOPBlue_setDefaultStatus();	/** �ָ�ָʾ���뱳��һ�� **/
		break;
					
	default:
		break;
	}
    return  0;
}

/*******************************************************************************
 * f_keyInputDone
 * ���������ѽ���
 * ֻ��Ӧbeep��ʾ��,ֻ����ʾ��Ӱ��״̬ת��
 *******************************************************************************/
int f_keyInputDone(unsigned *pMsg)
{
	func_t func;

    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_INIT:
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
    	//bbeep();
    	promptDelay(TIMER_100MS);
		break;

	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10)
		{
			goto GOTO_FUNCOVER;
		}
		break;
		
	case CPMT_OVER:
	GOTO_FUNCOVER:
		promptInit();
		init_fstack(&g_fstack);
		func.func = f_Unlock;
    	stackpush(&g_fstack, &func);
    	
    	g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		break;
					
	default:
		break;
	}
    return  0;
}

/*******************************************************************************
 * f_keyUnlockFail
 * ��������ʧ��
 * ֻ��Ӧbeep��ʾ��,ֻ����ʾ��Ӱ��״̬ת��
 *******************************************************************************/
int f_keyUnlockFail(unsigned *pMsg)
{
	func_t func;

    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_INIT:
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
    	
		promptInit();
    	beep();
    	vp_stor(CVOPID_COMPARISON);
    	vp_stor(CVOPID_FAILED);
    	
    	bbbeep();
		ledWarn(TIMER_100MS);		/** ������˸ **/
		break;

	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10)
		{
			goto GOTO_FUNCOVER;
		}
		break;
		
	case CPMT_OVER:
	GOTO_FUNCOVER:
		promptInit();
		//MOPBlue_on();			/** ��˸֮��ָ����� **/		
		MOPBlue_setDefaultStatus();	/** �ָ�ָʾ���뱳��һ�� **/
		
		init_fstack(&g_fstack);
		func.func = f_Unlock;
    	stackpush(&g_fstack, &func);
    	
    	g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		break;
					
	default:
		break;
	}
    return  0;
}

/*******************************************************************************
 * ���ڰ���(���ܼ�)����
 *******************************************************************************/
int f_KEYToOut(unsigned *pMsg)		/** ���� **/
{
	func_t func;
	
	//switch(((msg_t *)pMsg)->msgType) 
	switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_TMR:
		g_tick++;
		if(g_tick > 10)
		{	   
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		break;

	case CMSG_INIT:		/** ����, ����ʼ����ʱ **/
		g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		lockAction();
		
		if(getpassword_errno() != 0)		//���������Ҫ����
    	{
			setpassword_errno(0);
			flashWrite(g_flash.arrInt);
		}
		
		break;
	
	case CMSG_BACK:
		MBlackLED_on();				/** ������� **/
		MOPBlue_on();				/** �뱳��һ���ָʾ�� **/
		break;
	
	case CMSG_DKEY:			/** ��⵽�������� **/
		switch(((msg_t *)pMsg)->msgValue)
		{
			case CKEY_1:		/** ������������� **/
			case CKEY_2:
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
				MBlackLED_on();				/** ������� **/
				MOPBlue_on();				/** �뱳��һ���ָʾ�� **/
				promptInit();
				beep();
				break;
		}
		break;
		
	case CPMT_OVER:
		promptInit();
		break;
		
	case CACT_OVER:
		actionInit();
		
		MBlackLED_off();	/** ������� **/
		MFingerPower_off();	/** ָ��ͷ�ϵ� **/
		MMasterPower_off();	/** ���ϵ� **/
		  
		init_fstack(&g_fstack);
		func.func = f_Unlock;
    	stackpush(&g_fstack, &func);	
		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		break;
		
	default:
		break;
	}

	return	0;
}

/*******************************************************************************
 * ���ü�����-�����ڼ���Խ���һЩ��������(��:1��ϵͳ��ʼ��; 2��ADCУ��; 3������testģʽ)
 *��
 * ���е��ĳ�ʼ��ָ��ģ��, ����ֻ��Ҫ��һ���ֽڵ����������
 *******************************************************************************/
 unsigned char menuVOPTab[] = {
 	#if	CSETOP_asterisk_pound
 	CVOPID_EROLL, CVOPID_FINGER, CVOPID_PLEASEPRESS, CVOPID_pound, CVOPID_1,
 	CVOPID_DEL, CVOPID_FINGER, CVOPID_PLEASEPRESS, CVOPID_asterisk, CVOPID_1, 
 	CVOPID_DEL, CVOPID_ALL, CVOPID_FINGER, CVOPID_PLEASEPRESS, CVOPID_asterisk, CVOPID_pound,
 	CVOPID_ADD, CVOPID_PSWORD, CVOPID_PLEASEPRESS, CVOPID_pound, CVOPID_2,
 	CVOPID_DEL, CVOPID_PSWORD, CVOPID_PLEASEPRESS, CVOPID_asterisk, CVOPID_2,
 	#else
 	CVOPID_EROLL, CVOPID_FINGER, CVOPID_PLEASEPRESS, CVOPID_1, CVOPID_3,
 	CVOPID_DEL, CVOPID_FINGER, CVOPID_PLEASEPRESS, CVOPID_1, CVOPID_4,
 	CVOPID_DEL, CVOPID_ALL, CVOPID_FINGER, CVOPID_PLEASEPRESS, CVOPID_1, CVOPID_5,
 	CVOPID_ADD, CVOPID_PSWORD, CVOPID_PLEASEPRESS, CVOPID_2, CVOPID_3,
 	CVOPID_DEL, CVOPID_PSWORD, CVOPID_PLEASEPRESS, CVOPID_2, CVOPID_4,
 	#endif
};	

int f_KEYSET(unsigned *pMsg)		/** ����&���� **/
{
	func_t func;
	msg_t msg;
	//static int iTimeOut = 0;
	static int iCount = 0;
	static unsigned char VOPIdx = 0;
	static unsigned char ucInputKey[10];

	msg_init(&msg);
	msg.msgType = ((msg_t *)pMsg)->msgType;
	msg.msgValue = ((msg_t *)pMsg)->msgValue;

	//switch(((msg_t *)pMsg)->msgType) 
	switch(msg.msgType)
	{
	case CMSG_TMR:
		g_tick++;
		//if(((g_tick > 10) && (iTimeOut == 0)) || ((g_tick > 20) && (iTimeOut != 0))) 
		if(g_tick > 15)
		{
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		break;

	case CMSG_INIT:		/** ����, ����ʼ����ʱ **/
		g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		beep();
    	vp_stor(menuVOPTab[VOPIdx]);
		break;
	
	case CMSG_BACK:
		MBlackLED_on();				/** ������� **/
		MOPBlue_on();				/** �뱳��һ���ָʾ�� **/
		break;
				
	case CMSG_DKEY:			/** ��⵽�������� **/
		switch(msg.msgValue)
		{
		case CKEY_SET:				/** �������ü�,���ڰ���ʧЧ������ΪӦ��������ʽ **/
			if(g_tick < 1) {		/** �������״̬��1�������ڲ�����set������ **/
				break;
			}
			
			if(iCount != 8) {
				init_fstack(&g_fstack);
				func.func = f_KEYToOut;
    			stackpush(&g_fstack, &func);
				g_tick = 0;
    			SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
				break;
			}
		case CKEY_GOOUT:				
		case CKEY_1:
		case CKEY_2:
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
			
//			iTimeOut = 20;		/** �趨��ʱʱ�� **/
			MBlackLED_on();				/** ������� **/
			MOPBlue_on();				/** �뱳��һ���ָʾ�� **/
			ucInputKey[iCount] = msg.msgValue;	
			iCount++;
			
			VOPIdx = TABSIZE(menuVOPTab);
			promptInit();			
			beep();
			
			if(iCount == 2)
			{
				#if	CSETOP_asterisk_pound
				if((ucInputKey[0] == CKEY_pound) && (ucInputKey[1] == CKEY_1))	/** ¼��ָ�� **/
				#else
				if((ucInputKey[0] == CKEY_1) && (ucInputKey[1] == CKEY_3))	/** ¼��ָ�� **/
				#endif
				{
    				g_tick = 0;
					SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_INIT);	/** �ӻ���Ϣ���͹���,�ȴ�beep��� **/
    				
					promptInit();
					beep();
					init_fstack(&g_fstack);
					func.func = f_RgstfingerInit;
    				stackpush(&g_fstack, &func);
				}
				
				#if	CSETOP_asterisk_pound
				if((ucInputKey[0] == CKEY_asterisk) && (ucInputKey[1] == CKEY_1))	/** ɾ��ָ�� **/
				#else
				if((ucInputKey[0] == CKEY_1) && (ucInputKey[1] == CKEY_4))	/** ɾ��ָ�� **/
				#endif
				{
					if(fingerTab_isEmpty())	{	/** ָ��ģ��գ�������ʾ **/
						g_tick = 0;
						SetTimer_irq(&g_timer[0], TIMER_60MS, CMSG_INIT);	/** �ӻ���Ϣ���͹��� **/
						promptInit();
						
						init_fstack(&g_fstack);
						func.func = f_fingerNoFPWarn;
    					stackpush(&g_fstack, &func);
					} else {
						g_tick = 0;
						SetTimer_irq(&g_timer[0], TIMER_60MS, CMSG_INIT);	/** �ӻ���Ϣ���͹��� **/
						MFingerPower_on();	/** ָ��ͷ�ϵ� **/	
						promptInit();
						
						init_fstack(&g_fstack);
						//func.func = f_DegstfingerInit;
						func.func = f_fingerDeleteInit;
    					stackpush(&g_fstack, &func);
    				}
				}
				
				#if	CSETOP_asterisk_pound
				if((ucInputKey[0] == CKEY_asterisk) && (ucInputKey[1] == CKEY_pound))	/** ɾ��ȫ��ָ�� **/
				#else
				if((ucInputKey[0] == CKEY_1) && (ucInputKey[1] == CKEY_5))	/** ɾ��ȫ��ָ�� **/
				#endif
				{
					if(fingerTab_isEmpty())	{	/** ָ��ģ��գ�������ʾ **/
						g_tick = 0;
						SetTimer_irq(&g_timer[0], TIMER_60MS, CMSG_INIT);	/** �ӻ���Ϣ���͹��� **/
						promptInit();
						
						init_fstack(&g_fstack);
						func.func = f_fingerNoFPWarn;
    					stackpush(&g_fstack, &func);
					} else {
						promptInit();
						g_tick = 0;
						SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);	/** �ӻ���Ϣ���͹��� **/
						//MFingerPower_off();			/** ָ��ͷ�ϵ��ޱ�Ҫ **/
						
						g_subState = 0;
						init_fstack(&g_fstack);
						func.func = f_fingerDelAllFP;
    					stackpush(&g_fstack, &func);
    				}
				}
				
				#if	CSETOP_asterisk_pound
				if((ucInputKey[0] == CKEY_pound) && (ucInputKey[1] == CKEY_2))	/** ¼������ **/
				#else
				if((ucInputKey[0] == CKEY_2) && (ucInputKey[1] == CKEY_3))	/** ¼������ **/
				#endif
				{
					if(fingerTab_isEmpty())	{	/** ָ��ģ��գ�������ʾ **/
						g_tick = 0;
						SetTimer_irq(&g_timer[0], TIMER_60MS, CMSG_INIT);	/** �ӻ���Ϣ���͹��� **/
						promptInit();
						
						init_fstack(&g_fstack);
						func.func = f_fingerNoFPWarn;
    					stackpush(&g_fstack, &func);
					} else {
						g_next_func.func  = f_keyAdminRgstPSWord;
						init_fstack(&g_fstack);
						func.func = f_keyAdminAuthen;
    					stackpush(&g_fstack, &func);
    					
						promptInit();
						g_tick = 0;
    					SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
    				}
				}
				
				#if	CSETOP_asterisk_pound
				if((ucInputKey[0] == CKEY_asterisk) && (ucInputKey[1] == CKEY_2))	/** ɾ������ **/
				#else
				if((ucInputKey[0] == CKEY_2) && (ucInputKey[1] == CKEY_4))	/** ɾ������ **/
				#endif
				{
					if(fingerTab_isEmpty())	{	/** ָ��ģ��գ�������ʾ **/
						g_tick = 0;
						SetTimer_irq(&g_timer[0], TIMER_60MS, CMSG_INIT);	/** �ӻ���Ϣ���͹��� **/
						promptInit();
						
						init_fstack(&g_fstack);
						func.func = f_fingerNoFPWarn;
    					stackpush(&g_fstack, &func);
					} else {
						g_next_func.func  = f_keyAdminDergstPSWord;
						init_fstack(&g_fstack);
						func.func = f_keyAdminAuthen;
    					stackpush(&g_fstack, &func);
    					
						promptInit();
						g_tick = 0;
    					SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
    				}
				}
				
				if((ucInputKey[0] == CKEY_3) && (ucInputKey[1] == CKEY_5)) {	/** �͵�У�� **/
					if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7) == 1) {			/** Ҫ��һֱѹ��set�� **/
					//if(1) {	/** test for function **/
						init_fstack(&g_fstack);
						func.func = f_keyLowPower;
    					stackpush(&g_fstack, &func);
						g_tick = 0;
    					SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
    				}
				}
				
				if((ucInputKey[0] == CKEY_3) && (ucInputKey[1] == CKEY_6)) {	/** ����ģʽtestMode **/
					if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7) == 1) {			/** Ҫ��һֱѹ��set�� **/
						init_fstack(&g_fstack);
						func.func = f_testMode;
    					stackpush(&g_fstack, &func);
						g_tick = 0;
    					SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
    				}
				}
				
				if((ucInputKey[0] == CKEY_3) && (ucInputKey[1] == CKEY_7)) {	/** �ָ�����ֵ **/
					if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7) == 1) {			/** Ҫ��һֱѹ��set�� **/
						init_fstack(&g_fstack);
						func.func = f_keyRestorFactorySetting;
    					stackpush(&g_fstack, &func);
    					promptInit();
						g_tick = 0;
    					SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
    				}
				}
			} else if(iCount == 9) {
				#if	0
				if((ucInputKey[0] == CKEY_3) && (ucInputKey[1] == CKEY_5))	
				#else
				if(	(ucInputKey[0] == CKEY_1)
					 && (ucInputKey[1] == CKEY_8)
					 && (ucInputKey[2] == CKEY_9) 
					 && (ucInputKey[3] == CKEY_3)
					 && (ucInputKey[4] == CKEY_1) 
					 && (ucInputKey[5] == CKEY_2)
					 && (ucInputKey[6] == CKEY_2)
					 && (ucInputKey[7] == CKEY_6) 
					 && ((ucInputKey[8] == CKEY_GOOUT) || (ucInputKey[8] == CKEY_SET)))
					 //&& (ucInputKey[8] == CKEY_GOOUT)))
				#endif
				{
					init_fstack(&g_fstack);
					func.func = f_keyLowPower;
    				stackpush(&g_fstack, &func);
					g_tick = 0;
    				SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
				}
			}
			
			if(iCount > 9) {
				init_fstack(&g_fstack);
				func.func = f_KEYSETINVALID;
    			stackpush(&g_fstack, &func);
    				
				promptInit();
				g_tick = 0;
    			SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
			}
			break;
			
		default:
			break;
		}
		break;
			
	case CPMT_OVER:
		promptInit();
		VOPIdx++;
		if(VOPIdx < TABSIZE(menuVOPTab)) {
			if(g_tick > 7) {
				g_tick = 7;
			}
    		vp_stor(menuVOPTab[VOPIdx]);
    	} else {
			g_tick = 0;
    		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
    	}
		break;
		
		#if	0
	case CACT_OVER:
		actionInit();
		if(iTimeOut == 0)		/** û�й����� **/
		{
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
    		
			msg.msgType = CMSG_INIT;
			inq_irq(&g_msgq, &msg);
			
			MBlackLED_off();	/** ������� **/
			MFingerPower_off();	/** ָ��ͷ�ϵ� **/
			MMasterPower_off();	/** ���ϵ� **/
		}
		break;
		#endif
		
	default:
		break;
	}

	return	0;
}

/** ��set���������Ч�������� **/
int f_KEYSETINVALID(unsigned *pMsg)		/** ����&������Ч **/
{
	func_t func;
	
	switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_INIT:
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
    	
		promptInit();
    	beep();
    	vp_stor(CVOPID_SET);
    	vp_stor(CVOPID_INVALID);
    	bbbeep();
		ledWarn(TIMER_100MS);
		break;
		
	case CMSG_TMR:
		g_tick++;
		if(g_tick > 10)
		{
			/** ��ʱ�˳� **/
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		
		break;

	case CPMT_OVER:
		promptInit();
				
		//MOPBlue_on();			/** ��˸֮��ָ����� **/		
		MOPBlue_setDefaultStatus();	/** �ָ�ָʾ���뱳��һ�� **/
		
		init_fstack(&g_fstack);
		func.func = f_Unlock;
    	stackpush(&g_fstack, &func);
    	SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		break;
			
	default:
		break;
	}

	return	0;
}

/******************************************************************************* 
 * �͵籨��-У׼
 *******************************************************************************/
int f_keyLowPower(unsigned *pMsg)
{
	func_t func;
	msg_t msg;
	
	switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_INIT:
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
    	
		msg.msgType = CMSG_CALC;	/** ��ѹADC��������������Ҳ����һ�� **/
		inq_irq(&g_msgq, &msg);
		break;
		
	case CMSG_TMR:
		g_tick++;
		if(g_tick > 10)
		{
			/** ��ʱ�˳� **/
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		
		if(g_flag & (1 << 8))	/** ������� **/
		{
			ADC_setValue(g_iADCCurr);
			flashWrite(g_flash.arrInt);
			
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
    		g_tick = 0;
    		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
    		
    		lbeep();	
			promptDelay(TIMER_1SEC);
    		bbeep();
		}
		break;

	case CPMT_OVER:
		promptInit();
		break;
			
	default:
		break;
	}

	return	0;
}

#if	0
/******************************************************************************* 
 * �͵籨��-У׼���
 *******************************************************************************/
int f_keyLowPowerDone(unsigned *pMsg)
{
	func_t func;
	
	switch(((msg_t *)pMsg)->msgType)
	{
	case CPMT_OVER:
		promptInit();
	
		func.func = f_Unlock;
    	stackpush(&g_fstack, &func);
    	SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		break;
		
	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10)
		{
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		break;
		
	default:
		break;
	}

	return	0;
}
#endif


/******************************************************************************* 
 * �ָ�����ֵ
 *******************************************************************************/
int f_keyRestorFactorySetting(unsigned *pMsg)
{
	func_t func;
	//msg_t msg;
	
	switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_INIT:
		promptInit();
		beep();
		promptDelay(TIMER_1SEC);
		
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_2SEC, CFIGER_INIT);
    	
		password_del();
		flashWrite(g_flash.arrInt);		/** ��ɾ��ȫ������ **/
    	
    	fingerTab_fill(0xff, 0xff);		/** ɾ��ȫ��ָ��ģ������ **/
		resetOP_setValue(0xa0);
		flashWrite(g_flash.arrInt);	
    	
    	MFingerPower_on();				/** ָ��ͷͨ�� **/
		break;
	
	case CFIGER_INIT:		/** ָ��ͷͨ��1����ʼ��ָ��ͷ **/
		g_tick++;
		if(g_tick > 2) {
			/** ָ��ͷ��ʼ��ʧ�� **/
			init_fstack(&g_fstack);
			//func.func = f_Unlock;
			func.func = f_fingerError;
    		stackpush(&g_fstack, &func);
    		g_tick = 0;
			SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);			
		}
    	//SetTimer_irq(&g_timer[0], TIMER_1SEC, CFIGER_INIT);		/**�����û�С�**/
    	FP_INIT();					/** ָ��ͷ��ʼ�� **/
		break;
		
	case CMSG_FGINIT:		/** ָ��ͷ��ʼ�����, ��ʼ���� **/
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_TMR);
		FP_DelAll();
		break;
	
	case CMSG_FGDERG:
		MFingerPower_off();				/** ָ��ͷ�ϵ� **/
		
		init_fstack(&g_fstack);
		//func.func = f_Unlock;
		func.func = f_keyRestorFactorySettingDone;
    	stackpush(&g_fstack, &func);
    	g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);			
		break;
			
	case CMSG_TMR:
		g_tick++;
		if(g_tick > 10)
		{
			/** ��ʱ�˳� **/
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		break;

	case CPMT_OVER:
		promptInit();
		break;
			
	default:
		break;
	}

	return	0;
}

/******************************************************************************* 
 * �ָ�����ֵ
 *******************************************************************************/
int f_keyRestorFactorySettingDone(unsigned *pMsg)
{
	func_t func;
	//msg_t msg;
	
	switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_INIT:
		promptInit();
    	vp_stor(CVOPID_RECOVERED);				/** �ָ��������� **/
		lbeep();
		promptDelay(TIMER_1SEC);
		lbeep();
		ledPrompt(TIMER_100MS);
		
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		break;
			
	case CMSG_TMR:
		g_tick++;
		if(g_tick > 10)
		{
			/** ��ʱ�˳� **/
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		break;

	case CPMT_OVER:
		promptInit();
		MOPBlue_setDefaultStatus();	/** �ָ�ָʾ���뱳��һ�� **/
		break;
			
	default:
		break;
	}

	return	0;
}

/////////////////////////////////////////////////////

