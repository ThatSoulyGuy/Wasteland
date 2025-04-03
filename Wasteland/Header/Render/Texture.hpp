#pragma once

#include <string>
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <FreeImage.h>
#include "ECS/Component.hpp"
#include "Math/Vector.hpp"
#include "Utility/AssetPath.hpp"
#include "Utility/FileSystem.hpp"
#include "Utility/Exception/Exceptions/IOException.hpp"

using namespace Wasteland::ECS;
using namespace Wasteland::Math;
using namespace Wasteland::Utility;
using namespace Wasteland::Utility::Exception::Exceptions;

namespace Wasteland::Render
{
    class Texture final : public Component
    {

    public:

        Texture(const Texture&) = delete;
        Texture(Texture&&) = delete;
        Texture& operator=(const Texture&) = delete;
        Texture& operator=(Texture&&) = delete;

        std::string GetName() const
        {
            return name;
        }

        AssetPath GetPath() const
        {
            return path;
        }

        void Bind(unsigned int slot)
        {
            glActiveTexture(GL_TEXTURE0 + slot);
            glBindTexture(GL_TEXTURE_2D, id);
        }

        void Unbind()
        {
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        void Uninitialize()
        {
            glDeleteTextures(1, &id);
        }

        static std::shared_ptr<Texture> Create(const std::string& name, const AssetPath& path)
        {
            std::shared_ptr<Texture> result(new Texture());

            result->name = name;
            result->path = path;

            result->Generate();

            return result;
        }

    private:

        Texture() = default;

        void Generate()
        {
            std::string fullPath = path.GetFullPath();

            FreeImage_Initialise();
            FIBITMAP* bitmap = FreeImage_Load(FIF_PNG, fullPath.c_str(), PNG_DEFAULT);

            if (!bitmap)
                throw MAKE_EXCEPTION(IOException, std::format("Failed to load texture '{}'!", name));

            FIBITMAP* image = FreeImage_ConvertTo32Bits(bitmap);

            FreeImage_Unload(bitmap);

            int width = FreeImage_GetWidth(image);
            int height = FreeImage_GetHeight(image);
            void* pixels = FreeImage_GetBits(image);

            glGenTextures(1, &id);
            glBindTexture(GL_TEXTURE_2D, id);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixels);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            FreeImage_Unload(image);

            glBindTexture(GL_TEXTURE_2D, 0);

            FreeImage_DeInitialise();
        }

        std::string name;
        AssetPath path;
        unsigned int id = 0;
    };
}