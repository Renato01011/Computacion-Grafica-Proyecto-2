#ifndef MESH_H
#define MESH_H

#include <iostream>
#include <vector>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "shader_m.h"
#include "stb_image.h"

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false) {
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class VBO {
public:
    GLuint ID;
    VBO(std::vector<Vertex>& v) {
        glGenBuffers(1, &ID);
        glBindBuffer(GL_ARRAY_BUFFER, ID);
        glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(Vertex), &v[0], GL_STATIC_DRAW);
    }

    void Bind() { glBindBuffer(GL_ARRAY_BUFFER, ID); }
    void UnBind() { glBindBuffer(GL_ARRAY_BUFFER, 0); }
    void Delete() { glDeleteBuffers(1, &ID); }
};

class VAO {
public:
    GLuint ID;
    VAO() { glGenVertexArrays(1, &ID); }

    void LinkAttrib(VBO& VBO, GLuint layout, GLuint numComp, GLenum type, GLsizeiptr stride, void* offset) {
        VBO.Bind();
        glVertexAttribPointer(layout, numComp, type, GL_FALSE, stride, offset);
        glEnableVertexAttribArray(layout);
        VBO.UnBind();
    }

    void Bind() { glBindVertexArray(ID); }
    void UnBind() { glBindVertexArray(0); }
    void Delete() { glDeleteVertexArrays(1, &ID); }
};

class EBO {
public:
    GLuint ID;
    EBO(std::vector<GLuint>& indices) {
        glGenBuffers(1, &ID);
	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
	    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    }

    void Bind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID); }
    void UnBind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
    void Delete() { glDeleteBuffers(1, &ID); }
};

class Mesh {
public:
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture>      textures;

    VAO vao;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) : vertices{vertices}, indices{indices}, textures{textures} { Setup(); }
    
    void Setup() {
        vao.Bind();
        
        VBO vbo(vertices);
        EBO ebo(indices);

        vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
        vao.LinkAttrib(vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        vao.LinkAttrib(vbo, 2, 2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        
        vao.UnBind();
        vbo.UnBind();
        ebo.UnBind();
    }
    
    void Draw(Shader &shader) {
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        for(unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            std::string number;
            std::string name = textures[i].type;
            if(name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if(name == "texture_specular")
                number = std::to_string(specularNr++);

            shader.setInt(("material." + name + number).c_str(), i);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }
        glActiveTexture(GL_TEXTURE0);

        vao.Bind();
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        vao.UnBind();
    }
};

#endif