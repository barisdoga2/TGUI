/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2014 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/MessageBox.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox::MessageBox()
    {
        m_callback.widgetType = Type_MessageBox;

        add(m_label, "MessageBoxText");
        m_label->setTextSize(m_textSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox::MessageBox(const MessageBox& messageBoxToCopy) :
        ChildWindow               {messageBoxToCopy},
        m_loadedConfigFile        {messageBoxToCopy.m_loadedConfigFile},
        m_buttonConfigFileFilename{messageBoxToCopy.m_buttonConfigFileFilename},
        m_textSize                {messageBoxToCopy.m_textSize}
    {
        m_label = Label::copy(messageBoxToCopy.m_label);
        add(m_label, "MessageBoxText");

        for (auto it = messageBoxToCopy.m_buttons.begin(); it != messageBoxToCopy.m_buttons.end(); ++it)
        {
            Button::Ptr button = Button::copy(*it);
            button->unbindAllCallback();
            button->bindCallbackEx(Button::LeftMouseClicked | Button::SpaceKeyPressed | Button::ReturnKeyPressed,
                                   std::bind(&MessageBox::buttonClickedCallbackFunction, this, std::placeholders::_1));

            m_buttons.push_back(button);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox& MessageBox::operator= (const MessageBox& right)
    {
        if (this != &right)
        {
            MessageBox temp(right);
            ChildWindow::operator=(right);

            std::swap(m_loadedConfigFile,         temp.m_loadedConfigFile);
            std::swap(m_buttonConfigFileFilename, temp.m_buttonConfigFileFilename);
            std::swap(m_buttons,                  temp.m_buttons);
            std::swap(m_label,                    temp.m_label);
            std::swap(m_textSize,                 temp.m_textSize);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox::Ptr MessageBox::create(const std::string& configFileFilename)
    {
        auto messageBox = std::make_shared<MessageBox>();

        messageBox->m_loadedConfigFile = getResourcePath() + configFileFilename;

        // Open the config file
        ConfigFile configFile{messageBox->m_loadedConfigFile, "MessageBox"};

        // Find the folder that contains the config file
        std::string configFileFolder = "";
        std::string::size_type slashPos = configFileFilename.find_last_of("/\\");
        if (slashPos != std::string::npos)
            configFileFolder = configFileFilename.substr(0, slashPos+1);

        bool childWindowPropertyFound = false;
        bool buttonPropertyFound = false;

        // Handle the read properties
        for (auto it = configFile.getProperties().cbegin(); it != configFile.getProperties().cend(); ++it)
        {
            if (it->first == "textcolor")
            {
                messageBox->m_label->setTextColor(configFile.readColor(it));
            }
            else if (it->first == "childwindow")
            {
                if ((it->second.length() < 3) || (it->second[0] != '"') || (it->second[it->second.length()-1] != '"'))
                    throw Exception{"Failed to parse value for ChildWindow in section MessageBox in " + messageBox->m_loadedConfigFile + "."};

                try
                {
                    messageBox->ChildWindow::operator=(*ChildWindow::create(configFileFolder + it->second.substr(1, it->second.length()-2)));
                    childWindowPropertyFound = true;
                }
                catch (const Exception& e)
                {
                    throw Exception{"Failed to load the internal ChildWindow for MessageBox. " + std::string{e.what()}};
                }
            }
            else if (it->first == "button")
            {
                if ((it->second.length() < 3) || (it->second[0] != '"') || (it->second[it->second.length()-1] != '"'))
                    throw Exception{"Failed to parse value for Button in section MessageBox in " + messageBox->m_loadedConfigFile + "."};

                messageBox->m_buttonConfigFileFilename = configFileFolder + it->second.substr(1, it->second.length()-2);
                buttonPropertyFound = true;
            }
            else
                throw Exception{"Unrecognized property '" + it->first + "' in section MessageBox in " + messageBox->m_loadedConfigFile + "."};
        }

        if (!childWindowPropertyFound)
            throw Exception{"Missing a ChildWindow property in section MessageBox in " + messageBox->m_loadedConfigFile + "."};

        if (!buttonPropertyFound)
            throw Exception{"Missing a Button property in section MessageBox in " + messageBox->m_loadedConfigFile + "."};

        return messageBox;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::setText(const sf::String& text)
    {
        m_label->setText(text);

        rearrange();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::setTextSize(unsigned int size)
    {
        m_textSize = size;

        m_label->setTextSize(size);

        for (unsigned int i = 0; i < m_buttons.size(); ++i)
            m_buttons[i]->setTextSize(m_textSize);

        rearrange();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::addButton(const sf::String& caption)
    {
        auto button = Button::create(m_buttonConfigFileFilename);
        button->setTextSize(m_textSize);
        button->setText(caption);
        button->bindCallbackEx(Button::LeftMouseClicked | Button::SpaceKeyPressed | Button::ReturnKeyPressed,
                               std::bind(&MessageBox::buttonClickedCallbackFunction, this, std::placeholders::_1));

        add(button);
        m_buttons.push_back(button);

        rearrange();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
    void MessageBox::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "text")
        {
            setText(value);
        }
        else if (property == "textcolor")
        {
            setTextColor(extractColor(value));
        }
        else if (property == "textsize")
        {
            setTextSize(tgui::stoi(value));
        }
        else if (property == "buttons")
        {
            removeAllWidgets();
            m_buttons.clear();
            add(m_label);

            std::vector<sf::String> buttons;
            decodeList(value, buttons);

            for (auto it = buttons.cbegin(); it != buttons.cend(); ++it)
                addButton(*it);
        }
        else if (property == "callback")
        {
            ChildWindow::setProperty(property, value);

            std::vector<sf::String> callbacks;
            decodeList(value, callbacks);

            for (auto it = callbacks.begin(); it != callbacks.end(); ++it)
            {
                if ((*it == "ButtonClicked") || (*it == "buttonclicked"))
                    bindCallback(ButtonClicked);
            }
        }
        else // The property didn't match
            ChildWindow::setProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::getProperty(std::string property, std::string& value) const
    {
        property = toLower(property);

        if (property == "text")
            value = getText().toAnsiString();
        else if (property == "textcolor")
            value = "(" + tgui::to_string(int(getTextColor().r)) + "," + tgui::to_string(int(getTextColor().g)) + "," + tgui::to_string(int(getTextColor().b)) + "," + tgui::to_string(int(getTextColor().a)) + ")";
        else if (property == "textsize")
            value = tgui::to_string(getTextSize());
        else if (property == "buttons")
        {
            std::vector<sf::String> buttons;
            for (auto it = m_buttons.cbegin(); it != m_buttons.cend(); ++it)
                buttons.push_back((*it)->getText());

            encodeList(buttons, value);
        }
        else if (property == "callback")
        {
            std::string tempValue;
            ChildWindow::getProperty(property, tempValue);

            std::vector<sf::String> callbacks;

            if ((m_callbackFunctions.find(ButtonClicked) != m_callbackFunctions.end()) && (m_callbackFunctions.at(ButtonClicked).size() == 1) && (m_callbackFunctions.at(ButtonClicked).front() == nullptr))
                callbacks.push_back("ButtonClicked");

            encodeList(callbacks, value);

            if (value.empty())
                value = tempValue;
            else if (!tempValue.empty())
                value += "," + tempValue;
        }
        else // The property didn't match
            ChildWindow::getProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::list< std::pair<std::string, std::string> > MessageBox::getPropertyList() const
    {
        auto list = ChildWindow::getPropertyList();
        list.push_back(std::pair<std::string, std::string>("Text", "string"));
        list.push_back(std::pair<std::string, std::string>("TextColor", "color"));
        list.push_back(std::pair<std::string, std::string>("TextSize", "uint"));
        list.push_back(std::pair<std::string, std::string>("Buttons", "string"));
        return list;
    }
*/
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::rearrange()
    {
        // Calculate the button size
        float buttonWidth = 5.0f * m_textSize;
        float buttonHeight = m_textSize * 10.0f / 8.0f;
        for (unsigned int i = 0; i < m_buttons.size(); ++i)
        {
            float width = sf::Text(m_buttons[i]->getText(), *m_buttons[i]->getTextFont(), m_textSize).getLocalBounds().width;
            if (buttonWidth < width * 10.0f / 9.0f)
                buttonWidth = width * 10.0f / 9.0f;
        }

        // Calculate the space needed for the buttons
        float distance = buttonHeight * 2.0f / 3.0f;
        float buttonsAreaWidth = distance;
        for (unsigned int i = 0; i < m_buttons.size(); ++i)
        {
            m_buttons[i]->setSize({buttonWidth, buttonHeight});
            buttonsAreaWidth += m_buttons[i]->getSize().x + distance;
        }

        // Calculate the suggested size of the window
        sf::Vector2f size = {2*distance + m_label->getSize().x, 3*distance + m_label->getSize().y + buttonHeight};

        // Make sure the buttons fit inside the message box
        if (buttonsAreaWidth > size.x)
            size.x = buttonsAreaWidth;

        // Set the size of the window
        setSize(size);

        // Set the text on the correct position
        m_label->setPosition({distance, distance});

        // Set the buttons on the correct position
        float leftPosition = 0;
        float topPosition = 2*distance + m_label->getSize().y;
        for (unsigned int i = 0; i < m_buttons.size(); ++i)
        {
            leftPosition += distance + ((size.x - buttonsAreaWidth) / (m_buttons.size()+1));
            m_buttons[i]->setPosition({leftPosition, topPosition});
            leftPosition += m_buttons[i]->getSize().x;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::buttonClickedCallbackFunction(const Callback& callback)
    {
        if (m_callbackFunctions[ButtonClicked].empty() == false)
        {
            m_callback.trigger = ButtonClicked;
            m_callback.text    = static_cast<Button*>(callback.widget)->getText();
            addCallback();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
