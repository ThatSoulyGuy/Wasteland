#pragma once

#include <iostream>
#include <string>
#include <sstream>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#undef ERROR
#undef min
#undef max
#undef WriteConsole
#undef ReadConsole
#undef GetMessage
#elif defined __linux__ || __apple__
#include <execinfo.h>
#endif

#define MAKE_EXCEPTION(type, message) type(__FUNCTION__, __LINE__, message)

namespace Wasteland::Utility::Exception
{
    class BaseException
    {

    public:

        BaseException(const std::string& function, int line, const std::string& message) : function(function), line(line), message(message)
        {
            stackTrace = CaptureStackTrace();
        }

        virtual ~BaseException() = default;

        virtual std::string GetType() const = 0;
        
        std::string GetStackTrace() const
        {
            return stackTrace;
        }

        std::string GetFunction() const
        {
            return function;
        }

        int GetLine() const
        {
            return line;
        }

        std::string GetMessage() const
        {
            return message;
        }

        virtual void PrintAndTerminate() const
        {
            std::cerr << "\x1b[1;31m[ ===== " << GetType() << " =====\n\n"
                << "  Function: " << function << "\n"
                << "  Line: " << line << "\n"
                << "  Message: " << message << "\n\n"
                << "  Stack Trace:\n" << stackTrace
                << "]\n\x1b[1;0m";

            std::exit(EXIT_FAILURE);
        }

        static void OnException(const BaseException& exception)
        {
            exception.PrintAndTerminate();
        }

    private:

        static std::string CaptureStackTrace(size_t maxFrames = 64)
        {
#ifdef __linux__ || __apple__
            std::vector<void*> array(maxFrames);

            size_t size = backtrace(array.data(), maxFrames);
            char** symbols = backtrace_symbols(array.data(), size);

            std::ostringstream oss;

            for (size_t i = 3; i < size; ++i)
                oss << symbols[i] << "\n";
            
            free(symbols);

            return oss.str();

#elif defined(_WIN32)
            HANDLE process = GetCurrentProcess();
            SymInitialize(process, NULL, TRUE);

            void* stack[64];

            unsigned short frames = CaptureStackBackTrace(0, maxFrames, stack, NULL);
            SYMBOL_INFO* symbol = (SYMBOL_INFO*)malloc(sizeof(SYMBOL_INFO) + 256);

            symbol->MaxNameLen = 255;
            symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

            std::ostringstream oss;

            for (unsigned short i = 3; i < frames; i++)
            {
                SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol);
                oss << "    " << i - 3 << ": " << symbol->Name << " - 0x" << symbol->Address << "\n";
            }

            free(symbol);
            return oss.str();
#else
            return "Stack tracing not supported on this platform.";
#endif
        }

        std::string stackTrace;
        std::string function;
        int line;
        std::string message;
    };
}