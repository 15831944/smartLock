
#ifndef __FINGERPRINT_H
#define __FINGERPRINT_H


#include <stdint.h>
#include "safety.h"

#define  QS808_TIMx               TIM17
#define  QS808_CMD_TIMEOUT        350  //ms

#define  QS808_USARTx             USART1
#define  QS808_DMA_TX             DMA1_Channel2
#define  QS808_DMA_RX             DMA1_Channel3 

#define  CMD_TX_PREFIX            0xAA55
#define  CMD_RX_PREFIX            0x55AA

#define  CMD_TEST_CONNECTION 			0x0001 //�������豸��ͨѶ����
#define  CMD_SET_PARAM 						0x0002 // �����豸���� (Device ID, Security Level,Baudrate, Duplication Check, Auto Learn)
#define  CMD_GET_PARAM 						0x0003 //��ȡ�豸���� (Device ID, Security Level,Baudrate, Duplication Check, Auto Learn)
#define  CMD_GET_DEVICE_INFO 			0x0004 //��ȡ�豸��Ϣ
//#define  CMD_ENTER_IAP_MODE				0x0005 //���豸����Ϊ IAP ״̬
#define  CMD_GET_IMAGE 						0x0020 //�Ӳɼ����ɼ�ָ��ͼ�񲢱����� ImageBuffer��
#define  CMD_FINGER_DETECT 				0x0021 //���ָ������״̬
//#define  CMD_UP_IMAGE 						0x0022 //�������� ImageBuffer�е�ָ��ͼ���ϴ��� HOST
//#define  CMD_DOWN_IMAGE 					0x0023 //HOST ����ָ��ͼ��ģ��� ImageBuffer ��
//#define  CMD_SLED_CTRL 						0x0024 //���Ʋɼ�������ƵĿ�/�أ�ע���뵼�崫�������ô˹��ܣ�
#define  CMD_STORE_CHAR 					0x0040 //��ָ����� Ram Buffer �е� Template��ע�ᵽָ����ŵĿ���
#define  CMD_LOAD_CHAR 						0x0041 //��ȡ����ָ������е� Template ��ָ����ŵ�Ram Buffer
//#define  CMD_UP_CHAR 							0x0042 //��������ָ����ŵ� Ram Buffer �е�Template �ϴ��� HOST
//#define  CMD_DOWN_CHAR 						0x0043 //�� HOST ���� Template ��ģ��ָ����ŵ� RamBuffer ��
#define  CMD_DEL_CHAR 						0x0044 //ɾ��ָ����ŷ�Χ�ڵ� Template ��
#define  CMD_GET_EMPTY_ID 				0x0045 //��ȡָ����Χ�ڿ�ע��ģ�û��ע��ģ���һ��ģ���š�
#define  CMD_GET_STATUS 					0x0046 //��ȡָ����ŵ�ģ��ע��״̬��
#define  CMD_GET_BROKEN_ID 				0x0047 //���ָ����ŷ�Χ�ڵ�����ָ��ģ���Ƿ���ڻ�������
#define  CMD_GET_ENROLL_COUNT			0x0048 //��ȡָ����ŷ�Χ����ע���ģ�������
#define  CMD_GENERATE 						0x0060 //�� ImageBuffer �е�ָ��ͼ������ģ�����ݣ���������ָ����ŵ� Ram Buffer �С�
#define  CMD_MERGE 								0x0061 //�������� Ram Buffer �е���������ģ�������ںϳ�һ��ģ������
#define  CMD_MATCH 								0x0062 //ָ�� Ram Buffer �е�����ָ��ģ��֮�����1:1 �ȶ�
#define  CMD_SEARCH 							0x0063 //ָ�� Ram Buffer �е�ģ����ָ�ƿ���ָ����ŷ�Χ�ڵ�����ģ��֮����� 1:N �ȶ�
#define  CMD_VERIFY 							0x0064 //ָ�� Ram Buffer �е�ָ��ģ����ָ�ƿ���ָ����ŵ�ָ��ģ��֮����� 1:1 �ȶ�
//#define  CMD_SET_MODULE_SN 				0x0008 //���豸������ģ�����к���Ϣ��Module SN��
//#define  CMD_GET_MODULE_SN 				0x0009 //��ȡ���豸��ģ�����кţ� Module SN��
//#define  CMD_FP_CANCEL 						0x0025 //ȡ��ָ�Ʋɼ�������ֻ�����ڴ� TimeOut �����Ļ�����������
//#define  CMD_GET_ENROLLED_ID_LIST	0x0049 //��ȡ��ע�� ID �б�
//#define  CMD_ENTER_STANDY_STATE		0x000C //ʹģ���������״̬��

typedef enum {
	QS808_STATE_IDLE    = 0,
	QS808_STATE_BUSY    = 1,
	QS808_STATE_OK      = 2,
	QS808_STATE_READY   = 3,
	QS808_STATE_FAIL    = 4, 
	QS808_STATE_TIMEOUT = 5,
	QS808_STATE_PARAM   = 6,
}QS808_Status;


