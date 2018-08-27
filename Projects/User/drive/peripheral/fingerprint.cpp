
#include "fingerprint.h"
#include <string.h>
#include "main.h"
#include "delay.h"
#include "ssd1306syp.h"


__align(4)
static __IO uint8_t qs808_tx_buf[QS808_MAX_BUF_SIZE];
__align(4) 
static __IO uint8_t qs808_rx_buf[QS808_MAX_BUF_SIZE];

static inline void QS808_Send_Data(uint16_t len);
static uint16_t QS808_Checksum(uint8_t *data,uint16_t len);
static inline QS808_Status QS808_Read_State(void);


__IO QS808_Status qs808_state=QS808_STATE_IDLE;

__weak uint8_t menutask(uint8_t state,uint8_t change)
{
	if(change)
	{
		switch(state)
		{
			case 0:
			case 2:
			case 4:	
				display.clear(false);
				display.format(0, 0, 2, 500, "Collect image,%d",state);
				break;
			case 1:
			case 3:
			case 5:
				display.clear(false);
				display.format(0, 0, 2, 500, "Move fingers,%d",state);
				break;
			case 6:
				display.clear(false);
				display.format(0, 0, 2, "Synthesis template,%d",state);
				break;
			case 7:
				display.clear(false);
				display.format(0, 0, 2, "Empty template,%d",state);
				break;
			case 8:
				display.clear(false);
				display.format(0, 0, 2, "write template,%d",state);
				break;
		}
	}
	return 0;
}
void FingerPrint::Test(void)
{
	int8_t ret;
	uint16_t tmp_num=0;
	
	delay_ms(2000);
	
	ret=Delete_Fingerprint(NULL,1,100);
	if(ret==0)
	{
		display.clear(false);
		display.format(0, 0, 2, 2000, "ɾ���ɹ�");
	}
	else
	{
		display.clear(false);
		display.format(0, 0, 2, 2000, "ɾ��ʧ��");
	}
	
	while(0)
	{
		display.clear(false);
		display.format(0, 0, 2, 2000, "fingerprint");
		
		
		ret=Fingerprint_Entry(menutask,(uint16_t *)&tmp_num);
		if(ret==0)
		{
			display.clear(false);
			display.format(0, 0, 2, 2000, "д�� %d �ɹ�",tmp_num);
		}
		else
		{
			display.clear(false);
			display.format(0, 0, 2, 2000, "д�� %d ʧ��",(int8_t)0-ret);
		}
		display.clear(false);
		display.format(0, 0, 2, 2000, "��ʼУ��");
		
		ret=Verify_Fingerprint(NULL,(uint16_t *)&tmp_num);
		if(ret==0)
		{
			display.clear(false);
			display.format(0, 0, 2, 2000, "У��ɹ�");
		}
		else
		{
			display.clear(false);
			display.format(0, 0, 2, 2000, "У��ʧ��");
		}
	}
}

void FingerPrint::init(void)
{
	QS808_Init();
}
int8_t FingerPrint::add(uint16_t &id, const uint16_t timeout)
{
	return Fingerprint_Entry(menutask, &id);
}
int8_t FingerPrint::remove(const uint16_t id_start, const uint16_t id_end)
{
	int8_t ret;
	//return Delete_Fingerprint(NULL, id_start, id_end);
	ret=Delete_Fingerprint(NULL,id_start,id_end);
	if(ret!=0) ret=Delete_Fingerprint(NULL,id_start,id_end);
	return ret;
}
int8_t FingerPrint::verify(uint16_t &id, const uint16_t timeout)
{
	return Verify_Fingerprint(NULL, &id);
}
int8_t FingerPrint::poll(uint16_t &id)
{
	return Verify_Fingerprint(NULL, &id);
}
void FingerPrint::QS808_Init(void)
{
	USART_InitTypeDef  			 USART_InitStructure;
	GPIO_InitTypeDef  			 GPIO_InitStructure;
	DMA_InitTypeDef 				 DMA_InitStructure;
	EXTI_InitTypeDef   			 EXTI_InitStructure;
	NVIC_InitTypeDef  			 NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_TIM17, ENABLE);

	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
#if 1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_0);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_0);
#else	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	/* Connect EXTI0 Line to PA0 pin */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource15);

	/* Configure EXTI0 line */
	EXTI_InitStructure.EXTI_Line = EXTI_Line15;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTI0 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(QS808_USARTx,&USART_InitStructure);

	USART_DMACmd(QS808_USARTx,USART_DMAReq_Tx,ENABLE); 
	USART_DMACmd(QS808_USARTx,USART_DMAReq_Rx,ENABLE); 

	USART_ITConfig(QS808_USARTx, USART_IT_IDLE, ENABLE);
	/* USART IRQ Channel configuration */
	NVIC_InitStructure.NVIC_IRQChannel =USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Common DMA configuration */
	DMA_InitStructure.DMA_BufferSize =QS808_MAX_BUF_SIZE;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	/*DMA for QS808_USARTx rx*/
	DMA_InitStructure.DMA_MemoryBaseAddr=(uint32_t)qs808_rx_buf;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(QS808_USARTx->RDR);
	DMA_Init(QS808_DMA_RX, &DMA_InitStructure);

	/*DMA for QS808_USARTx tx*/
	DMA_InitStructure.DMA_MemoryBaseAddr=(uint32_t)qs808_tx_buf;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(QS808_USARTx->TDR);
	DMA_Init(QS808_DMA_TX, &DMA_InitStructure);

	/* DMA remap for QS808_USARTx Tx Rx */
	DMA_RemapConfig(DMA1, DMA1_CH2_USART1_TX); 
	DMA_RemapConfig(DMA1, DMA1_CH3_USART1_RX); 


	TIM_DeInit(QS808_TIMx);
	TIM_TimeBaseStructure.TIM_Prescaler = (SystemCoreClock/1000)-1; //1ms
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;     //��
	TIM_TimeBaseStructure.TIM_Period = QS808_CMD_TIMEOUT;           //���ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;                    //ʱ��Ԥ��Ƶ
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;                //�ظ���������

	TIM_TimeBaseInit(QS808_TIMx, &TIM_TimeBaseStructure);
	TIM_ClearFlag(QS808_TIMx,TIM_FLAG_Update);                       //�����־λ
	TIM_ITConfig(QS808_TIMx,TIM_IT_Update,ENABLE);                   //ʹ������ж�


	/* USART IRQ Channel configuration */
	NVIC_InitStructure.NVIC_IRQChannel =TIM17_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	DMA_Cmd(QS808_DMA_RX, ENABLE);
	USART_Cmd(QS808_USARTx,ENABLE);
}

