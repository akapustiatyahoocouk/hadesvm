//
//  hadesvm-cereon/MemoryBusEditor.cpp
//
//  hadesvm::cereon::MemoryBusEditor class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;
#include "ui_MemoryBusEditor.h"

//////////
//  Construction/destruction
MemoryBusEditor::MemoryBusEditor(MemoryBus * memoryBus)
    :   hadesvm::core::ComponentEditor(),
        //  Implementation
        _memoryBus(memoryBus),
        //  Controls & resources
        _ui(new Ui::MemoryBusEditor)
{
    _ui->setupUi(this);

    //  Fill in the "clock freuency unit" combo box
    _ui->clockUnitComboBox->addItem("Hz", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::ClockFrequency::Unit::Hz)));
    _ui->clockUnitComboBox->addItem("KHz", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::ClockFrequency::Unit::KHz)));
    _ui->clockUnitComboBox->addItem("MHz", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::ClockFrequency::Unit::MHz)));
    _ui->clockUnitComboBox->addItem("GHz", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::ClockFrequency::Unit::GHz)));
    _ui->clockUnitComboBox->setCurrentIndex(2);  //  MB
}

MemoryBusEditor::~MemoryBusEditor()
{
    delete _ui;
}

//////////
//  hadesvm::core::ComponentEditor
void MemoryBusEditor::loadComponentConfiguration()
{
    _ui->clockNumberOfUnitsLineEdit->setText(hadesvm::util::toString(_memoryBus->clockFrequency().numberOfUnits()));
    _setSelectedClockFrequencyUnit(_memoryBus->clockFrequency().unit());
}

bool MemoryBusEditor::canSaveComponentConfiguration() const
{
    uint64_t clockNumberOfUnits = 0;

    return hadesvm::util::fromString(_ui->clockNumberOfUnitsLineEdit->text(), clockNumberOfUnits) &&
           clockNumberOfUnits > 0;
}

void MemoryBusEditor::saveComponentConfiguration()
{
    uint64_t clockNumberOfUnits = 0;
    if (hadesvm::util::fromString(_ui->clockNumberOfUnitsLineEdit->text(), clockNumberOfUnits) &&
        clockNumberOfUnits > 0)
    {
        _memoryBus->setClockFrequency(hadesvm::core::ClockFrequency(clockNumberOfUnits, _selectedClockFrequencyUnit()));
    }
}

//////////
//  Implementation helpers
hadesvm::core::ClockFrequency::Unit MemoryBusEditor::_selectedClockFrequencyUnit() const
{
    switch (_ui->clockUnitComboBox->currentIndex())
    {
        case 0:
            return hadesvm::core::ClockFrequency::Unit::Hz;
        case 1:
            return hadesvm::core::ClockFrequency::Unit::KHz;
        case 2:
            return hadesvm::core::ClockFrequency::Unit::MHz;
        case 3:
            return hadesvm::core::ClockFrequency::Unit::GHz;
        default:
            return hadesvm::core::ClockFrequency::Unit::Hz;
    }
}

void MemoryBusEditor::_setSelectedClockFrequencyUnit(hadesvm::core::ClockFrequency::Unit unit)
{
    switch (unit)
    {
        case hadesvm::core::ClockFrequency::Unit::Hz:
            _ui->clockUnitComboBox->setCurrentIndex(0);
            break;
        case hadesvm::core::ClockFrequency::Unit::KHz:
            _ui->clockUnitComboBox->setCurrentIndex(1);
            break;
        case hadesvm::core::ClockFrequency::Unit::MHz:
            _ui->clockUnitComboBox->setCurrentIndex(2);
            break;
        case hadesvm::core::ClockFrequency::Unit::GHz:
            _ui->clockUnitComboBox->setCurrentIndex(3);
            break;
        default:
            _ui->clockUnitComboBox->setCurrentIndex(0);
            break;
    }
}

//////////
//  Signal handlers
void MemoryBusEditor::_onClockNumberOfUnitsLineEditTextChanged(QString)
{
    emit contentChanged();
}

void MemoryBusEditor::_onClockUnitComboBoxCurrentIndexChanged(int)
{
    emit contentChanged();
}

//  End of hadesvm-cereon/MemoryBusEditor.cpp
