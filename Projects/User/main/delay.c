#include "delay.h"
#include "stm32f0xx.h"

static uint8_t  fac_us=0;							//us��ʱ������			   
//static uint16_t fac_ms=0;							//ms��ʱ������,��ucos��,����ÿ�����ĵ�ms��
static uint64_t uwTick=0;
static volatile uint16_t timeout_tick=0; // timeout timer
static uint16_t Seconds=0;
static uint32_t counter_time=0;
uint16_t backlight_delay=0;

//__weak void delay_tick(void)
//{
//	;//if(display_delay>0) display_delay--;
//}
//__weak void SecondsTick(void)
//{
//}
extern void dhcp_check(void);
__weak void dhcp_check(void)
{
	;
}
__weak void seconds_poll(void)
{
}
uint32_t GetSeconds(void)
{
	return counter_time;
}
void SetSeconds(uint32_t sec)
{
	counter_time = sec;
}
extern uint8_t uart_timeout;
//extern void delay_tick(void);
//systick�жϷ�����,ʹ��ucosʱ�õ�
//extern void timeout_Handler(void);
//extern void TimingDelay_Decrement(void);
void SysTick_Handler(void)
{	
	uwTick++;
	Seconds++;
	if(Seconds>=1000)
	{
		counter_time++;
		Seconds=0;
		dhcp_check();
		seconds_poll();
	}
	//if(timeout_tick<60000) timeout_tick++; // ms, 60000 ==> 60s
	if(timeout_tick>0) timeout_tick--; // ms
	if(backlight_delay>0) backlight_delay--; // ms
	if(uart_timeout>0) uart_timeout--;
	//delay_tick();
	//timeout_Handler();
	//TimingDelay_Decrement();
}
uint32_t waitting_time(const uint32_t ms)
{
	if(ms)
	{
		timeout_tick=ms;
		backlight_delay = ms+5000;
	}
	return timeout_tick;
}
//void set_timeout(const uint32_t ms)
//{
//	timeout_tick=ms;
//}
//volatile uint32_t get_timeout(void)
//{
//	return timeout_tick;
//}

uint64_t GetTick(void)
{
  return uwTick;
}

			   
//��ʼ���ӳٺ���
//��ʹ��OS��ʱ��,�˺������ʼ��OS��ʱ�ӽ���
//SYSTICK��ʱ�ӹ̶�ΪHCLKʱ�ӵ�1/8
//SYSCLK:ϵͳʱ��
void delay_init()
{
	SysTick_Config(SystemCoreClock / 1000);
	fac_us=48;
	//fac_ms=(uint16_t)fac_us*1000;					//��OS��,����ÿ��ms��Ҫ��systickʱ����   
}								    

//��ʱnus
//nusΪҪ��ʱ��us��.
void delay_us(uint32_t nus)
{		
	uint32_t ticks;
	uint32_t told,tnow,tcnt=0;
	uint32_t reload=SysTick->LOAD;					//LOAD��ֵ	    	 
	ticks=nus*fac_us; 							//��Ҫ�Ľ�����	  		 
	tcnt=0;
	//delay_osschedlock();						//��ֹOS���ȣ���ֹ���us��ʱ
	told=SysTick->VAL;        					//�ս���ʱ�ļ�����ֵ
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;		//����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;				//ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
		}  
	};
	//delay_osschedunlock();						//�ָ�OS����									    
}
void delay_ms(uint16_t nms)
{
	delay_us((uint32_t)(nms*1000));					//��ͨ��ʽ��ʱ  
}








































