//
//  hadesvm-cereon/Fdc1FloppyDriveEditor.cpp
//
//  hadesvm::cereon::Fdc1FloppyDriveEditor class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;
#include "ui_Fdc1FloppyDriveEditor.h"

//////////
//  Construction/destruction
Fdc1FloppyDriveEditor::Fdc1FloppyDriveEditor(Fdc1FloppyDrive * fdc1FloppyDrive)
    :   hadesvm::core::ComponentEditor(),
        //  Implementation
        _fdc1FloppyDrive(fdc1FloppyDrive),
        //  Controls & resources
        _ui(new Ui::Fdc1FloppyDriveEditor)
{
    _ui->setupUi(this);

    //  Fill in the "channel" combo box
    for (unsigned i = 0; i < 4; i++)
    {
        _ui->channelComboBox->addItem(hadesvm::util::toString(i), QVariant::fromValue(i));
    }
    _ui->channelComboBox->setCurrentIndex(0);
}

Fdc1FloppyDriveEditor::~Fdc1FloppyDriveEditor()
{
    delete _ui;
}

//////////
//  hadesvm::core::ComponentEditor
void Fdc1FloppyDriveEditor::loadComponentConfiguration()
{
    _ui->channelComboBox->setCurrentIndex(_fdc1FloppyDrive->channel());
    _ui->diskImagePathLineEdit->setText(_fdc1FloppyDrive->diskImagePath());
    _ui->mountOnStartupCheckBox->setChecked(_fdc1FloppyDrive->isMounted());
}

bool Fdc1FloppyDriveEditor::canSaveComponentConfiguration() const
{
    return true;
}

void Fdc1FloppyDriveEditor::saveComponentConfiguration()
{
    _fdc1FloppyDrive->setChannel(_ui->channelComboBox->currentIndex());
    _fdc1FloppyDrive->setDiskImagePath(_ui->diskImagePathLineEdit->text());
    _fdc1FloppyDrive->setMounted(_ui->mountOnStartupCheckBox->isChecked());
}

//////////
//  Signal handlers
void Fdc1FloppyDriveEditor::_onDiskImagePathLineEditTextChanged(QString)
{
    emit contentChanged();
}

void Fdc1FloppyDriveEditor::_onBrowsePushButtonClicked()
{
    QString fileName =
        QFileDialog::getOpenFileName(
            this->topLevelWidget(),
            "Select floppy image",
            _fdc1FloppyDrive->virtualAppliance()->directory(),
            "Floppy images (*.flp *.vfd)");
    if (fileName.length() != 0)
    {
        _ui->diskImagePathLineEdit->setText(_fdc1FloppyDrive->virtualAppliance()->toRelativePath(fileName));
        emit contentChanged();
    }
}

//  End of hadesvm-cereon/Fdc1FloppyDriveEditor.cpp
