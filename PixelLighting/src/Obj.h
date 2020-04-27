#include "tiny_obj_loader/tiny_obj_loader.h"
#include <iostream>


class Obj
{
private:
    void FixNegativeZeros(std::vector<float>& vec, int i, int n);
    void NormalizeTangents(int, int);
public:
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string filepath;
    std::vector<float> positions;
    std::vector<unsigned int> indices;

	Obj(const std::string& path);
	~Obj();

	void Load();
};
