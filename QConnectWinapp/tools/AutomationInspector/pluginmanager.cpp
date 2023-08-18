/*
 * pluginmanager.cpp
 *
 *  Created on: Jul 24, 2023
 *      Author: cuongnht
 */
#include "pluginmanager.h"
#include "../common/MessageType.h"

PluginManager::PluginManager()
{

}

void PluginManager::loadPlugins(const std::string& folderPath) {
#ifdef BUILD_ON_LINUX
    DIR* dir = opendir(folderPath.c_str());
    if (dir) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            if (entry->d_type == DT_REG) { // Check if it's a regular file
                std::string filename = entry->d_name;
                if (endsWith(filename, ".dll") || endsWith(filename, ".so") || endsWith(filename, ".dylib")) {
                    std::string fullPath = folderPath + "/" + filename;
                    void* handle = dlopen(fullPath.c_str(), RTLD_NOW);
                    if (handle != nullptr) {
                        GetNameFunc getName = reinterpret_cast<GetNameFunc>(dlsym(handle, "getPluginName"));
                        if (getName) {
                            const char* pluginName = getName();
                            std::string name = string(getName());
                            dllHandles_[name] = handle;
                            std::cout << "Loaded plugin: " << name << std::endl;
                        }
                    }
                    else {
                        std::cerr << "dlopen failed: " << dlerror() << std::endl;
                    }
                }
            }
        }
        closedir(dir);
    }
#else
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wFolderPath = converter.from_bytes(folderPath);
    WIN32_FIND_DATAW findFileData;
    HANDLE hFind = FindFirstFileW((wFolderPath + L"\\*").c_str(), &findFileData);
    std::cout << "Load plugins...";
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                continue; // Skip directories
            }

            std::wstring filename = findFileData.cFileName;
            if (endsWith(filename, L".dll") || endsWith(filename, L".so") || endsWith(filename, L".dylib")) {
                std::wstring wFullPath = wFolderPath + L"\\" + filename;
                HMODULE handle = LoadLibraryW(wFullPath.c_str());

                if (handle != nullptr) {
                    CreateFunc createInstance = reinterpret_cast<CreateFunc>(GetProcAddress(handle, "createInstance"));
                    DeleteFunc deleteInstance = reinterpret_cast<DeleteFunc>(GetProcAddress(handle, "deleteInstance"));
                    if (createInstance && deleteInstance) {
                        PluginInterface* instance = createInstance();
                        std::string name(instance->getPluginName());
                        dllHandles_[name] = instance;
                        dllDestructor[name] = deleteInstance;
                        std::cout << "Loaded plugin: " << name << std::endl;
                    }
                } else {
                    DWORD error = GetLastError();
                    LPVOID lpMsgBuf;
                     FormatMessageW(
                        FORMAT_MESSAGE_ALLOCATE_BUFFER |
                        FORMAT_MESSAGE_FROM_SYSTEM |
                        FORMAT_MESSAGE_IGNORE_INSERTS,
                        NULL,
                        error,
                        0, // Default language
                        reinterpret_cast<LPWSTR>(&lpMsgBuf),
                        0,
                        NULL
                    );
                    std::wcerr << L"LoadLibrary failed: " << reinterpret_cast<LPWSTR>(lpMsgBuf) << std::endl;
                    LocalFree(lpMsgBuf);
                }
            }
        } while (FindNextFileW(hFind, &findFileData) != 0);
        FindClose(hFind);
    }

#endif
}

///
/// \brief PluginManager::unloadPlugins
///
void PluginManager::unloadPlugins()
{
    for (auto it = dllDestructor.begin(); it != dllDestructor.end(); it++)
    {
        if (dllHandles_[it->first])
        {
            dllDestructor[it->first](dllHandles_[it->first]);
        }
    }
}

string PluginManager::getPluginPath(const std::string& pluginName)
{
    string res = "";
    res = string(dllHandles_[pluginName]->getPluginPath());
//#ifdef BUILD_ON_LINUX
//    void* handle = dllHandles_[pluginName];
//#else
//    HMODULE handle = (HMODULE)dllHandles_[pluginName];
//#endif
//    if (handle) {
//#ifdef BUILD_ON_LINUX
//        GetPluginPathFunc getPluginPath = reinterpret_cast<GetPluginPathFunc>(dlsym(handle, "getPluginPath"));
//#else
//        GetPluginPathFunc getPluginPath = reinterpret_cast<GetPluginPathFunc>(GetProcAddress(handle, "getPluginPath"));
//#endif
//        if (getPluginPath) {
//            res = string(getPluginPath());
//        }
//    }
    return res;
}

