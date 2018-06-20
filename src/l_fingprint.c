
#include "stm32f10x_lib.h" 
#include "string.h"
#include "../inc/CONST.h"
#include "../inc/ctype.h" 
#include "../inc/global.h" 
#include "../inc/debug.h"

#include "l_fingprint.h"
#include "charQue.h"

/**********************************************************************
 * ָ��ģ��������
 * 0..57��Ӧ58��ָ����Ϣ
 **********************************************************************/
void flashCache_init(void)
{
	int i;
	for(i = 0; i < CFLASH_PAGE_SIZE; i++)g_flash.arrChar[i] = CENTRYFLAG_IDLE;
}
/**********************************************************************
 * name: fingerTab_empty 
 * description: check weather the fingerprint REPO empty
 **********************************************************************/
int fingerTab_isEmpty(void)
//int fingerTab_empty(void)
{
	return	((memcmp(g_flash.arrChar, g_magic, MAGIC_SIZE) != 0) || 
				g_flash.arrChar[CFLASH_FPIndex_START] == CENTRYFLAG_IDLE);
	//return	((memcmp(g_flash.arrChar, g_magic, MAGIC_SIZE) != 0) || g_flash.arrChar[CFLASH_FPIndex_START] != CENTRYFLAG_BUSY);
}

unsigned char fingerTab_getEntry(int idx)
{
	return	g_flash.arrChar[idx + CFLASH_FPIndex_START];
}

void fingerTab_setEntry(int idx, unsigned char value)
{
	g_flash.arrChar[idx + CFLASH_FPIndex_START] = value;
}
/*******************************************************************************
 * ���������е�ָ��λ�õ����ݸ�ֵ
 *******************************************************************************/
 #if	1
void fingerTab_fill(unsigned char Manager_ID, unsigned char value)
{
	int i;
	int begin = 0;
	int end = CTOTALFINGER - 1;
	
	if(Manager_ID == 0)                 //��һ������ɾ����������ָ��
    {
    	begin = 1;
    	end = CTOTALFINGER - 1;
    }
    else if(Manager_ID == 1)      		//�ڶ�������ɾ������¼��ָ��
    {
    	begin = C2NDMANAGES;
    	end = C2NDMANAGEE;	/** ȡʣ�µ�������һ�룬���ϵ�һ�����ߵĹ��������͵õ����������� **/
    }
    else if(Manager_ID == 2)      		//����������ɾ������¼��ָ��
    {
    	begin = C3RDMANAGES;
    	end = C3RDMANAGEE;
    }
    
	for(i = begin; i <= end; i++)			//���������
	{
		g_flash.arrChar[i + CFLASH_FPIndex_START] = value;
	}
	
}
#endif

/*******************************************************************************
 * �ж�ָ����Ŀ¼���Ƿ����
 *******************************************************************************/
unsigned char fingerTab_entryIsEmpty(int idx)
{
	//return	(g_flash.arrChar[idx + CFLASH_FPIndex_START] == CENTRYFLAG_IDLE);
	return	(g_flash.arrChar[idx + CFLASH_FPIndex_START] != CENTRYFLAG_BUSY);
}

/**********************************************************************
 * name: fingprintCmd
 * description:	fingerprint operation command
 * input: cmd
 * output: no
 * return: no
 * date/time: 
 **********************************************************************/
void fingprintCmd(unsigned char *cmd)
{
	struct charData_s charData;
#if	1
	charData.ucVal = 0x3a;
	charQueueIn_irq(&g_com2TxQue, &charData); 
	charData.ucVal = 0xff;
	charQueueIn_irq(&g_com2TxQue, &charData); 
	charData.ucVal = 0xff;
	charQueueIn_irq(&g_com2TxQue, &charData); 
	charData.ucVal = 0xff;
	charQueueIn_irq(&g_com2TxQue, &charData); 
	charData.ucVal = cmd[0];
	charQueueIn_irq(&g_com2TxQue, &charData); 
	charData.ucVal = cmd[1];
	charQueueIn_irq(&g_com2TxQue, &charData); 
	charData.ucVal = cmd[2];
	charQueueIn_irq(&g_com2TxQue, &charData); 
	charData.ucVal = 0x3a+0xff+0xff+0xff+cmd[0]+cmd[1]+cmd[2];
	charQueueIn_irq(&g_com2TxQue, &charData); 

#endif
}

