/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : stm32f10x_it.c
* Author             : MCD Application Team
* Date First Issued  : 05/21/2007
* Description        : Main Interrupt Service Routines.
*                      This file can be used to describe all the exceptions 
*                      subroutines that may occur within user application.
*                      When an interrupt happens, the software will branch 
*                      automatically to the corresponding routine.
*                      The following routines are all empty, user can write code 
*                      for exceptions handlers and peripherals IRQ interrupts.
********************************************************************************
* History:
* 05/21/2007: V0.3
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "string.h"
#include "stm32f10x_it.h"
#include "main.h"

#include "inc/CONST.h"
#include "inc/ctype.h"
#include "inc/macro.h" 
#include "inc/global.h"
#include "src/arch.h"
//#include "src/bitmapOp.h" 
#include "src/charQue.h" 
//#include "src/X10Que.h"

#include "inc/debug.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

vu16 CCR1_Val = 0xBB8;			/** f = 8kHz **/
//vu16 CCR1_Val = 0x1770;			/** f = 4kHz **/
//vu16 CCR1_Val = 0x2EE1;       /** �ǳ��ӽ�1000Hz, ʾ�����ֹ�����2f��1000Hz, �Զ���ʾ499.984 **/
//vu16 CCR1_Val = 0x2EE0;       /** �ǳ��ӽ�1000Hz, ʾ�����ֹ�����2f��1000Hz, �Զ���ʾ500.026 **/

//vu16 CCR4_Val = 0x177;			/** 128kHz **/
vu16 CCR4_Val = 0xC8;			/** ʾ�����ֹ�������: f = 119.99kHz **/
//vu16 CCR4_Val = 0xCB;			/** ʾ�����ֹ�������: f = 118.2kHz **/


//charBuf_queue_t g_canRevBuf;             /** data fragment **/
charBuf_queue_t g_comRevBuf;             /** data fragment **/

/** 
 * noteע�⣺�Դ�ȫ�����ݵĴ���û��ͬ����������Ϊ��һ��x10����֡��Ҫ0.5s���ϣ�ʱ���㹻
 * �ڴ�������յ���x10����֮ǰ�������ܻ��ٴ��յ�һ��22bit��x10����֡
 * 
 **/
 
//X10RevData_t	g_x10Revframe;
//X10Data_t	g_x10Sndframe;	
//X10Data_t	g_x10bakframe;	
//unsigned int g_x10RcvAddr;		/** x10�յ���ַ **/
//bitmap_t    bmpRcvCan;



/** 
 * �����ز�ƫ��ͬ�� 
 * �������Ϊ�������߹���㵽�����жϵ�ʱ��
 * �����ϣ��ڵ����߹����ʱ�����ж��źš�
 * ʵ����, ������źŵĴ���Ҫ��������BJT�����ܷŴ󣬻�ͨ�������ϣ���һ��ʱ���ӳ١�
 * �˱������Դ��ӳٽ��в���
 * ���ڵ���Ԫ���ĸ������, ������������Ҫ��У��
 **/
//unsigned char g_x10_sync_offset = CX10_SYNC_OFFSET;

//unsigned int x10_RcvBit = 0;		/** ���ս��Ķ�β������� **/
//volatile unsigned int x10_RcvXData = 0;		/** ���ս������� **/
//unsigned int x10_Rcv1stData;		/** Rcv1���ս������� **/
//unsigned int x10_Rcv2Data;		/** ���3��cycle֮��Rcv2���ս������� **/
//unsigned int x10_tmpData;		/** ���ͳ������� **/

/** 
 * �Թ����Ϊ���ĵ�ǰ��Χ(unit: 0.125ms)
 * �����ݷǳ����С����Է��ֽ��䶨���4��ʱ�����ݷ��ͳɹ��ʷǳ���
 **/
//#define	CDELTA_BC	5					/** �������ǰ **/
//#define	CDELTA_AD	(10 - CDELTA_BC)	/** ������Ժ� **/

//static unsigned char EXTI15_count;	/** �ⲿ�жϼ���, �����ڿ���x10��x10_timer������λ **/


/** ���ݲ��������Ҳ�������ȡ�� **/
//#define	X10_BitRead()	do{x10_RcvBit <<= 1;x10_RcvBit |= !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4);}while(0)

/** 
 * name:
 * description: count the number of 1 in data 
 * input:
 * output:
 * return: 
 * author: 
 *
 **/


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : NMIException
* Description    : This function handles NMI exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NMIException(void)
{
}

