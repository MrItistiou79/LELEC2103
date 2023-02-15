#!/usr/bin/env python
"""
Script post-processing the saved outputs from GnuRadio console
"""

import numpy as np
import sys
import matplotlib.pyplot as plt

filename = str(sys.argv[1]) # to give the filename as an argument
print(filename)
# filename = 'output.txt' # if needed, can choose the filename here

SNR_list = []
packet_received_list = []
packet_error_list = []
CFO_list = []

static_CFO_corr = 0 # if you decided to change the carrier frequency in GnuRadio to 'pre-correct' for a fixed frequency offset

## Parse output file
with open(filename, encoding="ISO-8859-1") as openfileobject:
    for line in openfileobject:
        # SYNCHRONIZATION MESSAGE
        if line[0:6] == '[SYNC]' :
            #print(line)
            if line[0:21] =='[SYNC] Estimated SNR:': #recover estimated SNR
                try:
                    SNR = float(line[22:28])
                except:
                    SNR = float(line[22:26])
                #print(SNR)
                SNR_list.append(SNR)
                
            elif line[0:28] == '[SYNC] New preamble detected': #recover estimated CFO
                possibility_prev = ''
                for possibility in line.split():
                    try:
                        number = float(possibility.replace(',', '.'))
                        if not (possibility_prev == '@'):#since several numbers in the same line, make sure extract CFO
                            CFO_list.append(number)
                            #print(number)   
                    except ValueError:
                        pass
                    possibility_prev = possibility
        
        #PER MESSAGE            
        elif line[0:4] == '--- ':
            #print(line)
            list_int = [int(s) for s in line.split() if s.isdigit()]
            packet_received_list.append(list_int[0])
            packet_error_list.append(list_int[1])

            
## Compute main results            
CFO_list = np.array(CFO_list)+static_CFO_corr

SNR_mean = np.mean(SNR_list)
SNR_std = np.std(SNR_list)

print("SNR mean", SNR_mean)
print("SNR std", SNR_std)
print("Packets received", packet_received_list[-1])
print("Packet errors", packet_error_list[-1])
# The PER can be computed from the number of packet errors compared to the number of packets received


# Quick print of CFO histogram based on the CFO estimated on each packet
plt.figure()
plt.hist(CFO_list,50)
plt.show()
