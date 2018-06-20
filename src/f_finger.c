
#include "stm32f10x_lib.h"
#include "string.h"
#include "../inc/CONST.h"
#include "../inc/ctype.h" 
#include "../inc/macro.h" 
#include "../inc/global.h" 
#include "../inc/debug.h"
#include "arch.h"	  
#include "driver.h"		
#include "l_voicePrompt.h"	  
#include "l_fingprint.h"
#include "func_idle.h"
#include "f_Unlock.h "
#include "f_finger.h "	
#include "f_key.h "	  
#include "charQue.h"	
#include "main.h"
 
/**
 *	ֻ��¼���ָ�ƲŻ����е�����
 * f_finger -> f_fingerInit -> f_fingerSearch(1��������2��¼ָ�� 3��ɾָ��)
 *��������������������������-> f_fingerError(ָ��ͷ����)
 **/
 
//static unsigned char ManagerId = 0;		/** ¼���ָ�ƶ�Ӧ��id�� **/
static unsigned char ucRgstCnt = 0;			/** ¼��ָ�Ʋ���� **/
/*******************************************************************************
 * ͨ������1����ָ�ƿ���״̬
 *******************************************************************************/
#if	0
int f_fingerStart(unsigned *pMsg)
{
	func_t func;

    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_TMR:
		g_tick++;
		break;
		
	case CMSG_INIT:	
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		MFingerPower_on();			/** ��ָ��ͷ���� **/
		beep();
		break;
		
	case CPMT_OVER:
		promptInit();
		g_tick = 0;
		FP_INIT();		/** reset fingerPrint(The command have no responsed) **/

		func.func = f_fingerInit;
    	stackpush(&g_fstack, &func);
		
		break;
		
	default:
		break;
	}
    
    return  0;
}
#endif

/*******************************************************************************
 * step 1: search fingerPrint for Unlock only��ͨ����ָ��ͷ���뿪��״̬
 *******************************************************************************/
int f_finger(unsigned *pMsg)
{
	func_t func;
	   
    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_TMR:
		g_tick++;
		break;
		
	case CMSG_INIT:	
		g_tick = 0;
		promptInit();
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);

		init_fstack(&g_fstack);
		func.func = f_fingerInit;
    	stackpush(&g_fstack, &func);
		FP_INIT();		/** reset fingerPrint(The command have no responsed) **/
		break;
		
	default:
		break;
	}
    
    return  0;
}

static int iSearchCount = 0;	/** �ȶԴ��� **/
/** ********************************************************
 * initial finger print 
 * Description: ��ʼ��ָ��ͷ
 * 				sucess: ��ָ��ģ��: ��ָ�ƾͿ���, ��ָ�ƾͷ��Ͳ�ָ������
 * 				fail: ��������, �������3�ζ�ʧ��, �Ͷϵ��˳�
 ***********************************************************/
int f_fingerInit(unsigned *pMsg)
{
	func_t func;
//	msg_t msg; 
	//charData_t charData;
//	unsigned char ucArr[12];
//	unsigned char ucResp;   
//	unsigned char ucRet;
	
//	msg.msgType = ((msg_t *)pMsg)->msgType;
//	msg.msgValue = ((msg_t *)pMsg)->msgValue;

    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_TMR:
		g_tick++;
		if(g_tick < 3)
		{
			FP_INIT();		/** reset fingerPrint again(The command have no responsed) **/
		}
		else	/** initial failed more than 3 times **/
		{
			/** ָ��ͷ��ʼ��ʧ�� **/
			init_fstack(&g_fstack);
			//func.func = f_Unlock;
			func.func = f_fingerError;
    		stackpush(&g_fstack, &func);
    		g_tick = 0;
			SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		break;
	
	case CPMT_OVER:		/** ����ǰ����ʾ����� **/
		promptInit();
		break;
		
	case CMSG_FGINIT:
		g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		
		init_fstack(&g_fstack);
		func.func = f_fingerSearch;
    	stackpush(&g_fstack, &func);
		break;
		
	default:
		break;
	}
	return	0;
}

/** ָ��ͷ���ϱ��� **/
int f_fingerError(unsigned *pMsg)
{
	func_t func;
//	msg_t msg; 

//	msg.msgType = ((msg_t *)pMsg)->msgType;
//	msg.msgValue = ((msg_t *)pMsg)->msgValue;

    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_INIT:
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		promptInit();
		lbeep();
		promptDelay(TIMER_500MS);
		lbeep();
		promptDelay(TIMER_500MS);
		lbeep();
		break;
		
	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10) {
			
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
		}
		break;
	
	case CPMT_OVER:		/** ����ǰ����ʾ����� **/
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


/***********************************************************
 * function: f_fingerSearch
 * Description: search fingerPrint unlock if passed or retry if failed
 *
 * ָ�ƱȶԲ�����
 * 
 ***********************************************************/
