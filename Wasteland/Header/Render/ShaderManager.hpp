#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <optional>
#include "Render/Shader.hpp"
#include "Utility/Exception/Exceptions/NoSuchElementException.hpp"

using namespace Wasteland::Utility::Exception::Exceptions;

namespace Wasteland::Render
{
	class ShaderManager final
	{

	public:
		
		ShaderManager(const ShaderManager&) = delete;
		ShaderManager(ShaderManager&&) = delete;
		ShaderManager& operator=(const ShaderManager&) = delete;
		ShaderManager& operator=(ShaderManager&&) = delete;

		void Register(const std::shared_ptr<Shader>& object)
		{
			shaderMap.insert({ object->GetName(), std::move(object) });
		}

		void Unregister(const std::string& name)
		{
			if (!shaderMap.contains(name))
				throw MAKE_EXCEPTION(NoSuchElementException, std::format("Shader map does not contain '{}'!", name));

			shaderMap.erase(name);
		}

		bool Has(const std::string& name) const
		{
			return shaderMap.contains(name);
		}

		std::optional<std::shared_ptr<Shader>> Get(const std::string& name)
		{
			return shaderMap.contains(name) ? std::make_optional<std::shared_ptr<Shader>>(shaderMap[name]) : std::nullopt;
		}

		std::vector<std::shared_ptr<Shader>> GetAll() const
		{
			std::vector<std::shared_ptr<Shader>> result;

			result.reserve(shaderMap.size());

			std::transform(shaderMap.begin(), shaderMap.end(), std::back_inserter(result), [](const auto& pair) { return pair.second; });

			return result;
		}

		static ShaderManager& GetInstance()
		{
			std::call_once(initializationFlag, [&]()
			{
				instance = std::unique_ptr<ShaderManager>(new ShaderManager());
			});

			return *instance;
		}

	private:

		ShaderManager() = default;

		std::unordered_map<std::string, std::shared_ptr<Shader>> shaderMap;

		static std::once_flag initializationFlag;
		static std::unique_ptr<ShaderManager> instance;

	};

	std::once_flag ShaderManager::initializationFlag;
	std::unique_ptr<ShaderManager> ShaderManager::instance;
}