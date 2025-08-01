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

#include <DenOfIzGraphics/Utilities/Interop.h>
#include <DenOfIzGraphics/Utilities/InteropMath.h>
#include <DenOfIzGraphics/UI/UIMeasurements.h>
#include <cfloat>

namespace DenOfIz
{

    enum class ClayPointerState : uint8_t
    {
        Pressed,
        Released
    };

    enum class ClayLayoutDirection : uint8_t
    {
        LeftToRight,
        TopToBottom
    };

    enum class ClayAlignmentX : uint8_t
    {
        Left,
        Right,
        Center
    };

    enum class ClayAlignmentY : uint8_t
    {
        Top,
        Bottom,
        Center
    };

    enum class ClaySizingType : uint8_t
    {
        Fit,
        Grow,
        Percent,
        Fixed
    };

    enum class ClayTextWrapMode : uint8_t
    {
        Words,
        Newlines,
        None
    };

    enum class ClayTextAlignment : uint8_t
    {
        Left,
        Center,
        Right
    };

    enum class ClayFloatingAttachPoint : uint8_t
    {
        LeftTop,
        LeftCenter,
        LeftBottom,
        CenterTop,
        CenterCenter,
        CenterBottom,
        RightTop,
        RightCenter,
        RightBottom
    };

    enum class ClayFloatingAttachTo : uint8_t
    {
        None,
        Parent,
        ElementWithId,
        Root
    };

    enum class ClayRenderCommandType : uint8_t
    {
        None,
        Rectangle,
        Border,
        Text,
        Image,
        ScissorStart,
        ScissorEnd,
        Custom
    };

    struct DZ_API ClayDimensions
    {
        float Width;
        float Height;
    };

    struct DZ_API ClayColor
    {
        float R, G, B, A;

        ClayColor( ) : R( 0 ), G( 0 ), B( 0 ), A( 0 )
        {
        }
        ClayColor( const float r, const float g, const float b, const float a ) : R( r ), G( g ), B( b ), A( a )
        {
        }

        Float4 ToFloat4( ) const
        {
            return Float4{ R / 255.0f, G / 255.0f, B / 255.0f, A / 255.0f };
        }
    };

    struct DZ_API ClayBoundingBox
    {
        float X, Y, Width, Height;
    };

    struct DZ_API ClayCornerRadius
    {
        UIPoints TopLeft;
        UIPoints TopRight;
        UIPoints BottomLeft;
        UIPoints BottomRight;

        ClayCornerRadius( ) : TopLeft( 0 ), TopRight( 0 ), BottomLeft( 0 ), BottomRight( 0 )
        {
        }
        explicit ClayCornerRadius( const UIPoints radius ) : TopLeft( radius ), TopRight( radius ), BottomLeft( radius ), BottomRight( radius )
        {
        }
        ClayCornerRadius( UIPoints tl, UIPoints tr, UIPoints bl, UIPoints br ) : TopLeft( tl ), TopRight( tr ), BottomLeft( bl ), BottomRight( br )
        {
        }
    };

    struct DZ_API ClayBorderWidth
    {
        UIPoints Left;
        UIPoints Right;
        UIPoints Top;
        UIPoints Bottom;
        UIPoints BetweenChildren;

        ClayBorderWidth( ) : Left( 0 ), Right( 0 ), Top( 0 ), Bottom( 0 ), BetweenChildren( 0 )
        {
        }
        explicit ClayBorderWidth( const UIPoints width ) : Left( width ), Right( width ), Top( width ), Bottom( width ), BetweenChildren( 0 )
        {
        }
        ClayBorderWidth( UIPoints l, UIPoints r, UIPoints t, UIPoints b, UIPoints between ) : Left( l ), Right( r ), Top( t ), Bottom( b ), BetweenChildren( between )
        {
        }
    };

    struct DZ_API ClayPadding
    {
        UIPoints Left;
        UIPoints Right;
        UIPoints Top;
        UIPoints Bottom;