int f_fingerSearch(unsigned *pMsg)
{
	func_t func;
//	msg_t msg; 

//	msg.msgType = ((msg_t *)pMsg)->msgType;
//	msg.msgValue = ((msg_t *)pMsg)->msgValue;

    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_INIT:
		g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		
		FP_Query(0, CTOTALFINGER - 1);	/** �����ָ��ģ�壬����ָ�Ʊȶ����� **/
		iSearchCount = 0;	/** �ȶԴ��� **/
		break;
		
	case CMSG_TMR:
		g_tick++;
		if(g_tick > 10)	/** ��ʱ **/
		{
			g_tick = 0;
			SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);	/** ��ʱ **/
			init_fstack(&g_fstack);
			func.func = f_fingerSearchFail;
    		stackpush(&g_fstack, &func);
		}
		break;
		
	case CMSG_FGQUERY:
		/** Ȩ��ͨ��,���� **/
		MFingerPower_off();				/** ָ��ͷ�ϵ� **/
		g_tick = 0;						/** ��ֹ���ܵĲ���;�У���ʱ�˳� **/
		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);	/** ��ʱ **/
    	
    	init_fstack(&g_fstack);
		func.func = f_fingerSearchSuccess;
    	stackpush(&g_fstack, &func);
		break;
		
	case CMSG_FGOPFAIL:
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);	/** ��ʱ **/
		iSearchCount++;
		if(iSearchCount < 3)
		{
			FP_Query(0,  CTOTALFINGER - 1);	/** �ȶ�ʧ�ܣ����·���ָ�Ʊȶ����� **/
		}
		else
		{
			MFingerPower_off();
			g_tick = 0;
			SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);	/** ��ʱ **/
			init_fstack(&g_fstack);
			func.func = f_fingerSearchFail;
    		stackpush(&g_fstack, &func);
		}
		break;
		
	case CPMT_OVER:		/** ����ǰ����ʾ����� **/
		promptInit();
		break;
		
	#if	0
	case CACT_OVER:		/** ����������� **/
		actionInit();
		#if	1
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
    		
    		g_tick = 0;
			SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_INIT);
    	#endif
		break;
   	#endif
		
	case CMSG_BACK:
		MBlackLED_on();			/** ���� **/
		MOPBlue_on();			/** ͬ����λ�õ�ָʾ�ƹ� **/	
		break;
		
	case CMSG_DKEY:						/** ��ѯָ�ƹ����м�⵽�������� **/
		switch(((msg_t *)pMsg)->msgValue)
		{
		case CKEY_GOOUT:
		case CKEY_SET:
			/** ˢָ��ʱ�������������հ��֣������ü����� **/
			MFingerPower_off();				/** ָ��ͷ�ϵ� **/
			init_fstack(&g_fstack);
			func.func = f_KEYToOut;
    		stackpush(&g_fstack, &func);
    		g_tick = 0;
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
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
			if(MFP_isInductionPower()) {	/** ָ��ͷ�и�Ӧ�ϵ繦��ʱ�������޷�Ӧ **/
				promptInit();
				beep();
			} else {		/** ָ��ͷû�и�Ӧ�ϵ繦��ʱ����ָ�ƿ���״̬������а������½������뿪��״̬  **/
				MFingerPower_off();				/** ָ��ͷ�ϵ� **/
				init_fstack(&g_fstack);
				func.func = f_keyInput;
    			stackpush(&g_fstack, &func);
    			
				//inq_irq(&g_msgq, &msg);
				inq_irq(&g_msgq, (msg_t *)pMsg);	/** !!!��������Ϣ�ط�һ�� **/
				
    			//g_tick = 0;
    			//SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
			}
			break;
				
		default:
			
			break;
		}
	
	default:
		break;
	}
	return	0;
}


/*******************************************************************************
 * ָ�ƱȶԳɹ� 
 *******************************************************************************/
int f_fingerSearchSuccess(unsigned *pMsg)
{
	func_t func;
	
    switch(((msg_t *)pMsg)->msgType)
	{		
	case CMSG_INIT:
		g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);		/** CMSG_INIT��Ϣ **/
		
		promptInit();
		beep();
    	vp_stor(CVOPID_COMPARISON);
    	vp_stor(CVOPID_PASSED);
    	vp_stor(CVOPID_OK);
    	ledPrompt(TIMER_100MS);
		lockAction();	/** ���� **/
		
		if(getpassword_errno() != 0)		//���������Ҫ����
    	{
			setpassword_errno(0);
			flashWrite(g_flash.arrInt);
		}
		
		break;
		
	case CMSG_TMR:
		g_tick++;
		if(g_tick > 10)	/** ��ʱ **/
		{
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
		}
		break;
		
	case CPMT_OVER:		/** ����ǰ����ʾ����� **/	
		promptInit();
		MOPBlue_setDefaultStatus();	/** �ָ�ָʾ���뱳��һ�� **/
		break;

	case CACT_OVER:		/** ����ǰ����ʾ����� **/	
		actionInit();	
		g_tick = 0;	
		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		
		init_fstack(&g_fstack);
    	func.func = f_Unlock;
    	stackpush(&g_fstack, &func);
		break;
		
	default:
		break;
	}

	return	0;
}


/*******************************************************************************
 * ָ�Ʊȶ�ʧ�� 
 *******************************************************************************/
int f_fingerSearchFail(unsigned *pMsg)
{
	func_t func;
	
    switch(((msg_t *)pMsg)->msgType)
	{		
	case CMSG_INIT:
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);		/** CMSG_INIT��Ϣ **/
		g_tick = 0;
		MFingerPower_off();			/** ָ��ͷ�ϵ� **/
		MBlackLED_off();			/** ������ **/
		MOPBlue_setDefaultStatus();	/** �ָ�ָʾ���뱳��һ�� **/
		promptInit();
		bbbeep();
    	vp_stor(CVOPID_COMPARISON);
    	vp_stor(CVOPID_FAILED);
		ledWarn(TIMER_100MS);		/** ������˸ **/
		break;
		
	case CMSG_TMR:
		g_tick++;
		if(g_tick > 10)	/** ��ʱ **/
		{
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
		}
		break;
		
	case CPMT_OVER:		/** ����ǰ����ʾ����� **/	
		promptInit();
		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		
		init_fstack(&g_fstack);
		func.func = f_Unlock;
    	stackpush(&g_fstack, &func);
		break;
		
	default:
		break;
	}

	return	0;
}


