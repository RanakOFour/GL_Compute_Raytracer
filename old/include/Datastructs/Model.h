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
    std::vector<Triangle> m_triangles;
    int m_textureId;
    int m_materialId;

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

    inline void SetTextureID(int _id)
    {
        m_textureId = _id;
    };

    inline void SetMaterialID(int _id)
    {
        m_materialId = _id;
    };

    /**
     * @brief Get the total vertex count
     * @return Number of vertices (faces * 3)
     */
    GLsizei GetVertexCount() const;
    
    /**
     * @brief Return reference to list of Triangles
     * @return Vector of Triangle structures
     */
    std::vector<Triangle> GetTriangles(glm::vec3 _pos, glm::vec3 _scale);
};

#include <stdexcept>


inline Model::Model()
{ }

inline Model::Model(const std::string& _path)
{
    m_materialId = -1;
    m_textureId = -1;

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
            Triangle t;
            std::vector<std::string> sub;
            SplitString(L_tokens.at(1), '/', sub);
            if(sub.size() >= 1) t.a = L_positions.at(atoi(sub.at(0).c_str()) - 1);
            if(sub.size() >= 2) t.uvA = L_tcs.at(atoi(sub.at(1).c_str()) - 1);
            if(sub.size() >= 3) t.aNormal = L_normals.at(atoi(sub.at(2).c_str()) - 1);

            for(size_t ti = 2; ti + 1 < L_tokens.size(); ti++)
            {
                SplitString(L_tokens.at(ti), '/', sub);
                if(sub.size() >= 1) t.b = L_positions.at(atoi(sub.at(0).c_str()) - 1);
                if(sub.size() >= 2) t.uvB = L_tcs.at(atoi(sub.at(1).c_str()) - 1);
                if(sub.size() >= 3) t.bNormal = L_normals.at(atoi(sub.at(2).c_str()) - 1);

                SplitString(L_tokens.at(ti + 1), '/', sub);
                if(sub.size() >= 1) t.c = L_positions.at(atoi(sub.at(0).c_str()) - 1);
                if(sub.size() >= 2) t.uvC = L_tcs.at(atoi(sub.at(1).c_str()) - 1);
                if(sub.size() >= 3) t.cNormal = L_normals.at(atoi(sub.at(2).c_str()) - 1);

                CalculateNormal(t);
                m_triangles.push_back(t);
            }
        }
    }
}

inline Model::~Model()
{
}

inline Model::Model(const Model& _copy)
    : m_triangles(_copy.m_triangles)
{ }

inline Model& Model::operator=(const Model& _assign)
{
    m_triangles = _assign.m_triangles;
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

inline std::vector<Triangle> Model::GetTriangles(glm::vec3 _position = glm::vec3(0.0f),
                                                 glm::vec3 _scale = glm::vec3(1.0f))
{
    std::vector<Triangle> l_triangles;
    
    Triangle* l_currentTriangle = &m_triangles[0];
    int i = 0;
    do
    {
        Triangle newTri;
        newTri.a = (l_currentTriangle->a * _scale) + _position;
        newTri.b = (l_currentTriangle->b * _scale) + _position;
        newTri.c = (l_currentTriangle->c * _scale) + _position;
        
        newTri.uvA = l_currentTriangle->uvA;
        newTri.uvB = l_currentTriangle->uvB;
        newTri.uvC = l_currentTriangle->uvC;

        newTri.aNormal = l_currentTriangle->aNormal;
        newTri.bNormal = l_currentTriangle->bNormal;
        newTri.cNormal = l_currentTriangle->cNormal;

        newTri.materialId = m_materialId;
        newTri.textureId = m_textureId;

        CalculateNormal(newTri);
        l_triangles.push_back(newTri);

        l_currentTriangle++;
        i++;
    } while(i < m_triangles.size());

    return l_triangles;
}

inline GLsizei Model::GetVertexCount() const
{
    return (GLsizei)m_triangles.size() * 3;
}

#endif