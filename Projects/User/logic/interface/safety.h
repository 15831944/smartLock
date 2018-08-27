/************************ (C) COPYLEFT 2018 Merafour *************************

* File Name          : safety.h
* Author             : Merafour
* Last Modified Date : 04/23/2018
* Description        : This file provides the safety library.
* Description        : ��ȫ�豸�ӿ�.
******************************************************************************/
#ifndef __SAFETY_H__
#define __SAFETY_H__

#include <stdint.h>



#define EVENT_RFID      0x01
#define EVENT_FINGER    0x02
#define EVENT_KEY       0x04
#define SAFETY_OK       0
#define SAFETY_TIMEOUT  -30

class safety {
private:
	static uint16_t event;
protected:
	/**
	  ��ȡϵͳʱ��,��λ:ms,�� main.cpp��ʵ��
     */
	uint64_t tick(void);
	/**
	  ��ʱ����,��λ:ms,�� main.cpp��ʵ��
     */
	void _delay_ms(uint16_t nms);
public:
    safety() {}
	/**
	  �¼���������:��,д,���
     */
	static uint16_t revent(void);
	static void wevent(const uint16_t mask);
	static void cevent(const uint16_t mask);
	
	/** ��ʼ���ӿ� */
	virtual void init(void) = 0;
		/** �����ӿ�,���ڳ���ʱ���³�ʼ���豸,�ڳ�ʼ��ǰ��Ҫ������Դ���ͷŲ��� */
	virtual void reset(void) = 0;
	/**
		��Ӻ���,��ӳɹ�ʱͨ�� id���ر�ʶ
		����ֵ��
		0���ɹ�, ��ֵ:����
     */
	virtual int8_t add(uint16_t &id, const uint16_t timeout) = 0;
	/**
		ɾ������
		����ֵ��
		0���ɹ�, ��ֵ:����
	 */
	virtual int8_t remove(const uint16_t id_start, const uint16_t id_end) = 0;
	/**
		У�麯��,ͨ�� id���ر�ʶ,�ú����ڴﵽ timeout(��λ:ms)ʱ��ʱ���뷵��
		����ֵ��
		0���ɹ�, SAFETY_TIMEOUT:��ʱ,  ��ֵ:����
	 */
	virtual int8_t verify(uint16_t &id, const uint16_t timeout) = 0;
	/** 
		��ѯ����,��������ʽ��ѯ�û��¼�,��������ʱͨ�� id���ر�ʶ
		����ֵ��
		0���ɹ�, ��ֵ:����
	 */
	virtual int8_t poll(uint16_t &id) = 0;
	/** 
		������,Ӧ�ò�ͨ���¼�������ѯ���ݵ���,��������ʱͨ�� id��ȡ��ʶ
		����ֵ��
		0���ɹ�, ��ֵ:����
	 */
	virtual int8_t read(uint16_t &id) = 0;
};

#endif // __SAFETY_H__

