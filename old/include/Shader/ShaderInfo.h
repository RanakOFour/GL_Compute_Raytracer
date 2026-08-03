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
#include "GLM/glm.hpp"
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
    HiddenUniformManager();

public:
    /**
     * @brief Get the singleton instance
     * @return Shared pointer to the HiddenUniformManager instance
     */
    static std::shared_ptr<HiddenUniformManager> Get();

    /**
     * @brief Add a new prefix to the hidden list
     * @param _prefix The prefix string to add
     */
    void AddPrefix(const std::string& _prefix);

    /**
     * @brief Remove a prefix from the hidden list
     * @param _prefix The prefix string to remove
     */
    void RemovePrefix(const std::string& _prefix);

    /**
     * @brief Check if a prefix exists in the hidden list
     * @param _prefix The prefix to check
     * @return True if the prefix is in the hidden list
     */
    bool HasPrefix(const std::string& _prefix) const;

    /**
     * @brief Get all hidden prefixes
     * @return Const reference to the set of hidden prefixes
     */
    const std::set<std::string>& GetPrefixes() const;

    /**
     * @brief Remove all prefixes from the hidden list
     */
    void ClearPrefixes();

    /**
     * @brief Reset the hidden prefixes to their default values
     */
    void ResetToDefaults();

    /**
     * @brief Check if a uniform name should be hidden based on prefixes
     * @param _name The uniform name to check
     * @return True if the uniform should be hidden
     */
    bool ShouldBeHidden(const std::string& _name) const;
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
    ReadOnlyUniformManager();

public:
    /**
     * @brief Get the singleton instance
     * @return Shared pointer to the ReadOnlyUniformManager instance
     */
    static std::shared_ptr<ReadOnlyUniformManager> Get();

    /**
     * @brief Add a new prefix to the read-only list
     * @param _prefix The prefix string to add
     */
    void AddPrefix(const std::string& _prefix);

    /**
     * @brief Remove a prefix from the read-only list
     * @param _prefix The prefix string to remove
     */
    void RemovePrefix(const std::string& _prefix);

    /**
     * @brief Check if a prefix exists in the read-only list
     * @param _prefix The prefix to check
     * @return True if the prefix is in the read-only list
     */
    bool HasPrefix(const std::string& _prefix) const;

    /**
     * @brief Get all read-only prefixes
     * @return Const reference to the set of read-only prefixes
     */
    const std::set<std::string>& GetPrefixes() const;

    /**
     * @brief Check if a uniform name should be read-only based on prefixes
     * @param _name The uniform name to check
     * @return True if the uniform should be read-only
     */
    bool ShouldBeReadOnly(const std::string& _name) const;
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
        glm::vec2 vec2;  ///< 2D vector value
        glm::vec3 vec3;  ///< 3D vector value
        glm::vec4 vec4;  ///< 4D vector value
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
    ShaderProperty(std::string _name, PropertyType _type, GLuint _loc);

    /**
     * @brief Constructor with initial value
     * @param _name Uniform name
     * @param _type Property type
     * @param _value Initial value
     * @param _loc OpenGL uniform location
     */
    ShaderProperty(std::string _name, PropertyType _type, Value _value, GLuint _loc);

    /**
     * @brief Re-evaluate visibility based on current hidden prefixes
     */
    void RefreshVisibility();

    /**
     * @brief Re-evaluate read-only status based on current read-only prefixes
     */
    void RefreshReadOnly();

    /**
     * @brief Manually set visibility (overrides prefix-based visibility)
     * @param _visible New visibility state
     */
    void SetVisible(bool _visible);

    /**
     * @brief Manually set read-only status
     * @param _readOnly New read-only state
     */
    void SetReadOnly(bool _readOnly);

    /**
     * @brief Get string representation of the property type
     * @return Type name as string
     */
    std::string Type() const;

    /**
     * @brief Get string representation of the property value
     * @return Value as formatted string
     */
    std::string AsString() const;
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
    std::unique_ptr<ComputeShader> m_shader;
    
    /** @brief Whether this shader is enabled for rendering */
    bool m_enabled;

