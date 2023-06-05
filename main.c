/**
  ******************************************************************************
  * @file    main.c
  * @author  ������
  * @version V1.0
  * @date    2022-4-22
  * @brief   �ɿ�ϵͳ����
  ******************************************************************************
  * Ӳ��ƽ̨	 :STM32F103RCT6ϵͳ��+MPU6050ģ��+��·PWM��+���������+PID����
  ******************************************************************************
  */ 
#include "main.h"//������ͷ�ļ�

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void) 
{	
	SYS_Init();//ϵͳ��ʼ���ܺ���
	set_motor1_dutyfactor(ChannelPulse1);
	set_motor2_dutyfactor(ChannelPulse2);
	while(1)   //��ѭ��
	{
		MPU_Read();    //MPU6050���ݶ�ȡ
		DATA_Report();		//MPU6050�����ϱ�
		deal_serial_data();		
		//Controler();
		/* �������� */
    if (Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)
    {
      ChannelPulse1 += 10;    
			 
			set_motor1_dutyfactor(ChannelPulse1);
			
    }
		if (Key_Scan(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == KEY_ON)
    {
//      ChannelPulse1 -= 10;    
//			ChannelPulse2 -= 5;   
//      set_motor1_dutyfactor(ChannelPulse1);
//			set_motor2_dutyfactor(ChannelPulse2);
			ChannelPulse2 += 10;
			set_motor2_dutyfactor(ChannelPulse2);
    }
	}
}













/**
  * @brief  ϵͳ��ʼ���ܺ���
  * @param  ��
  * @retval ��
  */
void SYS_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //�ж����ȼ����麯��
	delay_init();	    							 //��ʱ������ʼ��	  
	uart_init(115200);	 	                         //���ڳ�ʼ��Ϊ115200
    uart3_init(115200);
	LED_Init();		                               	 //��ʼ����LED���ӵ�Ӳ���ӿ�
	//BASIC_TIM_Init();                            //��ʼ��������ʱ��
	MPU_Init();	                                   //��ʼ��MPU6050
	HCSR04_Init();                                 //����������ʼ��
	Key_GPIO_Config();                             //������ʼ��
	Motor_TIM_Init();                              //���PWM��ʼ��
	while(mpu_dmp_init())                          //��ʼ��mpu_dmp��
 	{
		LED2 = !LED2;delay_ms(50);LED2 = !LED2;     //LED��˸ָʾ
		printf("Initialization failed��\r\n");		//���ڳ�ʼ��ʧ���ϱ�
	}
	printf("Initialization succeed��\r\n");			//���ڳ�ʼ���ɹ��ϱ�
	LED1 = !LED1;delay_ms(50);LED1 = !LED1;		//LED��˸ָʾ
	delay_ms(50);									//��ʱ��������ʾ	
	mpu6050.flag = 0;                               //�ɼ��ɹ���־λ��ʼ��
	mpu6050.speed = 0;								//�ϱ��ٶȳ�ʼ��
	MPU_Read();
	pitch0 = pitch;
	yaw0 = yaw;
	roll0 = roll;
	Length_target = (uint32_t)Length;
	Yaw_target = (uint32_t)yaw;
}
/**
  * @brief  MPU6050���ݶ�ȡ����
  * @param  ��
  * @retval ��
  */
void MPU_Read(void)
{
	
	if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)//dmp����õ����ݣ��Է���ֵ�����ж�
	{ 
		temp=MPU_Get_Temperature();	                //�õ��¶�ֵ
		MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//�õ����ٶȴ���������
		MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//�õ�����������
		Length = Hcsr04GetLength();
		mpu6050.speed++;                            //�ϱ��ٶ��Լ�
		if(mpu6050.speed == 1)						//�ϱ��ٶ���ֵ����
		{
			mpu6050.flag = 1;						//�ɼ��ɹ���־λ����Ϊ��Ч
			mpu6050.speed = 0;						//�ϱ��ٶȹ���
		}	
	}
	else 											//�ɼ����ɹ�										
	{
		mpu6050.flag = 0;							//�ɼ��ɹ���־λ����Ϊ��Ч
	}	
}
/**
  * @brief  MPU6050�����ϱ�
  * @param  ��
  * @retval ��
  */