        ClayPadding( ) : Left( 0 ), Right( 0 ), Top( 0 ), Bottom( 0 )
        {
        }
        explicit ClayPadding( const UIPoints padding ) : Left( padding ), Right( padding ), Top( padding ), Bottom( padding )
        {
        }
        ClayPadding( UIPoints l, UIPoints r, UIPoints t, UIPoints b ) : Left( l ), Right( r ), Top( t ), Bottom( b )
        {
        }
    };

    // Point-based UI structures (industry standard approach)
    struct DZ_API ClayDimensionsPoints
    {
        UIPoints Width;
        UIPoints Height;
        
        ClayDimensionsPoints() : Width(0.0f), Height(0.0f) {}
        ClayDimensionsPoints(UIPoints w, UIPoints h) : Width(w), Height(h) {}
    };

    struct DZ_API ClayBoundingBoxPoints
    {
        UIPoints X, Y, Width, Height;
        
        ClayBoundingBoxPoints() : X(0.0f), Y(0.0f), Width(0.0f), Height(0.0f) {}
        ClayBoundingBoxPoints(UIPoints x, UIPoints y, UIPoints w, UIPoints h) : X(x), Y(y), Width(w), Height(h) {}
    };

    struct DZ_API ClayPaddingPoints  
    {
        UIPoints Left;
        UIPoints Right;
        UIPoints Top;
        UIPoints Bottom;

        ClayPaddingPoints() : Left(0.0f), Right(0.0f), Top(0.0f), Bottom(0.0f) {}
        explicit ClayPaddingPoints(UIPoints padding) : Left(padding), Right(padding), Top(padding), Bottom(padding) {}
        ClayPaddingPoints(UIPoints l, UIPoints r, UIPoints t, UIPoints b) : Left(l), Right(r), Top(t), Bottom(b) {}
    };

    struct ClaySizingAxis
    {
        ClaySizingType Type;
        union
        {
            struct
            {
                float Min;
                float Max;  
            } MinMaxFloats;  // Use floats directly in union to avoid constructor issues
            float Percent;
        } Size;
        
        // Helper methods to work with points while storing as floats
        UIPoints GetMinPoints() const { return Size.MinMaxFloats.Min; }
        UIPoints GetMaxPoints() const { return Size.MinMaxFloats.Max; }
        void SetMinPoints(UIPoints points) { Size.MinMaxFloats.Min = points.value; }
        void SetMaxPoints(UIPoints points) { Size.MinMaxFloats.Max = points.value; }
        
        // Constructor to properly initialize union
        ClaySizingAxis() : Type(ClaySizingType::Fit) {
            Size.MinMaxFloats.Min = 0.0f;
            Size.MinMaxFloats.Max = FLT_MAX;
        }

        DZ_API static ClaySizingAxis Fit( UIPoints min = 0.0f, UIPoints max = FLT_MAX );
        DZ_API static ClaySizingAxis Grow( UIPoints min = 0.0f, UIPoints max = FLT_MAX );
        DZ_API static ClaySizingAxis Fixed( UIPoints size );
        DZ_API static ClaySizingAxis Percent( float percent );
    };

    struct DZ_API ClaySizing
    {
        ClaySizingAxis Width;
        ClaySizingAxis Height;
    };

    struct DZ_API ClayChildAlignment
    {
        ClayAlignmentX X;
        ClayAlignmentY Y;

        ClayChildAlignment( ) : X( ClayAlignmentX::Left ), Y( ClayAlignmentY::Top )
        {
        }
    };

    struct DZ_API ClayLayoutDesc
    {
        ClaySizing          Sizing;
        ClayPadding         Padding;
        uint16_t            ChildGap;
        ClayChildAlignment  ChildAlignment;
        ClayLayoutDirection LayoutDirection;

        ClayLayoutDesc( );
    };

    struct DZ_API ClayTextDesc
    {
        ClayColor         TextColor          = ClayColor( 0, 0, 0, 255 );
        uint16_t          FontId             = 0;
        uint16_t          FontSize           = 16;
        uint16_t          LetterSpacing      = 0;
        uint16_t          LineHeight         = 0;
        ClayTextWrapMode  WrapMode           = ClayTextWrapMode::Words;
        ClayTextAlignment TextAlignment      = ClayTextAlignment::Left;
        bool              HashStringContents = false;