/*******************************************************************************
 * ָ��¼��ʧ��,����(�����Ĺ�����ʧ��Ҳ��ִ�е���)
 *******************************************************************************/
int f_fingerEnrollFail(unsigned *pMsg)
{
	func_t func;
	
    switch(((msg_t *)pMsg)->msgType)
	{
		
	case CMSG_INIT:
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);		/** CMSG_INIT��Ϣ **/
		g_tick = 0;
		MFingerPower_off();			/** ָ��ͷ�ϵ� **/
		promptInit();
		
    	vp_stor(CVOPID_EROLL);
    	vp_stor(CVOPID_FAILED);
		bbbeep();
		ledWarn(TIMER_100MS);		/** ������˸ **/
		
		break;
		
	case CMSG_TMR:
		g_tick++;
		if(g_tick > 10)	/** ��ʱ **/
		{
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
		}
		break;
		
	case CPMT_OVER:		/** ����ǰ����ʾ����� **/	
		promptInit();
		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		
		init_fstack(&g_fstack);
		func.func = f_Unlock;
    	stackpush(&g_fstack, &func);
		break;
		
	default:
		break;
	}

	return	0;
}

/*******************************************************************************
 * ָ��ɾ��ʧ��,����
 *******************************************************************************/
int f_fingerDelFail(unsigned *pMsg)
{
	func_t func;
	
    switch(((msg_t *)pMsg)->msgType)
	{
		
	case CMSG_INIT:
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);		/** CMSG_INIT��Ϣ **/
		g_tick = 0;
		MFingerPower_off();			/** ָ��ͷ�ϵ� **/
		promptInit();
		
    	vp_stor(CVOPID_DEL);
    	vp_stor(CVOPID_FAILED);
		bbbeep();
		ledWarn(TIMER_100MS);		/** ������˸ **/
		break;
		
	case CMSG_TMR:
		g_tick++;
		if(g_tick > 10)	/** ��ʱ **/
		{
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
		}
		break;
		
	case CPMT_OVER:		/** ����ǰ����ʾ����� **/	
		promptInit();
		MOPBlue_setDefaultStatus();	/** �ָ�ָʾ���뱳��һ�� **/
		
		g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		
		init_fstack(&g_fstack);
		func.func = f_Unlock;
    	stackpush(&g_fstack, &func);
		break;
		
	default:
		break;
	}

	return	0;
}

/*******************************************************************************
 * �ȴ���������(ֻ����һ������:���ڰ���)
 *******************************************************************************/
//int f_fingerInitAll(unsigned *pMsg)
int f_fingerDelAllFP(unsigned *pMsg)
{
	func_t func;
//	msg_t msg;
//	unsigned char ucResp;
//	unsigned char ucRet;
//	unsigned char ucArr[12];
	int iTmp = 0;
//	charData_t charData;

//	msg.msgType = ((msg_t *)pMsg)->msgType;
//	msg.msgValue = ((msg_t *)pMsg)->msgValue;

    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10)
		{
    		init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
    		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		}
		break;
		
	case CPMT_OVER:
		promptInit();
		break;
			
	case CMSG_INIT:
		g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		FP_INIT();
		beep();
		break;
		
	case CMSG_DKEY:					/** ��⵽�������� **/
		switch(((msg_t *)pMsg)->msgValue) {
		case CKEY_GOOUT:	//���ڰ�����Ϊ��ʽ��ʼ
		case CKEY_SET:		//���ڰ��ֻ����ü���Ϊ��ʽ��ʼ
    		init_fstack(&g_fstack);
			func.func = f_fingerDelAllFPDone;
    		stackpush(&g_fstack, &func);
    		
    		g_tick = 0;
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
    		
    		iTmp = resetOP_getValue();
			iTmp++;
    		if(iTmp == 0xa2) {
				init_fstack(&g_fstack);
				func.func = f_fingerDelAllFPDoing;
    			stackpush(&g_fstack, &func);
    			
			} else if((iTmp < 0xa2) && (iTmp > 0xa0)) {	/** 0xa1 **/
				resetOP_setValue(iTmp);
				flashWrite(g_flash.arrInt);
			} else {									/** δ֪��ֵ **/
				resetOP_setValue(0xa0);
				flashWrite(g_flash.arrInt);
			}
			break;
		default:
			break;
		}
		break;
		
	case CMSG_FGINIT:
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		break;
				
	default:
		break;
    }	

	return	0;
}


/*******************************************************************************
 * ɾ��ȫ��ָ��
 * �ӱ����Ͽ�����ɾ��ȫ��ָ�Ƽ�ָ�Ƶ�ȫ���������롰ɾ����һ�����߹�Ͻ��ȫ��ָ�Ƽ�ȫ��������Ч����ͬ
 *******************************************************************************/
