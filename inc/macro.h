#ifndef __MACRO_H__
#define __MACRO_H__
/*******************************************************************************/
/** �����С **/
#define	TABSIZE(xxx)	(sizeof(xxx)/sizeof(xxx[0]))

/** ����LED **/
#define	MBlackLED_off() (GPIO_ResetBits(GPIOB, GPIO_Pin_0))
#define	MBlackLED_on() (GPIO_SetBits(GPIOB, GPIO_Pin_0))
#define	MBlackLED_status() (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_0))

/** ָ��ͷ���� **/
#define	MFingerPower_off() (GPIO_ResetBits(GPIOB, GPIO_Pin_2))
#define	MFingerPower_on() (GPIO_SetBits(GPIOB, GPIO_Pin_2))

/** ��IC������· **/
//#define	MMasterPower_off() (GPIO_ResetBits(GPIOB, GPIO_Pin_9))
#define	MMasterPower_off() do{GPIO_ResetBits(GPIOB, GPIO_Pin_9);}while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9))
#define	MMasterPower_on() (GPIO_SetBits(GPIOB, GPIO_Pin_9))
 /*****************************************************************/
/** ����ָʾ�� **/
//#define	MOPGreen_off() (GPIO_SetBits(GPIOA, GPIO_Pin_0))
//#define	MOPGreen_on() do{GPIO_ResetBits(GPIOA, GPIO_Pin_0); GPIO_SetBits(GPIOA, GPIO_Pin_1);}while(0)

#define	MOPBlue_off() do{GPIO_SetBits(GPIOA, GPIO_Pin_0);}while(0)
#define	MOPBlue_on() do{GPIO_ResetBits(GPIOA, GPIO_Pin_0); GPIO_SetBits(GPIOA, GPIO_Pin_1);}while(0)
#define	MOPBlue_setDefaultStatus() do{\
	if(MBlackLED_status()){MOPBlue_on();}else{MOPBlue_off();}\
							}while(0)

#define	MOPRed_off() do{GPIO_SetBits(GPIOA, GPIO_Pin_1);}while(0)	
#define	MOPRed_on() do{GPIO_ResetBits(GPIOA, GPIO_Pin_1); GPIO_SetBits(GPIOA, GPIO_Pin_0);}while(0)

/** �������� **/
#if	1
//#define	VOP_H() (GPIO_SetBits(GPIOA, GPIO_Pin_4))
//#define	VOP_L() (GPIO_ResetBits(GPIOA, GPIO_Pin_4))

#define	VOPDATA_H() (GPIO_SetBits(GPIOA, GPIO_Pin_4))
#define	VOPDATA_L() (GPIO_ResetBits(GPIOA, GPIO_Pin_4))

#define	VOPRESET_H() (GPIO_SetBits(GPIOA, GPIO_Pin_6))
#define	VOPRESET_L() (GPIO_ResetBits(GPIOA, GPIO_Pin_6))

#endif
/** ������ **/
#define	MLock()  do{\
		GPIO_ResetBits(GPIOC, GPIO_Pin_6);\
		GPIO_SetBits(GPIOC, GPIO_Pin_7);\
				}while(0)
#define	MUnLock()  do{\
	GPIO_SetBits(GPIOC, GPIO_Pin_6);\
	GPIO_ResetBits(GPIOC, GPIO_Pin_7);\
				}while(0)
#define	MLockPWR_off()  do{\
	GPIO_ResetBits(GPIOC, GPIO_Pin_6);\
	GPIO_ResetBits(GPIOC, GPIO_Pin_7);\
			}while(0)

/** ��Ӧ���翪�� **/
#define	MPKEPower_off() (GPIO_ResetBits(GPIOC, GPIO_Pin_9))
#define	MPKEPower_on() (GPIO_SetBits(GPIOC, GPIO_Pin_9))
/** ��Կ�׽����Ӧ�ź� **/


/** ָ��ͷ��Ӧ�ϵ繦�� **/
#define MFP_InductionPower_disable()	do{g_flash.arrChar[CFLASH_INDUCTION_POWER] = CINDUCTION_POWER_DISABLE;}while(0)
#define MFP_InductionPower_enable()		do{g_flash.arrChar[CFLASH_INDUCTION_POWER] = CINDUCTION_POWER_ENABLE;}while(0)
#define	MFP_isInductionPower()		(g_flash.arrChar[CFLASH_INDUCTION_POWER] != CINDUCTION_POWER_DISABLE)
#define	MFP_isON()		((g_flag & (1 << 6)) == (1 << 6))
/*******************************************************************************/
#endif
