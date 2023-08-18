/*
 * main.cpp
 *
 *  Created on: Jul 24, 2023
 *      Author: cuongnht
 */
#include "automationinspector.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AutomationInspector w;
    w.show();
    return a.exec();
}
