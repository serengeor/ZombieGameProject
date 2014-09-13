#include "precomp.h"
#include "VoxelzApp.h"
#include "application/window.h"
#include "opengl/Mesh.h"
#include "opengl/Shader.h"
#include "opengl/MVar.h"
#include "resources/ShaderLoader.h"
#include "scenegraph/Camera.h"
#include "Opengl/CubeMesh.h"
#include "Opengl/GridMesh.h"
#include "Voxel/Block.h"
#include "Voxel/Chunk.h"
#include "Voxel/ChunkManager.h"
#include "GUI/GUI.h"

VoxelzApp::VoxelzApp(uint32_t argc, const char ** argv): Application(argc,argv)
{

}

VoxelzApp::~VoxelzApp()
{

}

static MeshPtr mesh;
static ShaderPtr sh,vsh;
static CameraPtr cam;
static CubeMesh *cub,*smallcub;
static GridMesh *grid;
static ChunkManager *chkmgr;
static GUIEnvironment *env;
static glm::vec3 voxpos,newvoxpos,pointpos;
static bool validvoxel,wireframe;
static int face;

void InitPlaneMesh(AppContext * ctx)
{
    validvoxel=false;
    wireframe=false;
    voxpos=glm::vec3(0);

    BufferObject<glm::vec3> * pos = new BufferObject<glm::vec3>();
    pos->data.push_back(glm::vec3(-0.5, 0.5, 0));
    pos->data.push_back(glm::vec3( 0.5, 0.5, 0));
    pos->data.push_back(glm::vec3( -0.5,-0.5, 0));
    pos->data.push_back(glm::vec3(0.5,-0.5, 0));

    IndexBufferObject<uint32_t> * id = new IndexBufferObject<uint32_t>();
    id->data.push_back(0);
    id->data.push_back(1);
    id->data.push_back(2);
    id->data.push_back(2);
    id->data.push_back(1);
    id->data.push_back(3);

    mesh = share(new Mesh());
    mesh->buffers[Mesh::POSITION] = pos;
    mesh->buffers[Mesh::INDICES] = id;
    mesh->Init();

    sh = (new shader_loader(ctx->_logger))->load("res/engine/shaders/solid_unlit");
    vsh = (new shader_loader(ctx->_logger))->load("res/engine/shaders/voxelphong");
    cam=share(new Camera(ctx,glm::vec3(0,0,5),glm::vec3(0,0,-5),glm::vec3(0,1,0)));

    env=new GUIEnvironment(ctx->_window,ctx->_logger);
    gui_pane* pan=new gui_pane(env,Rect2D<int>(0,0,200,200),true);

    gui_static_text* texts[10];

    std::stringstream ss;

    loopi(10)
    {
        ss<<i;
        texts[i]=new gui_static_text(env,Rect2D<int>(0,i*20,200,20),L"",glm::vec4(1),false,true);
        texts[i]->SetName(ss.str());
        texts[i]->SetParent(pan);
        ss.str(std::string()); ///clear stream
    }

    cub=new CubeMesh(1);
    smallcub=new CubeMesh(0.25);

    grid=new GridMesh(1.f,1024,16);
    Timer timer;

    chkmgr=new ChunkManager();
}

bool VoxelzApp::Init(const std::string & title, uint32_t width, uint32_t height)
{
    Application::Init(title,width,height);

    _appContext->_window->SigKeyEvent().connect(sigc::mem_fun(this,&VoxelzApp::OnKeyEvent));
    _appContext->_window->SigMouseKey().connect(sigc::mem_fun(this,&VoxelzApp::OnMouseKey));
    _appContext->_window->SigMouseMoved().connect(sigc::mem_fun(this,&VoxelzApp::OnMouseMove));

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glClearColor(0.5,0.5,0.7,0);

    _appContext->_timer->tick();

    InitPlaneMesh(_appContext);

    _appContext->_timer->tick();

    printf("Building took: %d ms\n",_appContext->_timer->get_delta_time());
    return true;
}

