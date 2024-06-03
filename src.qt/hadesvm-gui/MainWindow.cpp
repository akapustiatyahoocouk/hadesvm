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
MainWindow::MainWindow(QWidget * parent)
    :   QMainWindow(parent),
        //  Implementation
        _virtualAppliances(),
        _currentVirtualAppliance(nullptr),
        _settings(this),
        //  Controls & resources
        _ui(new Ui::MainWindow)
{
    _ui->setupUi(this);

    //  Restore size & position TODO

    //  Restore VM list and update UI accordingly
    _loadVirtualAppliance();

    //  Done
    _refresh();
}

MainWindow::~MainWindow()
{
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
            _currentVirtualAppliance->state() == hadesvm::core::VirtualAppliance::State::Running &&
            _currentVirtualAppliance->suspendable());
        _ui->actionResumeVm->setEnabled(
            _currentVirtualAppliance != nullptr &&
            _currentVirtualAppliance->state() == hadesvm::core::VirtualAppliance::State::Suspended);
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
    {   //  TODO implement VA creation from template
        Q_ASSERT(false);
    }
    else
    {   //  Create a raw VA
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
        if (_currentVirtualAppliance->suspendable())
        {   //  Try to suspend first...
            try
            {
                _currentVirtualAppliance->suspend();
            }
            catch (...)
            {   //  TODO report exception to the user?
                _currentVirtualAppliance->stop();
            }
        }
        else
        {   //  Stop outright
            _currentVirtualAppliance->stop();
        }
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
            if (virtualAppliance->suspendable())
            {   //  Try to suspend first...
                try
                {
                    virtualAppliance->suspend();
                }
                catch (...)
                {   //  TODO report exception to the user?
                    virtualAppliance->stop();
                }
            }
            else
            {   //  Stop outright
                virtualAppliance->stop();
            }
        }
    }

    //  Proceed with window destruction
    destroy();
}

void MainWindow::_onStartVm()
{
}

void MainWindow::_onStopVm()
{
}

void MainWindow::_onSuspendVm()
{
}

void MainWindow::_onResumeVm()
{
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

//  End of hadesvm-gui/MainWindow.cpp
