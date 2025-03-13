#pragma once

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace Wasteland::Utility
{
    class FileSystem
    {

    public:

        FileSystem(const FileSystem&) = delete;
        FileSystem(FileSystem&&) = delete;
        FileSystem& operator=(const FileSystem&) = delete;
        FileSystem& operator=(FileSystem&&) = delete;

        static bool CreateFile(const std::string& filePath)
        {
            std::ofstream file(filePath);

            return file.good();
        }

        static bool WriteToFile(const std::string& filePath, const std::string& content)
        {
            std::ofstream file(filePath, std::ios::out);

            if (!file)
                return false;

            file << content;

            return true;
        }

        static bool AppendToFile(const std::string& filePath, const std::string& content)
        {
            std::ofstream file(filePath, std::ios::app);

            if (!file)
                return false;

            file << content;

            return true;
        }

        static std::string ReadFile(const std::string& filePath)
        {
            std::ifstream file(filePath, std::ios::in);

            if (!file)
                return "";

            return { std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
        }

        static bool FileExists(const std::string& filePath)
        {
            return std::filesystem::exists(filePath);
        }

        static std::uintmax_t GetFileSize(const std::string& filePath)
        {
            return std::filesystem::exists(filePath) ? std::filesystem::file_size(filePath) : 0;
        }

        static bool CreateDirectory(const std::string& dirPath)
        {
            return std::filesystem::create_directories(dirPath);
        }

        static bool DirectoryExists(const std::string& dirPath)
        {
            return std::filesystem::exists(dirPath) && std::filesystem::is_directory(dirPath);
        }

        static bool RemoveFile(const std::string& filePath)
        {
            return std::filesystem::remove(filePath);
        }

        static bool RemoveDirectory(const std::string& dirPath)
        {
            return std::filesystem::remove_all(dirPath) > 0;
        }

        static std::vector<std::string> ListFiles(const std::string& dirPath)
        {
            std::vector<std::string> files;

            if (!DirectoryExists(dirPath))
                return files;

            for (const auto& entry : std::filesystem::directory_iterator(dirPath))
                files.push_back(entry.path().string());
            
            return files;
        }

        static std::time_t GetLastWriteTime(const std::string& filePath)
        {
            if (!std::filesystem::exists(filePath))
                return 0;

            auto ftime = std::filesystem::last_write_time(filePath);

            auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());

            return std::chrono::system_clock::to_time_t(sctp);
        }


    private:

        FileSystem() = default;

    };
}