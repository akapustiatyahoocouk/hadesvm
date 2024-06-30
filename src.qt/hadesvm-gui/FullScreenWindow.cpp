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
FullScreenWindow::FullScreenWindow()
    :   QWindow(),
        //  Controls & resources
        _backingStore(this)
{
    this->setWindowState(Qt::WindowState::WindowFullScreen);
    this->setCursor(Qt::CursorShape::BlankCursor);

    //  Listen to activation change events
    connect(this, &QWindow::activeChanged,
            this, &FullScreenWindow::_onActveChanged);
}

FullScreenWindow::~FullScreenWindow()
{
}

//////////
//  QWindow
void FullScreenWindow::paintEvent(QPaintEvent * event)
{
    QRect rc = this->geometry();
    _backingStore.resize(rc.size());
    _backingStore.beginPaint(QRegion(rc));

    QPaintDevice * paintDevice = _backingStore.paintDevice();
    {
        QPainter p(paintDevice);
        p.fillRect(rc, QBrush(QColor("green")));
        p.fillRect(rc.adjusted(1, 1, -1, -1), QBrush(QColor("red")));
    }
    _backingStore.flush(QRegion(rc));

    _backingStore.endPaint();
}

void FullScreenWindow::closeEvent(QCloseEvent * event)
{
    event->ignore();
}

//////////
//  Signal handlers
void FullScreenWindow::_onActveChanged()
{
    if (!isActive())
    {   //  Destroy
        delete this;
    }
}

//  End of hadesvm-gui/FullScreenWindow.cpp