int PluginManager::launchAUT(const std::string& pluginName, const std::string autPath)
{
    int iRes = -1;
    PluginInterface* plugin = dllHandles_[pluginName];
    iRes = plugin->launchAUT(autPath.c_str());
    return iRes;
}

string PluginManager::getAgentPath(const std::string& pluginName)
{
    string res = "";
    void* handle = dllHandles_[pluginName];
    if (handle) {
#ifdef BUILD_ON_LINUX
        GetAgentPathFunc getAgentPath = reinterpret_cast<GetAgentPathFunc>(dlsym(handle, "getAgentPath"));
#else
        GetAgentPathFunc getAgentPath = reinterpret_cast<GetAgentPathFunc>(GetProcAddress((HMODULE)handle, "getAgentPath"));
#endif
        if (getAgentPath) {
//            res = string(getAgentPath());
            char *agentPath = new char[MAX_MSG_SIZE];
            memset(agentPath, 0, MAX_MSG_SIZE);
            if (getAgentPath(agentPath) != -1)
            {
                res = string(agentPath);
            }
            delete []agentPath;
        }
    }
    return res;
}

int PluginManager::callGetNumber(const std::string& pluginName) {
    void* handle = dllHandles_[pluginName];
    if (handle)
    {
#ifdef BUILD_ON_LINUX
        GetNumberFunc getNumber = reinterpret_cast<GetNumberFunc>(dlsym(handle, "getNumber"));
#else
        GetNumberFunc getNumber = reinterpret_cast<GetNumberFunc>(GetProcAddress((HMODULE)handle, "getNumber"));
#endif
        if (getNumber) {
               return getNumber();
        }
    }
    return -1; // Indicates an error
}

ViewerAutoObject* PluginManager::getWindowInfor(const std::string& pluginName) {
    void* handle = dllHandles_[pluginName];
    if (handle) {
#ifdef BUILD_ON_LINUX
        GetWindowInfor getWindowInfor = reinterpret_cast<GetWindowInfor>(dlsym(handle, "getWindowInfor"));
#else
        GetWindowInfor getWindowInfor = reinterpret_cast<GetWindowInfor>(GetProcAddress((HMODULE)handle, "getWindowInfor"));
#endif
        if (getWindowInfor) {
            ViewerAutoObject* obj = getWindowInfor();
            return obj;
        }
    }
    return NULL; // Indicates an error
}

void PluginManager::getWindowInforString(const std::string& pluginName, char* outInfor) {
    PluginInterface* plugin = dllHandles_.at(pluginName);
    if (plugin)
    {
        plugin->getWindowInforString(outInfor);
    }
}

int PluginManager::initializePlugin(const std::string& pluginName)
{
//    void* handle = dllHandles_[pluginName];
//    if (handle) {
//#ifdef BUILD_ON_LINUX
//        InitializePlugin initializePlugin = reinterpret_cast<InitializePlugin>(dlsym(handle, "initializePlugin"));
//#else
//        InitializePlugin initializePlugin = reinterpret_cast<InitializePlugin>(GetProcAddress((HMODULE)handle, "initializePlugin"));
//#endif
//        if (initializePlugin) {
//            initializePlugin();
//        }
//    }
    int iRes = -1;
    PluginInterface* plugin = dllHandles_[pluginName];
    iRes = plugin->initializePlugin();
    return iRes;
}

void PluginManager::uninitializePlugin(const std::string& pluginName)
{
//    void* handle = dllHandles_[pluginName];
//    if (handle) {
//#ifdef BUILD_ON_LINUX
//        UnInitializePlugin uninitializePlugin = reinterpret_cast<UnInitializePlugin>(dlsym(handle, "uninitializePlugin"));
//#else
//        UnInitializePlugin uninitializePlugin = reinterpret_cast<UnInitializePlugin>(GetProcAddress((HMODULE)handle, "uninitializePlugin"));
//#endif
//        if (uninitializePlugin) {
//            uninitializePlugin();
//        }
//    }
    PluginInterface* plugin = dllHandles_[pluginName];
    plugin->uninitializePlugin();
}
