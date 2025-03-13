#pragma once

#include <vector>
#include "ECS/GameObject.hpp"
#include "Math/Transform.hpp"
#include "Render/Camera.hpp"
#include "Render/Shader.hpp"
#include "Render/Texture.hpp"
#include "Render/Vertex.hpp"
#include "Utility/Exception/Exceptions/GraphicalErrorException.hpp"

namespace Wasteland::Render
{
	class Mesh final : public Component
	{

	public:

		~Mesh()
		{
			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(1, &VBO);
			glDeleteBuffers(1, &EBO);
		}

		Mesh(const Mesh&) = delete;
		Mesh(Mesh&&) = delete;

		void Generate()
		{
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);

			glBindVertexArray(VAO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
			glEnableVertexAttribArray(0);

			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
			glEnableVertexAttribArray(1);

			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
			glEnableVertexAttribArray(2);

			glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uvs));
			glEnableVertexAttribArray(3);

			glBindVertexArray(0);
		}

		void Render(std::shared_ptr<Camera> camera) override
		{
			auto shader = Super::GetGameObject()->GetComponent<Shader>().value();
			auto texture = Super::GetGameObject()->GetComponent<Texture>().value();

			glBindVertexArray(VAO);

			shader->Bind();
			texture->Bind(0);

			shader->SetUniform("projection", camera->GetProjectionMatrix());
			shader->SetUniform("view", camera->GetViewMatrix());
			shader->SetUniform("model", Super::GetGameObject()->GetTransform()->GetModelMatrix());

			glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);

			texture->Unbind();
			shader->Unbind();

			glBindVertexArray(0);

			int error = glGetError();

			if (error != 0)
				throw MAKE_EXCEPTION(GraphicalErrorException, std::string("OpenGL Error: '") + std::to_string(error) + "'!");
		}

		static std::shared_ptr<Mesh> Create(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
		{
			std::shared_ptr<Mesh> result(new Mesh());

			result->vertices = vertices;
			result->indices = indices;

			return result;
		}

	private:

		Mesh() = default;

		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		unsigned int VAO, VBO, EBO;

	};
}