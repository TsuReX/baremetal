#include <stdint.h>

extern uint32_t msp;
extern uint32_t _exc_return;

/*
 * @brief	Сохраняет контекст текущего процесса.
 */
void context_store(void)
{
	/* 0.	msp -= (9 * 4) (36=0x24); Не забыть сохранить EXC_RETURN!!!
	 * 1.	Сохранить регистры r4-r11, psp начиная с адреса из msp;
	 * 2.	На основании анализа EXC_RETURN;
	 *		определить размер сохраняемого контекста
	 *		0x68 + 0x24 или 0x20 + 0x24;
	 * 3.	Сохранить контекст начиная с адреса msp в contexts[proc_num];
	 * 4.	msp += context_size.
	 *
	 * Оптимизировать (1) - сохранять сразу в contexts! */
}

/*
 * @brief	Восстанавливает контекст процесса.
 */
void context_load(void)
{
	/* 1.	msp -= context_size (вычислить на основе contexts[proc_next].exc_return);
	 * 2.	Скопировать по адресу msp контекст из contexts[proc_next];
	 * 3.	Восстановить регистры r4-r11, psp начиная с адреса из msp;
	 * 4.	msp += (9 * 4) (36=0x24); Не забыть загрузить EXC_RETURN!!!
	 *
	 *  Оптимизировать (3) - восстановить  сразу из contexts! */
}

/*
 *
 */
__attribute__((naked, section(".after_vectors")))
void nmi_handler(void)
{
	__asm(	".syntax unified\n"
			"mov %0, lr\n"
			"mov %1, sp\n"
			".syntax divided\n"
			: "=r" (_exc_return), "=r" (msp)
		);

	__asm(	".syntax unified\n"
			"mrs r0, CONTROL\n"
			"mov r1, #0x1\n"
			"and r1, r0\n"

			"ite eq\n"
			"moveq r0, #0x68\n"
			"movne r0, #0x20\n"

			"mov r1, lr\n"
			"mov r2, sp\n"
			"push {lr}\n"
			"bl context_parse\n"
			"pop {lr}\n"
			"bx lr\n"
			".syntax divided\n"
		);
}
