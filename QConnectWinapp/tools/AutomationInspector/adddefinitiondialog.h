/*
 * adddefinitiondialog.h
 *
 *  Created on: Jul 24, 2023
 *      Author: cuongnht
 */
#ifndef ADDDEFINITIONDIALOG_H
#define ADDDEFINITIONDIALOG_H

#include <QDialog>
#include <string>

using namespace std;

namespace Ui {
class AddDefinitionDialog;
}

class AddDefinitionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddDefinitionDialog(QWidget *parent = nullptr, string sDefinition="");
    ~AddDefinitionDialog();
    string getName();
    string getDefinition();

private:
    Ui::AddDefinitionDialog *ui;
};

#endif // ADDDEFINITIONDIALOG_H