//int f_fingerInitAllDoing(unsigned *pMsg)
int f_fingerDelAllFPDoing(unsigned *pMsg)
{
	func_t func;
//	msg_t msg;
	
    switch(((msg_t *)pMsg)->msgType) {		
	case CMSG_INIT:
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CFIGER_DEL);
   		MFingerPower_on();			/** ��ָ��ͷ����, ���ȴ�1sec��ʼ���� **/
   		beep();
   		promptDelay(TIMER_100MS);
		break;
		
	case CFIGER_DEL:
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		g_tick = 0;	
   		iSearchCount = 0;		/** ����ִ�д��� **/
   		
    	fingerTab_fill(0xff, 0xff);				/** delte all FP **/
		resetOP_setValue(0xa0);
		flashWrite(g_flash.arrInt);	
		#if	1
   			FP_DelByManager(0);
   		#else
   			FP_DelAll();
   		#endif
		break;
		
	case CMSG_TMR:
		g_tick++;
		
		if(g_tick >= 10) {
		GOTO_DELETE_FAILED:
    		init_fstack(&g_fstack);
			func.func = f_fingerDelFail;
    		stackpush(&g_fstack, &func);
    		g_tick = 0;
    		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		}
		break;
		
	case CPMT_OVER:
		promptInit();
		break;
		
	case CMSG_FGDERG:				//ָ��ɾ���ɹ�
		MFingerPower_off();			/** ָ��ͷ�ϵ� **/
		init_fstack(&g_fstack);
		func.func = f_fingerDelAllFPDone;
    	stackpush(&g_fstack, &func);
    		
    	g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);	
		break;
		
	case CMSG_FGOPFAIL:	
		iSearchCount++;
		if(iSearchCount < 3) {
			#if	1
   				FP_DelByManager(0);
   			#else
   				FP_DelAll();
   			#endif
   		} else {
   			goto GOTO_DELETE_FAILED;
   		}
		break;
		
	default:
		break;
    }	

	return	0;
}

/*******************************************************************************
 * ������ɣ��ȴ���ʾ������
 *******************************************************************************/
//int f_fingerInitAllDone(unsigned *pMsg)
int f_fingerDelAllFPDone(unsigned *pMsg)
{
	func_t func;
//	msg_t msg;
	
	int iTmp = 0;

    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10)
		{
		GOTO_INITALL:
    		init_fstack(&g_fstack);
			func.func = f_UnlockFail;
    		stackpush(&g_fstack, &func);
    		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		}
		break;
		
	case CPMT_OVER:
		promptInit();
		goto GOTO_INITALL;
		//break;
		
	case CMSG_INIT:
		iTmp = resetOP_getValue();
    	if(iTmp == 0xa0) {		/** 0xa1->0xa2(then set 0xa0), then delte all FP **/
    	
    		vp_stor(CVOPID_DEL);
    		vp_stor(CVOPID_ALL);
    		vp_stor(CVOPID_FINGER);
    		vp_stor(CVOPID_SUCESS);
			lbeep();
			promptDelay(TIMER_1SEC);
			lbeep();
			ledPrompt(TIMER_100MS);
		} else if((iTmp < 0xa2) && (iTmp > 0xa0)) {		/** 0xa0->0xa1 **/
			lbeep();
		} else {
			bbbeep();
		}

    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		g_tick = 0;
		break;
		
	default:
		break;
    }	

	return	0;
}

/*******************************************************************************
 * Only search fingerPrint before register fingerPrint
 * Description:
 * initial FingerPrint and play VOP. VOP always longer than initial.
 * end of the VOP, then compare the FP
 * �˵�����--->����¼ָ������֮ǰ��ָ��ͷ�ϵ磬��ʼ��ָ��ͷ
 *******************************************************************************/
//int f_RgstfingerBefore(unsigned *pMsg)
int f_RgstfingerInit(unsigned *pMsg)
{
	func_t func;
//	msg_t msg;

//	unsigned char ucRet;
//	unsigned char ucArr[12];
	
//	msg.msgType = ((msg_t *)pMsg)->msgType;
//	msg.msgValue = ((msg_t *)pMsg)->msgValue;

    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10)		/** ��ʱ�˳� **/
		{
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
		}
		break;
		
	case CPMT_OVER:
		promptInit();
		g_tick = 0;
		
		if(g_subState >= 4) {	//the FP initial OK
			#if	1
			init_fstack(&g_fstack);
			func.func = f_Rgstfinger;
    		stackpush(&g_fstack, &func);
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);	/** ��ʼ��ָ��ͷ���1sec��, �ٿ�ʼ�������� **/
    		#endif
    	} else {
    		g_tick = 0;
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
    	}
		break;
		
	case CMSG_INIT:
		promptInit();
    	if(fingerTab_isEmpty()) {	/** ָ��ģ��գ�ֱ�ӿ�ʼ¼ָ�� **/
			init_fstack(&g_fstack);
			func.func = f_RgstAdminfingerInit;
    		stackpush(&g_fstack, &func);
			g_tick = 0;
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		} else {
    		MFingerPower_on();			/** ָ��ͷ���� **/
			g_tick = 0;
    		SetTimer_irq(&g_timer[0], TIMER_1SEC, CFIGER_INIT);
    		g_subState = 0;			//initial
    		
    		vp_stor(CVOPID_COMPARISON);
    		vp_stor(CVOPID_MANAGE);
    		vp_stor(CVOPID_FINGER);
    		vp_stor(CVOPID_PRESSFINGER);
			beep();
		}
		break;
		
	case CFIGER_INIT:
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
    	g_subState++;				//Fingerprint initial ok
		FP_INIT();					/** ָ��ͷ��ʼ�� **/	
		break;
		
	case CMSG_FGINIT:	/** ָ��ͷ��ʼ��������� **/
		#if	0
		init_fstack(&g_fstack);
		func.func = f_Rgstfinger;
    	stackpush(&g_fstack, &func);
    	SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);	/** ��ʼ��ָ��ͷ���1sec��, �ٿ�ʼ�������� **/
    	#endif
    	
    	g_subState += 4;	//Fingerprint initial ok
		break;
				
	default:
		break;
    }	

	return	0;
}

