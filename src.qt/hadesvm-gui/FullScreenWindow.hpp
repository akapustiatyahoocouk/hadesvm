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
            FullScreenWindow();
            virtual ~FullScreenWindow();

            //////////
            //  QWindow
        protected:
            virtual void    paintEvent(QPaintEvent * event) override;
            virtual void    closeEvent(QCloseEvent * event) override;

            //////////
            //  Controls & resources
        private:
            QBackingStore   _backingStore;

            //////////
            //  Signal handlers
        private slots:
            void            _onActveChanged();
        };
    }
}

//  End of hadesvm-gui/FullScreenWindow.hpp
