//
//  hadesvm-gui/NewVirtualApplianceDialog.cpp
//
//  hadesvm::gui::NewVirtualApplianceDialog class implementation
//
//////////
#include "hadesvm-gui/API.hpp"
using namespace hadesvm::gui;
#include "ui_NewVirtualApplianceDialog.h"

//////////
//  Construction/destruction
NewVirtualApplianceDialog::NewVirtualApplianceDialog(QWidget * parent)
    :   QDialog(parent),
        _ui(new Ui::NewVirtualApplianceDialog)
{
    _ui->setupUi(this);

    //  Fill the "VA architecture" combo box
    hadesvm::core::VirtualArchitectureList architectures = hadesvm::core::VirtualArchitecture::all();
    std::sort(architectures.begin(),
              architectures.end(),
              [](auto a, auto b) -> bool { return a->displayName() < b->displayName(); });
    for (auto architecture : architectures)
    {
        _ui->architectureComboBox->addItem(architecture->displayName(), QVariant::fromValue(architecture));
    }
    if (_ui->architectureComboBox->count() > 0)
    {
        _ui->architectureComboBox->setCurrentIndex(0);  //  TODO last used
    }

    //  Fill the VA type list
    hadesvm::core::VirtualApplianceTypeList types = hadesvm::core::VirtualApplianceType::all();
    std::sort(types.begin(),
              types.end(),
              [](auto a, auto b) -> bool { return a->displayName() < b->displayName(); });
    for (auto type : types)
    {
        _ui->typeComboBox->addItem(type->displayName(), QVariant::fromValue(type));
    }
    if (_ui->typeComboBox->count() > 0)
    {
        _ui->typeComboBox->setCurrentIndex(0);  //  TODO last used
    }

    //  Done
    _refillTemplatesList();
    _refresh();
}

NewVirtualApplianceDialog::~NewVirtualApplianceDialog()
{
    delete _ui;
}

//////////
//  Implementation helpers
void NewVirtualApplianceDialog::_refresh()
{
    if (_ui->architectureComboBox->currentIndex() == -1 ||
        _ui->typeComboBox->currentIndex() == -1)
    {   //  OOPS! No architecture/type selected!
        _ui->architectureLabel->setEnabled(false);
        _ui->architectureComboBox->setEnabled(false);
        _ui->nameLabel->setEnabled(false);
        _ui->nameLineEdit->setEnabled(false);
        _ui->locationLabel->setEnabled(false);
        _ui->locationLineEdit->setEnabled(false);
        _ui->typeLabel->setEnabled(false);
        _ui->typeComboBox->setEnabled(false);
        _ui->browsePushButton->setEnabled(false);
        _ui->templateLabel->setEnabled(false);
        _ui->templateComboBox->setEnabled(false);
        _ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
        return;
    }

    //  Architecture + type selected
    _ui->architectureLabel->setEnabled(true);
    _ui->architectureComboBox->setEnabled(true);
    _ui->nameLabel->setEnabled(true);
    _ui->nameLineEdit->setEnabled(true);
    _ui->locationLabel->setEnabled(true);
    _ui->locationLineEdit->setEnabled(true);
    _ui->typeLabel->setEnabled(true);
    _ui->typeComboBox->setEnabled(true);
    _ui->browsePushButton->setEnabled(true);
    _ui->templateLabel->setEnabled(true);
    _ui->templateComboBox->setEnabled(true);

    //  General case
    _ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(
        _ui->architectureComboBox->currentIndex() != -1 &&
        hadesvm::core::VirtualAppliance::isValidName(_ui->nameLineEdit->text()) &&
        hadesvm::core::VirtualAppliance::isValidLocation(_ui->locationLineEdit->text()) &&
        _ui->typeComboBox->currentIndex() != -1);
}

void NewVirtualApplianceDialog::_refillTemplatesList()
{
    if (_ui->architectureComboBox->currentIndex() == -1 ||
        _ui->typeComboBox->currentIndex() == -1)
    {
        _ui->templateComboBox->clear();
        return;
    }

    auto architecture = _ui->architectureComboBox->currentData().value<hadesvm::core::VirtualArchitecture*>();
    auto type = _ui->typeComboBox->currentData().value<hadesvm::core::VirtualApplianceType*>();
    hadesvm::core::VirtualApplianceTemplateList templates;
    for (auto vaTemplate : hadesvm::core::VirtualApplianceTemplate::all())
    {
        if (vaTemplate->virtualArchitecture() == architecture &&
            vaTemplate->virtualApplianceType() == type)
        {
            templates.append(vaTemplate);
        }
    }
    std::sort(templates.begin(),
              templates.end(),
              [](auto a, auto b) -> bool { return a->displayName() < b->displayName(); });

    _ui->templateComboBox->clear();
    _ui->templateComboBox->addItem("-", QVariant::fromValue(nullptr));
    for (auto vaTemplate : templates)
    {
        _ui->templateComboBox->addItem(vaTemplate->displayName(), QVariant::fromValue(vaTemplate));
    }
    _ui->templateComboBox->setCurrentIndex(0);
}

//////////
//  Signal handlers
void NewVirtualApplianceDialog::_onArchitectureChanged(int)
{
    _refillTemplatesList();
    _refresh();
}

void NewVirtualApplianceDialog::_onNameChanged(QString)
{
    _refresh();
}

void NewVirtualApplianceDialog::_onLocationChanged(QString)
{
    _refresh();
}

void NewVirtualApplianceDialog::_onTypeChanged(int)
{
    _refillTemplatesList();
    _refresh();
}

void NewVirtualApplianceDialog::_onBrowse()
{
    QString location = QFileDialog::getSaveFileName(
        this,
        "Create new VM",
        QDir::homePath(),
        "VM Files (*" + hadesvm::core::VirtualAppliance::PreferredExtension + ")");
    if (location.isEmpty())
    {
        return;
    }
    //  Slap extension if there isn't one
    if (QFileInfo(location).completeSuffix().isEmpty())
    {
        location += hadesvm::core::VirtualAppliance::PreferredExtension;
    }
    qDebug() << location;
    //  Update UI
    _ui->locationLineEdit->setText(location);
    _refresh();
}

void NewVirtualApplianceDialog::_onTemplateChanged(int)
{
    _refresh();
}

void NewVirtualApplianceDialog::_onOk()
{
    _virtualArchitecture = _ui->architectureComboBox->currentData().value<hadesvm::core::VirtualArchitecture*>();;
    _virtualApplianceName = _ui->nameLineEdit->text();
    _virtualApplianceLocation = _ui->locationLineEdit->text();
    _virtualApplianceType = _ui->typeComboBox->currentData().value<hadesvm::core::VirtualApplianceType*>();
    _virtualApplianceTemplate = _ui->templateComboBox->currentData().value<hadesvm::core::VirtualApplianceTemplate*>();

    accept();
}

void NewVirtualApplianceDialog::_onCancel()
{
    reject();
}

//  End of hadesvm-gui/NewVirtualApplianceDialog.cpp