/*******************************************************************************
* Function Name  : HardFaultException
* Description    : This function handles Hard Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void HardFaultException(void)
{
}

/*******************************************************************************
* Function Name  : MemManageException
* Description    : This function handles Memory Manage exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MemManageException(void)
{
}

/*******************************************************************************
* Function Name  : BusFaultException
* Description    : This function handles Bus Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BusFaultException(void)
{
}

/*******************************************************************************
* Function Name  : UsageFaultException
* Description    : This function handles Usage Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UsageFaultException(void)
{
}

/*******************************************************************************
* Function Name  : DebugMonitor
* Description    : This function handles Debug Monitor exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DebugMonitor(void)
{
}

/*******************************************************************************
* Function Name  : SVCHandler
* Description    : This function handles SVCall exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SVCHandler(void)
{
}

/*******************************************************************************
* Function Name  : PendSVC
* Description    : This function handles PendSVC exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PendSVC(void)
{
}

/*******************************************************************************
* Function Name  : SysTickHandler
* Description    : This function handles SysTick Handler.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SysTickHandler(void)
{
}

/*******************************************************************************
* Function Name  : WWDG_IRQHandler
* Description    : This function handles WWDG interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WWDG_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : PVD_IRQHandler
* Description    : This function handles PVD interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PVD_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TAMPER_IRQHandler
* Description    : This function handles Tamper interrupt request. 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TAMPER_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : RTC_IRQHandler
* Description    : This function handles RTC global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : FLASH_IRQHandler
* Description    : This function handles Flash interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FLASH_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : RCC_IRQHandler
* Description    : This function handles RCC interrupt request. 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : EXTI0_IRQHandler
* Description    : This function handles External interrupt Line 0 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI0_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : EXTI1_IRQHandler
* Description    : This function handles External interrupt Line 1 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI1_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : EXTI2_IRQHandler
* Description    : This function handles External interrupt Line 2 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI2_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : EXTI3_IRQHandler
* Description    : This function handles External interrupt Line 3 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI3_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : EXTI4_IRQHandler
* Description    : This function handles External interrupt Line 4 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI4_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMAChannel1_IRQHandler
* Description    : This function handles DMA Stream 1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMAChannel1_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMAChannel2_IRQHandler
* Description    : This function handles DMA Stream 2 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMAChannel2_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMAChannel3_IRQHandler
* Description    : This function handles DMA Stream 3 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMAChannel3_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMAChannel4_IRQHandler
* Description    : This function handles DMA Stream 4 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMAChannel4_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMAChannel5_IRQHandler
* Description    : This function handles DMA Stream 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMAChannel5_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMAChannel6_IRQHandler
* Description    : This function handles DMA Stream 6 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMAChannel6_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : DMAChannel7_IRQHandler
* Description    : This function handles DMA Stream 7 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMAChannel7_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : ADC_IRQHandler
* Description    : This function handles ADC global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : USB_HP_CAN_TX_IRQHandler
* Description    : This function handles USB High Priority or CAN TX interrupts 
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_HP_CAN_TX_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : USB_LP_CAN_RX0_IRQHandler
* Description    : This function handles USB Low Priority or CAN RX0 interrupts 
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
/**
 * i_flag 0..3 - reserved
 *          4 - canbus receiving
 *          5..31 reserved
 **/
int i_flag = 0;
/**
 *  timer for canbus receiving
 **/
//int i_canRxTmr = 0;
void USB_LP_CAN_RX0_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : CAN_RX1_IRQHandler
* Description    : This function handles CAN RX1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_RX1_IRQHandler(void)
{
    /** Same as USB_LP_CAN_RX0_IRQHandler ? **/
}

/*******************************************************************************
* Function Name  : CAN_SCE_IRQHandler
* Description    : This function handles CAN SCE interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_SCE_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : EXTI9_5_IRQHandler
* Description    : This function handles External lines 9 to 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI9_5_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM1_BRK_IRQHandler
* Description    : This function handles TIM1 Break interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_BRK_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM1_UP_IRQHandler
* Description    : This function handles TIM1 overflow and update interrupt 
*                  request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_UP_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM1_TRG_COM_IRQHandler
* Description    : This function handles TIM1 Trigger and Commutation interrupts 
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_TRG_COM_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM1_CC_IRQHandler
* Description    : This function handles TIM1 capture compare interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_CC_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM2_IRQHandler
* Description    : This function handles TIM2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int comm_flag = 0;
int bit_count = 0;
/** 
 * bit3..0 ����
 * bit4 - uart2���ڽ��ձ�־
 * bit5 - �����жϲ�������ʵ�ϵ����ǰ��0.5ms��Χ��
 * bit6 .. - reserved
 **/
 #define	TMR_COMIDLE	400
int Timer_comBusy = TMR_COMIDLE;      /** ��ʱ **/


