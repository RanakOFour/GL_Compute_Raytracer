#ifndef PIPELINELOADER_H
#define PIPELINELOADER_H

#include "ComputeShader.h"
#include "Raytracer.h"

#include <vector>
#include <string>

class Raytracer;
class PipelineLoader
{
    private:
    static void LoadShader(std::string& _path, std::vector<ComputeInformation>* _out);

    public:
    static void LoadPipeline(std::string& _pathToFile, Raytracer* _rt);

};

#endif