#include "winappdriverclient.h"


//bool isWinDriverStarted()
//{
//    bool bRes = false;
//    const char* processName = "WinAppDriver.exe";

//    // Check if the process is already running
//    HWND hWnd = FindWindowA(nullptr, processName);
//    if (hWnd != nullptr) {
//        std::cout << "Process is already running." << std::endl;
//        bRes = true;
//    }

//    return bRes;
//}

///
/// \brief WinAppDriverClient::LaunchAUT
/// \param autPath
/// \return
///
int WinAppDriverClient::LaunchAUT(string autPath)
{
    int iRes = -1;
    web::http::client::http_client client(serverUrl);
    web::http::http_request request(web::http::methods::POST);
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring endpoint = converter.from_bytes(WINAPP_DRIVER_MAP.at(WINAPP_DRIVER_REQUEST::LAUNCH_AUT_REQ).first);
    request.set_request_uri(endpoint);

    // Set request headers
    request.headers().set_content_type(L"application/json");

    // Set request body (payload)
    char buffer[2048];
    std::sprintf(buffer, WINAPP_DRIVER_MAP.at(WINAPP_DRIVER_REQUEST::LAUNCH_AUT_REQ).second.c_str(), autPath.c_str(), autPath.c_str());
    string payload = buffer;
    request.set_body(converter.from_bytes(payload));

    // Send the POST request
    web::http::http_response response = client.request(request).get();

    // Check if the response status is OK (200)
    if (response.status_code() == web::http::status_codes::OK) {
        // Extract and display the response payload as JSON
        web::json::value responseBody = response.extract_json().get();
        if (responseBody.has_field(U("sessionId"))) {
                web::json::value sessionId = responseBody[U("sessionId")];
                if (sessionId.is_string()) {
                    utility::string_t ssId = sessionId.as_string();
                    session = utility::conversions::to_utf8string(ssId);
                    iRes = 0;
                }
        }
        std::wcout << L"Response JSON:\n" << responseBody.serialize() << std::endl;
    } else {
        std::cerr << "Request failed with status code: " << response.status_code() << std::endl;
    }
    return iRes;
}

string WinAppDriverClient::GetWindowHandle(/*string sessionId */)
{
    string iRes = "";
    web::http::client::http_client client(serverUrl);
    web::http::http_request request(web::http::methods::GET);
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    char buffer[2048];
    std::sprintf(buffer, WINAPP_DRIVER_MAP.at(WINAPP_DRIVER_REQUEST::GET_WINDOW_HWND_REQ).first.c_str(), session.c_str());
    std::wstring endpoint = converter.from_bytes(string(buffer));
    request.set_request_uri(endpoint);

    // Set request headers
    request.headers().set_content_type(L"application/json");

    // Send the POST request
    web::http::http_response response = client.request(request).get();

    // Check if the response status is OK (200)
    if (response.status_code() == web::http::status_codes::OK) {
        // Extract and display the response payload as JSON
        web::json::value responseBody = response.extract_json().get();
        if (responseBody.has_field(U("value"))) {
                web::json::value windowHandle = responseBody[U("value")];
                if (windowHandle.is_string()) {
                    utility::string_t hwnd = windowHandle.as_string();
                    iRes = utility::conversions::to_utf8string(hwnd);
                }
        }
        std::wcout << L"Response JSON:\n" << responseBody.serialize() << std::endl;
    } else {
        std::cerr << "Request failed with status code: " << response.status_code() << std::endl;
    }
    return iRes;
}

Point WinAppDriverClient::GetWindowPosition(/*string sessionId, */string winHwnd )
{
    Point iRes;
    web::http::client::http_client client(serverUrl);
    web::http::http_request request(web::http::methods::GET);
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    char buffer[2048];
    std::sprintf(buffer, WINAPP_DRIVER_MAP.at(WINAPP_DRIVER_REQUEST::GET_WINDOW_POS_REQ).first.c_str(), session.c_str(), winHwnd.c_str());
    std::wstring endpoint = converter.from_bytes(string(buffer));
    request.set_request_uri(endpoint);

    // Set request headers
    request.headers().set_content_type(L"application/json");

    // Send the POST request
    web::http::http_response response = client.request(request).get();

    // Check if the response status is OK (200)
    if (response.status_code() == web::http::status_codes::OK) {
        // Extract and display the response payload as JSON
        web::json::value responseBody = response.extract_json().get();
        if (responseBody.has_field(U("value"))) {
                web::json::value windowPos = responseBody[U("value")];
                iRes.x = windowPos[U("x")].as_integer();
                iRes.y = windowPos[U("y")].as_integer();
        }
        std::wcout << L"Response JSON:\n" << responseBody.serialize() << std::endl;
    } else {
        std::cerr << "Request failed with status code: " << response.status_code() << std::endl;
    }
    return iRes;
}

