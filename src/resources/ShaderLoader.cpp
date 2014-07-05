#include "Precomp.h"
#include "ShaderLoader.h"
#include "utility/Logger.h"

shader_loader::shader_loader(Logger * l)
{
    _logger = l;
}

shader_loader::~shader_loader()
{
    //dtor
}

shader_ptr shader_loader::load(const std::string & vertex_file, const std::string & fragment_file)
{
    resource<shader> res;

    ///not sure about this one.
    std::string res_name = vertex_file.substr(0,vertex_file.rfind('.')) + fragment_file.substr(0,fragment_file.rfind('.'));

    res = this->get_resource(res_name);

    if(res.resource)
    {
        _logger->log(LOG_LOG, "Found shader in cache, skipping loading.");
        return res.resource;
    }

    char * vsh=NULL;
    char * fsh=NULL;

    if(!helpers::read(vertex_file,vsh)) return shader_ptr();
    if(!helpers::read(fragment_file,fsh)) return shader_ptr();

    _logger->log(LOG_LOG, "Shader name: %s", res_name.c_str());
    shader * sh = new shader(res_name,vsh,fsh);
	sh->compile();
	sh->link();

	if(sh->program)
    {
        res.resource = shader_ptr(sh);
        res.path = res_name;
        this->add_resource(res);
    }

	delete [] vsh;
	delete [] fsh;

	if(res.resource)
        _logger->log(LOG_LOG, "Shader '%s' loaded.",res_name.c_str());

    return res.resource;
}

shader_ptr shader_loader::load(const std::string & file)
{
    resource<shader> res;

    res = this->get_resource(file);

    if(res.resource)
    {
        _logger->log(LOG_LOG, "Found shader in cache, skipping loading.");
        return res.resource;
    }

    char * vsh=NULL;
    char * fsh=NULL;

    if(!helpers::read(file + ".vert",vsh)) return shader_ptr();
    if(!helpers::read(file + ".frag",fsh)) return shader_ptr();

    std::string sh_name = file.substr(file.rfind("/")+1);
    _logger->log(LOG_LOG, "Shader name: %s", sh_name.c_str());
    shader * sh = new shader(sh_name,vsh,fsh);
	sh->compile();
	//sh->link();

	if(sh->program)
    {
        res.resource = shader_ptr(sh);
        res.path = file;
        this->add_resource(res);
    }

	delete [] vsh;
	delete [] fsh;

	if(res.resource)
        _logger->log(LOG_LOG, "Shader '%s' loaded.",file.c_str());

    return res.resource;
}

shader_ptr shader_loader::get_shader_by_name(const std::string & name)
{
    for(resource<shader> & res: m_resources)
    {
        if(res.resource->name == name)
            return res.resource;
    }

    return shader_ptr();
}
