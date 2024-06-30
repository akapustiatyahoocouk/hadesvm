//
//  hadesvm-cereon/Kis1KeyboardEditor.cpp
//
//  hadesvm::cereon::Kis1KeyboardEditor class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;
#include "ui_Kis1KeyboardEditor.h"

//////////
//  Construction/destruction
Kis1KeyboardEditor::Kis1KeyboardEditor(QWidget * parent, Kis1Keyboard * kis1Keyboard)
    :   hadesvm::core::ComponentEditor(parent),
        //  Implementation
        _kis1Keyboard(kis1Keyboard),
        _keyboardLayouts(),
        //  Controls & resources
        _ui(new Ui::Kis1KeyboardEditor)
{
    _ui->setupUi(this);

    //  Fill in the "controller compartment" combo box
    for (unsigned i = 0; i < 256; i++)
    {
        _ui->controllerCompartmentComboBox->addItem(hadesvm::util::toString(i), QVariant::fromValue(i));
    }
    _ui->controllerCompartmentComboBox->setCurrentIndex(0);

    //  Prepare sorted list of available keyboard layouts...
    for (auto keyboardLayout : Kis1KeyboardLayout::all())
    {
        _keyboardLayouts.append(keyboardLayout);
    }
    std::sort(_keyboardLayouts.begin(),
              _keyboardLayouts.end(),
              [](auto a, auto b) { return a->displayName() < b->displayName(); });
    //  ...and populate the "layout" combo box
    for (auto keyboardLayout : _keyboardLayouts)
    {
        _ui->layoutComboBox->addItem(keyboardLayout->displayName());
    }
    _ui->layoutComboBox->setCurrentIndex(0);
}

Kis1KeyboardEditor::~Kis1KeyboardEditor()
{
    delete _ui;
}

//////////
//  hadesvm::core::ComponentEditor
void Kis1KeyboardEditor::loadComponentConfiguration()
{
    _ui->controllerStatePortLineEdit->setText(hadesvm::util::toString(_kis1Keyboard->controllerStatePortAddress(), "%04X"));
    _ui->controllerCompartmentComboBox->setCurrentIndex(_kis1Keyboard->controllerCompartmentNumber());
    _ui->layoutComboBox->setCurrentIndex(static_cast<int>(_keyboardLayouts.indexOf(_kis1Keyboard->layout())));
}

bool Kis1KeyboardEditor::canSaveComponentConfiguration() const
{
    uint16_t controllerStatePortAddress = 0;

    return hadesvm::util::fromString(_ui->controllerStatePortLineEdit->text(), "%X", controllerStatePortAddress);
}

void Kis1KeyboardEditor::saveComponentConfiguration()
{
    uint16_t controllerStatePortAddress = 0;
    if (hadesvm::util::fromString(_ui->controllerStatePortLineEdit->text(), "%X", controllerStatePortAddress))
    {
        _kis1Keyboard->setControllerStatePortAddress(controllerStatePortAddress);
    }

    _kis1Keyboard->setControllerCompartmentNumber(static_cast<uint8_t>(_ui->controllerCompartmentComboBox->currentIndex()));

    _kis1Keyboard->setLayout(_keyboardLayouts[_ui->layoutComboBox->currentIndex()]);
}

//////////
//  Signal handlers
void Kis1KeyboardEditor::_onControllerStatePortLineEditTextChanged(QString)
{
    emit contentChanged();
}

//  End of hadesvm-cereon/Kis1KeyboardEditor.cpp
