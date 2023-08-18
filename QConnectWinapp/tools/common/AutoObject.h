/*
 * AutoObject.h
 *
 *  Created on: Jul 24, 2023
 *      Author: cuongnht
 */

#ifndef AUTOOBJECT_H_
#define AUTOOBJECT_H_

#include <vector>
#include <cstring>
#include <string>
#ifdef BUILD_ON_LINUX
#include <jsoncpp/json/json.h>
#else
#include <windows.h>
#include <WinUser.h>
#include <json/json.h>
#endif
#include <QRect>
#include <unordered_map>
#include <sstream>
#include <cerrno>
#include <iostream>

using namespace std;

class AutoObject;
typedef AutoObject* (*CreateInstanceFunc)(int, int, int, int, string, string, int, string, string, string, AutoObject*);
typedef AutoObject* (*CreateInstanceByMapFunc)(std::map<string, string>, AutoObject*);
class AutoObject {
public:
    AutoObject(){};
    AutoObject(int x, int y, int w, int h, string sLabel, string sValue, int winId, string objName="", string accName="", string className="", AutoObject* parent=NULL);
    AutoObject(std::map<string, string> initialPropsMap, AutoObject* parent=NULL);
    virtual ~AutoObject();
    AutoObject* mParent;
    std::vector<AutoObject*> vChildrenObj;

    int X() const { return x; }
    int Y() const { return y; }
    unsigned int Height() const { return h; }
    unsigned int Width() const { return w; }
    unsigned int WindowId() const { return windowId; }
    int BorderWidth() const { return boder_width; }
    int TitleBarHeight() const { return titlebar_height; }

    string ClassName() const { return className; }
    void setClassName(string value) { className = value;}
    string ObjName() const { return objName; }
    void setObjName(string value) { objName = value;}
    string AccName() const { return accName; }
    void setAccName(string value) { accName = value;}
    string Label() const { return label; }
    void setLabel(string value) { label = value;}
    string Value() const { return sValue; }
    void setValue(string value) { sValue = value;}
    string strX() const { return to_string(x); }
    void setX(string value) { x = std::stoi(value); }
    string strY() const { return to_string(y); }
    void setY(string value) { y = std::stoi(value); }
    string strHeight() const { return to_string(h); }
    void setHeight(string value) { h = std::stoi(value);}
    string strWidth() const { return to_string(w); }
    void setWidth(string value) { w = std::stoi(value);}
    string strWindowId() const { return to_string(windowId); }
    void setWindowId(string value) { windowId = std::stoi(value);}
    string UUID() const { return uuid; }
    void setUUID(string value) { uuid = value;}

    bool isMatchedDefinition(string def);
    AutoObject* FindSmallestChildContainingPoint(int x, int y);
    std::vector<AutoObject*> Children() const { return vChildrenObj; }
    static AutoObject* createInstance(int x, int y, int w, int h, string sLabel, string sValue, int winId, string objName="", string accName="", string className="", AutoObject* parent=NULL);
    static AutoObject* createInstanceByMap(std::map<string, string> initialPropsMap, AutoObject* parent=NULL);
    static std::string serializeTree(AutoObject* root);
    static AutoObject* deserializeTree(const std::string& jsonString, AutoObject* parent=NULL, CreateInstanceByMapFunc createInstanceFunc=NULL);

protected:
    int x;
    int y;
    unsigned int h;
    unsigned int w;
    string label;
    string sValue;
    string objName;
    string accName;
    string className;
    int boder_width;
    int titlebar_height;
    unsigned int windowId;
    string uuid;
    static std::unordered_map<std::string, AutoObject*> autoObjectMap;
};

#endif /* AUTOOBJECT_H_ */
