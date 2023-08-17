#ifndef WINAPPDRIVERPLUGIN_H
#define WINAPPDRIVERPLUGIN_H

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <stdarg.h>
#include <limits.h>
#include <windows.h>
#include "WinappDriverPlugin_global.h"
#include "../common/plugininterface.h"
#include "winappdriverclient.h"
#include "../common/rapidxml.hpp"
#include "../common/AutoObject.h"

using namespace rapidxml;

class WinappDriverPlugin : public PluginInterface
{
public:
    WinappDriverPlugin();
    virtual ~WinappDriverPlugin();
    int launchAUT(const char* autPath);
    const char* getPluginName();
    void getWindowInforString(char* outInfor);
    void getMatchedElement(char* definitions, int length, char* outUUID);
    int initializePlugin();
    void uninitializePlugin();
    const char* getPluginPath();
    int getAgentPath(char* outPath);
private:
    WinAppDriverClient* winApp;
    PROCESS_INFORMATION pi;
    AutoObject* creatAutoObject(xml_node<>* node, AutoObject* parent, Point delta, int winHandle);
};

DLL_EXPORT_FUNC(WinappDriverPlugin)

#endif // WINAPPDRIVERPLUGIN_H
