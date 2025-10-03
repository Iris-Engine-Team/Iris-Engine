#include "CSharpScripting/ScriptManager.h"

#include "Core/Time.h"

#include "Core/Tools/FilePath.h"
#include "Core/Tools/Log.h"
#include "Windows.h"

#include "SECS/Components/IComponent.h"
#include "SECS/Components/Transform.h"
#include "SECS/Components/RigidBody.h"
#include "SECS/Components/Collider.h"
#include "SECS/Components/CameraComponent.h"
#include "SECS/Components/LightComponent.h"
#include "SECS/Components/ModelRenderer.h"
#include "SECS/Components/ScriptComponent.h"


namespace Iris
{

    namespace Engine
    {

        void ScriptManager::Init()
        {
            // Get exe path
            m_RootPath = ToWstring(GetExecutablePath().string() + '\\');

            // STEP 1: Load HostFxr and get exported hosting functions
            LoadHostfxr(nullptr);

            // STEP 2: Initialize and start the .NET Core runtime
            const std::basic_string<char_t> configPath = m_RootPath + STR("CSharp.runtimeconfig.json");
            GetDotnetLoadAssembly(configPath.c_str());
        }

        void ScriptManager::InitCSharpSingleton(void* _singletonPtr, SingletonType _singletonType)
        {
            // STEP 3: Load managed assembly and get function pointer to a managed method
            std::basic_string<char_t> dotnetLibPath = m_RootPath + STR("UserCSharpProject.dll");
            const char_t* dotnetType = &(m_csSingleton[static_cast<int>(_singletonType)][0]);
            const char_t* dotnetTypeMethod = STR("Init");
            // Function pointer to managed delegate
            custom_entry_point_fn csFunction = nullptr;
            int rc = m_LoadAssemblyAndGetFunctionPointer(
                dotnetLibPath.c_str(),
                dotnetType,
                dotnetTypeMethod,
                UNMANAGEDCALLERSONLY_METHOD,
                nullptr,
                (void**)&csFunction);

            if (rc != 0 || !csFunction)
                DEBUG_WARN("Faile to get load_assembly_and_get_function_pointer()!");

            // STEP 4: Run managed code
            csFunction(_singletonPtr);
        }

        void* ScriptManager::InitCSharpScript(ComponentHandle<ScriptComponent>* _scriptHandle)
        {
            // STEP 3: Load managed assembly and get function pointer to a managed method
            const std::basic_string<char_t> dotnetLibPath = m_RootPath + STR("UserCSharpProject.dll");
            const char_t* dotnetType = STR("ScriptFactory, UserCSharpProject");
            const char_t* dotnetTypeMethod = STR("CreateScriptInstance");
            // Function pointer to managed delegate
            init_script_entry_point_fn csFunction = nullptr;
            int rc = m_LoadAssemblyAndGetFunctionPointer(
                dotnetLibPath.c_str(),
                dotnetType,
                dotnetTypeMethod,
                UNMANAGEDCALLERSONLY_METHOD,
                nullptr,
                (void**)&csFunction);

            if (rc != 0 || !csFunction)
                DEBUG_WARN("Faile to get load_assembly_and_get_function_pointer()!");

            // STEP 4: Run managed code
            std::string fullName = _scriptHandle->UseComponent()->GetScriptName();
            size_t lastindex = fullName.find_last_of(".");
            std::string rawName = fullName.substr(0, lastindex);
            void* result = csFunction(_scriptHandle, rawName.c_str());
            return result;
        }

        void ScriptManager::DestroyCSharpScript(void* _scriptInstance)
        {
            // STEP 3: Load managed assembly and get function pointer to a managed method
            const std::basic_string<char_t> dotnetLibPath = m_RootPath + STR("UserCSharpProject.dll");
            const char_t* dotnetType = STR("ScriptFactory, UserCSharpProject");
            const char_t* dotnetTypeMethod = STR("DestroyScriptInstance");
            // Function pointer to managed delegate
            custom_entry_point_fn csFunction = nullptr;
            int rc = m_LoadAssemblyAndGetFunctionPointer(
                dotnetLibPath.c_str(),
                dotnetType,
                dotnetTypeMethod,
                UNMANAGEDCALLERSONLY_METHOD,
                nullptr,
                (void**)&csFunction);

            if (rc != 0 || !csFunction)
                DEBUG_WARN("Faile to get load_assembly_and_get_function_pointer()!");

            // STEP 4: Run managed code
            csFunction(_scriptInstance);
        }

        void ScriptManager::RunCSFunction(const ScriptComponent* _script, FuncType _fnType)
        {
            // STEP 3: Load managed assembly and get function pointer to a managed method
            const std::basic_string<char_t> dotnetLibPath = m_RootPath + STR("UserCSharpProject.dll");
            const char_t* dotnetType = STR("ScriptFactory, UserCSharpProject");
            const char_t* dotnetTypeMethod = &m_csFunc[static_cast<int>(_fnType)][0];
            // Function pointer to managed delegate
            custom_entry_point_fn csFunction = nullptr;
            int rc = m_LoadAssemblyAndGetFunctionPointer(
                dotnetLibPath.c_str(),
                dotnetType,
                dotnetTypeMethod,
                UNMANAGEDCALLERSONLY_METHOD,
                nullptr,
                (void**)&csFunction);

            if (rc != 0 || !csFunction)
                DEBUG_WARN("Faile to get load_assembly_and_get_function_pointer()!");

            // STEP 4: Run managed code
            csFunction(_script->GetScriptInstance());
        }

