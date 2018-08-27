/************************ (C) COPYLEFT 2018 Merafour *************************

* File Name          : main.cpp
* Author             : Merafour
* Last Modified Date : 04/29/2018
* Description        : .

******************************************************************************/
#include "main.h"
#include "delay.h"
#include <string.h>
#include <stdint.h>
#include "ssd1306syp.h"
#include "rgbw.h"
#include "RTC.h"
#include "menu_level.h"
#include "eeprom.h"
#include "data_flash.h"
#include "input_manage.h"
#include "dhcp_check.h"
#include "uart.h"
#include "fingerprint.h"
#include "power.h"
#include "safety.h"
#include "wtn6.h"
#include "model.h"
#include "rfid.h"

#ifdef __cplusplus
 extern "C" {
#endif
//int main(void);
void seconds_poll(void);

#ifdef __cplusplus
}
#endif

void seconds_poll(void)
{
	if(menu.error_delay>0) menu.error_delay--;
	if(menu.door_delay>0) menu.door_delay--;
}

//int16_t match_passwd(const uint8_t uid, uint32_t input);
void dhcp_test(void);


/*
finger_state=1 �ɼ���һ��ͼ��ɹ�
finger_state=2 ��ָ�뿪
finger_state=3 �ɼ��ڶ���ͼ��ɹ�
finger_state=4 ��ָ�뿪
finger_state=5 �ɼ�������ͼ��ɹ�
finger_state=6 ��ָ�뿪
finger_state=7 �������ݺϳ�ģ��ɹ�
finger_state=8 ��ȡ��ģ��ųɹ�
*/
uint8_t menutask(uint8_t state,uint8_t change)
{
	if(change)
	{
		switch(state)
		{
			case 0://�������
			case 2://��ָ�뿪
			case 4://��ָ�뿪	
				display.format(false, 0, 0, 2, 500, "�����ָ,%d",state);
			
				break;
			case 1://�ɼ���һ��ͼ��ɹ�
			case 3://�ɼ��ڶ���ͼ��ɹ�
			case 5://�ɼ�������ͼ��ɹ�
				display.format(false, 0, 0, 2, 500, "���ƿ���ָ,%d",state);
				break;
			case 6://��ָ�뿪
				//display.format(false, 0, 0, 2, 0, "���ںϳ�ģ��,%d",state);//���ںϳ�ģ��
				break;
			case 7://�������ݺϳ�ģ��ɹ�
				//display.format(false, 0, 0, 2, 0, "�����ҿ�ģ��,%d",state);//�����ҿ�ģ��
				break;
			case 8://��ȡ��ģ��ųɹ�
				//display.format(false, 0, 0, 2, 0, "д��ģ��,%d",state);//д��ģ��
				break;
		}
	}
	
	return 0;
}


//RGBW rgbw;
static RGBW rgbw_s;
rgbw_obj &rgbw=rgbw_s;

