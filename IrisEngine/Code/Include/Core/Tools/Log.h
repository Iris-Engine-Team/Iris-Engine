#pragma once

#include <mutex>

#include "Vector/Vector3.h"

#include "Core/Tools/Macros.h"

#include <fstream>
#include <iostream>
#include <chrono>
#include <stdarg.h>
#include <Windows.h>
#include <sstream>
#include <string>
#include <functional>

#include <stdio.h>

// Simple colors

constexpr auto IE_RED = "\33[31m";
constexpr auto IE_GREEN = "\33[32m";
constexpr auto IE_BLUE = "\33[34m";
constexpr auto IE_YELLOW = "\33[33m";

// Light colors

constexpr auto IE_L_RED = "\33[91m";
constexpr auto IE_L_GREEN = "\33[92m";
constexpr auto IE_L_BLUE = "\33[94m";
constexpr auto IE_L_YELLOW = "\33[93m";
constexpr auto IE_L_PURPLE = "\33[95m";

// End color (White)

constexpr auto IE_END = "\033[0m";

#define LOG_FOLDER_PATH "/Logs/"
#define LOG_PRINT "[Log] "
#define LOG_WARN "[Warning] "
#define LOG_ERROR "[Error] "

#define __FILE_NAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#define DEBUG_LOG(_format, ...){ Iris::Debug::Log::Print(Iris::Debug::IrisLogStatus::IE_LOG, _format, __VA_ARGS__);}
#define DEBUG_WARN(_format, ...){ Iris::Debug::Log::Print(Iris::Debug::IrisLogStatus::IE_WARNING, _format, __VA_ARGS__);}
#define DEBUG_ERROR(_format, ...){ Iris::Debug::Log::Print(Iris::Debug::IrisLogStatus::IE_ERROR, _format, __VA_ARGS__);}


namespace Iris
{
    namespace Debug
    {
        enum class IrisLogStatus
        {
            IE_LOG,
            IE_WARNING,
            IE_ERROR
        };

        class Log
        {
        public:
            using LogCallback = std::function<void(const std::string&, Vector3)>;
            using ListenerID = size_t;

        public:
            /// <summary>
            /// Open a log file
            /// </summary>
            /// <param name="_filename"> : path to the log file</param>
            /// <returns></returns>
            static void OpenFile(const std::filesystem::path& _filename);

            /// <summary>
            /// Close the current log file if it is open
            /// </summary>
            /// <returns></returns>
            IRIS_ENGINE_API static void CloseFile();

            /// <summary>
            /// Print function
            /// </summary>
            /// <param name="_type">: log status</param>
            /// <param name="_format">: message you want to display</param>
            /// <param name=""></param>
            /// <returns>-1 if fail</returns>
            IRIS_ENGINE_API static int Print(IrisLogStatus _type, const char* _format, ...);

            IRIS_ENGINE_API static Log::ListenerID AddListener(LogCallback callback);
            IRIS_ENGINE_API static void RemoveListener(ListenerID id);

        private:
            /// <summary>
            /// Print Log
            /// </summary>
            /// <param name="_format">: message you want to display</param>
            static void Print(std::string& _format);

            /// <summary>
            /// Print Warning
            /// </summary>
            /// <param name="_format">: message you want to display</param>
            static void Warning(std::string& _format);

            /// <summary>
            /// Print Error
            /// </summary>
            /// <param name="_format">: message you want to display</param>
            static void Error(std::string& _format);

            /// <summary>
            /// Returns the current time
            /// </summary>
            /// <param name="_fileName"> : specifies the format of the time</param>
            /// <returns>The current time</returns>
            static std::string CurrentLogTime(bool _fileName = false);

        private:
            static std::mutex m_Mutex;

            static std::ofstream m_CurrentLogFile;
            static std::unordered_map<ListenerID, LogCallback> m_Listeners;
            static ListenerID nextListenerID;
        };
    }
}