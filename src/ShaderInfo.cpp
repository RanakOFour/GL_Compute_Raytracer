/**
 * @file ShaderInfo.cpp
 * @brief Implementation of shader info management classes
 */

#include "ShaderInfo.h"
#include <cstdio>

//=============================================================================
// HiddenUniformManager Implementation
//=============================================================================

HiddenUniformManager::HiddenUniformManager()
{
    m_hiddenPrefixes = {
        "u_camera",          
        "u_lastFrameCamera", 
        "u_cameraPos",       
        "u_lights",          
        "u_lightCount",      
        "u_numLights",       
        "u_resolution",      
        "u_aspect",          
        "u_time",            
        "u_frameCount",      
        "u_seed",            
    };
}

std::shared_ptr<HiddenUniformManager> HiddenUniformManager::Get()
{
    if (!m_instance)
    {
        m_instance = std::shared_ptr<HiddenUniformManager>(new HiddenUniformManager());
    }
    return m_instance;
}

void HiddenUniformManager::AddPrefix(const std::string& _prefix)
{
    m_hiddenPrefixes.insert(_prefix);
}

void HiddenUniformManager::RemovePrefix(const std::string& _prefix)
{
    m_hiddenPrefixes.erase(_prefix);
}

bool HiddenUniformManager::HasPrefix(const std::string& _prefix) const
{
    return m_hiddenPrefixes.find(_prefix) != m_hiddenPrefixes.end();
}

const std::set<std::string>& HiddenUniformManager::GetPrefixes() const
{
    return m_hiddenPrefixes;
}

void HiddenUniformManager::ClearPrefixes()
{
    m_hiddenPrefixes.clear();
}

void HiddenUniformManager::ResetToDefaults()
{
    m_hiddenPrefixes = {
        "u_camera", "u_lastFrameCamera", "u_cameraPos",
        "u_lights", "u_lightCount", "u_numLights",
        "u_resolution", "u_aspect", "u_time", "u_frameCount", "u_seed",
    };
}

bool HiddenUniformManager::ShouldBeHidden(const std::string& _name) const
{
    for (const std::string& prefix : m_hiddenPrefixes)
    {
        if (_name.find(prefix) == 0)
        {
            return true;
        }
    }
    return false;
}

//=============================================================================
// ReadOnlyUniformManager Implementation
//=============================================================================

ReadOnlyUniformManager::ReadOnlyUniformManager()
{
    m_readOnlyPrefixes = {
        "u_lastFrameCamera", 
        "u_camera",          
        "u_lights",          
        "u_lightCount",      
        "u_numLights",       
        "u_resolution",      
        "u_aspect",          
        "u_time",            
        "u_frameCount",      
        "u_seed",            
        "u_cameraPos",       
    };
}

std::shared_ptr<ReadOnlyUniformManager> ReadOnlyUniformManager::Get()
{
    if (!m_instance)
    {
        m_instance = std::shared_ptr<ReadOnlyUniformManager>(new ReadOnlyUniformManager());
    }
    return m_instance;
}

void ReadOnlyUniformManager::AddPrefix(const std::string& _prefix)
{
    m_readOnlyPrefixes.insert(_prefix);
}

void ReadOnlyUniformManager::RemovePrefix(const std::string& _prefix)
{
    m_readOnlyPrefixes.erase(_prefix);
}

bool ReadOnlyUniformManager::HasPrefix(const std::string& _prefix) const
{
    return m_readOnlyPrefixes.find(_prefix) != m_readOnlyPrefixes.end();
}

const std::set<std::string>& ReadOnlyUniformManager::GetPrefixes() const
{
    return m_readOnlyPrefixes;
}

bool ReadOnlyUniformManager::ShouldBeReadOnly(const std::string& _name) const
{
    for (const std::string& prefix : m_readOnlyPrefixes)
    {
        if (_name.find(prefix) == 0)
        {
            return true;
        }
    }
    return false;
}

//=============================================================================
// ShaderProperty Implementation
//=============================================================================

ShaderProperty::ShaderProperty(std::string _name, PropertyType _type, GLuint _loc)
: name(_name)
, type(_type)
, value{}
, location(_loc)
, visible(!HiddenUniformManager::Get()->ShouldBeHidden(_name))
, readOnly(ReadOnlyUniformManager::Get()->ShouldBeReadOnly(_name))
{
}

ShaderProperty::ShaderProperty(std::string _name, PropertyType _type, Value _value, GLuint _loc)
: name(_name)
, type(_type)
, value(_value)
, location(_loc)
, visible(!HiddenUniformManager::Get()->ShouldBeHidden(_name))
, readOnly(ReadOnlyUniformManager::Get()->ShouldBeReadOnly(_name))
{
}

