//
//  hadesvm-cereon/Vds1Controller.VideoSignal.cpp
//
//  hadesvm::cereon::Vds1Controller::_VideoSignal class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;

//////////
//  Construction/destruction
Vds1Controller::_VideoSignal::_VideoSignal(_Compartment * compartment)
    :   _compartment(compartment)
{
    for (unsigned y = 0; y < PixelHeight; y++)
    {
        for (unsigned x = 0; x < PixelWidth; x++)
        {
            _pixels[y][x] = qRgb(0, 0, 0);
        }
    }
}

//////////
//  edp::core::IRasterVideoSignal
QRgb Vds1Controller::_VideoSignal::getPixelColor(unsigned x, unsigned y) const
{
    if (x < PixelWidth && y < PixelHeight)
    {
        const_cast<_VideoSignal*>(this)->_actualize();
        return _pixels[y][x];
    }
    return qRgb(0, 0, 0);
}

void Vds1Controller::_VideoSignal::getPixelColors(unsigned x, unsigned y, unsigned width, unsigned height, QRgb * buffer) const
{
    const_cast<_VideoSignal*>(this)->_actualize();

    if (x == 0 && y == 0 && width == PixelWidth && height == PixelHeight)
    {   //  We rely on QRgb being a POD type
        Q_ASSERT(sizeof(_pixels) == PixelWidth * PixelHeight * sizeof(QRgb));
        memcpy(buffer, _pixels, sizeof(_pixels));
    }
    else
    {
        failure();
        //TODO uncomment IRasterVideoSignal::getPixelColors(x, y, width, height, buffer);
    }
}

//////////
//  Operations
void Vds1Controller::_VideoSignal::setPixelColor(unsigned x, unsigned y, QRgb color)
{
    Q_ASSERT(qAlpha(color) == 255);

    if (x < PixelWidth && y < PixelHeight)
    {
        _pixels[y][x] = color;
    }
}

void Vds1Controller::_VideoSignal::clear(QRgb color)
{
    for (unsigned y = 0; y < PixelHeight; y++)
    {
        for (unsigned x = 0; x < PixelWidth; x++)
        {
            _pixels[y][x] = color;
        }
    }
}

//////////
//  Implementation helpers
void Vds1Controller::_VideoSignal::_actualize()
{
    if (_compartment->_videoSignalNeedsRefreshing)
    {
        _compartment->_regenerateVideoSignal();
        _compartment->_videoSignalNeedsRefreshing = false;
    }
}

//  End of hadesvm-cereon/Vds1Controller.VideoSignal.cpp
