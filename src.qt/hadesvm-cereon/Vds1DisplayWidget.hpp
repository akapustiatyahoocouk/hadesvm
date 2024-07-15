//
//  hadesvm-cereon/Vds1DisplayWidget.hpp
//
//  hadesvm-cereon DisplayWidget for a Vds1Display component
//
//////////
#pragma once
#include "hadesvm-cereon/API.hpp"

namespace hadesvm
{
    namespace cereon
    {
        //////////
        //  The DisplayWidget for a Vds1DisplayWidget component
        namespace Ui { class Vds1DisplayWidget; }

        class HADESVM_CEREON_PUBLIC Vds1DisplayWidget final : public hadesvm::core::DisplayWidget
        {
            Q_OBJECT
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Vds1DisplayWidget)

            friend class Vds1Display;

            //////////
            //  Construction/destruction
        public:
            explicit Vds1DisplayWidget(Vds1Display * vds1Display);
            virtual ~Vds1DisplayWidget();

            //////////
            //  QWidget
        protected:
            virtual void        paintEvent(QPaintEvent * event) override;
            virtual void        keyPressEvent(QKeyEvent * event) override;
            virtual void        keyReleaseEvent(QKeyEvent * event) override;
            virtual bool        focusNextPrevChild(bool next) override;

            //////////
            //  hadesvm::core::DisplayWidget
        public:
            virtual QString     displayName() const override;
            virtual QMenu *     controlMenu() const override;
            virtual QImage      currentImage() const override;

            //////////
            //  Implementation
        private:
            Vds1Display *const  _vds1Display;
            bool                _canDestruct = false;

            //////////
            //  Controls & resources
        private:
            Ui::Vds1DisplayWidget * _ui;

            QTimer              _refreshTimer;

            mutable QMenu       _controlMenu;
            QAction *const      _showOriginalSize;
            QAction *const      _showIntegralStretch;
            QAction *const      _showFill;

            QImage              _offScreenBuffer;
            QRgb                _currentOffScreenPixels[Vds1Controller::_VideoSignal::PixelWidth *
                                                        Vds1Controller::_VideoSignal::PixelHeight];
            QRgb                _newOffScreenPixels[Vds1Controller::_VideoSignal::PixelWidth *
                                                    Vds1Controller::_VideoSignal::PixelHeight];

            //////////
            //  Signal handlers
        private slots:
            void                _onRefreshTimerTick();
            void                _onShowOriginalSize();
            void                _onShowIntegralStretch();
            void                _onShowFill();
        };
    }
}
