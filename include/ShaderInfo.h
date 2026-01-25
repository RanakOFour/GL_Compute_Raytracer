#ifndef SHADERFACTORY_H
#define SHADERFACTORY_H

#include "GL/glew.h"
#include "ComputeShader.h"

#include <vector>
#include <string>
#include <memory>

struct ShaderProperty
{
    std::string name;
    enum PropertyType
    {
        INT,
        FLOAT,
        BOOL,
        VEC2,
        VEC3,
        VEC4
    } type;

    union Value
    {
        int i;
        float f;
        bool b;
        float vec2[2];
        float vec3[3];
        float vec4[4];
    } value;

    GLuint location;

    ShaderProperty(std::string _name, PropertyType _type, GLuint _loc)
    {
        name = _name;
        type = _type;
        location = _loc;
    };

    ShaderProperty(std::string _name, PropertyType _type, Value _value, GLuint _loc)
    {
        name = _name;
        type = _type;
        value = _value;
        location = _loc;
    };
};

class ComputeShader;
class ShaderInfo
{
    private:
    std::string m_name;
    std::vector<ShaderProperty> m_properties;
    ComputeShader* m_shader;

    bool m_enabled;

    public:
    ShaderInfo(std::string _name, ComputeShader* _shader)
    : m_name(_name)
    , m_shader(_shader)
    , m_enabled(true)
    {
        GLint l_count = 0;
        GLuint l_programID = _shader->GetID();
        glGetProgramiv(l_programID, GL_ACTIVE_UNIFORMS, &l_count);

        char nameBuf[256];
        
        for (GLint i = 0; i < l_count; ++i) 
        {
            GLsizei len = 0;
            GLint size = 0;
            GLenum type = 0;
            glGetActiveUniform(l_programID, (GLuint)i, sizeof(nameBuf), &len, &size, &type, nameBuf);
            GLint loc = glGetUniformLocation(l_programID, nameBuf);
            printf("active uniform %d: %s size=%d type=0x%x loc=%d\n", i, nameBuf, size, type, loc);

            ShaderProperty::PropertyType propType;
            switch(type)
            {
                case GL_INT:
                    propType = ShaderProperty::INT;
                    break;
                case GL_FLOAT:
                    propType = ShaderProperty::FLOAT;
                    break;
                case GL_BOOL:
                    propType = ShaderProperty::BOOL;
                    break;
                case GL_FLOAT_VEC2:
                    propType = ShaderProperty::VEC2;
                    break;
                case GL_FLOAT_VEC3:
                    propType = ShaderProperty::VEC3;
                    break;
                case GL_FLOAT_VEC4:
                    propType = ShaderProperty::VEC4;
                    break;
                default:
                    continue; // unsupported type
            }

            ShaderProperty l_property(nameBuf, propType, loc);

            switch(propType)
            {
                case ShaderProperty::INT:
                    glGetUniformiv(l_programID, loc, &l_property.value.i);
                    break;
                case ShaderProperty::FLOAT:
                    glGetUniformfv(l_programID, loc, &l_property.value.f);
                    break;
                case ShaderProperty::BOOL:
                    glGetUniformiv(l_programID, loc, &l_property.value.i);
                    break;
                case ShaderProperty::VEC2:
                    glGetUniformfv(l_programID, loc, l_property.value.vec2);
                    break;
                case ShaderProperty::VEC3:
                    glGetUniformfv(l_programID, loc, l_property.value.vec3);
                    break;
                case ShaderProperty::VEC4:
                    glGetUniformfv(l_programID, loc, l_property.value.vec4);
                    break;
            }

            m_properties.push_back(l_property);
        }
    };

    ~ShaderInfo()
    {
        // shared_ptr will automatically delete the shader
    };

    inline std::string Name() { return m_name; };
    inline void Name(std::string _n) { m_name = _n; }

    inline void Use() { if (m_enabled && m_shader) { m_shader->use(); } };
    inline ComputeShader* Shader() { return m_shader; };

    inline bool Enabled() { return m_enabled; };
    inline void Enabled(bool _e) { m_enabled = _e; };
};

class ShaderInfoCollection
{
    private:
    std::vector<ShaderInfo> m_shaders;

    static std::shared_ptr<ShaderInfoCollection> m_selfPtr;

    public:
    static std::shared_ptr<ShaderInfoCollection> Init()
    {
        if(!m_selfPtr)
        {
            m_selfPtr = std::make_shared<ShaderInfoCollection>();
        }
        else
        {
            printf("WARNING: ShaderInfoCollection instance already exists!\n");
        }

        return m_selfPtr;
    };

    /*
    *   Adds a new shader to the collection
    */
    void AddShader(ShaderInfo _shader)
    {
        m_shaders.push_back(_shader);
    };

    static void LogShader(ComputeShader* _shader, std::string _name)
    {
        if(m_selfPtr)
        {
            m_selfPtr->AddShader(ShaderInfo(_name, _shader));
        }
        else
        {
            printf("WARNING: No ShaderInfoCollection instance exists!\n");
        }
    };

    /*
    *   Returns a reference to the vector of shaders
    */
    std::vector<ShaderInfo>& GetShaders()
    {
        return m_shaders;
    };
};

#endif