
#include "maxm86161.h"
#include "pulsometr.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define SimplMAX 350
//float bufPuls[500];
static uint16_t BuffSize;
uint16_t ir_max, ir_min;
static uint16_t red_max, red_min;
static uint16_t SyncIdx = 0;

static uint16_t SampleCount = 0;
static uint16_t PrevSample = 0;
static uint16_t BuffSize=SimplMAX;
static uint16_t SampleRate=SimplMAX;

uint16_t ir_input_buff[SimplMAX+2];
uint16_t red_input_buff[SimplMAX+2];
uint16_t ir_stage1_buff[SimplMAX+2];
uint16_t ir_stage2_buff[SimplMAX+2];
uint16_t ir_stage3_buff[SimplMAX+2];
//static uint16_t *red_input_buff;
uint16_t red_stage1_buff[SimplMAX+2];
uint16_t red_stage2_buff[SimplMAX+2];
 float HeartRate,HeartRate2;
 uint16_t HeartRate_Result;
uint16_t ir_delta;
uint16_t red_delta;
    uint16_t Idx1, Idx2;
	uint16_t Idx22;
	
#define MARK_VALUE		30000

//************************************************
//void readLastSamples(uint16_t *ir_buff, uint16_t *red_buff, uint16_t samplesNum)
//{
//	uint16_t LastSample = WrIdx ? WrIdx - 1: BuffSize - 1;

//	while (samplesNum--)
//	{
//		uint16_t Idx = LastSample + BuffSize - samplesNum;
//		while (Idx >= BuffSize)
//			Idx -= BuffSize;

//		*(ir_buff++) = cycle_buff[Idx].ir;
//		*(red_buff++) = cycle_buff[Idx].red;
//	}
//}
//************************************************
uint16_t max30100_calcMaxMinIdx(uint16_t *buff, uint16_t idx1, uint16_t idx2, uint16_t *pmax, uint16_t *pmin)
{
	*pmax = *pmin = *buff;
	uint16_t idx = idx1;
	while (idx <= idx2)
	{
		if (buff[idx] < *pmin)
			*pmin = buff[idx];
		if (buff[idx] > *pmax)
			*pmax = buff[idx];
		idx++;
	}

	return *pmax - *pmin;
}
//************************************************
static void normaliseBuff(uint16_t *buffIn, uint16_t *buffOut, uint16_t samples, uint16_t max, uint16_t min)
{
	uint16_t delta = max - min;
	while (samples--)
	{
		uint32_t temp = *buffIn - min;
		temp = temp * 65535 / delta;
		*buffOut = (uint16_t) temp;
		buffIn++;
		buffOut++;
	}
}

//************************************************
uint16_t max30100_calcMaxMin(uint16_t *buff, uint16_t samples, uint16_t *pmax, uint16_t *pmin)
{
	max30100_calcMaxMinIdx(buff, 1, samples - 1, pmax, pmin);
	if (*pmax - *pmin < 500)
		*pmax = *pmin + 500;

	return *pmax - *pmin;
}
//************************************************
static void filterBuff(uint16_t *buff_in, uint16_t *buff_out, uint16_t samples, uint8_t window)
{
	if ((window % 2) == 0)
		window++;

	uint8_t hw = (window - 1) / 2;
	buff_out[0] = buff_in[0];

	for (uint16_t i = 1; i < samples; i++)
	{
		uint32_t sum = 0;
		uint16_t k1, k2;
		uint8_t samples_cnt;

		if (i < hw)
	    {
	    	k1 = 0;
	        k2 = 2 * i;
	        samples_cnt = k2 + 1;
	    }
	    else if ((i + hw) > (samples - 1))
	    {
	    	k1 = i - samples + i + 1;
	        k2 = samples - 1;
	        samples_cnt = k2 - k1 + 1;
	    }
	    else
	    {
	    	k1 = i - hw;
	        k2 = i + hw;
	        samples_cnt = window;
	    }

	    for (uint16_t j = k1; j <= k2; j++)
	    	sum += buff_in[j];

	    buff_out[i] = sum / samples_cnt;
	}
}
//************************************************
static void calcDiffBuff(uint16_t *buff_in, uint16_t *buff_out, uint16_t samples, uint8_t step, uint16_t offset)
{
	for (uint16_t i = 1; i < samples; i++)
	{
		if (i >= step)
			buff_out[i] = buff_in[i] + offset - buff_in[i - step];
		else
			buff_out[i] = buff_in[i] + offset - buff_in[0];
	}
}
//************************************************
static float getBeat(uint16_t *buff, uint16_t samples, uint16_t *pBeatIdx1, uint16_t *pBeatIdx2)
{
	uint8_t firstBeat = 1;
	uint16_t startIdx = SampleRate / 15;
	PrevSample = buff[startIdx++];
	*pBeatIdx1 = *pBeatIdx2 = 0;

	for (uint16_t i = startIdx; i < samples; i++)
	{
		uint16_t newValue = buff[i];
		if ((newValue < PrevSample) && (PrevSample - newValue > 50000))	// ?????? ?????
		{

			if (firstBeat)
			{
				firstBeat = 0;
				SampleCount = 0;
				SyncIdx = i;
				*pBeatIdx1 = i;
			}
			else
			{
				if (SampleCount)
				{
					*pBeatIdx2 = i;
					return (60 * SampleRate) / SampleCount;
				}
			}
		}

		SampleCount++;
		PrevSample = newValue;
	}

	SyncIdx = 0;
	return 0;
}
//************************************************+
static float getBeat2(uint16_t *buff, uint16_t samples, uint16_t *pBeatIdx1, uint16_t *pBeatIdx2)
{
	uint8_t firstBeat = 1;
	uint16_t startIdx = SampleRate / 15;
	*pBeatIdx1  = 0;

	for (uint16_t i = startIdx; i < samples; i++)
	{
		if (buff[i] == MARK_VALUE)
		{
			if (firstBeat)
			{
				firstBeat = 0;
				SampleCount = 0;
				SyncIdx = i;
				*pBeatIdx1 = i;
			}
			else
			{
				if (SampleCount)
				{
					*pBeatIdx2 = i;
					return (60 * SampleRate) / SampleCount;
				}
			}
		}

		SampleCount++;
	}

	SyncIdx = 0;
	return 0;
}
//************************************************+
void markExtremums(uint16_t *buff, uint16_t samples)
{
	uint16_t Idx1 = 0, Idx2 = 0;
	uint16_t startIdx = SampleRate / 15;
	PrevSample = buff[startIdx++];
	for (uint16_t i = startIdx; i < samples; i++)
	{
		uint16_t newValue = buff[i];
		if ((newValue > PrevSample) && (newValue - PrevSample > 50000))	// ???????? ?????
		{
			Idx1 = i;
		}
		if ((newValue < PrevSample) && (PrevSample - newValue > 50000))	// ?????? ?????
		{
			Idx2 = i;

			for (uint16_t j = Idx1; j < (Idx2 - 1); j++)
			{
				if (buff[j + 1] > buff[j])
				{
					buff[j] = MARK_VALUE;
					break;
				}
			}
		}
		PrevSample = newValue;
	}
}

