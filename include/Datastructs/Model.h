/**
 * @file Model.h
 * @brief OBJ model loader and mesh data structures
 * 
 * This file provides the Model class for loading OBJ model files,
 * along with supporting Vertex and Face structures. The Model class
 * can convert loaded geometry to the Triangle format used by the raytracer.
 * 
 * @note Code taken from previous work, then modified to support Triangle export.
 */

#ifndef MODEL_H
#define MODEL_H

#include "GL/glew.h"
#include "GLM/glm.hpp"

#include "Triangle.h"

#include <string>
#include <fstream>
#include <vector>

/**
 * @struct Vertex
 * @brief A single vertex with position, texture coordinate, and normal
 */
struct Vertex
{
    /**
     * @brief Default constructor - initializes all values to zero
     */
    Vertex();

    /** @brief 3D position of the vertex */
    glm::vec3 position;
    
    /** @brief 2D texture coordinate */
    glm::vec2 texcoord;
    
    /** @brief Vertex normal vector */
    glm::vec3 normal;
};
  
/**
 * @struct Face
 * @brief A triangular face composed of three vertices
 */
struct Face
{
    /** @brief First vertex of the triangle */
    Vertex a;
    
    /** @brief Second vertex of the triangle */
    Vertex b;
    
    /** @brief Third vertex of the triangle */
    Vertex c;
    
    /** @brief Face normal vector */
    glm::vec3 normal;

    /**
     * @brief Calculate the face normal from vertex positions
     */
    inline void CalculateNormal()
    {
        glm::vec3 ac = c.position - a.position;
        glm::vec3 ab = b.position - a.position;
        normal = glm::cross(ac, ab);
    };
};

/**
 * @class Model
 * @brief Loads and stores 3D model data from OBJ files
 * 
 * Parses OBJ files to extract vertex positions, texture coordinates,
 * normals, and faces. Provides methods to export geometry as triangles
 * for use in the raytracer.
 */
class Model
{
private:
    /** @brief Collection of faces making up the model */
    std::vector<Face> m_faces;

    /**
     * @brief Split a string by whitespace characters
     * @param _input String to split
     * @param _output Vector to store resulting tokens
     */
    void SplitStringWhitespace(const std::string& _input,
        std::vector<std::string>& _output);

    /**
     * @brief Split a string by a specific delimiter character
     * @param _input String to split
     * @param _splitter Delimiter character
     * @param _output Vector to store resulting tokens
     */
    void SplitString(const std::string& _input, char _splitter,
        std::vector<std::string>& _output);

public:
    /**
     * @brief Default constructor - creates empty model
     */
    Model();
    
    /**
     * @brief Load a model from an OBJ file
     * @param _path Filepath to the OBJ file
     * @throws std::runtime_error if file cannot be opened
     */
    Model(const std::string& _path);

    /**
     * @brief Copy constructor
     * @param _copy Model to copy from
     */
    Model(const Model& _copy);
    
    /**
     * @brief Copy assignment operator
     * @param _assign Model to copy from
     * @return Reference to this model
     */
    Model& operator=(const Model& _assign);
    
    /** @brief Virtual destructor */
    virtual ~Model();

    /**
     * @brief Get reference to the face list
     * @return Reference to the internal face vector
     */
    std::vector<Face>& GetFaces();

    /**
     * @brief Get the total vertex count
     * @return Number of vertices (faces * 3)
     */
    GLsizei GetVertexCount() const;
    
    /**
     * @brief Convert model faces to Triangle format for raytracing
     * @param _position World position offset for the model
     * @param _scale Scale factor for the model
     * @return Vector of Triangle structures
     */
    std::vector<Triangle> GetTriangles(glm::vec3 _position, glm::vec3 _scale);
};

#include <stdexcept>


inline Model::Model()
{ }

