
#include "stm32f10x_lib.h"

#include "../inc/CONST.h"
#include "../inc/ctype.h"  
#include "../inc/macro.h"
#include "../inc/global.h"	
#include "../inc/debug.h"

#include "arch.h"	
#include "driver.h"		  
#include "charQue.h"	  
#include "l_fingprint.h"

/*******************************************************************************
 *
 * sysProcess
 * input: data - msg
 * output: none
 * return: TRUE - process ok
 *         FALSE - Can't process
 * descript: 
 *
 *******************************************************************************/
int sysProcess(msg_t *val)
{
    int iRet = TRUE;
    msg_t msg;
    static unsigned int samplingVolt[16] = {0};
    static int	samplingCNT = 0;
    
    unsigned char ucResp;
	unsigned char ucRet;
	unsigned char ucArr[12];
    
    //charData_t charData;
    //int	iADCref = 0;
    //int iADCCurr = 0;
	//int i;
   
    switch(val->msgType)
	{
	case CACT_TOUT:			/** һ�ζ������ **/
		actProcess(&g_actionQueue);
		break;
		
	case CPMT_TOUT:			/** һ����ʾ������� **/
		actProcess(&g_promptQueue);
		break;
	case CMSG_CALC:
		samplingCNT = 0;
		g_flag &= ~(1<<8);
		break;
		
	case CMSG_ADC:			/** 0.512s�Ķ�ʱ�� **/
		if((g_flag & (1<<8)) == 0)		/** ���û�з����͵籨��, ��һֱ��ȡ���� **/
		{
			samplingVolt[samplingCNT & 0x0f] = ADC_GetConversionValue(ADC1);
			samplingCNT++;
			
			if((samplingCNT & 0x0f) == 0)		/** ���16�β��� **/
			{
				g_flag |= (1<<8);		/** ��ѹ������� **/
				arrSort(samplingVolt, 16);
				samplingCNT = 0;
				g_iADCCurr = arrAverage(samplingVolt, 4, 11);	/** the average volt **/
				
				g_iADCref = ADC_getValue();
				if(g_iADCCurr < g_iADCref)
				{
					g_flag |= (1 << 9);		/** �͵籨�� **/
				}
			}
		}
		break;
		
	#if	1
	case CMSG_COMRX:	/** ���������� **/
		ucRet = FP_GetRespData(&g_comRevBuf, ucArr);
		if(ucRet != 0)			/** �յ���Ч���� **/
		{
				#if	0
					charData.ucVal = 0xAA;
					charQueueIn_irq(&g_com1TxQue, &charData); 
					for(i = 0; i < 12; i++)
					{
						charData.ucVal = ucArr[i];
						charQueueIn_irq(&g_com1TxQue, &charData);
					} 
                	                    
					charData.ucVal = 0x55;                    
					charQueueIn_irq(&g_com1TxQue, &charData); 
				#endif
				
			ucRet = FP_GetRespType(ucArr);
			switch(ucRet)
			{
			case CFP_INIT:
				ucRet = FP_RespInit(ucArr, ucRet);
				if(ucRet  == 0xaa)	/** ��ʼ��-���� **/ 
				{
					/** ָ��ͷ��ʼ��-�ɹ� **/
					msg.msgType = CMSG_FGINIT;
					inq_irq(&g_msgq, &msg);
				}
				else if(ucRet  == 0xab)
				{
					/** busy, wait for responsed **/
					
				}
				else
				{
					/** ָ��ͷ��ʼ��ʧ�� **/
					msg.msgType = CMSG_FGOPFAIL;
					inq_irq(&g_msgq, &msg);
				}
				break;
				
			case CFP_ENROLL:
				ucRet = FP_RespRgst(ucArr, &ucResp);
				if(ucRet == 0xaa)
				{
					/** ע��ָ��-���� **/
					msg.msgType = CMSG_FGRGST;
					inq_irq(&g_msgq, &msg);
				}
				else if(ucRet  == 0xab)
				{
					/** busy, wait for responsed **/
				}
				else
				{
					/** ָ��ע��-ʧ�� **/
					msg.msgType = CMSG_FGOPFAIL;
					inq_irq(&g_msgq, &msg);
				}
				break;
				
			case CFP_MATCH:
				ucRet = FP_RespSearch(ucArr, &g_ucUserId);
				if(ucRet == 0xaa)
				{
					/** ����ָ��-�ɹ� **/
					msg.msgValue = g_ucUserId;
					msg.msgType = CMSG_FGQUERY;
					inq_irq(&g_msgq, &msg);
				}
				else if(ucRet  == 0xab)
				{
					/** busy, wait for responsed **/
				}
				else
				{
					/** ����ָ��-ʧ�� **/
					msg.msgType = CMSG_FGOPFAIL;
					inq_irq(&g_msgq, &msg);
				}
				break;
				
			case CFP_DELETE:
				ucRet = FP_RespDeRgst(ucArr, &g_ucUserId);
				if(ucRet == 0xaa)
				{
					/** ɾ��ָ��-�ɹ� **/
					msg.msgType = CMSG_FGDERG;
					inq_irq(&g_msgq, &msg);
				}
				else if(ucRet  == 0xab)
				{
					/** busy, wait for responsed **/
				}
				else
				{
					/** ɾ��ָ��-ʧ�� **/
					msg.msgType = CMSG_FGOPFAIL;
					inq_irq(&g_msgq, &msg);
				}
				break;
							
			default:
				break;
			}
		}
		break;
	#endif
		
	default:
		/** Can't process the msg here **/
		iRet = FALSE;

		break;
	}
	
	return  iRet;
}