void ShaderProperty::RefreshVisibility()
{
    visible = !HiddenUniformManager::Get()->ShouldBeHidden(name);
}

void ShaderProperty::RefreshReadOnly()
{
    readOnly = ReadOnlyUniformManager::Get()->ShouldBeReadOnly(name);
}

void ShaderProperty::SetVisible(bool _visible)
{
    visible = _visible;
}

void ShaderProperty::SetReadOnly(bool _readOnly)
{
    readOnly = _readOnly;
}

std::string ShaderProperty::Type() const
{
    switch (type)
    {
        case INT:   return "INT";
        case FLOAT: return "FLOAT";
        case BOOL:  return "BOOL";
        case VEC2:  return "VEC2";
        case VEC3:  return "VEC3";
        case VEC4:  return "VEC4";
        default:    return "UNKNOWN";
    }
}

std::string ShaderProperty::AsString() const
{
    std::ostringstream oss;
    switch (type)
    {
        case INT:
            oss << value.i;
            break;
        case FLOAT:
            oss << value.f;
            break;
        case BOOL:
            oss << (value.b ? "true" : "false");
            break;
        case VEC2:
            oss << "(" << value.vec2[0] << ", " << value.vec2[1] << ")";
            break;
        case VEC3:
            oss << "(" << value.vec3[0] << ", " << value.vec3[1] << ", " << value.vec3[2] << ")";
            break;
        case VEC4:
            oss << "(" << value.vec4[0] << ", " << value.vec4[1] << ", " << value.vec4[2] << ", " << value.vec4[3] << ")";
            break;
    }
    return oss.str();
}

//=============================================================================
// ShaderInfo Implementation
//=============================================================================

ShaderInfo::ShaderInfo(std::string _name, ComputeShader* _shader)
    : m_name(_name)
    , m_shader(_shader)
    , m_enabled(true)
{
    printf("Initializing ShaderInfo for shader: %s\n", m_name.c_str());
    GLint l_count = 0;
    GLuint l_programID = _shader->GetID();
    glGetProgramiv(l_programID, GL_ACTIVE_UNIFORMS, &l_count);

    char l_nameBuf[256];
    
    for (GLint i = 0; i < l_count; ++i) 
    {
        GLsizei len = 0;
        GLint size = 0;
        GLenum type = 0;
        glGetActiveUniform(l_programID, (GLuint)i, sizeof(l_nameBuf), &len, &size, &type, l_nameBuf);
        GLint loc = glGetUniformLocation(l_programID, l_nameBuf);

        ShaderProperty::PropertyType propType;
        switch(type)
        {
            case GL_INT:        propType = ShaderProperty::INT;   break;
            case GL_FLOAT:      propType = ShaderProperty::FLOAT; break;
            case GL_BOOL:       propType = ShaderProperty::BOOL;  break;
            case GL_FLOAT_VEC2: propType = ShaderProperty::VEC2;  break;
            case GL_FLOAT_VEC3: propType = ShaderProperty::VEC3;  break;
            case GL_FLOAT_VEC4: propType = ShaderProperty::VEC4;  break;
            default: continue;
        }

        ShaderProperty l_property(l_nameBuf, propType, loc);

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
                glGetnUniformfv(l_programID, loc, 2 * sizeof(float), &l_property.value.vec2[0]);
                break;
            case ShaderProperty::VEC3:
                glGetnUniformfv(l_programID, loc, 3 * sizeof(float), &l_property.value.vec3[0]);
                break;
            case ShaderProperty::VEC4:
                glGetnUniformfv(l_programID, loc, 4 * sizeof(float), &l_property.value.vec4[0]);
                break;
        }

        m_properties.push_back(l_property);

        printf("Logged uniform: %s (Type: %s, Location: %d, Value: %s)\n", 
            l_property.name.c_str(), 
            l_property.Type().c_str(), 
            l_property.location,
            l_property.AsString().c_str()
        );
    }
}

