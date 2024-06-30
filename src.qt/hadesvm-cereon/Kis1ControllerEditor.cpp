//
//  hadesvm-cereon/Kis1ControllerEditor.cpp
//
//  hadesvm::cereon::Kis1ControllerEditor class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;
#include "ui_Kis1ControllerEditor.h"

//////////
//  Construction/destruction
Kis1ControllerEditor::Kis1ControllerEditor(QWidget * parent, Kis1Controller * kis1Controller)
    :   hadesvm::core::ComponentEditor(parent),
        //  Implementation
        _kis1Controller(kis1Controller),
        //  Controls & resources
        _ui(new Ui::Kis1ControllerEditor)
{
    _ui->setupUi(this);

    //  Fill in the "clock freuency unit" combo box
    _ui->clockUnitComboBox->addItem("Hz", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::ClockFrequency::Unit::Hz)));
    _ui->clockUnitComboBox->addItem("KHz", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::ClockFrequency::Unit::KHz)));
    _ui->clockUnitComboBox->addItem("MHz", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::ClockFrequency::Unit::MHz)));
    _ui->clockUnitComboBox->addItem("GHz", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::ClockFrequency::Unit::GHz)));
    _ui->clockUnitComboBox->setCurrentIndex(2);  //  MB

    //  Fill in the "number of compartments" combo box
    for (unsigned i = 1; i <= 256; i++)
    {
        _ui->compartmentsComboBox->addItem(hadesvm::util::toString(i), QVariant::fromValue(i));
    }
    _ui->compartmentsComboBox->setCurrentIndex(0);
}

Kis1ControllerEditor::~Kis1ControllerEditor()
{
    delete _ui;
}

//////////
//  hadesvm::core::ComponentEditor
void Kis1ControllerEditor::loadComponentConfiguration()
{
    _ui->statePortLineEdit->setText(hadesvm::util::toString(_kis1Controller->statePortAddress(), "%04X"));
    _ui->currentDevicePortLineEdit->setText(hadesvm::util::toString(_kis1Controller->currentDevicePortAddress(), "%04X"));
    _ui->interruptMaskPortLineEdit->setText(hadesvm::util::toString(_kis1Controller->interruptMaskPortAddress(), "%04X"));
    _ui->inputSourcePortLineEdit->setText(hadesvm::util::toString(_kis1Controller->inputSourcePortAddress(), "%04X"));
    _ui->deviceStatePortLineEdit->setText(hadesvm::util::toString(_kis1Controller->deviceStatePortAddress(), "%04X"));
    _ui->dataInPortLineEdit->setText(hadesvm::util::toString(_kis1Controller->dataInPortAddress(), "%04X"));
    _ui->layoutPortLineEdit->setText(hadesvm::util::toString(_kis1Controller->layoutPortAddress(), "%04X"));

    _ui->clockNumberOfUnitsLineEdit->setText(hadesvm::util::toString(_kis1Controller->clockFrequency().numberOfUnits()));
    _setSelectedClockFrequencyUnit(_kis1Controller->clockFrequency().unit());

    _ui->compartmentsComboBox->setCurrentIndex(_kis1Controller->numberOfCompartments() - 1);
}

bool Kis1ControllerEditor::canSaveComponentConfiguration() const
{
    uint16_t statePortAddress = 0,
             currentDevicePortAddress = 0,
             interruptMaskPortAddress = 0,
             inputSourcePortAddress = 0,
             deviceStatePortAddress = 0,
             dataInPortAddress = 0,
             layoutPortAddress = 0;
    uint64_t clockNumberOfUnits = 0;

    return hadesvm::util::fromString(_ui->statePortLineEdit->text(), "%X", statePortAddress) &&
           hadesvm::util::fromString(_ui->currentDevicePortLineEdit->text(), "%X", currentDevicePortAddress) &&
           hadesvm::util::fromString(_ui->interruptMaskPortLineEdit->text(), "%X", interruptMaskPortAddress) &&
           hadesvm::util::fromString(_ui->inputSourcePortLineEdit->text(), "%X", inputSourcePortAddress) &&
           hadesvm::util::fromString(_ui->deviceStatePortLineEdit->text(), "%X", deviceStatePortAddress) &&
           hadesvm::util::fromString(_ui->dataInPortLineEdit->text(), "%X", dataInPortAddress) &&
           hadesvm::util::fromString(_ui->layoutPortLineEdit->text(), "%X", layoutPortAddress)  &&
           hadesvm::util::fromString(_ui->clockNumberOfUnitsLineEdit->text(), clockNumberOfUnits) &&
           clockNumberOfUnits > 0;;
}

