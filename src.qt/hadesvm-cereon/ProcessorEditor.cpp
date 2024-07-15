//
//  hadesvm-cereon/ProcessorEditor.cpp
//
//  hadesvm::cereon::ProcessorEditor class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;
#include "ui_ProcessorEditor.h"

//////////
//  Construction/destruction
ProcessorEditor::ProcessorEditor(Processor * processor)
    :   ComponentEditor(),
        //  Implementation
        _processor(processor),
        //  Controls & resources
        _ui(new Ui::ProcessorEditor)
{
    _ui->setupUi(this);

    //  Fill in the "clock freuency unit" combo box
    _ui->clockUnitComboBox->addItem("Hz", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::ClockFrequency::Unit::Hz)));
    _ui->clockUnitComboBox->addItem("KHz", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::ClockFrequency::Unit::KHz)));
    _ui->clockUnitComboBox->addItem("MHz", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::ClockFrequency::Unit::MHz)));
    _ui->clockUnitComboBox->addItem("GHz", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::ClockFrequency::Unit::GHz)));
    _ui->clockUnitComboBox->setCurrentIndex(2);  //  MB
}

ProcessorEditor::~ProcessorEditor()
{
    delete _ui;
}

//////////
//  hadesvm::core::ComponentEditor
void ProcessorEditor::loadComponentConfiguration()
{
    Features features = _processor->features();

    _ui->idLineEdit->setText(hadesvm::util::toString(_processor->id(), "%02X"));
    _ui->primaryCheckBox->setChecked(_processor->isPrimaryProcessor());
    _ui->clockNumberOfUnitsLineEdit->setText(hadesvm::util::toString(_processor->clockFrequency().numberOfUnits()));
    _setSelectedClockFrequencyUnit(_processor->clockFrequency().unit());
    _ui->bigEndianRadioButton->setChecked(_processor->byteOrder() == hadesvm::util::ByteOrder::BigEndian);
    _ui->littleEndianRadioButton->setChecked(_processor->byteOrder() == hadesvm::util::ByteOrder::LittleEndian);
    _ui->restartAddressLineEdit->setText(hadesvm::util::toString(_processor->restartAddress(), "%016X"));

    _ui->baseCheckBox->setChecked(features.has(Feature::Base));
    _ui->floatingPointCheckBox->setChecked(features.has(Feature::FloatingPoint));
    _ui->unalignedOperandsCheckBox->setChecked(features.has(Feature::UnalignedOperand));
    _ui->debugCheckBox->setChecked(features.has(Feature::Debug));
    _ui->protectedMemoryCheckBox->setChecked(features.has(Feature::ProtectedMemory));
    _ui->virtualMemoryCheckBox->setChecked(features.has(Feature::VirtualMemory));
    _ui->performanceMonitoringCheckBox->setChecked(features.has(Feature::PerformanceMonitoring));

    _ui->byteOrderChangeCheckBox->setChecked(_processor->canChangeByteOrder());
}

bool ProcessorEditor::canSaveComponentConfiguration() const
{
    uint8_t id = 0;
    uint64_t restartAddress = 0;
    uint64_t clockNumberOfUnits = 0;

    return hadesvm::util::fromString(_ui->idLineEdit->text(), "%X", id) &&
           hadesvm::util::fromString(_ui->restartAddressLineEdit->text(), "%X", restartAddress) &&
           restartAddress % 4 == 0 &&
           hadesvm::util::fromString(_ui->clockNumberOfUnitsLineEdit->text(), clockNumberOfUnits) &&
           clockNumberOfUnits > 0 &&
           _ui->clockUnitComboBox->currentIndex() != -1;
}

void ProcessorEditor::saveComponentConfiguration()
{
    uint8_t id = 0;
    if (hadesvm::util::fromString(_ui->idLineEdit->text(), "%X", id))
    {
        _processor->setId(id);
    }

    _processor->setPrimaryProcessor(_ui->primaryCheckBox->isChecked());

    uint64_t clockNumberOfUnits = 0;
    if (hadesvm::util::fromString(_ui->clockNumberOfUnitsLineEdit->text(), clockNumberOfUnits) &&
        clockNumberOfUnits > 0)
    {
        _processor->setClockFrequency(hadesvm::core::ClockFrequency(clockNumberOfUnits, _selectedClockFrequencyUnit()));
    }

    _processor->setByteOrder(_ui->bigEndianRadioButton->isChecked() ?
                                 hadesvm::util::ByteOrder::BigEndian :
                                 hadesvm::util::ByteOrder::LittleEndian);

    uint64_t restartAddress = 0;
    if (hadesvm::util::fromString(_ui->restartAddressLineEdit->text(), "%X", restartAddress) &&
        restartAddress % 4 == 0)
    {
        _processor->setRestartAddress(restartAddress);
    }
}

//////////
//  Implementation helpers
hadesvm::core::ClockFrequency::Unit ProcessorEditor::_selectedClockFrequencyUnit() const
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

void ProcessorEditor::_setSelectedClockFrequencyUnit(hadesvm::core::ClockFrequency::Unit unit)
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
void ProcessorEditor::_onIdLineEditTextChanged(QString)
{
    emit contentChanged();
}

void ProcessorEditor::_onPrimaryCheckBoxToggled(bool)
{
    emit contentChanged();
}

void ProcessorEditor::_onRestartAddressLineEditTextChanged(QString)
{
    emit contentChanged();
}

void ProcessorEditor::_onClockNumberOfUnitsLineEditTextChanged(QString)
{
    emit contentChanged();
}

void ProcessorEditor::_onClockUnitComboBoxCurrentIndexChanged(int)
{
    emit contentChanged();
}

void ProcessorEditor::_onBigEndianRadioButtonClicked()
{
    emit contentChanged();
}

void ProcessorEditor::_onLittleEndianRadioButtonClicked()
{
    emit contentChanged();
}

//  End of hadesvm-cereon/ProcessorEditor.cpp
