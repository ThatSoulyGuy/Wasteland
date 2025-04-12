#pragma once

#include "Utility/Exception/BaseException.hpp"

using namespace Wasteland::Utility::Exception;

namespace Wasteland::Utility::Exception::Exceptions
{
	class NullPointerException final : public BaseException
	{

	public:

		NullPointerException(const std::string& function, int line, const std::string& message) : BaseException(function, line, message) {}

		std::string GetType() const override { return "NullPointerException"; }

	};
}