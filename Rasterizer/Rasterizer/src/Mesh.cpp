#include "Mesh.h"

#include <fstream>
#include <string>
#include <sstream>
#include <cstring>

Mesh::Mesh() :
	Primitive()
{
}

Mesh::Mesh(const math::Float3 * pos, const math::Float3 * rot, const math::Float3 * scl, const std::string* fPath) :
	Primitive(pos, rot, scl)
{
	LoadFromFile(fPath);
}


Mesh::~Mesh()
{
}


void Mesh::LoadFromFile(const std::string * filePath)
{
	std::ifstream file(MESH_PATH + *filePath + MESH_EXTENSION);
	std::string line;
	std::vector<std::string> lineSplitSpace;
	std::vector<std::string> indexSplitSpace;
	math::Float3 n;
	if (file.is_open())
	{
		while (std::getline(file, line))
		{
			lineSplitSpace.clear();
			SplitString(&line, " ", &lineSplitSpace);

			// check type of a line
			if (!std::strcmp(lineSplitSpace[0].c_str(), "v"))
			{
				_positionArray.push_back(math::Float3(
					std::stof(lineSplitSpace[1].c_str()),
					std::stof(lineSplitSpace[2].c_str()),
					std::stof(lineSplitSpace[3].c_str())
					));

				// this is here because obj doesnt support vertex color
				_colorArray.push_back(math::Float3(
					1.0f, 1.0f, 1.0f
				));

				++_vertexCount;
			}
			else if (!std::strcmp(lineSplitSpace[0].c_str(), "vt"))
			{
				_uvArray.push_back(math::Float2(
					std::stof(lineSplitSpace[1].c_str()),
					std::stof(lineSplitSpace[2].c_str())
				));
				++_uvCount;
			}
			else if (!std::strcmp(lineSplitSpace[0].c_str(), "vn"))
			{
				n = math::Float3(
					std::stof(lineSplitSpace[1].c_str()),
					std::stof(lineSplitSpace[2].c_str()),
					std::stof(lineSplitSpace[3].c_str())
				);

				math::Float3::Normalize(n);
				_normalArray.push_back(n);
				++_normalCount;
			}
			else if (!std::strcmp(lineSplitSpace[0].c_str(), "f"))
			{
				for (size_t i = 0; i < 3; ++i)
				{
					indexSplitSpace.clear();
					SplitString(&lineSplitSpace[i + 1], "/", &indexSplitSpace);
					_indexArray.push_back(math::UShort3(
						(uint16_t)std::stoul(indexSplitSpace[0].c_str()) - 1,
						(uint16_t)std::stoul(indexSplitSpace[1].c_str()) - 1,
						(uint16_t)std::stoul(indexSplitSpace[2].c_str()) - 1
					));
				}
				++_triangleCount;
			}
		}
		return;
	}

#ifdef _DEBUG

	std::cout << "Unable to open file: " << MESH_PATH + *filePath + MESH_EXTENSION << std::endl;

#endif // _DEBUG
}

void Mesh::SplitString(const std::string * str, const std::string delim, std::vector<std::string>* vec)
{
	std::string token;
	std::istringstream iss(*str);
	while (std::getline(iss, token, delim[0]))
	{
		vec->push_back(token);
	}
}