static uint16_t QS808_Checksum(uint8_t *data,uint16_t len)
{
	uint16_t checksum=0;
	while(len--)
	{
		checksum+=*data;
		data++;
	}
	return checksum;
}

static inline void QS808_Send_Data(uint16_t len)
{
	DMA_SetCurrDataCounter(QS808_DMA_TX,len);
	DMA_Cmd(QS808_DMA_TX, ENABLE);
	QS808_TIMx->CNT=0;
	TIM_Cmd(QS808_TIMx,ENABLE);
}

inline QS808_Status QS808_Read_State(void)
{
	return qs808_state;
}


/*
������:QS808_Test_Connection
����:QS808���Ͳ�������״̬����
����:��
״̬����ֵ:�ɹ����� QS808_STATE_OK��ʧ�ܷ��� QS808_STATE_FAIL
*/
QS808_Status FingerPrint::QS808_Test_Connection(void)
{
	QS808_CMD_Type *cmd_buf=(QS808_CMD_Type *)qs808_tx_buf;
	if((qs808_state==QS808_STATE_IDLE)||(qs808_state==QS808_STATE_READY))
	{
		DMA_Cmd(QS808_DMA_TX, DISABLE);
		cmd_buf->prefix=CMD_TX_PREFIX;
		cmd_buf->sid=0;
		cmd_buf->did=0;
		cmd_buf->cmd=CMD_TEST_CONNECTION;
		cmd_buf->len=0;
		memset(cmd_buf->data,0,16);
		cmd_buf->cks=QS808_Checksum((uint8_t *)cmd_buf,sizeof(QS808_CMD_Type)-2);
		QS808_Send_Data(sizeof(QS808_CMD_Type));
		qs808_state=QS808_STATE_BUSY;
		return QS808_STATE_OK;
	}
	return QS808_STATE_FAIL;
}

/*
������:QS808_Set_Parameter
����:QS808���ò���
����type:0->ID
				 1->��ȫ�ȼ�
				 2->ָ���ظ����
				 3->������
				 4->ָ����ѧϰ
				 5->�ɼ�ָ�Ƴ�ʱʱ��
����val:	ID                ��ʾ���豸��š������� 1 ~ 255 
					Security_Level    ��ʾ��ȫ�ȼ���������ֵ��1~5 ��Ĭ��Ϊ��3
					Duplication_Check ָ���ظ����״̬��/�ء������� 0 �� 1
					Baudrate          1:9600bps, 2:19200bps, 3:38400bps, 4:57600bps, 5:115200bps 6:230400bps, 7:460800bps, 8:921600bps Ĭ��5
					Auto_Learn        ��ʾָ��ģ����ѧϰ״̬��/�ء������� 0 �� 1
					Fp TimeOut        ��ʾ�ɼ�ָ�Ƴ�ʱʱ�䣬������ֵ��1 ���� 60 ��
״̬����ֵ:�ɹ����� QS808_STATE_OK��ʧ�ܷ��� QS808_STATE_FAIL
*/
QS808_Status FingerPrint::QS808_Set_Parameter(QS808_Parameter type,uint8_t val)
{
	QS808_CMD_Type *cmd_buf=(QS808_CMD_Type *)qs808_tx_buf;
	if((qs808_state==QS808_STATE_IDLE)||(qs808_state==QS808_STATE_READY))
	{
		DMA_Cmd(QS808_DMA_TX, DISABLE);
		cmd_buf->prefix=CMD_TX_PREFIX;
		cmd_buf->sid=0;
		cmd_buf->did=0;
		cmd_buf->cmd=CMD_SET_PARAM;
		cmd_buf->len=5;
		cmd_buf->data[0]=type;
		cmd_buf->data[1]=val;
		memset(&cmd_buf->data[2],0,14);
		cmd_buf->cks=QS808_Checksum((uint8_t *)cmd_buf,sizeof(QS808_CMD_Type)-2);
		QS808_Send_Data(sizeof(QS808_CMD_Type));
		qs808_state=QS808_STATE_BUSY;
		return QS808_STATE_OK;
	}
	return QS808_STATE_FAIL;
}

/*
������:QS808_Get_Parameter
����:QS808��ȡ����
����:0->ID
		 1->��ȫ�ȼ�
		 2->ָ���ظ����
		 3->������
		 4->ָ����ѧϰ
		 5->�ɼ�ָ�Ƴ�ʱʱ��
״̬����: 1 byte
ID                ��ʾ���豸��š������� 1 ~ 255 
Security_Level    ��ʾ��ȫ�ȼ���������ֵ��1~5 ��Ĭ��Ϊ��3
Duplication_Check ָ���ظ����״̬��/�ء������� 0 �� 1
Baudrate          1:9600bps, 2:19200bps, 3:38400bps, 4:57600bps, 5:115200bps 6:230400bps, 7:460800bps, 8:921600bps Ĭ��5
Auto_Learn        ��ʾָ��ģ����ѧϰ״̬��/�ء������� 0 �� 1
Fp TimeOut        ��ʾ�ɼ�ָ�Ƴ�ʱʱ�䣬������ֵ��1 ���� 60 ��
*/
QS808_Status FingerPrint::QS808_Get_Parameter(QS808_Parameter type)
{
	QS808_CMD_Type *cmd_buf=(QS808_CMD_Type *)qs808_tx_buf;
	if((qs808_state==QS808_STATE_IDLE)||(qs808_state==QS808_STATE_READY))
	{
		DMA_Cmd(QS808_DMA_TX, DISABLE);
		cmd_buf->prefix=CMD_TX_PREFIX;
		cmd_buf->sid=0;
		cmd_buf->did=0;
		cmd_buf->cmd=CMD_GET_PARAM;
		cmd_buf->len=1;
		cmd_buf->data[0]=type;
		memset(&cmd_buf->data[cmd_buf->len],0,16-cmd_buf->len);
		cmd_buf->cks=QS808_Checksum((uint8_t *)cmd_buf,sizeof(QS808_CMD_Type)-2);
		QS808_Send_Data(sizeof(QS808_CMD_Type));
		qs808_state=QS808_STATE_BUSY;
		return QS808_STATE_OK;
	}
	return QS808_STATE_FAIL;
}

