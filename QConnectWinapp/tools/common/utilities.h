#ifndef UTILITIES_H
#define UTILITIES_H

#ifdef BUILD_ON_LINUX
#include <X11/Xlib.h>
#else
#include <windows.h>
#endif
#include <iostream>
#include <fstream>
#include <cerrno>
#include <random>
#include <ctime>
#include <algorithm>
#include <filesystem>
#include <sstream>

#define DELETE_POINTER(p)  if(p != NULL) { delete p; p = NULL; }

class Utilities
{
public:
    Utilities();
#ifdef BUILD_ON_LINUX
    static int GetPosWindow(Window windowId, int&x, int&y, unsigned int &h, unsigned int &w, int &border_width, int& titlebar_height);

    static int drawHighlight(int x,
                             int y,
                             unsigned int rectWidth,
                             unsigned int rectHeight,
                             int borderWidth,
                             unsigned long borderColor,
                             int function);
#else
    static int HighlightWindow( HWND hwnd, RECT rc);
    static void ClearRectangles(HWND hwnd);
#endif
//    void logInfor(const char* format, ...) {
//        char buffer[1024];
//        va_list args;
//        va_start(args, format);
//        vsnprintf(buffer, sizeof(buffer), format, args);
//        va_end(args);
//        std::ofstream outfile;
//        outfile.open(LOG_FILE, std::ios::out | std::ios::app); // Open in append mode
//        if (outfile.is_open()) {
//            outfile << buffer << std::endl;
//            outfile.close();
//        }
//    }
    static bool isFilePath(const std::string& path) {
    #ifdef _WIN32
        struct _stat info;
        return _stat(path.c_str(), &info) == 0 && (info.st_mode & S_IFREG);
    #else
        struct stat info;
        return stat(path.c_str(), &info) == 0 && S_ISREG(info.st_mode);
    #endif
    }

    static std::string readFileContent(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filePath << std::endl;
            return "";
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    static std::string demangleTypeName(const std::string& mangledName);
    static std::string generateUUID() {
            static std::default_random_engine randomEngine(static_cast<unsigned>(time(nullptr)));
            static std::uniform_int_distribution<int> distribution(0, 15);
            static const char* hexChars = "0123456789ABCDEF";

            std::string uuid;
            for (int i = 0; i < 32; ++i) {
                uuid += hexChars[distribution(randomEngine)];
            }
            return uuid;
        }
};

#endif // UTILITIES_H
