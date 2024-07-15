//
//  hadesvm-cereon/Cmos1Editor.cpp
//
//  hadesvm::cereon::Cmos1Editor class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;
#include "ui_Cmos1Editor.h"

//////////
//  Construction/destruction
Cmos1Editor::Cmos1Editor(Cmos1 * cmos1)
    :   hadesvm::core::ComponentEditor(),
        //  Implementation
        _cmos1(cmos1),
        //  Controls & resources
        _ui(new Ui::Cmos1Editor)
{
    _ui->setupUi(this);

    //  Fill in the "clock freuency unit" combo box
    _ui->clockUnitComboBox->addItem("Hz", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::ClockFrequency::Unit::Hz)));
    _ui->clockUnitComboBox->addItem("KHz", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::ClockFrequency::Unit::KHz)));
    _ui->clockUnitComboBox->addItem("MHz", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::ClockFrequency::Unit::MHz)));
    _ui->clockUnitComboBox->addItem("GHz", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::ClockFrequency::Unit::GHz)));
    _ui->clockUnitComboBox->setCurrentIndex(2);  //  MB

    //  Fill in the "read/write delay unit" combo boxes
    _ui->readDelayUnitComboBox->addItem("s", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::TimeInterval::Unit::S)));
    _ui->readDelayUnitComboBox->addItem("ms", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::TimeInterval::Unit::Ms)));
    _ui->readDelayUnitComboBox->addItem("us", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::TimeInterval::Unit::Us)));
    _ui->readDelayUnitComboBox->addItem("ns", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::TimeInterval::Unit::Ns)));

    _ui->writeDelayUnitComboBox->addItem("s", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::TimeInterval::Unit::S)));
    _ui->writeDelayUnitComboBox->addItem("ms", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::TimeInterval::Unit::Ms)));
    _ui->writeDelayUnitComboBox->addItem("us", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::TimeInterval::Unit::Us)));
    _ui->writeDelayUnitComboBox->addItem("ns", QVariant::fromValue(static_cast<uint64_t>(hadesvm::core::TimeInterval::Unit::Ns)));
}

Cmos1Editor::~Cmos1Editor()
{
    delete _ui;
}

//////////
//  hadesvm::core::ComponentEditor
void Cmos1Editor::loadComponentConfiguration()
{
    _ui->clockNumberOfUnitsLineEdit->setText(hadesvm::util::toString(_cmos1->clockFrequency().numberOfUnits()));
    _setSelectedClockFrequencyUnit(_cmos1->clockFrequency().unit());

    _ui->readDelayNumberOfUnitsLineEdit->setText(hadesvm::util::toString(_cmos1->readDelay().numberOfUnits()));
    _setSelectedReadDelayUnit(_cmos1->readDelay().unit());

    _ui->writeDelayNumberOfUnitsLineEdit->setText(hadesvm::util::toString(_cmos1->writeDelay().numberOfUnits()));
    _setSelectedWriteDelayUnit(_cmos1->writeDelay().unit());

    _ui->statePortLineEdit->setText(hadesvm::util::toString(_cmos1->statePortAddress(), "%04X"));
    _ui->addressPortLineEdit->setText(hadesvm::util::toString(_cmos1->addressPortAddress(), "%04X"));
    _ui->dataPortLineEdit->setText(hadesvm::util::toString(_cmos1->dataPortAddress(), "%04X"));
    _ui->interruptMaskPortLineEdit->setText(hadesvm::util::toString(_cmos1->interruptMaskPortAddress(), "%04X"));

    _ui->contentLineEdit->setText(_cmos1->contentFilePath());
}

