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
	// STEP 1  : Windowing of input samples

		arm_mult_q15(samples, hamming_window, buf, SAMPLES_PER_MELVEC);

		// STEP 2  : Discrete Fourier Transform
		arm_rfft_instance_q15 rfft_inst;
		arm_rfft_init_q15(&rfft_inst, SAMPLES_PER_MELVEC, 0, 1);
		arm_rfft_q15(&rfft_inst, buf, buf_fft);

//
//		arm_matrix_instance_q15 hz2mel_inst, fftmag_inst, melvec_inst;
//
//		arm_mat_init_q15(&hz2mel_inst, MELVEC_LENGTH, SAMPLES_PER_MELVEC/2, hz2mel_mat);
//		arm_mat_init_q15(&fftmag_inst, SAMPLES_PER_MELVEC/2, 1, buf);
//		arm_mat_init_q15(&melvec_inst, MELVEC_LENGTH, 1, melvec);
//
//		arm_mat_mult_fast_q15(&hz2mel_inst, &fftmag_inst, &melvec_inst, buf_tmp);

	//------------------------------------------------------
	// new method : start
	//------------------------------------------------------

	//computation of an estimation of the norm
	for (int i = 0; i < (uint16_t) (SAMPLES_PER_MELVEC/2); i++){
		buf[i] = (q15_t) (0.96043387 * ABS(MAX(buf_fft[2*i], buf_fft[2*i+1])) + 0.39782473 * ABS(MIN(buf_fft[2*i], buf_fft[2*i + 1])));
	}

	arm_matrix_instance_q15 melvec_inst;

	//transforms the vectors into matrix
	arm_mat_init_q15(&melvec_inst, MELVEC_LENGTH, 1, melvec);



	for(uint8_t i; i < 20; i++){
		uint8_t curr 	 = *(ind + 2*i);
		uint8_t nbr_elem = *(ind + 2*i + 1);

		 //computation of the ind elem of the melvec
		for(uint8_t j; j < nbr_elem; j++){
			melvec_inst.pData[i] += buf[curr%256 + j] * hz2mel_mat[curr + j];
		}
	}
}
