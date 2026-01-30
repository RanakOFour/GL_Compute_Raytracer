#ifndef MATERIALCOLLECTION_H
#define MATERIALCOLLECTION_H

#include "Datastructs/Material.h"
#include <vector>

class MaterialCollection
{
    private:
    std::vector<Material> m_materials;
    std::vector<bool> m_dirtyFlags;

    public:
    MaterialCollection() = default;
    ~MaterialCollection() = default;

    Material& AddMaterial();
    void AddMaterial(const Material& material);

    Material& GetMaterial(int index);
    std::vector<Material>* GetMaterials();
};

#endif