/*
 * adddefinitiondialog.cpp
 *
 *  Created on: Jul 24, 2023
 *      Author: cuongnht
 */
#include "adddefinitiondialog.h"
#include "ui_adddefinitiondialog.h"

///
/// \brief AddDefinitionDialog::AddDefinitionDialog
/// \param parent
/// \param sDefinition
///
AddDefinitionDialog::AddDefinitionDialog(QWidget *parent, string sDefinition) :
    QDialog(parent),
    ui(new Ui::AddDefinitionDialog)
{
    ui->setupUi(this);
    ui->txtDefinition->setText(QString::fromStdString(sDefinition));
    // Connect the buttons to accept or reject the dialog
    QObject::connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    QObject::connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

///
/// \brief AddDefinitionDialog::~AddDefinitionDialog
///
AddDefinitionDialog::~AddDefinitionDialog()
{
    delete ui;
}

///
/// \brief AddDefinitionDialog::getName
/// \return
///
string AddDefinitionDialog::getName()
{
    return ui->txtName->text().toStdString();
}

///
/// \brief AddDefinitionDialog::getDefinition
/// \return
///
string AddDefinitionDialog::getDefinition()
{
    return ui->txtDefinition->text().toStdString();
}
