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
        _controlMenu(),
        _showOriginalSize(_controlMenu.addAction("Original size")),
        _showIntegralStretch(_controlMenu.addAction("Intergel stretch")),
        _showFill(_controlMenu.addAction("Fill"))
{
    _ui->setupUi(this);

    _showOriginalSize->setCheckable(true);
    _showIntegralStretch->setCheckable(true);
    _showFill->setCheckable(true);
}

Vds1DisplayWidget::~Vds1DisplayWidget()
{
    Q_ASSERT(_canDestruct);
    delete _ui;
}

//////////
//  QWidget
void Vds1DisplayWidget::paintEvent(QPaintEvent * event)
{
    QRect g = this->geometry();
    qDebug() << g;

    QPainter painter;
    painter.begin(this);
    painter.fillRect(g, Qt::cyan);
    //  TODO kill off QWidget::paintEvent(event);
    painter.end();
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

//  End of hadesvm-cereon/Vds1DisplayWidget.cpp