/**
 * This command have no responsed
 **/
void FP_INIT(void)
{
	unsigned char cmd[3];
	
	cmd[0] = 0;
	cmd[1] = 0;
	cmd[2] = 0;
	
	fingprintCmd(cmd);
}
/*******************************************************************************
 * function: FPRegister
 * Description:ע��ָ��
 * input: 	ID - ָ����ID��
 * 			stime - ��ʱʱ��
 * output:��no
 * return:  no
 *
 * other: �����յ�0x3a+0x02+0xaa+0x01��ʾע��ɹ�
 *******************************************************************************/
void FP_Register(unsigned char ID, unsigned char stime)
{
	unsigned char cmd[3];
	
	cmd[0] = 2;
	cmd[1] = stime;
	cmd[2] = ID;
	
	fingprintCmd(cmd);
}
/*******************************************************************************
 * function: FPDeRegister
 * Description:ע��ָ����Χ�ڵ�ָ��ģ��
 * input: 	ID1 - ָ����Χ����ʼID��
 * 		 	ID2 - ָ����Χ�Ľ���ID��
 * output:��no
 * return:  no
 *
 * other: �˲���֮��һ����޸�������
 *******************************************************************************/
void FP_DeRegister(unsigned char ID1,unsigned char ID2)
{
	unsigned char cmd[3];
	
	cmd[0] = 4;
	cmd[1] = ID1;
	cmd[2] = ID2;
	
	fingprintCmd(cmd);
}
/*******************************************************************************
 * function: FPQuery
 * ��ѯȫ��ָ��(0..57)
 * 
 * input: no
 * output:
 * return: no
 * other: �Ӵ����յ�������Ϊ0x3a+0x03+0xaa+0x01���ʾ�˴β�ѯ�ɹ�
 *******************************************************************************/
void FP_Query(unsigned char ID1,unsigned char ID2)
{
	unsigned char cmd[3];
	
	cmd[0] = 3;
	//cmd[1] = 0;
	cmd[1] = ID1;
	//cmd[2] = 57;
	cmd[2] = ID2;
	
	fingprintCmd(cmd);
}
/*******************************************************************************
 * function: FP_AddByManager
 * Description: ����Ա¼��ָ��(������ָ��ģ��յ��龰)
 * input: ����Աָ��ID
 * output: no
 * return: 	�ҵ��Ŀ��õ�id��
 *			1, 2, ...
 *			0 - û����Ӧ���õĿ�ID��
 *			
 *******************************************************************************/
