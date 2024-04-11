/*编程实现通过LED状态显示当前电压范围
 *电压在1501mv~1800mv时，LED2、LED3、LED4、LED5点亮
  电压在1001mv~1500mv时，LED2、LED3、LED4点亮
  电压在501mv~1000mv时，LED2、LED3点亮
  电压在0mv~500mv时，LED2闪烁
 *
 * */
#include "exynos_4412.h"

void Delay(unsigned int time){
	while(time--);
}

int main(int argc, char *argv[]){
	/*配置LED2，使其GPX2_7输出模式*/
	GPX2.CON = GPX2.CON & (~(0xF << 28)) | (0x1 << 28);
	/*配置LED3，使其GPX1_0输出模式*/
	GPX1.CON = GPX1.CON & (~(0xF << 0)) | (0x1 << 0); 
	/*配置LED4，使其GPF3_4输出模式*/
	GPF3.CON = GPF3.CON & (~(0xF << 16)) | (0x1 << 16);
	/*配置LED5，使其GPF3_5输出模式*/
	GPF3.CON = GPF3.CON & (~(0xF << 20)) | (0x1 << 20);

	/*选择转化通道*/
	ADCMUX = ADCMUX & (~(0xF << 0)) | (0x3 << 0);
	/*转化成12位*/
	ADCCON = ADCCON | (1 << 16);
	/*使用分频*/
	ADCCON = ADCCON | (1 << 14);
	/*分频倍数*/
	ADCCON = ADCCON & (~(0xFF << 6)) | (0x13 << 6);
	/*正常操作模式*/
	ADCCON = ADCCON & (~(1 << 2));
	/*将ADC读使能关闭*/
	ADCCON = ADCCON & (~(1 << 1));

	unsigned int AdcValue = 0;

	while(1){
		/*ADC转化开始*/
		ADCCON = ADCCON | 1;
		/*等待转化完成*/
		while( !(ADCCON & (1 << 15)) );
		/*读取转化结果*/
		AdcValue = ADCDAT & 0xFFF;
		/*将结果转化为电压(mv)*/
		AdcValue = AdcValue * 0.44;
		/*打印出结果*/
		printf("AdcValue = %dmv\n", AdcValue);

		if( (AdcValue >= 1501) && (AdcValue <= 1800) ){
			/*点亮LED2，LED3，LED4，LED5*/
			GPX2.DAT = GPX2.DAT | (0x1 << 7);
			GPX1.DAT = GPX1.DAT | 1;
			GPF3.DAT = GPF3.DAT | (0x1 << 4);
			GPF3.DAT = GPF3.DAT | (0x1 << 5);
		}else if( (AdcValue >= 1001) && (AdcValue <= 1500) ){
			GPX2.DAT = GPX2.DAT | (0x1 << 7);
			GPX1.DAT = GPX1.DAT | 1;
			GPF3.DAT = GPF3.DAT | (0x1 << 4);
			/*LED5关闭*/
			GPF3.DAT = GPF3.DAT & (~(0x1 << 5));
		}else if( (AdcValue >= 501) && (AdcValue <= 1000) ){
			GPX2.DAT = GPX2.DAT | (0x1 << 7);
			GPX1.DAT = GPX1.DAT | 1;
			/*LED4 LED5关闭*/
			GPF3.DAT = GPF3.DAT & (~(0x1 << 4));
			GPF3.DAT = GPF3.DAT & (~(0x1 << 5));
		}else{
			while(1){
				/*LED3 LED4 LED5关闭*/
				GPX1.DAT = GPX1.DAT & (~(0x1));
				GPF3.DAT = GPF3.DAT & (~(0x1 << 4));
				GPF3.DAT = GPF3.DAT & (~(0x1 << 5));
				/*LED2闪烁*/
				GPX2.DAT = GPX2.DAT & (~(0x1 << 7));
				Delay(1000000);
				GPX2.DAT = GPX2.DAT | (0x1 << 7);
				Delay(1000000);
			}
		}
	}

	return 0;
}

#if 0
#include "exynos_4412.h"

int main()
{
	unsigned int AdcValue = 0;
	/*选择转化通道*/
	ADCMUX = ADCMUX & (~(0xF << 0)) | (0x3 << 0);
	/*转化成12位*/
	ADCCON = ADCCON | (1 << 16);
	/*使用分频*/
	ADCCON = ADCCON | (1 << 14);
	/*分频倍数*/
	ADCCON = ADCCON & (~(0xFF << 6)) | (0x13 << 6);
	/*正常操作模式*/
	ADCCON = ADCCON & (~(1 << 2));
	/*将ADC读使能关闭*/
	ADCCON = ADCCON & (~(1 << 1));

	while(1){
		/*ADC转化开始*/
		ADCCON = ADCCON | 1;
		/*等待转化完成*/
		while( !(ADCCON & (1 << 15)) );
		/*读取转化结果*/
		AdcValue = ADCDAT & 0xFFF;
		/*将结果转化为电压(mv)*/
		AdcValue = AdcValue * 0.44;
		/*打印出结果*/
		printf("AdcValue = %dmv\n", AdcValue);
	}

	return 0;
}
#endif
