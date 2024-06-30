//
//  hadesvm-gui/MainWindow.hpp
//
//  hadesvm-gui main UI frame
//
//////////
#pragma once
#include "hadesvm-gui/API.hpp"

namespace hadesvm
{
    namespace gui
    {
        class VirtualApplianceWindow;

        //////////
        //  The main UI frame
        namespace Ui { class MainWindow; }

        class MainWindow final : public QMainWindow
        {
            Q_OBJECT
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(MainWindow)

            //////////
            //  Construction/destruction
        public:
            MainWindow();
            virtual ~MainWindow();

            //////////
            //  QWidget
        protected:
            virtual void        moveEvent(QMoveEvent * event) override;
            virtual void        resizeEvent(QResizeEvent * event) override;
            virtual void        closeEvent(QCloseEvent * event) override;

            //////////
            //  Implementation
        private:
            hadesvm::core::VirtualApplianceList _virtualAppliances;
            hadesvm::core::VirtualAppliance *   _currentVirtualAppliance;

            QMap<hadesvm::core::VirtualAppliance*, VirtualApplianceWindow*> _virtualApplianceWindows;   //  for a;ll Running VAs

            QSettings           _settings;

            bool                _refreshUnderway = false;

            //  Helpers
            void                _refresh();
            void                _saveVirtualAppliance();
            void                _loadVirtualAppliance();

            //////////
            //  Controls & resources
        private:
            Ui::MainWindow *    _ui;
            QTimer              _refreshTimer;

            //////////
            //  Signal handlers
        private slots:
            void                _onNewVm();
            void                _onOpenVm();
            void                _onCloseVm();
            void                _onExit();
            void                _onStartVm();
            void                _onStopVm();
            void                _onSuspendVm();
            void                _onResumeVm();
            void                _onResetVm();
            void                _onConfigureVm();
            void                _onHelpAbout();
            void                _onCurrentVmChanged(int);
            void                _onRefreshTimerTick();
        };
    }
}

//  End of hadesvm-gui/MainWindow.hpp
