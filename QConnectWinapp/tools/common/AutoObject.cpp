/*
 * AutoObject.cpp
 *
 *  Created on: Jul 24, 2023
 *      Author: cuongnht
 */

#include "AutoObject.h"
#include "constants.h"
#include <fstream>

extern void logInfor(const char* format, ...);

std::unordered_map<std::string, AutoObject*> AutoObject::autoObjectMap;

///
/// \brief AutoObject::AutoObject
/// \param x
/// \param y
/// \param w
/// \param h
/// \param sLabel
/// \param sValue
/// \param winId
/// \param parent
///
AutoObject::AutoObject(int x, int y, int w, int h, string sLabel, string sValue, int winId, string objName, string accName, string className, AutoObject* parent)
{
    this->w = w;
    this->h = h;
    this->x = x;
    this->y = y;
    this->label = sLabel;
    this->windowId = winId;
    this->mParent = parent;
    this->boder_width = 0;
    this->titlebar_height = 0;
    this->sValue = sValue;
    this->objName = objName;
    this->accName = accName;
    this->className = className;
}

///
/// \brief Constructor of Autoobject.
/// \param initialPropsMap - The map contains properties value of the initialized AutoObject.
/// \param parent - The parent of creating AutoObject.
///
AutoObject::AutoObject(std::map<string, string> initialPropsMap, AutoObject* parent)
{
    for (auto it = initialPropsMap.begin(); it != initialPropsMap.end(); it++)
    {
        auto setPropFunc = PROPERTIES_MAP.at(it->first).second;
        if (setPropFunc)
        {
            setPropFunc(*this, it->second);
        }
    }
    this->mParent = parent;
}

///
/// \brief Serialize an AutoObject to a json string.
/// \param root - The AutoObject need to be serialized.
/// \return String in json format.
///
std::string AutoObject::serializeTree(AutoObject* root) {
    Json::Value jsonRoot;
    for (auto it = PROPERTIES_MAP.begin(); it != PROPERTIES_MAP.end(); it++)
    {
        auto getPropFunc = it->second.first;
        if(getPropFunc)
        {
            jsonRoot[it->first] =  getPropFunc(*root);
        }
    }

    Json::Value jsonChildren(Json::arrayValue);
    for (const auto& child : root->vChildrenObj) {
        jsonChildren.append(serializeTree(child));
    }

    jsonRoot["children"] = jsonChildren;
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "    ";
    std::string jsonString = Json::writeString(builder, jsonRoot);
    return jsonString;
}

///
/// \brief Deserialize a string to an AutoObject
/// \param jsonString - string in json format to be deserialized.
/// \param parent - Parent of the AutoObject.
/// \param createInstanceFunc - Function used to create an AutoObject instance.
/// \return An AutoObject instance.
///
AutoObject* AutoObject::deserializeTree(const std::string& jsonString, AutoObject* parent, CreateInstanceByMapFunc createInstanceFunc) {
    Json::CharReaderBuilder builder;
    Json::Value jsonRoot;
    JSONCPP_STRING err;
    std::istringstream iss(jsonString);

    if (!Json::parseFromStream(builder, iss, &jsonRoot, &err)) {
        std::cerr << "Error parsing JSON: " << err << std::endl;
        return nullptr;
    }

    if (!jsonRoot.isObject()) {
        std::cerr << "Invalid JSON format: Root should be an object." << std::endl;
        return nullptr;
    }

    map<string, string> initialPropsMap;
    for (auto it = PROPERTIES_MAP.begin(); it != PROPERTIES_MAP.end(); it++)
    {
        if(jsonRoot.isMember(it->first))
        {
            initialPropsMap[it->first] = jsonRoot[it->first].asString();
        }
    }

    AutoObject* root;
    if (createInstanceFunc)
    {
        root = createInstanceFunc(initialPropsMap, parent);
    }
    else
    {
        root = createInstanceByMap(initialPropsMap, parent);
    }

    const Json::Value& jsonChildren = jsonRoot["children"];
    for (const auto& jsonChild : jsonChildren) {
        AutoObject* child = deserializeTree(jsonChild.asString(), root, createInstanceFunc);
        if (child) {
            root->vChildrenObj.push_back(child);
        }
    }

    return root;
}

