//
//  hadesvm-cereon/Fdc1FloppyDriveStatusBarWidget.cpp
//
//  hadesvm::cereon::Fdc1FloppyDriveStatusBarWidget class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;
#include "ui_Fdc1FloppyDriveStatusBarWidget.h"

//////////
//  Construction/destruction
Fdc1FloppyDriveStatusBarWidget::Fdc1FloppyDriveStatusBarWidget()
    :   hadesvm::core::StatusBarWidget(),
        _ui(new Ui::Fdc1FloppyDriveStatusBarWidget)
{
    _ui->setupUi(this);
}

Fdc1FloppyDriveStatusBarWidget::~Fdc1FloppyDriveStatusBarWidget()
{
    delete _ui;
}

//  End of hadesvm-cereon/Fdc1FloppyDriveStatusBarWidget.cpp
