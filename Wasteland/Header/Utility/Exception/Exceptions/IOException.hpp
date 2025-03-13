#pragma once

#include "Utility/Exception/BaseException.hpp"

using namespace Wasteland::Utility::Exception;

namespace Wasteland::Utility::Exception::Exceptions
{
	class IOException final : public BaseException
	{

	public:

		IOException(const std::string& function, int line, const std::string& message) : BaseException(function, line, message) {}

		std::string GetType() const override { return "IOException"; }

	};
}