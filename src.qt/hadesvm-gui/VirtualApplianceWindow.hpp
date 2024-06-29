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
        //  The "running VA" window
        namespace Ui { class VirtualApplianceWindow; }

        class VirtualApplianceWindow final : public QMainWindow
        {
            Q_OBJECT
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(VirtualApplianceWindow)

            //////////
            //  Construction/destruction
        public:
            explicit VirtualApplianceWindow(hadesvm::core::VirtualAppliance * virtualAppliance);
            ~VirtualApplianceWindow();

            //////////
            //  QWidget
        protected:
            virtual void        closeEvent(QCloseEvent * event) override;

            //////////
            //  Implementation
        private:
            hadesvm::core::VirtualAppliance *   _virtualAppliance;

            QMap<hadesvm::core::IComponent*, hadesvm::core::IComponent::Ui*>    _componentUis;
            QMap<int, hadesvm::core::DisplayWidget*> _displayWidgetsByTabIndex;

            //  Helpers
            void                _refresh();

            //////////
            //  Controls & resources
        private:
            Ui::VirtualApplianceWindow *    _ui;

            //////////
            //  Signal handlers
        private slots:
            void                _onStopVm();
            void                _onSuspendVm();

            void                _onCustomContextMenuRequested(const QPoint &point);
        };
    }
}

//  End of hadesvm-gui/NewVirtualMachineDialog.hpp

