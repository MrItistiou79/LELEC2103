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
	// melvec = vecteur de retour dans lequel on met le résultat de la fft * melvec
	// fft vecteur = 256

	// STEP 1  : Windowing of input samples
	arm_mult_q15(samples, hamming_window, buf, SAMPLES_PER_MELVEC);

	// STEP 2  : Discrete Fourier Transform
	arm_rfft_instance_q15 rfft_inst;
	arm_rfft_init_q15(&rfft_inst, SAMPLES_PER_MELVEC, 0, 1);
	arm_rfft_q15(&rfft_inst, buf, buf_fft);

	//computation of an estimation of the norm
	q15_t alpha = 0.96043387;
	q15_t beta  = 0.39782473;

	for (int i = 0; i < (uint16_t) (SAMPLES_PER_MELVEC/2); i++){
		buf[i] = alpha * MAX(buf_fft[2*i], buf_fft[2*i+1]) + beta * MIN(buf_fft[2*i], buf_fft[2*i + 1]);
	}




	q31_t vmax=0, tmp;
	for (int i=0; i< (uint16_t) (SAMPLES_PER_MELVEC/2); i++) {

		if ( ((q31_t) (ABS(buf_fft[2*i]) + ABS(buf_fft[2*i+1]))) > vmax) {

			tmp = ((q31_t)buf_fft[2*i]*(q31_t)buf_fft[2*i]+(q31_t)buf_fft[2*i+1]*(q31_t)buf_fft[2*i+1]);
			if (tmp>vmax){
				vmax = tmp;
			}
		}
	}
	

	vmax = sqrt(vmax);

	// STEP 3.2: Normalize the vector - Dynamic range increase
	//           Complexity: O(N)
	//           Number of cycles: <TODO>

	for (int i=0; i < SAMPLES_PER_MELVEC; i++) // We don't use the second half of the symmetric spectrum
	{
		buf[i] = (q15_t) (((q31_t) buf_fft[i] << 15) /((q31_t)vmax));
	}
	// ABS max

	// what would be the issue in using a q31_t buffer ? avoids normalization...

	// STEP 3.3: Compute the complex magnitude
	//           --> The output buffer is now two times smaller because (real|imag) --> (mag)
	//           Complexity: O(N)
	//           Number of cycles: <TODO>

	arm_cmplx_mag_q15(buf, buf, SAMPLES_PER_MELVEC/2);

	// STEP 3.4: Denormalize the vector
	//           Complexity: O(N)
	//           Number of cycles: <TODO>

	for (int i=0; i < SAMPLES_PER_MELVEC/2; i++)
	{
		buf[i] = (q15_t) ((((q31_t) buf[i]) * ((q31_t) vmax) ) >> 15 );
	}

	// STEP 4:   Apply MEL transform
	//           --> Fast Matrix Multiplication
	//           Complexity: O(Nmel*N)
	//           Number of cycles: <TODO>

	// /!\ The difference between the function arm_mat_mult_q15() and the fast variant is that the fast variant use a 32-bit rather than a 64-bit accumulator.
	// The result of each 1.15 x 1.15 multiplication is truncated to 2.30 format. These intermediate results are accumulated in a 32-bit register in 2.30 format.
	// Finally, the accumulator is saturated and converted to a 1.15 result. The fast version has the same overflow behavior as the standard version but provides
	// less precision since it discards the low 16 bits of each multiplication result.

	// /!\ In order to avoid overflows completely the input signals should be scaled down. Scale down one of the input matrices by log2(numColsA) bits to avoid overflows,
	// as a total of numColsA additions are computed internally for each output element. Because our hz2mel_mat matrix contains lots of zeros in its rows, this is not necessary.
	
	arm_matrix_instance_q15 hz2mel_inst, fftmag_inst, melvec_inst;

	arm_mat_init_q15(&hz2mel_inst, MELVEC_LENGTH, SAMPLES_PER_MELVEC/2, hz2mel_mat);
	arm_mat_init_q15(&fftmag_inst, SAMPLES_PER_MELVEC/2, 1, buf);
	arm_mat_init_q15(&melvec_inst, MELVEC_LENGTH, 1, melvec);

	arm_mat_mult_fast_q15(&hz2mel_inst, &fftmag_inst, &melvec_inst, buf_tmp);
}