        ClayTextDesc( );
    };

    struct DZ_API ClayImageDesc
    {
        Byte          *ImageData;
        ClayDimensions SourceDimensions;
    };

    struct DZ_API ClayFloatingDesc
    {
        Float2                 Offset;
        ClayDimensions          Expand;
        float                   ZIndex;
        uint32_t                ParentId;
        ClayFloatingAttachPoint ElementAttachPoint;
        ClayFloatingAttachPoint ParentAttachPoint;
        ClayFloatingAttachTo    AttachTo = ClayFloatingAttachTo::None;
    };

    struct DZ_API ClayBorderDesc
    {
        ClayColor       Color;
        ClayBorderWidth Width;
    };

    struct DZ_API ClayScrollDesc
    {
        bool Horizontal;
        bool Vertical;
    };

    struct DZ_API ClayCustomDesc
    {
        void *CustomData;
    };

    struct DZ_API ClayElementDeclaration
    {
        uint32_t         Id;
        ClayLayoutDesc   Layout;
        ClayColor        BackgroundColor;
        ClayCornerRadius CornerRadius;
        ClayImageDesc    Image;
        ClayFloatingDesc Floating;
        ClayCustomDesc   Custom;
        ClayScrollDesc   Scroll;
        ClayBorderDesc   Border;
        void            *UserData;

        ClayElementDeclaration( );
    };

    struct DZ_API ClayRectangleRenderData
    {
        ClayColor        BackgroundColor;
        ClayCornerRadius CornerRadius;
    };

    struct DZ_API ClayTextRenderData
    {
        InteropString StringContents;
        ClayColor     TextColor;
        uint16_t      FontId;
        uint16_t      FontSize;
        uint16_t      LetterSpacing;
        uint16_t      LineHeight;
    };

    struct DZ_API ClayImageRenderData
    {
        ClayColor        BackgroundColor;
        ClayCornerRadius CornerRadius;
        ClayDimensions   SourceDimensions;
        void            *ImageData;
    };

    struct DZ_API ClayBorderRenderData
    {
        ClayColor        Color;
        ClayCornerRadius CornerRadius;
        ClayBorderWidth  Width;
    };

    struct DZ_API ClayCustomRenderData
    {
        ClayColor        BackgroundColor;
        ClayCornerRadius CornerRadius;
        void            *CustomData;
    };

    struct DZ_API ClayScrollRenderData
    {
        bool Horizontal;
        bool Vertical;
    };

    enum class ClayTextFieldType : uint8_t
    {
        SingleLine,
        MultiLine,
        Password
    };

    struct DZ_API ClayTextFieldState
    {
        InteropString Text;
        size_t        CursorPosition  = 0;
        size_t        SelectionStart  = 0;
        size_t        SelectionEnd    = 0;
        bool          IsFocused       = false;
        bool          HasSelection    = false;
        float         CursorBlinkTime = 0.0f;
        bool          CursorVisible   = true;
        bool          IsSelecting     = false;
        size_t        DragStartPos    = 0;
        size_t        SelectionAnchor = 0; // The fixed end of the selection when using Shift+arrows

        InteropString GetSelectedText( ) const;
        void          ClearSelection( );
        void          DeleteSelection( );

        // Helper methods for string operations
        bool          IsTextEmpty( ) const;
        size_t        GetTextLength( ) const;
        void          InsertText( size_t position, const InteropString &text );
        void          EraseText( size_t position, size_t count );
        InteropString GetTextSubstring( size_t start, size_t length ) const;
    };

