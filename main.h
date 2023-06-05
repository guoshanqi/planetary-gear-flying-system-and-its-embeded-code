#include "stdio.h"//��׼���������
#include "string.h"//�ַ�����
#include "stdlib.h"//���õ�ϵͳ������
#include "sys.h"//ϵͳ�жϷ����
#include "delay.h"//��ʱ������
#include "usart.h"//�������ÿ�
#include "led.h"//LED������
#include "mpu6050.h"//MPU6050������
#include "inv_mpu.h"//������������
#include "inv_mpu_dmp_motion_driver.h" //DMP��̬�����
#include "TIMgenerate.h" //���PWM������
#include "bsp_key.h"  //����������
//#include "bsp_basic_tim.h" //������ʱ����
#include "HCSR04.h"
#include "PID.h"
	


char tmp_buf[33];			//�ַ�������
float pitch0,roll0,yaw0;  //�����ʼ�Ƕ�
short pitch1,roll1,yaw1;   //��ʼ����ĽǶ�
float pitch,roll,yaw; 		//ԭʼŷ����:�����ǣ�ƫ���ǣ���ת��
short aacx,aacy,aacz;		//���ٶȴ�����ԭʼ����  angular acceleration
short gyrox,gyroy,gyroz;	//������ԭʼ����  gyroscope
short temp;					//�¶�
short Mspeed1,Mspeed2;


float Length = 0;
int Yaw_target = 0;
int Length_target = 0;
__IO uint16_t ChannelPulse1 = 1.5/4.0*Motor1_TIM_Period;
__IO uint16_t ChannelPulse2 = 1.0/4.0*Motor2_TIM_Period;

struct MPU6050				//MPU6050�ṹ��
{
	u8 flag;				//�ɼ��ɹ���־λ
	u8 speed;				//�ϱ��ٶ�
}mpu6050;					//Ψһ�ṹ�����


int main(void);				//������
void SYS_Init(void);		//ϵͳ��ʼ���ܺ���
void MPU_Read(void);		//MPU6050���ݶ�ȡ����	
void DATA_Report(void);		//MPU6050�����ϱ�	
void Controler(void);     //������
void deal_serial_data(void); //ͨ��

