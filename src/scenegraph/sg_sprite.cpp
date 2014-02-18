#include "precomp.h"
#include "sg_sprite.h"
#include "scenegraph.h"
#include "sg_graphics_manager.h"
#include "opengl/mesh.h"
#include "opengl/buffer_object.h"
namespace sg
{

sg_sprite::sg_sprite(scenegraph * sg): isg_object(sg)
{
    m_mesh = mesh_ptr(new mesh());
    buffer_object<glm::vec3> * buf = new buffer_object<glm::vec3>();
    m_mesh->buffers[mesh::POSITION] = buf;

    buf->data.push_back(glm::vec3(-0.5, 0.5,0));
    buf->data.push_back(glm::vec3(-0.5,-0.5,0));
    buf->data.push_back(glm::vec3( 0.5, 0.5,0));

    buf->data.push_back(glm::vec3( 0.5, 0.5,0));
    buf->data.push_back(glm::vec3(-0.5,-0.5,0));
    buf->data.push_back(glm::vec3( 0.5,-0.5,0));

    m_mesh->init();

    m_material=std::static_pointer_cast<sg_material_point_sprite>(this->m_scenegraph->get_graphics_manager()->create_material(SGMT_POINT_SPRITE));
}

sg_sprite::~sg_sprite()
{
    //dtor
}
void sg_sprite::render(scenegraph * sg)
{
    sg_material_point_sprite * mat = m_material.get();
    glm::mat4x4 ViewMatrix = sg->get_active_camera()->get_absolute_transform();
    const glm::mat4x4 & bpos = get_absolute_transform();

    ///set uniforms
    mat->vp = sg->get_active_camera()->get_projection() * sg->get_active_camera()->get_absolute_transform();
    mat->cam_right  = glm::vec3(glm::vec4(1,0,0,1)*ViewMatrix);
    mat->cam_up     = glm::vec3(glm::vec4(0,1,0,1)*ViewMatrix);
    mat->pos        = glm::vec3(bpos[3][0], bpos[3][1], bpos[3][2] );
    mat->size       = glm::vec3(5,5,5);

    sg->on_set_material(m_material);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_mesh->render_triangle_strip();
    glDisable(GL_BLEND);
}

sg_aabb sg_sprite::get_aabb()
{
    return m_aabb;
}

uint32_t sg_sprite::get_type()
{
    return SGO_POINT_SPRITE;
}

sg_material_ptr sg_sprite::get_material(uint32_t index)
{
    return m_material;
}

uint32_t        sg_sprite::get_material_count()
{
    return 1;
}

void            sg_sprite::recalculate_aabb()
{
    if(m_mesh)
    {
        buffer_object<glm::vec3> * bo = static_cast<buffer_object<glm::vec3> *>(m_mesh->buffers[0]);

        if(bo->data.size()>0)
        {
            m_aabb.reset(bo->data[0]);

            for(uint32_t i = 1; i < bo->data.size(); i++)
            {
                m_aabb.add_point(bo->data[i]);
            }
        }
    }
    else
        m_aabb.reset(glm::vec3());
}

}
