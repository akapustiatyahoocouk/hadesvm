//
//  hadesvm-gui/MainWindow.cpp
//
//  hadesvm::gui::MainWindow class implementation
//
//////////
#include "hadesvm-gui/API.hpp"
using namespace hadesvm::gui;
#include "ui_MainWindow.h"

//////////
//  Construction/destruction
MainWindow::MainWindow()
    :   QMainWindow(nullptr),
        //  Implementation
        _virtualAppliances(),
        _currentVirtualAppliance(nullptr),
        _virtualApplianceWindows(),
        _settings(this),
        //  Controls & resources
        _ui(new Ui::MainWindow),
        _refreshTimer(this)
{
    _ui->setupUi(this);

    _ui->toolBar->addAction(_ui->actionNewVm);
    _ui->toolBar->addAction(_ui->actionOpenVm);
    _ui->toolBar->addSeparator();
    _ui->toolBar->addAction(_ui->actionStartVm);
    _ui->toolBar->addAction(_ui->actionStopVm);
    _ui->toolBar->addSeparator();
    _ui->toolBar->addAction(_ui->actionSuspendVm);
    _ui->toolBar->addAction(_ui->actionResumeVm);
    _ui->toolBar->addSeparator();
    _ui->toolBar->addAction(_ui->actionResetVm);
    _ui->toolBar->addSeparator();
    _ui->toolBar->addAction(_ui->actionConfigureVm);
    _ui->toolBar->addSeparator();
    _ui->toolBar->addAction(_ui->actionAbout);

    //  Restore size & position TODO

    //  Restore VM list and update UI accordingly
    _loadVirtualAppliance();

    //  Done - but need to connect & start refresh timer
    _refresh();

    _refreshTimer.setInterval(1000);    //  1 sec
    connect(&_refreshTimer, &QTimer::timeout, this, &MainWindow::_onRefreshTimerTick);
    _refreshTimer.start();
}

MainWindow::~MainWindow()
{
    for (auto virtualApplianceWindow : _virtualApplianceWindows)
    {
        delete virtualApplianceWindow;
    }
    _virtualApplianceWindows.clear();

    _refreshTimer.stop();
    delete _ui;
}

//////////
//  QWidget
void MainWindow::moveEvent(QMoveEvent * event)
{
    QMainWindow::moveEvent(event);

    qDebug() << this->geometry();
}

void MainWindow::resizeEvent(QResizeEvent * event)
{
    QMainWindow::resizeEvent(event);

    qDebug() << this->geometry();
}

void MainWindow::closeEvent(QCloseEvent * /*event*/)
{
    _onExit();
}

//////////
//  Implementation helpers
void MainWindow::_refresh()
{
    if (!_refreshUnderway)
    {
        _refreshUnderway = true;

        //  Refresh the list of VAs - make sure it has a proper number...
        while (_virtualAppliances.count() < _ui->listWidget->count())
        {   //  Too many items in the list
            delete _ui->listWidget->takeItem(_ui->listWidget->count() - 1);
        }
        while (_virtualAppliances.count() > _ui->listWidget->count())
        {   //  Too few items in the list
            new QListWidgetItem("TODO", _ui->listWidget);
        }
        //  ...of proper items
        for (int i = 0; i < _virtualAppliances.count(); i++)
        {
            QString text = _virtualAppliances[i]->name() +
                           " [" + _virtualAppliances[i]->architecture()->displayName() + "]";
            switch (_virtualAppliances[i]->state())
            {
                case hadesvm::core::VirtualAppliance::State::Stopped:
                    break;
                case hadesvm::core::VirtualAppliance::State::Running:
                    text += " - Running";
                    break;
                case hadesvm::core::VirtualAppliance::State::Suspended:
                    text += " - Suspended";
                    break;
                default:
                    failure();
            }
            //  TODO also reflect Running/Suspended state in "text"
            _ui->listWidget->item(i)->setText(text);
            _ui->listWidget->item(i)->setData(Qt::ItemDataRole::UserRole, QVariant::fromValue(_virtualAppliances[i]));
        }

        //  ...and a proper list item is selected
        qsizetype currentIndex = _virtualAppliances.indexOf(_currentVirtualAppliance);
        if (currentIndex == -1)
        {   //  No selection
            _currentVirtualAppliance = nullptr;
            _ui->listWidget->clearSelection();
        }
        else
        {   //  Set selection
            _ui->listWidget->setCurrentRow(static_cast<int>(currentIndex));
        }

        //  Menu items
        _ui->actionCloseVm->setEnabled(
            _currentVirtualAppliance != nullptr &&
            _currentVirtualAppliance->state() != hadesvm::core::VirtualAppliance::State::Running);
        _ui->actionStartVm->setEnabled(
            _currentVirtualAppliance != nullptr &&
            _currentVirtualAppliance->state() == hadesvm::core::VirtualAppliance::State::Stopped);
        _ui->actionStopVm->setEnabled(
            _currentVirtualAppliance != nullptr &&
            _currentVirtualAppliance->state() != hadesvm::core::VirtualAppliance::State::Stopped);
        _ui->actionSuspendVm->setEnabled(
            _currentVirtualAppliance != nullptr &&
            _currentVirtualAppliance->state() == hadesvm::core::VirtualAppliance::State::Running);
        _ui->actionResumeVm->setEnabled(
            _currentVirtualAppliance != nullptr &&
            _currentVirtualAppliance->state() == hadesvm::core::VirtualAppliance::State::Suspended);
        _ui->actionResetVm->setEnabled(
            _currentVirtualAppliance != nullptr &&
            _currentVirtualAppliance->state() == hadesvm::core::VirtualAppliance::State::Running);
        _ui->actionConfigureVm->setEnabled(
            _currentVirtualAppliance != nullptr &&
            _currentVirtualAppliance->state() == hadesvm::core::VirtualAppliance::State::Stopped);


        //  Action buttons
        _ui->startVmPushButton->setEnabled(_ui->actionStartVm->isEnabled());
        _ui->stopVmPushButton->setEnabled(_ui->actionStopVm->isEnabled());
        _ui->suspendPushButton->setEnabled(_ui->actionSuspendVm->isEnabled());
        _ui->resumePushButton->setEnabled(_ui->actionResumeVm->isEnabled());
        _ui->configureVmPushButton->setEnabled(_ui->actionConfigureVm->isEnabled());

        _refreshUnderway = false;
    }
}