bool VoxelzApp::Update()
{
    if(_appContext->_window->Update() && !_appContext->_window->GetShouldClose() && !_appContext->_window->GetKey(GLFW_KEY_ESCAPE))
    {
        _appContext->_timer->tick();

        cam->Update(0);
        if(wireframe==true)
        {
            glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 Model = glm::mat4(1.0f);
        glm::mat4 MVP   = cam->GetViewProjMat() * Model;

        sh->Set();

        Model = glm::mat4(1.0f);
        Model = glm::translate(voxpos);
        MVP   = cam->GetViewProjMat() * Model;
        MVar<glm::mat4>(0, "mvp", MVP).Set();
        cub->Render(true);

        Model = glm::mat4(1.0f);
        Model = glm::translate(newvoxpos);
        MVP   = cam->GetViewProjMat() * Model;
        MVar<glm::mat4>(0, "mvp", MVP).Set();
        cub->Render(true);

        Model = glm::mat4(1.0f);
        Model = glm::translate(pointpos);
        MVP   = cam->GetViewProjMat() * Model;
        MVar<glm::mat4>(0, "mvp", MVP).Set();
        smallcub->Render(true);

        Model = glm::mat4(1.0f);
        MVP   = cam->GetViewProjMat() * Model;
        MVar<glm::mat4>(0, "mvp", MVP).Set();
        grid->render_lines();

        vsh->Set();
        Model = glm::mat4(1.0f);
        MVP   = cam->GetViewProjMat() * Model;
        MVar<glm::mat4>(vsh->getparam("v_inv"), "v_inv", glm::inverse(glm::mat4(1)*cam->GetViewMat())).Set();
        glm::mat3 m_3x3_inv_transp = glm::transpose(glm::inverse(glm::mat3(glm::mat4(1))));
        MVar<glm::mat3>(vsh->getparam("m_3x3_inv_transp"), "m_3x3_inv_transp", m_3x3_inv_transp).Set();
        MVar<glm::mat4>(vsh->getparam("P"), "P", cam->GetProjectionMat()).Set();
        MVar<glm::mat4>(vsh->getparam("V"), "V", cam->GetViewMat()).Set();
        MVar<glm::mat4>(vsh->getparam("M"), "M", Model).Set();
        chkmgr->Render(cam.get(),vsh);

        if(wireframe==true)
        {
            glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
        }

        env->Render();
        _appContext->_window->SwapBuffers();
        return true;
    }
    return false;
}

void VoxelzApp::Exit()
{
    Application::Exit();
}

void VoxelzApp::OnWindowClose()
{

}
void VoxelzApp::OnKeyEvent(int32_t key, int32_t scan_code, int32_t action, int32_t modifiers)
{
    if(key==GLFW_KEY_W)
        cam->Walk(1);
    if(key==GLFW_KEY_S)
        cam->Walk(-1);
    if(key==GLFW_KEY_A)
        cam->Strafe(-1);
    if(key==GLFW_KEY_D)
        cam->Strafe(1);
    if(key==GLFW_KEY_SPACE&&action==GLFW_RELEASE)
    {
        chkmgr->Explode(voxpos,20);
    }
    if(key==GLFW_KEY_Z&&action==GLFW_RELEASE)
    {
        wireframe=!wireframe;
    }
}

void VoxelzApp::OnMouseMove(double x, double y)
{
    /* Very naive ray casting algorithm to find out which block we are looking at */

    glm::vec3 position=cam->GetPosition();
    glm::vec3 lookat=glm::normalize(cam->GetLook());

    glm::vec3 testpos = position;
    glm::vec3 prevpos = position;
    int mx,my,mz;
    wchar_t buf[256];
    /// 50/10 ~ 5 voxels
    for(int i = 0; i < 100; i++)
    {
        /* Advance from our currect position to the direction we are looking at, in small steps */
        prevpos = testpos;
        testpos += lookat * 0.1f;

        mx = glm::floor(testpos.x);
        my = glm::floor(testpos.y);
        mz = glm::floor(testpos.z);

        /* If we find a block that is not air, we are done */
        if(chkmgr->GetBlock(glm::vec3(mx, my, mz)).GetBlockType()!=EBT_DEFAULT)
        {
            validvoxel=true;
            break;
        }
    }

    swprintf(buf,L"LookAt %.2f %.2f %.2f",lookat.x,lookat.y,lookat.z);
    env->get_element_by_name_t<gui_static_text>("0")->set_text(buf);

    glm::vec3 aa=WorldToChunkCoords(glm::vec3(mx,my,mz)),bb=ChunkSpaceCoords(glm::vec3(mx,my,mz));

    swprintf(buf,L"Chunk %.2f %.2f %.2f",aa.x,aa.y,aa.z);
    env->get_element_by_name_t<gui_static_text>("1")->set_text(buf);

    swprintf(buf,L"Chunk Coords %.2f %.2f %.2f",bb.x,bb.y,bb.z);
    env->get_element_by_name_t<gui_static_text>("2")->set_text(buf);

    /* Find out which face of the block we are looking at */

    int px = glm::floor(prevpos.x);
    int py = glm::floor(prevpos.y);
    int pz = glm::floor(prevpos.z);

    if(px > mx)
        face = 0;
    else if(px < mx)
        face = 3;
    else if(py > my)
        face = 1;
    else if(py < my)
        face = 4;
    else if(pz > mz)
        face = 2;
    else if(pz < mz)
        face = 5;

    /* If we are looking at air, move the cursor out of sight */

    if(chkmgr->GetBlock(glm::vec3(mx, my, mz)).GetBlockType()==EBT_DEFAULT)
    {
        mx = my = mz = 99999;
        validvoxel=false;
    }

    voxpos=glm::vec3(mx,my,mz);

    if(face == 0)
        mx++;
    if(face == 3)
        mx--;
    if(face == 1)
        my++;
    if(face == 4)
        my--;
    if(face == 2)
        mz++;
    if(face == 5)
        mz--;

    newvoxpos=glm::vec3(mx,my,mz);

    swprintf(buf,L"Face %d",face);
    env->get_element_by_name_t<gui_static_text>("3")->set_text(buf);

    swprintf(buf,L"VoxPos %.2f %.2f %.2f",voxpos.x,voxpos.y,voxpos.z);
    env->get_element_by_name_t<gui_static_text>("4")->set_text(buf);

    swprintf(buf,L"NewVoxPos %.2f %.2f %.2f",newvoxpos.x,newvoxpos.y,newvoxpos.z);
    env->get_element_by_name_t<gui_static_text>("5")->set_text(buf);
}

void VoxelzApp::OnMouseKey(int32_t button, int32_t action, int32_t mod)
{
    if(action==GLFW_PRESS)
    {
        wchar_t buf[256];
        swprintf(buf,L"Total Chunks %d",chkmgr->GetChunkCount());
        env->get_element_by_name_t<gui_static_text>("6")->set_text(buf);

        swprintf(buf,L"Total Blocks %d",chkmgr->GetTotalBlocks());
        env->get_element_by_name_t<gui_static_text>("7")->set_text(buf);
        switch(button)
        {
        case GLFW_MOUSE_BUTTON_LEFT:
            if(validvoxel)
                chkmgr->SetBlock(voxpos,EBT_DEFAULT,false);
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            if(validvoxel)
                chkmgr->SetBlock(newvoxpos,EBT_GRASS,true);
            break;
        }
    }
}

