

#include "wtn6.h"
#include "delay.h"

__IO uint8_t wtn_bit_state;//λ״̬����ʶ
__IO uint8_t wtn_data_ptr;//����ָ��
__IO uint8_t wtn_data[WTN_MAX_DATA_LEN];//��������
__IO uint8_t wtn_data_len;//���ݳ���
__IO uint16_t wtn_delay=0;

void Voice::Init(void)
{
	GPIO_InitTypeDef 				 GPIO_InitStructure;
	NVIC_InitTypeDef  			 NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	RCC_AHBPeriphClockCmd(WTN_CLK_GPIO_CLK | WTN_DATA_GPIO_CLK | WTN_POWER_GPIO_CLK | WTN_BUSY_GPIO_CLK, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16,ENABLE);
	
	/* Configure PC10 and PC11 in output pushpull mode */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_InitStructure.GPIO_Pin = WTN_CLK_BIT;
	GPIO_Init(WTN_CLK_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = WTN_DATA_BIT;
	GPIO_Init(WTN_DATA_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = WTN_POWER_BIT;
	GPIO_Init(WTN_POWER_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	
	GPIO_InitStructure.GPIO_Pin = WTN_BUSY_BIT;
	GPIO_Init(WTN_BUSY_PORT, &GPIO_InitStructure);
	
	TIM_DeInit(WTN_TIMx);
  TIM_TimeBaseStructure.TIM_Prescaler = (SystemCoreClock/100000)-1; //10us
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;     //��
  TIM_TimeBaseStructure.TIM_Period = 5000;           //���ֵ
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;                    //ʱ��Ԥ��Ƶ
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;                //�ظ���������
	
	TIM_TimeBaseInit(WTN_TIMx, &TIM_TimeBaseStructure);
	TIM_ClearFlag(WTN_TIMx,TIM_FLAG_Update);                       //�����־λ
	TIM_ITConfig(WTN_TIMx,TIM_IT_Update,ENABLE);                   //ʹ������ж�
	

	/* USART IRQ Channel configuration */
  NVIC_InitStructure.NVIC_IRQChannel =TIM16_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	WTN_CLK_1;
}

int8_t Voice::Play_Num(uint8_t volume,uint8_t speed,char *str_num)
{
	uint8_t len,i;
	uint8_t num_tmp;
	if(WTN_TIMx->CR1&TIM_CR1_CEN)
	{
		return -1;
	}
	if(speed==0)
	{
		return -3;
	}
	wtn_bit_state=0;
	wtn_data_ptr=0;
	wtn_data_len=0;
	wtn_delay=speed*WTN_DELAY_BUSY;
	wtn_data[wtn_data_len++]=0xE0|(volume&0x0f);
	len=strlen(str_num);
	if(len>(WTN_MAX_DATA_LEN-1))
	{
		return -2;
	}
	for(i=0;i<len;i++)
	{
		if((str_num[i]>='0')&&(str_num[i]<='9'))
		{
			num_tmp=str_num[i]-'0';
			if(num_tmp==0)
			{
				wtn_data[wtn_data_len]=23;
			}
			else
			{
				wtn_data[wtn_data_len]=num_tmp+13;
			}
			wtn_data_len++;
		}
	}
	WTN_POWER_ON;
	WTN_CLK_1;
	WTN_TIMx->CNT=0;
	WTN_TIMx->ARR=WTN_DELAY_PWR;
	WTN_TIMx->CR1 |= TIM_CR1_CEN;
	return 0;
}

int8_t Voice::Play_voice(uint8_t volume,uint8_t speed,uint8_t num)
{
	if(WTN_TIMx->CR1&TIM_CR1_CEN)
	{
		return -1;
	}
	if(speed==0)
	{
		return -3;
	}
	wtn_bit_state=0;
	wtn_data_ptr=0;
	wtn_data_len=2;
	wtn_delay=speed*WTN_DELAY_BUSY;
	//wtn_data[0]=volume;
	wtn_data[0]=0xE0|(volume&0x0f);
	wtn_data[1]=num;
	WTN_POWER_ON;
	WTN_CLK_1;
	WTN_TIMx->CNT=0;
	WTN_TIMx->ARR=WTN_DELAY_PWR;
	WTN_TIMx->CR1 |= TIM_CR1_CEN;
	return 0;
}

int8_t Voice::Read_Busy(void)
{
	if(WTN_TIMx->CR1&TIM_CR1_CEN)
	{
		return -1;
	}
	return 0;
}

//uint8_t i;
//	while(!WTN_BUSY_READ);
//	WTN_CLK_0;
//	delay_ms(5); //��ʱ 5ms
//	for(i=0;i<8;i++)
//	{
//		if(data&0x01)
//		{
//			WTN_DATA_1;
//		}
//		else
//		{
//			WTN_DATA_0;
//		}
//		WTN_CLK_0;
//		delay_us(300); //��ʱ 300us
//		WTN_CLK_1; //����
//		delay_us(300); //��ʱ 300us
//		data>>=1;
//	}
//	WTN_CLK_1;
extern "C" void TIM16_IRQHandler(void);

void TIM16_IRQHandler(void)
{
	static uint8_t bit=0;
	switch(wtn_bit_state)
	{
		case 0:
			WTN_CLK_0;
			WTN_TIMx->ARR=WTN_DELAY_PRE;//5ms��ʱ
			bit=0x01;
			if(wtn_data[wtn_data_ptr]&bit)
			{
				WTN_DATA_1;
			}
			else
			{
				WTN_DATA_0;
			}
			wtn_bit_state++;
			break;
		case 1: //���� 1bit
		case 3: //���� 2bit
		case 5:	//���� 3bit
		case 7: //���� 4bit
		case 9:	//���� 5bit
		case 11://���� 6bit
		case 13://���� 7bit
		case 15://���� 8bit		
			WTN_CLK_1;
			WTN_TIMx->ARR=WTN_DELAY_DATA;//300us��ʱ
			wtn_bit_state++;
			break;
		case 2: //����׼�� 2bit
		case 4: //����׼�� 3bit
		case 6: //����׼�� 4bit
		case 8: //����׼�� 5bit
		case 10://����׼�� 6bit
		case 12://����׼�� 7bit
		case 14://����׼�� 8bit
			WTN_CLK_0;
			bit<<=1;
			if(wtn_data[wtn_data_ptr]&bit)
			{
				WTN_DATA_1;
			}
			else
			{
				WTN_DATA_0;
			}
			WTN_TIMx->ARR=WTN_DELAY_DATA;//300us��ʱ
			wtn_bit_state++;
			break;
		case 16:
			WTN_CLK_0;
			WTN_TIMx->ARR=WTN_DELAY_DATA;//300us��ʱ
			wtn_bit_state++;
			break;
		case 17://�ȴ�æµִ�����
			if(WTN_BUSY_READ)
			{
				WTN_TIMx->ARR=wtn_delay;//
				if(wtn_data_ptr<(wtn_data_len-1))
				{
					wtn_data_ptr++;
				}
				else
				{
					WTN_TIMx->CR1 &= ~TIM_CR1_CEN;
					WTN_POWER_OFF;
				}
				wtn_bit_state=0;
			}
			break;
		default:
			WTN_TIMx->CR1 &= ~TIM_CR1_CEN;
			break;
	}
	
	WTN_TIMx->SR = (uint16_t)~TIM_IT_Update;
}

