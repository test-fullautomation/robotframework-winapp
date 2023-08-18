#ifndef WINAPPDRIVERCLIENT_H
#define WINAPPDRIVERCLIENT_H

#include <string>
#include <map>
#include <vector>
#include <cpprest/http_client.h>
#include <cpprest/json.h> // For JSON parsing
#include <cpprest/asyncrt_utils.h> // For con
#include <locale>
#include <codecvt>
//#include <format>
#include <utility>
#include <windows.h>
#include "../common/rapidxml.hpp"

using namespace std;

struct Point
{
    int x;
    int y;
};

struct WinSize
{
    unsigned w;
    unsigned h;
};

enum class WINAPP_DRIVER_REQUEST
{
    LAUNCH_AUT_REQ=1,
    GET_WINDOW_HWND_REQ,
    GET_WINDOW_SIZE_REQ,
    GET_WINDOW_POS_REQ,
    GET_ELEMENTS_REQ,
    GET_ELEMENT_CHILDREN_REQ,
    GET_SOURCE_REQ
};

const std::map<WINAPP_DRIVER_REQUEST,  std::pair <std::string, std::string>> WINAPP_DRIVER_MAP =
{
    { WINAPP_DRIVER_REQUEST::LAUNCH_AUT_REQ             , {    std::string("/session"),
                                                               std::string("{"
                                                                               "\"capabilities\": { "
                                                                                 "\"capabilities\": { "
                                                                                     "\"firstMatch\": [{}],  "
                                                                                     "\"alwaysMatch\": { "
                                                                                         "\"appium:app\": \"%s\" "
                                                                                     "} "
                                                                                 "} "
                                                                               "},  "
                                                                               "\"desiredCapabilities\": { "
                                                                                   "\"app\": \"%s\" "
                                                                               "}"
                                                                             "}") } },
    { WINAPP_DRIVER_REQUEST::GET_WINDOW_HWND_REQ        , {     std::string("/session/%s/window_handle"),
                                                                std::string("") } },
    { WINAPP_DRIVER_REQUEST::GET_WINDOW_POS_REQ         , {     std::string("/session/%s/window/%s/position"),
                                                                std::string("") } },
    { WINAPP_DRIVER_REQUEST::GET_WINDOW_SIZE_REQ        , {     std::string("/session/%s/window/%s/size"),
                                                                std::string("") } },
    { WINAPP_DRIVER_REQUEST::GET_ELEMENTS_REQ           , {     std::string("/session/%s/elements"),
                                                                std::string("{\"using\": \"%s\", \"value\": \"%s\"}") } },
//                                                                std::string("{\"using\": \"xpath\", \"value\": \"./*\"}") } },
    { WINAPP_DRIVER_REQUEST::GET_ELEMENT_CHILDREN_REQ   , {     std::string("/session/%s/element/%s/elements"),
                                                                std::string("{\"using\": \"%s\", "
                                                                             "\"value\": \"%s\", "
                                                                             "\"id\": \"%s\" }") } },
    { WINAPP_DRIVER_REQUEST::GET_SOURCE_REQ             , {     std::string("/session/%s/source"),
                                                                std::string("") } },
};

class WinAppDriverClient
{
public:
    WinAppDriverClient(const std::wstring& Url) : serverUrl(Url) {
        session = "";
    }

    ~WinAppDriverClient() {
        // Clean up resources
    }

    int LaunchAUT(string autPath);
    string GetWindowHandle(/*string sessionId */);
    Point GetWindowPosition(/*string sessionId, */string winHwnd );
    WinSize GetWindowSize(/*string sessionId, */string winHwnd );
    vector<string> GetWindowElements(/*string sessionId, */string ElementId, string locatorType, string locatorValue);
    string GetSource(/*string sessionId */);

private:
    std::wstring serverUrl;
    std::string session;
};

#endif // WINAPPDRIVERCLIENT_H
