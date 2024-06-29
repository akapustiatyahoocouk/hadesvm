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
        _componentUis(),
        _displayWidgetsByTabIndex(),
        //  Controls & resources
        _ui(new Ui::VirtualApplianceWindow)
{
    Q_ASSERT(_virtualAppliance != nullptr);

    _ui->setupUi(this);

    this->setWindowTitle(_virtualAppliance->name());

    //  Create UIs for all VM components/adapters
    for (auto component : _virtualAppliance->components())
    {
        if (auto ui = component->createUi())
        {
            _componentUis.insert(component, ui);
        }
    }
    for (auto componentAdaptor : _virtualAppliance->componentAdaptors())
    {
        if (auto ui = componentAdaptor->createUi())
        {
            _componentUis.insert(componentAdaptor, ui);
        }
    }

    //  Create tabs for all display wodgets
    for (auto ui : _componentUis.values())
    {
        for (auto displayWidget : ui->displayWidgets())
        {
            int tabIndex = _ui->tabWidget->addTab(displayWidget, displayWidget->displayName());
            _displayWidgetsByTabIndex.insert(tabIndex, displayWidget);
        }
    }

    //  Set up tab bar event handling
    _ui->tabWidget->tabBar()->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(_ui->tabWidget->tabBar(), &QTabBar::customContextMenuRequested,
            this, &VirtualApplianceWindow::_onCustomContextMenuRequested);
}

VirtualApplianceWindow::~VirtualApplianceWindow()
{
    while (_ui->tabWidget->count() != 0)
    {
        _ui->tabWidget->removeTab(0);
    }
    for (auto [component, ui] : _componentUis.asKeyValueRange())
    {
        delete ui;
    }

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

void VirtualApplianceWindow::_onCustomContextMenuRequested(const QPoint &point)
{
    if (!point.isNull())
    {
        int tabIndex = _ui->tabWidget->tabBar()->tabAt(point);
        qDebug() << "tabIndex = " << tabIndex;
        if (!_displayWidgetsByTabIndex.contains(tabIndex))
        {   //  No context menu possible
            return;
        }
        //  Context (control) menu is up to the DeviceWidget
        QMenu * controlMenu = _displayWidgetsByTabIndex[tabIndex]->controlMenu();
        if (controlMenu != nullptr)
        {
            if (tabIndex != _ui->tabWidget->currentIndex())
            {   //  Change current tab before displaying the context (control) menu
                _ui->tabWidget->setCurrentIndex(tabIndex);
            }
            controlMenu->exec(_ui->tabWidget->tabBar()->mapToGlobal(point));
        }
    }
}


//  End of hadesvm-gui/VirtualApplianceWindow.cpp
