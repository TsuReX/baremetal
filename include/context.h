/*
 * context.h
 *
 *  Created on: Apr 15, 2020
 *      Author: vasily
 */

#ifndef INCLUDE_CONTEXT_H_
#define INCLUDE_CONTEXT_H_

#include <stddef.h>

struct base_frame_t {
	uint32_t	r0;
	uint32_t	r1;
	uint32_t	r2;
	uint32_t	r3;
	uint32_t	r12;
	uint32_t	lr;
	uint32_t	pc;
	uint32_t	xpsr;
};

struct extended_frame_t {
	uint32_t	r0;
	uint32_t	r1;
	uint32_t	r2;
	uint32_t	r3;
	uint32_t	r12;
	uint32_t	lr;
	uint32_t	pc;
	uint32_t	xpsr;
	uint32_t	s0;
	uint32_t	s1;
	uint32_t	s2;
	uint32_t	s3;
	uint32_t	s4;
	uint32_t	s5;
	uint32_t	s6;
	uint32_t	s7;
	uint32_t	s8;
	uint32_t	s9;
	uint32_t	s10;
	uint32_t	s11;
	uint32_t	s12;
	uint32_t	s13;
	uint32_t	s14;
	uint32_t	s15;
	uint32_t	fpscr;
	uint32_t	reserved;
};

void nmi_trigger();

#endif /* INCLUDE_CONTEXT_H_ */