    struct DZ_API ClayTextFieldDesc
    {
        ClayTextFieldType Type             = ClayTextFieldType::SingleLine;
        InteropString     PlaceholderText  = "";
        ClayColor         PlaceholderColor = ClayColor( 150, 150, 150, 255 );
        ClayColor         TextColor        = ClayColor( 0, 0, 0, 255 );
        ClayColor         BackgroundColor  = ClayColor( 255, 255, 255, 255 );
        ClayColor         BorderColor      = ClayColor( 200, 200, 200, 255 );
        ClayColor         FocusBorderColor = ClayColor( 0, 120, 215, 255 );
        ClayColor         SelectionColor   = ClayColor( 0, 120, 215, 100 );
        ClayColor         CursorColor      = ClayColor( 0, 0, 0, 255 );
        uint16_t          FontId           = 0;
        uint16_t          FontSize         = 14;
        uint16_t          LineHeight       = 0;
        ClayPadding       Padding          = ClayPadding( 8 );
        UIPoints          CursorWidth      = 1.0f;
        bool              ReadOnly         = false;
        size_t            Height           = 0;
        size_t            MaxLength        = 0; // 0 = unlimited
    };

    enum class ClayCustomWidgetType : uint32_t
    {
        TextField          = 0x54455854, // 'TEXT'
        Checkbox           = 0x43484543, // 'CHEC'
        Slider             = 0x534C4944, // 'SLID'
        Dropdown           = 0x44524F50, // 'DROP'
        ColorPicker        = 0x434F4C4F, // 'COLO'
        ResizableContainer = 0x52455349, // 'RESI'
        DockableContainer  = 0x444F434B  // 'DOCK'
    };

    struct DZ_API ClayCustomWidgetData
    {
        ClayCustomWidgetType Type;
        void                *Data;
    };

    struct DZ_API ClayTextFieldRenderData
    {
        ClayTextFieldState *State;
        ClayTextFieldDesc   Desc;
        uint32_t            ElementId;
    };

    struct DZ_API ClayCheckboxState
    {
        bool Checked = false;
    };

    struct DZ_API ClayCheckboxDesc
    {
        ClayColor BackgroundColor      = ClayColor( 255, 255, 255, 255 );
        ClayColor BorderColor          = ClayColor( 200, 200, 200, 255 );
        ClayColor CheckColor           = ClayColor( 0, 120, 215, 255 );
        ClayColor HoverBackgroundColor = ClayColor( 240, 240, 240, 255 );
        ClayColor HoverBorderColor     = ClayColor( 0, 120, 215, 255 );
        float     Size                 = 20.0f;
        float     BorderWidth          = 1.0f;
        float     CornerRadius         = 2.0f;
    };

    struct DZ_API ClayCheckboxRenderData
    {
        ClayCheckboxState *State;
        ClayCheckboxDesc   Desc;
        uint32_t           ElementId;
    };

    struct DZ_API ClaySliderState
    {
        float Value          = 0.0f;
        bool  IsBeingDragged = false;
    };

    struct DZ_API ClaySliderDesc
    {
        float     MinValue        = 0.0f;
        float     MaxValue        = 1.0f;
        float     Step            = 0.01f;
        ClayColor BackgroundColor = ClayColor( 200, 200, 200, 255 );
        ClayColor FillColor       = ClayColor( 0, 120, 215, 255 );
        ClayColor KnobColor       = ClayColor( 255, 255, 255, 255 );
        ClayColor KnobBorderColor = ClayColor( 150, 150, 150, 255 );
        float     Height          = 6.0f;
        float     KnobSize        = 20.0f;
        float     CornerRadius    = 3.0f;
    };

    struct DZ_API ClaySliderRenderData
    {
        ClaySliderState *State;
        ClaySliderDesc   Desc;
        uint32_t         ElementId;
    };

    struct DZ_API ClayDropdownState
    {
        bool          IsOpen        = false;
        int32_t       SelectedIndex = -1;
        InteropString SelectedText  = "";
        float         ScrollOffset  = 0.0f;
    };