/*******************************************************************************
 * DAEMON_USARTx_Send()
 *******************************************************************************/
void DAEMON_USARTx_Send(USART_TypeDef* USARTx, charBuf_queue_t* comTxQue)
{
    charData_t   comData;
    //static unsigned char iTmp;

    if(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == SET)
    {
        if(charQueueOut_irq(comTxQue, &comData) == TRUE)
        {
            USART_SendData(USARTx, comData.ucVal);
        }
    }
}

/*******************************************************************************
 * keyscan()
 * PA.5 --- INT(���ְ���)
 * PC.5 --- TO()
 * PA.7 --  SET(���ü�)
 *******************************************************************************/
#define CKEYINIT		(0xff)
const unsigned char keyMap[] = {
		0x00, 0x01, 0x02, 0x03,
		0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0a, 0x0b,
		0x0c, 0x0d, 0x0e, 0x0f
};

void keyscan(void)
{
	static unsigned char key_old = CKEYINIT; 
	static unsigned char key = CKEYINIT;
//	charData_t charData;
	static unsigned char ucKey = CKEYINIT;
	msg_t msg;
//	int i;
	
	/** ��ⰴ��״̬�ı仯 **/
	if((g_flag & (1 << 7)) == 0) {	/** ��ǰû�д��������ְ��� **/
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5) == 1) {	/** ���°��� **/
			if((g_flag & (1 << 14)) == 0) {		/** ��û��⵽���°��� **/
				msg.msgType = CMSG_BACK;	//!!!!!!!!!!!!!!!!!
				inq_irq(&g_msgq, &msg);		//!!!!!!!!!!!!!!!!!
			}
		} else {	/** �ɿ����� **/
			if((g_flag & (1 << 14)) != 0) {		/** ��û��⵽�ɿ����� **/
				g_flag |= (1 << 7);
			}
		}	
	} else {
		if(key != key_old) {					/** �������� **/
			msg.msgValue = keyMap[ucKey];	/** ����ӳ���ֵ **/
			
			if(key != CKEYINIT) {			/** ���°��� **/
				if(g_tmr_key == TIMER_80MS) {
					msg.msgType = CMSG_DKEY;
					inq_irq(&g_msgq, &msg);
				
					g_tmr_key = TIMER_100MS;
					key_old = key;
				}
			} else {					/** �ɿ����� **/
				key_old = key;		
				//g_tmr_key = TIMER_200MS;
				
				msg.msgType = CMSG_UKEY;
				inq_irq(&g_msgq, &msg);
			}
		}
	}
	
	/** ȡ���ְ�����ǰ״̬����ֵPC(3..0) **/
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5) == 1) {	/** key pressed(����Ч) **/
		if((g_flag & (1 << 14)) != 0) {		/** ״̬�ǳ�������? **/
			if(g_tmr_key == TIMER_60MS) {	/** �ѳ���60ms��? **/
				g_tmr_key = TIMER_80MS;
				
				key = (GPIO_ReadInputData(GPIOC) & 0x0f);
				ucKey = key;
				
			}
		} else {
			g_flag |= (1 << 14);
			g_tmr_key = 0;		/** ��ʼ��ʱ **/
		}
	} else {	/** key released **/
		if((g_flag & (1 << 14)) == 0) {		/** ״̬�ǳ�������? **/
			if(g_tmr_key == TIMER_60MS) {	/** �ѳ���60ms��? **/
				g_tmr_key = TIMER_80MS;
				key = CKEYINIT;
			}
		} else {
			g_flag &= ~(1 << 14);
			g_tmr_key = 0;		/** ��ʼ��ʱ **/
		}
	}
	
	/** ȡ���ڳ��Ű���TO��ǰ״̬ **/
	//if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5) == 0)	/** key pressed(����Ч) **/
	if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5) == 1)	/** key pressed(����Ч) **/
	{
		if((g_flag & (1 << 4)) == 0)
		{
			//msg.msgValue = CTOKEY;
			msg.msgValue = CKEY_GOOUT;
			msg.msgType = CMSG_DKEY;
			inq_irq(&g_msgq, &msg);
			
			#if	0
			g_keys.iCount++;
			for(i = CKEYBUFSIZE - 1; i >= 1; i--)		/** ��λ���ڳ���λ **/
			{
				g_keys.buf[i] = g_keys.buf[i - 1];
			}
			g_keys.buf[0] = keyMap[ucKey];	/** �¼�ֵ **/
			#endif
		}
		g_flag |= (1 << 4);
	}
	else
	{
		g_flag &= ~(1 << 4);
	}
	/** ȡ������Ű���TI��ǰ״̬ **/
	//if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_10) == 0)	/** key pressed(����Ч) **/
	if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_10) == 1)	/** key pressed(����Ч) **/
	{
		if((g_flag & (1 << 5)) == 0)
		{
			//msg.msgValue = CTIKEY;
			msg.msgValue = CKEY_GOIN;
			msg.msgType = CMSG_DKEY;
			inq_irq(&g_msgq, &msg);
		}
		g_flag |= (1 << 5);
	}
	else
	{
		g_flag &= ~(1 << 5);
	}
	
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7) == 1)	/** ���ü� **/
	{
		if((g_flag & (1 << 12)) == 0)
		{
			msg.msgValue = CKEY_SET;
			msg.msgType = CMSG_DKEY;
			inq_irq(&g_msgq, &msg);
		}
		g_flag |= (1 << 12);
	}
	else
	{
		g_flag &= ~(1 << 12);
	}
}

