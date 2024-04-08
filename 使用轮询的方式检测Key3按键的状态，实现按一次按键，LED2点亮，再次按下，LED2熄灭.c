#if 0
/*轮训模式下测试按键*/
#include "exynos_4412.h"


int main()
{
	/*设置GPX1_1为接收模式*/
	GPX1.CON = GPX1.CON & (~(0xF << 4));

	while(1){
		/*判断是否按键*/
		if( !(GPX1.DAT & (0x1 << 1)) ){
			printf("key2 press\n");
			/*等待松手*/
			while( !(GPX1.DAT & (0x1 << 1)) );

		}else{
			
		}
	}


	return 0;
}
#endif

/*使用轮询的方式检测Key3按键的状态，实现按一次按键，LED2点亮，再次按下，LED2熄灭*/

#include "exynos_4412.h"

/*配置LED2 GPX2_7*/
void LED2_CONFIG(){
	GPX2.CON = GPX2.CON & (~(0xF << 28)) | (0x1 << 28);
}

int main(void){
	LED2_CONFIG();
	int flag = 0;
	/*设置GPX1_2为接收模式*/
	GPX1.CON = GPX1.CON & (~(0xF << 8));

	while(1){
		/*判断是否按键*/
		if( !(GPX1.DAT & (0x1 << 2)) ){
			printf("Key3 press\n");
			while( !(GPX1.DAT & (0x1 << 2)) );
			flag++;
		}
		if(flag % 2){
			/*灭*/
			GPX2.DAT = GPX2.DAT & (~(0x1 << 7));

		}else{
			/*点亮*/
			GPX2.DAT = GPX2.DAT | (0x1 << 7);
		}
	}

	return 0;
}
