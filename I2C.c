/*实时监测开发板的放置状态，当监测到开发板水平放置时，每隔一分钟向终端上打印一次当前的时间以及开发板的状态
*如：“2023-04-05 23:45:00 Status: Normal”
*当监测到开发板发生倾斜时，每隔一秒钟向终端上打印一次当前的时间以及开发板的状态
*如：“2023-04-05 23:45:00 Status: Warning”
*同时让蜂鸣器产生“滴滴”的警报声，在警报状态下，若按下Key2按键，解除蜂鸣器的警报声
*提示：
*开发板水平静止放置时MPU6050的Z轴上的加速度应该等于重力加速度的值(9.8m/s2)，而其X轴和Y轴上的加速度应该等于0
*当开发板发生倾斜时MPU6050的Z轴上的加速度的分量会减小，而其X轴和Y轴上的加速度分量会增大
*我们可以以此来判断开发板是否发生倾斜
**/

#include "exynos_4412.h"

/****************MPU6050内部寄存器地址****************/

#define	SMPLRT_DIV		0x19	//陀螺仪采样率，典型值：0x07(125Hz)
#define	CONFIG			0x1A	//低通滤波频率，典型值：0x06(5Hz)
#define	GYRO_CONFIG		0x1B	//陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
#define	ACCEL_CONFIG	0x1C	//加速计自检、测量范围及高通滤波频率，典型值：0x18(不自检，2G，5Hz)
#define	ACCEL_XOUT_H	0x3B
#define	ACCEL_XOUT_L	0x3C
#define	ACCEL_YOUT_H	0x3D
#define	ACCEL_YOUT_L	0x3E
#define	ACCEL_ZOUT_H	0x3F
#define	ACCEL_ZOUT_L	0x40
#define	TEMP_OUT_H		0x41
#define	TEMP_OUT_L		0x42
#define	GYRO_XOUT_H		0x43
#define	GYRO_XOUT_L		0x44
#define	GYRO_YOUT_H		0x45
#define	GYRO_YOUT_L		0x46
#define	GYRO_ZOUT_H		0x47
#define	GYRO_ZOUT_L		0x48
#define	PWR_MGMT_1		0x6B	//电源管理，典型值：0x00(正常启用)
#define	WHO_AM_I		0x75	//IIC地址寄存器(默认数值0x68，只读)
#define	SlaveAddress	0x68	//MPU6050-I2C地址

/************************延时函数************************/
void Delay(unsigned int time){
	while(time--);
}

void mydelay_ms(int time)
{
	int i,j;
	while(time--)
	{
		for(i=0;i<5;i++)
			for(j=0;j<514;j++);
	}
}

/************************RTC****************************/
void RTC_CONFIG(){
	/*使能RTC*/
	RTCCON = RTCCON | 1;
	/*设置年月日星期 2024年12月31日 星期七  日和星期需要交换一下*/
	RTC.BCDYEAR = 0x024;
	RTC.BCDMON  = 0x12;
	//RTC.BCDDAY  = 0x7;
	RTC.BCDWEEK = 0x31;
	/*设置时分秒 23:59:50*/
	RTC.BCDHOUR = 0x23;
	RTC.BCDMIN  = 0x59;
	RTC.BCDSEC  = 0x50;
	/*关闭RTC*/
	RTCCON = RTCCON & (~(1));
}

/************************PWM****************************/
void PWM_CONFIG(){
	/*将GPD0_0设置成PWM的输出引脚*/
	GPD0.CON = GPD0.CON & (~(0xF)) | (0x2);
	/*设置分频器1的分频为100(实际写99)*/
	PWM.TCFG0 = PWM.TCFG0 & (~(0xFF << 0)) | (0x63 << 0);
	/*二级分频1*/
	PWM.TCFG1 = PWM.TCFG1 & (~(0xF << 0));
	/*自动加载周期*/
	PWM.TCON = PWM.TCON | (1 << 3);
	/*设置频率为1000HHZ*/
	PWM.TCNTB0 = 1000;
	/*设置占空比(高电平所占比例)60%*/
	PWM.TCMPB0 = 600;
	/*将第一次的递减值写入(自动重载)*/
	PWM.TCON = PWM.TCON | (1 << 1);
	/*关闭手动更新*/
	PWM.TCON = PWM.TCON & (~(1 << 1));
	/*使能PWM*/
	PWM.TCON = PWM.TCON | 1;
	

}




/**********************************************************************
 * 函数功能：I2C向特定地址写一个字节
 * 输入参数：
 * 		slave_addr： I2C从机地址
 * 			  addr： 芯片内部特定地址
 * 			  data：写入的数据
**********************************************************************/