/*******************************************************************************
 * fingerOn()
 *******************************************************************************/
void fingerCheck(void)
{
	msg_t msg;
	/** ����ָ��ͷ **/
	//if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == 0)	/** finger pressed(����Ч) **/
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == 1)	/** finger pressed(����Ч) **/
	{
		if((g_flag & (1 << 6)) == 0)
		{
			msg.msgType = CFIGER_ON;
			inq_irq(&g_msgq, &msg);
			
			//MFingerPower_on();			/** ��ָ��ͷ���硣���ʺ��ڴ˴� **/
		}
		g_flag |= (1 << 6);
	}
	else
	{
		g_flag &= ~(1 << 6);
	}
}


/*******************************************************************************
 * noKeyEnter()��Կ�׽���
 *******************************************************************************/
void noKeyEnter(void)
{
	msg_t msg;
	/** ����Կ�׿����ź� **/
	if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11) == 1)	/** ��Կ�׽���(����Ч) **/
	//if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11) == 0)	/** ��Կ�׽���(����Ч) **/
	{
		if((g_flag & (1 << 11)) == 0)
		{
			msg.msgType = CNOKEY_ENTER;
			inq_irq(&g_msgq, &msg);
		}
		g_flag |= (1 << 11);
	}
	else
	{
		g_flag &= ~(1 << 11);
	}
}

