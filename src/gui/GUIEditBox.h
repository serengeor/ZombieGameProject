#pragma once

#include "gui/GUIElement.h"

template <typename T>
class Rect2D;
class gui_edit_box:public GUIElement
{
private:
    uint32_t blinktimer,curspos,reptimer,font_size;
    int32_t sx;
    float _mx,_mw,_my,_mh; //margins for drawing
    wchar_t lastkey;
    bool blink,clearonsubmit;
    std::wstring m_text,disp_text;
    glm::vec4 m_text_color;

    void add_text(int32_t index,std::wstring text);
    void remove_text(int32_t index, int32_t length);
public:
    gui_edit_box(GUIEnvironment* env, Rect2D<int> dimensions, std::wstring text=L"text", glm::vec4 text_color=glm::vec4(1,1,1,1), bool drawbackground=false, bool drawshadow=false, bool clearonsubmit=false);
    virtual ~gui_edit_box();

    void Render();

    void set_text(const std::wstring &text);

    const std::wstring &get_text()
    {
        return m_text;
    }

    bool OnEvent(const GUIEvent & e);
protected:
};
