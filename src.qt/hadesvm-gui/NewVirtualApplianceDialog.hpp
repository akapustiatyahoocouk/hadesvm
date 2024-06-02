//
//  hadesvm-gui/NewVirtualMachineDialog.hpp
//
//  hadesvm-gui "New VM" dialog
//
//////////
#pragma once
#include "hadesvm-gui/API.hpp"

namespace hadesvm
{
    namespace gui
    {
        //////////
        //  The "new VM" modal dialog
        namespace Ui { class NewVirtualApplianceDialog; }

        class NewVirtualApplianceDialog final : public QDialog
        {
            Q_OBJECT
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(NewVirtualApplianceDialog)

            //////////
            //  Construction/destruction
        public:
            explicit NewVirtualApplianceDialog(QWidget * parent = nullptr);
            virtual ~NewVirtualApplianceDialog();

            //////////
            //  Operations
        public:
            hadesvm::core::VirtualArchitecture *    virtualArchitecture() const { return _virtualArchitecture; }
            QString             virtualApplianceName() const { return _virtualApplianceName; }
            QString             virtualApplianceLocation() const { return _virtualApplianceLocation; }
            hadesvm::core::VirtualApplianceType *   virtualApplianceType() const { return _virtualApplianceType; }
            hadesvm::core::VirtualApplianceTemplate *   virtualApplianceTemplate() const { return _virtualApplianceTemplate; }

            //////////
            //  Implementation
        private:
            hadesvm::core::VirtualArchitecture *    _virtualArchitecture = nullptr;
            QString             _virtualApplianceName = "";
            QString             _virtualApplianceLocation = "";
            hadesvm::core::VirtualApplianceType *       _virtualApplianceType = nullptr;
            hadesvm::core::VirtualApplianceTemplate *   _virtualApplianceTemplate = nullptr;

            //  Helpers
            void                _refresh();
            void                _refillTemplatesList();

            //////////
            //  Controls & resources
        private:
            Ui::NewVirtualApplianceDialog * _ui;

            //////////
            //  Signal handlers
        private slots:
            void                _onArchitectureChanged(int);
            void                _onNameChanged(QString);
            void                _onLocationChanged(QString);
            void                _onBrowse();
            void                _onTypeChanged(int);
            void                _onTemplateChanged(int);
            void                _onOk();
            void                _onCancel();
        };
    }
}

//  End of hadesvm-gui/NewVirtualApplianceDialog.hpp
