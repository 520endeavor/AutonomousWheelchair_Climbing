#include "chaoshengbo.h"
static void IIC_Start(void);
static void IIC_Stop(void);
static u8 IIC_Wait_Ack(void);
static void IIC_Send_Byte(u8 txd);
static u8 IIC_Read_Byte(unsigned char ack);
ChaoShengBoData ChaoShengBodata;
static void delay_us(uint32_t cnt)  
{  
    uint32_t i,j;  
    for(i=0;i<cnt;i++)  
    {  
        for(j=0;j<180;j++);  
    }  
}

u8 KS103_ReadOneByte(u8 address, u8 reg)
{
	u8 temp=0;
	IIC_Start();
	IIC_Send_Byte(address); //���͵͵�ַ
	IIC_Wait_Ack();
	IIC_Send_Byte(reg); //���͵͵�ַ
	IIC_Wait_Ack();
	IIC_Start();
	IIC_Send_Byte(address + 1); //�������ģʽ
	IIC_Wait_Ack();
	delay_us(50); //���Ӵ˴���ͨ�ųɹ�������
	temp=IIC_Read_Byte(0); //���Ĵ���3
	IIC_Stop();//����һ��ֹͣ����
	return temp;
}
void KS103_WriteOneByte(u8 address,u8 reg,u8 command)
{
	IIC_Start();
	IIC_Send_Byte(address); //����д����
	IIC_Wait_Ack();
	IIC_Send_Byte(reg);//���͸ߵ�ַ
	IIC_Wait_Ack();
	IIC_Send_Byte(command); //���͵͵�ַ
	IIC_Wait_Ack();
	IIC_Stop();//����һ��ֹͣ����
}
void ChaoShengBo_IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;  
    __HAL_RCC_GPIOG_CLK_ENABLE();   //ʹ��GPIOEʱ��
    //PE4,5��ʼ������
  /*Configure GPIO pins : PGPin PGPin PGPin */
  GPIO_InitStruct.Pin = I2C_SCL_Pin|I2C_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
    
    ChaoShengBo_IIC_SDA=1;
    ChaoShengBo_IIC_SCL=1; 


}
//����IIC ��ʼ�ź�
static void IIC_Start(void)
{
	ChaoShengBo_SDA_OUT(); //sda �����
	ChaoShengBo_IIC_SDA=1;
	ChaoShengBo_IIC_SCL=1;
	delay_us(10);
	ChaoShengBo_IIC_SDA=0;//START:when CLK is high,DATA change form high to low
	delay_us(10);
	ChaoShengBo_IIC_SCL=0;//ǯסI2C ���ߣ�׼�����ͻ��������
}
//����IIC ֹͣ�ź�
static void IIC_Stop(void)
{
	ChaoShengBo_SDA_OUT();//sda �����
	ChaoShengBo_IIC_SCL=0;
	ChaoShengBo_IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
	delay_us(10);
	ChaoShengBo_IIC_SCL=1;
	ChaoShengBo_IIC_SDA=1;//����I2C ���߽����ź�
	delay_us(10);
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
// 0������Ӧ��ɹ�
static u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	ChaoShengBo_SDA_IN(); //SDA ����Ϊ����
	ChaoShengBo_IIC_SDA=1;delay_us(6);
	ChaoShengBo_IIC_SCL=1;delay_us(6);
	while(ChaoShengBo_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	ChaoShengBo_IIC_SCL=0;//ʱ�����0
	return 0;
}
//����ACK Ӧ��
static void IIC_Ack(void)
{
	ChaoShengBo_IIC_SCL=0;
	ChaoShengBo_SDA_OUT();
	ChaoShengBo_IIC_SDA=0;
	delay_us(10);
	ChaoShengBo_IIC_SCL=1;
	delay_us(10);
	ChaoShengBo_IIC_SCL=0;
}
//������ACK Ӧ��
static void IIC_NAck(void)
{
	ChaoShengBo_IIC_SCL=0;
	ChaoShengBo_SDA_OUT();
	ChaoShengBo_IIC_SDA=1;
	delay_us(10);
	ChaoShengBo_IIC_SCL=1;
	delay_us(10);
	ChaoShengBo_IIC_SCL=0;
}
//IIC ����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��

static void IIC_Send_Byte(u8 txd)
	{
	u8 t;
	ChaoShengBo_SDA_OUT();
	ChaoShengBo_IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
	for(t=0;t<8;t++)
	{
		ChaoShengBo_IIC_SDA=(txd&0x80)>>7;
		txd<<=1;
		delay_us(10);
		ChaoShengBo_IIC_SCL=1;
		delay_us(10);
		ChaoShengBo_IIC_SCL=0;
		delay_us(10);
	}
}
//��1 ���ֽڣ�ack=1 ʱ������ACK��ack=0������nACK
static u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	ChaoShengBo_SDA_IN();//SDA ����Ϊ����
	for(i=0;i<8;i++ )
	{
		ChaoShengBo_IIC_SCL=0;
		delay_us(10);
		ChaoShengBo_IIC_SCL=1;
		receive<<=1;
		if(ChaoShengBo_READ_SDA)receive++;
		delay_us(5);
	}
	if (!ack)
	IIC_NAck();//����nACK
	else
	IIC_Ack(); //����ACK
	return receive;
}


