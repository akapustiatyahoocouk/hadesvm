//
//  hadesvm-cereon/ResidentRamBlockEditor.cpp
//
//  hadesvm::cereon::ResidentRamBlockEditor class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;
#include "ui_ResidentRamBlockEditor.h"

//////////
//  Construction/destruction
ResidentRamBlockEditor::ResidentRamBlockEditor(QWidget * parent, ResidentRamBlock * residentRamBlock)
    :   ComponentEditor(parent),
        //  Implementation
        _residentRamBlock(residentRamBlock),
        //  Controls & resources
        _ui(new Ui::ResidentRamBlockEditor)
{
    _ui->setupUi(this);

    //  Fill in the "memory size unit" combo box
    _ui->sizeUnitComboBox->addItem("B", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::MemorySize::Unit::B)));
    _ui->sizeUnitComboBox->addItem("KB", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::MemorySize::Unit::KB)));
    _ui->sizeUnitComboBox->addItem("MB", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::MemorySize::Unit::MB)));
    _ui->sizeUnitComboBox->addItem("GB", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::MemorySize::Unit::GB)));
    _ui->sizeUnitComboBox->setCurrentIndex(2);  //  MB
}

ResidentRamBlockEditor::~ResidentRamBlockEditor()
{
    delete _ui;
}

//////////
//  hadesvm::core::ComponentEditor
void ResidentRamBlockEditor::loadComponentConfiguration()
{
    _ui->startAddressLineEdit->setText(hadesvm::util::toString(_residentRamBlock->startAddress(), "%016X"));
    _ui->sizeNumberOfUnitsLineEdit->setText(hadesvm::util::toString(_residentRamBlock->size().numberOfUnits()));
    _setSelectedMemorySizeUnit(_residentRamBlock->size().unit());
}

bool ResidentRamBlockEditor::canSaveComponentConfiguration() const
{
    uint64_t startAddress = 0;
    uint64_t sizeNumberOfUnits = 0;

    return hadesvm::util::fromEntireString(_ui->startAddressLineEdit->text(), "%X", startAddress) &&
           startAddress % 8 == 0 &&
           hadesvm::util::fromEntireString(_ui->sizeNumberOfUnitsLineEdit->text(), sizeNumberOfUnits) &&
           sizeNumberOfUnits > 0 &&
           _ui->sizeUnitComboBox->currentIndex() != -1 &&
           static_cast<uint64_t>(_selectedMemorySizeUnit()) * sizeNumberOfUnits / sizeNumberOfUnits == static_cast<uint64_t>(_selectedMemorySizeUnit()) &&
           (static_cast<uint64_t>(_selectedMemorySizeUnit()) * sizeNumberOfUnits) % 4096 == 0;
}

void ResidentRamBlockEditor::saveComponentConfiguration()
{
    uint64_t startAddress = 0;
    uint64_t sizeNumberOfUnits = 0;

    if (hadesvm::util::fromEntireString(_ui->startAddressLineEdit->text(), "%X", startAddress) &&
        hadesvm::util::fromEntireString(_ui->sizeNumberOfUnitsLineEdit->text(), sizeNumberOfUnits) &&
        sizeNumberOfUnits > 0 &&
        _ui->sizeUnitComboBox->currentIndex() != -1)
    {
        _residentRamBlock->setStartAddress(startAddress);
        _residentRamBlock->setSize(hadesvm::core::MemorySize(sizeNumberOfUnits, _selectedMemorySizeUnit()));
    }
}

//////////
//  Implementation helpers
hadesvm::core::MemorySize::Unit ResidentRamBlockEditor::_selectedMemorySizeUnit() const
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

void ResidentRamBlockEditor::_setSelectedMemorySizeUnit(hadesvm::core::MemorySize::Unit unit)
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
void ResidentRamBlockEditor::_onStartAddressLineEditTextChanged(QString)
{
    emit contentChanged();
}

void ResidentRamBlockEditor::_onSizeNumberOfUnitsLineEditTextChanged(QString)
{
    emit contentChanged();
}

void ResidentRamBlockEditor::_onSizeUnitComboBoxCurrentIndexChanged(int)
{
    emit contentChanged();
}

//  End of hadesvm-cereon/ResidentRamBlockEditor.cpp
