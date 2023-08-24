/**
  ******************************************************************************
  * @file    main.c
  * @author  郭善旗 Shanqi Guo
  * @version V1.0
  * @date    2022-4-22
  * @brief   飞控系统开发
  ******************************************************************************
  * 硬件平台	 :STM32F103RCT6系统板+MPU6050模块+两路PWM波+超声波测距+PID主控
  ******************************************************************************
  */ 
#include "main.h"//主函数头文件

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void) 
{	
	SYS_Init();//系统初始化总函数
	set_motor1_dutyfactor(ChannelPulse1);
	set_motor2_dutyfactor(ChannelPulse2);
	while(1)   //主循环
	{
		MPU_Read();    //MPU6050数据读取
		DATA_Report();		//MPU6050数据上报
		deal_serial_data();		
		//Controler();
		/* 处理数据 */
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
  * @brief  系统初始化总函数
  * @param  无
  * @retval 无
  */
void SYS_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //中断优先级分组函数
	delay_init();	    							 //延时函数初始化	  
	uart_init(115200);	 	                         //串口初始化为115200
    uart3_init(115200);
	LED_Init();		                               	 //初始化与LED连接的硬件接口
	//BASIC_TIM_Init();                            //初始化基本定时器
	MPU_Init();	                                   //初始化MPU6050
	HCSR04_Init();                                 //超声波测距初始化
	Key_GPIO_Config();                             //按键初始化
	Motor_TIM_Init();                              //电机PWM初始化
	while(mpu_dmp_init())                          //初始化mpu_dmp库
 	{
		LED2 = !LED2;delay_ms(50);LED2 = !LED2;     //LED闪烁指示
		printf("Initialization failed！\r\n");		//串口初始化失败上报
	}
	printf("Initialization succeed！\r\n");			//串口初始化成功上报
	LED1 = !LED1;delay_ms(50);LED1 = !LED1;		//LED闪烁指示
	delay_ms(50);									//延时初界面显示	
	mpu6050.flag = 0;                               //采集成功标志位初始化
	mpu6050.speed = 0;								//上报速度初始化
	MPU_Read();
	pitch0 = pitch;
	yaw0 = yaw;
	roll0 = roll;
	Length_target = (uint32_t)Length;
	Yaw_target = (uint32_t)yaw;
}
/**
  * @brief  MPU6050数据读取函数
  * @param  无
  * @retval 无
  */
void MPU_Read(void)
{
	
	if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)//dmp处理得到数据，对返回值进行判断
	{ 
		temp=MPU_Get_Temperature();	                //得到温度值
		MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//得到加速度传感器数据
		MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//得到陀螺仪数据
		Length = Hcsr04GetLength();
		mpu6050.speed++;                            //上报速度自加
		if(mpu6050.speed == 1)						//上报速度阈值设置
		{
			mpu6050.flag = 1;						//采集成功标志位设置为有效
			mpu6050.speed = 0;						//上报速度归零
		}	
	}
	else 											//采集不成功										
	{
		mpu6050.flag = 0;							//采集成功标志位设置为无效
	}	
}
/**
  * @brief  MPU6050数据上报
  * @param  无
  * @retval 无
  */
void DATA_Report(void)
{
	if(mpu6050.flag == 1)						//采集成功时
	{ 
		printf("temp:%d.%d,  ",temp/100,temp%10); //通过串口1输出温度
		//pitch
		pitch1=(pitch-pitch0)*10;							
		printf("pitch:%d,  ",(int)pitch1/100); //通过串口1输出pitch	
		//yaw
		yaw1=(yaw-yaw0)*10;                           //赋temp为yaw
		printf("yaw:%d,  ",(int)yaw1/10);//通过串口1输出yaw
		//roll
		roll1=(roll-roll0)*10;                            //赋temp为roll		
		printf("roll:%d,  ",(int)roll1/100);//通过串口1输出roll
		USARTx = USART3;
		printf("gyrox:%5d,  gyroy:%5d,  gyroz:%5d,  aacx:%5d,  aacy:%5d,  aacz:%5d\r",gyrox,gyroy,gyroz,aacx,aacy,aacz);//上报角速度数据，加速度数据
		printf("距离%5fcm\r\n",Length);
		printf("电机1脉宽%d,电机2脉宽%d\r\n",ChannelPulse1,ChannelPulse2);
		float spd1=(4.0*(float)ChannelPulse1/3999.0-1.55)*28788.0;
		float spd2=(4.0*(float)ChannelPulse2/3999.0-1.05)*19200.0;
		printf("电机1速度%d,电机2速度%d\r\n",(int)spd1,(int)spd2);
		LED1=!LED1;			//LED闪烁
		delay_ms(50);
		mpu6050.flag = 0;									//采集成功标志位设置为无效
	}
	else ;														//防卡死
}

/**
  * @brief  PID控制器设计
  * @param  无
  * @retval 无
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
  * @brief  通信协议
  * @param  无
  * @retval 无
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
		else if(USART_RX_BUF[0] == 'c'||USART_RX_BUF[0] == 'C') //初始化协议
		{
			ChannelPulse1 = 1.5/4.0*Motor1_TIM_Period;
			ChannelPulse2 = 1.0/4.0*Motor2_TIM_Period;
			set_motor1_dutyfactor(1.5/4.0*Motor1_TIM_Period);
			set_motor2_dutyfactor(1.0/4.0*Motor2_TIM_Period);
			printf("OK!");
		}
		else if(USART_RX_BUF[0] == 's'||USART_RX_BUF[0] == 'S') //启动协议
		{
			ChannelPulse1 = 1.62/4.0*Motor1_TIM_Period;
			ChannelPulse2 = 1.1/4.0*Motor1_TIM_Period;
			set_motor1_dutyfactor(ChannelPulse1);
			set_motor2_dutyfactor(ChannelPulse2);
			printf("OK!");
		}
		else if(USART_RX_BUF[0] == 'b'||USART_RX_BUF[0] == 'B') //启动协议
		{
			ChannelPulse1 -= 10;    
			set_motor1_dutyfactor(ChannelPulse1);
		}
		else if(USART_RX_BUF[0] == 'd'||USART_RX_BUF[0] == 'D') //启动协议
		{    
			ChannelPulse2 -= 10; 
			set_motor2_dutyfactor(ChannelPulse2);
		}
		else if(USART_RX_BUF[0] == 'q'||USART_RX_BUF[0] == 'B') //启动协议
		{
			ChannelPulse1 += 10;    
			set_motor1_dutyfactor(ChannelPulse1);
		}
		else if(USART_RX_BUF[0] == 'p'||USART_RX_BUF[0] == 'D') //启动协议
		{  
			ChannelPulse2 += 10; 
			set_motor2_dutyfactor(ChannelPulse2);
		}
		
	}
	USART_RX_STA=0;
}
