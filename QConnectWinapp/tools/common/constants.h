/*
 * constants.h
 *
 *  Created on: Jul 24, 2023
 *      Author: cuongnht
 */
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>
#include <utility>
#include <map>
#include <functional>
#include "AutoObject.h"

using namespace std;

/* PROPERTIES NAME */
const string PROP_LEFT      = "Left";
const string PROP_TOP       = "Top";
const string PROP_WIDTH     = "Width";
const string PROP_HEIGHT    = "Height";
const string PROP_TEXT      = "Text";
const string PROP_VALUE     = "Value";
const string PROP_WINDOWID  = "Window Id";
const string PROP_OBJNAME   = "Object Name";
const string PROP_ACCNAME   = "AutomationId";
const string PROP_CLASSNAME = "Class Name";
const std::map<std::string, std::pair <std::function<string(const AutoObject&)>, std::function<void(AutoObject&, string)>>> PROPERTIES_MAP =
{
    {PROP_LEFT      ,   {&AutoObject::strX, &AutoObject::setX}},
    {PROP_TOP       ,   {&AutoObject::strY, &AutoObject::setY}},
    {PROP_WIDTH     ,   {&AutoObject::strWidth, &AutoObject::setWidth}},
    {PROP_HEIGHT    ,   {&AutoObject::strHeight, &AutoObject::setHeight}},
    {PROP_TEXT      ,   {&AutoObject::Label, &AutoObject::setLabel}},
    {PROP_VALUE     ,   {&AutoObject::Value, &AutoObject::setValue}},
    {PROP_WINDOWID  ,   {&AutoObject::strWindowId, &AutoObject::setWindowId}},
    {PROP_OBJNAME   ,   {&AutoObject::ObjName, &AutoObject::setObjName}},
    {PROP_ACCNAME   ,   {&AutoObject::AccName, &AutoObject::setAccName}},
    {PROP_CLASSNAME ,   {&AutoObject::ClassName, &AutoObject::setClassName}},
};

#define INTERVAL_TIME   500     // miliseconds

#endif // CONSTANTS_H