void MainWindow::_saveVirtualAppliance()
{
    _settings.setValue("MainWindow/VirtualAppliances.Count", _virtualAppliances.count());
    for (qsizetype i = 0; i < _virtualAppliances.count(); i++)
    {
        _settings.setValue("MainWindow/VirtualAppliances." + QString::number(i), _virtualAppliances[i]->location());
    }
}

void MainWindow::_loadVirtualAppliance()
{
    int count = _settings.value("MainWindow/VirtualAppliances.Count").toInt();
    //  TODO kill off qDebug() << count;
    for (int i = 0; i < count; i++)
    {
        QString location = _settings.value("MainWindow/VirtualAppliances." + QString::number(i)).toString();
        //  TODO kill off qDebug() << location;

        //  If the VA with the same location is already loaded, ignore the 2nd time
        bool alreadyLoaded = false;
        for (auto va : _virtualAppliances)
        {
            if (va->location() == location)
            {   //  OOPS! This one!
                alreadyLoaded = true;
                break;
            }
        }
        if (alreadyLoaded)
        {
            continue;
        }

        //  Load the VA
        std::unique_ptr<hadesvm::core::VirtualAppliance> va;
        try
        {
            va.reset(hadesvm::core::VirtualAppliance::load(location));
        }
        catch (const hadesvm::core::VirtualApplianceException & /*ex*/)
        {
            //  TODO log ? QMessageBox::critical(this, "OOPS!", ex.message());
            continue;   //  auto-deletes "va"
        }
        _virtualAppliances.append(va.release());
    }
}

//////////
//  Signal handlers
void MainWindow::_onNewVm()
{
    NewVirtualApplianceDialog dlg(this);
    if (dlg.exec() != QDialog::DialogCode::Accepted)
    {
        return;
    }

    auto architecture = dlg.virtualArchitecture();
    auto name = dlg.virtualApplianceName();
    auto location = dlg.virtualApplianceLocation();
    auto type = dlg.virtualApplianceType();
    auto vaTemplate = dlg.virtualApplianceTemplate();
    //  TODO kill off qDebug() << architecture;
    //  TODO kill off qDebug() << name;
    //  TODO kill off qDebug() << location;
    //  TODO kill off qDebug() << type;
    //  TODO kill off qDebug() << vaTemplate;

    //  If a VA with the same location is already known, report & abort
    for (auto va : _virtualAppliances)
    {
        if (va->location() == location)
        {   //  OOPS! This one!
            QMessageBox::warning(this,
                                 "OOPS!",
                                 "The virtual appliance " + location + " already exists");
            return;
        }
    }

    //  If a file/directory already exists, we can't overwrite it
    if (QFileInfo(location).exists())
    {
        QMessageBox::warning(this,
                             "OOPS!",
                             "The file or directory " + location + " already exists");
        return;
    }

    //  Create a new VA
    hadesvm::core::VirtualAppliance * va = nullptr;
    if (vaTemplate != nullptr)
    {
        QMessageBox::critical(this, "OOPS!", "Not yet implemented");
        return;
    }
    else
    {
        va = type->createVirtualAppliance(name, location, architecture);
    }
    try
    {
        va->save();
    }
    catch (const hadesvm::core::VirtualApplianceException & ex)
    {
        QMessageBox::critical(this, "OOPS!", ex.message());
        delete va;
        return;
    }

    //  Add the newly created VA to the list of known VAs & save that list
    _virtualAppliances.append(va);  //  TODO may throw!
    std::sort(_virtualAppliances.begin(),
              _virtualAppliances.end(),
              [](auto a, auto b) -> bool { return a->name() < b->name(); });
    _currentVirtualAppliance = va;
    _saveVirtualAppliance();
    _refresh();

    //  Start configuring the newly created VA
    //  TODO
}