//#define	CVOPTMR_S	(2)
#define	CVOPTMR_S	(80)
#define CVOPTMR_INIT	(CVOPTMR_S + 1 + (14 * 7 + 10))
int g_tmr_vop = CVOPTMR_INIT + 1;
int g_tmr_iVopBusy = 0;
int g_tmr_iVopFault = 0;

/** ������������ʱ�� **/
int g_tmr_key = TIMER_200MS;

unsigned int g_TIM2_tick = 0;

void TIM2_IRQHandler(void)
{
    u16 capture1 = 0;
//    u16 capture4 = 0;
    u16 i;

	msg_t msg;

    /* TIM2_CH1 toggling with frequency = 8000 Hz */
    if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_CC1 );
        capture1 = TIM_GetCapture1(TIM2);
        TIM_SetCompare1(TIM2, capture1 + CCR1_Val );
 		/***************************************************/
 		g_TIM2_tick++;
 		//if((g_TIM2_tick & 0x0fff) == 0)	/** generate msg every 0.512s**/
 		if((g_TIM2_tick & 0x07ff) == 0)	/** generate msg every 0.256s**/
 		{
 			msg.msgType = CMSG_ADC;
			inq(&g_msgq, &msg);
 		}
 		/***************************************************/
        for(i = 0; i < TIMER_NUM; i++)
        {
            if(g_timer[i].tick_bak > 0)
            {
                if(g_timer[i].tick > 0)
                {
                    g_timer[i].tick--;
                }
                else
                {
                    g_timer[i].tick = g_timer[i].tick_bak;
                    
                    msg.msgType = g_timer[i].msgType;		/** message type **/
                    msg.msgValue = i;						/** message value(Timer index) **/
			 		inq(&g_msgq, &msg);
                }
            }
        }
        /***************************************************/
        Timer_comBusy++;
        if(Timer_comBusy > TMR_COMIDLE)		 /** out of 50ms **/
        {
        	Timer_comBusy = TMR_COMIDLE;
        }
        else if(Timer_comBusy == TMR_COMIDLE)	/** time out **/
        {
			msg.msgValue = 2;	  		/** UART2 **/
			msg.msgType = CMSG_COMRX;
			inq(&g_msgq, &msg);		  	/** �յ�һ����������. ����Ϣ **/

        }
        else	 /** doing receive **/
        {
        	/** do nothing **/
        }
        
        /***************************************************/
        if(g_tmr_delay != 0)
        {
            g_tmr_delay--;
        }
        /***************************************************/
        if(g_tmr_key < TIMER_60MS /** ����ȷ��ʱ��� **/) {
        	g_tmr_key++;
        }
        /***************************************************/
        //if(g_tmr_iVopBusy < TIMER_VOPBUSY /** ����ȷ��ʱ��� **/) {
        if(g_tmr_iVopBusy < TIMER_VOPBUSYMAX /** ����ȷ��ʱ��� **/) {
        	g_tmr_iVopBusy++;
        }
        if((g_tmr_iVopFault >= TIMER_VOPFAULTBEGIN) && (g_tmr_iVopFault < TIMER_VOPFAULTEND)) {
        	 /** �����쳣ʱ��� [TIMER_VOPFAULTBEGIN, TIMER_VOPFAULTEND] **/
     	   g_tmr_iVopFault++;
    	}
        /***************************************************/
        
		#if	CENABLEVOP
        if(g_tmr_vop <= CVOPTMR_INIT)	//working
        {
        	g_tmr_vop++;
        	
        	if((g_tmr_vop - (CVOPTMR_S)) >= 0)
        	{
        		if(g_tmr_vop - (CVOPTMR_S) == 0)	//��ʼʱ���
        		{
					bit_count = 0;
        			VOPRESET_L();
        			VOPDATA_L();
        		}
        		else if(g_tmr_vop - (CVOPTMR_S) == 1)	//reset pulse start 
        		{
        			VOPRESET_H();
        		}
        		else if(g_tmr_vop - (CVOPTMR_S) == 4)	//reset over
        		{
        			VOPRESET_L();
        			if(g_tmr_sbuf == 0)
        			{
        				g_tmr_vop = CVOPTMR_INIT + 1;
        			}
        		}
        		else if((g_tmr_vop - (CVOPTMR_S) - 4) > 0)	/** 10ms L -> start send **/
        		{
    #define	MSENDTIMING	(g_tmr_vop - (CVOPTMR_S) - 4)
					/*******************************************
					 * 1 - H/L - 125us/125us
					 *******************************************/
					if(bit_count < g_tmr_sbuf)			//�����������?
					{
        				if((MSENDTIMING & 0x01) == 1)								//0us - H
        				{
							VOPDATA_H();
						}
						else 
						{
							VOPDATA_L();
        					bit_count++;
						}
					}
					else	//��������
					{
						g_tmr_vop = CVOPTMR_INIT + 1;
						g_tmr_iVopBusy = 0;
					}
        		}
        	}
        }
        else
        {
			VOPDATA_L();		//����ʡ��
        }
        #endif
        
        #if	0
        /***************************************************/
        if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6))
        {
        }
        /***************************************************/
        #endif
    }
    
    /*** toggling with frequency = 120kHz ***/
    #if	0
	if (TIM_GetITStatus(TIM2, TIM_IT_CC4) != RESET) 
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC4);
		capture4 = TIM_GetCapture4(TIM2);
		TIM_SetCompare4(TIM2, capture4 + CCR4_Val);
	}
	#endif
}

