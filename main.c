/*the Cortex M processor has 2 stack pointer registers
 * The Main Stack Pointer  is the default stack pointer used after the processor reset and used for all exceptions
 * and interrupts and for thread mode code. After boot up the processor automatically initializes the MSP by
 * reading the first location of the vector table.
 * The Process Stack Pointer is and alternate stack pointer that can only be used in thread mode. Usually used for
 * for application tasks.
 *
 * In thread mode user has the choice to use SP or PSP stack pointers by default in thread mode SP = MSP.
In order to change value of PSP (a special register) user must use inline assembly.
Before using PSP we need to initialize it. In this example stack space is divided in two parts and use MSP and PSP
to track these two stack space regions.
Thread mode will use the PSP tracked stack space, and Handler mode will use MSP stack pointer

To access MSP and PDP in assembly code we use MSR and MRS instruction:
we use MSR to move the contents of a general-purpose register  into the specified special register.
and use MRS to move the content of a special register to a general purpose register
*/

#include <stdint.h>
#include<stdio.h>
#define SRAM_START 				0x20000000U //defined in linker script ORIGIN
#define SRAM_SIZE  				(128 * 1024) //linker script
#define SRAM_END  				( (SRAM_START) + (SRAM_SIZE) )
#define STACK_START SRAM_END
#define STACK_MSP_START 		 STACK_START
#define STACK_MSP_END   		(STACK_MSP_START - 512)
#define STACK_PSP_START 		STACK_MSP_END


int fun_add(int a, int b , int c , int d)
{
	return a+b+c+d;
}


/*function to change SP to PSP */
__attribute__((naked)) void change_sp_to_psp(void)
{
	__asm volatile(".equ SRAM_END, (0x20000000 + ( 128 * 1024))");
	__asm volatile(".equ PSP_START , (SRAM_END-512)");
	__asm volatile("LDR R0,= PSP_START");
	__asm volatile("MSR PSP, R0");
	__asm volatile("MOV R0,#0X02");
	__asm volatile("MSR CONTROL,R0");
	__asm volatile("BX LR"); //branch indirect = return to main  instruction PC = LR

}

void generate_exception(void)
{
	/* execute SVC instruction to trigger SVC exception  and call SVC handler*/
	__asm volatile("SVC #0X2");
}

int main(void)
{
	change_sp_to_psp();

/* starting here PSP is  used for stack activities */
	int ret;

	ret = fun_add(1, 4, 5, 6);

	printf("result = %d\n",ret);

	generate_exception();

	for(;;);
}


void SVC_Handler(void)
{
	printf(" in SVC_Handler\n");
}
