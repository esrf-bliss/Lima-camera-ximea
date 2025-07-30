cd tango

cmake -Bbuild -H. -DCMAKE_INSTALL_PREFIX=%LIBRARY_PREFIX% -DPython3_FIND_STRATEGY=LOCATION -DCMAKE_FIND_ROOT_PATH=%LIBRARY_PREFIX%
IF %ERRORLEVEL% NEQ 0 exit /b %ERRORLEVEL%

cmake --build build --config Release --target install
IF %ERRORLEVEL% NEQ 0 exit /b %ERRORLEVEL%
