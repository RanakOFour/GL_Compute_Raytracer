/**
 * @file ShaderInfo.h
 * @brief Shader property management and uniform handling system
 * 
 * This file provides classes for managing shader uniforms, their visibility
 * in the GUI, and read-only status. It includes singleton managers for
 * hidden and read-only uniform prefixes, as well as classes for storing
 * shader property information and collections.
 */

#ifndef SHADERFACTORY_H
#define SHADERFACTORY_H

#include "GL/glew.h"
#include "ComputeShader.h"

#include <vector>
#include <string>
#include <memory>
#include <sstream>
#include <algorithm>
#include <set>

/**
 * @class HiddenUniformManager
 * @brief Singleton manager for uniform prefixes that should be hidden from the GUI
 * 
 * Manages a list of uniform name prefixes that should not be displayed in the GUI.
 * These are typically uniforms managed by other systems (Camera, Lights, Raytracer, etc.)
 * and don't need user interaction.
 */
class HiddenUniformManager
{
private:
    /** @brief Set of uniform prefixes to hide from GUI */
    std::set<std::string> m_hiddenPrefixes;
    
    /** @brief Singleton instance pointer */
    static inline std::shared_ptr<HiddenUniformManager> m_instance;

    /**
     * @brief Private constructor initializing default hidden prefixes
     */
    HiddenUniformManager()
    {
        m_hiddenPrefixes = {
            "u_camera",          ///< Camera uniforms (managed by Camera object)
            "u_lastFrameCamera", ///< Previous frame camera data
            "u_cameraPos",       ///< Camera position variant
            "u_lights",          ///< Light array (managed by Light settings GUI)
            "u_lightCount",      ///< Light count
            "u_numLights",       ///< Light count variant
            "u_resolution",      ///< Screen resolution (managed by Raytracer)
            "u_aspect",          ///< Aspect ratio (managed by Raytracer)
            "u_time",            ///< Time uniform (managed by Raytracer)
            "u_frameCount",      ///< Frame counter (managed by Raytracer)
            "u_seed",            ///< Random seed (managed by Raytracer)
        };
    }

public:
    /**
     * @brief Get the singleton instance
     * @return Shared pointer to the HiddenUniformManager instance
     */
    static std::shared_ptr<HiddenUniformManager> Get()
    {
        if (!m_instance)
        {
            m_instance = std::shared_ptr<HiddenUniformManager>(new HiddenUniformManager());
        }
        return m_instance;
    }

    /**
     * @brief Add a new prefix to the hidden list
     * @param _prefix The prefix string to add
     */
    void AddPrefix(const std::string& _prefix)
    {
        m_hiddenPrefixes.insert(_prefix);
    }

    /**
     * @brief Remove a prefix from the hidden list
     * @param _prefix The prefix string to remove
     */
    void RemovePrefix(const std::string& _prefix)
    {
        m_hiddenPrefixes.erase(_prefix);
    }

    /**
     * @brief Check if a prefix exists in the hidden list
     * @param _prefix The prefix to check
     * @return True if the prefix is in the hidden list
     */
    bool HasPrefix(const std::string& _prefix) const
    {
        return m_hiddenPrefixes.find(_prefix) != m_hiddenPrefixes.end();
    }

    /**
     * @brief Get all hidden prefixes
     * @return Const reference to the set of hidden prefixes
     */
    const std::set<std::string>& GetPrefixes() const
    {
        return m_hiddenPrefixes;
    }

    /**
     * @brief Remove all prefixes from the hidden list
     */
    void ClearPrefixes()
    {
        m_hiddenPrefixes.clear();
    }

    /**
     * @brief Reset the hidden prefixes to their default values
     */
    void ResetToDefaults()
    {
        m_hiddenPrefixes = {
            "u_camera", "u_lastFrameCamera", "u_cameraPos",
            "u_lights", "u_lightCount", "u_numLights",
            "u_resolution", "u_aspect", "u_time", "u_frameCount", "u_seed",
        };
    }

    /**
     * @brief Check if a uniform name should be hidden based on prefixes
     * @param _name The uniform name to check
     * @return True if the uniform should be hidden
     */
    bool ShouldBeHidden(const std::string& _name) const
    {
        for (const auto& prefix : m_hiddenPrefixes)
        {
            if (_name.find(prefix) == 0)
            {
                return true;
            }
        }
        return false;
    }
};