//unsigned char FP_AddByManager(unsigned char Manager_ID) //���ݹ���Ȩ�� 1��2��3���ָ��
unsigned char FP_AddByManager(unsigned char Manager_ID, unsigned char stime) //���ݹ���Ȩ�� 1��2��3���ָ��
{
	int i = 0;
	
	if(Manager_ID == 0)
	{
		//for (i = 1; i <= 57; i++)
		for (i = 1; i <= C1STMANAGEE; i++)           //�ҿ�ָ��ģ�����(��һ�����ߣ���������¼�룬ֱ��¼��)
    	{
    		if(fingerTab_entryIsEmpty(i)) 	//��ѯ�׸���ID����
    		{
    			break;
    		}
    	}
    	if(i > C1STMANAGEE) 		//�����ѯ��20�Ŷ�û�п�ָ���˳�
    	{
    		i = 0;
    	}
	}
	else if(Manager_ID == 1)
	{
		if(fingerTab_entryIsEmpty(2))          //�ҿ�ָ��ģ�����(�ڶ������ߣ�����¼�������������)
    	{
    		i = 2;
    	}
    	else
    	{
			for (i = C2NDMANAGES; i <= C2NDMANAGEE; i++)      //��ѯ��ָ��
    		{
    			if(fingerTab_entryIsEmpty(i)) 	//��ѯ�׸���ID����
    			{
    				break;
    			}
    		}
   		}
    	if (i > C2NDMANAGEE) 		//�����ѯ��38�Ŷ�û�п�ָ���˳�
      	{
    		i = 0;
      	} 
	}
	else if(Manager_ID == 2)
	{
		for (i = C3RDMANAGES; i <= C3RDMANAGEE; i++)                 //��ʾ��ָ��
    	{
    		if(fingerTab_entryIsEmpty(i)) 	//��ѯ�׸���ID����
    		{
    			break;
    		}
    	}
    	if (i > C3RDMANAGEE)
      	{
    		i = 0;
      	}  		//�����ѯ��38�Ŷ�û�п�ָ���˳�
	}
	
	//if((i >= 3) && (i <= 57))
	if((i >= 1) && (i <= CTOTALFINGER - 1))
	{
		//FP_Register(i, 1);
		FP_Register(i, stime);
	}
	return	i;
}

/*******************************************************************************
 * function: FP_DelByManager
 * Description: ����Ա¼��ָ��(������ָ��ģ��յ��龰)
 * input: ����Աָ��ID
 * output: no
 * return: no
 * 		
 *******************************************************************************/
unsigned char FP_DelByManager(unsigned char Manager_ID) //���ݹ���Ȩ�� 1��2��3ɾ��ָ��
{
	if(Manager_ID == 0)                 //��һ������ɾ����������ָ��
    {
    	FP_DeRegister(1, CTOTALFINGER - 1);
    }
    else if(Manager_ID == 1)      		//�ڶ�������ɾ������¼��ָ��
    {
    	FP_DeRegister(C2NDMANAGES, C2NDMANAGEE);
    }
    else if(Manager_ID == 2)      		//����������ɾ������¼��ָ��
    {
    	FP_DeRegister(C3RDMANAGES, C3RDMANAGEE);
    }
    
    return	0;
}
/*******************************************************************************
 * function: FP_DelAll
 * Description: ɾ��ȫ��ָ��
 * input: no
 * output: no
 * return: no
 * 		
 *******************************************************************************/
void FP_DelAll(void) //���ݹ���Ȩ�� 1��
{
	#if	1
	FP_DeRegister(0, CTOTALFINGER - 1);
	#else
	unsigned char cmd[3];
	
	cmd[0] = 4;
	cmd[1] = 0;
	cmd[2] = CTOTALFINGER - 1;
	
	fingprintCmd(cmd);
	#endif
}

/*******************************************************************************
 * function: FP_DelSuperUser
 * Description: ɾ����һ������ָ��
 * input: no
 * output: no
 * return: no
 * 
 *******************************************************************************/
void FP_DelSuperUser(void)
{
	FP_DeRegister(0, 0);
}

#if	0
 unsigned char FPrespTab[][4] = {
 	{0, 0, 0, 0},
 	{0x3a, 0x00, 0xaa, 0x00},	/** ��ʼ��ָ��ģ��ɹ�(����5Bytes) **/
 	{0x3a, 0x02, 0xaa, 0x00},	/** ע��ָ��ģ��ɹ�(����5Bytes) **/
 	{0x3a, 0x03, 0xaa, 0x01},	/** ����ָ��ģ��ɹ�(����6Bytes) **/
 	{0x3a, 0x04, 0xaa, 0x00},	/** ɾ��ָ��ģ��ɹ�(����5Bytes, !!!ע��ָ��ģ��ֻ��Ҫ����������) **/
 	{0, 0, 0, 0},
 };
 #endif
 
 /******************************************************************************
  * function: FP_GetRespData
  * Description: get received data from FIFO
  * input: src - pointer of FIFO
  * output: arr - the data get from FIFO
  * return: < 12 the length of data get from FIFO
  * Get all of the data in the FIFO
  *******************************************************************************/
