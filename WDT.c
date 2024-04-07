#if 0
#include "exynos_4412.h"

void Delay(unsigned int time){
	while(time--);
}

int main()
{
	/*设置看门狗的一级分频为100（真实设置为99，自动加1），二级分频为64，不使用中断*/
	WDT.WTCON = WDT.WTCON & (~(0xFFFF << 0)) | (0x6331 << 0);
	/*设置看门狗的发送复位的时间为 (15625*5) 5秒*/
	// t_watchdog = 1/(PCLK/(Prescaler value + 1)/Division_factor)
	// 15625 为一秒就执行完
	WDT.WTCNT = (15625 * 5);

	while(1){
		printf("WDT.WTCNT = %d\n", WDT.WTCNT);
		Delay(100000);
	} 

	return 0;
}
#endif

/*
 * 编程实现将WDT的递减频率设置为10000HZ，程序运行5s后开发板复位
 * */

#include "exynos_4412.h"

int main(int argc, char *argv[]){
	/*设置一级分频为78*/
	WDT.WTCON = WDT.WTCON & (~(0xFF << 8)) | (0x4E << 8);
	/*设置二级分频为128*/
	WDT.WTCON = WDT.WTCON & (~(0x3 << 3)) | (0x3 << 3);
	/*不使用中断*/
	WDT.WTCON = WDT.WTCON & (~(1 << 2));
	/*正常操作*/
	WDT.WTCON = WDT.WTCON & (~(1 << 1));
	/*使用复位*/
	WDT.WTCON = WDT.WTCON | (1 << 0);
	/*复位时间*/
	WDT.WTCNT = (10000 * 5);
	/*WDT使能*/
	WDT.WTCON = WDT.WTCON | (1 << 5);

	while(1){
		;
	}
	return 0;
}