/*******************************************************************************
 * vop_busy()��������
 *	(CVOP_WT588D --- һ���ɵ͵��ߵ����䣬��ʾ���Ž���)
 *	(CVOP_GQPP5 --- һ���ɸߵ��͵����䣬��ʾ���Ž���)
 *	(CVOP_NY3P --- һ���ɸ�(�쵽2��)����(�쵽200��)�����䣬��ʾ���Ž���)
 *******************************************************************************/
void vop_busy(void)
{
	#define	CMAXH	2
	#define	CMAXL	200
	msg_t msg;
	static int iCount = 0;
	/* bit0: L
	 * bit1: H
	 * bit2: first L -> H 
	 * bit3: first H -> L
	 */
	static int iVopBusyFlag = 0;
	/** ������busy�ź� **/
	if(g_tmr_iVopBusy >= TIMER_VOPBUSY/** delay for busy check**/) 
	{
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0)		/** MSK3P/NY3P����busy(H->L��Ч) **/
		//if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 1)		/** WT588D����busy(L->H��Ч) **/
		{
			iVopBusyFlag |= (1 << 0);
			if((g_flag & (1 << 13)) == 0)
			{
				iCount++;
				if(iCount >= CMAXL)
				{
					msg.msgType = CPMT_TOUT;
					inq_irq(&g_msgq, &msg);
				
					g_flag |= (1 << 13);
					iCount = 0;
				}
			}
			else
			{
				
			}
		}
		else
		{
			iVopBusyFlag |= (1 << 1);
			if((g_flag & (1 << 13)) != 0)
			{
				iCount++;
				if(iCount >= CMAXH)
				{
					g_flag &= ~(1 << 13);
					iCount = 0;
				}
			}
		}
	}
}

/*******************************************************************************
 * ��������: �����µĶ���
 *******************************************************************************/
static void startAction(actionQueue_t *q, action_t * pAction)
{
	SetTimer_irq(q->timer, pAction->actionTime, q->stepMsgType);
	
	switch(pAction->actionType)
	{
	case	CACT_UNLOCK:
		q->flag = (1<<7);
		MUnLock();
		break;
		
	case	CACT_LOCK:
		q->flag = (1<<7);
		MLock();
		break;
		
	case	CACT_LEDPMTON:
		q->flag = (1<<6);	  
		MOPBlue_on();
		break;
		
	case	CACT_LEDPMTOFF:
		q->flag = (1<<6);
		MOPBlue_off();
		break;
		
	case	CACT_LEDWRNON:
		q->flag = (1<<5);
		MOPRed_on();
		break;
		
	case	CACT_LEDWRNOFF:
		q->flag = (1<<5);
		MOPRed_off();
		break;
		
	case	CACT_BEEPON:
		q->flag = (1<<4);
		VOPDATA_H();
		break;
		
	case	CACT_BEEPOFF:
		q->flag = (1<<4);
		VOPDATA_L();
		break;
		
	case	CACT_DELAY:
		q->flag = (1<<3);
		break;
	
	case	CACT_VOPON:
		q->flag = (1<<2);
		vp_play((unsigned char)pAction->actionPara);
		break;
			
	default:
		/** ����ʶ��Ķ��� **/
		q->flag = 0;
		break;
	}
}

 /*******************************************************************************
 * ��������ֹͣ��ǰ�Ķ���
 *******************************************************************************/
