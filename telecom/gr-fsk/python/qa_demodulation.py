#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2021 UCLouvain.
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

from gnuradio import gr, gr_unittest
from gnuradio import blocks

import scipy.io
import numpy as np

from synchronization import synchronization
from preamble_detect import preamble_detect
from demodulation import demodulation
from packet_parser import packet_parser

def gr_cast(x):
    return [complex(z) for z in x]

class qa_demodulation (gr_unittest.TestCase):

    def setUp (self):
        pass

    def tearDown (self):
        pass

    def test_001_t (self):
        fsamp = 400e3
        drate = 50e3
        fdev = drate/2
        hdr_len = 8
        payload_len = 500
        crc_len = 1
        packet_len = hdr_len + payload_len + crc_len

        y = np.fromfile('../misc/fsk_trace.mat', dtype=np.complex64)
        y = y*2
        osr = int(fsamp / drate)
        t = np.arange(len(y)) / fsamp
        cfo_fix = 14750
        y = y * np.exp(-1j * 2 * np.pi * cfo_fix * t)

        
        tb = gr.top_block()
        source = blocks.vector_source_c(gr_cast(y))
        detector = preamble_detect(drate, fdev, fsamp, packet_len)
        sync = synchronization(drate, fdev, fsamp, packet_len)
        demod = demodulation(drate, fdev, fsamp, payload_len, crc_len)
        parser = packet_parser(payload_len, crc_len)


        tb.connect(source, detector)
        tb.connect(detector, sync)
        tb.connect(sync, demod)

        #sink = blocks.vector_sink_b()
        #tb.connect(demod, sink)

        tb.connect(demod, parser)
        sink = blocks.vector_sink_b(payload_len)
        tb.connect(parser, sink)

        sink2 = blocks.vector_sink_c()
        tb.connect(sync, sink2)

        tb.run()
        s_out = sink2.data()
        tb.stop()


if __name__ == '__main__':
    gr_unittest.run(qa_demodulation)