/*******************************************************************************
 * Only play voice prompt register administrator fingerPrint
 * ��ѯָ��ģ������м�鵽3��--->����¼ָ������֮ǰ���³�ʼ��ָ��ͷ
 *******************************************************************************/
int f_RgstAdminfingerInit(unsigned *pMsg)
{
	func_t func;
//	msg_t msg;

    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10)		/** ��ʱ�˳� **/
		{
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
		}
		break;
		
	case CPMT_OVER:
		promptInit();
		g_tick = 0;
		
		if(g_subState >= 4) {	//the FP initial OK
    		init_fstack(&g_fstack);
			func.func = f_RgstAdminfingerDoing;
    		stackpush(&g_fstack, &func);
    		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_INIT);	/** ��ʼ��ָ��ͷ���1sec��, �ٿ�ʼ�������� **/
    		//bbeep();
    	} else {
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
    	}
		break;
		
	case CACT_OVER:
		actionInit();
		break;
		
	case CMSG_INIT:
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CFIGER_INIT);
    	MFingerPower_on();			/** ָ��ͷ���� **/
    	g_subState = 0;
    	
		promptInit();
    	vp_stor(CVOPID_EROLL);
    	vp_stor(CVOPID_MANAGE);
    	vp_stor(CVOPID_FINGER);
    	vp_stor(CVOPID_PRESSFINGER);
		beep();
		break;
		
	case CFIGER_INIT:
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
    	g_subState++;				//Fingerprint initial ok
		FP_INIT();					/** ָ��ͷ��ʼ�� **/	
		break;
		
	case CMSG_FGINIT:
		#if	0
		init_fstack(&g_fstack);
		func.func = f_RgstAdminfingerDoing;
    	stackpush(&g_fstack, &func);
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_INIT);	/** ��ʼ��ָ��ͷ���1sec��, �ٿ�ʼ�������� **/
    	#endif
    	
    	g_subState += 4;	//Fingerprint initial ok
		break;
				
	default:
		break;
    }	

	return	0;
}


/*******************************************************************************
 * Only search fingerPrint before register fingerPrint
 * ��ѯָ��ģ������м�鵽3��-����¼ָ������(�����ϵ磬���³�ʼ�������Ѿ����)
 *******************************************************************************/
int f_Rgstfinger(unsigned *pMsg)
{
	func_t func;
//	msg_t msg;

//	msg.msgType = ((msg_t *)pMsg)->msgType;
//	msg.msgValue = ((msg_t *)pMsg)->msgValue;

    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_TMR:
		g_tick++;
		break;
		
	case CPMT_OVER:
		promptInit();
		break;		
		
	case CACT_OVER:
		actionInit();
		break;
		
	case CMSG_INIT:
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
    	
		FP_Query(0, 2);		/** ����(������)ָ�Ʊȶ����� **/
		iSearchCount = 0;	/** �ȶԴ������� **/
		break;
		
	case CMSG_FGQUERY:
		if(g_ucUserId < 3)
		{
			/** ����ԱȨ��ͨ�������չ���Աid¼ָ�� **/
			g_tick = 0;
			SetTimer_irq(&g_timer[0], TIMER_4SEC, CMSG_INIT);	/** 3���Ӻ����һ��CMSG_INIT��Ϣ **/
			g_subState = 0;
			
			promptInit();
			bbeep();
    		vp_stor(CVOPID_COMPARISON);
    		vp_stor(CVOPID_PASSED);		
			ledPrompt(TIMER_100MS);
			vp_stor(CVOPID_PRESSFINGER);	
			
			lbeep();
			//promptDelay(TIMER_1SEC);
			
			init_fstack(&g_fstack);
			func.func = f_RgstfingerDoing;
    		stackpush(&g_fstack, &func);
		}
		else
		{
			iSearchCount++;
			if(iSearchCount < 3)
			{
				FP_Query(0, 2);	/** �ȶ�ʧ�ܣ����·���ָ�Ʊȶ����� **/
			}
			else
			{
				init_fstack(&g_fstack);
				func.func = f_fingerEnrollFail;
    			stackpush(&g_fstack, &func);
    			SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);		/** CMSG_INIT��Ϣ **/
			}
		}
		
		break;
		
	case CMSG_FGOPFAIL:		/** ����ʧ�� **/
		iSearchCount++;
		if(iSearchCount < 3)
		{
			FP_Query(0, 2);	/** �ȶ�ʧ�ܣ����·���ָ�Ʊȶ����� **/
		}
		else
		{
			init_fstack(&g_fstack);
			func.func = f_fingerEnrollFail;
    		stackpush(&g_fstack, &func);
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);		/** CMSG_INIT��Ϣ **/
		}
		break;
		
	default:
		break;
	}
	
    return  0;
}

/*******************************************************************************
 * register Administrator fingerPrint
 *  ����û��ָ�ƣ������￪ʼֱ��¼��һ������ָ��
 *******************************************************************************/
