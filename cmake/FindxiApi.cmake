if(MSVC)
	find_library(XiAPI_LIBRARY NAMES xiapi64 PATHS C:/XIMEA/API/xiAPI/)
	find_path(XiAPI_INCLUDE_DIR NAMES xiApi.h PATHS C:/XIMEA/API/xiAPI/)
else()
	find_library(XiAPI_LIBRARY NAMES m3api)
	find_path(XiAPI_INCLUDE_DIR NAMES m3api/xiApi.h)
endif()

mark_as_advanced(XiAPI_LIBRARY)
mark_as_advanced(XiAPI_INCLUDE_DIR)

if(NOT XiAPI_LIBRARY OR NOT XiAPI_INCLUDE_DIR)
	message(FATAL_ERROR "xiAPI not found!")
else()
	message(STATUS "xiAPI include: " ${XiAPI_INCLUDE_DIR})
	message(STATUS "xiAPI library: " ${XiAPI_LIBRARY})
endif()

add_library(xiAPI::xiAPI INTERFACE IMPORTED)
set_property(TARGET xiAPI::xiAPI PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${XiAPI_INCLUDE_DIR})
set_property(TARGET xiAPI::xiAPI PROPERTY INTERFACE_LINK_LIBRARIES ${XiAPI_LIBRARY})