public:
    /**
     * @brief Constructor that queries shader uniforms
     * @param _name Display name for the shader
     * @param _shader Pointer to the ComputeShader
     */
    ShaderInfo(std::string _name, ComputeShader* _shader);

    /** @brief Destructor */
    ~ShaderInfo();

    /**
     * @brief Get the shader's display name
     * @return The shader name
     */
    std::string Name();
    
    /**
     * @brief Set the shader's display name
     * @param _n New name
     */
    void Name(std::string _n);

    /**
     * @brief Activate the shader for use if enabled
     */
    void Use();
    
    /**
     * @brief Get the underlying ComputeShader pointer
     * @return Pointer to the ComputeShader
     */
    ComputeShader* Shader();

    /**
     * @brief Update shader uniforms with stored property values
     * @note Skips properties marked as read-only since they are managed externally
     */
    void UpdateShader();

    /**
     * @brief Check if the shader is enabled
     * @return True if enabled
     */
    bool Enabled();
    
    /**
     * @brief Set the enabled state
     * @param _e New enabled state
     */
    void Enabled(bool _e);

    /**
     * @brief Get all properties
     * @return Reference to the properties vector
     */
    std::vector<ShaderProperty>& GetProperties();

    /**
     * @brief Get only the properties that should be visible in the GUI
     * @return Vector of pointers to visible properties
     */
    std::vector<ShaderProperty*> GetVisibleProperties();

    /**
     * @brief Re-evaluate visibility for all properties based on current hidden prefixes
     */
    void RefreshPropertyVisibility();

    /**
     * @brief Set visibility for a specific property by name
     * @param _name Property name to find
     * @param _visible New visibility state
     * @return True if property was found and updated
     */
    bool SetPropertyVisibility(const std::string& _name, bool _visible);

    /**
     * @brief Get a property by name
     * @param _name Property name to find
     * @return Pointer to the property, or nullptr if not found
     */
    ShaderProperty* GetProperty(const std::string& _name);
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
    std::vector<std::shared_ptr<ShaderInfo>> m_shaders;
    
    /** @brief Singleton instance pointer */
    static inline std::shared_ptr<ShaderInfoCollection> m_selfPtr;

    /**
     * @brief Private constructor for singleton pattern
     */
    ShaderInfoCollection();

    /**
     * @brief Load and add a new shader from file at runtime
     * @param _path Path to the shader file
     * @param _name Display name for the shader
     * @return Pointer to the new ShaderInfo, or nullptr on failure
     */
    std::weak_ptr<ShaderInfo> LoadShader(const std::string& _path, const std::string& _name);

public:
    /** @brief Destructor */
    ~ShaderInfoCollection();

    /**
     * @brief Initialize the singleton instance
     * @return Shared pointer to the instance
     */
    static std::shared_ptr<ShaderInfoCollection> Init();

    /**
     * @brief Get the singleton instance
     * @return Shared pointer to the instance
     */
    static std::shared_ptr<ShaderInfoCollection> Get();

    /**
     * @brief Add a shader to the collection
     * @param _shader ShaderInfo to add
     */
    void AddShader(std::shared_ptr<ShaderInfo> _shader);

    /**
     * @brief Remove a shader from the collection by name
     * @param _name Name of the shader to remove
     * @return True if shader was found and removed
     */
    bool RemoveShader(const std::string& _name);

    /**
     * @brief Remove a shader from the collection by index
     * @param _index Index of the shader to remove
     * @return True if index was valid and shader was removed
     */
    bool RemoveShaderAt(size_t _index);

    /**
     * @brief Get a shader by name
     * @param _name Name of the shader to find
     * @return Pointer to the shader, or nullptr if not found
     */
    std::weak_ptr<ShaderInfo> GetShader(const std::string& _name);

    /**
     * @brief Refresh visibility for all properties in all shaders
     */
    void RefreshAllVisibility();

    /**
     * @brief Reorder shaders by moving one from one index to another
     * @param _fromIndex Source index
     * @param _toIndex Destination index
     * @return True if reorder was successful
     */
    bool ReorderShader(size_t _fromIndex, size_t _toIndex);

    /**
     * @brief Static method to log a shader to the collection
     * @param _shader Pointer to the ComputeShader
     * @param _name Display name for the shader
     */
    static void LogShader(ComputeShader* _shader, std::string _name);

    /**
     * @brief Static method to load a shader
     * @param _path Path to the shader file
     * @param _name Display name for the shader
     * @return Pointer to the new ShaderInfo, or nullptr on failure
     */
    static std::weak_ptr<ShaderInfo> Load(const std::string& _path, const std::string& _name);

    /**
     * @brief Static method to remove a shader by name
     * @param _name Name of the shader to remove
     * @return True if shader was found and removed
     */
    static bool RemoveShaderStatic(const std::string& _name);

    /**
     * @brief Static method to refresh visibility for all shaders
     */
    static void RefreshAllVisibilityStatic();

    /**
     * @brief Print all shaders and their properties to stdout
     */
    static void PrintAllShaders();

    /**
     * @brief Get reference to the vector of shaders
     * @return Reference to the shaders vector
     */
    std::vector<std::shared_ptr<ShaderInfo>>& GetShaders();

    /**
     * @brief Get the number of shaders in the collection
     * @return Number of shaders
     */
    size_t GetShaderCount() const;
};

#endif
