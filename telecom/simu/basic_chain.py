import numpy as np
from sim import run_sim
from chain import Chain

class BasicChain(Chain):
    name = "Basic Tx/Rx chain"

    cfo_val, sto_val = np.nan, np.nan # CFO and STO are random


    bypass_preamble_detect = True
    def preamble_detect(self, y):
        """
        Detect a preamble computing the received energy (average on a window)
        """

        L = 4*self.osr_rx
        y_abs = np.abs(y)

        for i in range(0, int(len(y)/L)):
            sum_abs = np.sum(y_abs[i*L:(i+1)*L])
            if sum_abs > (L-1): #fix threshold
                return i*L
        
        return None

    bypass_cfo_estimation = True
    def cfo_estimation(self, y):
        """
        Estimates CFO using Moose algorithm, on first samples of preamble
        
        Input: y (numpy array, complex 128) is a sequence of CPFSK modulated symbols, with an oversampling factor R.
                First samples of y are in the preamble
                
        Output: cfo_est (float 64) is the estimated CFO using the Moose algorithm on the first samples of y
        """
        
        N = 4               # Number of CPFSK symbols in one block
        R = self.osr_rx     # Receiver oversampling factor
        B = self.bit_rate   # B=1/T
        
        # TO DO: extract 2 blocks of size N*R at the start of y
        
        # TO DO: apply the Moose algorithm on these two blocks to estimate the CFO

        cfo_est = 0 # Default value, to change
        
        return cfo_est

    bypass_sto_estimation = True
    def sto_estimation(self, y):
        """
        Estimates symbol timing (fractional) based on phase shifts
        """
        
        R = self.osr_rx
        
        # Computation of derivatives of phase function
        phase_function = np.unwrap(np.angle(y))
        phase_derivative_1 = (phase_function[1:]-phase_function[:-1])
        phase_derivative_2 = np.abs(phase_derivative_1[1:]-phase_derivative_1[:-1])
        
        sum_der_saved = -np.inf
        save_i = 0
        for i in range(0,R):
            sum_der = np.sum(phase_derivative_2[i::R]) # Sum every R samples

            if sum_der > sum_der_saved:
                sum_der_saved = sum_der
                save_i = i
                
        return np.mod(save_i+1, R)

    def demodulate(self, y):
        """
        Non-coherent demodulator
        
        Input: y (numpy array, complex 128) is a sequence of CPFSK modulated symbols, with an oversampling factor R
        
        Output: bits_hat (numpy array, int 32) is the sequence of demodulated bits
        """
        
        fd = self.freq_dev          # Frequency deviation, Delta_f
        B = self.bit_rate           # B=1/T
        R = self.osr_rx             # Receiver oversampling factor
        nb_syms = int(len(y)/R)     # Number of CPFSK symbols in y
        
        # Group symbols together, in a matrix. Each row contains the R samples over one symbol period
        y = np.resize(y, (nb_syms, R)) 
        
        
        # TO DO: generate the reference waveforms used for the correlation
        # hint: look at what is done in modulate() in chain.py
        
        # TO DO: compute the correlations with the two reference waveforms (r0 and r1)
        
        # TO DO: performs the decision based on r0 and r1
        
        
        bits_hat = np.zeros(nb_syms) # Default value, all bits=0. TO CHANGE!
        
        # Return
        bits_hat = bits_hat.astype(int)
        return bits_hat
      
c = BasicChain()
run_sim(c)