///*
//������:QS808_Get_Device_Info
//����:QS808��ȡ�豸��Ϣ
//����ֵ:
//*/
//QS808_Status FingerPrint::QS808_Get_Device_Info(void)
//{
//	QS808_CMD_Type *cmd_buf=(QS808_CMD_Type *)qs808_tx_buf;
//	if((qs808_state==QS808_STATE_IDLE)||(qs808_state==QS808_STATE_READY))
//	{
//		DMA_Cmd(QS808_DMA_TX, DISABLE);
//		cmd_buf->prefix=CMD_TX_PREFIX;
//		cmd_buf->sid=0;
//		cmd_buf->did=0;
//		cmd_buf->cmd=CMD_GET_DEVICE_INFO;
//		cmd_buf->len=0;
//		memset(cmd_buf->data,0,16);
//		cmd_buf->cks=QS808_Checksum((uint8_t *)cmd_buf,sizeof(QS808_CMD_Type)-2);
//		QS808_Send_Data(sizeof(QS808_CMD_Type));
//		qs808_state=QS808_STATE_BUSY;
//		return QS808_STATE_OK;
//	}
//	return QS808_STATE_FAIL;
//}

/*
������:QS808_Get_Image
����:QS808��ȡͼ��
״̬����ֵ:�ɹ����� QS808_STATE_OK��ʧ�ܷ��� QS808_STATE_FAIL
*/
QS808_Status FingerPrint::QS808_Get_Image(void)
{
	QS808_CMD_Type *cmd_buf=(QS808_CMD_Type *)qs808_tx_buf;
	if((qs808_state==QS808_STATE_IDLE)||(qs808_state==QS808_STATE_READY))
	{
		DMA_Cmd(QS808_DMA_TX, DISABLE);
		cmd_buf->prefix=CMD_TX_PREFIX;
		cmd_buf->sid=0;
		cmd_buf->did=0;
		cmd_buf->cmd=CMD_GET_IMAGE;
		cmd_buf->len=0;
		memset(cmd_buf->data,0,16);
		cmd_buf->cks=QS808_Checksum((uint8_t *)cmd_buf,sizeof(QS808_CMD_Type)-2);
		QS808_Send_Data(sizeof(QS808_CMD_Type));
		qs808_state=QS808_STATE_BUSY;
		return QS808_STATE_OK;
	}
	return QS808_STATE_FAIL;
}

/*
������:QS808_Finger_Detect
����:QS808�����ָ
״̬����ֵ:��ָ�����뷵�� QS808_STATE_OK��û��ָ�����뷵�� QS808_STATE_FAIL
*/
QS808_Status FingerPrint::QS808_Finger_Detect(void)
{
	QS808_CMD_Type *cmd_buf=(QS808_CMD_Type *)qs808_tx_buf;
	if((qs808_state==QS808_STATE_IDLE)||(qs808_state==QS808_STATE_READY))
	{
		DMA_Cmd(QS808_DMA_TX, DISABLE);
		cmd_buf->prefix=CMD_TX_PREFIX;
		cmd_buf->sid=0;
		cmd_buf->did=0;
		cmd_buf->cmd=CMD_FINGER_DETECT;
		cmd_buf->len=0;
		memset(cmd_buf->data,0,16);
		cmd_buf->cks=QS808_Checksum((uint8_t *)cmd_buf,sizeof(QS808_CMD_Type)-2);
		QS808_Send_Data(sizeof(QS808_CMD_Type));
		qs808_state=QS808_STATE_BUSY;
		return QS808_STATE_OK;
	}
	return QS808_STATE_FAIL;
}


/*
������:QS808_Store_Char
����:QS808 ����ָ��ģ�����ݵ�ģ��ָ�ƿ�
״̬����ֵ:�ɹ����� QS808_STATE_OK��ʧ�ܷ��� QS808_STATE_FAIL
*/
QS808_Status FingerPrint::QS808_Store_Char(uint16_t tmp_num,uint16_t ram_num)
{
	QS808_CMD_Type *cmd_buf=(QS808_CMD_Type *)qs808_tx_buf;
	if((qs808_state==QS808_STATE_IDLE)||(qs808_state==QS808_STATE_READY))
	{
		DMA_Cmd(QS808_DMA_TX, DISABLE);
		cmd_buf->prefix=CMD_TX_PREFIX;
		cmd_buf->sid=0;
		cmd_buf->did=0;
		cmd_buf->cmd=CMD_STORE_CHAR;
		cmd_buf->len=4;
		cmd_buf->data[0]=tmp_num;
		cmd_buf->data[1]=tmp_num>>8;
		cmd_buf->data[2]=ram_num;
		cmd_buf->data[3]=ram_num>>8;
		memset(&cmd_buf->data[4],0,12);
		cmd_buf->cks=QS808_Checksum((uint8_t *)cmd_buf,sizeof(QS808_CMD_Type)-2);
		QS808_Send_Data(sizeof(QS808_CMD_Type));
		qs808_state=QS808_STATE_BUSY;
		return QS808_STATE_OK;
	}
	return QS808_STATE_FAIL;
}

