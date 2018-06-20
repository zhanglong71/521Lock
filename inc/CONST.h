#ifndef __CONST_H__
#define __CONST_H__

/*******************************************************************************/
#define	DUGARCH
//#undef	DUGARCH

#define	DUGTMR
//#undef	DUGTMR

#define	DUGFPRINT
//#undef	DUGFPRINT

//#define	CNOBEEP	0
#define	CENABLEVOP	1	//������ICȡ��������

#define	CSETOP_asterisk_pound	1		//set��������������ʽ

#define	CVOP_GQPP5
//#undef	CVOP_GQPP5
//#define	CVOP_WT588D
//#undef	CVOP_WT588D
/*******************************************************************************
 *
 * All constant define here
 *
 *******************************************************************************/
#define	STACKSIZE	8
#define	QUEUESIZE	8

#define	OK	0
#define	ERROR	-1

#define	TRUE	1
#define	FALSE	0

#define CSEND_BUF_NUM	8	//16	 //����������
#define CSEND_BUF_SIZE	128	//64	 //��������С

#define TIMER_NUM	3       /** ��ʱ������(�ڶ���ֻ����sysProcess��) **/
#define TIMER_0 	0		/** 8kHz��ʱ, 0���ڹر�ʱ�� **/
#define TIMER_10MS 80		/** 8kHz��ʱ, 10ms�������� **/
#define TIMER_20MS 160		/** 8kHz��ʱ, 20ms�������� **/
#define TIMER_60MS 480		/** 8kHz��ʱ, 60ms�������� **/
#define TIMER_80MS 640		/** 8kHz��ʱ, 80ms�������� **/
#define TIMER_100MS 800		/** 8kHz��ʱ, 100ms�������� **/
#define TIMER_200MS 1600	/** 8kHz��ʱ, 200ms�������� **/
#define TIMER_300MS 2400	/** 8kHz��ʱ, 300ms�������� **/
#define TIMER_500MS 4000	/** 8kHz��ʱ, 500ms�������� **/
#define TIMER_600MS 4800	/** 8kHz��ʱ, 600ms�������� **/
#define TIMER_700MS 5600	/** 8kHz��ʱ, 700ms�������� **/
#define TIMER_800MS 6400	/** 8kHz��ʱ, 800ms�������� **/
#define TIMER_1SEC  8000	/** 8kHz��ʱ, ÿ1��������� **/
#define TIMER_2SEC  16000	/** 8kHz��ʱ, ÿ2��������� **/
#define TIMER_3SEC  24000	/** 8kHz��ʱ, ÿ3��������� **/
#define TIMER_4SEC  32000	/** 8kHz��ʱ, ÿ4��������� **/
#define TIMER_5SEC  40000	/** 8kHz��ʱ, ÿ5��������� **/
#define TIMER_10SEC 80000	/** 8kHz��ʱ, 10��������� **/
#define TIMER_CALIB 800000	/** 8kHz��ʱ, 100�볬ʱ�˳� **/
#define TIMER_RGST  800		/** ע��ʱ���� **/

#define TIMER_VOPBUSY  800	/** ����æ�����ʱʱ�� **/
#define TIMER_VOPBUSYMAX  8000	/** �������������ʱʱ���������� **/
#define TIMER_VOPFAULTBEGIN  10	/** ������������쳣ʱ���������� **/
#define TIMER_VOPFAULTEND  (TIMER_VOPFAULTBEGIN + 8000) 	/** ������������쳣ʱ���������� **/

#define CTIMER_CALIB (16000)	/** ����У��״̬��Ҫ�������°�����ʱ��, ն��Ϊ2s **/

/** ���� **/
#define	CKEY_1	(0x02)
#define	CKEY_2	(0x01)
#define	CKEY_3	(0x05)
#define	CKEY_4	(0x04)
#define	CKEY_5	(0x0c)
#define	CKEY_6	(0x09)
#define	CKEY_7	(0x08)
#define	CKEY_8	(0x0a)
#define	CKEY_9	(0x0b)
#define	CKEY_asterisk	(0x07)
#define	CKEY_0	(0x06)
#define	CKEY_pound	(0x03)

