/*
Den Of Iz - Game/Game Engine
Copyright (c) 2020-2024 Muhammed Murat Cengiz

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <stddef.h>
#include "DenOfIzGraphics/Utilities/Interop.h"

namespace DenOfIz
{

    //! \brief Synchronization primitive for command lists.
    //
    class DZ_API ISemaphore
    {
    public:
        virtual void Notify( )            = 0;
        virtual bool IsCompleted( ) const = 0;
        virtual ~ISemaphore( )            = default;
    };

    struct DZ_API ISemaphoreArray
    {
        ISemaphore **Elements;
        size_t       NumElements;
    };
} // namespace DenOfIz