static void stopAction(actionQueue_t *q)
{
	ClrTimer_irq(q->timer);
		/** stop the timeout action  **/
	if(q->flag & (1<<7)) {
		MLockPWR_off();
	} else if(q->flag & (1<<6)) {
		#if	1
			MOPBlue_off();
		#else
			if(MBlackLED_status()/** ��������������ָ��ʾ����ɫָʾ���� **/) {
				MOPBlue_on();
			} else {			 /** ���������...... **/
				MOPBlue_off();
			}
		#endif
	} else if(q->flag & (1<<5)) {
		MOPRed_off();
		#if	0
			if(MBlackLED_status()/** ������ **/) {
				MOPBlue_on();
			} else {			 /** ������ **/
				MOPBlue_off();
			}
		#endif
	} else if(q->flag & (1<<4)) {
		VOPDATA_L();
	} else if(q->flag & (1<<3)) {
		/** delay... do Nothing **/
	} else if(q->flag & (1<<2)) {
		/** stop VP, send stop command when playing **/
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 1)vp_play((unsigned char)CVOP_STOP);
	} else {
	}
}
 /** ��ն������У�����ֹͣ��ǰ���� **/
void promptInit(void)
{
	actionQueueInit(&g_promptQueue, &(g_timer[2]), CPMT_TOUT, CPMT_OVER);
	stopAction(&g_promptQueue);
}

 /** ��ն������У�����ֹͣ��ǰ���� **/
void actionInit(void)
{
	actionQueueInit(&g_actionQueue, &(g_timer[1]), CACT_TOUT, CACT_OVER);
	stopAction(&g_actionQueue);
}
/** �¼����д��� **/
void actionDoing(actionQueue_t * q)
{
	action_t action;
	
	if(q->flag == 0)
	{
		if(actionQueueOut(q, &action) == TRUE)
		{
			startAction(q, &action);
		}
	}
}

/** ϵͳ��ǰ�¼����� **/
void actProcess(actionQueue_t * q)
{
	action_t action;
	
	/** stop the timeout action first  **/
	stopAction(q);
	
	/** start the next action **/
	if(actionQueueOut(q, &action) == TRUE)
	{
		startAction(q, &action);
	}
	else
	{
		if(q->flag != 0)
		{
			q->flag = 0;
			SetTimer_irq(q->timer, TIMER_100MS, q->overMsgType);
		}
	}
}

void promptDelay(int _delay)
{
	action_t action;
    action.actionType = CACT_DELAY;
    action.actionTime = _delay;
    actionQueueIn(&g_promptQueue, &action);
}

void lbeep(void)
{
	action_t action;
	#if	CENABLEVOP
    action.actionType = CACT_VOPON;
    action.actionTime = TIMER_10SEC;
    action.actionPara = CVOPID_LBEEP;
    #else
	action.actionType = CACT_BEEPON;
    action.actionTime = TIMER_1SEC;
	#endif
    actionQueueIn(&g_promptQueue, &action);
}

void beep(void)
{
	action_t action;
	#if	CENABLEVOP
    action.actionType = CACT_VOPON;
    action.actionTime = TIMER_10SEC;
    action.actionPara = CVOPID_BEEP;
    #else
	action.actionType = CACT_BEEPON;
    action.actionTime = TIMER_60MS;
	#endif
    actionQueueIn(&g_promptQueue, &action);
    
    #if	CENABLEVOP
    #else
    action.actionType = CACT_DELAY;			/** ����һ����ʱ **/
    action.actionTime = TIMER_60MS;
    actionQueueIn(&g_promptQueue, &action);
    #endif
}