void MainWindow::_onOpenVm()
{
    QString location = QFileDialog::getOpenFileName(
        this,
        "Open VM",
        QDir::homePath(),
        "VM Files (*" + hadesvm::core::VirtualAppliance::PreferredExtension + ")");
    if (location.isEmpty())
    {
        return;
    }
    //  TODO kill off qDebug() << location;

    //  If a VA with the same location already exists, just make it current
    for (auto va : _virtualAppliances)
    {
        if (va->location() == location)
        {   //  This one!
            _currentVirtualAppliance = va;
            _refresh();
            return;
        }
    }

    //  Load the VA
    std::unique_ptr<hadesvm::core::VirtualAppliance> va;
    try
    {
        va.reset(hadesvm::core::VirtualAppliance::load(location));
    }
    catch (const hadesvm::core::VirtualApplianceException & ex)
    {
        QMessageBox::critical(this, "OOPS!", ex.message());
        return; //  auto-deletes "vaPtr"
    }

    //  Add the newly opened VA to the list of known VAs & save that list
    _virtualAppliances.append(va.get());
    std::sort(_virtualAppliances.begin(),
              _virtualAppliances.end(),
              [](auto a, auto b) -> bool { return a->name() < b->name(); });
    _currentVirtualAppliance = va.release();
    _saveVirtualAppliance();
    _refresh();
}

void MainWindow::_onCloseVm()
{
    if (!_virtualAppliances.contains(_currentVirtualAppliance))
    {   //  Nothing to do
        return;
    }

    //  Confirm close
    if (QMessageBox::question(this,
                              "Close VM",
                              "Really close " + _currentVirtualAppliance->name() + "?") != QMessageBox::StandardButton::Yes)
    {   //  Don't close
        return;
    }

    //  Suspend/stop
    if (_currentVirtualAppliance->state() == hadesvm::core::VirtualAppliance::State::Running)
    {
        //  Try to suspend first...
        try
        {
            _currentVirtualAppliance->suspend();
        }
        catch (...)
        {   //  TODO report exception to the user?
            _currentVirtualAppliance->stop();
        }
    }

    //  Make sure there's no VA window
    if (_virtualApplianceWindows.contains(_currentVirtualAppliance))
    {
        delete _virtualApplianceWindows[_currentVirtualAppliance];
        _virtualApplianceWindows.remove(_currentVirtualAppliance);
    }

    //  Update UI
    _virtualAppliances.removeOne(_currentVirtualAppliance);
    delete _currentVirtualAppliance;
    _currentVirtualAppliance = nullptr;
    _saveVirtualAppliance();
    _refresh();
}

void MainWindow::_onExit()
{
    //  Suspend/stop running VMs
    for (auto virtualAppliance : _virtualAppliances)
    {
        if (virtualAppliance->state() == hadesvm::core::VirtualAppliance::State::Running)
        {
            //  Try to suspend first...
            try
            {
                virtualAppliance->suspend();
            }
            catch (...)
            {   //  TODO report exception to the user?
                virtualAppliance->stop();
            }
        }
    }

    //  Proceed with window destruction
    destroy();
}

void MainWindow::_onStartVm()
{
    if (!_virtualAppliances.contains(_currentVirtualAppliance) ||
        _currentVirtualAppliance->state() != hadesvm::core::VirtualAppliance::State::Stopped)
    {   //  Nothing to do
        return;
    }

    try
    {
        _currentVirtualAppliance->start();
        auto virtualApplianceWindow = new VirtualApplianceWindow(_currentVirtualAppliance);
        _virtualApplianceWindows.insert(_currentVirtualAppliance, virtualApplianceWindow);
        virtualApplianceWindow->setVisible(true);
    }
    catch (const hadesvm::core::VirtualApplianceException & ex)
    {
        QMessageBox::critical(this, "OOPS!", ex.message());
    }
    _refresh();
}