//DATA_FLASH data;
DATA_TYPE data;
dhcp_check dhcp;
static FingerPrint fingerprint_s;
safety &fingerprint=fingerprint_s;
static safe_rfid safe_rfid_s;
safety &rfid=safe_rfid_s;
#if 0
Voice voice;
#else
static Voice voice_s;
voice_obj &voice=voice_s;
#endif
static motor_obj motor_s(data);
model_obj &motor=motor_s;
ssd1306syp display(voice);
input_manage input(voice, rgbw);
MENU_LEVEL menu(display, input, data, fingerprint, rfid, voice, motor); // һ���˵�
int main(void)
{
	RTC_TimeTypeDef Time;
	RTC_DateTypeDef Date;
	uint8_t Seconds=0;
	uint8_t key=0;
	sys_data_t _sys;
	uint8_t user_avtic=0;
	Power_Switch_Init();
	
	// class init
	rgbw.init();
	rgbw.show(13);
	delay_init();
	RTC_Timer_Init();
	display.initialize();
	data.Init();
	//data.Test();
	
	input.Init();
	//input.key_test();
	//dhcp_test();
	fingerprint.init(); // fingerprint.QS808_Init();
	//fingerprint.Test();
	rfid.init();
	voice.init();
	motor.init();
	delay_ms(100);
	
	//display.format(false, 0, 0, 2, 2000, "���ѹ�000000!"); 
	//delay_ms(3000);
	
	display.setTextSize(2);
	data.get_sys(&_sys);
	waitting_time(_sys.back);
	rgbw.flowing(3);
	backlight_delay = 5000;
	while (1) 
	{
		int16_t ret=0;
		rgbw.flowing();
		ret=motor.poll();    // ��״̬��ѯ
		switch(ret)
		{
			case MODEL_CLOSE:
				display.vformat(false, 0, 0, "��", 2000, "���ѹ�!"); 
				break;
			case MODEL_OPEN:
				display.vformat(false, 0, 0, "���ѿ�", 2000, "���ѿ�!"); 
				break;
			case MODEL_ERROR:
				display.vformat(false, 0, 0, "��ע��", 2000, "״̬����!"); 
				break;
			case MODEL_NULL:
			default:
				break;
		}
		/* Get the current Time */
		RTC_GetTime(RTC_Format_BIN, &Time);
		RTC_GetDate(RTC_Format_BIN, &Date);
		if(Seconds != Time.RTC_Seconds) // ����������,��ʾʱ��
		{
//			if(menu.error_delay>0) menu.error_delay--;
//			if(menu.door_delay>0) menu.door_delay--;
#if 0
			display.clear(0);
			display.setCursor(20,3);
			display.setTextSize(2);
			display.printf("WELCOME");
			//display.printf("%0.4d��%0.2d��%0.2d�� %d\n\r", Date.RTC_Year+2000, Date.RTC_Month, Date.RTC_Date, Date.RTC_WeekDay);
			display.setTextSize(2);
			display.setCursor(0,32);
			display.printf(" %0.2d:%0.2d:%0.2d \n\r", Time.RTC_Hours, Time.RTC_Minutes, Time.RTC_Seconds);
			//display.setCursor(0,48);
			//display.printf("  %d \n\r", Date.RTC_WeekDay);
			display.setTextSize(1);
			display.setCursor(24,52);
			//display.printf("%0.4d.%0.2d.%0.2d %d\n\r", Date.RTC_Year+2000, Date.RTC_Month, Date.RTC_Date, motor.voltage());
			display.printf("%0.4d.%0.2d.%0.2d %02X\n\r", Date.RTC_Year+2000, Date.RTC_Month, Date.RTC_Date, motor.door());
			display.setTextSize(2);
			display.update();
#else
			display.clear(0);
			if(menu.door_delay)
				display.format(20, 3, 2, "LOCK %2d\n\r",  menu.door_delay);
			else
				display.format(20, 3, 2, "WELCOME");
			display.format(0, 32, 2, " %0.2d:%0.2d:%0.2d \n\r", Time.RTC_Hours, Time.RTC_Minutes, Time.RTC_Seconds);
			//display.format(24, 52, 2, "%0.4d.%0.2d.%0.2d %d\n\r", Date.RTC_Year+2000, Date.RTC_Month, Date.RTC_Date, motor.voltage());
			display.format(24, 52, 1, "%0.4d.%0.2d.%0.2d %02X\n\r", Date.RTC_Year+2000, Date.RTC_Month, Date.RTC_Date, motor.door());
			display.setTextSize(2);
			display.update();
#endif
			Seconds = Time.RTC_Seconds;
		}
		if(0==backlight_delay) // ����
		{
			display.off();
			menu.clear();
			NVIC_DisableIRQ(SysTick_IRQn);
			rgbw.show(13);
			motor.sleep(); // send sleep order
			delay_ms(600);
			motor.poll();
			uart_deinit();
			/* Request to enter SLEEP mode */
			//__WFI();
			/* Request to enter STOP mode with regulator in low power mode */
			PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
			//PWR_EnterSTANDBYMode();
			SystemInit();
			NVIC_EnableIRQ(SysTick_IRQn);
			display.on();
			motor.init();
			//delay_init();
			//SystemCoreClockUpdate();
			//display.format(false, 0, 0, 2, 10000, "SYS %2d\n\r",  SystemCoreClock);
			backlight_delay = 5000; // 5s
		}
		key=input.Infr_Scan(); // ����ɨ��
		if(key || safety::revent())  // ����ָ�ƻ�RFID�¼�,ָ����RFID�� safety ��̳�
		{
			display.on();
			data.get_sys(&_sys);
			waitting_time(_sys.back);
			//timer_count++;
			user_avtic += 10;
			if(user_avtic>64) user_avtic=64;
		}
		if(user_avtic>1)  // �������
		{
			// ����¼�
			if(EVENT_RFID == safety::revent())   // RFID
			{
				uint16_t id;
				if(0==rfid.read(id))
				{
					// check
					menu.rfid(id);
				}
				safety::cevent(EVENT_RFID);      // clear
			}
			if(EVENT_FINGER == safety::revent()) // ָ��
			{
				uint16_t id;
				if(0==fingerprint.read(id))
				{
					// check
					menu.finger(id);
				}
				safety::cevent(EVENT_FINGER);    // clear
			}
			// ����
			if('*'==key) // menu
			{
				voice.play_info(DATA::get_volume() , 1, "����˵�"); // ����˵�
				input.operation(OPERATION_MENU, key);
				menu.option();
			}
			else if('#'==key) // BEEP
			{
				input.operation(OPERATION_BEEP, key);
				display.vformat(false, 0, 0, "����", 2000, "BEEP");
			}
			else if(('0'<=key) && ('9'>=key)) // passwd
			{
				uint8_t passwd[32];
				uint8_t len = 0;
				uint8_t i=0;
				uint32_t input_num = 0;
				int16_t sign = -1;
				input.operation(OPERATION_NUM, key);
				if(menu.door_delay>0)
				{
					//display.vformat(false, 0, 0, "�����ܾ�", 2000, "��%d�������", error_delay);
					voice.play_info(DATA::get_volume() , 1, "����������"); // ��
					len = menu.get_text_input(0, passwd, 20, 0, "6λ��̬����");
					// DHCP6+'#'
					if(7==len) // DHCP6
					{
						uint16_t index=0;
						data_user_t _user;
						for(index=2; index<lock_admin_max; index++)
						{
							data.get_data(&_user, index);
							if((user_number_min<=_user.uid) && (user_number_max>_user.uid))
							{
								sign = dhcp.dhcp6_check(passwd, _user.devnumber);
								if(1==sign)
								{
									menu.door_delay = 0;
									display.vformat(false, 0, 0, "�������", 2000, "�ѽ���");
									break;
								}
							}
						}
					}
					key=0;
					user_avtic=1;
					//input.operation(OPERATION_NULL, 0);
					input.recover(0);
					continue;
				}
#if 0
				//passwd_handle(key);
				uint16_t uid=0;
				uint16_t uid2=0;
				uint32_t passwd=0;
				passwd = menu.get_user_input(key, 8, 0, "����������");
				uid2=data.check_admin(1+user_number_min, passwd); // static passwd
				if(0xFFFF==uid2) 
				{
					menu.show_title(0, 32, "Ȩ�޾ܾ�!");
				}
				else
				{
					menu.show_title(0, 32, "��ȷ��!");
				}
#else
				//voice.play_voice(DATA::get_volume(),1,3); // ����������
				voice.play_info(DATA::get_volume() , 1, "����������"); // ��
				len = menu.get_text_input(key, passwd, 20, 0, "����������");
				if(len<4) 
				{
					key=0;
					user_avtic=1;
					//input.operation(OPERATION_NULL, 0);
					input.recover(0);
					continue;
				}
				input_num=0;
				for(i=0; i<len; i++)
				{
					if(('0'<=passwd[i]) && ('9'>=passwd[i])) input_num = input_num*10 + (passwd[i]-'0');
					else break;
				}
				//menu.show_title(0, 32, (const char* )passwd, input);
				menu.match_passwd(1, input_num);  // static passwd,�������볤��
				// DHCP4+'#'
				if((5==len) && (4!=menu.passwd_len)) // DHCP4, ��̬��������λ���뻥��
				{
					menu.dhcp4(passwd);
				}
				// DHCP6+'#'
				else if((7==len) && (6!=menu.passwd_len)) // DHCP6, ��̬��������λ���뻥��
				{
					//menu.show_title(0, 32, "DHCP6");
					menu.dhcp6(passwd);
				}
				// DHCP8+'#'
				else if((9==len) && (8!=menu.passwd_len)) // DHCP8, ��̬�������λ���뻥��
				{
					menu.dhcp8(passwd);
				}
				else if(len<10) // static passwd
				{
					sign = menu.match_passwd(1, input_num);  // static passwd
					menu.showToast(sign, VERIFY_PASSWD, 1);
				}
				// order+'#': 6+1+2+6+1+1
				else if(17<=len) // order,ָ��
				{
					menu.show_title(0, 32, "order");
					check_t _check;
					//menu.show_title(0, 32, "order");
					//sign = dhcp.dhcp4_check(passwd, 345678);
					dhcp.order_decode(passwd, 345678, &_check);
					menu.showToast(_check.status, 0, 0);
				}
#endif
				//rgbw.flowing(1);
				
			}
			key=0;
			user_avtic=1;
			//input.operation(OPERATION_NULL, 0);
			input.recover(0);
		}
	}
}