void bbeep(void)
{
	action_t action;
	#if	CENABLEVOP
    action.actionType = CACT_VOPON;
    action.actionTime = TIMER_10SEC;
    action.actionPara = CVOPID_BEEP;
    #else
	action.actionType = CACT_BEEPON;
    action.actionTime = TIMER_60MS;
	#endif
    actionQueueIn(&g_promptQueue, &action);
    
	#if	CENABLEVOP
	#else
    action.actionType = CACT_DELAY;
    action.actionTime = TIMER_60MS;
    actionQueueIn(&g_promptQueue, &action);
    #endif
    
    #if	CENABLEVOP
    action.actionType = CACT_VOPON;
    action.actionTime = TIMER_10SEC;
    action.actionPara = CVOPID_BEEP;
    #else
	action.actionType = CACT_BEEPON;
    action.actionTime = TIMER_60MS;
    #endif
    actionQueueIn(&g_promptQueue, &action);
}

void bbbeep(void)
{
	action_t action;
	
    #if	CENABLEVOP
    action.actionType = CACT_VOPON;
    action.actionTime = TIMER_10SEC;
    action.actionPara = CVOPID_BEEP;
    #else
	action.actionType = CACT_BEEPON;
    action.actionTime = TIMER_60MS;
	#endif
    actionQueueIn(&g_promptQueue, &action);
    
	#if	CENABLEVOP
	#else
    action.actionType = CACT_DELAY;
    action.actionTime = TIMER_60MS;
    actionQueueIn(&g_promptQueue, &action);
    #endif
    
    #if	CENABLEVOP
    action.actionType = CACT_VOPON;
    action.actionTime = TIMER_10SEC;
    action.actionPara = CVOPID_BEEP;
    #else
	action.actionType = CACT_BEEPON;
    action.actionTime = TIMER_60MS;
	#endif
    actionQueueIn(&g_promptQueue, &action);
    
	#if	CENABLEVOP
	#else
    action.actionType = CACT_DELAY;
    action.actionTime = TIMER_60MS;
    actionQueueIn(&g_promptQueue, &action);
    #endif
    
    #if	CENABLEVOP
    action.actionType = CACT_VOPON;
    action.actionTime = TIMER_10SEC;
    action.actionPara = CVOPID_BEEP;
    #else
	action.actionType = CACT_BEEPON;
    action.actionTime = TIMER_60MS;
	#endif
    actionQueueIn(&g_promptQueue, &action);
}

void vp_stor(unsigned char __vpIdx)
{
	action_t action;
	#if	CENABLEVOP
    action.actionType = CACT_VOPON;
    action.actionTime = TIMER_10SEC;
    action.actionPara = __vpIdx;
    #else
	action.actionType = CACT_DELAY;
    action.actionTime = TIMER_60MS;
	#endif
    actionQueueIn(&g_promptQueue, &action);
}

volatile u8 g_tmr_sbuf;
//void setSbuf(u8 __sbuf)
void vp_play(u8 __vpIdx)
{
    #if	CENABLEVOP
    	IRQ_disable();
    	g_tmr_sbuf = __vpIdx;
   		g_tmr_vop = 0;
    	IRQ_enable();
    #else
    #endif
}

/** ָʾ�ƶ�����ʾ **/
void ledPrompt(int _delay/** ���� **/)
{
	action_t action;
    action.actionType = CACT_LEDPMTON;
    action.actionTime = _delay;
    actionQueueIn(&g_promptQueue, &action);
    
    action.actionType = CACT_LEDPMTOFF;
    action.actionTime = _delay;
    actionQueueIn(&g_promptQueue, &action);
    
    action.actionType = CACT_LEDPMTON;
    action.actionTime = _delay;
    actionQueueIn(&g_promptQueue, &action);
    
    action.actionType = CACT_LEDPMTOFF;
    action.actionTime = _delay;
    actionQueueIn(&g_promptQueue, &action);
    
    action.actionType = CACT_LEDPMTON;
    action.actionTime = _delay;
    actionQueueIn(&g_promptQueue, &action);
    
    action.actionType = CACT_LEDPMTOFF;
    action.actionTime = _delay;
    actionQueueIn(&g_promptQueue, &action);
    
}

