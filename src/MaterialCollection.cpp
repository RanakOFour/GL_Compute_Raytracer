#include "MaterialCollection.h"

Material& MaterialCollection::AddMaterial()
{
    Material mat{};
    m_materials.push_back(mat);
    m_dirtyFlags.push_back(true);
    return m_materials.back();
}

void MaterialCollection::AddMaterial(const Material& material)
{
    m_materials.push_back(material);
    m_dirtyFlags.push_back(true);
}

Material& MaterialCollection::GetMaterial(int index)
{
    return m_materials.at(index);
}

std::vector<Material>* MaterialCollection::GetMaterials()
{
    return &m_materials;
}