int f_RgstAdminfingerDoing(unsigned *pMsg)
{
	func_t func;
	msg_t msg; 
//	charData_t charData;

	msg.msgType = ((msg_t *)pMsg)->msgType;
	msg.msgValue = ((msg_t *)pMsg)->msgValue;

    switch(msg.msgType)
	{
	case CMSG_INIT:
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
    	
		ucRgstCnt = 1;
		FP_Register(0, ucRgstCnt);
		
		break;
		
	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10)
		{
			/** ��ʱ�˳� **/
			msg.msgType = CMSG_INIT;
			inq_irq(&g_msgq, &msg);
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
		}
		break;
		
	case CFIGER_TOUT:
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);	/**  **/
		break;
		
	case CMSG_FGOPFAIL:
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		init_fstack(&g_fstack);
		func.func = f_fingerEnrollFail;
    	stackpush(&g_fstack, &func);
		break;	
	
	case CFIGER_ON:		/** ��⵽ָ�ư���, �ر�������ʾ�����ٸ��� **/
		//promptInit();
		break;
			
	case CFIGER_FGPRPRD:
		g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		ucRgstCnt++;
		FP_Register(0, ucRgstCnt);
		break;	
		
	case CMSG_FGRGST:	
		g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_3SEC, CFIGER_FGPRPRD);	/** ��ʱ **/
		if(ucRgstCnt == 1) {
			promptInit();
			beep();
			vp_stor(CVOPID_PLEASEEROLLAGAIN);
			beep();
			//ucRgstCnt++;
			//FP_Register(0, ucRgstCnt);
		} else if(ucRgstCnt == 2) {
			/** ע��ɹ� **/
			fingerTab_setEntry(0, 0xa5);
			if(MFP_isON()) {		/** ��ָ��ͷ�Ƿ��и�Ӧ�ϵ繦�� **/
				MFP_InductionPower_enable();
			} else {
				MFP_InductionPower_disable();
			}
			flashWrite(g_flash.arrInt);
			MFingerPower_off();			/** ָ��ͷ�ϵ� **/
					
			promptInit();
			g_tick = 0;
			SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
			func.func = f_RgstAdminfingerDone;
    		stackpush(&g_fstack, &func);
		}
		break;
		
	case CPMT_OVER:
		promptInit();		/** ֹͣ����Ϣ�ķ��� **/
		break;
		
	case CACT_OVER:
		actionInit();		/** ֹͣ����Ϣ�ķ��� **/
		break;
		
	default:
		break;
	}

	return	0;
}

/*******************************************************************************
 * ��һ��¼��ָ����ɡ������п�����������Ҫ������������ʾ������ɣ������˳�
 *******************************************************************************/
int f_RgstAdminfingerDone(unsigned *pMsg)
{
	func_t func;
//	msg_t msg; 
//	static int complete = 0;
//	msg.msgType = ((msg_t *)pMsg)->msgType;
//	msg.msgValue = ((msg_t *)pMsg)->msgValue;

    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_INIT:
		g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		
		promptInit();
		beep();
    	vp_stor(CVOPID_FINGER);
		vp_stor(CVOPID_EROLL);
    	vp_stor(CVOPID_SUCESS);
		bbeep();
		ledPrompt(TIMER_100MS);		/** ע��ɹ�led��ʾ **/
		break;	
		
	case CMSG_TMR:
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
				
	case CPMT_OVER:
		promptInit();		/** ֹͣ����Ϣ�ķ��� **/
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

	return 0;
}
/*******************************************************************************
 * register��fingerPrint
 *******************************************************************************/
int f_RgstfingerDoing(unsigned *pMsg)
{
	func_t func;
	msg_t msg; 	
			   
//	int i;
	unsigned char ucRet;
//	unsigned char ucResp;
//	unsigned char ucArr[12];
//	charData_t charData;
	
	msg.msgType = ((msg_t *)pMsg)->msgType;
	msg.msgValue = ((msg_t *)pMsg)->msgValue;

    switch(msg.msgType)
	{
	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10)
		{
			/** ��ʱ�˳�������֮ **/
			bbbeep();
			
			msg.msgType = CMSG_INIT;
			inq_irq(&g_msgq, &msg);
			
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
		}
		break;
		
	case CMSG_INIT:		/** ��ʼ¼��ָ�� **/
		g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);	/** ��ʼ����ʱ�� **/
		ucRgstCnt = 1;
		g_ucAddUserId = FP_AddByManager(g_ucUserId, ucRgstCnt);
		if(g_ucAddUserId == 0)
		{
			msg.msgType = CMSG_INIT;
			inq_irq(&g_msgq, &msg);
			
			func.func = f_fingerEnrollFail;
    		stackpush(&g_fstack, &func);
		}
		else
		{
		}		

		#if	0	/** debug **/
		charData.ucVal = 0xAA;
		charQueueIn_irq(&g_com1TxQue, &charData); 
		
		charData.ucVal = g_ucUserId;  
		charQueueIn_irq(&g_com1TxQue, &charData);
		charData.ucVal = g_ucAddUserId;  
		charQueueIn_irq(&g_com1TxQue, &charData);
                            
		charData.ucVal = 0x55;                    
		charQueueIn_irq(&g_com1TxQue, &charData); 
		#endif
		
		break;
			
	case CMSG_FGOPFAIL:		/** ¼�����ʧ�� **/
		g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);	/** ��ʼ����ʱ�� **/

		init_fstack(&g_fstack);		
		func.func = f_fingerEnrollFail;
    	stackpush(&g_fstack, &func);
		break;	
	
	case CFIGER_ON:		/** ��⵽ָ�ư���, �ر�������ʾ�����ٸ��� **/
		//promptInit();
		break;
			
	case CFIGER_FGPRPRD:
		g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		
		ucRgstCnt++;
		ucRet = FP_AddByManager(g_ucUserId, ucRgstCnt);
		if(ucRet != g_ucAddUserId)
		{
			g_ucAddUserId = ucRet;
			SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);	/** ��ʼ����ʱ�� **/
				
			init_fstack(&g_fstack);	
			func.func = f_fingerEnrollFail;
    		stackpush(&g_fstack, &func);
		}
		break;	
			
	case CMSG_FGRGST:
		if(ucRgstCnt == 1)
		{
			#if	1
			promptInit();
			vp_stor(CVOPID_PLEASEEROLLAGAIN);
			beep();
			SetTimer_irq(&g_timer[0], TIMER_3SEC, CFIGER_FGPRPRD);
			#else
			ucRgstCnt++;
			ucRet = FP_AddByManager(g_ucUserId, ucRgstCnt);
			if(ucRet != g_ucAddUserId)
			{
				g_ucAddUserId = ucRet;
				SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);	/** ��ʼ����ʱ�� **/
				
				init_fstack(&g_fstack);	
				func.func = f_fingerEnrollFail;
    			stackpush(&g_fstack, &func);
			}
			#endif
		}
		else if(ucRgstCnt == 2)
		{
			/** ע��ɹ�, �޸������� **/
			fingerTab_setEntry(g_ucAddUserId, 0xa5);
			flashWrite(g_flash.arrInt);
			
			g_tick = 0;
			SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
			
			//bbeep();
			init_fstack(&g_fstack);
			func.func = f_fingerEnrollSuccess;
    		stackpush(&g_fstack, &func);
		}
		
		break;
			
	case CPMT_OVER:
    	promptInit();		/** ֹͣ����Ϣ�ķ��� **/
		//MOPBlue_on();			/** ��˸֮��ָ����� **/
		MOPBlue_setDefaultStatus();	/** �ָ�ָʾ���뱳��һ�� **/
		break;
		
	default:
		break;
	}

	return	0;
}


