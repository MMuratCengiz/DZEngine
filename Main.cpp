// Copyright (c) 2025 Muhammed Murat Cengiz
// Licensed under the terms of the root LICENSE file.

#include "DZEngine/App.h"

#if defined _WIN32 && not defined NDEBUG
#include <crtdbg.h>
#endif

using namespace DenOfIz;
using namespace DZEngine;

int main( )
{
#if defined _WIN32 && not defined NDEBUG
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
    _CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_DEBUG );
#endif

    constexpr AppDesc appDesc{ };
    const auto        app = new App( appDesc );
    app->Run( );
    delete app;

    GraphicsApi::ReportLiveObjects( );
    return 0;
}
