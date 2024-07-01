//
//  hadesvm-gui/FullScreenWindow.hpp
//
//  hadesvm-gui VA full-screen window
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
        //  The VA full-screen window
        class FullScreenWindow final : public QWindow
        {
            Q_OBJECT
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(FullScreenWindow)

            //////////
            //  Construction/destruction
        public:
            explicit FullScreenWindow(hadesvm::core::DisplayWidget * displayWidget);
            virtual ~FullScreenWindow();

            //////////
            //  QWindow
        protected:
            virtual bool    event(QEvent * event) override;
            virtual void    paintEvent(QPaintEvent * event) override;
            virtual void    closeEvent(QCloseEvent * event) override;
            virtual void    keyPressEvent(QKeyEvent * event) override;
            virtual void    keyReleaseEvent(QKeyEvent * event) override;
            virtual void    mouseMoveEvent(QMouseEvent * event) override;
            virtual void    mousePressEvent(QMouseEvent * event) override;

            //////////
            //  Implementation
        private:
            hadesvm::core::DisplayWidget *  _displayWidget;
            bool            _controlBarVisible = false;

            //  Helpers
            void            _render();
            void            _repaint();
            void            _destroy();

            //////////
            //  Controls & resources
        private:
            QBackingStore   _backingStore;

            QRect           _controlBarRect;
            QRect           _controlBarHoverRect;

            QRect           _exitFullScreenIconRect;
            QIcon           _exitFullScreenIcon;

            QPoint          _cpos{0,0}; //  TODO kill off
            QTimer          _refreshTimer;

            //////////
            //  Signal handlers
        private slots:
            void            _onActiveChanged();
            void            _onRefreshTimerTick();
        };
    }
}

//  End of hadesvm-gui/FullScreenWindow.hpp
