#pragma once

#include <string>
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "ECS/Component.hpp"
#include "Math/Matrix.hpp"
#include "Math/Vector.hpp"
#include "Utility/AssetPath.hpp"
#include "Utility/FileSystem.hpp"

using namespace Wasteland::ECS;
using namespace Wasteland::Math;
using namespace Wasteland::Utility;

namespace Wasteland::Render
{
	class Shader final : public Component
	{

	public:
		
		Shader(const Shader&) = delete;
		Shader(Shader&&) = delete;
		Shader& operator=(const Shader&) = delete;
		Shader& operator=(Shader&&) = delete;

		std::string GetName() const
		{
			return name;
		}

		AssetPath GetPath() const
		{
			return path;
		}

		void Bind()
		{
			glUseProgram(id);
		}

		void Unbind()
		{
			glUseProgram(0);
		}

		void SetUniform(const std::string& name, bool value)
		{
			glUniform1i(glGetUniformLocation(id, name.c_str()), value);
		}

		void SetUniform(const std::string& name, int value)
		{
			glUniform1i(glGetUniformLocation(id, name.c_str()), value);
		}

		void SetUniform(const std::string& name, float value)
		{
			glUniform1f(glGetUniformLocation(id, name.c_str()), value);
		}

		void SetUniform(const std::string& name, const Vector<int, 2>& value)
		{
			glUniform2i(glGetUniformLocation(id, name.c_str()), value.x(), value.y());
		}

		void SetUniform(const std::string& name, const Vector<float, 2>& value)
		{
			glUniform2f(glGetUniformLocation(id, name.c_str()), value.x(), value.y());
		}

		void SetUniform(const std::string& name, const Vector<int, 3>& value)
		{
			glUniform3i(glGetUniformLocation(id, name.c_str()), value.x(), value.y(), value.z());
		}

		void SetUniform(const std::string& name, const Vector<float, 3>& value)
		{
			glUniform3f(glGetUniformLocation(id, name.c_str()), value.x(), value.y(), value.z());
		}

		void SetUniform(const std::string& name, const Matrix<float, 4, 4>& value)
		{
			glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &value[0][0]);
		}

		void Uninitialize()
		{
			glDeleteProgram(id);
		}

		static std::shared_ptr<Shader> Create(const std::string& name, const AssetPath& path)
		{
			std::shared_ptr<Shader> result(new Shader());

			result->name = name;
			result->path = path;
			result->vertexPath = result->path.GetFullPath() + std::string("Vertex.glsl");
			result->fragmentPath = result->path.GetFullPath() + std::string("Fragment.glsl");
			result->vertexData = FileSystem::ReadFile(result->vertexPath);
			result->fragmentData = FileSystem::ReadFile(result->fragmentPath);

			result->Generate();

			return result;
		}

	private:

		Shader() = default;

		void Generate()
		{
			unsigned int vertexShader = CompileShader(GL_VERTEX_SHADER, vertexData);
			unsigned int fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentData);

			id = glCreateProgram();

			glAttachShader(id, vertexShader);
			glAttachShader(id, fragmentShader);
			glLinkProgram(id);

			int success;
			char infoLog[512];

			glGetProgramiv(id, GL_LINK_STATUS, &success);

			if (!success)
			{
				glGetProgramInfoLog(id, 512, NULL, infoLog);
				std::cerr << "Shader linking failed: " << infoLog << std::endl;
			}

			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);
		}

		unsigned int CompileShader(GLenum type, const std::string& data)
		{
			unsigned int shader = glCreateShader(type);
			const char* shaderSource = data.c_str();

			glShaderSource(shader, 1, &shaderSource, NULL);
			glCompileShader(shader);

			int success;
			char infoLog[512];

			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

			if (!success)
			{
				glGetShaderInfoLog(shader, 512, NULL, infoLog);

				std::cerr << "Shader compilation failed ("
					<< (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment")
					<< "): " << infoLog << std::endl;
			}

			return shader;
		}

		std::string name;
		AssetPath path;

		std::string vertexPath, fragmentPath;
		std::string vertexData, fragmentData;

		unsigned int id;

	};
}