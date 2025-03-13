#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <optional>
#include "Render/Texture.hpp"
#include "Utility/Exception/Exceptions/NoSuchElementException.hpp"

using namespace Wasteland::Utility::Exception::Exceptions;

namespace Wasteland::Render
{
	class TextureManager final
	{

	public:
		
		TextureManager(const TextureManager&) = delete;
		TextureManager(TextureManager&&) = delete;
		TextureManager& operator=(const TextureManager&) = delete;
		TextureManager& operator=(TextureManager&&) = delete;

		void Register(const std::shared_ptr<Texture>& object)
		{
			textureMap.insert({ object->GetName(), std::move(object) });
		}

		void Unregister(const std::string& name)
		{
			if (!textureMap.contains(name))
				throw MAKE_EXCEPTION(NoSuchElementException, std::format("Texture map does not contain '{}'!", name));

			textureMap.erase(name);
		}

		bool Has(const std::string& name) const
		{
			return textureMap.contains(name);
		}

		std::optional<std::shared_ptr<Texture>> Get(const std::string& name)
		{
			return textureMap.contains(name) ? std::make_optional<std::shared_ptr<Texture>>(textureMap[name]) : std::nullopt;
		}

		std::vector<std::shared_ptr<Texture>> GetAll() const
		{
			std::vector<std::shared_ptr<Texture>> result;

			result.reserve(textureMap.size());

			std::transform(textureMap.begin(), textureMap.end(), std::back_inserter(result), [](const auto& pair) { return pair.second; });

			return result;
		}

		static TextureManager& GetInstance()
		{
			std::call_once(initalizationFlag, [&]()
			{
				instance = std::unique_ptr<TextureManager>(new TextureManager());
			});

			return *instance;
		}

	private:

		TextureManager() = default;

		std::unordered_map<std::string, std::shared_ptr<Texture>> textureMap;

		static std::once_flag initalizationFlag;
		static std::unique_ptr<TextureManager> instance;

	};

	std::once_flag TextureManager::initalizationFlag;
	std::unique_ptr<TextureManager> TextureManager::instance;
}