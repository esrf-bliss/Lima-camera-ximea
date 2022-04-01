
import pytest
import PyTango

ximea_devel_device = "id16ni/limaccd/ximea"

def pytest_addoption(parser):
    parser.addoption('--device', default=ximea_devel_device, help='device name to run tests on')

@pytest.fixture(scope='session')
def device(pytestconfig):
    devname = pytestconfig.getoption('--device')
    try:
        return PyTango.DeviceProxy(devname)
    except:
        raise ValueError("cannot import device %s" % devname)