///
/// \brief AutoObject::isMatchDefinition
/// \param def
/// \return
///
bool AutoObject::isMatchedDefinition(string def)
{
    bool bRes = false;
    Json::CharReaderBuilder builder;
    Json::Value jsonRoot;
    JSONCPP_STRING err;
    std::istringstream iss(def);

    if (!Json::parseFromStream(builder, iss, &jsonRoot, &err)) {
        std::cerr << "Error parsing JSON: " << err << std::endl;
        return false;
    }

    if (!jsonRoot.isObject()) {
        std::cerr << "Invalid JSON format: Root should be an object." << std::endl;
        return false;
    }

    bRes = true;
    for (const auto& key : jsonRoot.getMemberNames()) {
        const Json::Value& value = jsonRoot[key];
        if (PROPERTIES_MAP.at(key).first(*this) != value.asString())
        {
            bRes = false;
            break;
        }
    }

    return bRes;
}

///
/// \brief Find the smallest AutoObject which contains the inout point.
/// \param x - Left position of to be checked point.
/// \param y - Top position of to be checked point.
/// \return The smallest AutoObject which contains the inout point.
///
AutoObject* AutoObject::FindSmallestChildContainingPoint(int x, int y)
{
    AutoObject* res = this;
    QRect rect;
    rect.setLeft(this->x);
    rect.setTop(this->y);
    rect.setWidth(this->w);
    rect.setHeight(this->h);
    QPoint point(x, y);
    if (rect.contains(point)) {
        // If the point is within the current rect, check children
        AutoObject* smallestChild = nullptr;
        int smallestArea = INT_MAX;

        for (AutoObject* child : vChildrenObj) {
            AutoObject* foundChild = child->FindSmallestChildContainingPoint(x, y);
            if (foundChild) {
                int area = (foundChild->x + foundChild->w - foundChild->x) *
                           (foundChild->y + foundChild->h - foundChild->y);
                if (area < smallestArea) {
                    smallestChild = foundChild;
                    smallestArea = area;
                }
            }
        }

        if (smallestChild) {
            return smallestChild;
        } else {
            return res; // No smaller child found, return current
        }
    }

    return nullptr;
}

///
/// \brief Create a new instance of AutoObject based on input properties.
/// \param x - AutoObject left position.
/// \param y - AutoObject top position.
/// \param w - AutoObject width.
/// \param h - AutoObject height.
/// \param sLabel - AutoObject label.
/// \param sValue - AutoObject value.
/// \param winId - AutoObject winId.
/// \param parent - AutoObject parent.
/// \return A new instance of AutoObject.
///
AutoObject* AutoObject::createInstance(int x, int y, int w, int h, string sLabel, string sValue, int winId, string objName, string accName, string className, AutoObject* parent)
{
    return new AutoObject(  x,
                            y,
                            w,
                            h,
                            sLabel,
                            sValue,
                            winId,
                            objName,
                            accName,
                            className,
                            parent);

}

///
/// \brief AutoObject::createInstanceByMap
/// \param initialPropsMap
/// \param parent
/// \return
///
AutoObject* AutoObject::createInstanceByMap(std::map<string, string> initialPropsMap, AutoObject *parent)
{
    return  new AutoObject(initialPropsMap, parent);
}

///
/// \brief AutoObject::~AutoObject
///
AutoObject::~AutoObject() {
	// TODO Auto-generated destructor stub
//    logInfor("AutoObject destructor end...");
    for (AutoObject* child : vChildrenObj)
	{
		delete child;
    }
}



