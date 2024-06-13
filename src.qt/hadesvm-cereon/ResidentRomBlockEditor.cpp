//
//  hadesvm-cereon/ResidentRomBlockEditor.cpp
//
//  hadesvm::cereon::ResidentRomBlockEditor class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;
#include "ui_ResidentRomBlockEditor.h"

//////////
//  Construction/destruction
ResidentRomBlockEditor::ResidentRomBlockEditor(QWidget * parent, ResidentRomBlock * residentRomBlock)
    :   ComponentEditor(parent),
        //  Implementation
        _residentRomBlock(residentRomBlock),
        //  Controls & resources
        _ui(new Ui::ResidentRomBlockEditor)
{
    _ui->setupUi(this);

    //  Fill in the "memory size unit" combo box
    _ui->sizeUnitComboBox->addItem("B", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::MemorySize::Unit::B)));
    _ui->sizeUnitComboBox->addItem("KB", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::MemorySize::Unit::KB)));
    _ui->sizeUnitComboBox->addItem("MB", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::MemorySize::Unit::MB)));
    _ui->sizeUnitComboBox->addItem("GB", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::MemorySize::Unit::GB)));
    _ui->sizeUnitComboBox->setCurrentIndex(2);  //  MB
}

ResidentRomBlockEditor::~ResidentRomBlockEditor()
{
    delete _ui;
}

//////////
//  hadesvm::core::ComponentEditor
void ResidentRomBlockEditor::loadComponentConfiguration()
{
    _ui->startAddressLineEdit->setText(hadesvm::util::toString(_residentRomBlock->startAddress(), "%016X"));
    _ui->sizeNumberOfUnitsLineEdit->setText(hadesvm::util::toString(_residentRomBlock->size().numberOfUnits()));
    _setSelectedMemorySizeUnit(_residentRomBlock->size().unit());
    _ui->contentLineEdit->setText(_residentRomBlock->contentFilePath());
}

bool ResidentRomBlockEditor::canSaveComponentConfiguration() const
{
    uint64_t startAddress = 0;
    uint64_t sizeNumberOfUnits = 0;

    return hadesvm::util::fromEntireString(_ui->startAddressLineEdit->text(), "%X", startAddress) &&
           startAddress % 8 == 0 &&
           hadesvm::util::fromEntireString(_ui->sizeNumberOfUnitsLineEdit->text(), sizeNumberOfUnits) &&
           sizeNumberOfUnits > 0 &&
           _ui->sizeUnitComboBox->currentIndex() != -1 &&
           static_cast<uint64_t>(_selectedMemorySizeUnit()) * sizeNumberOfUnits / sizeNumberOfUnits == static_cast<uint64_t>(_selectedMemorySizeUnit()) &&
           (static_cast<uint64_t>(_selectedMemorySizeUnit()) * sizeNumberOfUnits) % 4096 == 0 &&
           _ui->contentLineEdit->text().length() > 0 &&
           _ui->contentLineEdit->text().trimmed().length() == _ui->contentLineEdit->text().length();
}

void ResidentRomBlockEditor::saveComponentConfiguration()
{
    uint64_t startAddress = 0;
    uint64_t sizeNumberOfUnits = 0;

    if (hadesvm::util::fromEntireString(_ui->startAddressLineEdit->text(), "%X", startAddress) &&
        hadesvm::util::fromEntireString(_ui->sizeNumberOfUnitsLineEdit->text(), sizeNumberOfUnits) &&
        sizeNumberOfUnits > 0 &&
        _ui->sizeUnitComboBox->currentIndex() != -1)
    {
        _residentRomBlock->setStartAddress(startAddress);
        _residentRomBlock->setSize(hadesvm::core::MemorySize(sizeNumberOfUnits, _selectedMemorySizeUnit()));
        _residentRomBlock->setContentFilePath(_ui->contentLineEdit->text());
    }
}

//////////
//  Implementation helpers
hadesvm::core::MemorySize::Unit ResidentRomBlockEditor::_selectedMemorySizeUnit() const
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

void ResidentRomBlockEditor::_setSelectedMemorySizeUnit(hadesvm::core::MemorySize::Unit unit)
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
void ResidentRomBlockEditor::_onStartAddressLineEditTextChanged(QString)
{
    emit contentChanged();
}

void ResidentRomBlockEditor::_onSizeNumberOfUnitsLineEditTextChanged(QString)
{
    emit contentChanged();
}

void ResidentRomBlockEditor::_onSizeUnitComboBoxCurrentIndexChanged(int)
{
    emit contentChanged();
}

void ResidentRomBlockEditor::_onContentLineEditTextChanged(QString)
{
    emit contentChanged();
}

void ResidentRomBlockEditor::_onBrowsePushButtonClicked()
{
    QString fileName =
        QFileDialog::getOpenFileName(
            this->topLevelWidget(),
            "Select ROM image",
            _residentRomBlock->virtualAppliance()->directory(),
            "ROM image files (*.img *.bhin)");
    if (fileName.length() != 0)
    {
        _ui->contentLineEdit->setText(_residentRomBlock->virtualAppliance()->toRelativePath(fileName));
        emit contentChanged();
    }
}

//  End of hadesvm-cereon/ResidentRomBlockEditor.cpp
