//
//  hadesvm-cereon/Fdc1FloppyDriveStatusBarWidget.cpp
//
//  hadesvm::cereon::Fdc1FloppyDriveStatusBarWidget class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;
#include "ui_Fdc1FloppyDriveStatusBarWidget.h"

//////////
//  Construction/destruction
Fdc1FloppyDriveStatusBarWidget::Fdc1FloppyDriveStatusBarWidget(Fdc1FloppyDrive * fdc1FloppyDrive)
    :   hadesvm::core::StatusBarWidget(),
        //  Implementation
        _fdc1FloppyDrive(fdc1FloppyDrive),
        //  Controls & resources
        _ui(new Ui::Fdc1FloppyDriveStatusBarWidget),
        _normalBkColor(this->palette().color(QPalette::ColorRole::Window)),
        _readingBkColor("green"),
        _writingBkColor("red"),
        _connectedFloppyImage(":/hadesvm/cereon/images/objects/FloppyDiskSmall.png"),
        _disconnectedFloppyImage(_connectedFloppyImage.width(), _connectedFloppyImage.height(), QImage::Format::Format_ARGB32),
        _controlMenu(),
        _mountAction(_controlMenu.addAction("Mount")),
        _unmountAction(_controlMenu.addAction("Unmount")),
        _separatorAction(_controlMenu.addSeparator()),
        _selectImageAction(_controlMenu.addAction("Select image")),
        _refreshTimer()
{
    _ui->setupUi(this);

    for (int x = 0; x < _connectedFloppyImage.width(); x++)
    {
        for (int y = 0; y < _connectedFloppyImage.height(); y++)
        {
            QColor c = _connectedFloppyImage.pixelColor(x, y);
            //  NTSC formula: 0.299 ∙ Red + 0.587 ∙ Green + 0.114 ∙ Blue
            int gray = (c.red() * 299 + c.green() * 587 + c.blue() * 114) / 1000;
            _disconnectedFloppyImage.setPixel(x, y, qRgba(gray, gray, gray, c.alpha()));
        }
    }

    //  Prepare to show context menus
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QWidget::customContextMenuRequested,
            this, &Fdc1FloppyDriveStatusBarWidget::_showContextMenu);

    //  Set up control menu signal handlers
    connect(_mountAction, &QAction::triggered,
            this, &Fdc1FloppyDriveStatusBarWidget::_onMountActionTriggered);
    connect(_unmountAction, &QAction::triggered,
            this, &Fdc1FloppyDriveStatusBarWidget::_onUnmountActionTriggered);
    connect(_selectImageAction, &QAction::triggered,
            this, &Fdc1FloppyDriveStatusBarWidget::_onSelectImageActionTriggered);

    //  Start refreshes
    _refreshTimer.setInterval(50);  //  20fps - for now
    connect(&_refreshTimer, &QTimer::timeout, this, &Fdc1FloppyDriveStatusBarWidget::_onRefreshTimerTick);
    _refreshTimer.start();
}

Fdc1FloppyDriveStatusBarWidget::~Fdc1FloppyDriveStatusBarWidget()
{
    _refreshTimer.stop();
    delete _ui;
}

//////////
//  QWidget
void Fdc1FloppyDriveStatusBarWidget::paintEvent(QPaintEvent * event)
{
    hadesvm::core::StatusBarWidget::paintEvent(event);

    QRect cr = this->rect();

    QPainter painter;
    painter.begin(this);

    if (_fdc1FloppyDrive->isWriteInProgress())
    {
        painter.fillRect(cr, _writingBkColor);
    }
    else if (_fdc1FloppyDrive->getMotorStatus() != Fdc1FloppyDrive::MotorStatus::Stopped)
    {
        painter.fillRect(cr, _readingBkColor);
    }
    else
    {
        painter.fillRect(cr, _normalBkColor);
    }

    if (_fdc1FloppyDrive->isFloppyImageMounted())
    {
        painter.drawImage(cr, _connectedFloppyImage);
    }
    else
    {
        painter.drawImage(cr, _disconnectedFloppyImage);
    }

    painter.end();
}

//////////
//  hadesvm::core::StatusBarWidget
QString Fdc1FloppyDriveStatusBarWidget::displayName() const
{
    QString result = _fdc1FloppyDrive->displayName();
    if (_fdc1FloppyDrive->isFloppyImageMounted())
    {
        result += " [" + _fdc1FloppyDrive->diskImagePath() + "]";
    }
    return result;
}

//////////
//  Signal handlers
void Fdc1FloppyDriveStatusBarWidget::_onRefreshTimerTick()
{
    QString oldTooltip = this->toolTip();
    QString newTooltip = this->displayName();
    if (newTooltip != oldTooltip)
    {
        this->setToolTip(newTooltip);
    }
    this->repaint();
}

void Fdc1FloppyDriveStatusBarWidget::_showContextMenu(const QPoint & p)
{
    _mountAction->setEnabled(!_fdc1FloppyDrive->isFloppyImageMounted() &&
                             _fdc1FloppyDrive->diskImagePath().length() > 0);
    _mountAction->setText("Mount " + _fdc1FloppyDrive->diskImagePath());

    _unmountAction->setEnabled(_fdc1FloppyDrive->isFloppyImageMounted());
    _unmountAction->setText("Unmount " + _fdc1FloppyDrive->diskImagePath());

    _selectImageAction->setEnabled(_fdc1FloppyDrive->getMotorStatus() == Fdc1FloppyDrive::MotorStatus::Stopped);

    QPoint origin(p.x(), p.y() - _controlMenu.sizeHint().height());
    _controlMenu.exec(mapToGlobal(origin));
}

void Fdc1FloppyDriveStatusBarWidget::_onMountActionTriggered(bool)
{
    _fdc1FloppyDrive->mountImage(_fdc1FloppyDrive->diskImagePath());
}

void Fdc1FloppyDriveStatusBarWidget::_onUnmountActionTriggered(bool)
{
    _fdc1FloppyDrive->unmountImage();
}

void Fdc1FloppyDriveStatusBarWidget::_onSelectImageActionTriggered(bool)
{
    QString fileName =
        QFileDialog::getOpenFileName(
            this->topLevelWidget(),
            "Select floppy image",
            _fdc1FloppyDrive->virtualAppliance()->directory(),
            "Floppy images (*.flp *.vfd)");
    if (fileName.length() != 0)
    {
        _fdc1FloppyDrive->_diskImagePath = _fdc1FloppyDrive->virtualAppliance()->toRelativePath(fileName);
    }
}

//  End of hadesvm-cereon/Fdc1FloppyDriveStatusBarWidget.cpp
