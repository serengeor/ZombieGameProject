#include "Precomp.h"

#include "Application/Window.h"
#include "utility/Timer.h"
#include "utility/Logger.h"

#include "Application.h"

void Application::OutputVersions()
{
    PHYSFS_Version compiled;
    PHYSFS_Version linked;

    PHYSFS_VERSION(&compiled);
    PHYSFS_getLinkedVersion(&linked);

    printf("Compiled against PhysicsFS version %d.%d.%d,\n"
           " and linked against %d.%d.%d.\n\n",
            (int) compiled.major, (int) compiled.minor, (int) compiled.patch,
            (int) linked.major, (int) linked.minor, (int) linked.patch);
} /* output_versions */

Application::Application(int32_t argc, const char ** argv)
{
    this->_argc = argc;
    this->_argv = argv;
    _appContext = new AppContext();
}

Application::~Application()
{

}

bool Application::Init(const std::string  &title, uint32_t width, uint32_t height)
{

    _appContext->_timer = timer_ptr(new Timer());

    if(!PHYSFS_init(_argv[0]))
    {
        std::cout<<"PHYSFS_init() failed: " <<PHYSFS_getLastError()<<std::endl;
        return false;
    }

    std::string realDir = PHYSFS_getBaseDir();

    std::string dir;

    uint32_t pos = realDir.find_last_of(PHYSFS_getDirSeparator());
    realDir = realDir.substr(0,pos);

    pos = realDir.find_last_of(PHYSFS_getDirSeparator());
    realDir = realDir.substr(0,pos);

    pos = realDir.find_last_of(PHYSFS_getDirSeparator());
    dir = realDir.substr(0,pos);

    dir+=PHYSFS_getDirSeparator();

    _appContext->_logger=new Logger(this,0);
    _appContext->_logger->log(LOG_LOG,"Initializing \"%s\"",title.c_str());

    _appContext->_logger->log(LOG_LOG,"Base Directory: \"%s\"",PHYSFS_getBaseDir());
    _appContext->_logger->log(LOG_LOG,"Directory: \"%s\"",dir.c_str());

    #ifdef RELEASE_FS
    PHYSFS_mount(PHYSFS_getBaseDir(),NULL,0);
    #else
    PHYSFS_mount(dir.c_str(), NULL, 0);
    #endif // RELEASE_FS

    std::string combo=PHYSFS_getBaseDir();
    combo+="res/";
    PHYSFS_mount(combo.c_str(),NULL,0);
    OutputVersions();

    _appContext->_window = new Window();

    if(!_appContext->_window->Init(title, width, height))
    {
        delete _appContext->_window;
        _appContext->_window = nullptr;
        return false;
    }

    _appContext->_window->SigWindowClosed().connect(sigc::mem_fun(this,&Application::OnWindowClose));

    this->_appContext->_glUtil = new OpenGLUtil(_appContext->_logger);

    if(!this->_appContext->_glUtil->load_extensions())
    {
        delete _appContext->_window;
        return false;
    }

    _appContext->_scenegraph = new sg::SGScenegraph(_appContext->_logger, _appContext->_timer);
    _appContext->_graphicsManager = _appContext->_scenegraph->get_graphics_manager();

    _appContext->_logger->log(LOG_CRITICAL,(const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

    if(!_appContext->IsInitialized())
        throw "Failed to initialize app context.";

    return true;
}

void Application::Exit()
{
    _appContext->_logger->log(LOG_LOG,"Exitting.");

    Window::DestroyWindow(_appContext->_window);
    delete _appContext->_logger;

    if (!PHYSFS_deinit())
        std::cout<< "PHYSFS_deinit() failed!\n reason: " << PHYSFS_getLastError() << "." << std::endl;

    _appContext->_timer = nullptr;
}

AppContext * Application::Ctx()
{
    return _appContext;
}
