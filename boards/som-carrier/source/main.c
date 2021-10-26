/*
 * attiny826.c
 *
 * Created: 10/7/2021 6:35:03 PM
 * Author : ATBAdm
 */

#include <avr/io.h>
#define DELAY_CNT 2000

int main(void)
{
	PORTB.DIRCLR = PIN4_bm;
	/* Switch off GREEN */
	PORTB.OUTSET = PIN2_bm;
	/* Switch on RED */
	PORTB.OUTCLR = PIN3_bm;
	PORTB.DIRSET = PIN2_bm | PIN3_bm;

	PORTA.OUTCLR = 0xFF;
	PORTA.DIRSET = PIN5_bm | PIN7_bm;

	while (1) {
		while (1)
		{
			/* Check button press */
			if ((PORTB.IN & PIN4_bm) == 0x0) {
				/* Enable 3.3V, deassert SYS_RSTn */
				PORTA.OUTSET = PIN5_bm | PIN7_bm;
				/* Switch off RED */
				PORTB.OUTSET = PIN3_bm;
				/* Switch on GREEN */
				PORTB.OUTCLR = PIN2_bm;
				break;
			}
		}

		/* Check button press */
		while ((PORTB.IN & PIN4_bm) == 0x0);

		volatile uint32_t i;
		for (i = 0; i < DELAY_CNT; ++i);

		while (1)
		{
			/* Check button press */
			if ((PORTB.IN & PIN4_bm) == 0x0) {
				/* Disable 3.3V, assert SYS_RSTn */
				PORTA.OUTCLR = PIN5_bm | PIN7_bm;
				/* Switch on RED */
				PORTB.OUTCLR = PIN3_bm;
				/* Switch off GREEN */
				PORTB.OUTSET = PIN2_bm;
				break;
			}
		}
		/* Check button press */
		while ((PORTB.IN & PIN4_bm) == 0x0);

		for (i = 0; i < DELAY_CNT; ++i);
	}
}

