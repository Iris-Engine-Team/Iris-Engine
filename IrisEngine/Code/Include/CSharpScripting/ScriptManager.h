#include "dotnet/nethost.h"
#include "dotnet/coreclr_delegates.h"
#include "dotnet/hostfxr.h"

#include <iostream>
#include <assert.h>
#include <vector>

#include "Core/Tools/Macros.h"


#define STR(s) L##s

typedef void (CORECLR_DELEGATE_CALLTYPE* custom_entry_point_fn)(void* _objPtr);
typedef void* (CORECLR_DELEGATE_CALLTYPE* init_script_entry_point_fn)(void* scriptHandle, const char* _typeName);
typedef void (CORECLR_DELEGATE_CALLTYPE* callback_entry_point_fn)(void* _scriptPtr, void* _rigidBodyHandle);

using get_hostfxr_path_fn = int(HOSTFXR_CALLTYPE*)(char_t*, size_t*, const get_hostfxr_parameters*);


namespace Iris
{

    namespace Engine
    {
        class Time;
        class SceneManager;
        class Scene;
        class ScriptComponent;
        class RigidBody;
        
        template<class T>
        class ComponentHandle;

        enum class SingletonType : int
        {
            TIME = 0,
            SCENE_MANAGER = 1,
            SCENE = 2
        };

        enum class FuncType : int
        {
            AWAKE = 0,
            START = 1,
            UPDATE = 2,
            FIXED_UPDATE = 3,
            COL_ENTER = 4,
            COL_STAY = 5,
            COL_EXIT = 6,
            TRIG_ENTER = 7,
            TRIG_STAY = 8,
            TRIG_EXIT = 9
        };

        class ScriptManager
        {
        public:
            IRIS_ENGINE_API void Init();
            IRIS_ENGINE_API void InitCSharpSingleton(void* _singletonPtr, SingletonType _singletonType);
            void* InitCSharpScript(ComponentHandle<ScriptComponent>* _scriptHandle);
            void DestroyCSharpScript(void* _scriptInstance);
            void RunCSFunction(const ScriptComponent* _script, FuncType _fnType);
            void RunCSCallback(const ScriptComponent* _script, FuncType _fnType, ComponentHandle<RigidBody>* _rigidBody);

        protected:
        private:
            void* LoadLib(const char_t* _path);
            void* GetExport(void* _h, const char* _name);
            void LoadHostfxr(const char_t* assembly_path);
            void GetDotnetLoadAssembly(const char_t* _configPath);
            static std::wstring ToWstring(std::string _str);

        public:
        protected:
        private:
            std::wstring m_RootPath = L"";
            hostfxr_initialize_for_runtime_config_fn m_InitForConfigFptr = nullptr;
            hostfxr_get_runtime_delegate_fn m_GetDelegateFptr = nullptr;
            hostfxr_close_fn m_CloseFptr = nullptr;
            load_assembly_and_get_function_pointer_fn m_LoadAssemblyAndGetFunctionPointer = nullptr;
            std::vector<std::wstring> m_csFunc{ L"CallAwake", L"CallStart", L"CallUpdate", L"CallFixedUpdate",
                L"CallCollisionEnter", L"CallCollisionStay", L"CallCollisionExit",
                L"CallTriggerEnter", L"CallTriggerStay", L"CallTriggerExit" };
            std::vector<std::wstring> m_csSingleton{ L"Time, UserCSharpProject", L"SceneManager, UserCSharpProject", L"Scene, UserCSharpProject" };
        };
    }
}