/*******************************************************************************
 * ָ�ƹ�������ɹ�
 *******************************************************************************/
int f_fingerEnrollSuccess(unsigned *pMsg)
{
	func_t func;
	//msg_t msg;
//	charData_t charData;
//	unsigned char ucResp;
//	unsigned char ucRet;
//	unsigned char ucArr[12];
	
    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_INIT:
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);		/** CMSG_INIT��Ϣ **/
		g_tick = 0;
		MFingerPower_off();			/** ָ��ͷ�ϵ� **/
		
		promptInit();
		vp_stor(CVOPID_EROLL);
		vp_stor(CVOPID_SUCESS);
		bbeep();
		ledPrompt(TIMER_100MS);		/** �ɹ���ʾ��˸ **/
		break;
		
	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10)
		{
			/** ��ʱ�˳� **/
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		break;

	case CPMT_OVER:
    	promptInit();			/** ��ʾ��������ֹͣ����Ϣ�ķ��� **/
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
 * ��ָ���м�⵽4��������ɾ��ָ��״̬��������ʾ��
 *******************************************************************************/
//int f_DegstfingerInit(unsigned *pMsg)
int f_fingerDeleteInit(unsigned *pMsg)
{
	func_t func;
	//msg_t msg;
//	charData_t charData;
//	unsigned char ucRet;
//	unsigned char ucArr[12];

    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_TMR:
		g_tick++;

		break;
		
	case CMSG_INIT:
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
    	FP_INIT();
    	beep();
    	vp_stor(CVOPID_COMPARISON);
    	vp_stor(CVOPID_MANAGE);
    	vp_stor(CVOPID_FINGER);
    	vp_stor(CVOPID_PRESSFINGER);
    	beep();							/** ��ʾ���ȴ�1sec **/
    	    	
    	g_subState = 0;
		break;

	case CMSG_FGINIT:
		g_tick = 0;
		SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		break;
		
	case CPMT_OVER:
    	promptInit();			/** ��ʾ��������ֹͣ����Ϣ�ķ��� **/
		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		
		init_fstack(&g_fstack);
		func.func = f_fingerDelete;
    	stackpush(&g_fstack, &func);
		break;
		
	default:
		break;
	}
    
    return  0;
}

/*******************************************************************************
 * Deregister��fingerPrint
 *******************************************************************************/
//int f_Degstfinger(unsigned *pMsg)
int f_fingerDelete(unsigned *pMsg)
{
	func_t func;
	//msg_t msg;
//	charData_t charData;
//	unsigned char ucResp;
//	unsigned char ucRet;
//	unsigned char ucArr[12];
	
    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10)
		{
			/** ��ʱ�˳� **/
			init_fstack(&g_fstack);
			func.func = f_fingerDelFail;
    		stackpush(&g_fstack, &func);
			SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		
		break;
		
	case CMSG_INIT:
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);	
		FP_Query(0, 2);		/** ����(������)ָ�Ʊȶ����� **/
		break;
		
	case CMSG_FGQUERY:
		/** Ȩ��ͨ��, ɾ��ָ�� **/
		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		init_fstack(&g_fstack);
		func.func = f_fingerDeleteDoing;
    	stackpush(&g_fstack, &func);
		break;
	
	case CMSG_FGOPFAIL:
		iSearchCount++;
		if(iSearchCount < 3)
		{
			FP_Query(0, 2);	/** �ȶ�ʧ�ܣ����·���ָ�Ʊȶ����� **/
		}
		else
		{
			init_fstack(&g_fstack);
			func.func = f_fingerDelFail;
    		stackpush(&g_fstack, &func);
			SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		break;
				
	case CPMT_OVER:	
    	promptInit();		/** ֹͣ����Ϣ�ķ��� **/
		SetTimer_irq(&g_timer[0], TIMER_200MS, CMSG_INIT);
		init_fstack(&g_fstack);
		func.func = func_idle;
    	stackpush(&g_fstack, &func);
		
		break;
	default:
		break;
	}
    
    return  0;
}

