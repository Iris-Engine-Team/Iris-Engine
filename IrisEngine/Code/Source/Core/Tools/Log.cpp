#include "Core/Tools/Log.h"

#include "Core/Tools/FilePath.h"

namespace Iris
{
    namespace Debug
    {
        std::unordered_map<Log::ListenerID, Log::LogCallback> Log::m_Listeners;
        Log::ListenerID Log::nextListenerID = 0;

        std::ofstream Log::m_CurrentLogFile;

        std::mutex Log::m_Mutex;

        void Log::OpenFile(std::filesystem::path const& _filename)
        {
            std::filesystem::path fullpath = std::filesystem::path(LOG_FOLDER_PATH) / _filename;

            // Check if the Log folder exists and creates it if not
            if (!std::filesystem::exists(LOG_FOLDER_PATH))
            {
                DEBUG_WARN("Log folder could not be found\n");
                std::ofstream MyFile("log.txt");
                MyFile.close();
                DEBUG_LOG("Log folder has been created\n");
            }

            m_CurrentLogFile.open(FIND_PATH("log.txt"), std::ios_base::out | std::ios_base::app);

            if (!m_CurrentLogFile)
            {
                DEBUG_WARN("Log file could not be opened\n");
            }
            else
            {
                DEBUG_LOG("Log file opened\n");
            }
        }

        void Log::CloseFile()
        {
            if (!m_CurrentLogFile.is_open())
                return;

            m_CurrentLogFile.close();

            DEBUG_LOG("Log file closed\n");
        }

        int Log::Print(IrisLogStatus _type, const char* _format, ...)
        {
            va_list params;
            va_start(params, _format);

            int size = vsnprintf(nullptr, 0, _format, params);
            if (size < 0) {
                va_end(params);
                return -1;
            }

            std::string output(size + 1, '\0');
            int result = vsnprintf(&output[0], output.size(), _format, params);

            va_end(params);

            m_Mutex.lock();
            switch (_type)
            {
            case IrisLogStatus::IE_LOG:
                Log::Print(output);
                break;
            case IrisLogStatus::IE_WARNING:
                Log::Warning(output);
                break;
            case IrisLogStatus::IE_ERROR:
                Log::Error(output);
                break;
            default:
                break;
            }

            m_Mutex.unlock();
            return result;
        }

        void Log::Print(std::string& _format)
        {
            std::string textToPrint = '[' + CurrentLogTime() + "] " + LOG_PRINT + _format + '\n';
            if (m_CurrentLogFile.is_open())
            {
                m_CurrentLogFile << textToPrint << std::flush;
            }
            for (auto& [id, cb] : m_Listeners)
            {
                cb(textToPrint, Vector3(1.f,1.f,1.f));
            }
            std::cout << textToPrint << std::endl;
        }

        void Log::Warning(std::string& _format)
        {
            std::string textToPrint = '[' + CurrentLogTime() + "] " + LOG_WARN + _format + IE_END + '\n';
            if (m_CurrentLogFile.is_open())
            {
                m_CurrentLogFile << textToPrint << std::flush;
            }
            for (auto& [id, cb] : m_Listeners)
            {
                cb(textToPrint, Vector3(1.f,1.f,0.f));
            }
            std::string debugText = IE_L_YELLOW + textToPrint;
            std::cout << debugText << std::endl;
        }

        void Log::Error(std::string& _format)
        {
            std::string textToPrint = '[' + CurrentLogTime() + "] " + LOG_ERROR + _format + IE_END + '\n';
            if (m_CurrentLogFile.is_open())
            {
                m_CurrentLogFile << textToPrint << std::flush;
            }
            for (auto& [id, cb] : m_Listeners)
            {
                cb(textToPrint, Vector3(1.f,0.f,0.f));
            }
            std::string debugText = IE_L_RED + textToPrint;
            std::cout << debugText << std::endl;
        }

        Log::ListenerID Log::AddListener(LogCallback callback)
        {
            ListenerID id = nextListenerID++;
            m_Listeners[id] = callback;
            return id;
        }

        void Log::RemoveListener(ListenerID id)
        {
            m_Listeners.erase(id);
        }

        std::string Log::CurrentLogTime(bool _fileName)
        {
            auto now = std::chrono::system_clock::now();
            std::time_t now_time = std::chrono::system_clock::to_time_t(now);

            std::tm local_tm;
#ifdef _WIN32
            localtime_s(&local_tm, &now_time); // Windows
#else
            localtime_r(&now_time, &local_tm); // Unix/Linux
#endif

            std::ostringstream oss;
            if (_fileName) {
                oss << std::put_time(&local_tm, "%Y%m%d_%H%M%S");
            }
            else {
                //oss << std::put_time(&local_tm, "%Y/%m/%d %H:%M:%S");
                oss << std::put_time(&local_tm, "%H:%M:%S");
            }

            return oss.str();
        }
    }
}