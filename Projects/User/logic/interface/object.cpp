/************************ (C) COPYLEFT 2018 Merafour *************************

* File Name          : object.cpp
* Author             : Merafour
* Last Modified Date : 04/23/2018
* Description        : This file provides the object library.
* Description        : ͨ�ýӿ���.
******************************************************************************/
#include <stdint.h>
#include <string.h>
#include "object.h"
#include "safety.h"
#include "delay.h"
//#include "data.h"
typedef struct {
	const uint8_t num;  // ���
	const char *title;  // ��������
}voice_t;

static voice_t voice_info[]={
	{0 , "��"},
	{1 , "����"},
	{2 , "�������û����"},
	{3 , "����������"},
	{4 , "���ٴ���������"},
	{5 , "����ʧ��"},
	{6 , "�����ܾ�"},
	{7 , "�������"},
	{8 , "�������"},
	{9 , "û��Ȩ��"},
	{10, "��ȷ��"},
	{11, "���ѿ�"},
	{12, "����˵�"},
	{13, "����"},
	{14, "1"},
	{15, "2"},
	{16, "3"},
	{17, "4"},
	{18, "5"},
	{19, "6"},
	{20, "7"},
	{21, "8"},
	{22, "9"},
	{23, "0"},
	{24, "��ӭ����"},
	{25, "��ӭ����"},
	{26, "��δ��"},
	{27, "���ѿ�"},
	{28, "��ע��"},
};

uint8_t voice_obj::search(const char* _voice)
{
	uint8_t i=0;
	for(i=0; i<sizeof(voice_info)/sizeof(voice_info[0]); i++)
	{
		if(0==strcmp(voice_info[i].title, _voice))
		{
			return voice_info[i].num;
		}
	}
	return 0;
}
int8_t voice_obj::play_info(uint8_t volume, uint8_t speed, const char* info)
{
	return play_voice(volume, speed, search(info));
}


uint64_t rgbw_obj::tick(void)
{
	return GetTick();
}
uint64_t model_obj::tick(void)
{
	return GetTick();
}
void model_obj::_delay_ms(uint16_t nms)
{
	delay_ms(nms);
}

uint64_t safety::tick(void)
{
	return GetTick();
}
void safety::_delay_ms(uint16_t nms)
{
	delay_ms(nms);
}


