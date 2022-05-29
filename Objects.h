#ifndef OBJECT_H
#define OBJECT_H

#include "Mesh.h"
#include "BoundingVolume.h"

#define RAD_FOR_BOUNDS 1

class Object {
public:
    glm::vec3 pos;
    float rot;
    glm::vec3 scale;
    glm::mat4 model;
    
    GLuint index_count;
    bool Visible = true;

    BoundingBox bx;
    std::string name;

    virtual void Setup() = 0;
    virtual void Draw(Shader &sh) = 0;
    virtual void Update(float t) = 0;
    virtual void CollisionDetection(std::vector<Object*> vObj) = 0;
};

// Constants For Interaction
bool MoveBall = false;
Object* CollidedObject = nullptr;

class Sphere : public Object {
public:
    float radius;
    int slices, stacks;

    //Movement After Launch
    glm::vec3 pos_ini;
    glm::vec3 vel_ini;

    VAO vao;

    Sphere(glm::vec3 pos, glm::vec3 scale, float radius, int slices, int stacks) {
        this->pos = pos;
        this->scale = scale;
        this->radius = radius;
        this->slices = slices;
        this->stacks = stacks;
    }

    void Setup() {
        const float pi = 3.1415926535897932384626433832795f;
        const float _2pi = 2.0f * pi;
        std::vector<Vertex> vertices;
        for( int i = 0; i <= stacks; ++i )
        {
            // V texture coordinate.
            float V = i / (float)stacks;
            float phi = V * pi;
            for ( int j = 0; j <= slices; ++j )
            {
                // U texture coordinate.
                float U = j / (float)slices;
                float theta = U * _2pi;
                float X = cos(theta) * sin(phi);
                float Y = cos(phi);
                float Z = sin(theta) * sin(phi);
                Vertex temp;
                temp.Position = glm::vec3( X, Y, Z) * radius;
                temp.Normal = glm::vec3(X, Y, Z);
                temp.TexCoords = glm::vec2(U, V);
                vertices.push_back(temp);
                //vertices.push_back( glm::vec3( X, Y, Z) * radius );
                //normals.push_back( glm::vec3(X, Y, Z) );
                //textureCoords.push_back( glm::vec2(U, V) );
            }
        }
        // Now generate the index buffer
        std::vector<unsigned int> indices;
        for( int i = 0; i < slices * stacks + slices; ++i ) {
            indices.push_back( i );
            indices.push_back( i + slices + 1  );
            indices.push_back( i + slices );
            indices.push_back( i + slices + 1  );
            indices.push_back( i );
            indices.push_back( i + 1 );
        }

        index_count = indices.size();

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

    void Draw(Shader &sh) {
        model = glm::mat4(1.0);
        model = glm::scale(model, glm::vec3(0.5));
        model = glm::translate(model, pos);
        sh.setMat4("model", model);
        if (Visible) {
            vao.Bind();
            glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
            vao.UnBind();
        }
    }

    void Update(float t) {
        glm::vec3 g(0.0f,-9.8f,0.0f);
        pos.x = pos_ini.x + vel_ini.x * t + 0.5 * g.x * t * t;
        pos.y = pos_ini.y + vel_ini.y * t + 0.5 * g.y * t * t;
        pos.z = pos_ini.z + vel_ini.z * t + 0.5 * g.z * t * t;
    }

    void CollisionDetection(std::vector<Object*> vObj) {

    }
};

class Model : public Object {
public:
    std::vector<Mesh> m;
    std::string dir;
    std::string filepath;
    std::vector<Texture> textures_loaded;

    //Movement After Launch
    glm::vec3 pos_ini;
    glm::vec3 vel_ini;

    //For Idle Movement
    bool idleMovement = true;
    float firstPosY;

    Model(glm::vec3 pos, float rot, glm::vec3 scale, char *path, std::string name) {
        this->pos = pos;
        this->rot = rot;
        this->scale = scale;
        filepath = path;
        this->name = name;
        firstPosY = pos.y;
    }

    void Setup() {
        loadModel(filepath);
    }

    void Draw(Shader &sh) {
        for(unsigned int i = 0; i < m.size(); i++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pos);
            model = glm::rotate(model, glm::radians(rot), glm::vec3(0.0f,1.0f,0.0f));
            model = glm::scale(model, scale);
            sh.setMat4("model", model);
            m[i].Draw(sh);
        }
    }

    void Update(float t) {
        if (name == "box") {
            rot += 1; if (rot > 360) { rot = 0.0f; }
            if (idleMovement) {
                pos.y -= 0.01;
                if (pos.y < firstPosY - 0.5) { idleMovement = !idleMovement; }
            }
            else if (!idleMovement) {
                pos.y += 0.01;
                if (pos.y > firstPosY + 0.5) { idleMovement = !idleMovement; }
            }
        }
        else if (name == "ball") {
            if (MoveBall) {
                glm::vec3 g(0.0f,-9.8f,0.0f);
                pos.x = pos_ini.x + vel_ini.x * t + 0.5 * g.x * t * t;
                pos.y = pos_ini.y + vel_ini.y * t + 0.5 * g.y * t * t;
                pos.z = pos_ini.z + vel_ini.z * t + 0.5 * g.z * t * t;
                rot += 1; if (rot > 360) { rot = 0.0f; }
            }
        }
        bx.Calculate(pos, RAD_FOR_BOUNDS);
    }

    void CollisionDetection(std::vector<Object*> vObj) {
        for (auto obj : vObj) {
            if (bx.Collision(obj->bx)) {
                if (obj->name == "box") {
                    CollidedObject = obj;
                }
            }
        }
    }

    void loadModel(std::string path) {
        Assimp::Importer import;
        const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);	
        
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
        {
            std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
            return;
        }
        dir = path.substr(0, path.find_last_of('/'));

        processNode(scene->mRootNode, scene);
    }

    void processNode(aiNode *node, const aiScene *scene) {
        // process all the node's meshes (if any)
        for(unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
            m.push_back(processMesh(mesh, scene));			
        }
        // then do the same for each of its children
        for(unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }

    Mesh processMesh(aiMesh *mesh, const aiScene *scene) {
        // data to fill
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        // walk through each of the mesh's vertices
        for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            // normals
            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }
            // texture coordinates
            if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x; 
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
                // tangent
                // vector.x = mesh->mTangents[i].x;
                // vector.y = mesh->mTangents[i].y;
                // vector.z = mesh->mTangents[i].z;
                // vertex.Tangent = vector;
                // // bitangent
                // vector.x = mesh->mBitangents[i].x;
                // vector.y = mesh->mBitangents[i].y;
                // vector.z = mesh->mBitangents[i].z;
                // vertex.Bitangent = vector;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }
        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for(unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for(unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);        
        }
        // process materials
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];    
        // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
        // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
        // Same applies to other texture as the following list summarizes:
        // diffuse: texture_diffuseN
        // specular: texture_specularN
        // normal: texture_normalN

        // 1. diffuse maps
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. specular maps
        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. normal maps
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. height maps
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        
        // return a mesh object created from the extracted mesh data
        return Mesh(vertices, indices, textures);
    }

    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName) {
        std::vector<Texture> textures;
        for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            bool skip = false;
            for(unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true; 
                    break;
                }
            }
            if(!skip)
            {   // if texture hasn't been loaded already, load it
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), dir);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture); // add to loaded textures
            }
        }
        return textures;
    }
};

#endif