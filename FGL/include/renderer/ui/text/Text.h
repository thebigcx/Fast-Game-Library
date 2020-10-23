#pragma once

#include <renderer/ui/text/Font.h>
#include <renderer/Transformable.h>
#include <util/Color.h>
#include <util/maths/Vector2.h>

class Text : public Transformable
{
public:
    Text(Font& p_font);

    void setColor(const Color& color);

    void setString(const std::string& string);

    void setFont(Font& font);

    inline const Color& getColor() const { return m_color; };
    inline const Font* getFont()   const { return m_pFont; };

    inline const std::string& getString() const { return m_string; };

private:
    Font* m_pFont;

    std::string m_string = "";

    Color m_color;
};