/*
������:QS808_Load_Char
����:��ȡָ�ƿ��е�ָ�Ʋ��ݴ���ָ����RamBuffer ��
״̬����ֵ:�ɹ����� QS808_STATE_OK��ʧ�ܷ��� QS808_STATE_FAIL
*/
QS808_Status FingerPrint::QS808_Load_Char(uint16_t tmp_num,uint16_t ram_num)
{
	QS808_CMD_Type *cmd_buf=(QS808_CMD_Type *)qs808_tx_buf;
	if((qs808_state==QS808_STATE_IDLE)||(qs808_state==QS808_STATE_READY))
	{
		DMA_Cmd(QS808_DMA_TX, DISABLE);
		cmd_buf->prefix=CMD_TX_PREFIX;
		cmd_buf->sid=0;
		cmd_buf->did=0;
		cmd_buf->cmd=CMD_LOAD_CHAR;
		cmd_buf->len=4;
		cmd_buf->data[0]=tmp_num;
		cmd_buf->data[1]=tmp_num>>8;
		cmd_buf->data[2]=ram_num;
		cmd_buf->data[3]=ram_num>>8;
		memset(&cmd_buf->data[4],0,12);
		cmd_buf->cks=QS808_Checksum((uint8_t *)cmd_buf,sizeof(QS808_CMD_Type)-2);
		QS808_Send_Data(sizeof(QS808_CMD_Type));
		qs808_state=QS808_STATE_BUSY;
		return QS808_STATE_OK;
	}
	return QS808_STATE_FAIL;
}


/*
������:QS808_Del_Char
����:QS808ɾ��ָ����ŷ�Χ�ڵ�ָ��
״̬����ֵ:�ɹ����� QS808_STATE_OK��ʧ�ܷ��� QS808_STATE_FAIL
*/
QS808_Status FingerPrint::QS808_Del_Char(uint16_t tmp_start,uint16_t tmp_end)
{
	QS808_CMD_Type *cmd_buf=(QS808_CMD_Type *)qs808_tx_buf;
	if((qs808_state==QS808_STATE_IDLE)||(qs808_state==QS808_STATE_READY))
	{
		DMA_Cmd(QS808_DMA_TX, DISABLE);
		cmd_buf->prefix=CMD_TX_PREFIX;
		cmd_buf->sid=0;
		cmd_buf->did=0;
		cmd_buf->cmd=CMD_DEL_CHAR;
		cmd_buf->len=4;
		cmd_buf->data[0]=tmp_start;
		cmd_buf->data[1]=tmp_start>>8;
		cmd_buf->data[2]=tmp_end;
		cmd_buf->data[3]=tmp_end>>8;
		memset(&cmd_buf->data[4],0,12);
		cmd_buf->cks=QS808_Checksum((uint8_t *)cmd_buf,sizeof(QS808_CMD_Type)-2);
		QS808_Send_Data(sizeof(QS808_CMD_Type));
		qs808_state=QS808_STATE_BUSY;
		return QS808_STATE_OK;
	}
	return QS808_STATE_FAIL;
}

/*
������:QS808_Get_Empty_Id
����:QS808��ȡָ����ŷ�Χ�ڿ�ע����׸���� 
״̬���ز���:2 byte ����ע���ģ���
*/
QS808_Status FingerPrint::QS808_Get_Empty_Id(uint16_t tmp_start,uint16_t tmp_end)
{
	QS808_CMD_Type *cmd_buf=(QS808_CMD_Type *)qs808_tx_buf;
	if((qs808_state==QS808_STATE_IDLE)||(qs808_state==QS808_STATE_READY))
	{
		DMA_Cmd(QS808_DMA_TX, DISABLE);
		cmd_buf->prefix=CMD_TX_PREFIX;
		cmd_buf->sid=0;
		cmd_buf->did=0;
		cmd_buf->cmd=CMD_GET_EMPTY_ID;
		cmd_buf->len=4;
		cmd_buf->data[0]=tmp_start;
		cmd_buf->data[1]=tmp_start>>8;
		cmd_buf->data[2]=tmp_end;
		cmd_buf->data[3]=tmp_end>>8;
		memset(&cmd_buf->data[4],0,12);
		cmd_buf->cks=QS808_Checksum((uint8_t *)cmd_buf,sizeof(QS808_CMD_Type)-2);
		QS808_Send_Data(sizeof(QS808_CMD_Type));
		qs808_state=QS808_STATE_BUSY;
		return QS808_STATE_OK;
	}
	return QS808_STATE_FAIL;
}



/*
������:QS808_Get_Status
����:QS808 ���ָ���ı���Ƿ��ѱ�ע��
״̬���ز���: �Ѿ�ע��->QS808_STATE_OK��û��ע��->QS808_STATE_FAIL
*/
QS808_Status FingerPrint::QS808_Get_Status(uint16_t tmp_num)
{
	QS808_CMD_Type *cmd_buf=(QS808_CMD_Type *)qs808_tx_buf;
	if((qs808_state==QS808_STATE_IDLE)||(qs808_state==QS808_STATE_READY))
	{
		DMA_Cmd(QS808_DMA_TX, DISABLE);
		cmd_buf->prefix=CMD_TX_PREFIX;
		cmd_buf->sid=0;
		cmd_buf->did=0;
		cmd_buf->cmd=CMD_GET_STATUS;
		cmd_buf->len=2;
		cmd_buf->data[0]=tmp_num;
		cmd_buf->data[1]=tmp_num>>8;
		memset(&cmd_buf->data[2],0,14);
		cmd_buf->cks=QS808_Checksum((uint8_t *)cmd_buf,sizeof(QS808_CMD_Type)-2);
		QS808_Send_Data(sizeof(QS808_CMD_Type));
		qs808_state=QS808_STATE_BUSY;
		return QS808_STATE_OK;
	}
	return QS808_STATE_FAIL;
}


/*
������:QS808_Get_Broken_Id
����:QS808 ���ָ����ŷ�Χ�ڵ�ָ�ƿ��Ƿ���������
����ֵ:�ɹ�����0��ʧ�ܷ���-1
*/
QS808_Status FingerPrint::QS808_Get_Broken_Id(uint16_t tmp_start,uint16_t tmp_end)
{
	QS808_CMD_Type *cmd_buf=(QS808_CMD_Type *)qs808_tx_buf;
	if((qs808_state==QS808_STATE_IDLE)||(qs808_state==QS808_STATE_READY))
	{
		DMA_Cmd(QS808_DMA_TX, DISABLE);
		cmd_buf->prefix=CMD_TX_PREFIX;
		cmd_buf->sid=0;
		cmd_buf->did=0;
		cmd_buf->cmd=CMD_GET_BROKEN_ID;
		cmd_buf->len=4;
		cmd_buf->data[0]=tmp_start;
		cmd_buf->data[1]=tmp_start>>8;
		cmd_buf->data[2]=tmp_end;
		cmd_buf->data[3]=tmp_end>>8;
		memset(&cmd_buf->data[4],0,12);
		cmd_buf->cks=QS808_Checksum((uint8_t *)cmd_buf,sizeof(QS808_CMD_Type)-2);
		QS808_Send_Data(sizeof(QS808_CMD_Type));
		qs808_state=QS808_STATE_BUSY;
		return QS808_STATE_OK;
	}
	return QS808_STATE_FAIL;
}