int FP_GetRespData(charBuf_queue_t *src, unsigned char arr[]) 
{
    int i = 255;
	charData_t charData;
    
    /** �ҵ�һ�����ݣ���������һ���Ǳ������� **/
	while(charQueueOut_irq(src, &charData) == TRUE)
    {
        if(charData.ucVal == 0x3a)		/** �ҵ���һ��0x3a **/
        {
    		i = 0;
    		arr[i] = charData.ucVal;
            break;
        }
    }
    /** �����Ƶ��ҵ�comTmpBuf�С��������ţ���֮ǰ����һ���Ǳ������� **/
    if(i == 0)
    {
    	while(charQueueOut_irq(src, &charData) == TRUE)
    	{
    		i++;
    		if(i < 12)
    		{
    			arr[i] = charData.ucVal;
    		}
    	}
    	
    	return	i + 1;		/** �յ��������ݶ����� **/
	}
	
    return	0;		/** û���յ��������� **/
}

/** �鷵��ֵ����,  **/
int FP_GetRespType(unsigned char arr[]) 
{
	if(arr[0] == 0x3a)
	{
    	return	arr[1];
	}
	return	0xff;	/** ����ʶ����������� **/
}

/***********************************************************
 * function: FP_RespInit
 * Description: check responsed of initial command
 * input: arr - the responsed data
 *        ucLen - the data length
 * output: no
 * return: ResultCode, same as FP-ResultCode
 * other:
 * 		check responsed: initial
 * 		0x3a+0x00+0xab+0x00+0xe5
 * 		0x3a+0x00+0xaa+0x00+0xe4
 ***********************************************************/
int FP_RespInit(unsigned char arr[], unsigned char ucLen)
//int FP_RespInit(unsigned char arr[])
{	
	#define	CSTEP	5
	if((arr[0 + CSTEP] == 0x3a) && (arr[1 + CSTEP] == 0x00) && (arr[3 + CSTEP] == 0x00))
	{
		return	arr[2 + CSTEP];		/** 0xaa or oxab **/
	}
	
	if((arr[0] == 0x3a) && (arr[1] == 0x00) && (arr[3] == 0x00))
	{
		return	arr[2];		/** 0xaa or oxab **/
	}
	#undef 	CSTEP
	
	return	0;	/** �޹�����, ����0 **/
}

/***********************************************************
 * function: FP_RespRgst
 * Description: check responsed of register command
 * input: arr - the responsed data
 *        ucLen - the data length
 * output: no
 * return: ResultCode, same as FP-ResultCode
 *
 * check responsed: Register
 *		0x3a+0x02+0xab+0x00+0xe7
 *		0x3a+0x02+0xaa+0x01+0xID+CKSUM
 * 3�ֿ�������:	case 1: 0x3a+0x02+0xab+0x00+0xe7	(0xab)
 *				case 2: 0x3a+0x02+0xaa+0x01+0xID+CKSUM	(0xaa)
 *				case 3: 0x3a+0x02+0xab+0x00+0xe7+0x3a+0x02+0xaa+0x01+0xID+CKSUM	(0xab+0xaa)	- һ��Ҫ�ȴ������
 ***********************************************************/
