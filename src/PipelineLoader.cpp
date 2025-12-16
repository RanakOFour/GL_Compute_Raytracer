#include "PipelineLoader.h"
#include "Raytracer.h"

#include "glm/ext.hpp"

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

void ReadVector(std::string& _vector, std::vector<float>& _out)
{
    std::string l_currentFloat = "";
    for(int i = 0; i < _vector.size(); ++i)
    {
        switch(_vector[i])
        {
            case ' ':
            break;

            case ',':
            _out.push_back(std::stof(l_currentFloat));
            l_currentFloat = std::string("");
            break;

            default:
            l_currentFloat += _vector[i];
        }
    }

    // Last number is not picked up by loop
    _out.push_back(std::stof(l_currentFloat));
}

void PipelineLoader::LoadShader(std::string& _path, std::vector<ComputeInformation>* _out)
{
    ComputeShader* l_newShader = new ComputeShader(_path);
    
    // Determine name by filename
    std::stringstream l_ss(_path);

    std::string l_shaderName = "";

    while(getline(l_ss, l_shaderName, '/'))
    {
        // This runs through the path until we are left with the final string
    }

    // Cut out the ".comp" at the end
    l_shaderName = l_shaderName.substr(0, l_shaderName.size() - 6);

    ComputeInformation l_newCompInfo(l_shaderName, l_newShader);
    _out->push_back(l_newCompInfo);
}

void PipelineLoader::LoadPipeline(std::string& _pathToFile, Raytracer* _rt)
{
    std::vector<ComputeInformation> l_shaders;
    std::ifstream l_pipelineText(_pathToFile);
    if(l_pipelineText.is_open())
    {
        std::string l_currentLine;
        while(getline(l_pipelineText, l_currentLine))
        {
            // Ignore lines with '#' to allow comments
            if(l_currentLine[0] == '#')
            {
                continue;
            }

            // Split string into components
            std::vector<std::string> l_splitLine;
            std::string l_split = "";

            for(int i = 0; i < l_currentLine.size(); ++i)
            {
                switch(l_currentLine[i])
                {
                    // Allow whitespace
                    case ' ':
                    break;

                    // Use ':' as parameter seperation
                    case ':':
                    l_splitLine.push_back(l_split);
                    l_split = std::string("");
                    break;

                    default:
                    l_split += l_currentLine[i];
                    break;
                }
            }

            // Catches last component
            l_splitLine.push_back(l_split);
            
            // Open another file
            if(l_splitLine[0] == "Load")
            {
                std::string l_pathToFile = l_splitLine[1];

                printf("Opening new file: %s", l_pathToFile.c_str());

                // String sanitization
                std::string l_sub = l_pathToFile.substr(l_pathToFile.size() - 4, 4);
                if(l_pathToFile.substr(l_pathToFile.size() - 4, 4) != ".txt")
                {
                    if(l_pathToFile.find('.') == std::string::npos)
                    {
                        l_pathToFile += ".txt";
                    }
                }

                PipelineLoader::LoadPipeline(l_pathToFile, _rt);
            }
            else if( l_splitLine[0] == "Shader")
            {
                PipelineLoader::LoadShader(l_splitLine[1], &l_shaders);
            }
        }
    }

    _rt->SetShaders(l_shaders);
}