    struct DZ_API ClayDropdownDesc
    {
        StringArray Options;
        const char *PlaceholderText   = "Select option...";
        ClayColor   BackgroundColor   = ClayColor( 255, 255, 255, 255 );
        ClayColor   BorderColor       = ClayColor( 200, 200, 200, 255 );
        ClayColor   TextColor         = ClayColor( 0, 0, 0, 255 );
        ClayColor   PlaceholderColor  = ClayColor( 150, 150, 150, 255 );
        ClayColor   HoverColor        = ClayColor( 240, 240, 240, 255 );
        ClayColor   SelectedColor     = ClayColor( 0, 120, 215, 255 );
        ClayColor   DropdownBgColor   = ClayColor( 255, 255, 255, 255 );
        uint16_t    FontId            = 0;
        uint16_t    FontSize          = 14;
        ClayPadding Padding           = ClayPadding( 8 );
        float       MaxDropdownHeight = 200.0f;
        float       ItemHeight        = 0.0f;
    };

    struct DZ_API ClayDropdownRenderData
    {
        ClayDropdownState *State;
        ClayDropdownDesc   Desc;
        uint32_t           ElementId;
    };

    struct DZ_API ClayColorPickerState
    {
        Float3 Hsv                  = Float3{ 0.0f, 1.0f, 1.0f };
        Float3 Rgb                  = Float3{ 1.0f, 0.0f, 0.0f };
        bool    IsColorWheelDragging = false;
        bool    IsValueBarDragging   = false;
        bool    IsExpanded           = false;
    };

    struct DZ_API ClayColorPickerDesc
    {
        float     Size            = 150.0f;
        float     ValueBarWidth   = 20.0f;
        float     CompactSize     = 40.0f;
        ClayColor BorderColor     = ClayColor( 200, 200, 200, 255 );
        ClayColor BackgroundColor = ClayColor( 255, 255, 255, 255 );
        float     BorderWidth     = 1.0f;
        float     CornerRadius    = 4.0f;
    };

    struct DZ_API ClayColorPickerRenderData
    {
        ClayColorPickerState *State;
        ClayColorPickerDesc   Desc;
        uint32_t              ElementId;
    };

    struct DZ_API ClayResizableContainerState
    {
        float   Width           = 300.0f;
        float   Height          = 200.0f;
        bool    IsResizing      = false;
        uint8_t ResizeDirection = 0; // ResizeDirection::None
        Float2 ResizeStartPos  = Float2{ 0.0f, 0.0f };
        Float2 InitialSize     = Float2{ 0.0f, 0.0f };
        Float2 InitialPosition = Float2{ 0.0f, 0.0f };
    };

    struct DZ_API ClayResizableContainerDesc
    {
        float         MinWidth         = 100.0f;
        float         MinHeight        = 100.0f;
        float         MaxWidth         = 1000.0f;
        float         MaxHeight        = 800.0f;
        float         ResizeHandleSize = 8.0f;
        ClayColor     BackgroundColor  = ClayColor( 245, 245, 245, 255 );
        ClayColor     BorderColor      = ClayColor( 200, 200, 200, 255 );
        ClayColor     HandleColor      = ClayColor( 150, 150, 150, 255 );
        ClayColor     TitleBarColor    = ClayColor( 240, 240, 240, 255 );
        ClayColor     TitleTextColor   = ClayColor( 0, 0, 0, 255 );
        uint16_t      FontId           = 0;
        uint16_t      FontSize         = 14;
        uint16_t      TitleBarFontId   = 0;
        uint16_t      TitleBarFontSize = 14;
        float         TitleBarHeight   = 32.0f;
        float         BorderWidth      = 1.0f;
        bool          ShowTitleBar     = true;
        bool          EnableResize     = true;
        InteropString Title            = "Container";
    };

    struct DZ_API ClayResizableContainerRenderData
    {
        ClayResizableContainerState *State;
        ClayResizableContainerDesc   Desc;
        uint32_t                     ElementId;
    };

    struct DZ_API ClayDockableContainerState
    {
        uint8_t  Mode             = 0; // DockingMode::Floating
        uint8_t  DockedSide       = 0; // DockingSide::None
        uint32_t ParentDockId     = 0;
        Float2  FloatingPosition = Float2{ 100.0f, 100.0f };
        Float2  FloatingSize     = Float2{ 300.0f, 200.0f };
        bool     IsDragging       = false;
        Float2  DragStartPos     = Float2{ 0.0f, 0.0f };
        Float2  DragOffset       = Float2{ 0.0f, 0.0f };
        bool     ShowDockZones    = false;
        uint8_t  HoveredDockZone  = 0; // DockingSide::None
        int32_t  TabIndex         = -1;
        bool     IsActive         = true;
    };

