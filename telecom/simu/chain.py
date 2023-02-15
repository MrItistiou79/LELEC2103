import numpy as np

BIT_RATE = 50e3

class Chain:
    name = ""

    ## Communication parameters
    bit_rate = BIT_RATE
    freq_dev = BIT_RATE/4

    osr_tx = 64
    osr_rx = 8

    preamble = [1,0]*16
    sync_word = [0,0,1,1,1,1,1,0,0,0,1,0,1,0,1,0,0,1,0,1,0,1,0,0,1,0,1,1,0,1,1,1]
    
    payload_len = 50 # Number of bits per packet

    ## Simulation parameters
    n_packets = 100 # Number of sent packets

    ## Channel parameters
    sto_val = 0
    sto_range = 10/BIT_RATE # defines the delay range when random

    cfo_val = 0
    cfo_range = 10000 # defines the CFO range when random

    snr_range = np.arange(-10,25)
    
    ## Lowpass filter parameters
    numtaps = 100
    cutoff = BIT_RATE*osr_rx/2.0001 #or 2*BIT_RATE,...
    
    ## Tx methods

    def modulate(self, bits, R):
        """
        Continuous-Phase FSK modulation
        
        Input: bits to modulate in CPFSK, TX oversampling factor R
        Output: x, modulated bit sequence 
        """
        fd = self.freq_dev      # Frequency deviation, Delta_f
        B = self.bit_rate       # B=1/T
        h = 2*fd/B              # Modulation index
    
        x = np.zeros(len(bits) * R, dtype=np.complex64)
        ph = 2 * np.pi * fd * (np.arange(R)/R)/B # Phase of reference waveform

        phase_shifts = np.zeros(len(bits)+1) # To store all phase shifts between symbols
        phase_shifts[0] = 0 # Initial phase
        
        for i,b in enumerate(bits):
            x[i*R:(i+1)*R] = np.exp(1j * phase_shifts[i]) * np.exp(1j * (1 if b else -1) * ph) # Sent waveforms, with starting phase coming from previous symbol
            phase_shifts[i+1] = phase_shifts[i] + h*np.pi *(1 if b else -1) # Update phase to start with for next symbol
    
        return x

    ## Rx methods
    bypass_preamble_detect = False
    def preamble_detect(self, y):
        pass

    bypass_cfo_estimation = False
    def cfo_estimation(self, y):
        pass

    bypass_sto_estimation = False
    def sto_estimation(self, y):
        pass

    def demodulate():
        pass
