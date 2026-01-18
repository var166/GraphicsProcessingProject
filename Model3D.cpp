#include "Model3D.hpp"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace gps {

	void Model3D::LoadModel(std::string fileName) {

        std::string basePath = fileName.substr(0, fileName.find_last_of('/')) + "/";
		ReadOBJ(fileName, basePath);
	}

    void Model3D::LoadModel(std::string fileName, std::string basePath)	{

		ReadOBJ(fileName, basePath);
	}

	void Model3D::Draw(gps::Shader shaderProgram) {

		for (int i = 0; i < meshes.size(); i++)
			meshes[i].Draw(shaderProgram);
	}

    void Model3D::DrawSpaceship(gps::Shader shaderProgram, glm::mat4 modelMatrix) {

        GLint modelLoc = glGetUniformLocation(shaderProgram.shaderProgram, "model");
        GLint normalMatrixLoc = glGetUniformLocation(shaderProgram.shaderProgram, "normalMatrix");
        GLint viewLoc = glGetUniformLocation(shaderProgram.shaderProgram, "view");

        glm::mat4 view;
        glGetUniformfv(shaderProgram.shaderProgram, viewLoc, glm::value_ptr(view));

        for (int i = 0; i < meshes.size(); i++) {

            glm::mat4 currentModel = modelMatrix;

            if (meshes[i].name == "SpaceStation_low") {
                currentModel = glm::rotate(currentModel, -(float)glfwGetTime() * 0.1f, glm::vec3(0, 1, 0));
            }
            if (meshes[i].name == "SpaceStation.001_low") {
                currentModel = glm::rotate(currentModel, (float)glfwGetTime() * 0.2f, glm::vec3(0, 1, 0));
            }
            if (meshes[i].name == "SpaceStation.002_low") {
                currentModel = glm::rotate(currentModel, -(float)glfwGetTime() * 0.3f, glm::vec3(0, 1, 0));
            }
            if (meshes[i].name == "SpaceStation.003_low") {
                currentModel = glm::rotate(currentModel, (float)glfwGetTime() * 0.4f, glm::vec3(0, 1, 0));
            }
            if (meshes[i].name == "SpaceStation.004_low") {
                currentModel = glm::rotate(currentModel, (float)glfwGetTime() * 0.5f, glm::vec3(0, 1, 0));
            }
            if (meshes[i].name == "SpaceStation.005_low") {
                currentModel = glm::rotate(currentModel, (float)glfwGetTime() * 0.6f, glm::vec3(0, 1, 0));
            }
            if (meshes[i].name == "SpaceStation.006_low") {
                currentModel = glm::rotate(currentModel, (float)glfwGetTime() * 0.7f, glm::vec3(0, 1, 0));
            }
            if (meshes[i].name == "SpaceStation.007_low") {
                currentModel = glm::rotate(currentModel, -(float)glfwGetTime() * 0.8f, glm::vec3(0, 1, 0));
            }

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(currentModel));

            glm::mat3 normalMatrix = glm::mat3(glm::inverseTranspose(view * currentModel));
            glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

            meshes[i].Draw(shaderProgram);
        }
    }

	void Model3D::DrawMartian(gps::Shader shaderProgram, glm::mat4 modelMatrix) {
		shaderProgram.useShaderProgram();

		GLint modelLoc = glGetUniformLocation(shaderProgram.shaderProgram, "model");
		GLint normalMatrixLoc = glGetUniformLocation(shaderProgram.shaderProgram, "normalMatrix");
		GLint viewLoc = glGetUniformLocation(shaderProgram.shaderProgram, "view");

		glm::mat4 view;
		glGetUniformfv(shaderProgram.shaderProgram, viewLoc, glm::value_ptr(view));

		float t = (float)glfwGetTime();
		glm::vec3 pivotAdjustment = glm::vec3(0.0f, -0.5f, 0.0f);

		glm::mat4 rotatedMatrix = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.2f)), glm::vec3(t / 10.0f, t / 10.0f, 0.0f));
		rotatedMatrix = glm::rotate(rotatedMatrix, t, glm::vec3(0.0f, 0.0f, 1.0f));
		rotatedMatrix = glm::translate(rotatedMatrix, pivotAdjustment);


		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(rotatedMatrix));

		glm::mat3 normalMatrix = glm::mat3(glm::inverseTranspose(view * rotatedMatrix));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

		for (int i = 0; i < meshes.size(); i++)
			meshes[i].Draw(shaderProgram);
	}

	void Model3D::ReadOBJ(std::string fileName, std::string basePath) {

        std::cout << "Loading : " << fileName << std::endl;
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		int materialId;

		std::string err;
		bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, fileName.c_str(), basePath.c_str(), GL_TRUE);

		if (!err.empty()) {
			std::cerr << err << std::endl;
		}

		if (!ret) {
			exit(1);
		}

		std::cout << "# of shapes    : " << shapes.size() << std::endl;
		std::cout << "# of materials : " << materials.size() << std::endl;

		for (size_t s = 0; s < shapes.size(); s++) {

			std::vector<gps::Vertex> vertices;
			std::vector<GLuint> indices;
			std::vector<gps::Texture> textures;

			size_t index_offset = 0;
			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {

				int fv = shapes[s].mesh.num_face_vertices[f];

				for (size_t v = 0; v < fv; v++) {

					tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

					float vx = attrib.vertices[3 * idx.vertex_index + 0];
					float vy = attrib.vertices[3 * idx.vertex_index + 1];
					float vz = attrib.vertices[3 * idx.vertex_index + 2];
					float nx = attrib.normals[3 * idx.normal_index + 0];
					float ny = attrib.normals[3 * idx.normal_index + 1];
					float nz = attrib.normals[3 * idx.normal_index + 2];
					float tx = 0.0f;
					float ty = 0.0f;

					if (idx.texcoord_index != -1) {

						tx = attrib.texcoords[2 * idx.texcoord_index + 0];
						ty = attrib.texcoords[2 * idx.texcoord_index + 1];
					}

					glm::vec3 vertexPosition(vx, vy, vz);
					glm::vec3 vertexNormal(nx, ny, nz);
					glm::vec2 vertexTexCoords(tx, ty);

					gps::Vertex currentVertex;
					currentVertex.Position = vertexPosition;
					currentVertex.Normal = vertexNormal;
					currentVertex.TexCoords = vertexTexCoords;

					vertices.push_back(currentVertex);

					indices.push_back((GLuint)(index_offset + v));
				}

				index_offset += fv;
			}

			size_t a = shapes[s].mesh.material_ids.size();

			if (a > 0 && materials.size()>0) {

				materialId = shapes[s].mesh.material_ids[0];
				if (materialId != -1) {

					gps::Material currentMaterial;
					currentMaterial.ambient = glm::vec3(materials[materialId].ambient[0], materials[materialId].ambient[1], materials[materialId].ambient[2]);
					currentMaterial.diffuse = glm::vec3(materials[materialId].diffuse[0], materials[materialId].diffuse[1], materials[materialId].diffuse[2]);
					currentMaterial.specular = glm::vec3(materials[materialId].specular[0], materials[materialId].specular[1], materials[materialId].specular[2]);

					std::string ambientTexturePath = materials[materialId].ambient_texname;

					if (!ambientTexturePath.empty()) {

						gps::Texture currentTexture;
						currentTexture = LoadTexture(basePath + ambientTexturePath, "ambientTexture");
						textures.push_back(currentTexture);
					}

					std::string diffuseTexturePath = materials[materialId].diffuse_texname;

					if (!diffuseTexturePath.empty()) {

						gps::Texture currentTexture;
						currentTexture = LoadTexture(basePath + diffuseTexturePath, "diffuseTexture");
						textures.push_back(currentTexture);
					}

					std::string specularTexturePath = materials[materialId].specular_texname;

					if (!specularTexturePath.empty()) {

						gps::Texture currentTexture;
						currentTexture = LoadTexture(basePath + specularTexturePath, "specularTexture");
						textures.push_back(currentTexture);
					}

					std::string normalTexturePath = materials[materialId].normal_texname;
					if (normalTexturePath.empty()) normalTexturePath = materials[materialId].bump_texname;

					if (!normalTexturePath.empty()) {

						gps::Texture currentTexture;
						currentTexture = LoadTexture(basePath + normalTexturePath, "normalTexture");
						textures.push_back(currentTexture);
					}

					std::string emissiveTexturePath = materials[materialId].emissive_texname;

					if (!emissiveTexturePath.empty()) {

						gps::Texture currentTexture;
						currentTexture = LoadTexture(basePath + emissiveTexturePath, "emissiveTexture");
						textures.push_back(currentTexture);
					}

					std::string metallicTexturePath = materials[materialId].metallic_texname;

					if (!metallicTexturePath.empty()) {

						gps::Texture currentTexture;
						currentTexture = LoadTexture(basePath + metallicTexturePath, "metallicTexture");
						textures.push_back(currentTexture);
					}

					std::string roughnessTexturePath = materials[materialId].roughness_texname;

					if (!roughnessTexturePath.empty()) {

						gps::Texture currentTexture;
						currentTexture = LoadTexture(basePath + roughnessTexturePath, "roughnessTexture");
						textures.push_back(currentTexture);
					}
				}
			}

			meshes.push_back(gps::Mesh(vertices, indices, textures, shapes[s].name));
		}
	}

	gps::Texture Model3D::LoadTexture(std::string path, std::string type) {

			for (int i = 0; i < loadedTextures.size(); i++) {

				if (loadedTextures[i].path == path)	{

					return loadedTextures[i];
				}
			}

			gps::Texture currentTexture;
			currentTexture.id = ReadTextureFromFile(path.c_str());
			currentTexture.type = std::string(type);
			currentTexture.path = path;

			loadedTextures.push_back(currentTexture);

			return currentTexture;
		}

	GLuint Model3D::ReadTextureFromFile(const char* file_name) {

		int x, y, n;
		int force_channels = 4;
		unsigned char* image_data = stbi_load(file_name, &x, &y, &n, force_channels);

		if (!image_data) {
			fprintf(stderr, "ERROR: could not load %s\n", file_name);
			return false;
		}
		if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) {
			fprintf(
				stderr, "WARNING: texture %s is not power-of-2 dimensions\n", file_name
			);
		}

		int width_in_bytes = x * 4;
		unsigned char *top = NULL;
		unsigned char *bottom = NULL;
		unsigned char temp = 0;
		int half_height = y / 2;

		for (int row = 0; row < half_height; row++) {

			top = image_data + row * width_in_bytes;
			bottom = image_data + (y - row - 1) * width_in_bytes;

			for (int col = 0; col < width_in_bytes; col++) {

				temp = *top;
				*top = *bottom;
				*bottom = temp;
				top++;
				bottom++;
			}
		}

		GLuint textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			x,
			y,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			image_data
		);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		return textureID;
	}

	Model3D::~Model3D() {

        for (size_t i = 0; i < loadedTextures.size(); i++) {

            glDeleteTextures(1, &loadedTextures.at(i).id);
        }

        for (size_t i = 0; i < meshes.size(); i++) {

            GLuint VBO = meshes.at(i).getBuffers().VBO;
            GLuint EBO = meshes.at(i).getBuffers().EBO;
            GLuint VAO = meshes.at(i).getBuffers().VAO;
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
            glDeleteVertexArrays(1, &VAO);
        }
	}
}
