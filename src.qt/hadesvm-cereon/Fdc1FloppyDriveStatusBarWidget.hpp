//
//  hadesvm-cereon/Fdc1FloppyDriveStatsBarWidget.hpp
//
//  hadesvm-cereon StatusBarWidget for a Fdc1FloppyDrive component
//
//////////
#pragma once
#include "hadesvm-cereon/API.hpp"

namespace hadesvm
{
    namespace cereon
    {
        //////////
        //  The StatusBarWidget for a Fdc1FloppyDrive component
        namespace Ui { class Fdc1FloppyDriveStatusBarWidget; }

        class HADESVM_CEREON_PUBLIC Fdc1FloppyDriveStatusBarWidget final : public hadesvm::core::StatusBarWidget
        {
            Q_OBJECT
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Fdc1FloppyDriveStatusBarWidget)

            friend class Fdc1FloppyDrive;

            //////////
            //  Construction/destruction
        public:
            explicit Fdc1FloppyDriveStatusBarWidget(Fdc1FloppyDrive * fdc1FloppyDrive);
            virtual ~Fdc1FloppyDriveStatusBarWidget();

            //////////
            //  QWidget
        protected:
            virtual void        paintEvent(QPaintEvent * event) override;

            //////////
            //  hadesvm::core::StatusBarWidget
        public:
            virtual QString     displayName() const override;

            //////////
            //  Implementation
        private:
            Fdc1FloppyDrive *const  _fdc1FloppyDrive;
            bool                _canDestruct = false;

            //////////
            //  Controls & resources
        private:
            Ui::Fdc1FloppyDriveStatusBarWidget *    _ui;

            QColor              _normalBkColor;
            QColor              _readingBkColor;
            QColor              _writingBkColor;

            QImage              _connectedFloppyImage;
            QImage              _disconnectedFloppyImage;

            mutable QMenu       _controlMenu;
            QAction *const      _mountAction;
            QAction *const      _unmountAction;
            QAction *const      _separatorAction;
            QAction *const      _selectImageAction;

            QTimer              _refreshTimer;

            //////////
            //  Signal handlers
        private slots:
            void                _onRefreshTimerTick();
            void                _showContextMenu(const QPoint &);
            void                _onMountActionTriggered(bool);
            void                _onUnmountActionTriggered(bool);
            void                _onSelectImageActionTriggered(bool);
        };
    }
}

//  End of hadesvm-cereon/Fdc1FloppyDriveStatsBarWidget.hpp
