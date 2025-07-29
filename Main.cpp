// Copyright (c) 2025 Muhammed Murat Cengiz
// Licensed under the terms of the root LICENSE file.

#include <DenOfIzGraphics/Backends/Common/GraphicsWindowHandle.h>
#include <DenOfIzGraphics/Backends/GraphicsApi.h>
#include <DenOfIzGraphics/Input/InputSystem.h>
#include "Kindling/KindlingApp.h"

using namespace DenOfIz;
using namespace Kindling;

int main( )
{
    EngineDesc engineDesc{ };
    Engine::Init( engineDesc );

#if defined _WIN32 && not defined NDEBUG
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
    _CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_DEBUG );
#endif
    WindowDesc windowDesc{ };
    windowDesc.Width    = Window::GetDisplayWindowSize( 0 ).Width * 0.8f;
    windowDesc.Height   = Window::GetDisplayWindowSize( 0 ).Height * 0.8f;
    windowDesc.Position = WindowPosition::Centered;
    Window window( windowDesc );

    window.SetResizable( true );

    auto        graphicsWindowHandle = window.GetGraphicsWindowHandle( );
    const auto  app                  = new KindlingApp( KindlingAppDesc{ graphicsWindowHandle } );
    auto        running              = true;
    Event       event;
    InputSystem inputSystem{ };
    while ( running )
    {
        while ( InputSystem::PollEvent( event ) )
        {
            if ( event.Type == EventType::Quit )
            {
                running = false;
            }
            app->HandleEvent( event );
            if ( !app->IsRunning( ) )
            {
                running = false;
            }
        }
        app->Update( );
    }

    delete app;
    GraphicsApi::ReportLiveObjects( );
    Engine::Shutdown( );
    return 0;
}