#define	CKEY_X1	(0x00)
#define	CKEY_X2	(0x0d)
#define	CKEY_X3	(0x0e)
#define	CKEY_X4	(0x0f)


#define	CKEY_GOOUT	(0x10)
#define	CKEY_GOIN	(0x11)
#define CKEY_SET 	(0x12)	//�ź�����ü�

//#define CTOKEY 16	//go out
//#define CTIKEY 17	//go in
//#define CSETKEY 18	//�ź�����ü�


#define CDEV_TYPE	2

#define CHARQUEUESIZE   128
#define CKEYBUFSIZE   12

#define ACTIONQUEUESIZE 16

#define X10QUEUESIZE   16

#define CANQUEUESIZE    (128 >> 2)

/*******************************************************************************/
#define FILTER_SID  (0x01)
#define FILTER_EID  (0x0)
#define FILTER_ID    ((FILTER_SID << 18) | (FILTER_EID))
//#define FILTER_ID   ((u32)((FILTER_SID << 18) | (FILTER_EID)) << 3) | CAN_ID_EXT | CAN_RTR_DATA)

#define MASK_SID    (0x7f)
#define MASK_EID    (0x0)
#define FILTER_MASK    ((MASK_SID << 18) | (MASK_EID))

/*******************************************************************************/
#define ASCII_STX   0x02
#define ASCII_ETX   0x03

/*******************************************************************************/
#define	CTOTALADDRESS	1024


/*******************************************************************************/

#define CFP_INIT					0
#define CFP_ENROLL     			2
#define CFP_MATCH     			3
#define CFP_DELETE    			4

//#define	CTOTALFINGER	58
#define	CTOTALFINGER	118	/** ָ��������Ŀ��, Ҳ��ָ��ģ������ **/
#define	C1STMANAGEE		20	/** ��һ�����ߣ�����ӵı������(��ȥ1���ǹ��������) **/

	/** �ڶ������߹���ָ�Ƶ���ʼ������(��һ�����ߵ�������) **/
#define	C2NDMANAGES 	(C1STMANAGEE + 1)
	/** �ڶ������߹���ָ�ƵĽ���������(ȡʣ�µ�������һ�룬���ϵ�һ�����ߵĹ��������͵õ�����������) **/
#define	C2NDMANAGEE 	(((CTOTALFINGER - C1STMANAGEE) >> 1) + C1STMANAGEE)

#define	C3RDMANAGES 	(C2NDMANAGEE + 1)	/** ���������߹���ָ�Ƶ���ʼ������(�ڶ������ߵĽ�β������+1) **/
#define	C3RDMANAGEE 	(CTOTALFINGER - 1)	/** ���������߹���ָ�ƵĽ���������(��������-1) **/


#define CADC1_DR_ADDRESS    ((u32)0x4001244C)

#define	CFLASHSTARTADDR (0x0801FC00)		/** the 127th Flash Page **/
//#define	CFLASHSTARTADDR (0x0801F800)	/** the 126th Flash Page **/

#define CMAX_PSERR 6		//�������ʧ�ܴ���
/*******************************************************************************/
#define	CFLASH_PAGE_SIZE		1024

#define	CFLASH_ADC_VALUE		4			/** 4..5 **/
//#define	CFLASH_ADC_FLAG			6
#define	CFLASH_RESET_VALUE		8
#define	CFLASH_INDUCTION_POWER		12		/** ָ��ͷ��Ӧ�ϵ��־A5/A0��Ϊ�л��ޣ�����Ϊ��ЧҲ�����и�Ӧ�ϵ��־ **/
		#define CINDUCTION_POWER_DISABLE	0xA0		/** ��ָ���ϵ繦��(Ĭ����) **/
		#define CINDUCTION_POWER_ENABLE		0xA5		/** ��ָ���ϵ繦��(Ĭ����) **/
#define	CFLASH_PASSWD_ERR		16		/** �������������� **/
#define	CFLASH_PASSWD_START		30
#define	CFLASH_FPIndex_START	130

