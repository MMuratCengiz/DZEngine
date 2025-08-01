#!/bin/bash
cd ../../../../DenOfIz
cmake --build --preset Install_OSX
if [ $? -ne 0 ]; then
    echo "Build failed"
    exit 1
fi

cp -r install/Install_OSX/include/* ../DZEngine/_ThirdParty/DenOfIzGraphics/win32/include/
cp -r install/Install_OSX/include/* ../DZEngine/_ThirdParty/DenOfIzGraphics/osx/include/
cp -r install/Install_OSX/include/* ../DZEngine/_ThirdParty/DenOfIzGraphics/linux/include/
cp -r install/Install_OSX/lib/* ../DZEngine/_ThirdParty/DenOfIzGraphics/osx/lib/

echo "DenOfIz updated successfully"