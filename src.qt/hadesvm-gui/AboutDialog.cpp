//
//  hadesvm-gui/AboutDialog.cpp
//
//  hadesvm::gui::AboutDialog class implementation
//
//////////
#include "hadesvm-gui/API.hpp"
using namespace hadesvm::gui;
#include "ui_AboutDialog.h"

//////////
//  Construction/destruction
AboutDialog::AboutDialog(QWidget * parent)
    :   QDialog(parent),
        _ui(new Ui::AboutDialog)
{
    _ui->setupUi(this);
}

AboutDialog::~AboutDialog()
{
    delete _ui;
}

//  End of hadesvm-gui/AboutDialog.cpp
