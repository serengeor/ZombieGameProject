#ifndef ISG_OBJECT_H
#define ISG_OBJECT_H

#include "sg_object_types.h"
#include "sg_material.h"
#include "sg_aabb.h"

namespace sg
{
class scenegraph;
// TODO (Ernestas#1#): implement parenting, separate out position, rotation, scale components to vec3 and combine them to mat4 when changed
enum
{
    SGOF_TRANSFORM_OUTDATED=BIT0,
};
class isg_object
{
public:
    isg_object(scenegraph * sg);
    isg_object(const isg_object & sgo)= delete;

    virtual ~isg_object();

    const glm::vec3 & get_position() const;
    const glm::vec3 & get_rotation() const;
    const glm::vec3 & get_scale() const;

    virtual void set_position(const glm::vec3 & v);
    virtual void set_rotation(const glm::vec3 & v);
    virtual void set_scale(const glm::vec3 & v);

    const glm::mat4x4 & get_absolute_transform();

    std::string &   get_name();
    const sg_aabb & get_aabb();

public:
    virtual uint32_t get_type() = 0;

    virtual void update(float delta_time){};
    virtual void render(scenegraph * sg)=0;

    virtual sg_material_ptr get_material(uint32_t index)=0;
    virtual uint32_t get_material_count()=0;

    glm::mat4x4 get_relative_transform();
    void update_absolute_transform();

protected:
    uint32_t m_flags;
    glm::vec3 m_position, m_rotation, m_scale;
    glm::mat4x4 m_absolute_transform;

    std::string m_name;
    sg_aabb     m_aabb;
    scenegraph * m_scenegraph;
};

typedef std::shared_ptr<isg_object> sg_object_ptr;
}

#endif // ISG_OBJECT_H