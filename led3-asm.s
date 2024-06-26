

.text
_start:

@ 使用汇编语言实现LED3闪烁实验,GPX1_0

MAIN:
	BL LED_CONFIG
LOOP:
	BL LED_ON
	BL DELAY
	BL LED_OFF
	BL DELAY
	B LOOP

DELAY:
	LDR R1, =100000000
L:
	SUB R1, R1, #1
	CMP R1, #0
	BNE L
	MOV PC, LR

LED_CONFIG:
	LDR R2, =0x11000c20
	LDR R1, =0x00000001  @  00000000 00000000 00000000 00000001
	STR R1, [R2]
	MOV PC, LR

LED_ON:
	LDR R2, =0x11000c24
	LDR R1, =0x00000001  @  00000000 00000000 00000000 00000001
	STR R1, [R2]
	MOV PC, LR

LED_OFF:
	LDR R2, =0x11000c24
	LDR R1, =0x00000000
	STR R1, [R2]
	MOV PC, LR

STOP:
	B STOP

.end

