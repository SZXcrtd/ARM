#if 0
#include "exynos_4412.h"
/*
void Delay(unsigned int time){
	while(time--);
}
*/



void UART2_Init(){
	
	/*1.将GPA1_0和GPA1_1分别设置成UART2的接收引脚和发送引脚 GPA1CON [7:0]*/
	GPA1.CON = GPA1.CON & (~(0xF << 0)) | (0x2 << 0); //接收引脚
	GPA1.CON = GPA1.CON & (~(0xF << 4)) | (0x2 << 4); //发送引脚
	
	/*2.设置UART2的帧格式 ULCON2  / 8位数据位 1位停止位 无校验 正常模式*/
	UART2.ULCON2 = UART2.ULCON2 | (0x3); //8位数据位
	UART2.ULCON2 = UART2.ULCON2 & (~(0x1 << 2)); //1位停止位
	UART2.ULCON2 = UART2.ULCON2 & (~(0x7 << 3)); //无校验
	UART2.ULCON2 = UART2.ULCON2 & (~(0x1 << 6)); //正常模式

	/*3.设置 UART2的接收和发送模式为轮训模式 UCON2 [3:0]*/
	UART2.UCON2 = UART2.UCON2 & (~(0x3 << 0)) | (0x1 << 0); //接收
	UART2.UCON2 = UART2.UCON2 & (~(0x3 << 2)) | (0x1 << 2); //发送

	/*4.设置UART2的波特率为 115200 UBRDIV2 / UFRACVAL2*/
	UART2.UBRDIV2 = UART2.UBRDIV2 & (~(0xFFFF << 0)) | (0x34 << 0);
	UART2.UFRACVAL2 = UART2.UFRACVAL2 & (~(0xF << 0)) | (0x0 << 0);
}

void UART2_Send_Byte(char data){

	/*判断UTXH2是否可写*/
	while( !(UART2.UTRSTAT2 & (1 << 1)) );
	/*将要发送的数据写入发送寄存器 UTXH2 */
	UART2.UTXH2 = data;
}

void UART2_Send_Str(char *str){
	while(*str != '\0'){
		UART2_Send_Byte(*str++);
	}
}

char UART2_Recv_Data(){
	char Recv_data = 0;
	if(UART2.UTRSTAT2 & 1){
		Recv_data = UART2.URXH2;
		return Recv_data;
	}else{
		return 0;
	}
}

int main()
{
	char Recv_data = 0;
	UART2_Init();

	while(1){
		/*
		Recv_data = UART2_Recv_Data(); // 这是开发板的程序，先从电脑获取数据
		if(Recv_data == 0){
		
		}else{
			Recv_data = Recv_data + 1;
			UART2_Send_Byte(Recv_data);
		}
		*/
		UART2_Send_Str("hello\n");
		//Delay(1000000);
	}

	return 0;
}
#endif

/*
 * 编程实现电脑远程控制LED状态
 * 注：在终端上输入‘2’，LED2点亮，再次输入‘2’，LED2熄灭... ...
 * */

#include "exynos_4412.h"


void UART2_Init(){
	
	/*1.将GPA1_0和GPA1_1分别设置成UART2的接收引脚和发送引脚 GPA1CON [7:0]*/
	GPA1.CON = GPA1.CON & (~(0xF << 0)) | (0x2 << 0); //接收引脚
	GPA1.CON = GPA1.CON & (~(0xF << 4)) | (0x2 << 4); //发送引脚
	
	/*2.设置UART2的帧格式 ULCON2  / 8位数据位 1位停止位 无校验 正常模式*/
	UART2.ULCON2 = UART2.ULCON2 | (0x3); //8位数据位
	UART2.ULCON2 = UART2.ULCON2 & (~(0x1 << 2)); //1位停止位
	UART2.ULCON2 = UART2.ULCON2 & (~(0x7 << 3)); //无校验
	UART2.ULCON2 = UART2.ULCON2 & (~(0x1 << 6)); //正常模式

	/*3.设置 UART2的接收和发送模式为轮训模式 UCON2 [3:0]*/
	UART2.UCON2 = UART2.UCON2 & (~(0x3 << 0)) | (0x1 << 0); //接收
	UART2.UCON2 = UART2.UCON2 & (~(0x3 << 2)) | (0x1 << 2); //发送

	/*4.设置UART2的波特率为 115200 UBRDIV2 / UFRACVAL2*/
	UART2.UBRDIV2 = UART2.UBRDIV2 & (~(0xFFFF << 0)) | (0x34 << 0);
	UART2.UFRACVAL2 = UART2.UFRACVAL2 & (~(0xF << 0)) | (0x0 << 0);
}

char UART2_Recv_Data(){
	char Recv_data = 0;
	if(UART2.UTRSTAT2 & 1){
		Recv_data = UART2.URXH2;
		return Recv_data;
	}else{
		return 0;
	}
}
void UART2_Send_Byte(char data){

	/*判断UTXH2是否可写*/
	while( !(UART2.UTRSTAT2 & (1 << 1)) );
	/*将要发送的数据写入发送寄存器 UTXH2 */
	UART2.UTXH2 = data;
}
void LED2_CONFIG(){
	GPX2.CON = GPX2.CON & (~(0xF << 28)) | (0x1 << 28);
}

int main(void){
	char num = 0;
	LED2_CONFIG();
	UART2_Init();
	while(1){
		while(1){
			num = UART2_Recv_Data();
			UART2_Send_Byte(num);
			if(num == '2'){
#if 1
				/*LED2灭*/
				GPX2.DAT = GPX2.DAT & (~(0x1 << 7));
				break;
#endif

#if 0
				/*点亮LED2*/
				GPX2.DAT = GPX2.DAT | (0x1 << 7);
				num = UART2_Recv_Data();
				UART2_Send_Byte(num);
				if(num == '2'){
					/*LED2灭*/
					GPX2.DAT = GPX2.DAT & (~(0x1 << 7));
				}
#endif
			}
#if 0
			num = UART2_Recv_Data();
			UART2_Send_Byte(num);
			/*点亮LED2*/
			GPX2.DAT = GPX2.DAT | (0x1 << 7);
#endif
		}
		while(1){

			num = UART2_Recv_Data();
			UART2_Send_Byte(num);
			if(num == '2'){
				/*LED2亮*/
				GPX2.DAT = GPX2.DAT | (0x1 << 7);
				break;
			}

		}
	}
	return 0;
}