void MainWindow::_onStopVm()
{
    if (!_virtualAppliances.contains(_currentVirtualAppliance) ||
        _currentVirtualAppliance->state() == hadesvm::core::VirtualAppliance::State::Stopped)
    {   //  Nothing to do
        return;
    }

    if (_virtualApplianceWindows.contains(_currentVirtualAppliance))
    {
        delete _virtualApplianceWindows[_currentVirtualAppliance];
        _virtualApplianceWindows.remove(_currentVirtualAppliance);
    }

    _currentVirtualAppliance->stop();
    _refresh();
}

void MainWindow::_onSuspendVm()
{
    if (!_virtualAppliances.contains(_currentVirtualAppliance) ||
        _currentVirtualAppliance->state() != hadesvm::core::VirtualAppliance::State::Running)
    {   //  Nothing to do
        return;
    }

    try
    {
        _currentVirtualAppliance->suspend();
    }
    catch (const hadesvm::core::VirtualApplianceException & ex)
    {
        QMessageBox::critical(this, "OOPS!", ex.message());
    }
    _refresh();
}

void MainWindow::_onResumeVm()
{
    if (!_virtualAppliances.contains(_currentVirtualAppliance) ||
        _currentVirtualAppliance->state() != hadesvm::core::VirtualAppliance::State::Suspended)
    {   //  Nothing to do
        return;
    }

    try
    {
        _currentVirtualAppliance->resume();
    }
    catch (const hadesvm::core::VirtualApplianceException & ex)
    {
        QMessageBox::critical(this, "OOPS!", ex.message());
    }
    _refresh();
}

void MainWindow::_onResetVm()
{
    if (!_virtualAppliances.contains(_currentVirtualAppliance) ||
        _currentVirtualAppliance->state() != hadesvm::core::VirtualAppliance::State::Running)
    {   //  Nothing to do
        return;
    }

    _currentVirtualAppliance->reset();
    _refresh();
}

void MainWindow::_onConfigureVm()
{
    if (!_virtualAppliances.contains(_currentVirtualAppliance) ||
        _currentVirtualAppliance->state() != hadesvm::core::VirtualAppliance::State::Stopped)
    {   //  Nothing to do
        return;
    }

    ConfigureVirtualApplianceDialog dlg(_currentVirtualAppliance, this);
    if (dlg.exec() == QDialog::DialogCode::Accepted)
    {
        try
        {
            _currentVirtualAppliance->save();
        }
        catch (const hadesvm::core::VirtualApplianceException & ex)
        {
            QMessageBox::critical(this, "OOPS!", ex.message());
        }
        _refresh();
    }
}

void MainWindow::_onHelpAbout()
{
    AboutDialog dlg(this);
    dlg.exec();
}

void MainWindow::_onCurrentVmChanged(int)
{
    int index = _ui->listWidget->currentRow();
    _currentVirtualAppliance =
        (index >= 0 && index < _virtualAppliances.count()) ?
            _virtualAppliances[index] :
            nullptr;
    _saveVirtualAppliance();
    _refresh();
}

void MainWindow::_onRefreshTimerTick()
{
    //  Stop any running VAs that want to be stopped
    for (auto virtualAppliance : _virtualAppliances)
    {
        if (virtualAppliance->state() == hadesvm::core::VirtualAppliance::State::Running &&
            virtualAppliance->stopRequested())
        {
            virtualAppliance->stop();
        }
    }
    //  Reset any running VAs that want to be reset
    for (auto virtualAppliance : _virtualAppliances)
    {
        if (virtualAppliance->state() == hadesvm::core::VirtualAppliance::State::Running &&
            virtualAppliance->resetRequested())
        {
            virtualAppliance->reset();
        }
    }
    //  If any VA window represents a non-Running VA, kill that VA window
    for (auto virtualAppliance : _virtualApplianceWindows.keys())
    {
        if (virtualAppliance->state() != hadesvm::core::VirtualAppliance::State::Running)
        {
            delete _virtualApplianceWindows[virtualAppliance];
            _virtualApplianceWindows.remove(virtualAppliance);
            break;
        }
    }

    //  ...and refresh UI
    _refresh();
}

//  End of hadesvm-gui/MainWindow.cpp
