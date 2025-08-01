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

#include "Widget.h"

using namespace DenOfIz;

namespace DenOfIz
{
    using DropdownStyle = ClayDropdownDesc;

    class DropdownWidget : public Widget
    {
        StringArray             m_options;
        std::vector<StringView> m_optionsStorage;
        int32_t                 m_selectedIndex    = -1;
        bool                    m_isOpen           = false;
        bool                    m_selectionChanged = false;
        float                   m_scrollOffset     = 0.0f;
        DropdownStyle           m_style;
        uint32_t                m_dropdownListId;
        bool                    m_dropdownListCreatedThisFrame = false;

    public:
        DZ_API DropdownWidget( IClayContext *clayContext, uint32_t id, const StringArray &options, const DropdownStyle &style = { } );
        DZ_API ~DropdownWidget( ) override;

        DZ_API void Update( float deltaTime ) override;
        DZ_API void CreateLayoutElement( ) override;
        DZ_API void Render( const ClayBoundingBox &boundingBox, IRenderBatch *renderBatch ) override;
        DZ_API void HandleEvent( const Event &event ) override;

        DZ_API int32_t     GetSelectedIndex( ) const;
        DZ_API void        SetSelectedIndex( int32_t index );
        DZ_API const char *GetSelectedText( ) const;
        DZ_API bool        WasSelectionChanged( ) const;
        DZ_API void        ClearSelectionChangedEvent( );
        DZ_API bool        IsOpen( ) const;
        DZ_API void        SetOpen( bool open );

        DZ_API void               SetOptions( const StringArray &options );
        DZ_API const StringArray &GetOptions( ) const;

        DZ_API void                 SetStyle( const DropdownStyle &style );
        DZ_API const DropdownStyle &GetStyle( ) const;

        DZ_API void RenderDropdownList( );

    private:
        void UpdateOptions( StringArray options );
    };

} // namespace DenOfIz
