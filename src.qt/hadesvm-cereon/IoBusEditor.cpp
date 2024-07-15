//
//  hadesvm-cereon/IoBusEditor.cpp
//
//  hadesvm::cereon::IoBusEditor class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;
#include "ui_IoBusEditor.h"

//////////
//  Construction/destruction
IoBusEditor::IoBusEditor(IoBus * ioBus)
    :   hadesvm::core::ComponentEditor(),
    //  Implementation
    _ioBus(ioBus),
    //  Controls & resources
    _ui(new Ui::IoBusEditor)
{
    _ui->setupUi(this);

    //  Fill in the "clock freuency unit" combo box
    _ui->clockUnitComboBox->addItem("Hz", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::ClockFrequency::Unit::Hz)));
    _ui->clockUnitComboBox->addItem("KHz", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::ClockFrequency::Unit::KHz)));
    _ui->clockUnitComboBox->addItem("MHz", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::ClockFrequency::Unit::MHz)));
    _ui->clockUnitComboBox->addItem("GHz", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::ClockFrequency::Unit::GHz)));
    _ui->clockUnitComboBox->setCurrentIndex(2);  //  MB
}

IoBusEditor::~IoBusEditor()
{
    delete _ui;
}

//////////
//  hadesvm::core::ComponentEditor
void IoBusEditor::loadComponentConfiguration()
{
    _ui->clockNumberOfUnitsLineEdit->setText(hadesvm::util::toString(_ioBus->clockFrequency().numberOfUnits()));
    _setSelectedClockFrequencyUnit(_ioBus->clockFrequency().unit());
}

bool IoBusEditor::canSaveComponentConfiguration() const
{
    uint64_t clockNumberOfUnits = 0;

    return hadesvm::util::fromString(_ui->clockNumberOfUnitsLineEdit->text(), clockNumberOfUnits) &&
           clockNumberOfUnits > 0;
}

void IoBusEditor::saveComponentConfiguration()
{
    uint64_t clockNumberOfUnits = 0;
    if (hadesvm::util::fromString(_ui->clockNumberOfUnitsLineEdit->text(), clockNumberOfUnits) &&
        clockNumberOfUnits > 0)
    {
        _ioBus->setClockFrequency(hadesvm::core::ClockFrequency(clockNumberOfUnits, _selectedClockFrequencyUnit()));
    }
}

//////////
//  Implementation helpers
hadesvm::core::ClockFrequency::Unit IoBusEditor::_selectedClockFrequencyUnit() const
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

void IoBusEditor::_setSelectedClockFrequencyUnit(hadesvm::core::ClockFrequency::Unit unit)
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
void IoBusEditor::_onClockNumberOfUnitsLineEditTextChanged(QString)
{
    emit contentChanged();
}

void IoBusEditor::_onClockUnitComboBoxCurrentIndexChanged(int)
{
    emit contentChanged();
}

//  End of hadesvm-cereon/IoBusEditor.cpp