/*
������:QS808_Get_Enroll_Count
����:QS808 ��ȡָ����ŷ�Χ����ע���ָ����
����ֵ:�ɹ�����0��ʧ�ܷ���-1
*/
QS808_Status FingerPrint::QS808_Get_Enroll_Count(uint16_t tmp_start,uint16_t tmp_end)
{
	QS808_CMD_Type *cmd_buf=(QS808_CMD_Type *)qs808_tx_buf;
	if((qs808_state==QS808_STATE_IDLE)||(qs808_state==QS808_STATE_READY))
	{
		DMA_Cmd(QS808_DMA_TX, DISABLE);
		cmd_buf->prefix=CMD_TX_PREFIX;
		cmd_buf->sid=0;
		cmd_buf->did=0;
		cmd_buf->cmd=CMD_GET_ENROLL_COUNT;
		cmd_buf->len=4;
		cmd_buf->data[0]=tmp_start;
		cmd_buf->data[1]=tmp_start>>8;
		cmd_buf->data[2]=tmp_end;
		cmd_buf->data[3]=tmp_end>>8;
		memset(&cmd_buf->data[4],0,12);
		cmd_buf->cks=QS808_Checksum((uint8_t *)cmd_buf,sizeof(QS808_CMD_Type)-2);
		QS808_Send_Data(sizeof(QS808_CMD_Type));
		qs808_state=QS808_STATE_BUSY;
		return QS808_STATE_OK;
	}
	return QS808_STATE_FAIL;
}


/*
������:QS808_Generate
����:QS808 ���ݴ��� ImageBuffer �е�ָ��ͼ�����ģ��,������Ϊ ram_num ��Ram Buffer
����:Ram Buffer ���
����ֵ:�ɹ�����0��ʧ�ܷ���-1
*/
QS808_Status FingerPrint::QS808_Generate(uint16_t ram_num)
{
	QS808_CMD_Type *cmd_buf=(QS808_CMD_Type *)qs808_tx_buf;
	if((qs808_state==QS808_STATE_IDLE)||(qs808_state==QS808_STATE_READY))
	{
		DMA_Cmd(QS808_DMA_TX, DISABLE);
		cmd_buf->prefix=CMD_TX_PREFIX;
		cmd_buf->sid=0;
		cmd_buf->did=0;
		cmd_buf->cmd=CMD_GENERATE;
		cmd_buf->len=2;
		cmd_buf->data[0]=ram_num;
		cmd_buf->data[1]=ram_num>>8;
		memset(&cmd_buf->data[2],0,14);
		cmd_buf->cks=QS808_Checksum((uint8_t *)cmd_buf,sizeof(QS808_CMD_Type)-2);
		QS808_Send_Data(sizeof(QS808_CMD_Type));
		qs808_state=QS808_STATE_BUSY;
		return QS808_STATE_OK;
	}
	return QS808_STATE_FAIL;
}


/*
������:QS808_Merge
����:QS808 �ϳ�ָ��ģ�������������
����ram_num->�ϳɺ����ram Buffer�ı��
����count->2(�ϳ�ram Buffer 0��1) ��3(�ϳ�ram Buffer 0��1��2)

����ֵ:�ɹ�����0��ʧ�ܷ���-1
*/
QS808_Status FingerPrint::QS808_Merge(uint16_t ram_num,uint8_t count)
{
	QS808_CMD_Type *cmd_buf=(QS808_CMD_Type *)qs808_tx_buf;
	if((qs808_state==QS808_STATE_IDLE)||(qs808_state==QS808_STATE_READY))
	{
		DMA_Cmd(QS808_DMA_TX, DISABLE);
		cmd_buf->prefix=CMD_TX_PREFIX;
		cmd_buf->sid=0;
		cmd_buf->did=0;
		cmd_buf->cmd=CMD_MERGE;
		cmd_buf->len=3;
		cmd_buf->data[0]=ram_num;
		cmd_buf->data[1]=ram_num>>8;
		cmd_buf->data[2]=count;
		memset(&cmd_buf->data[3],0,13);
		cmd_buf->cks=QS808_Checksum((uint8_t *)cmd_buf,sizeof(QS808_CMD_Type)-2);
		QS808_Send_Data(sizeof(QS808_CMD_Type));
		qs808_state=QS808_STATE_BUSY;
		return QS808_STATE_OK;
	}
	return QS808_STATE_FAIL;
}



/*
������:QS808_Match
����:QS808 ָ��2��RamBuffer ֮���ģ�����ȶ� 
����ֵ:�ɹ�����0��ʧ�ܷ���-1
*/
QS808_Status FingerPrint::QS808_Match(uint16_t ram_num1,uint16_t ram_num2)
{
	QS808_CMD_Type *cmd_buf=(QS808_CMD_Type *)qs808_tx_buf;
	if((qs808_state==QS808_STATE_IDLE)||(qs808_state==QS808_STATE_READY))
	{
		DMA_Cmd(QS808_DMA_TX, DISABLE);
		cmd_buf->prefix=CMD_TX_PREFIX;
		cmd_buf->sid=0;
		cmd_buf->did=0;
		cmd_buf->cmd=CMD_MATCH;
		cmd_buf->len=4;
		cmd_buf->data[0]=ram_num1;
		cmd_buf->data[1]=ram_num1>>8;
		cmd_buf->data[2]=ram_num2;
		cmd_buf->data[3]=ram_num2>>8;
		memset(&cmd_buf->data[4],0,12);
		cmd_buf->cks=QS808_Checksum((uint8_t *)cmd_buf,sizeof(QS808_CMD_Type)-2);
		QS808_Send_Data(sizeof(QS808_CMD_Type));
		qs808_state=QS808_STATE_BUSY;
		return QS808_STATE_OK;
	}
	return QS808_STATE_FAIL;
}


