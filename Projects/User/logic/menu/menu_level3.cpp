/************************ (C) COPYLEFT 2018 Merafour *************************

* File Name          : menu.h
* Author             : Merafour
* Last Modified Date : 03/09/2018
* Description        : This file provides the LTM022A69B LCD library.

******************************************************************************/

/* Includes -----------------------------------------------------------------*/
#include "menu_level.h"
//#include "Adafruit_ssd1306syp.h"
//#include "Infr.h"
//#include "delay.h"
//#include "user.h"
#include <stdio.h>
//#include "data_flash.h"
//#include "ssd1306syp.h"
//#include "fingerprint.h"

static void func_see(void)
{
	menu.func_see();
}
void MENU_LEVEL::func_see(void)
{
	char _speed[][3]= {"��", "��", "��", "��"};
	uint8_t key=1;
	sys_data_t _sys;
	uint8_t user_index=0;
	uint8_t i=0;
	_data.get_sys(&_sys);
	while(waitting(0)>0)
	{
		//key=_input.Infr_Scan();
		if(key)
		{
			waitting(_sys.back); 
			if('2'==key || '^'==key)
			{
				if(0==user_index) user_index=2;
				user_index--;
			}
			if('8'==key || '!'==key)
			{
				user_index++;
				if(2==user_index) user_index=0;
			}
			if('4'==key || '<'==key)
			{
				break;
			}
			if('6'==key || '>'==key)
			{
			}
			_display.clear(false);
			i=0;
			switch(user_index)
			{
				case 0:
					_display.format(0, i*16, 2, 0, "����:%d s", _sys.back/1000);
					i++;
					if(i>=4) break;
				case 1:
					_display.format(0, i*16, 2, 0, "����:%d", _sys.volume); 
					i++;
					if(i>=4) break;
				case 2:
					_display.format(0, i*16, 2, 0, "����У��:%s", (_sys.verify)?"��":"��"); 
					i++;
					if(i>=4) break;
				case 3:
					_display.format(0, i*16, 2, 0, "���ת��:%s��", _speed[_sys.speed&0x3]);
					i++;
					if(i>=4) break;
				case 4:
					_display.format(0, i*16, 2, 0, "������ʱ:%d s", _sys.door);
					i++;
					if(i>=4) break;
				default:
					break;
			}
		}
		key=_input.Infr_Scan();
//		_display.format(0, 0, 2, 0, "����: %d s", _sys.back/1000); 
//		_display.format(0, 16, 2, 0, "����: %d", _sys.volume); 
//		_display.format(0, 32, 2, 0, "����У��: %s", (_sys.verify)?"��":"��"); 
//		_display.format(0, 48, 2, 0, "���ת��: %s��", _speed[_sys.speed&0x3]); 
	}
}

static void func_door(void)
{
	menu.func_door();
}
void MENU_LEVEL::func_door(void)
{
	uint16_t delay = 0;
	sys_data_t _sys;
	_data.get_sys(&_sys);
	{
		delay = menu.get_user_input(0, 3, 0, "������ʱ:\n[5-200]s");
		if(delay>200 || delay<5) 
		{
			_display.vformat(false, 0, 16, "�����ܾ�", 2000, "����Χ:\n[5-200]s");
		}
		else
		{
			_sys.door = delay;
			_data.save_sys(&_sys);
		}
	}
}

static void func_speed(void)
{
	menu.func_speed();
}
void MENU_LEVEL::func_speed(void)
{
	uint8_t key=0;
	sys_data_t _sys;
	_data.get_sys(&_sys);
	_display.format(false, 0, 16, 2, 0, "1. �ߵ�");
	_display.format(0, 32, 2, 0, "2. �е�");
	_display.format(0, 48, 2, 0, "3. �͵�");
	while(waitting(0)>0)
	{
		key=_input.Infr_Scan();
		if(key)
		{
			waitting(_sys.verify);
			if('1'==key) // back
			{
				_sys.speed=0;
				_data.save_sys(&_sys);
				break;
			}
			else if('2'==key) // OK
			{
				_sys.speed=1;
				_data.save_sys(&_sys);
				break;
			}
			else if('3'==key) // OK
			{
				_sys.speed=2;
				_data.save_sys(&_sys);
				break;
			}
			else 
			{
				break;
			}
		}
	}
}