int FP_RespRgst(unsigned char arr[], unsigned char *ucpLen)
{
	int i;	  
//	charData_t charData;
	unsigned char ucTmp;


/** case 3 **/
	ucTmp = 0;
	for(i = 5; i < 10; i++)
	{
		ucTmp += arr[i];
	}
	if((arr[5] == 0x3a) && (arr[6] == 0x02) && (arr[7] == 0xaa) && (arr[8] == 0x01) && (arr[10] == ucTmp))
	{
		*ucpLen = arr[9];
		return	arr[2];				/** 0xaa **/
	}

	/** case 2 **/
	ucTmp = 0;
	for(i = 0; i < 5; i++)
	{
		ucTmp += arr[i];
	}
	if((arr[0] == 0x3a) && (arr[1] == 0x02) && (arr[2] == 0xaa) && (arr[3] == 0x01) && (arr[5] == ucTmp)) 
	//if((arr[0] == 0x3a) && (arr[1] == 0x02) && (arr[2] == 0xaa) && (arr[3] == 0x01))
	{
		*ucpLen = arr[4];
		return	arr[2];				/** 0xaa **/
	}

	/** case 1 **/
	ucTmp = 0;
	for(i = 0; i < 4; i++)
	{
		ucTmp += arr[i];
	}
	if((arr[0] == 0x3a) && (arr[1] == 0x02) && (arr[2] == 0xab) && (arr[3] == 0x00) && (arr[4] == ucTmp))
	//if((arr[0] == 0x3a) && (arr[1] == 0x02) && (arr[2] == 0xab) && (arr[3] == 0x00))
	{
		return	arr[2];				/** 0xaa or oxab **/
	}
		
	return	0;	/** �޹�����, ����0 **/
}

 
/***********************************************************
 * FP register done
 * 3�ֿ�������:	0x3a+0x02+0xab+0x00+0xe7	(0xab)
 *				0x3a+0x02+0xaa+0x01+0xID+CKSUM	(0xaa)
 *				0x3a+0x02+0xab+0x00+0xe7+0x3a+0x02+0xaa+0x01+0xID+CKSUM	(0xab+0xaa)
 *				0	 1	  2	   3	4	 5	  6	   7	8	 9	  10
 ***********************************************************/


 /***********************************************************
 * FP Deregister doing/done
 * 3�ֿ�������:	0x3a+0x04+0xab+0x00+sum	(0xab)
 *				0x3a+0x04+0xaa+0x00+sum	(0xaa)
 *				0x3a+0x04+0xab+0x00+0xe7+0x3a+0x02+0xaa+0x00+CKSUM	(0xab+0xaa)
 *				0	 1	  2	   3	4	 5	  6	   7	8	 9	  
 ***********************************************************/

int FP_RespDeRgst(unsigned char arr[], unsigned char *pManagerId)
{	
	int i;
	unsigned char ucTmp;

	/** case 3 **/
	ucTmp = 0;
	for(i = 5; i < 8; i++)
	{
		ucTmp += arr[i];
	}
	if((arr[5] == 0x3a) && (arr[6] == 0x04) && (arr[7] == 0xaa) && (arr[8] == 0x00) && (arr[9] == ucTmp))
	{
		return	arr[7];				/** 0xaa **/
	}
	
	/** case 2 **/
	ucTmp = 0;
	for(i = 0; i < 4; i++)
	{
		ucTmp += arr[i];
	}
	if((arr[0] == 0x3a) && (arr[1] == 0x04) && (arr[2] == 0xaa) && (arr[3] == 0x00) && (arr[4] == ucTmp)) 
	//if((arr[0] == 0x3a) && (arr[1] == 0x02) && (arr[2] == 0xaa) && (arr[3] == 0x01))
	{
		return	arr[2];				/** 0xaa **/
	}
	
	/** case 1 **/
	ucTmp = 0;
	for(i = 0; i < 4; i++)
	{
		ucTmp += arr[i];
	}
	if((arr[0] == 0x3a) && (arr[1] == 0x04) && (arr[2] == 0xab) && (arr[3] == 0x00) && (arr[4] == ucTmp))
	//if((arr[0] == 0x3a) && (arr[1] == 0x02) && (arr[2] == 0xab) && (arr[3] == 0x00))
	{
		return	arr[2];				/** 0xaa or oxab **/
	}
	
	return	0;	/** �޹�����, ����0 **/
}

 /***********************************************************
 * check responsed: search
 *
 * check responsed: Register
 *		0x3a+0x03+0xab+0x00+0xe7
 *		0x3a+0x03+0xaa+0x01+0xID+CKSUM
 * 3�ֿ�������:	case 1: 0x3a+0x03+0xab+0x00+CKSM	(0xab)
 *				case 2: 0x3a+0x03+0xaa+0x01+0xID+CKSM	(0xaa)
 *				case 3: 0x3a+0x03+0xab+0x00+0xe7+0x3a+0x02+0xaa+0x01+0xID+CKSM	(0xab+0xaa)
 ***********************************************************/
