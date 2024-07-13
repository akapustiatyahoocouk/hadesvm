//
//  hadesvm-cereon/Fdc1ControllerEditor.cpp
//
//  hadesvm::cereon::Fdc1ControllerEditor class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;
#include "ui_Fdc1ControllerEditor.h"

//////////
//  Construction/destruction
Fdc1ControllerEditor::Fdc1ControllerEditor(QWidget * parent, Fdc1Controller * fdc1Controller)
    :   hadesvm::core::ComponentEditor(parent),
        //  Implementation
        _fdc1Controller(fdc1Controller),
        //  Controls & resources
        _ui(new Ui::Fdc1ControllerEditor)
{
    _ui->setupUi(this);

    //  Fill in the "clock freuency unit" combo box
    _ui->clockUnitComboBox->addItem("Hz", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::ClockFrequency::Unit::Hz)));
    _ui->clockUnitComboBox->addItem("KHz", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::ClockFrequency::Unit::KHz)));
    _ui->clockUnitComboBox->addItem("MHz", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::ClockFrequency::Unit::MHz)));
    _ui->clockUnitComboBox->addItem("GHz", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::ClockFrequency::Unit::GHz)));
    _ui->clockUnitComboBox->setCurrentIndex(2);  //  MB
}

Fdc1ControllerEditor::~Fdc1ControllerEditor()
{
    delete _ui;
}

//////////
//  hadesvm::core::ComponentEditor
void Fdc1ControllerEditor::loadComponentConfiguration()
{
    _ui->stateAndControlPortLineEdit->setText(hadesvm::util::toString(_fdc1Controller->stateAndControlPortAddress(), "%04X"));
    _ui->dataPortLineEdit->setText(hadesvm::util::toString(_fdc1Controller->dataPortAddress(), "%04X"));
    _ui->interruptMaskPortLineEdit->setText(hadesvm::util::toString(_fdc1Controller->interruptMaskPortAddress(), "%04X"));

    _ui->clockNumberOfUnitsLineEdit->setText(hadesvm::util::toString(_fdc1Controller->clockFrequency().numberOfUnits()));
    _setSelectedClockFrequencyUnit(_fdc1Controller->clockFrequency().unit());
}

bool Fdc1ControllerEditor::canSaveComponentConfiguration() const
{
    uint16_t stateAndControlPortAddress = 0,
             dataPortAddress = 0,
             interruptMaskPortAddress = 0;
    uint64_t clockNumberOfUnits = 0;

    return hadesvm::util::fromString(_ui->stateAndControlPortLineEdit->text(), "%X", stateAndControlPortAddress) &&
           hadesvm::util::fromString(_ui->dataPortLineEdit->text(), "%X", dataPortAddress) &&
           hadesvm::util::fromString(_ui->interruptMaskPortLineEdit->text(), "%X", interruptMaskPortAddress)  &&
           hadesvm::util::fromString(_ui->clockNumberOfUnitsLineEdit->text(), clockNumberOfUnits) &&
           clockNumberOfUnits > 0;;
}

void Fdc1ControllerEditor::saveComponentConfiguration()
{
    uint16_t stateAndControlPortAddress = 0;
    if (hadesvm::util::fromString(_ui->stateAndControlPortLineEdit->text(), "%X", stateAndControlPortAddress))
    {
        _fdc1Controller->setStateAndControlPortAddress(stateAndControlPortAddress);
    }

    uint16_t dataPortAddress = 0;
    if (hadesvm::util::fromString(_ui->dataPortLineEdit->text(), "%X", dataPortAddress))
    {
        _fdc1Controller->setDataPortAddress(dataPortAddress);
    }

    uint16_t interruptMaskPortAddress = 0;
    if (hadesvm::util::fromString(_ui->interruptMaskPortLineEdit->text(), "%X", interruptMaskPortAddress))
    {
        _fdc1Controller->setInterruptMaskPortAddress(interruptMaskPortAddress);
    }

    uint64_t clockNumberOfUnits = 0;
    if (hadesvm::util::fromString(_ui->clockNumberOfUnitsLineEdit->text(), clockNumberOfUnits) &&
        clockNumberOfUnits > 0)
    {
        _fdc1Controller->setClockFrequency(hadesvm::core::ClockFrequency(clockNumberOfUnits, _selectedClockFrequencyUnit()));
    }
}

//////////
//  Implementation helpers
hadesvm::core::ClockFrequency::Unit Fdc1ControllerEditor::_selectedClockFrequencyUnit() const
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

void Fdc1ControllerEditor::_setSelectedClockFrequencyUnit(hadesvm::core::ClockFrequency::Unit unit)
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
void Fdc1ControllerEditor::_onStateAndControlPortLineEditTextChanged(QString)
{
    emit contentChanged();
}

void Fdc1ControllerEditor::_onDataPortLineEditTextChanged(QString)
{
    emit contentChanged();
}

void Fdc1ControllerEditor::_onInterruptMaskPortLineEditTextChanged(QString)
{
    emit contentChanged();
}

void Fdc1ControllerEditor::_onClockNumberOfUnitsLineEditTextChanged(QString)
{
    emit contentChanged();
}

void Fdc1ControllerEditor::_onClockUnitComboBoxCurrentIndexChanged(int)
{
    emit contentChanged();
}

//  End of hadesvm-cereon/Fdc1ControllerEditor.cpp
