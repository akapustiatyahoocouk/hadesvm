//
//  hadesvm-gui/PreferencesDialog.cpp
//
//  hadesvm::gui::PreferencesDialog class implementation
//
//////////
#include "hadesvm-gui/API.hpp"
using namespace hadesvm::gui;
#include "ui_PreferencesDialog.h"

//////////
//  Construction/destruction
PreferencesDialog::PreferencesDialog(QWidget * parent)
    :   QDialog(parent),
        _ui(new Ui::PreferencesDialog)
{
    _ui->setupUi(this);

    _ui->startMinimizedCheckBox->setChecked(Preferences::startMinimized());
}

PreferencesDialog::~PreferencesDialog()
{
    delete _ui;
}

//////////
//  Signal handlers
void PreferencesDialog::_accept()
{
    Preferences::setStartMinimized(_ui->startMinimizedCheckBox->isChecked());
    accept();
}

//  End of hadesvm-gui/PreferencesDialog.cpp
