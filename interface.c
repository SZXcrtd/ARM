/*中断测试按键*/
#include "exynos_4412.h"


int main()
{
	
	/*将GPX1_1设置成中断功能*/
	GPX1.CON = GPX1.CON & (~(0xF << 4)) | (0xF << 4);
	/*设置GPX1_1中断的触发方式*/
	EXT_INT41_CON = EXT_INT41_CON & (~(0x7 << 4)) | (0x2 << 4);
	/*使能GPX1_1的中断功能*/
	EXT_INT41_MASK & (~(0x1 << 1));

	/*中断控制器层次 - 让中断控制器接收外设产生的中断信号并对其进行管理然后再转发给CPU处理*/
	/*全局使能中断控制器使其能接收外设产生的中断信号并转发到CPU接口*/
	ICDDCR = ICDDCR | 1;
	/*在中断控制器中使能57号中断，使中断控制器接收到57号中断后能将其转发到CPU接口*/
	ICDISER.ICDISER1 = ICDISER.ICDISER1 | (1 << 25);
	/*选择由CPU0来处理57号中断*/
	ICDIPTR.ICDIPTR14 = ICDIPTR.ICDIPTR14 & (~(0xFF	<< 8)) | (0X01 << 8);
	/*使能中断控制器和CPU0之间的接口，使中断控制器转发的中断信号能够到达CPU0*/
	CPU0.ICCICR = CPU0.ICCICR | 1;
		


	while(1){
		
	}

	return 0;
}
