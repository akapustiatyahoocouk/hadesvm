//
//  hadesvm-gui/ConfigureVirtualMachineDialog.hpp
//
//  hadesvm-gui "Configure VM" dialog
//
//////////
#pragma once
#include "hadesvm-gui/API.hpp"

namespace hadesvm
{
    namespace gui
    {
        //////////
        //  The "Configure VM" modal dialog
        namespace Ui { class ConfigureVirtualApplianceDialog; }

        class ConfigureVirtualApplianceDialog final : public QDialog
        {
            Q_OBJECT
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(ConfigureVirtualApplianceDialog)

            //////////
            //  Construction/destruction
        public:
            explicit ConfigureVirtualApplianceDialog(hadesvm::core::VirtualAppliance * virtualAppliance,
                                                     QWidget * parent = nullptr);
            virtual ~ConfigureVirtualApplianceDialog();

            //////////
            //  Implementation
        private:
            hadesvm::core::VirtualAppliance *const  _virtualAppliance;

            hadesvm::core::ComponentCategoryList    _componentCategories = {};   //  sorted by displayName
            QMap<QTreeWidgetItem*, hadesvm::core::ComponentCategory*> _mapTreeItemsToComponentCategories = {};
            QMap<QTreeWidgetItem*, hadesvm::core::Component*> _mapTreeItemsToComponents = {};

            //  Saved state & state changes
            QString             _originalName;
            hadesvm::core::ComponentList    _addedComponents = {};   //  sorted by displayName
            hadesvm::core::ComponentList    _removedComponents = {};   //  sorted by displayName

            //  Helper agents
            class _ComponentCreator final
            {
                HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(_ComponentCreator)

            public:
                _ComponentCreator(ConfigureVirtualApplianceDialog * dlg,
                                  hadesvm::core::ComponentType * componentType);
                ~_ComponentCreator() {}

            public:
                void            invoke();

            private:
                ConfigureVirtualApplianceDialog *   _dlg;
                hadesvm::core::ComponentType *      _componentType;
            };
            QList<_ComponentCreator*>   _componentCreators = {};

            bool                _refreshUnderway = false;

            //  Helpers
            void                _refresh();
            void                _refreshComponentNodes(QTreeWidgetItem * componentCategoryItem,
                                                       hadesvm::core::ComponentCategory * componentCategory);
            hadesvm::core::ComponentCategory *  _selectedComponentCategory();
            hadesvm::core::Component *  _selectedComponent();
            bool                _setSelectedComponent(hadesvm::core::Component * component);

            //////////
            //  Controls & resources
        private:
            Ui::ConfigureVirtualApplianceDialog *   _ui;
            QMenu *             _addComponentPopupMenu = nullptr;

            //////////
            //  Signal handlers
        private slots:
            void                _onNameChanged(QString);
            void                _onConfigurationTreeWidgetSelectionChanged();
            void                _onAddComponentPushButtonClicked();
            void                _onRemoveComponentPushButtonClicked();
            void                _onOk();
            void                _onCancel();
        };
    }
}

//  End of hadesvm-gui/ConfigureVirtualMachineDialog.hpp