        void ScriptManager::RunCSCallback(const ScriptComponent* _script, FuncType _fnType, ComponentHandle<RigidBody>* _rigidBody)
        {
            // STEP 3: Load managed assembly and get function pointer to a managed method
            const std::basic_string<char_t> dotnetLibPath = m_RootPath + STR("UserCSharpProject.dll");
            std::wstring wDotnetType = ToWstring("ScriptFactory, UserCSharpProject");
            const char_t* dotnetType = &wDotnetType[0];
            const char_t* dotnetTypeMethod = &m_csFunc[static_cast<int>(_fnType)][0];
            // Function pointer to managed delegate
            callback_entry_point_fn csFunction = nullptr;
            int rc = m_LoadAssemblyAndGetFunctionPointer(
                dotnetLibPath.c_str(),
                dotnetType,
                dotnetTypeMethod,
                UNMANAGEDCALLERSONLY_METHOD,
                nullptr,
                (void**)&csFunction);

            if (rc != 0 || !csFunction)
                DEBUG_WARN("Faile to get load_assembly_and_get_function_pointer()!");

            // STEP 4: Run managed code
            csFunction(_script->GetScriptInstance(), _rigidBody);
        }

        void* ScriptManager::LoadLib(const char_t* _path)
        {
            HMODULE h = ::LoadLibraryW(_path);

            if (!h)
                DEBUG_WARN("Library not found!");

            return (void*)h;
        }

        void* ScriptManager::GetExport(void* _h, const char* _name)
        {
            void* f = ::GetProcAddress((HMODULE)_h, _name);

            if (!f)
                DEBUG_WARN("%s lib's function not fond!", _name);

            return f;
        }

        get_hostfxr_path_fn load_get_hostfxr_path()
        {
            HMODULE hNethost = LoadLibraryW(L"nethost.dll");
            if (!hNethost)
            {
                return nullptr;
            }

            FARPROC pFunc = GetProcAddress(hNethost, "get_hostfxr_path");
            if (!pFunc)
            {
                return nullptr;
            }

            return reinterpret_cast<get_hostfxr_path_fn>(pFunc);
        }

        void ScriptManager::LoadHostfxr(const char_t* assembly_path)
        {
            get_hostfxr_path_fn get_hostfxr_path = load_get_hostfxr_path();
            if (!get_hostfxr_path)
            {
                DEBUG_WARN("get_hostfxr_path function not found!")
                return;
            }

            // Get hostfxr path
            get_hostfxr_parameters params{ sizeof(get_hostfxr_parameters), assembly_path, nullptr };
            char_t hostfxr_path[MAX_PATH];
            size_t size = sizeof(hostfxr_path) / sizeof(char_t);
            int rc = get_hostfxr_path(hostfxr_path, &size, &params);
            if (rc != 0)
            {
                DEBUG_WARN("hostfxr path not found!")
                return;
            }

            // Load hostfxr and get desired exports
            // NOTE: The .NET Runtime does not support unloading any of its native libraries. Running
            // dlclose/FreeLibrary on any .NET libraries produces undefined behavior.
            void* lib = LoadLib(hostfxr_path);
            m_InitForConfigFptr = (hostfxr_initialize_for_runtime_config_fn)GetExport(lib, "hostfxr_initialize_for_runtime_config");
            m_GetDelegateFptr = (hostfxr_get_runtime_delegate_fn)GetExport(lib, "hostfxr_get_runtime_delegate");
            m_CloseFptr = (hostfxr_close_fn)GetExport(lib, "hostfxr_close");

            if (!(m_InitForConfigFptr && m_GetDelegateFptr && m_CloseFptr))
                DEBUG_WARN("initForConfigFptr, getDelegateFptr or closeFptr not initialized!");
        }

        void ScriptManager::GetDotnetLoadAssembly(const char_t* _configPath)
        {
            void* load_assembly_and_get_function_pointer = nullptr;
            hostfxr_handle cxt = nullptr;
            int rc = m_InitForConfigFptr(_configPath, nullptr, &cxt);
            if (rc != 0 || cxt == nullptr)
            {
                DEBUG_WARN("Faile to initialize dotnet runtime!")
                    m_CloseFptr(cxt);
                return;
            }

            rc = m_GetDelegateFptr(
                cxt,
                hdt_load_assembly_and_get_function_pointer,
                &load_assembly_and_get_function_pointer);
            if (rc != 0 || load_assembly_and_get_function_pointer == nullptr)
                DEBUG_WARN("Faile to get delegate!");

            m_CloseFptr(cxt);
            m_LoadAssemblyAndGetFunctionPointer = (load_assembly_and_get_function_pointer_fn)load_assembly_and_get_function_pointer;

            if (!m_LoadAssemblyAndGetFunctionPointer)
                DEBUG_WARN("Faile to get dotnet load assembly!")
        }

        std::wstring ScriptManager::ToWstring(std::string _str)
        {

            return std::wstring(_str.begin(), _str.end());
        }
    }
}