inline Model::Model(const std::string& _path)
{
    std::vector<glm::vec3> L_positions;
    std::vector<glm::vec2> L_tcs;
    std::vector<glm::vec3> L_normals;
    std::string L_currentline;

    std::ifstream file(_path.c_str());

    if(!file.is_open())
    {
        throw std::runtime_error("Failed to open model [" + _path + "]");
    }

    while(!file.eof())
    {
        std::getline(file, L_currentline);
        if(L_currentline.length() < 1) continue;

        std::vector<std::string> L_tokens;
        SplitStringWhitespace(L_currentline, L_tokens);
        if(L_tokens.size() < 1) continue;

        if(L_tokens.at(0) == "v" && L_tokens.size() >= 4)
        {
            glm::vec3 p(atof(L_tokens.at(1).c_str()),
                atof(L_tokens.at(2).c_str()),
                atof(L_tokens.at(3).c_str()));

            L_positions.push_back(p);
        }
        else if(L_tokens.at(0) == "vt" && L_tokens.size() >= 3)
        {
            glm::vec2 tc(atof(L_tokens.at(1).c_str()),
                atof(L_tokens.at(2).c_str()));

            L_tcs.push_back(tc);
        }
        else if(L_tokens.at(0) == "vn" && L_tokens.size() >= 4)
        {
            glm::vec3 n(atof(L_tokens.at(1).c_str()),
                atof(L_tokens.at(2).c_str()),
                atof(L_tokens.at(3).c_str()));

            L_normals.push_back(n);
        }
        else if(L_tokens.at(0) == "f" && L_tokens.size() >= 4)
        {
            Face f;
            std::vector<std::string> sub;
            SplitString(L_tokens.at(1), '/', sub);
            if(sub.size() >= 1) f.a.position = L_positions.at(atoi(sub.at(0).c_str()) - 1);
            if(sub.size() >= 2) f.a.texcoord = L_tcs.at(atoi(sub.at(1).c_str()) - 1);
            if(sub.size() >= 3) f.a.normal = L_normals.at(atoi(sub.at(2).c_str()) - 1);

            for(size_t ti = 2; ti + 1 < L_tokens.size(); ti++)
            {
                SplitString(L_tokens.at(ti), '/', sub);
                if(sub.size() >= 1) f.b.position = L_positions.at(atoi(sub.at(0).c_str()) - 1);
                if(sub.size() >= 2) f.b.texcoord = L_tcs.at(atoi(sub.at(1).c_str()) - 1);
                if(sub.size() >= 3) f.b.normal = L_normals.at(atoi(sub.at(2).c_str()) - 1);

                SplitString(L_tokens.at(ti + 1), '/', sub);
                if(sub.size() >= 1) f.c.position = L_positions.at(atoi(sub.at(0).c_str()) - 1);
                if(sub.size() >= 2) f.c.texcoord = L_tcs.at(atoi(sub.at(1).c_str()) - 1);
                if(sub.size() >= 3) f.c.normal = L_normals.at(atoi(sub.at(2).c_str()) - 1);

                f.CalculateNormal();
                m_faces.push_back(f);
            }
        }
    }
}

inline Model::~Model()
{
}

inline Model::Model(const Model& _copy)
    : m_faces(_copy.m_faces)
{ }

inline Model& Model::operator=(const Model& _assign)
{
    m_faces = _assign.m_faces;
    return *this;
}

inline void Model::SplitStringWhitespace(const std::string& _input,
    std::vector<std::string>& _output)
{
    std::string curr;

    _output.clear();

    for(size_t i = 0; i < _input.length(); i++)
    {
        if(_input.at(i) == ' ' ||
            _input.at(i) == '\r' ||
            _input.at(i) == '\n' ||
            _input.at(i) == '\t')
        {
            if(curr.length() > 0)
            {
                _output.push_back(curr);
                curr = "";
            }
        }
        else
        {
            curr += _input.at(i);
        }
    }

    if(curr.length() > 0)
    {
        _output.push_back(curr);
    }
}

inline void Model::SplitString(const std::string& _input, char _splitter,
    std::vector<std::string>& _output)
{
    std::string curr;

    _output.clear();

    for(size_t i = 0; i < _input.length(); i++)
    {
        if(_input.at(i) == _splitter)
        {
            _output.push_back(curr);
            curr = "";
        }
        else
        {
            curr += _input.at(i);
        }
    }

    if(curr.length() > 0)
    {
        _output.push_back(curr);
    }
}

inline std::vector<Face>& Model::GetFaces()
{
    return m_faces;
}

inline std::vector<Triangle> Model::GetTriangles(glm::vec3 _position = glm::vec3(0.0f),
                                                 glm::vec3 _scale = glm::vec3(1.0f))
{
    std::vector<Triangle> l_triangles;

    for(size_t fi = 0; fi < m_faces.size(); ++fi)
    {
        Triangle newTri;
        newTri.a = (m_faces[fi].a.position * _scale) + _position;
        newTri.b = (m_faces[fi].b.position * _scale) + _position;
        newTri.c = (m_faces[fi].c.position * _scale) + _position;
        
        newTri.uvA = m_faces[fi].a.texcoord;
        newTri.uvB = m_faces[fi].b.texcoord;
        newTri.uvC = m_faces[fi].c.texcoord;

        CalculateNormal(newTri);
        l_triangles.push_back(newTri);
    }

    return l_triangles;
}

inline GLsizei Model::GetVertexCount() const
{
    return (GLsizei)m_faces.size() * 3;
}

inline Vertex::Vertex()
    : position(0, 0, 0)
    , texcoord(0, 0)
    , normal(0, 0, 0)
{ }

#endif