/*
������:QS808_Search
����:QS808 ָ����ŷ�Χ��1:Nʶ��
����ֵ:�ɹ�����0��ʧ�ܷ���-1
*/
QS808_Status FingerPrint::QS808_Search(uint16_t ram_num,uint16_t tmp_start,uint16_t tmp_end)
{
	QS808_CMD_Type *cmd_buf=(QS808_CMD_Type *)qs808_tx_buf;
	if((qs808_state==QS808_STATE_IDLE)||(qs808_state==QS808_STATE_READY))
	{
		DMA_Cmd(QS808_DMA_TX, DISABLE);
		cmd_buf->prefix=CMD_TX_PREFIX;
		cmd_buf->sid=0;
		cmd_buf->did=0;
		cmd_buf->cmd=CMD_SEARCH;
		cmd_buf->len=6;
		cmd_buf->data[0]=ram_num;
		cmd_buf->data[1]=ram_num>>8;
		cmd_buf->data[2]=tmp_start;
		cmd_buf->data[3]=tmp_start>>8;
		cmd_buf->data[4]=tmp_end;
		cmd_buf->data[5]=tmp_end>>8;
		
		memset(&cmd_buf->data[6],0,10);
		cmd_buf->cks=QS808_Checksum((uint8_t *)cmd_buf,sizeof(QS808_CMD_Type)-2);
		QS808_Send_Data(sizeof(QS808_CMD_Type));
		qs808_state=QS808_STATE_BUSY;
		return QS808_STATE_OK;
	}
	return QS808_STATE_FAIL;
}

/*
������:QS808_Verify
����:QS808 ָ��RamBuffer ��ָ�ƿ���ָ����ŵ�ģ��ȶ�
����ֵ:�ɹ�����0��ʧ�ܷ���-1
*/
QS808_Status FingerPrint::QS808_Verify(uint16_t tmp,uint16_t ram_num)
{
	QS808_CMD_Type *cmd_buf=(QS808_CMD_Type *)qs808_tx_buf;
	if((qs808_state==QS808_STATE_IDLE)||(qs808_state==QS808_STATE_READY))
	{
		DMA_Cmd(QS808_DMA_TX, DISABLE);
		cmd_buf->prefix=CMD_TX_PREFIX;
		cmd_buf->sid=0;
		cmd_buf->did=0;
		cmd_buf->cmd=CMD_VERIFY;
		cmd_buf->len=4;
		cmd_buf->data[0]=tmp;
		cmd_buf->data[1]=tmp>>8;
		cmd_buf->data[2]=ram_num;
		cmd_buf->data[3]=ram_num>>8;
		
		memset(&cmd_buf->data[4],0,12);
		cmd_buf->cks=QS808_Checksum((uint8_t *)cmd_buf,sizeof(QS808_CMD_Type)-2);
		QS808_Send_Data(sizeof(QS808_CMD_Type));
		qs808_state=QS808_STATE_BUSY;
		return QS808_STATE_OK;
	}
	return QS808_STATE_FAIL;
}





/*
������:QS808_Cmd_Response
����:QS808����ؽ��
����:��
����ֵ:������������0������ʧ�ܷ���-1
*/


