@echo off
cd ..\..\..\..\DenOfIz
cmake --build --preset Install_Windows_Debug --target install
if %errorlevel% neq 0 (
    echo Build failed
    exit /b %errorlevel%
)

xcopy /s /y /i install\DenOfIz\Install_Windows_Debug\include\* ..\DZEngine\_ThirdParty\DenOfIzGraphics\win32\include\
xcopy /s /y /i install\DenOfIz\Install_Windows_Debug\include\* ..\DZEngine\_ThirdParty\DenOfIzGraphics\osx\include\
xcopy /s /y /i install\DenOfIz\Install_Windows_Debug\include\* ..\DZEngine\_ThirdParty\DenOfIzGraphics\linux\include\
xcopy /s /y /i install\DenOfIz\Install_Windows_Debug\lib\* ..\DZEngine\_ThirdParty\DenOfIzGraphics\win32\lib\
xcopy /s /y /i install\DenOfIz\Install_Windows_Debug\bin\*.dll ..\DZEngine\_ThirdParty\DenOfIzGraphics\win32\bin\

echo DenOfIz updated successfully