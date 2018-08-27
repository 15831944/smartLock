/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __WTN6_H
#define __WTN6_H

#include "main.h"
#include "stdint.h"
#include <string.h>
#include "object.h"


#define WTN_CLK_GPIO_CLK      RCC_AHBPeriph_GPIOB
#define WTN_DATA_GPIO_CLK     RCC_AHBPeriph_GPIOB
#define WTN_POWER_GPIO_CLK    RCC_AHBPeriph_GPIOA
#define WTN_BUSY_GPIO_CLK     RCC_AHBPeriph_GPIOB

#define WTN_CLK_PORT   				GPIOB
#define WTN_CLK_BIT    				GPIO_Pin_1

#define WTN_DATA_PORT  				GPIOB
#define WTN_DATA_BIT   				GPIO_Pin_0

#define WTN_POWER_PORT  			GPIOA
#define WTN_POWER_BIT   			GPIO_Pin_5

#define WTN_BUSY_PORT  				GPIOB
#define WTN_BUSY_BIT   				GPIO_Pin_2

#define WTN_CLK_0    					WTN_CLK_PORT->BRR=WTN_CLK_BIT
#define WTN_CLK_1   					WTN_CLK_PORT->BSRR=WTN_CLK_BIT

#define WTN_DATA_0   					WTN_DATA_PORT->BRR=WTN_DATA_BIT
#define WTN_DATA_1   					WTN_DATA_PORT->BSRR=WTN_DATA_BIT

#define WTN_POWER_ON          WTN_POWER_PORT->BSRR=WTN_POWER_BIT
#define WTN_POWER_OFF         WTN_POWER_PORT->BRR=WTN_POWER_BIT

#define WTN_BUSY_READ         (WTN_BUSY_PORT->IDR & WTN_BUSY_BIT)

#define WTN_TIMx              TIM16

#define WTN_DELAY_PWR         5000
#define WTN_DELAY_PRE         500
#define WTN_DELAY_DATA        30
#define WTN_DELAY_BUSY        100

#define WTN_MAX_DATA_LEN      20

class Voice : public voice_obj {
public:
	virtual void init(void){Init();}
	virtual int8_t play_num(uint8_t volume,uint8_t speed,char *str_num){return Play_Num(volume, speed, str_num);}
	virtual int8_t play_voice(uint8_t volume,uint8_t speed,uint8_t num)
	{
		while(Read_Busy()) __nop();
		{
			return Play_voice(volume, speed, num);
		}
		//return -1;
	}
	virtual int8_t busy(void){return Read_Busy();}
	void Init(void);
	/*
		������:Play_Num
		����:����һ������
		����volume->������С 0-15
		����speed->���ŵļ��1-255ms (���Ŷ�������Ǹò�������Ч)
		����:num->����
		����ֵ:0->�������� -1->æµ -2 ���ݳ��ȳ���Ԥ�� -3�ٶȲ�������
	*/
	int8_t Play_Num(uint8_t volume,uint8_t speed,char *str_num);
	/*
		������:Play_voice
		����:����һ������
	  ����volume->������С 0-15
		����speed->���ŵļ��1-255ms (���Ŷ�������Ǹò�������Ч)
		����:num->�������
		����ֵ:0->�������� -1->æµ -2 ���ݳ��ȳ���Ԥ�� -3�ٶȲ�������
	*/
	int8_t Play_voice(uint8_t volume,uint8_t speed,uint8_t num);
	/*
		������:Read_Busy
		����:��ȡ����״̬��æµ״̬
		����ֵ:0->���� -1->æµ
	*/
	int8_t Read_Busy(void);
	
private:
	
};


//extern Voice voice;

#endif



