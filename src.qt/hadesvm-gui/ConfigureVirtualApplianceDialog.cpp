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
        //  Saved state & state changes
        _addedComponents(),
        _removedComponents(),
        _savedConfiguration(),
        _savedComponentConfigurations(),
        _savedComponentAdaptorConfigurations(),
        //  Working data
        _componentCategories(),
        _mapTreeItemsToComponentCategories(),
        _mapTreeItemsToComponents(),
        //  Helper agents
        _componentCreators(),
        //  Controls & resources
        _ui(new Ui::ConfigureVirtualApplianceDialog),
        _addComponentPopupMenu(new QMenu()),
        _componentEditors()
{
    Q_ASSERT(_virtualAppliance != nullptr);
    Q_ASSERT(_virtualAppliance->state() == hadesvm::core::VirtualAppliance::State::Stopped);

    _ui->setupUi(this);

    _ui->nameLineEdit->setText(_virtualAppliance->name());
    _ui->locationLineEdit->setText(_virtualAppliance->location());
    _ui->architectureLineEdit->setText(_virtualAppliance->architecture()->displayName());
    _ui->startAutomaticallyCheckBox->setChecked(_virtualAppliance->startAutomatically());
    _ui->startFullScreenCheckBox->setChecked(_virtualAppliance->startFullScreen());
    _ui->startMinimizedCheckBox->setChecked(_virtualAppliance->startMinimized());

    //  Save configuration in case the user cancels the dialog
    _saveComponentConfigurations();

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

    //  Prepare component editors
    for (auto component : _virtualAppliance->components())
    {
        if (hadesvm::core::ComponentEditor * editor = component->createEditor(_ui->editorsFrame))
        {
            editor->loadComponentConfiguration();
            _componentEditors[component] = editor;
            editor->move(0, 0);
            connect(editor, &hadesvm::core::ComponentEditor::contentChanged,
                    this, &ConfigureVirtualApplianceDialog::_onEditorContentChanged);
        }
    }

    //  Done
    _resizeToFitAllEditors();
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

        _adjustEditorVisibility();

        bool okEnabled = hadesvm::core::VirtualAppliance::isValidName(_ui->nameLineEdit->text());
        for (auto editor : _componentEditors.values())
        {
            if (!editor->canSaveComponentConfiguration())
            {
                okEnabled = false;
            }
        }
        _ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(okEnabled);

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
        if (component->componentType()->category() == componentCategory)
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
        new QTreeWidgetItem(componentCategoryItem, QStringList("TODO"));
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

void ConfigureVirtualApplianceDialog::_saveComponentConfigurations()
{
    QDomElement rootElement = _savedConfiguration.createElement("Configurations");
    _savedConfiguration.appendChild(rootElement);

    for (auto component : _virtualAppliance->compatibleComponents())
    {
        QDomElement componentElement = _savedConfiguration.createElement("Component");
        componentElement.setAttribute("Type", component->componentType()->mnemonic());
        rootElement.appendChild(componentElement);
        component->serialiseConfiguration(componentElement);
        _savedComponentConfigurations[component] = componentElement;
    }

    for (auto adaptor : _virtualAppliance->componentAdaptors())
    {
        auto component = adaptor->adaptedComponent();

        QDomElement componentElement = _savedConfiguration.createElement("Component");
        componentElement.setAttribute("Type", component->componentType()->mnemonic());
        rootElement.appendChild(componentElement);
        component->serialiseConfiguration(componentElement);
        _savedComponentConfigurations[component] = componentElement;

        QDomElement adaptorElement = _savedConfiguration.createElement("Adaptor");
        adaptorElement.setAttribute("Type", adaptor->componentAdaptorType()->mnemonic());
        componentElement.appendChild(adaptorElement);
        adaptor->serialiseConfiguration(adaptorElement);
        _savedComponentAdaptorConfigurations[component] = adaptorElement;
    }

    qDebug() << _savedConfiguration.toString(); //  TODO kill off
}

void ConfigureVirtualApplianceDialog::_restoreComponentConfigurations()
{
    for (auto component : _virtualAppliance->compatibleComponents())
    {
        if (_savedComponentConfigurations.contains(component))
        {
            QDomElement componentElement = _savedComponentConfigurations[component];
            Q_ASSERT(componentElement.attribute("Type") == component->componentType()->mnemonic());
            component->deserialiseConfiguration(componentElement);
        }
    }

    for (auto component : _virtualAppliance->adaptedComponents())
    {
        if (_savedComponentConfigurations.contains(component))
        {
            QDomElement componentElement = _savedComponentConfigurations[component];
            Q_ASSERT(componentElement.attribute("Type") == component->componentType()->mnemonic());
            component->deserialiseConfiguration(componentElement);
        }
        hadesvm::core::ComponentAdaptor * adaptor = nullptr;
        for (auto ca : _virtualAppliance->componentAdaptors())
        {
            if (ca->adaptedComponent() == component)
            {
                adaptor = ca;
                break;
            }
        }
        if (adaptor != nullptr && _savedComponentAdaptorConfigurations.contains(component))
        {
            QDomElement adaptorElement = _savedComponentAdaptorConfigurations[component];
            if (adaptorElement.attribute("Type") == adaptor->componentAdaptorType()->mnemonic())
            {
                adaptor->deserialiseConfiguration(adaptorElement);
            }
        }
    }
}

void ConfigureVirtualApplianceDialog::_resizeToFitAllEditors()
{
    QSize editorsSize(0, 0);
    for (auto editor : _componentEditors.values())
    {
        QSize editorSize = editor->size();
        editorsSize.setWidth(qMax(editorsSize.width(), editorSize.width()));
        editorsSize.setHeight(qMax(editorsSize.height(), editorSize.height()));
    }
    const int Leeway = 8;
    if (editorsSize.width() + Leeway > _ui->editorsFrame->width())
    {   //  Expand "editorsFrame" and dialog
        int deltaX = editorsSize.width() - _ui->editorsFrame->width() + Leeway;
        _ui->configurationFrame->resize(_ui->configurationFrame->width() + deltaX, _ui->configurationFrame->height());
        _ui->editorsFrame->resize(_ui->editorsFrame->width() + deltaX, _ui->editorsFrame->height());
        this->resize(this->width() + deltaX, this->height());
    }
    if (editorsSize.height() + Leeway > _ui->editorsFrame->height())
    {   //  Expand "editorsFrame" and dialog
        /*  TODO this doesn't seem to work!!!
        int deltaY = editorsSize.height() - _ui->editorsFrame->height() + Leeway;
        QSize sz1 = _ui->configurationFrame->size();
        _ui->configurationFrame->resize(_ui->configurationFrame->width(), _ui->configurationFrame->height() + deltaY);
        QSize sz2 = _ui->configurationFrame->size();
        //_ui->configurationTreeWidget->resize(_ui->configurationTreeWidget->width(), _ui->configurationTreeWidget->height() + deltaY);
        //_ui->editorsFrame->resize(_ui->editorsFrame->width(), _ui->editorsFrame->height() + deltaY);
        this->resize(this->width(), this->height() + deltaY);
        */
    }
}

void ConfigureVirtualApplianceDialog::_adjustEditorVisibility()
{
    auto component = _selectedComponent();
    auto currentEditor = _componentEditors.contains(component) ? _componentEditors[component] : nullptr;
    for (auto editor : _componentEditors.values())
    {
        editor->setVisible(editor == currentEditor);
    }
}

//////////
//  Signal handlers
void ConfigureVirtualApplianceDialog::_onNameChanged(QString)
{
    _refresh();
}

void ConfigureVirtualApplianceDialog::_onStartFullScreenCheckBoxClicked()
{
    if (_ui->startFullScreenCheckBox->isChecked())
    {
        _ui->startMinimizedCheckBox->setChecked(false);
    }
    _refresh();
}

void ConfigureVirtualApplianceDialog::_onStartMinimizedCheckBoxClicked()
{
    if (_ui->startMinimizedCheckBox->isChecked())
    {
        _ui->startFullScreenCheckBox->setChecked(false);
    }
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
    if (_componentEditors.contains(component))
    {
        delete _componentEditors[component];
        _componentEditors.remove(component);
    }
    _virtualAppliance->removeComponent(component);
    if (!_addedComponents.contains(component))
    {
        _removedComponents.append(component);
    }
    else
    {
        _addedComponents.removeOne(component);
        delete component;
    }
    //  ...and update the UI
    _refresh();
}

void ConfigureVirtualApplianceDialog::_onEditorContentChanged()
{
    _refresh();
}

void ConfigureVirtualApplianceDialog::_onOk()
{
    //  Apply the changes
    _virtualAppliance->setName(_ui->nameLineEdit->text());
    _virtualAppliance->setStartAutomatically(_ui->startAutomaticallyCheckBox->isChecked());
    _virtualAppliance->setStartFullScreen(_ui->startFullScreenCheckBox->isChecked());
    _virtualAppliance->setStartMinimized(_ui->startMinimizedCheckBox->isChecked());
    for (auto editor : _componentEditors.values())
    {
        editor->saveComponentConfiguration();
    }
    //  Make sure VM configuration is sane
    try
    {
        _virtualAppliance->testConfiguration();
    }
    catch (const hadesvm::core::VirtualApplianceException & ex)
    {
        QMessageBox::critical(this, "OOPS!", ex.message());
        return;
    }
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
    }
    //  Restore configurations & we're done
    _restoreComponentConfigurations();
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
    //  Editor too ?
    if (hadesvm::core::ComponentEditor * editor = component->createEditor(_dlg->_ui->editorsFrame))
    {
        editor->loadComponentConfiguration();
        _dlg->_componentEditors[component] = editor;
        editor->move(0, 0);
        connect(editor, &hadesvm::core::ComponentEditor::contentChanged,
                _dlg, &ConfigureVirtualApplianceDialog::_onEditorContentChanged);
        _dlg->_resizeToFitAllEditors();
    }
    //  Update the UI and TODO select newly created component as "current"
    _dlg->_refresh();
    _dlg->_setSelectedComponent(component);
}

//  End of hadesvm-gui/ConfigureVirtualApplianceDialog.cpp