bool Cmos1Editor::canSaveComponentConfiguration() const
{
    uint64_t clockNumberOfUnits = 0,
             readDelayNumberOfUnits = 0,
             writeDelayNumberOfUnits = 0;
    uint16_t statePortAddress = 0,
             addressPortAddress = 0,
             dataPortAddress = 0,
             interruptMaskPortAddress = 0;

    return hadesvm::util::fromString(_ui->clockNumberOfUnitsLineEdit->text(), clockNumberOfUnits) &&
           clockNumberOfUnits > 0 &&
           hadesvm::util::fromString(_ui->readDelayNumberOfUnitsLineEdit->text(), readDelayNumberOfUnits) &&
           readDelayNumberOfUnits > 0 &&
           hadesvm::util::fromString(_ui->writeDelayNumberOfUnitsLineEdit->text(), writeDelayNumberOfUnits) &&
           writeDelayNumberOfUnits > 0 &&
           hadesvm::util::fromString(_ui->statePortLineEdit->text(), "%X", statePortAddress) &&
           hadesvm::util::fromString(_ui->addressPortLineEdit->text(), "%X", addressPortAddress) &&
           hadesvm::util::fromString(_ui->dataPortLineEdit->text(), "%X", dataPortAddress) &&
           hadesvm::util::fromString(_ui->interruptMaskPortLineEdit->text(), "%X", interruptMaskPortAddress) &&
           _ui->contentLineEdit->text().length() > 0 &&
           _ui->contentLineEdit->text().trimmed().length() == _ui->contentLineEdit->text().length();
}

void Cmos1Editor::saveComponentConfiguration()
{
    uint64_t clockNumberOfUnits = 0;
    if (hadesvm::util::fromString(_ui->clockNumberOfUnitsLineEdit->text(), clockNumberOfUnits) &&
        clockNumberOfUnits > 0)
    {
        _cmos1->setClockFrequency(hadesvm::core::ClockFrequency(clockNumberOfUnits, _selectedClockFrequencyUnit()));
    }

    uint64_t readDelayNumberOfUnits = 0;
    if (hadesvm::util::fromString(_ui->readDelayNumberOfUnitsLineEdit->text(), readDelayNumberOfUnits) &&
        readDelayNumberOfUnits > 0)
    {
        _cmos1->setReadDelay(hadesvm::core::TimeInterval(readDelayNumberOfUnits, _selectedReadDelayUnit()));
    }

    uint64_t writeDelayNumberOfUnits = 0;
    if (hadesvm::util::fromString(_ui->writeDelayNumberOfUnitsLineEdit->text(), writeDelayNumberOfUnits) &&
        writeDelayNumberOfUnits > 0)
    {
        _cmos1->setWriteDelay(hadesvm::core::TimeInterval(writeDelayNumberOfUnits, _selectedWriteDelayUnit()));
    }

    uint16_t statePortAddress = 0;
    if (hadesvm::util::fromString(_ui->statePortLineEdit->text(), "%X", statePortAddress))
    {
        _cmos1->setStatePortAddress(statePortAddress);
    }

    uint16_t addressPortAddress = 0;
    if (hadesvm::util::fromString(_ui->addressPortLineEdit->text(), "%X", addressPortAddress))
    {
        _cmos1->setAddressPortAddress(addressPortAddress);
    }

    uint16_t dataPortAddress = 0;
    if (hadesvm::util::fromString(_ui->dataPortLineEdit->text(), "%X", dataPortAddress))
    {
        _cmos1->setDataPortAddress(dataPortAddress);
    }

    uint16_t interruptMaskPortAddress = 0;
    if (hadesvm::util::fromString(_ui->interruptMaskPortLineEdit->text(), "%X", interruptMaskPortAddress))
    {
        _cmos1->setInterruptMaskPortAddress(interruptMaskPortAddress);
    }

    _cmos1->setContentFilePath(_ui->contentLineEdit->text());
}

//////////
//  Implementation helpers
hadesvm::core::ClockFrequency::Unit Cmos1Editor::_selectedClockFrequencyUnit() const
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

void Cmos1Editor::_setSelectedClockFrequencyUnit(hadesvm::core::ClockFrequency::Unit unit)
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

hadesvm::core::TimeInterval::Unit Cmos1Editor::_selectedReadDelayUnit() const
{
    switch (_ui->readDelayUnitComboBox->currentIndex())
    {
        case 0:
            return hadesvm::core::TimeInterval::Unit::S;
        case 1:
            return hadesvm::core::TimeInterval::Unit::Ms;
        case 2:
            return hadesvm::core::TimeInterval::Unit::Us;
        case 3:
            return hadesvm::core::TimeInterval::Unit::Ns;
        default:
            return hadesvm::core::TimeInterval::Unit::S;
    }
}