WinSize WinAppDriverClient::GetWindowSize(/*string sessionId, */string winHwnd )
{
    WinSize iRes;
    web::http::client::http_client client(serverUrl);
    web::http::http_request request(web::http::methods::GET);
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    char buffer[2048];
    std::sprintf(buffer, WINAPP_DRIVER_MAP.at(WINAPP_DRIVER_REQUEST::GET_WINDOW_SIZE_REQ).first.c_str(), session.c_str(), winHwnd.c_str());
    std::wstring endpoint = converter.from_bytes(string(buffer));
    request.set_request_uri(endpoint);

    // Set request headers
    request.headers().set_content_type(L"application/json");

    // Send the POST request
    web::http::http_response response = client.request(request).get();

    // Check if the response status is OK (200)
    if (response.status_code() == web::http::status_codes::OK) {
        // Extract and display the response payload as JSON
        web::json::value responseBody = response.extract_json().get();
        if (responseBody.has_field(U("value"))) {
                web::json::value windowPos = responseBody[U("value")];
                iRes.w = windowPos[U("width")].as_integer();
                iRes.h = windowPos[U("height")].as_integer();
        }
        std::wcout << L"Response JSON:\n" << responseBody.serialize() << std::endl;
    } else {
        std::cerr << "Request failed with status code: " << response.status_code() << std::endl;
    }
    return iRes;
}

vector<string> WinAppDriverClient::GetWindowElements(/*string sessionId, */string elementId, string locatorType, string locatorValue)
{
    vector<string> vRes;
    web::http::client::http_client client(serverUrl);
    web::http::http_request request(web::http::methods::POST);
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    char buffer[2048];
    if (elementId=="") {
        std::sprintf(buffer, WINAPP_DRIVER_MAP.at(WINAPP_DRIVER_REQUEST::GET_ELEMENTS_REQ).first.c_str(), session.c_str());
    }
    else {
        std::sprintf(buffer, WINAPP_DRIVER_MAP.at(WINAPP_DRIVER_REQUEST::GET_ELEMENT_CHILDREN_REQ).first.c_str(), session.c_str(),
                                                                                                                  elementId.c_str());
    }

    std::wstring endpoint = converter.from_bytes(string(buffer));
    request.set_request_uri(endpoint);

    // Set request headers
    request.headers().set_content_type(L"application/json");

    // Set request body (payload)
    char buffer2[2048];
    if (elementId=="") {
        std::sprintf(buffer2, WINAPP_DRIVER_MAP.at(WINAPP_DRIVER_REQUEST::GET_ELEMENTS_REQ).second.c_str(), locatorType.c_str(),
                                                                                                            locatorValue.c_str());
    }
    else
    {
        std::sprintf(buffer2, WINAPP_DRIVER_MAP.at(WINAPP_DRIVER_REQUEST::GET_ELEMENT_CHILDREN_REQ).second.c_str(), locatorType.c_str(),
                                                                                                                    locatorValue.c_str(),
                                                                                                                    elementId.c_str());
    }

    string payload = buffer2;
    request.set_body(converter.from_bytes(payload));

    // Send the POST request
    web::http::http_response response = client.request(request).get();

    // Check if the response status is OK (200)
    if (response.status_code() == web::http::status_codes::OK) {
        // Extract and display the response payload as JSON
        web::json::value responseBody = response.extract_json().get();
        if (responseBody.has_field(U("value"))) {
            web::json::value resValue = responseBody[U("value")];
            for(web::json::value ele : resValue.as_array())
            {
                vRes.push_back(utility::conversions::to_utf8string(ele[U("ELEMENT")].as_string()));
            }
        }
        std::wcout << L"Response JSON:\n" << responseBody.serialize() << std::endl;
    }
    else {
        std::cerr << "Request failed with status code: " << response.status_code() << std::endl;
    }
    return vRes;
}

string WinAppDriverClient::GetSource(/*string sessionId */)
{
    string sRes = "";
    web::http::client::http_client client(serverUrl);
    web::http::http_request request(web::http::methods::GET);
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    char buffer[2048];
    std::sprintf(buffer, WINAPP_DRIVER_MAP.at(WINAPP_DRIVER_REQUEST::GET_SOURCE_REQ).first.c_str(), session.c_str());
    std::wstring endpoint = converter.from_bytes(string(buffer));
    request.set_request_uri(endpoint);

    // Set request headers
    request.headers().set_content_type(L"application/json");

    // Send the POST request
    web::http::http_response response = client.request(request).get();

    // Check if the response status is OK (200)
    if (response.status_code() == web::http::status_codes::OK) {
        // Extract and display the response payload as JSON
        web::json::value responseBody = response.extract_json().get();
        if (responseBody.has_field(U("value"))) {
                web::json::value windowHandle = responseBody[U("value")];
                if (windowHandle.is_string()) {
                    utility::string_t hwnd = windowHandle.as_string();
                    sRes = utility::conversions::to_utf8string(hwnd);
                }
        }
        std::wcout << L"Response JSON:\n" << responseBody.serialize() << std::endl;
    } else {
        std::cerr << "Request failed with status code: " << response.status_code() << std::endl;
    }
    return sRes;
}

// Add more methods for interacting with elements as needed
