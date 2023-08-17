/*
 * viewerautoobject.cpp
 *
 *  Created on: Jul 24, 2023
 *      Author: cuongnht
 */
#include "viewerautoobject.h"

ViewerAutoObject* ViewerAutoObject::createInstance(int x, int y, int w, int h, string sLabel, string sValue, int winId, string objName, string accName, string className, ViewerAutoObject* parent)
{
    return new ViewerAutoObject(  x,
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

ViewerAutoObject* ViewerAutoObject::createInstanceByMap(std::map<string, string> initialPropsMap, ViewerAutoObject *parent)
{
    return new ViewerAutoObject(initialPropsMap, parent);
}
