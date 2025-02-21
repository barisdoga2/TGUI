/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2023 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef TGUI_HORIZONTAL_WRAP_HPP
#define TGUI_HORIZONTAL_WRAP_HPP

#include <TGUI/Widgets/BoxLayout.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Container that automatically arranges children in lines, adding a new line when a horizontal line is full
    ///
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API HorizontalWrap : public BoxLayout
    {
    public:
        using Ptr = std::shared_ptr<HorizontalWrap>; //!< Shared widget pointer
        using ConstPtr = std::shared_ptr<const HorizontalWrap>; //!< Shared constant widget pointer

        static constexpr const char StaticWidgetType[] = "HorizontalWrap"; //!< Type name of the widget


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Constructor
        /// @param typeName     Type of the widget
        /// @param initRenderer Should the renderer be initialized? Should be true unless a derived class initializes it.
        /// @see create
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        HorizontalWrap(const char* typeName = StaticWidgetType, bool initRenderer = true);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new horizontal wrap widget
        ///
        /// @return The new horizontal wrap
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD static HorizontalWrap::Ptr create(const Layout2d& size = {"100%", "100%"});


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Makes a copy of another layout
        ///
        /// @param layout  The other layout
        ///
        /// @return The new layout
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD static HorizontalWrap::Ptr copy(const HorizontalWrap::ConstPtr& layout);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // @brief Repositions and resize the widgets
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void updateWidgets() override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Makes a copy of the widget
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD Widget::Ptr clone() const override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_HORIZONTAL_WRAP_HPP