void iic_write (unsigned char slave_addr, unsigned char addr, unsigned char data)
{
	/*对时钟源进行512倍预分频  打开IIC中断（每次完成一个字节的收发后中断标志位会自动置位）*/
	I2C5.I2CCON = I2C5.I2CCON | (1<<6) | (1<<5);

	/*设置IIC模式为主机发送模式  使能IIC发送和接收*/
	I2C5.I2CSTAT = 0xd0;
	/*将第一个字节的数据写入发送寄存器  即从机地址和读写位（MPU6050-I2C地址+写位0）*/
	I2C5.I2CDS = slave_addr<<1;
	/*设置IIC模式为主机发送模式  发送起始信号启用总线  使能IIC发送和接收*/
	I2C5.I2CSTAT = 0xf0;

	/*等待从机接受完一个字节后产生应答信号（应答后中断挂起位自动置位）*/
	while(!(I2C5.I2CCON & (1<<4)));

	/*将要发送的第二个字节数据（即MPU6050内部寄存器的地址）写入发送寄存器*/
	I2C5.I2CDS = addr;
	/*清除中断挂起标志位  开始下一个字节的发送*/
	I2C5.I2CCON = I2C5.I2CCON & (~(1<<4));
	/*等待从机接受完一个字节后产生应答信号（应答后中断挂起位自动置位）*/
	while(!(I2C5.I2CCON & (1<<4)));

	/*将要发送的第三个字节数据（即要写入到MPU6050内部指定的寄存器中的数据）写入发送寄存器*/
	I2C5.I2CDS = data;
	/*清除中断挂起标志位  开始下一个字节的发送*/
	I2C5.I2CCON = I2C5.I2CCON & (~(1<<4));
	/*等待从机接受完一个字节后产生应答信号（应答后中断挂起位自动置位）*/
	while(!(I2C5.I2CCON & (1<<4)));

	/*发送停止信号  结束本次通信*/
	I2C5.I2CSTAT = 0xD0;
	/*清除中断挂起标志位*/
	I2C5.I2CCON = I2C5.I2CCON & (~(1<<4));
	/*延时*/
	mydelay_ms(10);
}

/**********************************************************************
 * 函数功能：I2C从特定地址读取1个字节的数据
 * 输入参数：         slave_addr： I2C从机地址
 * 			       addr： 芯片内部特定地址
 * 返回参数： unsigned char： 读取的数值
**********************************************************************/

unsigned char iic_read(unsigned char slave_addr, unsigned char addr)
{

	unsigned char data = 0;

	/*对时钟源进行512倍预分频  打开IIC中断（每次完成一个字节的收发后中断标志位会自动置位）*/
	I2C5.I2CCON = I2C5.I2CCON | (1<<6) | (1<<5);

	/*设置IIC模式为主机发送模式  使能IIC发送和接收*/
	I2C5.I2CSTAT = 0xd0;
	/*将第一个字节的数据写入发送寄存器  即从机地址和读写位（MPU6050-I2C地址+写位0）*/
	I2C5.I2CDS = slave_addr<<1;
	/*设置IIC模式为主机发送模式  发送起始信号启用总线  使能IIC发送和接收*/
	I2C5.I2CSTAT = 0xf0;
	/*等待从机接受完一个字节后产生应答信号（应答后中断挂起位自动置位）*/
	while(!(I2C5.I2CCON & (1<<4)));

	/*将要发送的第二个字节数据（即要读取的MPU6050内部寄存器的地址）写入发送寄存器*/
	I2C5.I2CDS = addr;
	/*清除中断挂起标志位  开始下一个字节的发送*/
	I2C5.I2CCON = I2C5.I2CCON & (~(1<<4));
	/*等待从机接受完一个字节后产生应答信号（应答后中断挂起位自动置位）*/
	while(!(I2C5.I2CCON & (1<<4)));

	/*清除中断挂起标志位  重新开始一次通信  改变数据传送方向*/
	I2C5.I2CCON = I2C5.I2CCON & (~(1<<4));

	/*将第一个字节的数据写入发送寄存器  即从机地址和读写位（MPU6050-I2C地址+读位1）*/
	I2C5.I2CDS = slave_addr << 1 | 0x01;
	/*设置IIC为主机接收模式  发送起始信号  使能IIC收发*/
	I2C5.I2CSTAT = 0xb0;
	/*等待从机接收到数据后应答*/
	while(!(I2C5.I2CCON & (1<<4)));


	/*禁止主机应答信号（即开启非应答  因为只接收一个字节）  清除中断标志位*/
	I2C5.I2CCON = I2C5.I2CCON & (~(1<<7))&(~(1<<4));
	/*等待接收从机发来的数据*/
	while(!(I2C5.I2CCON & (1<<4)));
	/*将从机发来的数据读取*/
	data = I2C5.I2CDS;

	/*直接发起停止信号结束本次通信*/
	I2C5.I2CSTAT = 0x90;
	/*清除中断挂起标志位*/
	I2C5.I2CCON = I2C5.I2CCON & (~(1<<4));
	/*延时等待停止信号稳定*/
	mydelay_ms(10);

	return data;

}


/**********************************************************************
 * 函数功能：MPU6050初始化
**********************************************************************/