/**
 * @class ReadOnlyUniformManager
 * @brief Singleton manager for uniform prefixes that should be read-only
 * 
 * Manages a list of uniform name prefixes that should not be updated by
 * UpdateShader(). These uniforms are managed externally by other systems
 * (e.g., u_lastFrameCamera is set by the Camera system).
 */
class ReadOnlyUniformManager
{
private:
    /** @brief Set of uniform prefixes that are read-only */
    std::set<std::string> m_readOnlyPrefixes;
    
    /** @brief Singleton instance pointer */
    static inline std::shared_ptr<ReadOnlyUniformManager> m_instance;

    /**
     * @brief Private constructor initializing default read-only prefixes
     */
    ReadOnlyUniformManager()
    {
        m_readOnlyPrefixes = {
            "u_lastFrameCamera", ///< Previous frame camera (managed by Camera)
            "u_camera",          ///< Current camera (managed by Camera)
            "u_lights",          ///< Lights (managed by Raytracer)
            "u_lightCount",      ///< Light count (managed by Raytracer)
            "u_numLights",       ///< Light count variant
            "u_resolution",      ///< Resolution (managed by Raytracer)
            "u_aspect",          ///< Aspect ratio (managed by Raytracer)
            "u_time",            ///< Time (managed by Raytracer)
            "u_frameCount",      ///< Frame count (managed by Raytracer)
            "u_seed",            ///< Random seed (managed by Raytracer)
            "u_cameraPos",       ///< Camera position variant
        };
    }

public:
    /**
     * @brief Get the singleton instance
     * @return Shared pointer to the ReadOnlyUniformManager instance
     */
    static std::shared_ptr<ReadOnlyUniformManager> Get()
    {
        if (!m_instance)
        {
            m_instance = std::shared_ptr<ReadOnlyUniformManager>(new ReadOnlyUniformManager());
        }
        return m_instance;
    }

    /**
     * @brief Add a new prefix to the read-only list
     * @param _prefix The prefix string to add
     */
    void AddPrefix(const std::string& _prefix)
    {
        m_readOnlyPrefixes.insert(_prefix);
    }

    /**
     * @brief Remove a prefix from the read-only list
     * @param _prefix The prefix string to remove
     */
    void RemovePrefix(const std::string& _prefix)
    {
        m_readOnlyPrefixes.erase(_prefix);
    }

    /**
     * @brief Check if a prefix exists in the read-only list
     * @param _prefix The prefix to check
     * @return True if the prefix is in the read-only list
     */
    bool HasPrefix(const std::string& _prefix) const
    {
        return m_readOnlyPrefixes.find(_prefix) != m_readOnlyPrefixes.end();
    }

    /**
     * @brief Get all read-only prefixes
     * @return Const reference to the set of read-only prefixes
     */
    const std::set<std::string>& GetPrefixes() const
    {
        return m_readOnlyPrefixes;
    }

    /**
     * @brief Check if a uniform name should be read-only based on prefixes
     * @param _name The uniform name to check
     * @return True if the uniform should be read-only
     */
    bool ShouldBeReadOnly(const std::string& _name) const
    {
        for (const auto& prefix : m_readOnlyPrefixes)
        {
            if (_name.find(prefix) == 0)
            {
                return true;
            }
        }
        return false;
    }
};

/**
 * @struct ShaderProperty
 * @brief Represents a single shader uniform property
 * 
 * Stores information about a shader uniform including its name, type,
 * value, location, visibility in GUI, and read-only status.
 */
struct ShaderProperty
{
    /** @brief Name of the uniform */
    std::string name;
    
    /**
     * @enum PropertyType
     * @brief Supported uniform types
     */
    enum PropertyType
    {
        INT,    ///< Integer type
        FLOAT,  ///< Float type
        BOOL,   ///< Boolean type
        VEC2,   ///< 2D vector type
        VEC3,   ///< 3D vector type
        VEC4    ///< 4D vector type
    } type;

    /**
     * @union Value
     * @brief Union storing the property value based on type
     */
    union Value
    {
        int i;          ///< Integer value
        float f;        ///< Float value
        bool b;         ///< Boolean value
        float vec2[2];  ///< 2D vector value
        float vec3[3];  ///< 3D vector value
        float vec4[4];  ///< 4D vector value
    } value;

