#ifndef MODEL_H

#define MODEL_H

#include "GL/glew.h"
#include "GLM/glm.hpp"

#include <string>
#include <fstream>
#include <vector>

struct Vertex
{
  Vertex();

  glm::vec3 position;
  glm::vec2 texcoord;
  glm::vec3 normal;
};
  
struct Face
{
  Vertex a;
  Vertex b;
  Vertex c;
  glm::vec3 normal;

  inline void CalculateNormal()
  {
    glm::vec3 ac = c.position - a.position;
    glm::vec3 ab = b.position - a.position;
    
    normal = glm::cross(ac, ab);
  };
};

class Model
{
  Model* model;
  std::vector<Face> m_faces;
  GLuint m_ssboId;
  bool m_dirty;

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

  std::vector<Face>& GetFaces();

  GLsizei GetVertexCount() const;
  GLuint GetSSBO();
};

#include <stdexcept>


inline Model::Model() :
    m_dirty(false)
  , m_position()
  , m_ssboId(0)
{ }

inline Model::Model(const std::string& _path) :
    m_dirty(false)
  , m_position()
  , m_ssboId(0)
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
        1.0f - atof(L_tokens.at(2).c_str()));

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
        m_dirty = true;
      }
    }
  }
}

inline Model::~Model()
{
  if(m_ssboId)
  {
    glDeleteBuffers(1, &m_ssboId);
  }
}

inline Model::Model(const Model& _copy)
  : m_ssboId(0)
  , m_faces(_copy.m_faces)
  , m_dirty(true)
{ }

inline Model& Model::operator=(const Model& _assign)
{
  m_faces = _assign.m_faces;
  m_dirty = true;

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

struct Triangle
{
  glm::vec3 a;
  float _padding_a;
  glm::vec3 b;
  float _padding_b;
  glm::vec3 c;
  float _padding_c;
  glm::vec3 normal;
  float _padding_d;
};

inline GLuint Model::GetSSBO()
{
  if(m_dirty)
  {
    std::vector<Triangle> data;

    for(size_t fi = 0; fi < m_faces.size(); ++fi)
    {
      Triangle newTri;
      newTri.a = m_faces[fi].a.position;
      newTri.b = m_faces[fi].b.position;
      newTri.c = m_faces[fi].c.position;
      newTri.normal = m_faces[fi].normal;
      data.push_back(newTri);
    }

    glGenBuffers(1, &m_ssboId);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssboId);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 4 * sizeof(GLfloat) * 4 * data.size(), &(data.at(0)), GL_DYNAMIC_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_ssboId);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    printf("SSBO Model uploaded\n");
    m_dirty = false;
  }

  return m_ssboId;
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