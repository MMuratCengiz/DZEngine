#!/bin/bash
cd ../../../../DenOfIz
cmake --build --preset Install_Linux
if [ $? -ne 0 ]; then
    echo "Build failed"
    exit 1
fi

cp -r install/Install_Linux/include/* ../TheKindlingScript/_ThirdParty/DenOfIzGraphics/win32/include/
cp -r install/Install_Linux/include/* ../TheKindlingScript/_ThirdParty/DenOfIzGraphics/osx/include/
cp -r install/Install_Linux/include/* ../TheKindlingScript/_ThirdParty/DenOfIzGraphics/linux/include/
cp -r install/Install_Linux/lib/* ../TheKindlingScript/_ThirdParty/DenOfIzGraphics/linux/lib/

echo "DenOfIz updated successfully"