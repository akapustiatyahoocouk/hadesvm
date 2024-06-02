//
//  hadesvm-gui/ConfigureVirtualApplianceDialog.cpp
//
//  hadesvm::gui::ConfigureVirtualApplianceDialog class implementation
//
//////////
#include "hadesvm-gui/API.hpp"
using namespace hadesvm::gui;
#include "ui_ConfigureVirtualApplianceDialog.h"

//////////
//  Construction/destruction
ConfigureVirtualApplianceDialog::ConfigureVirtualApplianceDialog(hadesvm::core::VirtualAppliance * virtualAppliance,
                                                                 QWidget * parent)
    :   QDialog(parent),
        //  Implementation
        _virtualAppliance(virtualAppliance),
        _originalName(virtualAppliance->name()),
        //  Controls & resources
        _ui(new Ui::ConfigureVirtualApplianceDialog),
        _addComponentPopupMenu(new QMenu())
{
    Q_ASSERT(_virtualAppliance != nullptr);
    Q_ASSERT(_virtualAppliance->state() == hadesvm::core::VirtualAppliance::State::Stopped);

    _ui->setupUi(this);

    _ui->nameLineEdit->setText(_virtualAppliance->name());
    _ui->locationLineEdit->setText(_virtualAppliance->location());
    _ui->architectureLineEdit->setText(_virtualAppliance->architecture()->displayName());

    //  Create component category nodes
    _componentCategories = hadesvm::core::ComponentCategory::all();
    std::sort(_componentCategories.begin(),
              _componentCategories.end(),
              [](auto a, auto b) -> bool { return a->displayName() < b->displayName(); });
    for (auto componentCategory : _componentCategories)
    {
        QTreeWidgetItem * item = new QTreeWidgetItem(QStringList(componentCategory->displayName()));
        _ui->configurationTreeWidget->addTopLevelItem(item);
        _mapTreeItemsToComponentCategories.insert(item, componentCategory);
    }

    //  Prepare "create component" popup menu
    for (auto componentCategory : _componentCategories)
    {
        QMenu * submenu = _addComponentPopupMenu->addMenu(componentCategory->displayName());
        bool submenuHasItems = false;
        hadesvm::core::ComponentTypeList componentTypes = componentCategory->componentTypes();
        for (auto componentType : componentTypes)
        {
            if (componentType->isCompatibleWith(_virtualAppliance->architecture()) &&
                componentType->isCompatibleWith(_virtualAppliance->type()))
            {
                QAction * action = submenu->addAction(componentType->displayName());
                _ComponentCreator * cc = new _ComponentCreator(this, componentType);
                _componentCreators.append(cc);
                connect(action, &QAction::triggered, [cc]() { cc->invoke(); });
                submenuHasItems = true;
            }
        }
        submenu->setEnabled(submenuHasItems);
    }

    //  Done
    _refresh();
    _ui->configurationTreeWidget->expandAll();
}

ConfigureVirtualApplianceDialog::~ConfigureVirtualApplianceDialog()
{
    delete _addComponentPopupMenu;
    delete _ui;

    for (auto cc : _componentCreators)
    {
        delete cc;
    }
}

//////////
//  Implementation helpers
void ConfigureVirtualApplianceDialog::_refresh()
{
    if (!_refreshUnderway)
    {
        _refreshUnderway = true;

        //  TODO kill off auto selectedComponentCategory = _selectedComponentCategory();
        auto selectedComponent = _selectedComponent();

        //  Update components tree
        _mapTreeItemsToComponents.clear();
        for (auto componentCategoryNode : _mapTreeItemsToComponentCategories.keys())
        {
            auto componentCategory = _mapTreeItemsToComponentCategories[componentCategoryNode];
            _refreshComponentNodes(componentCategoryNode, componentCategory);
        }

        //  always enabled TODO kill off this line _ui->addComponentPushButton->setEnabled(selectedComponentCategory != nullptr);
        _ui->removeComponentPushButton->setEnabled(selectedComponent != nullptr);

        _ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(
            hadesvm::core::VirtualAppliance::isValidName(_ui->nameLineEdit->text()));

        _refreshUnderway = false;
    }
}

void ConfigureVirtualApplianceDialog::_refreshComponentNodes(
        QTreeWidgetItem * componentCategoryItem,
        hadesvm::core::ComponentCategory * componentCategory)
{
    //  Prepare a list of all VA components of the required category...
    hadesvm::core::ComponentList components;
    for (auto component : _virtualAppliance->components())
    {
        if (component->type()->category() == componentCategory)
        {
            components.append(component);
        }
    }
    //  ...sorted by display name
    std::sort(components.begin(),
              components.end(),
              [](auto a, auto b) -> bool { return a->displayName() < b->displayName(); });

    //  Make sure the componentCategoryNode has a proper number of children...
    while (components.count() < componentCategoryItem->childCount())
    {   //  Too many children
        QTreeWidgetItem * childItem = componentCategoryItem->child(componentCategoryItem->childCount() - 1);
        componentCategoryItem->removeChild(childItem);
        delete childItem;
    }
    while (components.count() > componentCategoryItem->childCount())
    {   //  Too few children
        QTreeWidgetItem * childItem = new QTreeWidgetItem(componentCategoryItem, QStringList("TODO"));
    }
    //  ...with correct properties
    for (int i = 0; i < components.count(); i++)
    {
        componentCategoryItem->child(i)->setText(0, components[i]->displayName());
        _mapTreeItemsToComponents[componentCategoryItem->child(i)] = components[i];
    }
}

