/*
 * pluginmanager.h
 *
 *  Created on: Jul 24, 2023
 *      Author: cuongnht
 */
#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <iostream>
#ifdef BUILD_ON_LINUX
#include <dirent.h>
#include <dlfcn.h> // For Linux and macOS
#else
#include <windows.h>
#include <functional>
#include <locale>
#include <codecvt>
#endif
#include <string>
#include <map>

#include "viewerautoobject.h"
#include "../common/plugininterface.h"

typedef const char* (*GetNameFunc)();
typedef const char* (*GetPluginPathFunc)();
typedef int (*GetAgentPathFunc)(char*);
typedef int (*GetNumberFunc)();
typedef ViewerAutoObject* (*GetWindowInfor)();
typedef void (*GetWindowInforString)(char*);
typedef void (*InitializePlugin)();
typedef void (*UnInitializePlugin)();

typedef PluginInterface* (__cdecl* CreateFunc)();
typedef void (__cdecl* DeleteFunc)(PluginInterface*);

class PluginManager
{
public:
    PluginManager();
    void loadPlugins(const std::string& folderPath);
    void unloadPlugins();
    int callGetNumber(const std::string& pluginName);
    ViewerAutoObject* getWindowInfor(const std::string& pluginName);
    void getWindowInforString(const std::string& pluginName, char* outInfor);
    int initializePlugin(const std::string& pluginName);
    void uninitializePlugin(const std::string& pluginName);
    string getPluginPath(const std::string& pluginName);
    string getAgentPath(const std::string& pluginName);
    int launchAUT(const std::string& pluginName, const std::string autPath);
    vector<string> supportedPlugins() {
        std::vector<string> pluginNames;
        for(auto const& pluginName: dllHandles_)
            pluginNames.push_back(pluginName.first);
        return pluginNames;
    }
private:
    std::map<std::string, PluginInterface*> dllHandles_;
    std::map<std::string, DeleteFunc> dllDestructor;
    bool endsWith(const std::string& str, const std::string& suffix) {
            return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
        }

    bool endsWith(const std::wstring& str, const std::wstring& suffix) {
        return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }

};

#endif // PLUGINMANAGER_H
