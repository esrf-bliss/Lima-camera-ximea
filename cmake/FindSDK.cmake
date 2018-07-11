find_path(SDK_INCLUDE_DIRS sdk/sdk.h)
find_library(SDK_LIBRARIES sdk)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SDK
  DEFAULT_MSG
  CBF_LIBRARIES
  CBF_INCLUDE_DIRS)