hadesvm::core::ComponentCategory * ConfigureVirtualApplianceDialog::_selectedComponentCategory()
{
    QTreeWidgetItem * selectedItem = _ui->configurationTreeWidget->currentItem();
    return _mapTreeItemsToComponentCategories.contains(selectedItem) ?
                _mapTreeItemsToComponentCategories[selectedItem] :
                nullptr;
}

hadesvm::core::Component * ConfigureVirtualApplianceDialog::_selectedComponent()
{
    QTreeWidgetItem * selectedItem = _ui->configurationTreeWidget->currentItem();
    return _mapTreeItemsToComponents.contains(selectedItem) ?
               _mapTreeItemsToComponents[selectedItem] :
               nullptr;
}

bool ConfigureVirtualApplianceDialog::_setSelectedComponent(hadesvm::core::Component * component)
{
    for (auto componentItem : _mapTreeItemsToComponents.keys())
    {
        if (_mapTreeItemsToComponents[componentItem] == component)
        {
            _ui->configurationTreeWidget->setCurrentItem(componentItem);
            return true;
        }
    }
    return false;
}

//////////
//  Signal handlers
void ConfigureVirtualApplianceDialog::_onNameChanged(QString)
{
    _refresh();
}

void ConfigureVirtualApplianceDialog::_onConfigurationTreeWidgetSelectionChanged()
{
    _refresh();
}

void ConfigureVirtualApplianceDialog::_onAddComponentPushButtonClicked()
{
    _addComponentPopupMenu->popup(
        _ui->addComponentPushButton->mapToGlobal(QPoint(0, -_addComponentPopupMenu->sizeHint().height())));
}

void ConfigureVirtualApplianceDialog::_onRemoveComponentPushButtonClicked()
{
    hadesvm::core::Component * component = _selectedComponent();
    if (component == nullptr)
    {   //  Nothing to do
        return;
    }
    //  Confirm...
    if (QMessageBox::question(this,
                              "Remove component",
                              "Really remove " + component->displayName() + "?") != QMessageBox::StandardButton::Yes)
    {   //  Don't close
        return;
    }
    //  ...remove...
    _virtualAppliance->removeComponent(component);
    if (!_addedComponents.contains(component))
    {
        _removedComponents.append(component);
    }
    else
    {
        _addedComponents.removeOne(component);
    }
    //  ...and update the UI
    _refresh();
}

void ConfigureVirtualApplianceDialog::_onOk()
{
    //  Apply the rest of the changes
    _virtualAppliance->setName(_ui->nameLineEdit->text());
    //  If some component were removed from the VA, we must destroy them now
    for (auto removedComponent : _removedComponents)
    {
        delete removedComponent;
    }
    //  Done
    accept();
}

void ConfigureVirtualApplianceDialog::_onCancel()
{
    //  If some components were added to VA, remove and destroy them
    for (auto addedComponent : _addedComponents)
    {
        Q_ASSERT(!_removedComponents.contains(addedComponent));
        _virtualAppliance->removeComponent(addedComponent);
        delete addedComponent;
    }
    //  If some components were removed from the VA, return them there
    for (auto removedComponent : _removedComponents)
    {
        Q_ASSERT(!_addedComponents.contains(removedComponent));
        _virtualAppliance->addComponent(removedComponent);
        //  TODO if the removedComponent used to be adapted, we need to
        //  restore the adaptor's configuration...
        //  ...TODO ...as well as component's own configuration
    }
    //  Done
    reject();
}

//////////
//  ConfigureVirtualApplianceDialog::_ComponentCreator
ConfigureVirtualApplianceDialog::_ComponentCreator::_ComponentCreator(
        ConfigureVirtualApplianceDialog * dlg,
        hadesvm::core::ComponentType * componentType)
    :   _dlg(dlg),
        _componentType(componentType)
{
}

void ConfigureVirtualApplianceDialog::_ComponentCreator::invoke()
{
    qDebug() << _componentType->displayName();
    //  Create a new component and add it to the VA
    hadesvm::core::Component * component = _componentType->createComponent();
    _dlg->_virtualAppliance->addComponent(component);
    _dlg->_addedComponents.append(component);
    //  Update the UI and TODO select newly created component as "current"
    _dlg->_refresh();
    _dlg->_setSelectedComponent(component);
}

//  End of hadesvm-gui/ConfigureVirtualApplianceDialog.cpp