    /** @brief OpenGL uniform location */
    GLuint location;
    
    /** @brief Whether this property should be shown in the GUI */
    bool visible;
    
    /** @brief Whether this property should be skipped by UpdateShader() */
    bool readOnly;

    /**
     * @brief Constructor without initial value
     * @param _name Uniform name
     * @param _type Property type
     * @param _loc OpenGL uniform location
     */
    ShaderProperty(std::string _name, PropertyType _type, GLuint _loc)
        : name(_name)
        , type(_type)
        , location(_loc)
        , visible(!HiddenUniformManager::Get()->ShouldBeHidden(_name))
        , readOnly(ReadOnlyUniformManager::Get()->ShouldBeReadOnly(_name))
    {
    }

    /**
     * @brief Constructor with initial value
     * @param _name Uniform name
     * @param _type Property type
     * @param _value Initial value
     * @param _loc OpenGL uniform location
     */
    ShaderProperty(std::string _name, PropertyType _type, Value _value, GLuint _loc)
        : name(_name)
        , type(_type)
        , value(_value)
        , location(_loc)
        , visible(!HiddenUniformManager::Get()->ShouldBeHidden(_name))
        , readOnly(ReadOnlyUniformManager::Get()->ShouldBeReadOnly(_name))
    {
    }

    /**
     * @brief Re-evaluate visibility based on current hidden prefixes
     */
    void RefreshVisibility()
    {
        visible = !HiddenUniformManager::Get()->ShouldBeHidden(name);
    }

    /**
     * @brief Re-evaluate read-only status based on current read-only prefixes
     */
    void RefreshReadOnly()
    {
        readOnly = ReadOnlyUniformManager::Get()->ShouldBeReadOnly(name);
    }

    /**
     * @brief Manually set visibility (overrides prefix-based visibility)
     * @param _visible New visibility state
     */
    void SetVisible(bool _visible)
    {
        visible = _visible;
    }

    /**
     * @brief Manually set read-only status
     * @param _readOnly New read-only state
     */
    void SetReadOnly(bool _readOnly)
    {
        readOnly = _readOnly;
    }

    /**
     * @brief Get string representation of the property type
     * @return Type name as string
     */
    std::string Type() const
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

    /**
     * @brief Get string representation of the property value
     * @return Value as formatted string
     */
    std::string AsString() const
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
};

/**
 * @class ShaderInfo
 * @brief Stores information about a compute shader and its properties
 * 
 * Wraps a ComputeShader with metadata including name, enabled state,
 * and a collection of ShaderProperty objects representing its uniforms.
 */
class ShaderInfo
{
private:
    /** @brief Display name for the shader */
    std::string m_name;
    
    /** @brief Collection of shader properties/uniforms */
    std::vector<ShaderProperty> m_properties;
    
    /** @brief Pointer to the underlying compute shader */
    ComputeShader* m_shader;
    