//************************************************+

float calculate(uint8_t mode)
{
//	if ((!ir_input_buff) || (!ir_stage1_buff) || (!ir_stage2_buff) || (!ir_stage3_buff) || (!red_input_buff) || (!red_stage1_buff) || (!red_stage2_buff))
//		return 0;

	uint16_t samples = BuffSize;
//	readLastSamples(ir_input_buff, red_input_buff, samples);

	uint16_t filterWidth = SampleRate / 15;		// 200/15  - 50/15
	uint16_t diffStep = SampleRate / 30;//40;		// 5;

	ir_delta = max30100_calcMaxMin(ir_input_buff, samples, &ir_max, &ir_min);
	normaliseBuff(ir_input_buff, ir_stage1_buff, samples, ir_max, ir_min);
	filterBuff(ir_stage1_buff, ir_stage2_buff, samples, filterWidth);
	calcDiffBuff(ir_stage2_buff, ir_stage3_buff, samples, diffStep, ir_delta * 3);

    red_delta = max30100_calcMaxMin(red_input_buff, samples, &red_max, &red_min);
	normaliseBuff(red_input_buff, red_stage1_buff, samples, red_max, red_min);
    filterBuff(red_stage1_buff, red_stage2_buff, samples, filterWidth);

    // ?????? ??????? ????????????

	if (mode >= 5)
	{
		markExtremums(ir_stage3_buff, samples);
	    HeartRate = getBeat2(ir_stage3_buff, samples, &Idx1, &Idx2);
	}
	else
	    HeartRate = getBeat(ir_stage3_buff, samples, &Idx1, &Idx2);

    uint16_t ir_min2, ir_max2, red_min2, red_max2;
    ir_delta = max30100_calcMaxMinIdx(ir_input_buff, Idx1, Idx2, &ir_max2, &ir_min2);
    red_delta = max30100_calcMaxMinIdx(red_input_buff, Idx1, Idx2, &red_max2, &red_min2);

    // ?????? SpO2 ?? AN6409
//    if ((red_min2 != 0.0) && (ir_delta != 0.0))
//    {
//    	R = red_delta * ir_min2;
//    	R /= red_min2;
//    	R /= ir_delta;
//    	SpO2 = 104 - (17 * R);
//    }

    return HeartRate;
}
//************************************************

void DataReadPuls(void)
{
	int counter_smp = 0;
	int counter = 0;
	HeartRate_Result = 0;
	while (counter_smp < 3) {
		for (counter = 0;counter < SimplMAX + 1; counter++) {

			//if (HAL_GPIO_ReadPin(INT_HRV_GPIO_Port, INT_HRV_Pin) == 1) {
			uint8_t int_status =  maxm86161_i2c_read_from_register(MAXM86161_REG_IRQ_STATUS1);
			if(int_status & MAXM86161_INT_1_FULL) {
				maxm86161_ppg_sample_t sample;
				int  data_ready = maxm86161_read_samples_in_fifo(&sample);
				if(data_ready)
					if(counter!=0) {
						ir_input_buff[counter-1]=sample.ppg1;

						red_input_buff[counter-1]=sample.ppg3;
						//  printf("\n%du,%du,%du,", sample.ppg1, sample.ppg2, sample.ppg3);
						}
			}
			//  }
			//   osDelay(3);
			//counter++;
		}
		HeartRate2=	calculate(5)/2.3;
		//	HeartRate2=	calculate(2)/3;
		if((HeartRate2>40) &&(HeartRate2<160)) {
			HeartRate_Result=HeartRate_Result+HeartRate2;
			counter_smp++;
		}
	}
	HeartRate_Result=HeartRate_Result/3;

#warning Get heart rate and make output
//	PulsLivel=HeartRate_Result;
}
