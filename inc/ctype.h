#ifndef __PTYPE_H__
#define __PTYPE_H__

/*******************************************************************************
 *
 * All data type define here
 *
 *******************************************************************************/

typedef enum {
    FAILED = 0, 
    PASSED = !FAILED
} RetStatus;

typedef	int	(*pfunc_t)(unsigned *arg);

typedef struct func_s {
    pfunc_t		func;       /** function **/
    unsigned int  *arg;    /** argument **/
} func_t;

typedef struct func_stack{
	int		top;
	func_t		func[STACKSIZE]; 
}fstack_t;

typedef enum {
    CMSG_NONE = 0, 
    CMSG_INIT, 
    CMSG_TMR, 
    CMSG_DLAY, 	/** ʱ�� **/
    CMSG_BACK,	/** ����� **/
    CMSG_DKEY,
    CMSG_UKEY,
    
    //CMSG_FGON,	/** ��⵽ָ��ͷ������ָ **/
    CFIGER_ON,
    CFIGER_INIT,	/** ��λָ��ͷ��� **/
    CFIGER_ADD,		/** ע��ָ�� **/
    CFIGER_DEL,		/** ɾ��ָ�� **/
    CFIGER_TOUT,	/** ָ��ͷ��Ӧ��ʱ **/
    CFIGER_FGPRPRD,	/** ���׼�� **/
    
    
    CMSG_FGINIT,	/** ��ʼ���ɹ� **/
    CMSG_FGRGST,	/** ע��ָ�Ƴɹ� **/
    CMSG_FGQUERY,	/** ����ָ�Ƴɹ� **/
    CMSG_FGDERG,	/** ע��ָ�Ƴɹ� **/
    CMSG_FGOPFAIL,	/** ָ��ͷ����ʧ�� **/
    
	CNOKEY_ENTER,		/** ��Կ�׽��� **/
	
    CMSG_COMTX,
    CMSG_COMRX,

	/**  **/
	//CVOP_STRT,		/** voice prompt start **/
	//CVOP_OVER,

	CACT_TOUT,		/** ������ʱ **/
	CACT_OVER,		/** ������� **/
	
	CPMT_TOUT,		/** ��ʾ������ʱ **/
	CPMT_OVER,		/** ��ʾ������� **/

    CMSG_ADC,		/** ADת�� **/
    CMSG_CALC,		/** ����У��͵���յ� **/
    
    CUART1_RCV,
    CUART2_RCV,
    //CCAN_RCV,
    CMSG_COMTIMEOUT,
} msgType_t;

typedef struct msg_s {
	msgType_t msgType;
	int	msgValue;
}msg_t;

typedef struct msg_queue{
	int	head;
	int	tail;

	msgType_t	type[QUEUESIZE];
	//int	msgValue[QUEUESIZE];
	msg_t msg[QUEUESIZE];
} msgq_t;

typedef struct Timer_s {
    unsigned int tick_bak;
    unsigned int tick;
    msgType_t msgType;                      /** the type of msg when timeout **/
    //unsigned int count;                   /** Is it necessary ? **/
}Timer_t; 

typedef struct key_s {
	unsigned char buf[CKEYBUFSIZE];
	int iCount;
	int iPrivate;			/** �û������˽������ **/
}key_t;

typedef struct bitmap_s {
    unsigned int bitmap;		    /** 32 bits **/
} bitmap_t;

typedef enum {              /** ���ڿ���com���ݵķ��� **/
    CTRL_START = 0,         /** 0 - start transmission **/
    CTRL_CONTI = 1,         /** 1 - continue transmission  **/
                            /** 2 reserved. for other control **/
    CTRL_TMR = 5,           /** 3..200 for delay **/
    CTRL_STOP = 255,        /** 255 - stop transmission**/
} ctrlFlag_t;

typedef struct charData_s{
	unsigned char ucVal;
	ctrlFlag_t ctrl;         /** �����ֶΣ����ڱ�ﵱǰ��data����Ч���ݻ����ӳٻ��������� **/
} charData_t;

typedef struct charBuf_queue_s {
    int	head;
	int	tail;
    int len;
    unsigned char buf[CHARQUEUESIZE];     /** 128 bytes **/
    //ctrlFlag_t ctrl[CHARQUEUESIZE];     /** 128 bytes **/
} charBuf_queue_t;


typedef struct action_s {
	/*****************
	 * MOT-ON
	 * MOT-OFF
	 * LED-ON
	 * LED-OFF
	 * VOP-ON
	 * VOP-OFF
	 * Delay
	 *****************/
#define CACT_UNLOCK	0xff
#define CACT_LOCK	0xfe
#define CACT_LEDPMTON	0xfd
#define CACT_LEDPMTOFF	0xfc
#define CACT_LEDWRNON	0xfb
#define CACT_LEDWRNOFF	0xfa
#define CACT_VOPON	0xf9
//#define CACT_VOPOFF	0xf8
#define CACT_DELAY	0xf7

#define CACT_BEEPON	0xf6
#define CACT_BEEPOFF	0xf5

    unsigned int actionTime;	/** ����ʱ�� **/
    unsigned char actionType;	/** �������� **/
    unsigned char actionPara;	/** ������Ҫ��������������VPidx������ **/
    unsigned char __pad1;		/** �ֽڶ��� **/
    unsigned char __pad2;		/** �ֽڶ��� **/
} action_t;

typedef struct actionQueue_s {
    int	head;
	int	tail;
    int flag;
	msgType_t stepMsgType;			/** �׶��¼������Ϣ **/
	msgType_t overMsgType;			/** �����¼�ȫ�������Ϣ **/
	Timer_t *timer;
    action_t buf[ACTIONQUEUESIZE];     /** 16 bytes **/
} actionQueue_t;

/*******************************************************************************/
typedef union flashPage_u {
	unsigned char arrChar[CFLASH_PAGE_SIZE];
	u32  arrInt[CFLASH_PAGE_SIZE/4];
} flashPage_t;
/*******************************************************************************/
#endif /** ifndef end **/
