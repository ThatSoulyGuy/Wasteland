#pragma once

#include "Math/Vector.hpp"
#include <cereal/cereal.hpp>

using namespace Wasteland::Math;

namespace Wasteland::Render
{
	struct Vertex
	{
		Vector<float, 3> position;
		Vector<float, 3> color;
		Vector<float, 3> normal;
		Vector<float, 2> uvs;

		template <typename Archive>
		void serialize(Archive& archive)
		{
			archive(position, color, normal, uvs);
		}
	};
}