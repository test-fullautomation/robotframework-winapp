/*
 * viewerautoobject.h
 *
 *  Created on: Jul 24, 2023
 *      Author: cuongnht
 */
#ifndef VIEWERAUTOOBJECT_H
#define VIEWERAUTOOBJECT_H
#include <map>
#include "../common/AutoObject.h"
#include "../common/constants.h"

class ViewerAutoObject : public AutoObject
{
public:
    mutable map<string, bool> propertiesSelectedMap =
    {
        {PROP_LEFT      , false},
        {PROP_TOP       , false},
        {PROP_WIDTH     , false},
        {PROP_HEIGHT    , false},
        {PROP_TEXT      , false},
        {PROP_VALUE     , false},
        {PROP_WINDOWID  , false},
        {PROP_OBJNAME   , false},
        {PROP_ACCNAME   , false},
        {PROP_CLASSNAME , false},
    };

    ViewerAutoObject(int x, int y, int w, int h, string sLabel, string sValue, int winId, string objName="", string accName="", string className="", ViewerAutoObject* parent=NULL) :
        AutoObject(x, y, w, h, sLabel, sValue, winId, objName, accName, className, parent) {};

    static ViewerAutoObject* createInstance(int x, int y, int w, int h, string sLabel, string sValue, int winId, string objName="", string accName="", string className="", ViewerAutoObject* parent=NULL);

    ViewerAutoObject(std::map<string, string> initialPropsMap, ViewerAutoObject* parent=NULL) :
        AutoObject(initialPropsMap, parent) {};

    static ViewerAutoObject* createInstanceByMap(std::map<string, string> initialPropsMap, ViewerAutoObject* parent=NULL);
};

#endif // VIEWERAUTOOBJECT_H