void DATA_Report(void)
{
	if(mpu6050.flag == 1)						//�ɼ��ɹ�ʱ
	{ 
		printf("temp:%d.%d,  ",temp/100,temp%10); //ͨ������1����¶�
		//pitch
		pitch1=(pitch-pitch0)*10;							
		printf("pitch:%d,  ",(int)pitch1/100); //ͨ������1���pitch	
		//yaw
		yaw1=(yaw-yaw0)*10;                           //��tempΪyaw
		printf("yaw:%d,  ",(int)yaw1/10);//ͨ������1���yaw
		//roll
		roll1=(roll-roll0)*10;                            //��tempΪroll		
		printf("roll:%d,  ",(int)roll1/100);//ͨ������1���roll
		USARTx = USART3;
		printf("gyrox:%5d,  gyroy:%5d,  gyroz:%5d,  aacx:%5d,  aacy:%5d,  aacz:%5d\r",gyrox,gyroy,gyroz,aacx,aacy,aacz);//�ϱ����ٶ����ݣ����ٶ�����
		printf("����%5fcm\r\n",Length);
		printf("���1����%d,���2����%d\r\n",ChannelPulse1,ChannelPulse2);
		float spd1=(4.0*(float)ChannelPulse1/3999.0-1.55)*28788.0;
		float spd2=(4.0*(float)ChannelPulse2/3999.0-1.05)*19200.0;
		printf("���1�ٶ�%d,���2�ٶ�%d\r\n",(int)spd1,(int)spd2);
		LED1=!LED1;			//LED��˸
		delay_ms(50);
		mpu6050.flag = 0;									//�ɼ��ɹ���־λ����Ϊ��Ч
	}
	else ;														//������
}

/**
  * @brief  PID���������
  * @param  ��
  * @retval ��
  */
void Controler(void)
{
	

}

void bldcm1_pid_control(void)
{
	ChannelPulse1 = Motor1_PID_realize(Length , Length_target);
	if (ChannelPulse1)
	set_motor1_dutyfactor(ChannelPulse1);
}
void bldcm2_pid_control(void)
{
	ChannelPulse2 = Motor2_PID_realize((uint32_t)yaw1, Yaw_target);
	if (ChannelPulse2)
	set_motor2_dutyfactor(ChannelPulse2);
}


/**
  * @brief  ͨ��Э��
  * @param  ��
  * @retval ��
  */
void deal_serial_data(void)
{
	int  speed_temp=0;
	if (USART_RX_STA&0x8000)
	{
		
		if (USART_RX_BUF[0] == 'l' ||USART_RX_BUF[0] == 'L' )
		{
			speed_temp = atoi((char const *)USART_RX_BUF+1);
			ChannelPulse1 = (int)(((float)speed_temp/28788.0+1.55)/4.0*3999.0);
			set_motor1_dutyfactor(ChannelPulse1);
			printf("OK!");
		}
		else if(USART_RX_BUF[0] == 'r'||USART_RX_BUF[0] == 'R')
		{
			speed_temp = atoi((char const *)USART_RX_BUF+1);
			ChannelPulse2 = (int)(((float)speed_temp/19200.0+1.05)/4*3999.0);
			set_motor2_dutyfactor(ChannelPulse2);
			printf("OK!");
		}
		else if(USART_RX_BUF[0] == 'c'||USART_RX_BUF[0] == 'C') //��ʼ��Э��
		{
			ChannelPulse1 = 1.5/4.0*Motor1_TIM_Period;
			ChannelPulse2 = 1.0/4.0*Motor2_TIM_Period;
			set_motor1_dutyfactor(1.5/4.0*Motor1_TIM_Period);
			set_motor2_dutyfactor(1.0/4.0*Motor2_TIM_Period);
			printf("OK!");
		}
		else if(USART_RX_BUF[0] == 's'||USART_RX_BUF[0] == 'S') //����Э��
		{
			ChannelPulse1 = 1.62/4.0*Motor1_TIM_Period;
			ChannelPulse2 = 1.1/4.0*Motor1_TIM_Period;
			set_motor1_dutyfactor(ChannelPulse1);
			set_motor2_dutyfactor(ChannelPulse2);
			printf("OK!");
		}
		else if(USART_RX_BUF[0] == 'b'||USART_RX_BUF[0] == 'B') //����Э��
		{
			ChannelPulse1 -= 10;    
			set_motor1_dutyfactor(ChannelPulse1);
		}
		else if(USART_RX_BUF[0] == 'd'||USART_RX_BUF[0] == 'D') //����Э��
		{    
			ChannelPulse2 -= 10; 
			set_motor2_dutyfactor(ChannelPulse2);
		}
		else if(USART_RX_BUF[0] == 'q'||USART_RX_BUF[0] == 'B') //����Э��
		{
			ChannelPulse1 += 10;    
			set_motor1_dutyfactor(ChannelPulse1);
		}
		else if(USART_RX_BUF[0] == 'p'||USART_RX_BUF[0] == 'D') //����Э��
		{  
			ChannelPulse2 += 10; 
			set_motor2_dutyfactor(ChannelPulse2);
		}
		
	}
	USART_RX_STA=0;
}
