//
//  hadesvm-gui/VirtualApplianceWindow.cpp
//
//  hadesvm::gui::VirtualApplianceWindow class implementation
//
//////////
#include "hadesvm-gui/API.hpp"
using namespace hadesvm::gui;
#include "ui_VirtualApplianceWindow.h"

//////////
//  Construction/destruction
VirtualApplianceWindow::VirtualApplianceWindow(hadesvm::core::VirtualAppliance * virtualAppliance)
    :   QMainWindow(nullptr),
        //  Implementation
        _virtualAppliance(virtualAppliance),
        //  Controls & resources
        _ui(new Ui::VirtualApplianceWindow)
{
    Q_ASSERT(_virtualAppliance != nullptr);

    _ui->setupUi(this);

    this->setWindowTitle(_virtualAppliance->name());
}

VirtualApplianceWindow::~VirtualApplianceWindow()
{
    delete _ui;
}

//////////
//  QWidget
void VirtualApplianceWindow::closeEvent(QCloseEvent * event)
{
    //  TODO ask if the user wants to suspend a suspendable VA or
    //  just confirm whether to stop it
    _virtualAppliance->stop();
    event->ignore();    //  MainWindow will destroy this VA window when VA is no longer Running
}

//  End of hadesvm-gui/VirtualApplianceWindow.cpp