static void func_backlight(void)
{
	menu.level3_backlight();
}
void MENU_LEVEL::level3_backlight(void)
{
	uint16_t delay = 0;
	sys_data_t _sys;
	_data.get_sys(&_sys);
	{
		delay = menu.get_user_input(0, 2, 0, "����ʱ��:\n[5-30]s");
		if(delay>30 || delay<5) _display.vformat(false, 0, 16, "�����ܾ�", 2000, "����Χ:\n[5-30]s");
		else
		{
			_sys.back = delay*1000;
			_data.save_sys(&_sys);
		}
	}
}
static void func_delay(void)
{
	menu.level3_delay();
}
void MENU_LEVEL::level3_delay(void)
{
	uint16_t delay = 0;
	sys_data_t _sys;
	_data.get_sys(&_sys);
	{
		delay = menu.get_user_input(0, 2, 0, "����ʱ��:\n[5-30]s");
		if(delay>30 || delay<5) menu.show_title(0, 32, "����Χ:\n[5-30]s");
		else
		{
			_sys.back = delay*1000;
			_data.save_sys(&_sys);
		}
	}
}
static void func_volume(void)
{
	menu.level3_volume();
}
void MENU_LEVEL::level3_volume(void)
{
	uint16_t volume = 0;
	sys_data_t _sys;
	_data.get_sys(&_sys);
	{
		volume = menu.get_user_input(0, 2, 0, "���� [0-100]");
		if(volume>100) menu.show_title(0, 32, "����Χ:\n[0-100]s");
		else
		{
			_sys.volume = volume;
			_data.save_sys(&_sys);
		}
	}
}
static void func_verify(void)
{
	menu.level3_verify();
}
void MENU_LEVEL::level3_verify(void)
{
	uint8_t key=0;
	sys_data_t _sys;
	_data.get_sys(&_sys);
	_display.format(false, 0, 16, 2, 0, "%c0: ����У�鿪", (0==_sys.verify)?'*':' ');
	_display.format(0, 32, 2, 0, "%c1: ����У���", (1==_sys.verify)?'*':' ');
	while(waitting(0)>0)
	{
//		display.clear(0);
//		display.setCursor(0,16);
//		display.printf("%c0: ����У�鿪", (0==_sys.verify)?'*':' ');
//		display.setCursor(0,32);
//		display.printf("%c1: ����У���", (1==_sys.verify)?'*':' ');
//		display.update();
//		_display.format(false, 0, 16, 2, 0, "%c0: ����У�鿪", (0==_sys.verify)?'*':' ');
//		_display.format(0, 32, 2, 0, "%c1: ����У���", (1==_sys.verify)?'*':' ');
		key=_input.Infr_Scan();
		if(key)
		{
			waitting(_sys.verify);
			if('0'==key) // back
			{
				_sys.verify=0;
				_data.save_sys(&_sys);
				break;
			}
			else if('1'==key) // OK
			{
				_sys.verify=1;
				_data.save_sys(&_sys);
				break;
			}
			else 
			{
				break;
			}
		}
	}
}
static void func_default(void)
{
	menu.level3_default();
}
void MENU_LEVEL::level3_default(void)
{
	uint32_t passwd=0;
	passwd = menu.get_user_input(0, 6, 1, "����������");
	if(-1!=_data.update_passwd(MENU::check_uid , passwd))
	{
		// OK
		_data.factory();
		menu.show_title(0, 32, "�ѻָ���������!");
		waitting(1);
		//delay_ms(100);
	}
	else 
	{
		//menu.show_title(0, 32, "�����ܾ�!");
		_display.vformat(false, 0, 0, "����ʧ��", 2000, "�����ܾ�!"); 
		//set_timeout(10000); // 10s
	}
}
static void func_passwd(void)
{
	menu.level3_passwd();
}
void MENU_LEVEL::level3_passwd(void)
{
	data_user_t _user;
	uint32_t passwd=0;
	uint32_t passwd_tmp=0;
	uint16_t count=0;
	sys_data_t _sys;
	_data.get_sys(&_sys);
	waitting(_sys.back);
	while(waitting(0)>0)
	{
		browse(&_user);
		//if((user_number_min<=_user.uid) && (user_number_max>_user.uid))
		if((user_number_min>_user.uid) || (user_number_max<=_user.uid))
		{
			return ;
		}
//		if((MENU::check_uid>(1+user_number_min)) && (_user.uid<=(1+user_number_min)))
//		{
//			menu.show_title(0, 32, "�����ܾ�!");
//		}
		// check
		//if((_user.uid>(1+user_number_min)) && (_user.uid<(lock_admin_max+user_number_min)) && (_user.uid!=check_uid))
		if((check_uid>(1+user_number_min)) && (check_uid<(lock_admin_max+user_number_min)) && (_user.uid!=check_uid))
		{
			show_title(0, 32, "�����ܾ�!");
			//_display.format(false, 0, 0, 2, 2000, "�����ܾ�"); 
			count++;
			if(count>=3) break;
			continue;
		}
		else if(check_uid>=(lock_admin_max+user_number_min)) break;
		_voice.play_info(DATA::get_volume(), 1, "����������");
		passwd = get_user_input(0, 6, 0, "������������", _user.uid-user_number_min);
		if(0==waitting(0)) break;
		_voice.play_info(DATA::get_volume(), 1, "���ٴ���������");
		passwd_tmp = get_user_input(0, 6, 0, "�ٴ���������", _user.uid-user_number_min);
		if(0==waitting(0)) break;
		if(passwd==passwd_tmp)
		{
			if(-1!=_data.update_passwd(_user.uid , passwd))
			{
				// OK
				//menu.show_title(0, 32, "�����Ѹ���!");
				_display.vformat(false, 0, 0, "�������", 2000, "�����Ѹ���!"); 
				//printf("%d", uid);
				break;
			}
			else 
			{
				//menu.show_title(0, 32, "��������ʧ��!");
				_display.vformat(false, 0, 0, "����ʧ��", 2000, "��������ʧ��!"); 
				//set_timeout(10000); // 10s
			}
		}
		else
		{
			//menu.show_title(0, 32, "���벻һ��!");
			_display.vformat(false, 0, 0, "����ʧ��", 2000, "���벻һ��!"); 
			count++;
			//if(count<3) waitting(back);
			//else break;
			if(count>=3) break;
		}
	}
}
static void func_rfid(void)
{
	menu.show_title(0, 32, "rfid");
}
static void func_finger(void)
{
	menu.level3_finger();
}
void MENU_LEVEL::level3_finger(void)
{
	//menu.show_title(0, 32, "finger");
	data_user_t _user;
	int8_t ret;
	uint16_t count=0;
	uint16_t tmp_num = 0;//
	waitting(MENU::back);
	while(waitting(0)>0)
	{
		menu.browse(&_user);
		//if((user_number_min<=_user.uid) && (user_number_max>_user.uid))
		if((user_number_min>_user.uid) || (user_number_max<=_user.uid))
		{
			return ;
		}
		// check
		//if((_user.uid>(1+user_number_min)) && (_user.uid<(lock_admin_max+user_number_min)) && (_user.uid!=MENU::check_uid))
		if((check_uid>(1+user_number_min)) && (check_uid<(lock_admin_max+user_number_min)) && (_user.uid!=check_uid))
		{
			//menu.show_title(0, 32, "�����ܾ�!");
			_display.vformat(false, 0, 0, "�����ܾ�", 2000, "�����ܾ�!"); 
			count++;
			if(count>=3) break;
			continue;
		}
		else if(check_uid>=(lock_admin_max+user_number_min)) break;
		if(_user.finger>0)
		{
			tmp_num = _user.finger;
#if 0
			ret=_finger.Delete_Fingerprint(NULL,tmp_num,tmp_num);
			if(ret!=0) ret=_finger.Delete_Fingerprint(NULL,tmp_num,tmp_num);
#endif
			ret=_finger.remove(tmp_num,tmp_num);
			if(ret==0)
			{
				//_display.format(false, 0, 0, 2, 2000, "ɾ���ɹ�");
				_display.vformat(false, 0, 0, "�������", 2000, "ɾ���ɹ�!"); 
				_data.update_finger(_user.uid, 0); // del
			}
			else
			{
				//_display.format(false, 0, 0, 2, 2000, "ɾ��ʧ��");
				_display.vformat(false, 0, 0, "����ʧ��", 2000, "ɾ��ʧ��!"); 
			}
		}
		//else
		{
			_display.format(false, 0, 0, 2, 2000, "У��ָ��");
			//ret=fingerprint.Verify_Fingerprint(NULL,(uint16_t *)&tmp_num);
			ret=_finger.verify(tmp_num, 5000);
			if(0==ret)
			{
				//_display.format(false, 0, 0, 2, 2000, "ָ�ƴ����ѹ��� %d", tmp_num);
				_display.vformat(false, 0, 0, "�������", 2000, "ָ�ƴ����ѹ��� %d", tmp_num); 
				_data.update_finger(_user.uid, tmp_num);
				return ;
			}
		}
		_display.format(false, 0, 0, 2, 200, "�밴����ָ %d",tmp_num);
		{
			//ret=fingerprint.Fingerprint_Entry(menutask,(uint16_t *)&tmp_num);
			ret=_finger.add(tmp_num, 5000);
			if(ret==0)
			{
				//_display.format(false, 0, 0, 2, 2000, "д��ɹ� %d",tmp_num);
				_display.vformat(false, 0, 0, "�������", 2000, "д��ɹ� %d",tmp_num); 
				_data.update_finger(_user.uid, tmp_num);
				break;
			}
			else
			{
				//_display.format(false, 0, 0, 2, 2000, "д��ʧ�� %d %d",(int8_t)0-ret, tmp_num);
				_display.vformat(false, 0, 0, "����ʧ��", 2000, "д��ʧ�� %d %d",(int8_t)0-ret, tmp_num); 
			}
		}
//		count++;
//		if(count>=3) break;
	}
}
static void func_finger_del(void)
{
	menu.level3_finger_del();
}
void MENU_LEVEL::level3_finger_del(void)
{
	//menu.show_title(0, 32, "��ָ��");
	data_user_t _user;
	int8_t ret;
	uint16_t count=0;
	uint16_t tmp_num = 0;//
	waitting(MENU::back);
	while(waitting(0)>0)
	{
		menu.browse(&_user);
		//if((user_number_min<=_user.uid) && (user_number_max>_user.uid))
		if((user_number_min>_user.uid) || (user_number_max<=_user.uid))
		{
			return ;
		}
		// check
		//if((_user.uid>(1+user_number_min)) && (_user.uid<(lock_admin_max+user_number_min)) && (_user.uid!=MENU::check_uid))
		if((check_uid>(1+user_number_min)) && (check_uid<(lock_admin_max+user_number_min)) && (_user.uid!=check_uid))
		{
			//menu.show_title(0, 32, "�����ܾ�!");
			_display.vformat(false, 0, 0, "�����ܾ�", 2000, "�����ܾ�!"); 
			count++;
			if(count>=3) break;
			continue;
		}
		else if(check_uid>=(lock_admin_max+user_number_min)) break;
		if(_user.finger>0)
		{
			tmp_num = _user.finger;
#if 0
			ret=_finger.Delete_Fingerprint(NULL,tmp_num,tmp_num);
			if(ret!=0) ret=_finger.Delete_Fingerprint(NULL,tmp_num,tmp_num);
#endif
			ret=_finger.remove(tmp_num,tmp_num);
			if(ret==0)
			{
				//_display.format(false, 0, 0, 2, 2000, "ɾ���ɹ�");
				_display.vformat(false, 0, 0, "�������", 2000, "ɾ���ɹ�!"); 
				_data.update_finger(_user.uid, 0); // del
				break;
			}
			else
			{
				//_display.format(false, 0, 0, 2, 2000, "ɾ��ʧ��");
				_display.vformat(false, 0, 0, "����ʧ��", 2000, "ɾ��ʧ��!"); 
				//NVIC_SystemReset();
			}
		}
		else
		{
			//_display.format(false, 0, 0, 2, 2000, "���û���ָ����Ϣ");
			_display.vformat(false, 0, 0, "����ʧ��", 2000, "���û���ָ����Ϣ!"); 
			count++;
			if(count>=3) break;
		}
	}
}

