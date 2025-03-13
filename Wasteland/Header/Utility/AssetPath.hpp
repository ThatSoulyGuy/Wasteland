#pragma once

#include <format>
#include <string>

namespace Wasteland::Utility
{
	struct AssetPath final
	{

	public:

		AssetPath() = default;

		AssetPath(const std::string& domain, const std::string& localPath) : domain(domain), localPath(localPath) { }

		std::string GetDomain() const
		{
			return domain;
		}

		std::string GetLocalPath() const
		{
			return localPath;
		}

		std::string GetFullPath() const
		{
			return std::format("Assets/{}/{}", domain, localPath);
		}

	private:

		std::string domain;
		std::string localPath;

	};
}