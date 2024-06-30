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
        _ui(new Ui::VirtualApplianceWindow),
        _refreshTimer(this)
{
    Q_ASSERT(_virtualAppliance != nullptr);

    _ui->setupUi(this);

    _ui->toolBar->addAction(_ui->actionStopVm);
    _ui->toolBar->addSeparator();
    _ui->toolBar->addAction(_ui->actionSuspendVm);
    _ui->toolBar->addSeparator();
    _ui->toolBar->addAction(_ui->actionResetVm);
    _ui->toolBar->addSeparator();
    _ui->toolBar->addAction(_ui->actionFullScreen);

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

    //  Create tabs for all display wodgets in the order of their display names
    QList<hadesvm::core::DisplayWidget*> displayWidgets;
    for (auto ui : _componentUis.values())
    {
        for (auto displayWidget : ui->displayWidgets())
        {
            displayWidgets.append(displayWidget);
        }
    }
    std::sort(displayWidgets.begin(),
              displayWidgets.end(),
              [](auto a, auto b) { return a->displayName() < b->displayName(); });
    for (auto displayWidget : displayWidgets)
    {
        int tabIndex = _ui->tabWidget->addTab(displayWidget, displayWidget->displayName());
        _displayWidgetsByTabIndex.insert(tabIndex, displayWidget);
    }

    //  Set up tab bar event handling
    _ui->tabWidget->tabBar()->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(_ui->tabWidget->tabBar(), &QTabBar::customContextMenuRequested,
            this, &VirtualApplianceWindow::_onCustomContextMenuRequested);

    //  Start refreshes
    _refresh();
    _refreshTimer.setInterval(1000);  //  1sec
    connect(&_refreshTimer, &QTimer::timeout, this, &VirtualApplianceWindow::_onRefreshTimerTick);
    _refreshTimer.start();
}

VirtualApplianceWindow::~VirtualApplianceWindow()
{
    _refreshTimer.stop();

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
    _ui->actionStopVm->setEnabled(_virtualAppliance->state() == hadesvm::core::VirtualAppliance::State::Running);
    _ui->actionSuspendVm->setEnabled(_virtualAppliance->state() == hadesvm::core::VirtualAppliance::State::Running);
    _ui->actionResetVm->setEnabled(_virtualAppliance->state() == hadesvm::core::VirtualAppliance::State::Running);
    _ui->actionFullScreen->setEnabled(_virtualAppliance->state() == hadesvm::core::VirtualAppliance::State::Running &&
                                      _displayWidgetsByTabIndex.contains(_ui->tabWidget->currentIndex()));

    //  Statistics on the Home tab
    if (_virtualAppliance->state() == hadesvm::core::VirtualAppliance::State::Running)
    {   //  std::sort segfaults when VA is not Running!
        QMap<hadesvm::core::IClockedComponent*,
             hadesvm::core::ClockFrequency> achievedClockFrequencyByClockedComponent;
        _virtualAppliance->getRuntimeStatistics(achievedClockFrequencyByClockedComponent);
        QList<hadesvm::core::IClockedComponent*> clockedComponents = achievedClockFrequencyByClockedComponent.keys();
        std::sort(clockedComponents.begin(),
                  clockedComponents.end(),
                  [](auto a, auto b) { return a->displayName() < b->displayName(); });
        //  Refresh the list of VAs - make sure it has a proper number...
        while (clockedComponents.count() < _ui->statisticsListWidget->count())
        {   //  Too many items in the list
            delete _ui->statisticsListWidget->takeItem(_ui->statisticsListWidget->count() - 1);
        }
        while (clockedComponents.count() > _ui->statisticsListWidget->count())
        {   //  Too few items in the list
            new QListWidgetItem("TODO", _ui->statisticsListWidget);
        }
        //  ...of proper items
        for (int i = 0; i < clockedComponents.count(); i++)
        {
            QString text = clockedComponents[i]->displayName() +
                           " is running at " +
                           achievedClockFrequencyByClockedComponent[clockedComponents[i]].displayForm();
            _ui->statisticsListWidget->item(i)->setText(text);
        }

        //  VA's clock frequency is the max clock frequency of its clocked components
        hadesvm::core::ClockFrequency vaClockFrequency;
        for (auto cf : achievedClockFrequencyByClockedComponent.values())
        {
            vaClockFrequency = qMax(vaClockFrequency, cf);
        }
        QString vaSpeedText = _virtualAppliance->name() +
                              " is running";
        if (vaClockFrequency.toHz() > 0)
        {
            vaSpeedText += " at " + vaClockFrequency.displayForm();
        }
        _ui->statusbar->showMessage(vaSpeedText);
    }
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

void VirtualApplianceWindow::_onResetVm()
{
    _virtualAppliance->reset();
    _refresh();
}

void VirtualApplianceWindow::_onFullScreen()
{
    FullScreenWindow * fsw = new FullScreenWindow();
    fsw->setVisible(true);
}

void VirtualApplianceWindow::_onRefreshTimerTick()
{
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

void VirtualApplianceWindow::_onCurrentTabChanged()
{
    if (_displayWidgetsByTabIndex.contains(_ui->tabWidget->currentIndex()))
    {   //  When switching to a tab that hosts a DisplayWidget, focus that DisplayWidget
        _displayWidgetsByTabIndex[_ui->tabWidget->currentIndex()]->setFocus();
    }
    _refresh();
}

//  End of hadesvm-gui/VirtualApplianceWindow.cpp
