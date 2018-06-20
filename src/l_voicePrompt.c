
#include "stm32f10x_lib.h" 
#include "string.h"
#include "../inc/CONST.h"
#include "../inc/ctype.h" 
#include "../inc/global.h" 
#include "../inc/macro.h"
#include "../inc/debug.h"

#include "arch.h"
#include "l_voicePrompt.h"
#include "charQue.h"

/**
 * 5ms-20ms-5ms-(0.25+0.75)ms*8
 * [40- 160- 40- (2+6)*8] * 125us
 **/

#if	0

/** 100us��Ӧ1024��ѭ�� **/
void delay100usLoop(vu32 __100us)
{
	int i, j;
	
	for(i = 0; i < __100us; i++)
	{
		//for(j = 0; j < 1000; j++); 
		for(j = 0; j < 1024; j++);
		//for(j = 0; j < 1200; j++);
	}
}

void delay1ms(vu32 __1ms)
{
	int	i;
	for(i = 0; i < __1ms; i++)
	{
		delay100usLoop(10);
	}
}

void SendOneLine(unsigned char addr)
{
	int i;
	
	#if	1
	//rst=0; 			/* ��оƬ���и�λ */
	VOPRESET_L();
	//delay1ms(5); 	/* ��λ�źű��ֵ͵�ƽ5ms */
	delay1ms(10);
	//rst=1;
	VOPRESET_H();
	//delay1ms(17);	/* ��λ�źű��ָߵ�ƽ17ms */
	delay1ms(20);
	#endif
	
	IRQ_disable();
	
	//sda=0;
	VOPDATA_L();
	//delay1ms(5); 	/* �����ź����ڵ͵�ƽ5ms */
	delay1ms(10);
	
	for(i = 0; i < 8; i++)
	{
		//sda=1;
		VOPDATA_H();
		if(addr & 0x01)
		{
			delay100usLoop(6); /* �ߵ�ƽ�ȵ͵�ƽΪ600us��200us����ʾ��������1 */
			//sda=0;
			VOPDATA_L();
			delay100usLoop(2);
		}
		else
		{
			delay100usLoop(2); /* �ߵ�ƽ�ȵ͵�ƽΪ200us��600us����ʾ��������0 */
			//sda=0;
			VOPDATA_L();
			delay100usLoop(6);
		}
		addr >>= 1;
	}
	//sda=1;
	VOPDATA_H();
	
	IRQ_enable();
}
#endif
