#include "Mesh.hpp"
namespace gps {

	Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures) {

		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;
		this->name = "";

		this->setupMesh();
	}

	Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures, std::string name) {

		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;
		this->name = name;

		this->setupMesh();
	}

	Buffers Mesh::getBuffers() {
	    return this->buffers;
	}

	void Mesh::Draw(gps::Shader shader) {

		shader.useShaderProgram();

		glUniform1i(glGetUniformLocation(shader.shaderProgram, "diffuseTexture"), 0);
		glUniform1i(glGetUniformLocation(shader.shaderProgram, "specularTexture"), 0);
		glUniform1i(glGetUniformLocation(shader.shaderProgram, "normalTexture"), 0);
		glUniform1i(glGetUniformLocation(shader.shaderProgram, "emissiveTexture"), 0);
		glUniform1i(glGetUniformLocation(shader.shaderProgram, "metallicTexture"), 0);
		glUniform1i(glGetUniformLocation(shader.shaderProgram, "roughnessTexture"), 0);
		glUniform1i(glGetUniformLocation(shader.shaderProgram, "hasNormalMap"), GL_FALSE);
		glUniform1i(glGetUniformLocation(shader.shaderProgram, "hasEmissiveMap"), GL_FALSE);

		for (GLuint i = 0; i < textures.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			glUniform1i(glGetUniformLocation(shader.shaderProgram, this->textures[i].type.c_str()), i);
			glBindTexture(GL_TEXTURE_2D, this->textures[i].id);

			if (this->textures[i].type == "normalTexture") {
				glUniform1i(glGetUniformLocation(shader.shaderProgram, "hasNormalMap"), GL_TRUE);
			}
			if (this->textures[i].type == "emissiveTexture") {
				glUniform1i(glGetUniformLocation(shader.shaderProgram, "hasEmissiveMap"), GL_TRUE);
			}
		}

		glBindVertexArray(this->buffers.VAO);
		glDrawElements(GL_TRIANGLES, (GLsizei)this->indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		for (GLuint i = 0; i < this->textures.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

	void Mesh::setupMesh() {

		glGenVertexArrays(1, &this->buffers.VAO);
		glGenBuffers(1, &this->buffers.VBO);
		glGenBuffers(1, &this->buffers.EBO);

		glBindVertexArray(this->buffers.VAO);
		glBindBuffer(GL_ARRAY_BUFFER, this->buffers.VBO);
		glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->buffers.EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

		glBindVertexArray(0);
	}
}