void dhcp_test(void)
{
	//uint8_t passwd[32]="6108#";
	//uint8_t passwd[32]="605554#";
	//uint8_t passwd[32]="06503820#";
	uint8_t passwd[32] = "608324*632256741#";
	uint8_t len = 0;
	uint8_t i=0;
	uint32_t input = 0;
	int16_t sign = -1;
	//len = menu.get_text_input(key, passwd, 20, 0, "����������");
#if 1
	//const uint8_t* text = sub.dhcp4_build(2, 345678);
	//const uint8_t* text = sub.dhcp6_build(2, 345678);
	//const uint8_t* text = sub.dhcp8_build(2, 360480);
	const uint8_t* text = sub.order_encode(2, 345678, 1, 360480, 1);
	memcpy(passwd, text, 20);
	menu.show_title(0, 32, (const char*)passwd);
	//while(1) ;
#endif
	len = strlen((const char*)passwd);
	input=0;
	for(i=0; i<len; i++)
	{
		if(('0'<=passwd[i]) && ('9'>=passwd[i])) input = input*10 + (passwd[i]-'0');
		else break;
	}
	//menu.show_title(0, 32, (const char* )passwd, input);
	// DHCP4+'#'
	if(5==len) // DHCP4
	{
		// DHCP4
		//menu.show_title(0, 32, "DHCP4");
		sign = dhcp.dhcp4_check(passwd, 345678);
		menu.showToast(sign, VERIFY_DHCP4, 0);
	}
	// DHCP6+'#'
	else if(7==len) // DHCP6
	{
		//menu.show_title(0, 32, "DHCP6");
		sign = dhcp.dhcp6_check(passwd, 345678);
		menu.showToast(sign, VERIFY_DHCP6, 0);
	}
	// DHCP8+'#'
	else if(9==len) // DHCP8
	{
		//menu.show_title(0, 32, "DHCP8");
		sign = dhcp.dhcp8_check(passwd, 360480);
		menu.showToast(sign, VERIFY_DHCP8, 0);
	}
	// order+'#': 6+1+2+6+1+1
	else if(17<=len) // order
	{
		check_t _check;
		//menu.show_title(0, 32, "order");
		//sign = dhcp.dhcp4_check(passwd, 345678);
		dhcp.order_decode(passwd, 345678, &_check);
		menu.showToast(_check.status, 0, 0);
	}
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
