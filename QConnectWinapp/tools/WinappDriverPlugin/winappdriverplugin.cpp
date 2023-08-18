#include "winappdriverplugin.h"

#define DEFAULT_WINAPP_PATH "C:/Program Files (x86)/Windows Application Driver/WinAppDriver.exe"

const string LOG_FILE = "WinappPlugin.log";
void logInfor(const char* format, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    std::ofstream outfile;
    outfile.open(LOG_FILE, std::ios::out | std::ios::app); // Open in append mode
    if (outfile.is_open()) {
        outfile << buffer << std::endl;
        outfile.close();
    }
}

///
/// \brief WinappDriverPlugin::WinappDriverPlugin
///
WinappDriverPlugin::WinappDriverPlugin()
{
//    initializePlugin();
}

WinappDriverPlugin::~WinappDriverPlugin()
{
    uninitializePlugin();
}

///
/// \brief WinappDriverPlugin::launchAUT
/// \param autPath
/// \return
///
int WinappDriverPlugin::launchAUT(const char* autPath)
{
    return winApp->LaunchAUT(string(autPath));
}

///
/// \brief getPluginName
/// \return
///
const char* WinappDriverPlugin::getPluginName() {
    return "Winapp"; // Change this to your plugin name
}


///
/// \brief getPluginPath
/// \return
///
const char* WinappDriverPlugin::getPluginPath() {
//    Dl_info dl_info;
//    if (dladdr((void*)getPluginPath, &dl_info)) {
//        return dl_info.dli_fname;
//    }
    return nullptr;
}

///
/// \brief getAgentPath
/// \param outPath
/// \return
///
int WinappDriverPlugin::getAgentPath(char* outPath)
{
    int iRes = -1;
//    const char *pluginPath = getPluginPath();
//    char *pluginDirPath = dirname(strdup(pluginPath));
//    const char *relativePath = "../Agents/libQtAgent.so";
//    char agentRelativePath[1024];
//    memset(agentRelativePath,0,1024);
//    sprintf(agentRelativePath, "%s/%s", pluginDirPath, relativePath);
//    free(pluginDirPath);

//    if (realpath(agentRelativePath, outPath) != NULL) {
//            printf("Relative Path: %s\n", relativePath);
//            printf("Absolute Path: %s\n", outPath);
//            iRes = 0;
//    } else {
//            perror("realpath");
//    }
    return iRes;
}

bool testConnection(const char* host, int port) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock" << std::endl;
        return false;
    }

    // Create socket
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket" << std::endl;
        WSACleanup();
        return false;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(host);
    serverAddress.sin_port = htons(port);

    // Connect to server
    if (connect(clientSocket, reinterpret_cast<SOCKADDR*>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "Failed to connect" << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return false;
    }

    // Connection successful
    std::cout << "Connected successfully" << std::endl;

    closesocket(clientSocket);
    WSACleanup();
    return true;
}