int FP_RespSearch(unsigned char arr[], unsigned char *pManagerId)
{
	int i;	  
//	charData_t charData;
	unsigned char ucTmp;

	/** case 1 **/
	ucTmp = 0;
	for(i = 0; i < 4; i++)
	{
		ucTmp += arr[i];
	}
	if((arr[0] == 0x3a) && (arr[1] == 0x03) && (arr[2] == 0xab) && (arr[3] == 0x00) && (arr[4] == ucTmp))
	//if((arr[0] == 0x3a) && (arr[1] == 0x03) && (arr[2] == 0xab) && (arr[3] == 0x00))
	{
		return	arr[2];				/** 0xaa or oxab **/
	}
	
	/** case 2 **/
	ucTmp = 0;
	for(i = 0; i < 5; i++)
	{
		ucTmp += arr[i];
	}
	if((arr[0] == 0x3a) && (arr[1] == 0x03) && (arr[2] == 0xaa) && (arr[3] == 0x01) && (arr[5] == ucTmp)) 
	//if((arr[0] == 0x3a) && (arr[1] == 0x03) && (arr[2] == 0xaa) && (arr[3] == 0x01))
	{
		*pManagerId = arr[4];
		return	arr[2];				/** 0xaa **/
	}

	/** case 3 **/
	ucTmp = 0;
	for(i = 5; i < 10; i++)
	{
		ucTmp += arr[i];
	}
	if((arr[5] == 0x3a) && (arr[6] == 0x03) && (arr[7] == 0xaa) && (arr[8] == 0x01) && (arr[10] == ucTmp))
	{
		*pManagerId = arr[9];
		return	arr[7];				/** 0xaa **/
	}
	
	return	0;	/** �޹�����, ����0 **/
}

/////////////////////////////////////////////////////
/**********************************************************************
 * name: ADC_getValue 
 * description: get ADCref value(default value 0)
 **********************************************************************/
int ADC_getValue(void)
{
	return	(g_flash.arrChar[CFLASH_ADC_VALUE] | (g_flash.arrChar[CFLASH_ADC_VALUE + 1] << 8));
}

/**********************************************************************
 * name: ADC_getValue 
 * description: get ADCref value(default value 0)
 **********************************************************************/
void ADC_setValue(int value)
{
	g_flash.arrChar[CFLASH_ADC_VALUE] = (value & 0xff);
	g_flash.arrChar[CFLASH_ADC_VALUE + 1] = ((value >> 8) & 0xff);
	//return	(g_flash.arrChar[CFLASH_ADC_VALUE] | (g_flash.arrChar[CFLASH_ADC_VALUE + 1] << 8));
}

/**********************************************************************
 * name: resetOP_setValue/resetOP_getValue
 * description: get reset operation control value(default value 0xff)
 **********************************************************************/
int resetOP_getValue(void)
{
	return	g_flash.arrChar[CFLASH_RESET_VALUE];
}

void resetOP_setValue(int value)
{
	g_flash.arrChar[CFLASH_RESET_VALUE] = (value & 0xff);
}

/*********************************************************************
 *
 *
 *********************************************************************/
#define CPSWORDLEN	(8 + 2)
#define	CPSWORDCNT	(8)
//#define	CENTRYFLAG_BUSY	0xa5

/**********************************************************************
 * name: psword_isFull 
 * description: check weather the password is full
 **********************************************************************/
