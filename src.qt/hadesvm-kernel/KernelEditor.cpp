//
//  hadesvm-kernel/KernelEditor.cpp
//
//  hadesvm::kernel::KernelEditor class implementation
//
//////////
#include "hadesvm-kernel/API.hpp"
using namespace hadesvm::kernel;
#include "ui_KernelEditor.h"

namespace
{
    const QString NamePathSeparator = " -> ";
    const QString VaDirectoryPrefix = "./";
}

//////////
//  Construction/destruction
KernelEditor::KernelEditor(QWidget * parent, Kernel * kernel)
    :   hadesvm::core::ComponentEditor(parent),
        //  Implementation
        _kernel(kernel),
        //  Controls & resources
        _ui(new Ui::KernelEditor)
{
    Q_ASSERT(_kernel != nullptr);
    _ui->setupUi(this);

    _refresh();
}

KernelEditor::~KernelEditor()
{
    delete _ui;
}

//////////
//  hadesvm::core::ComponentEditor
void KernelEditor::loadComponentConfiguration()
{
    _ui->nodeNameLineEdit->setText(_kernel->nodeName());

    QStringList volumeNames = _kernel->mountedFolderVolumeNames();
    std::sort(volumeNames.begin(),
              volumeNames.end(),
              [](auto a, auto b) -> bool { return a.toLower() < b.toLower(); });
    for (QString volumeName : volumeNames)
    {
        new QListWidgetItem(volumeName + NamePathSeparator + _kernel->mountedFolderPath(volumeName), _ui->volumesListWidget);
    }

    _refresh();
}

bool KernelEditor::canSaveComponentConfiguration() const
{
    return Kernel::isValidNodeName(_ui->nodeNameLineEdit->text());
}

void KernelEditor::saveComponentConfiguration()
{
    _kernel->setNodeName(_ui->nodeNameLineEdit->text());

    //  Mounted folders
    for (auto volumeName : _kernel->mountedFolderVolumeNames())
    {
        _kernel->removeMountedFolder(volumeName);
    }
    for (int i = 0; i < _ui->volumesListWidget->count(); i++)
    {
        QListWidgetItem * item = _ui->volumesListWidget->item(i);
        QStringList volumeNameAndPath = item->text().split(NamePathSeparator);
        _kernel->setMountedFolderPath(volumeNameAndPath[0], volumeNameAndPath[1]);
    }
}

//////////
//  Implementation helpers
void KernelEditor::_refresh()
{
    QListWidgetItem * item = _ui->volumesListWidget->currentItem();

    _ui->modifyVolumePushButton->setEnabled(item != nullptr);
    _ui->deleteVolumePushButton->setEnabled(item != nullptr);
}

void KernelEditor::_addMountedFolder(const QString & volumeName, const QString & path)
{
    QDir baseDir(_kernel->virtualAppliance()->directory());
    QString relativePath = baseDir.relativeFilePath(path);
    if (!relativePath.contains("/") && !relativePath.contains("\\") && relativePath != ".")
    {
        relativePath = VaDirectoryPrefix + relativePath;
    }
    qDebug() << relativePath;

    //  Is there an item with the same volume name already ?
    for (int i = 0; i < _ui->volumesListWidget->count(); i++)
    {
        QListWidgetItem * item = _ui->volumesListWidget->item(i);
        QStringList volumeNameAndPath = item->text().split(NamePathSeparator);
        if (volumeNameAndPath[0] == volumeName)
        {   //  Replace this item
            item->setText(volumeName + NamePathSeparator + relativePath);
            return;
        }
    }
    //  Need to add a new item
    new QListWidgetItem(volumeName + NamePathSeparator + relativePath, _ui->volumesListWidget);
    //  TODO and sort items alphabetically
}

void KernelEditor::_deleteMountedFolder(const QString & volumeName)
{
    for (int i = 0; i < _ui->volumesListWidget->count(); i++)
    {
        QListWidgetItem * item = _ui->volumesListWidget->item(i);
        QStringList volumeNameAndPath = item->text().split(NamePathSeparator);
        if (volumeNameAndPath[0] == volumeName)
        {
            delete _ui->volumesListWidget->takeItem(i);
            return;
        }
    }
}

//////////
//  Signal handlers:
void KernelEditor::_onNodeNameTextChanged(QString)
{
    emit contentChanged();
}

void KernelEditor::_onVolumesListWidgetCurrentRowChanged(int)
{
    _refresh();
}

void KernelEditor::_onAddVolumePushButtonClicked()
{
    MountedFolderDialog dlg(this, "", "");
    if (dlg.exec() == QDialog::DialogCode::Accepted)
    {
        _addMountedFolder(dlg.volumeName(), dlg.path());
        _refresh();
    }
}

void KernelEditor::_onModifyVolumePushButtonClicked()
{
    QListWidgetItem * item = _ui->volumesListWidget->currentItem();
    QStringList volumeNameAndPath = item->text().split(NamePathSeparator);
    QDir baseDir(_kernel->virtualAppliance()->directory());

    QString absolutePath = volumeNameAndPath[1];
    if (absolutePath == ".")
    {
        absolutePath = _kernel->virtualAppliance()->directory();
    }
    else if (absolutePath.startsWith(VaDirectoryPrefix))
    {
        absolutePath = absolutePath.mid(VaDirectoryPrefix.length());
    }
    absolutePath = baseDir.absoluteFilePath(absolutePath);

    MountedFolderDialog dlg(this, volumeNameAndPath[0], absolutePath);
    if (dlg.exec() == QDialog::DialogCode::Accepted)
    {
        if (dlg.volumeName() == volumeNameAndPath[0])
        {   //  Changing a mount path of a volume
            _addMountedFolder(dlg.volumeName(), dlg.path());
        }
        else
        {   //  Renaming the volume
            _deleteMountedFolder(volumeNameAndPath[0]);
            _addMountedFolder(dlg.volumeName(), dlg.path());
        }
        _refresh();
    }
}

void KernelEditor::_onDeleteVolumePushButtonClicked()
{
    QListWidgetItem * item = _ui->volumesListWidget->currentItem();
    QStringList volumeNameAndPath = item->text().split(NamePathSeparator);

    _deleteMountedFolder(volumeNameAndPath[0]);
    _refresh();
}

//  End of hadesvm-kernel/KernelEditor.cpp