    /** @brief Whether this shader is enabled for rendering */
    bool m_enabled;

public:
    /**
     * @brief Constructor that queries shader uniforms
     * @param _name Display name for the shader
     * @param _shader Pointer to the ComputeShader
     */
    ShaderInfo(std::string _name, ComputeShader* _shader)
    : m_name(_name)
    , m_shader(_shader)
    , m_enabled(true)
    {
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
                    glGetnUniformfv(l_programID, loc, 2 * sizeof(float), l_property.value.vec2);
                    break;
                case ShaderProperty::VEC3:
                    glGetnUniformfv(l_programID, loc, 3 * sizeof(float), l_property.value.vec3);
                    break;
                case ShaderProperty::VEC4:
                    glGetnUniformfv(l_programID, loc, 4 * sizeof(float), l_property.value.vec4);
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

    /** @brief Destructor */
    ~ShaderInfo() {}

    /**
     * @brief Get the shader's display name
     * @return The shader name
     */
    inline std::string Name() { return m_name; }
    
    /**
     * @brief Set the shader's display name
     * @param _n New name
     */
    inline void Name(std::string _n) { m_name = _n; }

    /**
     * @brief Activate the shader for use if enabled
     */
    inline void Use() { if (m_enabled && m_shader) { m_shader->use(); } }
    
    /**
     * @brief Get the underlying ComputeShader pointer
     * @return Pointer to the ComputeShader
     */
    inline ComputeShader* Shader() { return m_shader; }

    /**
     * @brief Update shader uniforms with stored property values
     * @note Skips properties marked as read-only since they are managed externally
     */
    inline void UpdateShader()
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

    /**
     * @brief Check if the shader is enabled
     * @return True if enabled
     */
    inline bool Enabled() { return m_enabled; }
    
    /**
     * @brief Set the enabled state
     * @param _e New enabled state
     */
    inline void Enabled(bool _e) { m_enabled = _e; }

    /**
     * @brief Get all properties
     * @return Reference to the properties vector
     */
    inline std::vector<ShaderProperty>& GetProperties() { return m_properties; }

    /**
     * @brief Get only the properties that should be visible in the GUI
     * @return Vector of pointers to visible properties
     */
    inline std::vector<ShaderProperty*> GetVisibleProperties()
    {
        std::vector<ShaderProperty*> visibleProps;
        for (auto& prop : m_properties)
        {
            if (prop.visible)
            {
                visibleProps.push_back(&prop);
            }
        }
        return visibleProps;
    }

    /**
     * @brief Re-evaluate visibility for all properties based on current hidden prefixes
     */
    inline void RefreshPropertyVisibility()
    {
        for (ShaderProperty& prop : m_properties)
        {
            prop.RefreshVisibility();
        }
    }

    /**
     * @brief Set visibility for a specific property by name
     * @param _name Property name to find
     * @param _visible New visibility state
     * @return True if property was found and updated
     */
    inline bool SetPropertyVisibility(const std::string& _name, bool _visible)
    {
        for (auto& prop : m_properties)
        {
            if (prop.name == _name)
            {
                prop.SetVisible(_visible);
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Get a property by name
     * @param _name Property name to find
     * @return Pointer to the property, or nullptr if not found
     */
    inline ShaderProperty* GetProperty(const std::string& _name)
    {
        for (auto& prop : m_properties)
        {
            if (prop.name == _name)
            {
                return &prop;
            }
        }
        return nullptr;
    }
};

/**
 * @class ShaderInfoCollection
 * @brief Singleton collection managing multiple ShaderInfo objects
 * 
 * Provides functionality to add, remove, load, and manage multiple shaders.
 * Supports both instance and static methods for flexibility.
 */
class ShaderInfoCollection
{
private:
    /** @brief Collection of managed shaders */
    std::vector<ShaderInfo> m_shaders;
    
    /** @brief Singleton instance pointer */
    static inline std::shared_ptr<ShaderInfoCollection> m_selfPtr;

    /**
     * @brief Private constructor for singleton pattern
     */
    ShaderInfoCollection() : m_shaders() {}

public:
    /** @brief Destructor */
    ~ShaderInfoCollection() {}

    /**
     * @brief Initialize the singleton instance
     * @return Shared pointer to the instance
     */
    static std::shared_ptr<ShaderInfoCollection> Init()
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

    /**
     * @brief Get the singleton instance
     * @return Shared pointer to the instance
     */
    static std::shared_ptr<ShaderInfoCollection> Get()
    {
        return m_selfPtr;
    }

    /**
     * @brief Add a shader to the collection
     * @param _shader ShaderInfo to add
     */
    void AddShader(ShaderInfo _shader)
    {
        m_shaders.push_back(_shader);
    }

    /**
     * @brief Load and add a new shader from file at runtime
     * @param _path Path to the shader file
     * @param _name Display name for the shader
     * @return Pointer to the new ShaderInfo, or nullptr on failure
     */
    ShaderInfo* LoadShader(const std::string& _path, const std::string& _name)
    {
        ComputeShader* shader = new ComputeShader(_path.c_str());
        if (shader->GetID() == 0)
        {
            printf("ERROR: Failed to load shader from %s\n", _path.c_str());
            delete shader;
            return nullptr;
        }
        
        m_shaders.push_back(ShaderInfo(_name, shader));
        printf("Loaded shader: %s from %s\n", _name.c_str(), _path.c_str());
        return &m_shaders.back();
    }

    /**
     * @brief Remove a shader from the collection by name
     * @param _name Name of the shader to remove
     * @return True if shader was found and removed
     */
    bool RemoveShader(const std::string& _name)
    {
        for (auto it = m_shaders.begin(); it != m_shaders.end(); ++it)
        {
            if (it->Name() == _name)
            {
                m_shaders.erase(it);
                printf("Removed shader: %s\n", _name.c_str());
                return true;
            }
        }
        printf("WARNING: Shader '%s' not found for removal\n", _name.c_str());
        return false;
    }

    /**
     * @brief Remove a shader from the collection by index
     * @param _index Index of the shader to remove
     * @return True if index was valid and shader was removed
     */
    bool RemoveShaderAt(size_t _index)
    {
        if (_index >= m_shaders.size())
        {
            printf("WARNING: Invalid shader index %zu\n", _index);
            return false;
        }
        std::string name = m_shaders[_index].Name();
        m_shaders.erase(m_shaders.begin() + _index);
        printf("Removed shader at index %zu: %s\n", _index, name.c_str());
        return true;
    }

    /**
     * @brief Get a shader by name
     * @param _name Name of the shader to find
     * @return Pointer to the shader, or nullptr if not found
     */
    ShaderInfo* GetShader(const std::string& _name)
    {
        for (auto& shader : m_shaders)
        {
            if (shader.Name() == _name)
            {
                return &shader;
            }
        }
        return nullptr;
    }

    /**
     * @brief Refresh visibility for all properties in all shaders
     */
    void RefreshAllVisibility()
    {
        for (auto& shader : m_shaders)
        {
            shader.RefreshPropertyVisibility();
        }
    }

    /**
     * @brief Reorder shaders by moving one from one index to another
     * @param _fromIndex Source index
     * @param _toIndex Destination index
     * @return True if reorder was successful
     */
    bool ReorderShader(size_t _fromIndex, size_t _toIndex)
    {
        if (_fromIndex >= m_shaders.size() || _toIndex >= m_shaders.size())
        {
            return false;
        }
        
        if (_fromIndex == _toIndex)
        {
            return true;
        }

        ShaderInfo shader = std::move(m_shaders[_fromIndex]);
        m_shaders.erase(m_shaders.begin() + _fromIndex);
        m_shaders.insert(m_shaders.begin() + _toIndex, std::move(shader));
        return true;
    }

    /**
     * @brief Static method to log a shader to the collection
     * @param _shader Pointer to the ComputeShader
     * @param _name Display name for the shader
     */
    static void LogShader(ComputeShader* _shader, std::string _name)
    {
        if (m_selfPtr)
        {
            m_selfPtr->AddShader(ShaderInfo(_name, _shader));
        }
        else
        {
            printf("WARNING: No ShaderInfoCollection instance exists!\n");
        }
    }

    /**
     * @brief Static method to load a shader
     * @param _path Path to the shader file
     * @param _name Display name for the shader
     * @return Pointer to the new ShaderInfo, or nullptr on failure
     */
    static ShaderInfo* LoadShaderStatic(const std::string& _path, const std::string& _name)
    {
        if (m_selfPtr)
        {
            return m_selfPtr->LoadShader(_path, _name);
        }
        printf("WARNING: No ShaderInfoCollection instance exists!\n");
        return nullptr;
    }

    /**
     * @brief Static method to remove a shader by name
     * @param _name Name of the shader to remove
     * @return True if shader was found and removed
     */
    static bool RemoveShaderStatic(const std::string& _name)
    {
        if (m_selfPtr)
        {
            return m_selfPtr->RemoveShader(_name);
        }
        printf("WARNING: No ShaderInfoCollection instance exists!\n");
        return false;
    }

    /**
     * @brief Static method to refresh visibility for all shaders
     */
    static void RefreshAllVisibilityStatic()
    {
        if (m_selfPtr)
        {
            m_selfPtr->RefreshAllVisibility();
        }
    }

    /**
     * @brief Print all shaders and their properties to stdout
     */
    static void PrintAllShaders()
    {
        if (m_selfPtr)
        {
            for (ShaderInfo& shader : m_selfPtr->m_shaders)
            {
                printf("Shader: %s\n", shader.Name().c_str());
                for (ShaderProperty& prop : shader.GetProperties())
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

    /**
     * @brief Get reference to the vector of shaders
     * @return Reference to the shaders vector
     */
    std::vector<ShaderInfo>& GetShaders()
    {
        return m_shaders;
    }

    /**
     * @brief Get the number of shaders in the collection
     * @return Number of shaders
     */
    size_t GetShaderCount() const
    {
        return m_shaders.size();
    }
};

#endif
