/*
 * freq_meter.c
 *
 *  Created on: Apr 22, 2020
 *      Author: vasily
 */


#include <freq_meter.h>
#include <time.h>
#include <drivers.h>

struct fm_port_conf_t pconf[] = {
		{4,0,1},
		{4,7,0},
		{4,8,1},
		{4,9,2},
		{4,10,3},
		{4,11,4},
		{4,12,5},
		{4,13,6},
		{4,14,7},
		{-1,-1,7},
};

void fm_init(struct freq_meter_t *fm)
{
	/* Обработать специальный канал таймера - канал для тестирования. */
	if (fm->channel == fm_channel_9) {
		/* Подключить к 7му каналу таймера генератор FRO_HF. */
		SYSCON->SCTCLKSEL = SYSCON_SCTCLKSEL_SEL(0x2);
//		mdelay(1)
		/* Установить делитель частоты. */
		SYSCON->SCTCLKDIV = SYSCON_SCTCLKDIV_DIV(0);
//		mdelay(1);
		/* Отключить от 7го канала таймера внешний вход тактирования. */
		IOCON->PIO[4][14] =	IOCON_PIO_FUNC(0) |
							IOCON_PIO_DIGIMODE(1) |
							IOCON_PIO_FILTEROFF(1) |
							IOCON_PIO_MODE(0);

	} else {
		IOCON->PIO[pconf[fm->channel].gpio_port][pconf[fm->channel].gpio_pin] =	IOCON_PIO_FUNC(5) |
																	IOCON_PIO_DIGIMODE(1) |
																	IOCON_PIO_FILTEROFF(1) |
																	IOCON_PIO_MODE(0);
		if (fm->channel == fm_channel_8) {
			/* Отключить внутреннее тактирование от 7го канала */
			SYSCON->SCTCLKSEL = SYSCON_SCTCLKSEL_SEL(0x7);
		}
	}

}

void fm_free(struct freq_meter_t *fm)
{

	/* Обработать специальный канал таймера - канал для тестирования. */
	if (fm->channel == fm_channel_9) {
		/* Отключить внутреннее тактирование от 7го канала */
		SYSCON->SCTCLKSEL = SYSCON_SCTCLKSEL_SEL(0x7);
		/* Установить делитель частоты 7го канала таймера. */
		SYSCON->SCTCLKDIV = SYSCON_SCTCLKDIV_DIV(1);
		/* Отключить от 7го канала таймера внешний вход тактирования. */
		IOCON->PIO[4][14] =	IOCON_PIO_FUNC(0) |
							IOCON_PIO_DIGIMODE(1) |
							IOCON_PIO_FILTEROFF(1) |
							IOCON_PIO_MODE(0);

	} else {
		IOCON->PIO[pconf[fm->channel].gpio_port][pconf[fm->channel].gpio_pin] =	IOCON_PIO_FUNC(0) |
																	IOCON_PIO_DIGIMODE(1) |
																	IOCON_PIO_FILTEROFF(1) |
																	IOCON_PIO_MODE(0);
	}

}

void fm_clock_start_count(struct freq_meter_t *fm) {

	SYSCON->PRESETCTRLSET[1] = SYSCON_PRESETCTRL_SCT0_RST(1);

    /* wait until it reads 0b1 */
    while (0u == (SYSCON->PRESETCTRL[1] & SYSCON_PRESETCTRL_SCT0_RST(1)))  {
    }

    /* clear bit */
    SYSCON->PRESETCTRLCLR[1] = SYSCON_PRESETCTRL_SCT0_RST(1);

    /* wait until it reads 0b0 */
    while (SYSCON_PRESETCTRL_SCT0_RST(1) == (SYSCON->PRESETCTRL[1] & SYSCON_PRESETCTRL_SCT0_RST(1))) {
    }

    SCT0->CONFIG = SCT_CONFIG_CKSEL(pconf[fm->channel].sct_in << 1) | SCT_CONFIG_CLKMODE(0x1) |
				   SCT_CONFIG_UNIFY(0x1) | SCT_CONFIG_INSYNC(0xF);

	/* Write to the control register, clear the counter and keep the counters halted */
    SCT0->CTRL = SCT_CTRL_BIDIR_L(0x0) | SCT_CTRL_PRE_L(0x0) |
				 SCT_CTRL_CLRCTR_L_MASK | SCT_CTRL_HALT_L_MASK;

	/* Initial state of channel output */
	SCT0->OUTPUT = 0;

	SCT0->CTRL &= ~SCT_CTRL_HALT_L(1);
}

uint32_t fm_clock_stop_count(struct freq_meter_t *fm) {

	fm->last_count_val = SCT0->COUNT;

	SCT0->CTRL = SCT_CTRL_HALT_L(1);

	SCT0->CTRL = SCT_CTRL_CLRCTR_L(1);

    SYSCON->PRESETCTRLSET[1] = SYSCON_PRESETCTRL_SCT0_RST(1);

    /* wait until it reads 0b1 */
    while (0u == (SYSCON->PRESETCTRL[1] & SYSCON_PRESETCTRL_SCT0_RST(1)))  {
    }

	return fm->last_count_val;
}