/** ָʾ�ƾ�����ʾ **/
void ledWarn(int _delay)	/** ������ʾ **/
{
	action_t action;
    action.actionType = CACT_LEDWRNON;
    action.actionTime = _delay;
    actionQueueIn(&g_promptQueue, &action);
    
    action.actionType = CACT_LEDWRNOFF;
    action.actionTime = _delay;
    actionQueueIn(&g_promptQueue, &action);
    
    action.actionType = CACT_LEDWRNON;
    action.actionTime = _delay;
    actionQueueIn(&g_promptQueue, &action);
    
    action.actionType = CACT_LEDWRNOFF;
    action.actionTime = _delay;
    actionQueueIn(&g_promptQueue, &action);
    
    action.actionType = CACT_LEDWRNON;
    action.actionTime = _delay;
    actionQueueIn(&g_promptQueue, &action);
    
    action.actionType = CACT_LEDWRNOFF;
    action.actionTime = _delay;
    actionQueueIn(&g_promptQueue, &action);
}

#if	0
void Unlock(void)
{
	action_t action;
	action.actionType = CACT_UNLOCK;
    action.actionTime = TIMER_300MS;
    actionQueueIn(&g_actionQueue, &action);
}

void Lock(void)
{
	action_t action;
	action.actionType = CACT_LOCK;
    action.actionTime = TIMER_300MS;
    actionQueueIn(&g_actionQueue, &action);
}
#endif

void lockAction(void)
{
	action_t action;
	/** ��ת���� **/
	#if	1
	action.actionType = CACT_UNLOCK;
    action.actionTime = TIMER_500MS;
    actionQueueIn(&g_actionQueue, &action);
    #else
	action.actionType = CACT_UNLOCK;
    action.actionTime = TIMER_100MS;
    actionQueueIn(&g_actionQueue, &action);
    action.actionType = CACT_LOCK;
    action.actionTime = TIMER_60MS;
    actionQueueIn(&g_actionQueue, &action);
	action.actionType = CACT_UNLOCK;
    action.actionTime = TIMER_300MS;
    actionQueueIn(&g_actionQueue, &action);
    #endif
    
    /** ͣ�� **/
    action.actionType = CACT_DELAY;
    action.actionTime = TIMER_5SEC;
    actionQueueIn(&g_actionQueue, &action);
    
    #if	1
    action.actionType = CACT_LOCK;
    action.actionTime = TIMER_500MS;
    actionQueueIn(&g_actionQueue, &action);
    #else
    /** ��ת���� **/
    action.actionType = CACT_LOCK;
    action.actionTime = TIMER_100MS;
    actionQueueIn(&g_actionQueue, &action);
	action.actionType = CACT_UNLOCK;
    action.actionTime = TIMER_60MS;
    actionQueueIn(&g_actionQueue, &action);
    action.actionType = CACT_LOCK;
    action.actionTime = TIMER_300MS;
    actionQueueIn(&g_actionQueue, &action);
    #endif
}

void actionDelay(int _delay)
{
	action_t action;
    action.actionType = CACT_DELAY;
    action.actionTime = _delay;
    actionQueueIn(&g_promptQueue, &action);
}