//static void func_dhcp(void)
//{
//	uint32_t passwd_tmp=0;
//	passwd_tmp=menu.get_user_input(0, 6, 1, "DHCP PASSWD");
//	if(passwd_tmp)
//	{
//		//update_admin_dhcp(passwd_tmp);
//		menu.show_title(0, 32, "DHCP Update!");
//	}
//	else
//	{
//		menu.show_title(0, 32, "DHCP Update Fail!");
//	}
//}

menu_t info_menu[] = 
{
	{func_passwd, "������"},
	{func_rfid,   "��RFID"},
	{func_finger, "��ָ��"},
	{func_finger_del, "ɾ��ָ��"},
	//{func_dhcp,   "��DHCP"},
};
const unsigned int info_menu_size = sizeof(info_menu)/sizeof(info_menu[0]);
menu_t sys_menu[] = 
{
	{func_see,       "�鿴"},
	{func_backlight, "������ʱ"},
	{func_delay,     "������ʱ"},
	{func_volume,    "����"},
	{func_verify,    "����У��"},
	{func_speed,     "���ת��"},
	{func_door,      "������ʱ"},
	{func_default,   "�ָ���������"}
};
const unsigned int sys_menu_size = sizeof(sys_menu)/sizeof(sys_menu[0]);

void func_info(void)
{
	menu.common(info_menu, info_menu_size, 4);
}
void func_set(void)
{
	menu.common(sys_menu, sys_menu_size, 4);
}