void Cmos1Editor::_setSelectedReadDelayUnit(hadesvm::core::TimeInterval::Unit unit)
{
    switch (unit)
    {
        case hadesvm::core::TimeInterval::Unit::S:
        _ui->readDelayUnitComboBox->setCurrentIndex(0);
            break;
        case hadesvm::core::TimeInterval::Unit::Ms:
            _ui->readDelayUnitComboBox->setCurrentIndex(1);
            break;
        case hadesvm::core::TimeInterval::Unit::Us:
            _ui->readDelayUnitComboBox->setCurrentIndex(2);
            break;
        case hadesvm::core::TimeInterval::Unit::Ns:
            _ui->readDelayUnitComboBox->setCurrentIndex(3);
            break;
        default:
            _ui->readDelayUnitComboBox->setCurrentIndex(0);
            break;
    }
}

hadesvm::core::TimeInterval::Unit Cmos1Editor::_selectedWriteDelayUnit() const
{
    switch (_ui->writeDelayUnitComboBox->currentIndex())
    {
        case 0:
            return hadesvm::core::TimeInterval::Unit::S;
        case 1:
            return hadesvm::core::TimeInterval::Unit::Ms;
        case 2:
            return hadesvm::core::TimeInterval::Unit::Us;
        case 3:
            return hadesvm::core::TimeInterval::Unit::Ns;
        default:
            return hadesvm::core::TimeInterval::Unit::S;
    }
}

void Cmos1Editor::_setSelectedWriteDelayUnit(hadesvm::core::TimeInterval::Unit unit)
{
    switch (unit)
    {
        case hadesvm::core::TimeInterval::Unit::S:
            _ui->writeDelayUnitComboBox->setCurrentIndex(0);
            break;
        case hadesvm::core::TimeInterval::Unit::Ms:
            _ui->writeDelayUnitComboBox->setCurrentIndex(1);
            break;
        case hadesvm::core::TimeInterval::Unit::Us:
            _ui->writeDelayUnitComboBox->setCurrentIndex(2);
            break;
        case hadesvm::core::TimeInterval::Unit::Ns:
            _ui->writeDelayUnitComboBox->setCurrentIndex(3);
            break;
        default:
            _ui->writeDelayUnitComboBox->setCurrentIndex(0);
            break;
    }
}

//////////
//  Signal handlers
void Cmos1Editor::_onClockNumberOfUnitsLineEditTextChanged(QString)
{
    emit contentChanged();
}

void Cmos1Editor::_onClockUnitComboBoxCurrentIndexChanged(int)
{
    emit contentChanged();
}

void Cmos1Editor::_onReadDelayNumberOfUnitsLineEditTextChanged(QString)
{
    emit contentChanged();
}

void Cmos1Editor::_onReadDelayUnitComboBoxCurrentIndexChanged(int)
{
    emit contentChanged();
}

void Cmos1Editor::_onWriteDelayNumberOfUnitsLineEditTextChanged(QString)
{
    emit contentChanged();
}

void Cmos1Editor::_onWriteDelayUnitComboBoxCurrentIndexChanged(int)
{
    emit contentChanged();
}

void Cmos1Editor::_onStatePortLineEditTextChanged(QString)
{
    emit contentChanged();
}

void Cmos1Editor::_onAddressPortLineEditTextChanged(QString)
{
    emit contentChanged();
}

void Cmos1Editor::_onDataPortLineEditTextChanged(QString)
{
    emit contentChanged();
}

void Cmos1Editor::_onInterruptMaskPortLineEditTextChanged(QString)
{
    emit contentChanged();
}

void Cmos1Editor::_onBrowsePushButtonClicked()
{
    QString fileName =
        QFileDialog::getSaveFileName(
            this->topLevelWidget(),
            "Select CMOS1 image",
            _cmos1->virtualAppliance()->directory(),
            "CMOS1 image files (*.dat)");
    if (fileName.length() != 0)
    {
        _ui->contentLineEdit->setText(_cmos1->virtualAppliance()->toRelativePath(fileName));
        emit contentChanged();
    }
}

//  End of hadesvm-cereon/Cmos1Editor.cpp
