#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

#define DLL_EXPORT_FUNC(className) \
    extern "C" { \
        __declspec(dllexport) PluginInterface* createInstance() { \
            return new className(); \
        } \
        __declspec(dllexport) void deleteInstance(PluginInterface* instance) { \
            delete instance; \
        } \
    }

class PluginInterface {
public:
    virtual ~PluginInterface() {};
    virtual int launchAUT(const char* autPath) = 0;
    virtual const char* getPluginName() = 0;
    virtual void getWindowInforString(char* outInfor) = 0;
    virtual void getMatchedElement(char* definitions, int length, char* outUUID) = 0;
    virtual int initializePlugin() = 0;
    virtual void uninitializePlugin() = 0;
    virtual const char* getPluginPath() = 0;
    virtual int getAgentPath(char* outPath) = 0;
};

#endif // PLUGININTERFACE_H