#define	CENTRYFLAG_BUSY	0xa5	/** ��ʾָ�������ѱ�ʹ�� **/
#define	CENTRYFLAG_IDLE	0xff	/** ��ʾָ������δ��ʹ�� **/

#define	MAGIC_SIZE 4
/*******************************************************************************/
//������������
#define	CVOPID_BASE	1
#define	CVOPID_1	(CVOPID_BASE + 1)
#define	CVOPID_2	(CVOPID_BASE + 2)
#define	CVOPID_3	(CVOPID_BASE + 3)
#define	CVOPID_4	(CVOPID_BASE + 4)
#define	CVOPID_5	(CVOPID_BASE + 5)
#define	CVOPID_6	(CVOPID_BASE + 6)
#define	CVOPID_7	(CVOPID_BASE + 7)
#define	CVOPID_8	(CVOPID_BASE + 8)
#define	CVOPID_9	(CVOPID_BASE + 9)
#define	CVOPID_asterisk	(CVOPID_BASE + 10)
#define	CVOPID_0	(CVOPID_BASE + 0)
#define	CVOPID_pound	(CVOPID_BASE + 11)

#define	CVOPID_EROLLPSWORDFORYOURSECURITY	(CVOPID_BASE + 12)
#define	CVOPID_BEEP			(CVOPID_BASE + 13)
#define	CVOPID_LBEEP		(CVOPID_BASE + 14)
#define	CVOPID_EROLL		(CVOPID_BASE + 15)	//¼��
#define	CVOPID_ADD		(CVOPID_BASE + 16)		//���
#define	CVOPID_DEL		(CVOPID_BASE + 17)		//ɾ��
#define	CVOPID_ALL		(CVOPID_BASE + 18)		//ȫ��
#define	CVOPID_FINGER		(CVOPID_BASE + 19)	//ָ��
#define	CVOPID_PSWORD		(CVOPID_BASE + 20)	//����
#define	CVOPID_PLEASEPRESS	(CVOPID_BASE + 21)	//�밴
#define	CVOPID_SUCESS		(CVOPID_BASE + 22)	//�ɹ�
#define	CVOPID_FAILED		(CVOPID_BASE + 23)	//ʧ��
#define	CVOPID_PASSED		(CVOPID_BASE + 24)	//ͨ��
#define	CVOPID_CLEARED		(CVOPID_BASE + 25)	//�����
#define	CVOPID_TIMEOUT		(CVOPID_BASE + 26)	//��ʱ
#define	CVOPID_INVALID		(CVOPID_BASE + 27)	//��Ч
#define	CVOPID_RECOVERED	(CVOPID_BASE + 28)	//�ָ������ɹ�
#define	CVOPID_PLEASEEROLLAGAIN		(CVOPID_BASE + 29)	//����¼һ��
#define	CVOPID_COMPARISON	(CVOPID_BASE + 30)	//��֤(�ȶ�)
#define	CVOPID_SET			(CVOPID_BASE + 31)	//����
#define	CVOPID_OK			(CVOPID_BASE + 32)	//�밴���ֿ���
#define	CVOPID_PLEASEINPUTPSWORD		(CVOPID_BASE + 33)	//����������
#define	CVOPID_PRESSFINGER		(CVOPID_BASE + 34)		//�밴��ָ
#define	CVOPID_REGISTER		(CVOPID_BASE + 35)	//ע��
#define	CVOPID_MANAGE		(CVOPID_BASE + 36)	//����
#define	CVOPID_INPUT		(CVOPID_BASE + 37)	//����
#define	CVOPID_CONFIRM		(CVOPID_BASE + 38)	//ȷ��
#define	CVOPID_POWERLOW		(CVOPID_BASE + 39)	//�������㣬�������ػ���

#define	CVOPID_FINAL		(CVOPID_POWERLOW)	//��β




#define	CVOP_VOL0		0xe0
//...
#define	CVOP_VOL7		0xe7
#define	CVOP_LOOP		0xf2	//play voice again and again
#define	CVOP_STOP		0x00	//stop playing

/*******************************************************************************/
#endif