int psword_isFull(void)
{
	int	i;
	for(i = 0; i < CPSWORDCNT; i++)
	{
		if(g_flash.arrChar[i * 10 + CFLASH_PASSWD_START] != CENTRYFLAG_BUSY)
		{
			return	0;		/** find a idle entry **/
		}
	}
	
	return	1;
}

/**********************************************************************
 * name: psword_isEmpty 
 * description: check weather the password is  empty
 **********************************************************************/
int psword_isEmpty(void)
{
	int	i;
	for(i = 0; i < CPSWORDCNT; i++)
	{
		if(g_flash.arrChar[i * 10 + CFLASH_PASSWD_START] == CENTRYFLAG_BUSY)
		{
			return	0;
		}
	}
	
	return	1;
}

/**********************************************************************
 * name: psword_inputConfirm 
 * description: check weather the password is same as before
 **********************************************************************/
int psword_inputConfirm(unsigned char _password[])
{
	int	i;
	for(i = 0; i < 8; i++)
	{
		if(_password[i] != _password[i + 8])
		{
			return	0;
		}
	}
	
	return	1;
}

/**********************************************************************
 * name: password_Query 
 * description: Query weather the password is registered
 **********************************************************************/
int password_Query(unsigned char _password[])
{
	int i, j;

	for(i = 0; i < CPSWORDCNT; i++)
	{
		if(g_flash.arrChar[i * 10 + CFLASH_PASSWD_START] == CENTRYFLAG_BUSY)	/** ����������, ���Խ��бȽ� **/
		{
			for(j = 0; j < CPSWORDLEN - 2; j++)
			{
				if(g_flash.arrChar[i * 10 + CFLASH_PASSWD_START + j + 1] != _password[j])
				{
					break;			/** ����ȣ���ǰһ������ȶ�ʧ�ܣ��˳����Ƚ���һ�� **/
				}
			}
			if(j == CPSWORDLEN - 2)	/** û����;�˳���Ҳ���ǱȽ�ͨ�� **/
			{
				return	1;			/** ���ҳɹ� **/
			}
		}
	}
	return	0;	/** ����ʧ�� **/
}

 /******************************************************************************
  *	function: 
  * Description: ���һ������ 
  * input: 	_password
  * output: 
  * return:  �ɹ�����1, ʧ�ܷ���0
  *
  * other: ע������Ĵ���˳��
  ******************************************************************************/
int password_add(unsigned char _password[])
{
	int i, j;

	for(i = 0; i < CPSWORDCNT; i++)
	{
		if(g_flash.arrChar[i * 10 + CFLASH_PASSWD_START] != CENTRYFLAG_BUSY)	/** ����û������, ���Դ��� **/
		{
			g_flash.arrChar[i * 10 + CFLASH_PASSWD_START] = CENTRYFLAG_BUSY;
			
			for(j = 0; j < CPSWORDLEN - 2; j++)
			{
				g_flash.arrChar[i * 10 + CFLASH_PASSWD_START + j + 1] =  _password[j];
			}
			
			return	1;		/** ��ӳɹ� **/
		}
	}
	return	0;	/** ���ʧ�� **/
}

/*******************************************************************************
 * function:
 * Description: ɾ��ȫ������(��Ӧ������λ�����㼴��)
 * input: no
 * output: no
 * return: no
 *
 *******************************************************************************/
int password_del(void)
{
	int i;
	
	for(i = 0; i < CPSWORDCNT; i++)
	{
		g_flash.arrChar[i * 10 + CFLASH_PASSWD_START] = CENTRYFLAG_IDLE;		/** ����û������ **/
	}
	
	return	1;
}

/*******************************************************************************
 * get/set password_errno
 *******************************************************************************/
unsigned char getpassword_errno(void)
{
	return	g_flash.arrChar[CFLASH_PASSWD_ERR];
}

void setpassword_errno( unsigned char value)
{
	g_flash.arrChar[CFLASH_PASSWD_ERR] = value;
}