void Kis1ControllerEditor::saveComponentConfiguration()
{
    uint16_t statePortAddress = 0;
    if (hadesvm::util::fromString(_ui->statePortLineEdit->text(), "%X", statePortAddress))
    {
        _kis1Controller->setStatePortAddress(statePortAddress);
    }

    uint16_t currentDevicePortAddress = 0;
    if (hadesvm::util::fromString(_ui->currentDevicePortLineEdit->text(), "%X", currentDevicePortAddress))
    {
        _kis1Controller->setCurrentDevicePortAddress(currentDevicePortAddress);
    }

    uint16_t interruptMaskPortAddress = 0;
    if (hadesvm::util::fromString(_ui->interruptMaskPortLineEdit->text(), "%X", interruptMaskPortAddress))
    {
        _kis1Controller->setInterruptMaskPortAddress(interruptMaskPortAddress);
    }

    uint16_t inputSourcePortAddress = 0;
    if (hadesvm::util::fromString(_ui->inputSourcePortLineEdit->text(), "%X", inputSourcePortAddress))
    {
        _kis1Controller->setInputSourcePortAddress(inputSourcePortAddress);
    }

    uint16_t deviceStatePortAddress = 0;
    if (hadesvm::util::fromString(_ui->deviceStatePortLineEdit->text(), "%X", deviceStatePortAddress))
    {
        _kis1Controller->setDeviceStatePortAddress(deviceStatePortAddress);
    }

    uint16_t dataInPortAddress = 0;
    if (hadesvm::util::fromString(_ui->dataInPortLineEdit->text(), "%X", dataInPortAddress))
    {
        _kis1Controller->setDataInPortAddress(dataInPortAddress);
    }

    uint16_t layoutPortAddress = 0;
    if (hadesvm::util::fromString(_ui->layoutPortLineEdit->text(), "%X", layoutPortAddress))
    {
        _kis1Controller->setLayoutPortAddress(layoutPortAddress);
    }

    uint64_t clockNumberOfUnits = 0;
    if (hadesvm::util::fromString(_ui->clockNumberOfUnitsLineEdit->text(), clockNumberOfUnits) &&
        clockNumberOfUnits > 0)
    {
        _kis1Controller->setClockFrequency(hadesvm::core::ClockFrequency(clockNumberOfUnits, _selectedClockFrequencyUnit()));
    }

    _kis1Controller->setNumberOfCompartments(_ui->compartmentsComboBox->currentIndex() + 1);
}

//////////
//  Implementation helpers
hadesvm::core::ClockFrequency::Unit Kis1ControllerEditor::_selectedClockFrequencyUnit() const
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

void Kis1ControllerEditor::_setSelectedClockFrequencyUnit(hadesvm::core::ClockFrequency::Unit unit)
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
void Kis1ControllerEditor::_onStatePortLineEditTextChanged(QString)
{
    emit contentChanged();
}

void Kis1ControllerEditor::_onCurrentDevicePortLineEditTextChanged(QString)
{
    emit contentChanged();
}

void Kis1ControllerEditor::_onInterruptMaskPortLineEditTextChanged(QString)
{
    emit contentChanged();
}

void Kis1ControllerEditor::_onInputSourcePortLineEditTextChanged(QString)
{
    emit contentChanged();
}

void Kis1ControllerEditor::_onDeviceStatePortLineEditTextChanged(QString)
{
    emit contentChanged();
}

void Kis1ControllerEditor::_onDataInPortLineEditTextChanged(QString)
{
    emit contentChanged();
}

void Kis1ControllerEditor::_onLayoutPortLineEditTextChanged(QString)
{
    emit contentChanged();
}

void Kis1ControllerEditor::_onClockNumberOfUnitsLineEditTextChanged(QString)
{
    emit contentChanged();
}

void Kis1ControllerEditor::_onClockUnitComboBoxCurrentIndexChanged(int)
{
    emit contentChanged();
}

//  End of hadesvm-cereon/Kis1ControllerEditor.cpp
