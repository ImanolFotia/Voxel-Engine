#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <iostream>

class PointCloud
{
public:

    PointCloud(std::vector<glm::vec3> positions) : m_PointPositions(positions)
    {

    }

    PointCloud(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals) : m_PointPositions(positions), m_PointNormals(normals)
    {

    }
    PointCloud(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals, std::vector<unsigned int> indices)
    {
        m_PointIndices = indices;

        for(int i = 0; i < m_PointIndices.size(); i++)
        {
            m_PointPositions.push_back(positions.at(m_PointIndices.at(i)));
            m_PointNormals.push_back(normals.at(m_PointIndices.at(i)));
        }

    }
    PointCloud(float rawPositions[], int numP)
    {
        #pragma omp parallel for simd
        for(int i = 0; i < numP; ++i)
            m_PointPositions.push_back(glm::vec3(rawPositions[i], rawPositions[i+1], rawPositions[i+2]));
    }

    ~PointCloud() {}

    void Render(GLuint shader, glm::mat4 projection, glm::mat4 view)
    {
        if(firstTime)
        {
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3), &point, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
            firstTime = false;
        }

        glUseProgram(shader);

        glBindVertexArray(VAO);

        //#pragma omp parellel for simd
        {
        #pragma omp single
        for(auto i: m_PointPositions)
        {
            glm::mat4 model = glm::translate(glm::mat4(), i);
            glm::mat4 MVP = projection * view * model;
            glUniformMatrix4fv(glGetUniformLocation(shader, "MVP"), 1, GL_FALSE, &MVP[0][0]);
            glUniform1i(glGetUniformLocation(shader, "isQuad"), 0);

            glDrawArrays(GL_POINTS, 0, 1);
        }
        }

        glBindVertexArray(0);

        glUseProgram(0);

    }

    std::vector<glm::vec3> getPointsPositions()
    {
        return m_PointPositions;
    }

    std::vector<glm::vec3> getPointsNormals()
    {
        return m_PointNormals;
    }

private:

    bool firstTime = true;
    GLuint VAO, VBO;
    std::vector<glm::vec3> m_PointPositions;
    std::vector<glm::vec3> m_PointNormals;
    std::vector<unsigned int> m_PointIndices;
    glm::vec3 point = glm::vec3(0,0,0);

};