/*******************************************************************************
* Function Name  : TIM3_IRQHandler
* Description    : This function handles TIM3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM3_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : TIM4_IRQHandler
* Description    : This function handles TIM4 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM4_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : I2C1_EV_IRQHandler
* Description    : This function handles I2C1 Event interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C1_EV_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : I2C1_ER_IRQHandler
* Description    : This function handles I2C1 Error interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C1_ER_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : I2C2_EV_IRQHandler
* Description    : This function handles I2C2 Event interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C2_EV_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : I2C2_ER_IRQHandler
* Description    : This function handles I2C2 Error interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C2_ER_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : SPI1_IRQHandler
* Description    : This function handles SPI1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI1_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : SPI2_IRQHandler
* Description    : This function handles SPI2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI2_IRQHandler(void)
{
}

/*******************************************************************************
 * Function Name  : USART1_IRQHandler
 * Description    : This function handles USART1 global interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 ******************************************************************************/
void USART1_IRQHandler(void)
{
//	charData_t charData;
    
    #if	0
    /** �յ�USART1���� **/
    if(USART_GetFlagStatus(USART1, USART_IT_RXNE) == SET)
    {
    	//inq(&g_msgq, CMSG_COMRX);
    }

    /** ��ɷ���USART1���� **/
    if(USART_GetFlagStatus(USART1, USART_IT_TC) == SET)
    {
        //inq(&g_msgq, CMSG_COMTX);	
    }
    #endif
}

/*******************************************************************************
 * Function Name  : USART2_IRQHandler
 * Description    : This function handles USART2 global interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *
 * Note: ���ڴ������ݵ���ʼ�ͽ������ⲿi/o׼�����ʶ������������
 *        ÿ��һ���ֽں󣬲��ⲿ�ӿڣ������low, ����Ϊ��ͬһ�����ݣ������high, ����Ϊ�ǽ���
 *******************************************************************************/
void USART2_IRQHandler(void)
{
    charData_t charData;
    msg_t msg;
//	static int iCount = 0;	/** ʵ���յ����ֽ��� **/
//	static int iLen = 0;	/** ��������ʾ���ֽ��� **/

    /** �յ�USART2���� **/
    if(USART_GetFlagStatus(USART2, USART_IT_RXNE) == SET)
    {
        charData.ucVal = USART_ReceiveData(USART2);
        charQueueIn(&g_comRevBuf, &charData);
        
        //comm_flag |= (1 <<4);
        Timer_comBusy = 0;
    }

    /** ��ɷ���USART2���� **/
    if(USART_GetFlagStatus(USART2, USART_IT_TC) == SET)
    {
		msg.msgType = CMSG_COMTX;
		inq(&g_msgq, &msg);	
    }
}

/*******************************************************************************
* Function Name  : USART3_IRQHandler
* Description    : This function handles USART3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART3_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : EXTI15_10_IRQHandler
* Description    : This function handles External lines 15 to 10 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI15_10_IRQHandler(void)
{
/**
 * Ext interrupt occured every cycle, but we get synchronous clock cycle by 8kHz timer
 * we can correct sycnhronous clock after 
 **/	
	if(EXTI_GetITStatus(EXTI_Line13) != RESET)
	{
		/* Clear the EXTI line 13 pending bit */
		EXTI_ClearITPendingBit(EXTI_Line13);
	}
	
	if(EXTI_GetITStatus(EXTI_Line12) != RESET)		/** zero-cross calibration **/
	{
		/* Clear the EXTI line 12 pending bit */
		EXTI_ClearITPendingBit(EXTI_Line12);
	}
}

/*******************************************************************************
* Function Name  : RTCAlarm_IRQHandler
* Description    : This function handles RTC Alarm interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTCAlarm_IRQHandler(void)
{
}

/*******************************************************************************
* Function Name  : USBWakeUp_IRQHandler
* Description    : This function handles USB WakeUp interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USBWakeUp_IRQHandler(void)
{
}

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
