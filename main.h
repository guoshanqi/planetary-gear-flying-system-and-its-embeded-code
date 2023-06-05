#include "stdio.h"//标准输入输出库
#include "string.h"//字符串库
#include "stdlib.h"//常用的系统函数库
#include "sys.h"//系统中断分组库
#include "delay.h"//延时函数库
#include "usart.h"//串口设置库
#include "led.h"//LED驱动库
#include "mpu6050.h"//MPU6050驱动库
#include "inv_mpu.h"//陀螺仪驱动库
#include "inv_mpu_dmp_motion_driver.h" //DMP姿态解读库
#include "TIMgenerate.h" //电机PWM驱动库
#include "bsp_key.h"  //按键驱动库
//#include "bsp_basic_tim.h" //基本定时器库
#include "HCSR04.h"
#include "PID.h"
	


char tmp_buf[33];			//字符串数组
float pitch0,roll0,yaw0;  //储存初始角度
short pitch1,roll1,yaw1;   //初始化后的角度
float pitch,roll,yaw; 		//原始欧拉角:俯仰角，偏航角，滚转角
short aacx,aacy,aacz;		//加速度传感器原始数据  angular acceleration
short gyrox,gyroy,gyroz;	//陀螺仪原始数据  gyroscope
short temp;					//温度
short Mspeed1,Mspeed2;


float Length = 0;
int Yaw_target = 0;
int Length_target = 0;
__IO uint16_t ChannelPulse1 = 1.5/4.0*Motor1_TIM_Period;
__IO uint16_t ChannelPulse2 = 1.0/4.0*Motor2_TIM_Period;

struct MPU6050				//MPU6050结构体
{
	u8 flag;				//采集成功标志位
	u8 speed;				//上报速度
}mpu6050;					//唯一结构体变量


int main(void);				//主函数
void SYS_Init(void);		//系统初始化总函数
void MPU_Read(void);		//MPU6050数据读取函数	
void DATA_Report(void);		//MPU6050数据上报	
void Controler(void);     //控制器
void deal_serial_data(void); //通信