ShaderInfo::~ShaderInfo()
{
    printf("Destroying ShaderInfo for shader: %s\n Final values: \n", m_name.c_str());
    for (const ShaderProperty& prop : m_properties)
    {
        switch(prop.type)
        {
            case ShaderProperty::INT:
                glGetUniformiv(m_shader->GetID(), prop.location, (GLint*)&prop.value.i);
                break;
            case ShaderProperty::FLOAT:
                glGetUniformfv(m_shader->GetID(), prop.location, (GLfloat*)&prop.value.f);
                break;
            case ShaderProperty::BOOL:
                glGetUniformiv(m_shader->GetID(), prop.location, (GLint*)&prop.value.i);
                break;
            case ShaderProperty::VEC2:
                glGetnUniformfv(m_shader->GetID(), prop.location, 2 * sizeof(float), (GLfloat*)&prop.value.vec2[0]);
                break;
            case ShaderProperty::VEC3:
                glGetnUniformfv(m_shader->GetID(), prop.location, 3 * sizeof(float), (GLfloat*)&prop.value.vec3[0]);
                break;
            case ShaderProperty::VEC4:
                glGetnUniformfv(m_shader->GetID(), prop.location, 4 * sizeof(float), (GLfloat*)&prop.value.vec4[0]);
                break;
        }

        printf("  %s: %s\n", prop.name.c_str(), prop.AsString().c_str());
    }
}

std::string ShaderInfo::Name()
{
    return m_name;
}

void ShaderInfo::Name(std::string _n)
{
    m_name = _n;
}

void ShaderInfo::Use()
{
    if (m_enabled && m_shader)
    {
        m_shader->use();
    }
}

ComputeShader* ShaderInfo::Shader()
{
    return m_shader.get();
}

void ShaderInfo::UpdateShader()
{
    Use();
    for (const ShaderProperty& prop : m_properties)
    {
        if (prop.readOnly)
        {
            continue;
        }

        switch (prop.type)
        {
            case ShaderProperty::INT:
                m_shader->SetUniform(prop.name, prop.value.i);
                break;
            case ShaderProperty::FLOAT:
                m_shader->SetUniform(prop.name, prop.value.f);
                break;
            case ShaderProperty::BOOL:
                m_shader->SetUniform(prop.name, prop.value.b);
                break;
            case ShaderProperty::VEC2:
                m_shader->SetUniform(prop.name, glm::vec2(prop.value.vec2[0], prop.value.vec2[1]));
                break;
            case ShaderProperty::VEC3:
                m_shader->SetUniform(prop.name, glm::vec3(prop.value.vec3[0], prop.value.vec3[1], prop.value.vec3[2]));
                break;
            case ShaderProperty::VEC4:
                m_shader->SetUniform(prop.name, glm::vec4(prop.value.vec4[0], prop.value.vec4[1], prop.value.vec4[2], prop.value.vec4[3]));
                break;
        }
    }
}

bool ShaderInfo::Enabled()
{
    return m_enabled;
}

void ShaderInfo::Enabled(bool _e)
{
    m_enabled = _e;
}

std::vector<ShaderProperty>& ShaderInfo::GetProperties()
{
    return m_properties;
}

std::vector<ShaderProperty*> ShaderInfo::GetVisibleProperties()
{
    std::vector<ShaderProperty*> visibleProps;
    for (ShaderProperty& prop : m_properties)
    {
        if (prop.visible)
        {
            visibleProps.push_back(&prop);
        }
    }
    return visibleProps;
}

void ShaderInfo::RefreshPropertyVisibility()
{
    for (ShaderProperty& prop : m_properties)
    {
        prop.RefreshVisibility();
    }
}

bool ShaderInfo::SetPropertyVisibility(const std::string& _name, bool _visible)
{
    for (ShaderProperty& prop : m_properties)
    {
        if (prop.name == _name)
        {
            prop.SetVisible(_visible);
            return true;
        }
    }
    return false;
}

ShaderProperty* ShaderInfo::GetProperty(const std::string& _name)
{
    for (ShaderProperty& prop : m_properties)
    {
        if (prop.name == _name)
        {
            return &prop;
        }
    }
    return nullptr;
}

//=============================================================================
// ShaderInfoCollection Implementation
//=============================================================================

ShaderInfoCollection::ShaderInfoCollection()
    : m_shaders()
{
}

ShaderInfoCollection::~ShaderInfoCollection()
{
}

std::shared_ptr<ShaderInfoCollection> ShaderInfoCollection::Init()
{
    if (!m_selfPtr)
    {
        ShaderInfoCollection* l_instance = new ShaderInfoCollection();
        m_selfPtr.reset(l_instance);
    }
    else
    {
        printf("WARNING: ShaderInfoCollection instance already exists!\n");
    }
    return m_selfPtr;
}

std::shared_ptr<ShaderInfoCollection> ShaderInfoCollection::Get()
{
    return m_selfPtr;
}

void ShaderInfoCollection::AddShader(std::shared_ptr<ShaderInfo> _shader)
{
    m_shaders.push_back(_shader);
}