/*****************************************************************************
 * function: arrSort
 * Description: array sort
 * input: 	arr - array pointer
 			n - the number of element
 * output: no
 * return: no
****************************************************************************/
void arrSort(unsigned int arr[],unsigned char n)                         //��������
{
	unsigned char m,k,j,i;
	unsigned int d;
	k=0;
	m=n-1;
	while(k<m)
	{
		j=m-1;
		m=0;
		for(i=k;i<=j;i++)
		{
			if(arr[i]>arr[i+1])
			{
				d=arr[i];
				arr[i]=arr[i+1];
				arr[i+1]=d;
				m=i;
			}
		}
		j=k+1;
		k=0;
		for(i=m;i>=j;i--)
		{
			if(arr[i-1]>arr[i])
			{
				d=arr[i];
				arr[i]=arr[i-1];
				arr[i-1]=d;
				k=i;
			}
		}
	}
}
/*******************************************************************************
 * function: arrAverage
 * Description: calculate array average
 * input: 	arr - array pointer
 *			start - the start index of element
 *			end -   the start index of element
 * output: no
 * return: average
********************************************************************************/
unsigned int arrAverage(unsigned int arr[], unsigned char start, unsigned char end)       //����ƽ��ֵ
{
	int	i;
	unsigned int sum = 0;
	if(start > end)
	{
		return	0xffff;		/** error **/
	}
	for(i = start; i < end; i++)
	{
		sum += arr[i];
	}
	
	return	sum /(end - start +1);
}
/*******************************************************************************
���ݴ����滮���£�
 * 0..3 - MAGICCODE(����HUAR��ASCII����Ϊʶ��)
 * 4..7 - ADC(�͵�)
 * 8..11 - 
 * 12..29 - reserved
 * 30..39 - password 1(��m������ĵ�nλ����λ��:  (m+2)*10 + n)
 * 40..49 - password 2
 * 50..59 - password 3
 * 60..69 - password 4
 * 70..79 - password 5
 * 80..89 - password 6
 * 90..99 - reserved (�������)
 * 100..500 - ָ��ģ������(��m��ָ�Ƶ�������־����λ��:  m + 100)
 *
 *******************************************************************************/

/*******************************************************************************
 * function: flashWrite
 * Description: write 1KB data into a page
 * input: 	arr - the data array pointer
 * output: no
 * return: average
 * 
 * Note: flash Page size FLASH_PAGE_SIZE=1KBytes
********************************************************************************/
int flashWrite(u32 arr[])
{
	u32 FlashAddress;
	FLASH_Status FLASHStatus = FLASH_COMPLETE;
	
	/* Unlock the Flash Program Erase controller */
    FLASH_Unlock();   
    /* Clear All pending flags */
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	//���־λ
    
    /* Erases the specified Flash Page */
    //FLASHStatus = FLASH_ErasePage(CFLASHSTARTADDR);
    FLASHStatus = FLASH_ErasePage(CFLASHSTARTADDR); 	//�����������
	if(FLASHStatus != FLASH_COMPLETE)
	{
		return -1;
	}
    
    /** Writes the Data at the Address **/
    FlashAddress = CFLASHSTARTADDR;
    while(FlashAddress < CFLASHSTARTADDR + CFLASH_PAGE_SIZE)
    {
    	//status = FLASH_ProgramWord(FlashAddress, arr[FlashAddress - CFLASHSTARTADDR]); 
    	FLASH_ProgramWord(FlashAddress, arr[(FlashAddress - CFLASHSTARTADDR) >> 2]);
    	
    	FlashAddress += 4;
	}
	
	FLASH_Lock();
	
	return	0;
}

/*******************************************************************************
 * function: wdg_init
 * Description: 
 * input: no
 * output: no
 * return: no
********************************************************************************/
void wdg_init(void)
{

    /* IWDG timeout equal to 350ms (the timeout may varies due to LSI frequency
    dispersion) -------------------------------------------------------------*/
    /* Enable write access to IWDG_PR and IWDG_RLR registers */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    /* IWDG counter clock: 32KHz(LSI) / 32 = 1KHz */
    IWDG_SetPrescaler(IWDG_Prescaler_32);

    /* Set counter reload value to 349 */
    IWDG_SetReload(349);
    //IWDG_SetReload(1999);

    /* Reload IWDG counter */
    IWDG_ReloadCounter();

    /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
    IWDG_Enable();
}

void wdg_feed(void)
{
    /* Reload IWDG counter */
    IWDG_ReloadCounter();
}


////////////////////////////////////////////////////////////
