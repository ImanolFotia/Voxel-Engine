#pragma once

#include <include/emlFormat.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <glm/glm.hpp>

class Model {
public:
    Model(std::string path) {
        std::ifstream inFILE(path.c_str(), std::ios::binary);
        if(!inFILE.is_open())
        {
            std::cout << "Fail to open EML file" << std::endl;
            return;
        }

        int numvertices = 0;
        int numIndices = 0;

        t_eml_header* header = new t_eml_header();
        t_Lumps* lumps = new t_Lumps[maxLumps];

        inFILE.seekg(0, inFILE.beg);
        inFILE.read((char*)&header[0], sizeof(t_eml_header));

        if((int)header->format != EMLHEADER)
        {
            std::cout << "This file is not a valid SEML file. " << std::endl;
            return;
        }

        if((float)header->version != emlVersion)
        {
            std::cout << "This file version doesn't match the one required."<< std::endl <<
                      "This file: " << (float)header->version << " Required: " << emlVersion << std::endl;
            return;
        }

        //Read the lump of this file
        inFILE.seekg(sizeof(t_eml_header), inFILE.beg);
        for(int i = 0; i < maxLumps; ++i)
        {
            inFILE.read((char*)&lumps[i], sizeof(t_Lumps));
        }

        //Get the array of vertices in the file
        numvertices = lumps[lVertices].size / sizeof(t_Vertex);
        t_Vertex* l_vertices = new t_Vertex[numvertices];
        inFILE.seekg(lumps[lVertices].offset, inFILE.beg);
        for(int i = 0; i < numvertices; ++i)
        {
            inFILE.read((char*)&l_vertices[i], sizeof(t_Vertex));
        }

        //Get the array of indices in the file
        numIndices = lumps[lIndices].size / sizeof(t_Index);
        t_Index* l_indices = new t_Index[numIndices];
        inFILE.seekg(lumps[lIndices].offset, inFILE.beg);
        for(int i = 0; i < numIndices; ++i)
        {
            inFILE.read((char*)&l_indices[i], sizeof(t_Index));
        }

        for( auto i = 0; i < numvertices; ++i ) {
            m_Vertices.push_back(glm::vec3(l_vertices[i].position.x, l_vertices[i].position.y, l_vertices[i].position.z));
            m_Normals.push_back(glm::vec3(l_vertices[i].normal.x, l_vertices[i].normal.y, l_vertices[i].normal.z));
        }

        for(auto i = 0; i < numIndices; ++i){
            m_Indices.push_back(l_indices[i].index);
        }



        delete header;
        delete[] lumps;
        delete[] l_vertices;

        inFILE.close();

    }

    std::vector<glm::vec3> getVertices()
    {
        return m_Vertices;
    }

    std::vector<glm::vec3> getNormals()
    {
        return m_Normals;
    }

    std::vector<unsigned int> getIndices()
    {
        return m_Indices;
    }

private:
    std::vector<glm::vec3> m_Vertices;
    std::vector<glm::vec3> m_Normals;
    std::vector<unsigned int> m_Indices;
};