/*******************************************************************************
 * Deregister��fingerPrint
 *******************************************************************************/
//int f_DegstfingerDoing(unsigned *pMsg)
int f_fingerDeleteDoing(unsigned *pMsg)
{
	func_t func;
	//msg_t msg;
	//charData_t charData;
//	unsigned char ucResp;
//	unsigned char ucRet;
//	unsigned char ucArr[12];
	
    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10)
		{
			/** ��ʱ�˳� **/
    		init_fstack(&g_fstack);
			func.func = f_fingerDelFail;
    		stackpush(&g_fstack, &func);
			SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		break;
		
	case CMSG_INIT:
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CFIGER_DEL);
		g_tick = 0;
		break;

	case CFIGER_DEL:
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
    	
		FP_DelByManager(g_ucUserId);
		fingerTab_fill(g_ucUserId, 0xff);
		flashWrite(g_flash.arrInt);	
		
		promptInit();
    	beep();	
		vp_stor(CVOPID_COMPARISON);
		vp_stor(CVOPID_PASSED);
    	ledPrompt(TIMER_100MS);
		break;
	
	case CMSG_FGOPFAIL:
		init_fstack(&g_fstack);
		func.func = f_fingerDelFail;
    	stackpush(&g_fstack, &func);
		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		break;
			
	case CMSG_FGDERG:
    	SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
    	
		g_subState = 0;
		init_fstack(&g_fstack);
		func.func = f_fingerDeleteDone;
    	stackpush(&g_fstack, &func);
		break;

	case CPMT_OVER:
		promptInit();		/** ��ʾ��������ֹͣ����Ϣ�ķ��� **/
		//MOPBlue_on();			/** ��˸֮��ָ����� **/
		MOPBlue_setDefaultStatus();	/** �ָ�ָʾ���뱳��һ�� **/
		break;
	default:
		break;
	}
    
    return  0;
}


/*******************************************************************************
 * Deregister��fingerPrint
 *******************************************************************************/
//int f_DegstfingerDone(unsigned *pMsg)
int f_fingerDeleteDone(unsigned *pMsg)
{
	func_t func;

    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10)
		{
			/** ��ʱ�˳� **/
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
		}
		break;
		
	case CMSG_INIT:
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		g_tick = 0;
 		g_subState = 0;
 		lbeep();
		vp_stor(CVOPID_DEL);
		vp_stor(CVOPID_SUCESS);
		break;

	case CPMT_OVER:
		//MOPBlue_on();			/** ��˸֮��ָ����� **/
		MOPBlue_setDefaultStatus();	/** �ָ�ָʾ���뱳��һ�� **/
		
		g_subState++;
 		if(g_subState == 1)
 		{
 			promptInit();			/** ��ʾ��������ֹͣ����Ϣ�ķ��� **/
    		bbeep();
    		ledPrompt(TIMER_100MS);
 		}
 		else if(g_subState == 2)
 		{
			init_fstack(&g_fstack);
			func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
    		
			g_tick = 0;
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
    	}
    	
		break;
		
	default:
		break;
	}
    
    return  0;
}

/*******************************************************************************
 * f_keyFirstTouch
 * ��ָ��ʱ�ĵ�һ�δ���(������ܵ�¼��ָ�ƹ���)
 *******************************************************************************/
int f_keyFirstTouch(unsigned *pMsg)
{
//	msg_t msg;
	func_t func;

//	msg.msgType = ((msg_t *)pMsg)->msgType;
//	msg.msgValue = ((msg_t *)pMsg)->msgValue;

    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_INIT:
		lockAction();
		vp_stor(CVOPID_EROLLPSWORDFORYOURSECURITY);
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		break;
	
	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10) {
			init_fstack(&g_fstack);
    		func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		break;
	
	case CPMT_OVER:
		promptInit();
		break;
		
	case CACT_OVER:	/** ���û�κ������������ͻ�Ӵ��˳� **/
		actionInit();
		init_fstack(&g_fstack);
    	func.func = f_Unlock;
    	stackpush(&g_fstack, &func);
    	SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		break;
		
	case CMSG_DKEY:					/** ��⵽�������� **/
		MBlackLED_on();			/** ���� **/
		MOPBlue_on();			/** �뱳��һ���ָʾ�� **/
		
		promptInit();
		beep();
		break;
					
	default:
		break;
	}
    return  0;
}


/*******************************************************************************
 * ��ָ��ģ�徯��
 *******************************************************************************/
int f_fingerNoFPWarn(unsigned *pMsg)
{
	func_t func;
	   
    switch(((msg_t *)pMsg)->msgType)
	{
	case CMSG_TMR:
		g_tick++;
		if(g_tick >= 10) {
			init_fstack(&g_fstack);
    		func.func = f_Unlock;
    		stackpush(&g_fstack, &func);
    		SetTimer_irq(&g_timer[0], TIMER_100MS, CMSG_INIT);
		}
		break;
		
	case CMSG_INIT:	
		promptInit();
		beep();
		vp_stor(CVOPID_EROLLPSWORDFORYOURSECURITY);
		g_tick = 0;
    	SetTimer_irq(&g_timer[0], TIMER_1SEC, CMSG_TMR);
		break;

	case CPMT_OVER:
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
/////////////////////////////////////////////////////

