/*
 * spectrogram.c
 *
 *  Created on: Jun 4, 2021
 *      Author: math
 */

/**
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

	//int ind = 0;
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

**/


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

q15_t buf    [  SAMPLES_PER_MELVEC  ]; // Windowed samples
q15_t buf_fft[2*SAMPLES_PER_MELVEC  ]; // Double size (real|imag) buffer needed for arm_rfft_q15
q15_t buf_tmp[  SAMPLES_PER_MELVEC/2]; // Intermediate buffer for arm_mat_mult_fast_q15

// Convert 12-bit DC ADC samples to Q1.15 fixed point signal and remove DC component
void Spectrogram_Format(q15_t *buf)
{
	// STEP 0.1 : Increase fixed-point scale
	//            --> Pointwise shift
	//            Complexity: O(N)
	//            Number of cycles: <TODO>

	// The output of the ADC is stored in an unsigned 12-bit format, so buf[i] is in [0 , 2**12 - 1]
	// In order to better use the scale of the signed 16-bit format (1 bit of sign and 15 integer bits), we can multiply by 2**(15-12) = 2**3
	// That way, the value of buf[i] is in [0 , 2**15 - 1]

	// /!\ When multiplying/dividing by a power 2, always prefer shifting left/right instead, ARM instructions to do so are more efficient.
	// Here we should shift left by 3.

	arm_shift_q15(buf, 3, buf, SAMPLES_PER_MELVEC);

	// STEP 0.2 : Remove DC Component
	//            --> Pointwise substract
	//            Complexity: O(N)
	//            Number of cycles: <TODO>

	// Since we use a signed representation, we should now center the value around zero, we can do this by substracting 2**14.
	// Now the value of buf[i] is in [-2**14 , 2**14 - 1]

	for(uint16_t i=0; i < SAMPLES_PER_MELVEC; i++) { // Remove DC component
		buf[i] -= (1 << 14);
	}
}

// Compute spectrogram of samples and transform into MEL vectors.
void Spectrogram_Compute(q15_t *samples, q15_t *melvec)
{
	// STEP 1  : Windowing of input samples
	//           --> Pointwise product
	//           Complexity: O(N)
	//           Number of cycles: <TODO>
	arm_mult_q15(samples, hamming_window, buf, SAMPLES_PER_MELVEC);

	// STEP 2  : Discrete Fourier Transform
	//           --> In-place Fast Fourier Transform (FFT) on a real signal
	//           --> For our spectrogram, we only keep only positive frequencies (symmetry) in the next operations.
	//           Complexity: O(Nlog(N))
	//           Number of cycles: <TODO>

	// Since the FFT is a recursive algorithm, the values are rescaled in the function to ensure that overflow cannot happen.
	arm_rfft_instance_q15 rfft_inst;

	arm_rfft_init_q15(&rfft_inst, SAMPLES_PER_MELVEC, 0, 1);

	arm_rfft_q15(&rfft_inst, buf, buf_fft);

	// STEP 3  : Compute the complex magnitude of the FFT
	//           Because the FFT can output a great proportion of very small values,
	//           we should rescale all values by their maximum to avoid loss of precision when computing the complex magnitude
	//           In this implementation, we use integer division and multiplication to rescale values, which are very costly.

	// STEP 3.1: Find the extremum value (maximum of absolute values)
	//           Complexity: O(N)
	//           Number of cycles: <TODO>

	q31_t vmax=0, tmp;
	uint32_t pIndex=0;
	for (int i=0; i< (uint16_t) (SAMPLES_PER_MELVEC/2); i++) {
		tmp = ((q31_t)buf_fft[2*i]*(q31_t)buf_fft[2*i]+(q31_t)buf_fft[2*i+1]*(q31_t)buf_fft[2*i+1]);
		if (tmp>vmax){
			vmax = tmp;
			pIndex = i;
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


//TOKEN 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