std::weak_ptr<ShaderInfo> ShaderInfoCollection::LoadShader(const std::string& _path, const std::string& _name)
{
    ComputeShader* shader = new ComputeShader(_path.c_str());
    if (shader->GetID() == 0)
    {
        printf("ERROR: Failed to load shader from %s\n", _path.c_str());
        delete shader;
        return std::weak_ptr<ShaderInfo>();
    }
    
    m_shaders.push_back(std::make_shared<ShaderInfo>(_name, shader));
    printf("Loaded shader: %s from %s\n", _name.c_str(), _path.c_str());
    return m_shaders.back();
}

bool ShaderInfoCollection::RemoveShader(const std::string& _name)
{
    for (auto it = m_shaders.begin(); it != m_shaders.end(); ++it)
    {
        if ((*it)->Name() == _name)
        {
            m_shaders.erase(it);
            printf("Removed shader: %s\n", _name.c_str());
            return true;
        }
    }
    printf("WARNING: Shader '%s' not found for removal\n", _name.c_str());
    return false;
}

bool ShaderInfoCollection::RemoveShaderAt(size_t _index)
{
    if (_index >= m_shaders.size())
    {
        printf("WARNING: Invalid shader index %zu\n", _index);
        return false;
    }
    std::string name = m_shaders[_index]->Name();
    m_shaders.erase(m_shaders.begin() + _index);
    printf("Removed shader at index %zu: %s\n", _index, name.c_str());
    return true;
}

std::weak_ptr<ShaderInfo> ShaderInfoCollection::GetShader(const std::string& _name)
{
    for (std::shared_ptr<ShaderInfo> shader : m_shaders)
    {
        if (shader->Name() == _name)
        {
            return shader;
        }
    }
    return std::weak_ptr<ShaderInfo>();
}

void ShaderInfoCollection::RefreshAllVisibility()
{
    for (std::shared_ptr<ShaderInfo> shader : m_shaders)
    {
        shader->RefreshPropertyVisibility();
    }
}

bool ShaderInfoCollection::ReorderShader(size_t _fromIndex, size_t _toIndex)
{
    if (_fromIndex >= m_shaders.size() || _toIndex >= m_shaders.size())
    {
        return false;
    }
    
    if (_fromIndex == _toIndex)
    {
        return true;
    }

    std::shared_ptr<ShaderInfo> shader = std::move(m_shaders[_fromIndex]);
    m_shaders.erase(m_shaders.begin() + _fromIndex);
    m_shaders.insert(m_shaders.begin() + _toIndex, std::move(shader));
    return true;
}

void ShaderInfoCollection::LogShader(ComputeShader* _shader, std::string _name)
{
    if (m_selfPtr)
    {
        m_selfPtr->AddShader(std::make_shared<ShaderInfo>(_name, _shader));
    }
    else
    {
        printf("WARNING: No ShaderInfoCollection instance exists!\n");
    }
}

std::weak_ptr<ShaderInfo> ShaderInfoCollection::Load(const std::string& _path, const std::string& _name)
{
    if (m_selfPtr)
    {
        return m_selfPtr->LoadShader(_path, _name);
    }

    printf("WARNING: No ShaderInfoCollection instance exists!\n");
    return std::weak_ptr<ShaderInfo>();
}

bool ShaderInfoCollection::RemoveShaderStatic(const std::string& _name)
{
    if (m_selfPtr)
    {
        return m_selfPtr->RemoveShader(_name);
    }
    printf("WARNING: No ShaderInfoCollection instance exists!\n");
    return false;
}

void ShaderInfoCollection::RefreshAllVisibilityStatic()
{
    if (m_selfPtr)
    {
        m_selfPtr->RefreshAllVisibility();
    }
}

void ShaderInfoCollection::PrintAllShaders()
{
    if (m_selfPtr)
    {
        for (std::shared_ptr<ShaderInfo> shader : m_selfPtr->m_shaders)
        {
            printf("Shader: %s\n", shader->Name().c_str());
            for (ShaderProperty& prop : shader->GetProperties())
            {
                printf(" - Property: %s (visible: %s)\n", 
                    prop.name.c_str(), 
                    prop.visible ? "true" : "false");
            }
        }
    }
    else
    {
        printf("WARNING: No ShaderInfoCollection instance exists!\n");
    }
}

std::vector<std::shared_ptr<ShaderInfo>>& ShaderInfoCollection::GetShaders()
{
    return m_shaders;
}

size_t ShaderInfoCollection::GetShaderCount() const
{
    return m_shaders.size();
}