QS808_Status FingerPrint::QS808_Cmd_Response(uint8_t *parameter)
{
	QS808_CMD_Reply_Type  *cmd_rx=(QS808_CMD_Reply_Type *)qs808_rx_buf;
	//QS808_DATA_Reply_Type *data_rx=(QS808_DATA_Reply_Type *)qs808_rx_buf;
	if(qs808_state==QS808_STATE_READY)
	{
		switch(cmd_rx->rcm)
		{
			case CMD_TEST_CONNECTION:
			case CMD_SET_PARAM:
			case CMD_GET_IMAGE:	
			case CMD_DEL_CHAR:	
			case CMD_GENERATE:	
			case CMD_MERGE:
			case CMD_MATCH:
				if((qs808_state==QS808_STATE_READY)&&(cmd_rx->len==2)&&(cmd_rx->ret==0x00))
				{
					qs808_state=QS808_STATE_IDLE;
					return QS808_STATE_OK;
				}
				qs808_state=QS808_STATE_IDLE;
				return QS808_STATE_FAIL;
			case CMD_GET_PARAM:
				if(parameter!=NULL)
				{
					if((qs808_state==QS808_STATE_READY)&&(cmd_rx->len==6))
					{
						parameter[0]=cmd_rx->data[0];
						qs808_state=QS808_STATE_IDLE;
						return QS808_STATE_OK;
					}
					qs808_state=QS808_STATE_IDLE;
					return QS808_STATE_FAIL;
				}
				qs808_state=QS808_STATE_IDLE;
				return QS808_STATE_PARAM;
//			case CMD_GET_DEVICE_INFO:
//				if(parameter!=NULL)
//				{
//					if(data_rx->ret==0x00)
//					{
//						memcpy(parameter,data_rx->data,data_rx->len);
//						qs808_state=QS808_STATE_IDLE;
//						return QS808_STATE_OK;
//					}
//					qs808_state=QS808_STATE_IDLE;
//					return QS808_STATE_FAIL;
//				}
//				qs808_state=QS808_STATE_IDLE;
//				return QS808_STATE_PARAM;
			case CMD_FINGER_DETECT:	
			case CMD_GET_STATUS:
				if((qs808_state==QS808_STATE_READY)&&(cmd_rx->len==3)&&(cmd_rx->data[0]))
				{
					qs808_state=QS808_STATE_IDLE;
					return QS808_STATE_OK;
				}
				qs808_state=QS808_STATE_IDLE;
				return QS808_STATE_FAIL;	
			case CMD_STORE_CHAR:
			case CMD_LOAD_CHAR:
				if((qs808_state==QS808_STATE_READY)&&(cmd_rx->ret==0x00))
				{
					qs808_state=QS808_STATE_IDLE;
					return QS808_STATE_OK;
				}
				qs808_state=QS808_STATE_IDLE;
				return QS808_STATE_FAIL;
			case CMD_GET_EMPTY_ID:
				if(parameter!=NULL)
				{
					if((qs808_state==QS808_STATE_READY)&&(cmd_rx->len==4))
					{
						parameter[0]=cmd_rx->data[0];
						parameter[1]=cmd_rx->data[1];
						qs808_state=QS808_STATE_IDLE;
						return QS808_STATE_OK;
					}
					qs808_state=QS808_STATE_IDLE;
					return QS808_STATE_FAIL;
				}
				qs808_state=QS808_STATE_IDLE;
				return QS808_STATE_PARAM;
			case CMD_GET_BROKEN_ID:	
				if(parameter!=NULL)
				{
					if((qs808_state==QS808_STATE_READY)&&(cmd_rx->len==6)&&(cmd_rx->ret==0x00))
					{
						parameter[0]=cmd_rx->data[0];//��������Ŀ
						parameter[1]=cmd_rx->data[1];
						parameter[2]=cmd_rx->data[2];//��һ��������
						parameter[3]=cmd_rx->data[3];
						qs808_state=QS808_STATE_IDLE;
						return QS808_STATE_OK;
					}
					qs808_state=QS808_STATE_IDLE;
					return QS808_STATE_FAIL;
				}
				qs808_state=QS808_STATE_IDLE;
				return QS808_STATE_PARAM;
			case CMD_GET_ENROLL_COUNT:
				if(parameter!=NULL)
				{
					if((qs808_state==QS808_STATE_READY)&&(cmd_rx->len==4))
					{
						parameter[0]=cmd_rx->data[0];//�Ѿ�ע���ģ�����
						parameter[1]=cmd_rx->data[1];
						qs808_state=QS808_STATE_IDLE;
						return QS808_STATE_OK;
					}
					qs808_state=QS808_STATE_IDLE;
					return QS808_STATE_FAIL;
				}
				qs808_state=QS808_STATE_IDLE;
				return QS808_STATE_PARAM;
			case CMD_SEARCH:
			case CMD_VERIFY:	
				if(parameter!=NULL)
				{
					if((qs808_state==QS808_STATE_READY)&&(cmd_rx->len==5))
					{
						parameter[0]=cmd_rx->data[0];//ģ����
						parameter[1]=cmd_rx->data[1];
						parameter[2]=cmd_rx->data[2];//���ܸ��½��
						qs808_state=QS808_STATE_IDLE;
						return QS808_STATE_OK;
					}
					qs808_state=QS808_STATE_IDLE;
					return QS808_STATE_FAIL;
				}
				qs808_state=QS808_STATE_IDLE;
				return QS808_STATE_PARAM;
				
			default:
				qs808_state=QS808_STATE_IDLE;
				return QS808_STATE_OK;		
		}
	}
	else if(qs808_state==QS808_STATE_TIMEOUT)
	{
		qs808_state=QS808_STATE_IDLE;
		return QS808_STATE_TIMEOUT;
	}
	else
	{
		return qs808_state;
	}
}
/*
������:Fingerprint_Entry
����:¼��ָ��
����:menutask->�˵���ʾ����
     etmp_num->¼���ģ���
����ֵ:
0->�ɹ�
-1->�û�ȡ��
-2->ģ������ʧ��
-3->��ȡ�յ�ģ��ID��ʧ��
-4->д��ģ���ʧ��
-5->����ram Bufferʧ��
*/
int8_t FingerPrint::Fingerprint_Entry(uint8_t (*menutask)(uint8_t state,uint8_t change),uint16_t *etmp_num)
{
	uint8_t       finger_state=0;
	/*
		finger_state=0 �ɼ���һ��ͼ��ʧ��
		finger_state=1 ��ָ�뿪
		finger_state=2 �ɼ��ڶ���ͼ��ʧ��
		finger_state=3 ��ָ�뿪
		finger_state=4 �ɼ�������ͼ��ʧ��
		finger_state=5 ��ָ�뿪
		finger_state=6 �ϳ�ģ��
		finger_state=7 ��ȡ�յ�ģ����
		finger_state=8 д��ģ���
	*/
	QS808_Status  state_ret;
	__IO uint8_t       i;
	for(i=0;i<3;)//��ȡ����ͼ��
	{
		while(1)
		{
			QS808_Get_Image();
			while(1)
			{
				state_ret=QS808_Read_State();
				if((state_ret==QS808_STATE_TIMEOUT)||(state_ret==QS808_STATE_READY))
				{
					break;
				}
				if(menutask(finger_state,0)){return -1;}
			}
			if(QS808_Cmd_Response(NULL)==QS808_STATE_OK)
			{
				break;
			}
		}
		QS808_Generate(i);
		while(1)
		{
			state_ret=QS808_Read_State();
			if((state_ret==QS808_STATE_TIMEOUT)||(state_ret==QS808_STATE_READY))
			{
				break;
			}
			if(menutask(finger_state,0)){return -1;}
		}
		if(QS808_Cmd_Response(NULL)!=QS808_STATE_OK)
		{
			continue;
		}
		i++;
		finger_state++;
		if(menutask(finger_state,1)){return -1;}
		while(1)
		{
			QS808_Finger_Detect();//�����ָ
			while(1)
			{
				state_ret=QS808_Read_State();
				if((state_ret==QS808_STATE_TIMEOUT)||(state_ret==QS808_STATE_READY))
				{
					break;
				}
				if(menutask(finger_state,0)){return -1;}
			}
			if(QS808_Cmd_Response(NULL)==QS808_STATE_FAIL)
			{
				break;
			}
		}
		finger_state++;
		if(menutask(finger_state,1)){return -1;}
	}
	if(menutask(finger_state,1)){return -1;}
	while(1)
	{
		QS808_Merge(0,3);//�ϳ�ģ��
		while(1)
		{
			state_ret=QS808_Read_State();
			if((state_ret==QS808_STATE_TIMEOUT)||(state_ret==QS808_STATE_READY))
			{
				break;
			}
			if(menutask(finger_state,0)){return -1;}
		}
		if(QS808_Cmd_Response(NULL)==QS808_STATE_OK)
		{
			break;
		}
		else
		{
			return -2;//ģ������ʧ��
		}
	}
	finger_state++;
	if(menutask(finger_state,1)){return -1;}
	while(1)
	{
		QS808_Get_Empty_Id(1,100);//��ȡ�յ�ģ���
		while(1)
		{
			state_ret=QS808_Read_State();
			if((state_ret==QS808_STATE_TIMEOUT)||(state_ret==QS808_STATE_READY))
			{
				break;
			}
			if(menutask(finger_state,0)){return -1;}
		}
		if(QS808_Cmd_Response((uint8_t *)etmp_num)==QS808_STATE_OK)
		{
			break;
		}
		else
		{
			return -3;//��ȡ�յ�ģ��ID��ʧ��
		}
	}
	finger_state++;
	if(menutask(finger_state,1)){return -1;}
	while(1)
	{
		QS808_Store_Char(*etmp_num,0);
		while(1)
		{
			state_ret=QS808_Read_State();
			if((state_ret==QS808_STATE_TIMEOUT)||(state_ret==QS808_STATE_READY))
			{
				break;
			}
			if(menutask(finger_state,0)){return -1;}
		}
		if(QS808_Cmd_Response((uint8_t *)&etmp_num)==QS808_STATE_OK)
		{
			return 0;
		}
		else
		{
			return -4;//д��ģ���ʧ��
		}
	}
}

