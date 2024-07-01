//
//  hadesvm-cereon/Vds1DisplayWidget.cpp
//
//  hadesvm::cereon::Vds1DisplayWidget class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;
#include "ui_Vds1DisplayWidget.h"

//////////
//  Construction/destruction
Vds1DisplayWidget::Vds1DisplayWidget(QWidget * parent, Vds1Display * vds1Display)
    :   hadesvm::core::DisplayWidget(parent),
        //  Implementation
        _vds1Display(vds1Display),
        //  Controls & resources
        _ui(new Ui::Vds1DisplayWidget),
        _refreshTimer(this),
        _controlMenu(),
        _showOriginalSize(_controlMenu.addAction("Original size")),
        _showIntegralStretch(_controlMenu.addAction("Intergal stretch")),
        _showFill(_controlMenu.addAction("Fill")),
        _offScreenBuffer(Vds1Controller::_VideoSignal::PixelWidth,
                         Vds1Controller::_VideoSignal::PixelHeight,
                         QImage::Format::Format_RGB32),
        _currentOffScreenPixels(),
        _newOffScreenPixels()
{
    _ui->setupUi(this);

    memset(_currentOffScreenPixels, 0, sizeof(_currentOffScreenPixels));
    _vds1Display->_videoSignal->getPixelColors(0, 0,
                                               Vds1Controller::_VideoSignal::PixelWidth,
                                               Vds1Controller::_VideoSignal::PixelHeight,
                                               _newOffScreenPixels);
    //  Prepare control menu
    _showOriginalSize->setCheckable(true);
    _showIntegralStretch->setCheckable(true);
    _showFill->setCheckable(true);

    //  Set up control menu signal handlers
    connect(_showOriginalSize, &QAction::triggered,
            this, &Vds1DisplayWidget::_onShowOriginalSize);
    connect(_showIntegralStretch, &QAction::triggered,
            this, &Vds1DisplayWidget::_onShowIntegralStretch);
    connect(_showFill, &QAction::triggered,
            this, &Vds1DisplayWidget::_onShowFill);

    //  Start refreshes
    _refreshTimer.setInterval(50);  //  20fps - for now
    connect(&_refreshTimer, &QTimer::timeout, this, &Vds1DisplayWidget::_onRefreshTimerTick);
    _refreshTimer.start();
}

Vds1DisplayWidget::~Vds1DisplayWidget()
{
    Q_ASSERT(_canDestruct);

    _refreshTimer.stop();
    delete _ui;
}

//////////
//  QWidget
void Vds1DisplayWidget::paintEvent(QPaintEvent * /*event*/)
{
    if (_vds1Display == nullptr || _vds1Display->_videoSignal == nullptr)
    {   //  VA is shutting down?
        return;
    }

    QRect g = this->geometry();
    //  TODO kill off qDebug() << g;

    /*  TODO kill off - this is debug code
    QElapsedTimer elapsedTimer;
    elapsedTimer.start();
    */

    _vds1Display->_videoSignal->getPixelColors(0, 0,
                                               Vds1Controller::_VideoSignal::PixelWidth,
                                               Vds1Controller::_VideoSignal::PixelHeight,
                                               _newOffScreenPixels);
    //  Update the off-screen image
    unsigned offset = 0;
    for (unsigned y = 0; y < Vds1Controller::_VideoSignal::PixelHeight; y++)
    {
        for (unsigned x = 0; x < Vds1Controller::_VideoSignal::PixelWidth; x++)
        {
            if (_newOffScreenPixels[offset] != _currentOffScreenPixels[offset])
            {
                _currentOffScreenPixels[offset] = _newOffScreenPixels[offset];
                _offScreenBuffer.setPixel(x, y, _newOffScreenPixels[offset]);
            }
            offset++;
        }
    }

    QPainter painter;
    painter.begin(this);
    painter.drawImage(g, _offScreenBuffer); //  TODO honour the stretch mode
    painter.end();
    /*  TODO kill off - this is debug code
    qint64 actualNsElapsed = elapsedTimer.nsecsElapsed();
    qDebug() << "Vds1DisplayWidget::paintEvent took " << actualNsElapsed << " ns";
    */
}

void Vds1DisplayWidget::keyPressEvent(QKeyEvent * event)
{
    qDebug() << "keyPressEvent: "
             << "nativeScanCode=" << event->nativeScanCode()
             << "nativeVirtualKey=" << event->nativeVirtualKey();
    event->accept();
    if (_vds1Display->_kis1Keyboard != nullptr)
    {
        _vds1Display->_kis1Keyboard->handleKeyPressed(event);
    }
}

void Vds1DisplayWidget::keyReleaseEvent(QKeyEvent * event)
{
    qDebug() << "keyReleaseEvent: "
             << "nativeScanCode=" << event->nativeScanCode()
             << "nativeVirtualKey=" << event->nativeVirtualKey();
    event->accept();
    if (_vds1Display->_kis1Keyboard != nullptr)
    {
        _vds1Display->_kis1Keyboard->handleKeyReleased(event);
    }
}

bool Vds1DisplayWidget::focusNextPrevChild(bool /*next*/)
{
    return false;
}

//////////
//  hadesvm::core::DisplayWidget
QString Vds1DisplayWidget::displayName() const
{
    return "VDS1@" +
           hadesvm::util::toString(_vds1Display->controllerStatePortAddress(), "%04X") +
           ":" +
           hadesvm::util::toString(_vds1Display->controllerCompartmentNumber());
}

QMenu * Vds1DisplayWidget::controlMenu() const
{
    //  Adjust...
    _showOriginalSize->setChecked(_vds1Display->stretchMode() == Vds1Display::StretchMode::NoStretch);
    _showIntegralStretch->setChecked(_vds1Display->stretchMode() == Vds1Display::StretchMode::IntegralStretch);
    _showFill->setChecked(_vds1Display->stretchMode() == Vds1Display::StretchMode::Fill);
    //  ...and return
    return &_controlMenu;
}

QImage Vds1DisplayWidget::currentImage() const
{
    return _offScreenBuffer;
}

//////////
//  Signal handlers
void Vds1DisplayWidget::_onRefreshTimerTick()
{
    this->repaint();
}

void Vds1DisplayWidget::_onShowOriginalSize()
{
    _vds1Display->setStretchMode(Vds1Display::StretchMode::NoStretch);
}

void Vds1DisplayWidget::_onShowIntegralStretch()
{
    _vds1Display->setStretchMode(Vds1Display::StretchMode::IntegralStretch);
}

void Vds1DisplayWidget::_onShowFill()
{
    _vds1Display->setStretchMode(Vds1Display::StretchMode::Fill);
}

//  End of hadesvm-cereon/Vds1DisplayWidget.cpp
