//
//  hadesvm-kernel/MountedFolderDialog.cpp
//
//  hadesvm::kernel::MountedFolderDialog class implementation
//
//////////
#include "hadesvm-kernel/API.hpp"
using namespace hadesvm::kernel;
#include "ui_MountedFolderDialog.h"

//////////
//  Construction/destruction
MountedFolderDialog::MountedFolderDialog(QWidget * parent, const QString & volumeName, const QString & path)
    :   QDialog(parent),
        _ui(new Ui::MountedFolderDialog)
{
    _ui->setupUi(this);

    this->setWindowTitle((volumeName.length() == 0) ? "Add mounted volume" : "Modify mounted volume");
    _ui->volumeNameLineEdit->setText(volumeName);
    _ui->pathLineEdit->setText(path);

    _refresh();
}

MountedFolderDialog::~MountedFolderDialog()
{
    delete _ui;
}

//////////
//  Implementation helpers
void MountedFolderDialog::_refresh()
{
    _ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(
        Kernel::isValidNodeName(_ui->volumeNameLineEdit->text()) &&
        _ui->pathLineEdit->text().length() > 0 &&
        _ui->pathLineEdit->text().length() == _ui->pathLineEdit->text().trimmed().length());
}

//////////
//  Signal handlers
void MountedFolderDialog::_onLineEditTextChanged(QString)
{
    _refresh();
}

void MountedFolderDialog::_onBrowsePushButtonClicked()
{
    QString path = QFileDialog::getExistingDirectory(this, "Select mounted path", _ui->pathLineEdit->text());
    if (!path.isEmpty())
    {
        _ui->pathLineEdit->setText(path);
        _refresh();
    }
}

void MountedFolderDialog::_onOk()
{
    _volumeName = _ui->volumeNameLineEdit->text();
    _path = _ui->pathLineEdit->text();
    accept();
}

void MountedFolderDialog::_onCancel()
{
    reject();
}

//  End of hadesvm-kernel/MountedFolderDialog.cpp
