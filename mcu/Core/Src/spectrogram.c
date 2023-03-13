/*
 * spectrogram.c
 *
 *  Created on: Jun 4, 2021
 *      Author: math
 */

#include <stdio.h>
#include "spectrogram.h"
#include "spectrogram_tables.h"
#include "config.h"
#include "utils.h"
#include "arm_absmax_q15.h"

#define MAX(a, b) 	( a < b ? b : a)
#define MIN(a, b) 	( a > b ? b : a)
#define ABS(a)		( a > 0 ? a : -a)
q15_t buf    [  SAMPLES_PER_MELVEC  ]; // Windowed samples
q15_t buf_fft[2*SAMPLES_PER_MELVEC  ]; // Double size (real|imag) buffer needed for arm_rfft_q15
q15_t buf_tmp[  SAMPLES_PER_MELVEC/2]; // Intermediate buffer for arm_mat_mult_fast_q15

// Convert 12-bit DC ADC samples to Q1.15 fixed point signal and remove DC component
void Spectrogram_Format(q15_t *buf)
{
	// STEP 0.1 : Increase fixed-point scale
	arm_shift_q15(buf, 3, buf, SAMPLES_PER_MELVEC);

	// STEP 0.2 : Remove DC Component
	for(uint16_t i=0; i < SAMPLES_PER_MELVEC; i++) { // Remove DC component
		buf[i] -= (1 << 14);
	}
}

// Compute spectrogram of samples and transform into MEL vectors.
void Spectrogram_Compute(q15_t *samples, q15_t *melvec)
{
	// Windowing of input samples
	arm_mult_q15(samples, hamming_window, buf, SAMPLES_PER_MELVEC);

	// Discrete Fourier Transform
	arm_rfft_instance_q15 rfft_inst;
	arm_rfft_init_q15(&rfft_inst, SAMPLES_PER_MELVEC, 0, 1);
	arm_rfft_q15(&rfft_inst, buf, buf_fft);

	// computation of an estimation of the norm
	q15_t alpha;
	q15_t beta;

	float a = 0.96043;
	float b = 0.39782;

	arm_float_to_q15(&a, &alpha, 1);
	arm_float_to_q15(&b, &beta, 1);

	for (uint16_t i = 0; i < (uint16_t) (SAMPLES_PER_MELVEC/2); i++){
		buf[i] = (q15_t) (alpha * (ABS(MAX(buf_fft[2*i], buf_fft[2*i+1]))) + beta * (ABS(MIN(buf_fft[2*i], buf_fft[2*i + 1]))));
	}

	// matrix multiplication
	arm_matrix_instance_q15 melvec_inst, hz2mel_inst, fftmag_inst, melvec_inst_prov;

	q15_t data = 0;

	//transforms the vectors into matrix
	arm_mat_init_q15(&melvec_inst, MELVEC_LENGTH, 1, melvec);
	arm_mat_init_q15(&melvec_inst_prov, 1, 1, &data);

	for(uint8_t i = 0; i < 20; i++){
		uint16_t curr 	 = *(ind + 2*i);		// indices at which there are non zero elements in the mel matrix
		uint16_t nbr_elem = *(ind + 2*i + 1);	// nbr of non zero elements starting at indice curr

		arm_mat_init_q15(&hz2mel_inst, 1, nbr_elem, hz2mel_mat + curr);
		arm_mat_init_q15(&fftmag_inst, nbr_elem, 1, buf + (curr%256));

		arm_mat_mult_fast_q15(&hz2mel_inst, &fftmag_inst, &melvec_inst_prov, buf_tmp);

		melvec_inst.pData[i] = melvec_inst_prov.pData[0];
		melvec_inst_prov.pData[0] = 0;
	}
}
