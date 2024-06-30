//
//  hadesvm-cereon/Vds1ControllerEditor.cpp
//
//  hadesvm::cereon::Vds1ControllerEditor class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;
#include "ui_Vds1ControllerEditor.h"

//////////
//  Construction/destruction
Vds1ControllerEditor::Vds1ControllerEditor(QWidget * parent, Vds1Controller * vds1Controller)
    :   hadesvm::core::ComponentEditor(parent),
        //  Implementation
        _vds1Controller(vds1Controller),
        //  Controls & resources
        _ui(new Ui::Vds1ControllerEditor)
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

Vds1ControllerEditor::~Vds1ControllerEditor()
{
    delete _ui;
}

//////////
//  hadesvm::core::ComponentEditor
void Vds1ControllerEditor::loadComponentConfiguration()
{
    _ui->statePortLineEdit->setText(hadesvm::util::toString(_vds1Controller->statePortAddress(), "%04X"));
    _ui->commandPortLineEdit->setText(hadesvm::util::toString(_vds1Controller->commandPortAddress(), "%04X"));
    _ui->dataPortLineEdit->setText(hadesvm::util::toString(_vds1Controller->dataPortAddress(), "%04X"));

    _ui->clockNumberOfUnitsLineEdit->setText(hadesvm::util::toString(_vds1Controller->clockFrequency().numberOfUnits()));
    _setSelectedClockFrequencyUnit(_vds1Controller->clockFrequency().unit());

    _ui->compartmentsComboBox->setCurrentIndex(_vds1Controller->numberOfCompartments() - 1);
}

bool Vds1ControllerEditor::canSaveComponentConfiguration() const
{
    uint16_t statePortAddress = 0,
             commandPortAddress = 0,
             dataPortAddress = 0;
    uint64_t clockNumberOfUnits = 0;

    return hadesvm::util::fromString(_ui->statePortLineEdit->text(), "%X", statePortAddress) &&
           hadesvm::util::fromString(_ui->commandPortLineEdit->text(), "%X", commandPortAddress) &&
           hadesvm::util::fromString(_ui->dataPortLineEdit->text(), "%X", dataPortAddress) &&
           hadesvm::util::fromString(_ui->clockNumberOfUnitsLineEdit->text(), clockNumberOfUnits) &&
           clockNumberOfUnits > 0;
}

void Vds1ControllerEditor::saveComponentConfiguration()
{
    uint16_t statePortAddress = 0;
    if (hadesvm::util::fromString(_ui->statePortLineEdit->text(), "%X", statePortAddress))
    {
        _vds1Controller->setStatePortAddress(statePortAddress);
    }

    uint16_t commandPortAddress = 0;
    if (hadesvm::util::fromString(_ui->commandPortLineEdit->text(), "%X", commandPortAddress))
    {
        _vds1Controller->setCommandPortAddress(commandPortAddress);
    }

    uint16_t dataPortAddress = 0;
    if (hadesvm::util::fromString(_ui->dataPortLineEdit->text(), "%X", dataPortAddress))
    {
        _vds1Controller->setDataPortAddress(dataPortAddress);
    }

    uint64_t clockNumberOfUnits = 0;
    if (hadesvm::util::fromString(_ui->clockNumberOfUnitsLineEdit->text(), clockNumberOfUnits) &&
        clockNumberOfUnits > 0)
    {
        _vds1Controller->setClockFrequency(hadesvm::core::ClockFrequency(clockNumberOfUnits, _selectedClockFrequencyUnit()));
    }

    _vds1Controller->setNumberOfCompartments(_ui->compartmentsComboBox->currentIndex() + 1);
}

//////////
//  Implementation helpers
hadesvm::core::ClockFrequency::Unit Vds1ControllerEditor::_selectedClockFrequencyUnit() const
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

void Vds1ControllerEditor::_setSelectedClockFrequencyUnit(hadesvm::core::ClockFrequency::Unit unit)
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
void Vds1ControllerEditor::_onStatePortLineEditTextChanged(QString)
{
    emit contentChanged();
}

void Vds1ControllerEditor::_onCommandPortLineEditTextChanged(QString)
{
    emit contentChanged();
}

void Vds1ControllerEditor::_onDataPortLineEditTextChanged(QString)
{
    emit contentChanged();
}

void Vds1ControllerEditor::_onClockNumberOfUnitsLineEditTextChanged(QString)
{
    emit contentChanged();
}

void Vds1ControllerEditor::_onClockUnitComboBoxCurrentIndexChanged(int)
{
    emit contentChanged();
}

//  End of hadesvm-cereon/Vds1ControllerEditor.cpp
