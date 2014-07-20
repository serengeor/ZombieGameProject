#pragma once

// Stuff to still convert
//!TODO (murloc992#1#): GUI Event
//!TODO (murloc992#1#): GUI Event Listener
//!TODO (murloc992#1#): Rectangle
//!TODO (murloc992#1#): Vec2
//!TODO (murloc992#1#): GUI Element
//!TODO (murloc992#1#): GUI Environment
//!TODO (murloc992#1#): Font Renderer
//!TODO (murloc992#1#): GUI Static Text
//!TODO (murloc992#1#): GUI Button
//!TODO (murloc992#1#): GUI Window
//TODO (murloc992#1#): GUI Text Field
//TODO (murloc992#1#): GUI Slider

#include "GUIElement.h"
#include "GUISkin.h"
#include "font_rendering/FontRenderer.h"

class shader;
class quad;
class sliced_gui_quad;
class texture;
class Window;
class gui_skin;
class Logger;

GUIElement * search_elements(GUIElement * el, const std::string & name);

class GUIEnvironment : public GUIElement
{
public:
    GUIEnvironment(Window* win, Logger* log);
    ~GUIEnvironment();

    void update(float delta);
    void Render();

    virtual bool OnEvent(const GUIEvent & e);
    bool is_on_hover(GUIElement *e);
    bool is_on_focus(GUIElement *e);

    void draw_gui_quad(Rect2D<int> size, std::shared_ptr<texture> tex, bool tile=false);
    void draw_gui_quad(Rect2D<int> size, uint32_t style=gui_style::gui_skin_background, bool tile=false);
    void draw_sliced_gui_quad(Rect2D<int> size, std::shared_ptr<texture> tex, bool tile=false);
    void draw_sliced_gui_quad(Rect2D<int> size, uint32_t style=gui_style::gui_skin_background, bool tile=false);

    void set_skin(gui_skin* skin);

    glm::vec2 get_mouse_pos();
    glm::vec2 get_gui_scale();

    void on_mouse_moved(double x, double y);
    void on_mouse_button(int32_t button, int32_t action, int32_t mod);
    void on_mouse_scroll(double sx, double sy);
    void on_key_event(int32_t key, int32_t scan_code, int32_t action, int32_t mod);
    void on_char_typed(int32_t scan_code);

    const std::wstring &get_clipboard()
    {
        return clipboard_string;
    }

    wchar_t get_last_char()
    {
        return last_char;
    }

    int32_t get_last_key()
    {
        return last_key;
    }

    template <typename T>
    Rect2D<T> scale_gui_rect(Rect2D<T> unscaled)
    {
        T gsx=get_gui_scale().x;
        T gsy=get_gui_scale().y;
        T px=-1+unscaled.x*gsx+unscaled.w/2*gsx;
        T py=1-unscaled.y*gsy-unscaled.h/2*gsy;
        T sx=unscaled.w/2*gsx;
        T sy=unscaled.h/2*gsy;

        Rect2D<T> ret=Rect2D<T>(px,py,sx,sy);

        return ret;
    }

    font_renderer *get_font_renderer();


    ///GUI ELEMENTS
    GUIElement * get_element_by_name(const std::string & name);

    template<class T> T * get_element_by_name_t(const std::string & name)
    {
        return dynamic_cast<T*>(search_elements(this,name));
    }
//    gui_static_text *add_gui_static_text();
//    gui_button *add_gui_button();
//    gui_checkbox *add_gui_checkbox();
//    gui_edit_box *add_gui_edit_box();
//
//    gui_window *add_gui_window();
//    gui_pane* add_gui_pane();
private:
    gui_skin* skin;
    texture* skin_atlas;
    shader* gui_shader;

    quad* gui_quad;
    sliced_gui_quad* sliced_quad;

    font_renderer* m_font_renderer;
    Window* m_window;

    sigc::connection _sig_mouse_move,_sig_mouse_button,_sig_mouse_scroll,_sig_key,_sig_text;

    GUIElement *hover, *last_hover, *focus, *last_focus;

    bool m_mouse_down, m_mouse_moved, m_mouse_dragged;

    wchar_t last_char;
    int32_t last_key,last_mod;
    std::wstring clipboard_string;

    glm::vec2 mouse_pos, last_mouse_pos, gui_scale;
protected:
};