/*
������:Delete_Fingerprint
����:ɾ��ָ��
����:menutask->�˵���ʾ����
     tmp_start->��ʼģ���
		 tmp_end->����ģ���
*/
int8_t FingerPrint::Delete_Fingerprint(uint8_t (*menutask)(void),uint16_t tmp_start,uint16_t tmp_end)
{
	QS808_Status  state_ret;
	QS808_Del_Char(tmp_start,tmp_end);
	while(1)
	{
		state_ret=QS808_Read_State();
		if((state_ret==QS808_STATE_TIMEOUT)||(state_ret==QS808_STATE_READY))
		{
			break;
		}
		if(menutask!=NULL)
		{
			if(menutask()){return -1;}
		}
	}
	if(QS808_Cmd_Response(NULL)==QS808_STATE_OK)
	{
		return 0;
	}
	else
	{
		return -1;//ɾ��ʧ��
	}
}



/*
������:Verify_Fingerprint
����:��ָ֤��
����:menutask->�˵���ʾ����
     tmp_num
*/
int8_t FingerPrint::Verify_Fingerprint(uint8_t (*menutask)(void),uint16_t *tmp_num)
{
	QS808_Status  state_ret;
	uint8_t tmp_buf[3];
	while(1)
	{
		while(1)
		{
			QS808_Get_Image();
			while(1)
			{
				state_ret=QS808_Read_State();
				if((state_ret==QS808_STATE_TIMEOUT)||(state_ret==QS808_STATE_READY))
				{
					break;
				}
				if(menutask!=NULL)
				{
					if(menutask()){return -1;}
				}
			}
			if(QS808_Cmd_Response(NULL)==QS808_STATE_OK)
			{
				break;
			}
		}
		QS808_Generate(0);
		while(1)
		{
			state_ret=QS808_Read_State();
			if((state_ret==QS808_STATE_TIMEOUT)||(state_ret==QS808_STATE_READY))
			{
				break;
			}
			if(menutask!=NULL)
			{
				if(menutask()){return -1;}
			}
		}
		if(QS808_Cmd_Response(NULL)==QS808_STATE_OK)
		{
			break;
		}
	}	
	QS808_Search(0,1,100);
	while(1)
	{
		state_ret=QS808_Read_State();
		if((state_ret==QS808_STATE_TIMEOUT)||(state_ret==QS808_STATE_READY))
		{
			break;
		}
		if(menutask!=NULL)
		{
			if(menutask()){return -1;}
		}
	}
	if(QS808_Cmd_Response(tmp_buf)==QS808_STATE_OK)
	{
		*tmp_num=(tmp_buf[1]<<8)|tmp_buf[0];
		return 0;
	}
	else
	{
		return -1;//û���ҵ�
	}
}

extern "C" void USART1_IRQHandler(void);
extern "C" void TIM17_IRQHandler(void);
extern "C" void QS808_IRQ(void);

void USART1_IRQHandler(void)//�����յ�������
{
	uint16_t rx_len=0;
	uint16_t checksum[2];
	QS808_CMD_Type       *cmd_tx=(QS808_CMD_Type *)qs808_tx_buf;
	QS808_CMD_Reply_Type *cmd_rx=(QS808_CMD_Reply_Type *)qs808_rx_buf;
	
	if(USART_GetITStatus(QS808_USARTx,USART_IT_IDLE) != RESET)
	{
		DMA_Cmd(QS808_DMA_RX, DISABLE);
		rx_len=QS808_MAX_BUF_SIZE-QS808_DMA_RX->CNDTR;
		if(rx_len>=sizeof(QS808_CMD_Reply_Type))
		{
			checksum[0]=QS808_Checksum((uint8_t *)qs808_rx_buf,rx_len-2);
			checksum[1]=(qs808_rx_buf[rx_len-1]<<8)|qs808_rx_buf[rx_len-2];
			if((checksum[0]==checksum[1])&&(qs808_state==QS808_STATE_BUSY)&&(cmd_rx->prefix==CMD_RX_PREFIX)&&(cmd_tx->cmd==cmd_rx->rcm))//�յ����͵�����ظ�
			{
				qs808_state=QS808_STATE_READY;
			}
			else
			{
				__NOP();
			}
//			else if((qs808_state==QS808_STATE_IDLE)||(qs808_state==QS808_STATE_READY))//�յ�δ֪�Ļظ�
//			{
//				__NOP();
//			}
//			else
//			{
//				__NOP();
//			}
		}
		QS808_DMA_RX->CNDTR=QS808_MAX_BUF_SIZE;
		DMA_Cmd(QS808_DMA_RX, ENABLE);
		USART_ClearITPendingBit(QS808_USARTx,USART_IT_IDLE);
	}
}


//��ʱ�ж�
void TIM17_IRQHandler(void)
{
	if (TIM_GetITStatus(QS808_TIMx, TIM_IT_Update) != RESET)
	{
		TIM_Cmd(QS808_TIMx,DISABLE);
		qs808_state=QS808_STATE_TIMEOUT;
		TIM_ClearITPendingBit(QS808_TIMx, TIM_IT_Update);
	}
}

//�����ж� 
void QS808_IRQ(void)
{
	__NOP();
}



































