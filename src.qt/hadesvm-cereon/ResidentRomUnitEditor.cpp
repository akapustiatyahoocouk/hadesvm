//
//  hadesvm-cereon/ResidentRomUnitEditor.cpp
//
//  hadesvm::cereon::ResidentRomUnitEditor class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;
#include "ui_ResidentRomUnitEditor.h"

//////////
//  Construction/destruction
ResidentRomUnitEditor::ResidentRomUnitEditor(QWidget * parent, ResidentRomUnit * residentRomUnit)
    :   ComponentEditor(parent),
        //  Implementation
        _residentRomUnit(residentRomUnit),
        //  Controls & resources
        _ui(new Ui::ResidentRomUnitEditor)
{
    _ui->setupUi(this);

    //  Fill in the "memory size unit" combo box
    _ui->sizeUnitComboBox->addItem("B", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::MemorySize::Unit::B)));
    _ui->sizeUnitComboBox->addItem("KB", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::MemorySize::Unit::KB)));
    _ui->sizeUnitComboBox->addItem("MB", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::MemorySize::Unit::MB)));
    _ui->sizeUnitComboBox->addItem("GB", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::MemorySize::Unit::GB)));
    _ui->sizeUnitComboBox->setCurrentIndex(2);  //  MB
}

ResidentRomUnitEditor::~ResidentRomUnitEditor()
{
    delete _ui;
}

//////////
//  hadesvm::core::ComponentEditor
void ResidentRomUnitEditor::loadComponentConfiguration()
{
    _ui->startAddressLineEdit->setText(hadesvm::util::toString(_residentRomUnit->startAddress(), "%016X"));
    _ui->sizeNumberOfUnitsLineEdit->setText(hadesvm::util::toString(_residentRomUnit->size().numberOfUnits()));
    _setSelectedMemorySizeUnit(_residentRomUnit->size().unit());
    _ui->contentLineEdit->setText(_residentRomUnit->contentFilePath());
}

bool ResidentRomUnitEditor::canSaveComponentConfiguration() const
{
    uint64_t startAddress = 0;
    uint64_t sizeNumberOfUnits = 0;

    return hadesvm::util::fromString(_ui->startAddressLineEdit->text(), "%X", startAddress) &&
           startAddress % 8 == 0 &&
           hadesvm::util::fromString(_ui->sizeNumberOfUnitsLineEdit->text(), sizeNumberOfUnits) &&
           sizeNumberOfUnits > 0 &&
           _ui->sizeUnitComboBox->currentIndex() != -1 &&
           static_cast<uint64_t>(_selectedMemorySizeUnit()) * sizeNumberOfUnits / sizeNumberOfUnits == static_cast<uint64_t>(_selectedMemorySizeUnit()) &&
           (static_cast<uint64_t>(_selectedMemorySizeUnit()) * sizeNumberOfUnits) % 4096 == 0 &&
           _ui->contentLineEdit->text().length() > 0 &&
           _ui->contentLineEdit->text().trimmed().length() == _ui->contentLineEdit->text().length();
}

void ResidentRomUnitEditor::saveComponentConfiguration()
{
    uint64_t startAddress = 0;
    uint64_t sizeNumberOfUnits = 0;

    if (hadesvm::util::fromString(_ui->startAddressLineEdit->text(), "%X", startAddress) &&
        hadesvm::util::fromString(_ui->sizeNumberOfUnitsLineEdit->text(), sizeNumberOfUnits) &&
        sizeNumberOfUnits > 0 &&
        _ui->sizeUnitComboBox->currentIndex() != -1)
    {
        _residentRomUnit->setStartAddress(startAddress);
        _residentRomUnit->setSize(hadesvm::core::MemorySize(sizeNumberOfUnits, _selectedMemorySizeUnit()));
        _residentRomUnit->setContentFilePath(_ui->contentLineEdit->text());
    }
}

//////////
//  Implementation helpers
hadesvm::core::MemorySize::Unit ResidentRomUnitEditor::_selectedMemorySizeUnit() const
{
    switch (_ui->sizeUnitComboBox->currentIndex())
    {
        case 0:
            return hadesvm::core::MemorySize::Unit::B;
        case 1:
            return hadesvm::core::MemorySize::Unit::KB;
        case 2:
            return hadesvm::core::MemorySize::Unit::MB;
        case 3:
            return hadesvm::core::MemorySize::Unit::GB;
        default:
            return hadesvm::core::MemorySize::Unit::B;
    }
}

void ResidentRomUnitEditor::_setSelectedMemorySizeUnit(hadesvm::core::MemorySize::Unit unit)
{
    switch (unit)
    {
        case hadesvm::core::MemorySize::Unit::B:
            _ui->sizeUnitComboBox->setCurrentIndex(0);
            break;
        case hadesvm::core::MemorySize::Unit::KB:
            _ui->sizeUnitComboBox->setCurrentIndex(1);
            break;
        case hadesvm::core::MemorySize::Unit::MB:
            _ui->sizeUnitComboBox->setCurrentIndex(2);
            break;
        case hadesvm::core::MemorySize::Unit::GB:
            _ui->sizeUnitComboBox->setCurrentIndex(3);
            break;
        default:
            _ui->sizeUnitComboBox->setCurrentIndex(0);
            break;
    }
}

//////////
//  Signal handlers
void ResidentRomUnitEditor::_onStartAddressLineEditTextChanged(QString)
{
    emit contentChanged();
}

void ResidentRomUnitEditor::_onSizeNumberOfUnitsLineEditTextChanged(QString)
{
    emit contentChanged();
}

void ResidentRomUnitEditor::_onSizeUnitComboBoxCurrentIndexChanged(int)
{
    emit contentChanged();
}

void ResidentRomUnitEditor::_onContentLineEditTextChanged(QString)
{
    emit contentChanged();
}

void ResidentRomUnitEditor::_onBrowsePushButtonClicked()
{
    QString fileName =
        QFileDialog::getOpenFileName(
            this->topLevelWidget(),
            "Select ROM image",
            _residentRomUnit->virtualAppliance()->directory(),
            "ROM image files (*.img *.bhin)");
    if (fileName.length() != 0)
    {
        _ui->contentLineEdit->setText(_residentRomUnit->virtualAppliance()->toRelativePath(fileName));
        emit contentChanged();
    }
}

//  End of hadesvm-cereon/ResidentRomUnitEditor.cpp
