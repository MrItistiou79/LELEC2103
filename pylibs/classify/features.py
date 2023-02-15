import numpy as np
import struct

MELVEC_LENGTH = 20
N_MELVECS = 16
N_SAMPLES_MELVEC = 512
melvec_struct = struct.Struct(f'!{MELVEC_LENGTH}h')

def q15dec(x):
    """Q1.15 integer to float."""
    return x * 2**-15
vq15dec = np.vectorize(q15dec)

def payload2melvecs(payload):
    melvecs = vq15dec(np.array(list(melvec_struct.iter_unpack(payload))))
    assert melvecs.shape[0] == N_MELVECS
    # cast to float32 and reshape to a matrix
    melvecs = np.asarray(melvecs, dtype=np.float32).reshape((N_MELVECS, MELVEC_LENGTH))

    # rotate vectors in position
    # TODO avoid this or re-write it better
    melvecs = np.rot90(melvecs, k=-1, axes=(0,1))
    melvecs = np.fliplr(melvecs)
    return melvecs
