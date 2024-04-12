/*编程实现通过PWM控制蜂鸣器产生"嘀嘀"的声音
 *注：PWM的频率1000HZ，占空比%60
 * */
#include "exynos_4412.h"

void Delay(unsigned int time){
	while(time--);
}

int main()
{
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

	while(1){
		/*使能PWM*/
		PWM.TCON = PWM.TCON | 1;
		Delay(1000000);
		/*关闭PWM*/
		PWM.TCON = PWM.TCON & (~(1));
		Delay(1000000);
	}

	return 0;
}
