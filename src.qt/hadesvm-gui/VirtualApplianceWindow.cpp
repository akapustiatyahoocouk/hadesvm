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
//  Implementatiuon helpers
void VirtualApplianceWindow::_refresh()
{
    _ui->actionStop->setEnabled(_virtualAppliance->state() == hadesvm::core::VirtualAppliance::State::Running);
    _ui->actionSuspend->setEnabled(_virtualAppliance->state() == hadesvm::core::VirtualAppliance::State::Running &&
                                   _virtualAppliance->suspendable());
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

//////////
//  Signal handlers
void VirtualApplianceWindow::_onStopVm()
{
    _virtualAppliance->stop();
    _refresh();
}

void VirtualApplianceWindow::_onSuspendVm()
{
    try
    {
        _virtualAppliance->suspend();
    }
    catch (const hadesvm::core::VirtualApplianceException & ex)
    {
        QMessageBox::critical(this, "OOPS!", ex.message());
    }
    _refresh();
}

//  End of hadesvm-gui/VirtualApplianceWindow.cpp