///
/// \brief initializePlugin
///
int WinappDriverPlugin::initializePlugin()
{
    int iRes = -1;
    std::string address = "127.0.0.1";
    std::string port    = "4723";
    const char* envVarValue = getenv("WINAPPDRIVER_ADDR");
    if (envVarValue != NULL)
    {
        std::string urlString(envVarValue);
        size_t startPos = urlString.find("://");
        if (startPos != std::string::npos) {
            urlString.erase(0, startPos + 3);
        }

        // Find colon separating address and port
        size_t colonPos = urlString.find(':');
        if (colonPos != std::string::npos) {
            std::string address = urlString.substr(0, colonPos);
            std::string port = urlString.substr(colonPos + 1);
        }
        else {
            std::cout << "Invalid URL format" << std::endl;
        }
    }

    if(testConnection(address.c_str(), std::stoi(port)))
    {
        std::wstringstream wss;
        wss << L"http://" << std::wstring(address.begin(), address.end()) << L":" << std::wstring(port.begin(), port.end());
        std::wstring addr = wss.str();
        winApp = new WinAppDriverClient(addr);
    }
    else if (address == "localhost" || address == "127.0.0.1" || address == "0.0.0.0")
    {
        char* envWinappPath = getenv("WINAPPDRIVER_PATH");
        string winappPath = DEFAULT_WINAPP_PATH;
        if (envWinappPath != NULL)
        {
            winappPath = string(envWinappPath);
        }

        char command[1024];
        snprintf(command, sizeof(command), "%s %s %s", winappPath.c_str(), address.c_str(), port.c_str());

        // Start the process
        // CreateProcess variables
        STARTUPINFOA si;
//        PROCESS_INFORMATION pi;

        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        // Start the child process
        if (CreateProcessA(NULL, const_cast<LPSTR>(command), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            std::cout << "Process started successfully." << std::endl;

//            // Close process and thread handles
//            CloseHandle(pi.hProcess);
//            CloseHandle(pi.hThread);

            std::wstringstream wss;
            wss << L"http://" << std::wstring(address.begin(), address.end()) << L":" << std::wstring(port.begin(), port.end());
            std::wstring addr = wss.str();
            winApp = new WinAppDriverClient(addr);
        }
        else
        {
            std::cerr << "Failed to start process. Error code: " << GetLastError() << std::endl;
        }
    }

    return iRes;
}

///
/// \brief uninitializePlugin
///
void WinappDriverPlugin::uninitializePlugin()
{
    if(winApp)
    {
        delete winApp;
        winApp = NULL;
    }

    DWORD exitCode;
    if (GetExitCodeProcess(pi.hProcess, &exitCode) && exitCode == STILL_ACTIVE) {
        if (TerminateProcess(pi.hProcess, 0)) {
            std::cout << "Process terminated successfully." << std::endl;
        } else {
            std::cerr << "Failed to terminate process. Error code: " << GetLastError() << std::endl;
        }
    }

    // Close process and thread handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}


///
/// \brief getMatchedElement
/// \param definitions
/// \param length
/// \param outUUID
///
void WinappDriverPlugin::getMatchedElement(char* definitions, int length, char* outUUID)
{

}

///
/// \brief WinappDriverPlugin::creatAutoObject
/// \param node
/// \param parent
/// \param delta
/// \param winHandle
/// \return
///
AutoObject* WinappDriverPlugin::creatAutoObject(xml_node<>* node, AutoObject* parent, Point delta, int winHandle) {
    int x = std::stoi(node->first_attribute("x")->value()) + delta.x;
    int y = std::stoi(node->first_attribute("y")->value()) + delta.y;
    int w = std::stoi(node->first_attribute("width")->value());
    int h = std::stoi(node->first_attribute("height")->value());
    string label = "";
    string value = "";
    int winId = winHandle;//std::stoi(node->first_attribute("x")->value());
    rapidxml::xml_attribute<>* attrName = node->first_attribute("Name");
    string objName = attrName ? attrName->value() : "";
    string accName = node->first_attribute("AutomationId")->value();
    string className = node->first_attribute("ClassName")->value();
    AutoObject* autoObj = new AutoObject(x, y, w, h, label, value, winId, objName, accName, className, parent);
    for (xml_node<>* child = node->first_node(); child; child = child->next_sibling()) {
        AutoObject* childNode = creatAutoObject(child, autoObj, delta, winHandle);
        autoObj->vChildrenObj.push_back(childNode);
    }

    return autoObj;
}

///
/// \brief createTempFileWithJSON
/// \param jsonString
/// \return
///
std::string createTempFileWithJSON(const std::string& jsonString) {
    FILE* tempFile = nullptr;
    char tempFilePath[L_tmpnam_s];

    if (tmpnam_s(tempFilePath, L_tmpnam_s) != 0) {
        std::cerr << "Failed to generate temporary file path." << std::endl;
        return "";
    }

    if (fopen_s(&tempFile, tempFilePath, "wb") != 0) {
        std::cerr << "Failed to create temporary file." << std::endl;
        return "";
    }

    // Write the JSON string to the file
    if (!jsonString.empty()) {
        fwrite(jsonString.c_str(), 1, jsonString.size(), tempFile);
    }

    fclose(tempFile);
    return tempFilePath;
}

///
/// \brief getWindowInforString
/// \param outInfor
///
void WinappDriverPlugin::getWindowInforString(char* outInfor)
{
    string source = winApp->GetSource();
    /* std::ofstream file("output.txt", std::ios::binary); // Open the file in binary mode
    if (file.is_open()) {
        file.write(source.c_str(), source.size()); // Write the content to the file
        file.close();
        std::cout << "Content written to file." << std::endl;
    } else {
        std::cerr << "Failed to open the file." << std::endl;
    }*/
    rapidxml::xml_document<> doc;
    doc.parse<0>(const_cast<char*>(source.c_str()));
    string winHwnd = winApp->GetWindowHandle();
    Point delta = winApp->GetWindowPosition(winHwnd);
    AutoObject* autoObj = creatAutoObject(doc.first_node("Window"), NULL, delta, std::stoi(winHwnd));
    string strAutoObj = AutoObject::serializeTree(autoObj);
//    memcpy(outInfor, strAutoObj.c_str(), strAutoObj.size());

    string tmpFile = createTempFileWithJSON(strAutoObj);
    memcpy(outInfor, tmpFile.c_str(), tmpFile.size());
//    print_node(doc.first_node("Window"));
}