void MPU6050_Init ()
{
	iic_write(SlaveAddress, PWR_MGMT_1, 0x00); 		//设置使用内部时钟8M
	iic_write(SlaveAddress, SMPLRT_DIV, 0x07);		//设置陀螺仪采样率
	iic_write(SlaveAddress, CONFIG, 0x06);			//设置数字低通滤波器
	iic_write(SlaveAddress, GYRO_CONFIG, 0x18);		//设置陀螺仪量程+-2000度/s
	iic_write(SlaveAddress, ACCEL_CONFIG, 0x03);		//设置加速度量程+-16g
}



/**********************************************************************
 * 函数功能：主函数
 **********************************************************************/

int main(void)
{
	unsigned int Oldsec = 0, Newsec = 0, Oldmin = 0, Newmin = 0;
	unsigned char zvalue_h,zvalue_l,xvalue_h,xvalue_l,yvalue_h,yvalue_l;						//存储读取结果(加速度x,y,x轴)
	short int xvalue,yvalue,zvalue;

	/*设置GPB_2引脚和GPB_3引脚功能为I2C传输引脚*/
	GPB.CON = (GPB.CON & ~(0xF<<12)) | 0x3<<12;			 	//设置GPB_3引脚功能为I2C_5_SCL
	GPB.CON = (GPB.CON & ~(0xF<<8))  | 0x3<<8;				//设置GPB_2引脚功能为I2C_5_SDA
	
	/*设置GPX1_1为接收模式,按键2*/
	GPX1.CON = GPX1.CON & (~(0xF << 4));

	uart_init(); 											//初始化串口
	MPU6050_Init();											//初始化MPU6050
	
	RTC_CONFIG();
	PWM_CONFIG();
	

	printf("\n********** I2C test!! ***********\n");
	while(1)
	{
		xvalue_h = iic_read(SlaveAddress, ACCEL_XOUT_H);		//获取MPU6050-x轴加速度高字节
		xvalue_l = iic_read(SlaveAddress, ACCEL_XOUT_L);		//获取MPU6050-x轴加速度低字节
		xvalue   =  (xvalue_h << 8) | xvalue_l;					//获取MPU6050-x轴加速度
		
		yvalue_h = iic_read(SlaveAddress, ACCEL_YOUT_H);		//获取MPU6050-y轴加速度高字节
		yvalue_l = iic_read(SlaveAddress, ACCEL_YOUT_L);		//获取MPU6050-y轴加速度低字节
		yvalue   =  (yvalue_h << 8) | yvalue_l;					//获取MPU6050-y轴加速度
		
		zvalue_h = iic_read(SlaveAddress, ACCEL_ZOUT_H);		//获取MPU6050-z轴加速度高字节
		zvalue_l = iic_read(SlaveAddress, ACCEL_ZOUT_L);		//获取MPU6050-z轴加速度低字节
		zvalue   =  (zvalue_h << 8) | zvalue_l;					//获取MPU6050-z轴加速度
		
		//开发板水平静止放置时MPU6050的Z轴上的加速度应该等于重力加速度的值(9.8m/s2)，而其X轴和Y轴上的加速度应该等于0
		
		if( (zvalue == 9.8) && (xvalue == 0) && (yvalue == 0)){
			//水平放置,每隔一分钟向终端上打印一次当前的时间以及开发板的状态,如：“2023-04-05 23:45:00 Status: Normal”
			Newsec = RTC.BCDMIN;
			
				if(Oldmin != Newmin){
					printf("20%x-%x-%x %x:%x:%x Status: Normal\n", RTC.BCDYEAR, RTC.BCDMON, RTC.BCDWEEK, RTC.BCDHOUR, RTC.BCDMIN, RTC.BCDSEC);
					Oldmin = Newmin;
				}
			
		}else{
			//监测到开发板发生倾斜时，每隔一秒钟向终端上打印一次当前的时间以及开发板的状态,如：“2023-04-05 23:45:00 Status: Warning”
			//同时让蜂鸣器产生“滴滴”的警报声，在警报状态下，若按下Key2按键，解除蜂鸣器的警报声(PWM脉冲产生)


			Newsec = RTC.BCDSEC;
			if(Oldsec != Newsec){
				printf("20%x-%x-%x %x:%x:%x Status: Warning\n", RTC.BCDYEAR, RTC.BCDMON, RTC.BCDWEEK, RTC.BCDHOUR, RTC.BCDMIN, RTC.BCDSEC);
				Oldsec = Newsec;
			}

			/*判断是否按键*/
			if( !(GPX1.DAT & (0x1 << 1)) ){
				/*关闭PWM*/
				PWM.TCON = PWM.TCON & (~(1));
				Delay(10000000); //通过这个来判断现象
				while( !(GPX1.DAT & (0x1 << 1)) );

			}else{

				// 产生滴滴
				/*使能PWM*/
				PWM.TCON = PWM.TCON | 1;
				Delay(1000000);
				/*关闭PWM*/
				PWM.TCON = PWM.TCON & (~(1));
				Delay(1000000);
			}

		}
	}
	return 0;
}


