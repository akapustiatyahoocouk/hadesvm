//
//  hadesvm-cereon/Vds1DisplayEditor.cpp
//
//  hadesvm::cereon::Vds1DisplayEditor class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;
#include "ui_Vds1DisplayEditor.h"

//////////
//  Construction/destruction
Vds1DisplayEditor::Vds1DisplayEditor(QWidget * parent, Vds1Display * vds1Display)
    :   hadesvm::core::ComponentEditor(parent),
        //  Implementation
        _vds1Display(vds1Display),
        //  Controls & resources
        _ui(new Ui::Vds1DisplayEditor)
{
    _ui->setupUi(this);

    //  Fill in the "controller compartment" combo box
    for (unsigned i = 0; i < 256; i++)
    {
        _ui->controllerCompartmentComboBox->addItem(hadesvm::util::toString(i), QVariant::fromValue(i));
    }
    _ui->controllerCompartmentComboBox->setCurrentIndex(0);
}

Vds1DisplayEditor::~Vds1DisplayEditor()
{
    delete _ui;
}

//////////
//  hadesvm::core::ComponentEditor
void Vds1DisplayEditor::loadComponentConfiguration()
{
    _ui->controllerStatePortLineEdit->setText(hadesvm::util::toString(_vds1Display->controllerStatePortAddress(), "%04X"));
    _ui->controllerCompartmentComboBox->setCurrentIndex(_vds1Display->controllerCompartmentNumber());
}

bool Vds1DisplayEditor::canSaveComponentConfiguration() const
{
    uint16_t controllerStatePortAddress = 0;

    return hadesvm::util::fromString(_ui->controllerStatePortLineEdit->text(), "%X", controllerStatePortAddress);
}

void Vds1DisplayEditor::saveComponentConfiguration()
{
    uint16_t controllerStatePortAddress = 0;
    if (hadesvm::util::fromString(_ui->controllerStatePortLineEdit->text(), "%X", controllerStatePortAddress))
    {
        _vds1Display->setControllerStatePortAddress(controllerStatePortAddress);
    }

    _vds1Display->setControllerCompartmentNumber(static_cast<uint8_t>(_ui->controllerCompartmentComboBox->currentIndex()));
}

//////////
//  Signal handlers
void Vds1DisplayEditor::_onControllerStatePortLineEditTextChanged(QString)
{
    emit contentChanged();
}

//  End of hadesvm-cereon/Vds1DisplayEditor.cpp
