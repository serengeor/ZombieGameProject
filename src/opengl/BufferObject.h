#ifndef BUFFER_OBJECT_H
#define BUFFER_OBJECT_H

#include "IBufferObject.h"

template <class T>
struct buffer_object: public ibuffer_object
{
    std::vector<T> data;

    //void init_buffers();
    virtual void init();
    virtual void upload();

    virtual ibuffer_object::IBO_TYPE get_type(){return ibuffer_object::DATA;}
    virtual uint32_t get_data_type();
    virtual uint32_t get_component_count();
    virtual uint32_t get_size(){return data.size();}
};

template <class T>
struct index_buffer_object: public ibuffer_object
{
    std::vector<T> data;

    //void init_buffers();
    virtual void init();
    virtual void upload();

    virtual ibuffer_object::IBO_TYPE get_type(){return ibuffer_object::INDEX;}
    virtual uint32_t get_data_type();
    virtual uint32_t get_component_count();
    virtual uint32_t get_size(){return data.size();}
};

#endif // BUFFER_OBJECT_H