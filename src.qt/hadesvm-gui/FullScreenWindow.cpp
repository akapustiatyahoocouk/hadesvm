//
//  hadesvm-gui/FullScreenWindow.cpp
//
//  hadesvm::gui::FullScreenWindow class implementation
//
//////////
#include "hadesvm-gui/API.hpp"
using namespace hadesvm::gui;

//////////
//  Construction/destruction
FullScreenWindow::FullScreenWindow(hadesvm::core::DisplayWidget * displayWidget)
    :   QWindow(),
        //  Implementation
        _displayWidget(displayWidget),
        //  Controls & resources
        _backingStore(this),
        _controlBarRect(),
        _controlBarHoverRect(),
        _exitFullScreenIconRect(),
        _exitFullScreenIcon(":/hadesvm/gui/images/actions/MakeWindowedSmall.png"),
        _refreshTimer()
{
    Q_ASSERT(_displayWidget != nullptr);

    this->setWindowState(Qt::WindowState::WindowFullScreen);
    this->setMouseGrabEnabled(true);
    this->setKeyboardGrabEnabled(true);

    //  Listen to activation change events
    connect(this, &QWindow::activeChanged,
            this, &FullScreenWindow::_onActiveChanged);

    //  Refresh periodically
    _refreshTimer.setInterval(50);  //  20fps
    connect(&_refreshTimer, &QTimer::timeout,
            this, &FullScreenWindow::_onRefreshTimerTick);
    _refreshTimer.start();
}

FullScreenWindow::~FullScreenWindow()
{
    _refreshTimer.stop();
}

//////////
//  QWindow
bool FullScreenWindow::event(QEvent * event)
{
    if (event->type() == QEvent::UpdateRequest)
    {
        _render();
        return true;
    }
    return QWindow::event(event);
}

void FullScreenWindow::paintEvent(QPaintEvent * /*event*/)
{
    _render();
}

void FullScreenWindow::closeEvent(QCloseEvent * event)
{
    event->ignore();
}

void FullScreenWindow::keyPressEvent(QKeyEvent * event)
{
    qApp->postEvent(_displayWidget, event->clone());
}

void FullScreenWindow::keyReleaseEvent(QKeyEvent * event)
{
    qApp->postEvent(_displayWidget, event->clone());
}

void FullScreenWindow::mouseMoveEvent(QMouseEvent * event)
{
    _cpos = event->pos();
    if (_controlBarVisible && !_controlBarRect.contains(event->pos()))
    {
        _controlBarVisible = false;
    }
    if (!_controlBarVisible && _controlBarHoverRect.contains(event->pos()))
    {
        _controlBarVisible = true;
    }
    this->setCursor(
        (_controlBarVisible && _controlBarRect.contains(event->pos())) ?
            Qt::CursorShape::ArrowCursor :
            Qt::CursorShape::BlankCursor);
    _repaint();
}

void FullScreenWindow::mousePressEvent(QMouseEvent * event)
{
    if (_controlBarVisible && _exitFullScreenIconRect.contains(event->pos()))
    {
        _destroy();
    }
}

//////////
//  Implementation helpers
void FullScreenWindow::_render()
{
    QRect rc = this->geometry();
    _backingStore.resize(rc.size());
    _backingStore.beginPaint(QRegion(rc));

    QPaintDevice * paintDevice = _backingStore.paintDevice();

    _controlBarRect = QRect(rc.right() - 20, rc.y(), 20, 20);
    _controlBarHoverRect = QRect(rc.x(), rc.y(), rc.width(), 1);
    _exitFullScreenIconRect = QRect(_controlBarRect.x() + 2, _controlBarRect.y() + 2, 16, 16);

    {
        QPainter p(paintDevice);

        p.drawImage(rc, _displayWidget->currentImage());
        //  TODO kill off p.fillRect(_cpos.x(), _cpos.y(), 32, 32, QBrush(QColor("blue")));

        if (_controlBarVisible)
        {
            p.fillRect(_controlBarRect, QBrush(QColor("white")));
            p.setPen(QColor("black"));
            p.drawRect(_controlBarRect);
            _exitFullScreenIcon.paint(&p, _exitFullScreenIconRect);
        }

        p.end();
    }

    _backingStore.endPaint();
    _backingStore.flush(rc);
}

void FullScreenWindow::_repaint()
{
    requestUpdate();
}

void FullScreenWindow::_destroy()
{
    this->setMouseGrabEnabled(false);
    this->setKeyboardGrabEnabled(false);
    destroy();
}

//////////
//  Signal handlers
void FullScreenWindow::_onActiveChanged()
{
    if (!isActive())
    {   //  Destroy
        _destroy();
    }
}

void FullScreenWindow::_onRefreshTimerTick()
{
    _repaint();
}

//  End of hadesvm-gui/FullScreenWindow.cpp
