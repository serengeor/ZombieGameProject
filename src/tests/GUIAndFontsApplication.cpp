#include "Precomp.h"

#include "Application/Window.h"
#include "utility/Timer.h"
#include "utility/Logger.h"
#include "opengl/OpenGLUtil.h"

#include "gui/GUIEnvironment.h"
#include "gui/GUIWindow.h"
#include "gui/GUIPane.h"
#include "gui/GUIStaticText.h"
#include "gui/GUIButton.h"
#include "gui/GUICheckbox.h"

#include "GUIAndFontsApplication.h"

#include "utility/Rect2d.h"

gui_and_fonts_Application::gui_and_fonts_Application(uint32_t argc, const char ** argv): Application(argc,argv)
{
    //definitely a comment
    running=true;
}

gui_and_fonts_Application::~gui_and_fonts_Application()
{

}

bool gui_and_fonts_Application::Init(const std::string & title, uint32_t width, uint32_t height)
{
    Application::Init(title,width,height);

    frame_count = 0;
    last_time = 0;

    ///set up matrices

    M = glm::mat4(1.0f);
    M = glm::translate(M,glm::vec3(-10,5,-10));
    M = glm::scale(M,glm::vec3(0.01,0.01,0.01));

    V = glm::lookAt(glm::vec3(0,5,20),glm::vec3(0,5,0),glm::vec3(0,1,0));
    V = glm::rotate<float>(V,-90,glm::vec3(0,1,0));

    P = glm::perspective(45.f,4.f/3.f,1.0f,2048.f);

    MVP=P*V*M;

    ///gl setup
    glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    int ww,hh;
    glfwGetWindowSize(_window->getWindow(),&ww,&hh);

    env=new gui_environment(_window,this->GetLogger());

    gui_window* mainpaine=new gui_window(env,rect2d<int>(400,400,400,400),L"Tis but a Window",false,true,false);

    test1=new gui_static_text(env,rect2d<int>(900,10,125,12),L"testtext",glm::vec4(1,1,1,1),false,true);
    test2=new gui_button(env,rect2d<int>(120,0,100,20),L"buttons!");
    test2->set_event_listener(this);
    test2->set_id(1);
    test2->set_parent(mainpaine);

    gui_button* swit=new gui_button(env,rect2d<int>(220,0,20,20),L"X");
    swit->set_event_listener(this);
    swit->set_id(2);
    swit->set_parent(mainpaine);


    for(uint8_t i=0; i<10; i++)
    {
        gui_checkbox* c=new gui_checkbox(env,rect2d<int>(0,0+i*22,20,20),i%2==0);
        c->set_event_listener(this);
        c->set_id(10+i);
        c->set_parent(mainpaine);
        gui_static_text* lab=new gui_static_text(env,rect2d<int>(20,0+i*22+4,100,20),L"checkbox label",glm::vec4(1,1,1,1));
        lab->set_parent(mainpaine);
    }

    rect2d<int> testrect=rect2d<int>(0,0,100,100);
    bool a=testrect.is_point_inside(50,50);
    bool b=testrect.is_point_inside(99,99);
    bool c=testrect.is_point_inside(101,101);

    printf("%d %d %d\n",a,b,c);

    renderer=env->get_font_renderer();
    renderer->create_font("bits","res/bits.ttf",28);
    return true;
}

void gui_and_fonts_Application::on_event(gui_event e)
{
    logger* _log=this->GetLogger();
    switch(e.get_type())
    {
    case element_focused:
        _log->log(LOG_DEBUG,"Element %s got focused.",e.get_caller()->get_name().c_str());
        break;
    case element_focus_lost:
        _log->log(LOG_DEBUG,"Element %s lost focus.",e.get_caller()->get_name().c_str());
        break;
    case element_hovered:
        _log->log(LOG_DEBUG,"Element %s got hovered.",e.get_caller()->get_name().c_str());
        break;
    case element_exitted:
        _log->log(LOG_DEBUG,"Element %s got exitted.",e.get_caller()->get_name().c_str());
        break;

    case button_pressed:
        switch(e.get_caller()->get_id())
        {
        case 1:
            this->running=false;
            break;
        case 2:
            test2->set_enabled(!test2->is_enabled());
            break;
        default:
            break;
        }
        _log->log(LOG_DEBUG,"Button pressed on %s.",e.get_caller()->get_name().c_str());
        break;

    case checkbox_state_changed:
        _log->log(LOG_DEBUG,"Checkbox %i state changed.",e.get_caller()->get_id());
        break;

    case button_released:
        _log->log(LOG_DEBUG,"Button released on %s.",e.get_caller()->get_name().c_str());
        break;

    case mouse_pressed:
        _log->log(LOG_DEBUG,"Mouse pressed on %s.",e.get_caller()->get_name().c_str());
        break;
    case mouse_released:
        _log->log(LOG_DEBUG,"Mouse released on %s.",e.get_caller()->get_name().c_str());
        break;
    case mouse_dragged:
        _log->log(LOG_DEBUG,"Mouse dragged on %s.",e.get_caller()->get_name().c_str());
        break;
    case mouse_moved:
        _log->log(LOG_DEBUG,"Mouse moved on %s.",e.get_caller()->get_name().c_str());
        break;
    default:
        break;
    }
}

bool gui_and_fonts_Application::Update()
{
    if(_window->Update() && !_window->GetKey(GLFW_KEY_ESCAPE) && this->running)
    {
        // Measure speed
        _mainTimer->tick();

        env->update(0);

        show_fps();

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        env->render();

        renderer->render_string(L"TCHOFF tchoff PHYSFS!",glm::vec2(0,10),false);
        renderer->use_font("bits");
        renderer->render_string(L"I am le lcd machine!",glm::vec2(0,100),glm::vec4(1,1,1,1),false);
        renderer->render_string(L"I am le SHADOW lcd machine!",glm::vec2(0,150),glm::vec4(1,1,1,1),true);
        renderer->use_font();

        _window->swap_buffers();

        ///let's just rage quit on gl error
        return !this->_GLUtil->check_and_output_errors();
    }
    return false;
}

void gui_and_fonts_Application::show_fps()
{
    uint32_t currentTime = _mainTimer->get_time();
    frame_count++;
    if ( currentTime - last_time >= 1000 )  // If last prinf() was more than 1 sec ago
    {
        _logger->log(LOG_LOG,"FPS: %i (%f ms/frame)",frame_count,1000.0/double(frame_count));

        test1->set_text(L"FPS: "+helpers::to_wstr<int>(frame_count)+L" "+helpers::to_wstr(1000.0/double(frame_count))+L"ms");

        frame_count = 0;
        last_time = currentTime;
    }
}

void gui_and_fonts_Application::Exit()
{
    delete env;
    Application::Exit();
}