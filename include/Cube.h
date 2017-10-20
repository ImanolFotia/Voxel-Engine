#pragma once

#include <GL/glew.h>
#include <string>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Cube
{

public:
    Cube(std::vector<glm::vec3> Positions, std::vector<glm::vec3> Normals, std::vector<float> Scales)
    {
        amount  = Positions.size();

        glGenVertexArrays(1, &VAO);

        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);

        glGenBuffers(1, &Normalbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, Normalbuffer);
        glBufferData(GL_ARRAY_BUFFER, Normals.size() * sizeof(glm::vec3), &Normals[0], GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribDivisor(1, 1);

        glm::mat4* modelMatrices = new glm::mat4[Positions.size()];
        glm::mat4 model;
        for(int i = 0; i < Positions.size(); ++i)
        {
            model = glm::mat4(1);
            model = glm::translate(glm::mat4(1), Positions[i]);
            model = glm::scale(model, glm::vec3(Scales[i]));
            modelMatrices[i] = model;
        }

        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, Positions.size() * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
        // Set attribute pointers for matrix (4 times vec4)
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)0);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(2, 1);
        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);


        glBindVertexArray(0);

        delete[] modelMatrices;


    }
    virtual ~Cube()
    {
        //std::cout << "Deleted Grass" << std::endl;
    }

    void Destroy()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteBuffers(1, &Normalbuffer);
        glDeleteBuffers(1, &buffer);
    }

public:
    void Render()
    {
        glBindVertexArray(this->VAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, amount); //glDrawElementsInstanced(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0, amount);
        glBindVertexArray(0);
    }
private:

    GLuint buffer;
    GLuint Normalbuffer;
    GLuint VBO, EBO, VAO;
    GLuint grassTexture;
    int amount;
    GLfloat cubeVertices[36*3] = {
            // Positions
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
        };


};
