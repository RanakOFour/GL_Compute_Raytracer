// Code taken from previous work, then modified with Triangle instead of using Vertex and Face structs
#ifndef MODEL_H
#define MODEL_H

#include "GL/glew.h"
#include "GLM/glm.hpp"

#include "Triangle.h"

#include <string>
#include <fstream>
#include <vector>

class Model
{
  std::vector<Triangle> m_tris;

  void SplitStringWhitespace(const std::string& _input,
    std::vector<std::string>& _output);

  void SplitString(const std::string& _input, char _splitter,
    std::vector<std::string>& _output);

  public:

  glm::vec3 m_position;
   
  Model();
  Model(const std::string& _path);

  Model(const Model& _copy);
  Model& operator=(const Model& _assign);
  virtual ~Model();

  GLsizei GetVertexCount() const;
  std::vector<Triangle> GetTriangles(glm::vec3 _position);
};

#include <stdexcept>


inline Model::Model()
: m_position()
{ }

inline Model::Model(const std::string& _path)
: m_position()
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
      Triangle l_tri;
      std::vector<std::string> sub;
      SplitString(L_tokens.at(1), '/', sub);
      if(sub.size() >= 1) l_tri.a = L_positions.at(atoi(sub.at(0).c_str()) - 1);
      if(sub.size() >= 2) l_tri.uvA = L_tcs.at(atoi(sub.at(1).c_str()) - 1);

      for(size_t ti = 2; ti + 1 < L_tokens.size(); ti++)
      {
        SplitString(L_tokens.at(ti), '/', sub);
        if(sub.size() >= 1) l_tri.b = L_positions.at(atoi(sub.at(0).c_str()) - 1);
        if(sub.size() >= 2) l_tri.uvB = L_tcs.at(atoi(sub.at(1).c_str()) - 1);

        SplitString(L_tokens.at(ti + 1), '/', sub);
        if(sub.size() >= 1) l_tri.c = L_positions.at(atoi(sub.at(0).c_str()) - 1);
        if(sub.size() >= 2) l_tri.uvC = L_tcs.at(atoi(sub.at(1).c_str()) - 1);

        CalculateNormal(l_tri);
        m_tris.push_back(l_tri);
      }
    }
  }
}

inline Model::~Model()
{
}

inline Model::Model(const Model& _copy)
: m_tris(_copy.m_tris)
{ }

inline Model& Model::operator=(const Model& _assign)
{
  m_tris = _assign.m_tris;

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

inline std::vector<Triangle> Model::GetTriangles(glm::vec3 _position)
{
  std::vector<Triangle> l_tris;
  
  for(int i = 0; i < m_tris.size(); i++)
  {
    Triangle l_newTri;
    l_newTri.a = m_tris[i].a + _position;
    l_newTri.uvA = m_tris[i].uvA;
    l_newTri.b = m_tris[i].b + _position;
    l_newTri.uvB = m_tris[i].uvB;
    l_newTri.c = m_tris[i].c + _position;
    l_newTri.uvC = m_tris[i].uvC;
    l_tris.push_back(l_newTri);
  }

  return l_tris;
}

inline GLsizei Model::GetVertexCount() const
{
  return (GLsizei)m_tris.size() * 3;
}

#endif