    struct DZ_API ClayDockableContainerDesc
    {
        InteropString Title            = "Dockable Container";
        ClayColor     BackgroundColor  = ClayColor( 245, 245, 245, 255 );
        ClayColor     BorderColor      = ClayColor( 200, 200, 200, 255 );
        ClayColor     TitleBarColor    = ClayColor( 240, 240, 240, 255 );
        ClayColor     TitleTextColor   = ClayColor( 0, 0, 0, 255 );
        ClayColor     ActiveTabColor   = ClayColor( 255, 255, 255, 255 );
        ClayColor     InactiveTabColor = ClayColor( 230, 230, 230, 255 );
        ClayColor     DockZoneColor    = ClayColor( 0, 120, 215, 100 );
        uint16_t      FontId           = 0;
        uint16_t      FontSize         = 14;
        uint16_t      TitleBarFontId   = 0;
        uint16_t      TitleBarFontSize = 14;
        float         TitleBarHeight   = 0.0f;
        float         TabHeight        = 28.0f;
        float         BorderWidth      = 1.0f;
        float         MinWidth         = 100.0f;
        float         MinHeight        = 100.0f;
        bool          AllowUndock      = true;
        bool          AllowResize      = true;
        bool          ShowCloseButton  = true;
    };

    struct DZ_API ClayDockableContainerRenderData
    {
        ClayDockableContainerState *State;
        ClayDockableContainerDesc   Desc;
        uint32_t                    ElementId;
    };

    namespace ClayWidgets
    {
        inline ClayTextFieldDesc CreateSingleLineInput( const InteropString &placeholder = "Enter text..." )
        {
            ClayTextFieldDesc desc;
            desc.Type             = ClayTextFieldType::SingleLine;
            desc.PlaceholderText  = placeholder;
            desc.TextColor        = ClayColor( 0, 0, 0, 255 );
            desc.BackgroundColor  = ClayColor( 255, 255, 255, 255 );
            desc.BorderColor      = ClayColor( 200, 200, 200, 255 );
            desc.FocusBorderColor = ClayColor( 0, 120, 215, 255 );
            desc.Padding          = ClayPadding( 8 );
            return desc;
        }

        inline ClayTextFieldDesc CreatePasswordInput( const InteropString &placeholder = "Enter password..." )
        {
            ClayTextFieldDesc desc = CreateSingleLineInput( placeholder );
            desc.Type              = ClayTextFieldType::Password;
            return desc;
        }

        inline ClayTextFieldDesc CreateTextArea( const InteropString &placeholder = "Enter text..." )
        {
            ClayTextFieldDesc desc = CreateSingleLineInput( placeholder );
            desc.Type              = ClayTextFieldType::MultiLine;
            desc.Height            = 100; // More appropriate height for multi-line text
            return desc;
        }

        inline ClayCheckboxDesc CreateCheckbox( const float size = 20.0f )
        {
            ClayCheckboxDesc desc;
            desc.Size = size;
            return desc;
        }

        inline ClaySliderDesc CreateSlider( const float minValue = 0.0f, const float maxValue = 1.0f, const float step = 0.01f )
        {
            ClaySliderDesc desc;
            desc.MinValue = minValue;
            desc.MaxValue = maxValue;
            desc.Step     = step;
            return desc;
        }

        inline ClayDropdownDesc CreateDropdown( const StringArray &options, const char *placeholder = "Select option..." )
        {
            ClayDropdownDesc desc;
            desc.Options         = options;
            desc.PlaceholderText = placeholder;
            return desc;
        }

        inline ClayColorPickerDesc CreateColorPicker( const float size = 150.0f )
        {
            ClayColorPickerDesc desc;
            desc.Size = size;
            return desc;
        }
    } // namespace ClayWidgets
} // namespace DenOfIz
