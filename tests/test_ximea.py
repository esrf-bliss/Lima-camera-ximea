
import pytest
import PyTango
import time
 
def test_bin_read(device):
    try:
        print("bin value is %s" % str(device.image_bin))
        assert True
    except:
        assert False

def test_roi_read(device):
    try:
        print("roi value is %s" % str(device.image_roi))
        assert True
    except:
        assert False

def test_roi_boundaries(device):
    try:
        print("setting a roi in the high pixel area")
        device.image_roi = 4000,4000,200,200
        print("and now setting it big in the low pixel area")
        device.image_roi = 100,100, 4000, 4000
        assert True
    except:
        assert False

def test_roi_zeroes(device):
    device.image_roi = 0,0,0,0
    roi = device.image_roi
    print("setting roi to 0 actually sets roi to: %s" % str(roi))

    if list(roi) != [0,0,6144,6144]:
        assert False
    else:
        
        assert True

def test_stopacq(device):
    """ checks that camera can be stopped after prepareAcq()"""

    device.acq_mode = "SINGLE"
    device.acq_trigger_mode = "INTERNAL_TRIGGER"
    device.acq_nb_frames = 1
    device.acq_expo_time = 1
    device.prepareAcq()
    
    print(" Ok. prepare acq done")

    time.sleep(0.5)
    status = str(device.acq_status)

    print(" camera state is: {}".format(status))
    print(" now stopping without startAcq")

    device.stopAcq()

    t0 = time.time()

    max_wait = 2
    start_wait = time.time()
    while True:
       status = str(device.acq_status).lower()
       print(" camera state is: --{}--".format(status))
       if status.lower() == "ready":
            assert True
            break

       if time.time() - start_wait > max_wait:
            print(" timeout waiting to stop acquisition")
            assert False
            break

       time.sleep(0.5)