typedef enum {
	QS808_Id                = 0,
	QS808_Security_Level    = 1,
	QS808_Duplication_Check = 2,
	QS808_Baudrate          = 3,
	QS808_Auto_Learn        = 4, 
	//QS808_Fp_TimeOut        = 5,//����ָ�ƴ�����ģ��
}QS808_Parameter;

/*
ID                ��ʾ���豸��š������� 1 ~ 255 
Security_Level    ��ʾ��ȫ�ȼ���������ֵ��1~5 ��Ĭ��Ϊ��3
Duplication_Check ָ���ظ����״̬��/�ء������� 0 �� 1
Baudrate          1:9600bps, 2:19200bps, 3:38400bps, 4:57600bps, 5:115200bps 6:230400bps, 7:460800bps, 8:921600bps Ĭ��5
Auto_Learn        ��ʾָ��ģ����ѧϰ״̬��/�ء������� 0 �� 1
Fp TimeOut        ��ʾ�ɼ�ָ�Ƴ�ʱʱ�䣬������ֵ��1 ���� 60 ��
*/



typedef struct{
	uint16_t prefix;
	uint8_t  sid;
	uint8_t  did;
	uint16_t cmd;
	uint16_t len;
	uint8_t  data[16];
	uint16_t cks;
}QS808_CMD_Type;


typedef struct{
	uint16_t prefix;
	uint8_t  sid;
	uint8_t  did;
	uint16_t rcm;
	uint16_t len;
	uint16_t ret;
	uint8_t  data[14];
	uint16_t cks;
}QS808_CMD_Reply_Type;


typedef struct{
	uint16_t prefix;
	uint8_t  sid;
	uint8_t  did;
	uint16_t cmd;
	uint16_t len;
	uint8_t  data[500];
	uint16_t cks;
}QS808_DATA_Type;

typedef struct{
	uint16_t prefix;
	uint8_t  sid;
	uint8_t  did;
	uint16_t cmd;
	uint16_t len;
	uint16_t ret;
	uint8_t  data[498];
	uint16_t cks;
}QS808_DATA_Reply_Type;

#define QS808_MAX_BUF_SIZE  (sizeof(QS808_CMD_Type)+5)

class FingerPrint : public safety {
	
public:
	virtual void init(void);
	virtual void reset(void){} // �ú���Ϊ��
	virtual int8_t add(uint16_t &id, const uint16_t timeout);
	virtual int8_t remove(const uint16_t id_start, const uint16_t id_end);
	virtual int8_t verify(uint16_t &id, const uint16_t timeout);
	virtual int8_t poll(uint16_t &id);
	virtual int8_t read(uint16_t &id){return 0;}

	void QS808_Init(void);
	void Test(void);
private:
	int8_t Fingerprint_Entry(uint8_t (*menutask)(uint8_t state,uint8_t change),uint16_t *etmp_num);
	int8_t Delete_Fingerprint(uint8_t (*menutask)(void),uint16_t tmp_start,uint16_t tmp_end);
	int8_t Verify_Fingerprint(uint8_t (*menutask)(void),uint16_t *tmp_num);
private:
	QS808_Status QS808_Test_Connection(void);
	QS808_Status QS808_Set_Parameter(QS808_Parameter type,uint8_t val);
	QS808_Status QS808_Get_Parameter(QS808_Parameter type);
	//QS808_Status QS808_Get_Device_Info(void);
	QS808_Status QS808_Get_Image(void);
	QS808_Status QS808_Finger_Detect(void);
	QS808_Status QS808_Store_Char(uint16_t tmp_num,uint16_t ram_num);
	QS808_Status QS808_Del_Char(uint16_t tmp_start,uint16_t tmp_end);
	QS808_Status QS808_Get_Empty_Id(uint16_t tmp_start,uint16_t tmp_end);
	QS808_Status QS808_Get_Status(uint16_t tmp_num);//����OK��ʾ�Ѿ�ע�ᣬFAILû��ע��
	QS808_Status QS808_Get_Broken_Id(uint16_t tmp_start,uint16_t tmp_end);
	QS808_Status QS808_Get_Enroll_Count(uint16_t tmp_start,uint16_t tmp_end);
	QS808_Status QS808_Generate(uint16_t ram_num);
	QS808_Status QS808_Merge(uint16_t ram_num,uint8_t count);
	QS808_Status QS808_Match(uint16_t ram_num1,uint16_t ram_num2);
	QS808_Status QS808_Search(uint16_t ram_num,uint16_t tmp_start,uint16_t tmp_end);
	QS808_Status QS808_Verify(uint16_t tmp,uint16_t ram_num);
	QS808_Status QS808_Cmd_Response(uint8_t *parameter);
	QS808_Status QS808_Load_Char(uint16_t tmp_num,uint16_t ram_num);
};

extern uint8_t menutask(uint8_t state,uint8_t change);

#endif
