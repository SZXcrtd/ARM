/*使用中断的方式检测Key3按键的状态，实现按一次按键，LED2点亮，再次按下，LED2熄灭*/
#include "exynos_4412.h"

void do_irq(void){
	static int flag = 0;
	/*获取产生中断的中断号*/
	unsigned int irqNum = 0;
	irqNum = CPU0.ICCIAR & 0x3FF;
	/*根据中断号处理相关的中断*/
	switch(irqNum){
		case 0:
			/*0号中断的处理程序*/
			break;
		case 1:
			//1号中断的处理程序
			break;
			/*
			 * ... ...
			 * */
		case 58: //按键3
			printf("key3 press\n");
			if(flag){
				/*点亮*/
				GPX2.DAT = GPX2.DAT | (0x1 << 7);
				flag = !flag;
			}
			else{
				/*熄灭*/
				GPX2.DAT = GPX2.DAT & (~(0x1 << 7));
				flag = !flag;
			}
			/*清除GPIO控制器中GPX1_2中断挂起标志位*/
			EXT_INT41_PEND = (1 << 2);
			/*将当前中断的中断号写回中断控制器中，以这种方式告知中断控制器当前中断已处理完毕，可以处理其他中断*/
			CPU0.ICCEOIR = CPU0.ICCEOIR & (~(0x3FF)) | (58);
			break;
				
			/*
			 * ... ...
			 * */
		case 159:
			//159号中断的处理程序
			break;
		default:
			break;

	}
}

int main(int argc, char *argv[]){

	/*外设层次 - 让外部的硬件控制器产生一个中断信号发送给中断控制器*/
	/*将GPX1_2设置成中断功能*/
	GPX1.CON = GPX1.CON & (~(0xF << 8)) | (0xF << 8);
	/*设置GPX1_2中断的触发方式下降沿触发*/
	EXT_INT41_CON = EXT_INT41_CON & (~(0x7 << 8)) | (0x2 << 8);
	/*使能GPX1_2的中断功能*/
	EXT_INT41_MASK = EXT_INT41_MASK & (~(0x1 << 2));

	/*中断控制器层次 - 让中断控制器接收外设产生的中断信号并对其进行管理然后再转发给CPU处理*/
	/*全局使能中断控制器使其能接收外设产生的中断信号并转发到CPU接口*/
	ICDDCR = ICDDCR | 1;
	/*在中断控制器中使能58号中断，使中断控制器接收到58号中断后能将其转发到CPU接口*/
	ICDISER.ICDISER1 = ICDISER.ICDISER1 | (1 << 26);
	/*选择由CPU0来处理58号中断*/
	ICDIPTR.ICDIPTR14 = ICDIPTR.ICDIPTR14 & (~(0xFF	<< 16)) | (0x1 << 16);
	/*使能中断控制器和CPU0之间的接口，使中断控制器转发的中断信号能够到达CPU0*/
	CPU0.ICCICR = CPU0.ICCICR | 1;
		
	/*配置GPX2_7,使其输出模式*/
	GPX2.CON = GPX2.CON & (~(0xF << 28)) | (0x1 << 28);

	while(1);

	return 0;
}

#if 0
/*中断测试按键*/
#include "exynos_4412.h"

void Delay(unsigned int time){
	while(time--);
}

void do_irq(void){
	/*获取产生中断的中断号*/
	unsigned int irqNum = 0;
	irqNum = CPU0.ICCIAR & 0x3FF;
	/*根据中断号处理相关的中断*/
	switch(irqNum){
		case 0:
			/*0号中断的处理程序*/
			break;
		case 1:
			//1号中断的处理程序
			break;
			/*
			 * ... ...
			 * */
		case 57: //按键2
			printf("key2 press\n");
			/*清除GPIO控制器中GPX1_1中断挂起标志位*/
			EXT_INT41_PEND = (1 << 1);
			/*将当前中断的中断号写回中断控制器中，以这种方式告知中断控制器当前中断已处理完毕，可以处理其他中断*/
			CPU0.ICCEOIR = CPU0.ICCEOIR & (~(0x3FF)) | (57);
			break;
			/*
			 * ... ...
			 * */
		case 159:
			//159号中断的处理程序
			break;
		default:
			break;

	}

}

int main()
{
	/*外设层次 - 让外部的硬件控制器产生一个中断信号发送给中断控制器*/
	/*将GPX1_1设置成中断功能*/
	GPX1.CON = GPX1.CON & (~(0xF << 4)) | (0xF << 4);
	/*设置GPX1_1中断的触发方式*/
	EXT_INT41_CON = EXT_INT41_CON & (~(0x7 << 4)) | (0x2 << 4);
	/*使能GPX1_1的中断功能*/
	EXT_INT41_MASK = EXT_INT41_MASK & (~(0x1 << 1));

	/*中断控制器层次 - 让中断控制器接收外设产生的中断信号并对其进行管理然后再转发给CPU处理*/
	/*全局使能中断控制器使其能接收外设产生的中断信号并转发到CPU接口*/
	ICDDCR = ICDDCR | 1;
	/*在中断控制器中使能57号中断，使中断控制器接收到57号中断后能将其转发到CPU接口*/
	ICDISER.ICDISER1 = ICDISER.ICDISER1 | (1 << 25);
	/*选择由CPU0来处理57号中断*/
	ICDIPTR.ICDIPTR14 = ICDIPTR.ICDIPTR14 & (~(0xFF	<< 8)) | (0x1 << 8);
	/*使能中断控制器和CPU0之间的接口，使中断控制器转发的中断信号能够到达CPU0*/
	CPU0.ICCICR = CPU0.ICCICR | 1;
		
	/*配置GPX2_7,使其输出模式*/
	GPX2.CON = GPX2.CON & (~(0xF << 28)) | (0x1 << 28);

	while(1)
	{
		/*点亮*/
		GPX2.DAT = GPX2.DAT | (0x1 << 7);
		/*延时*/
		Delay(1000000);
		/*熄灭*/
		GPX2.DAT = GPX2.DAT & (~(0x1 << 7));
		/*延时*/
		Delay(1000000);

	}



	return 0;
}
#endif
