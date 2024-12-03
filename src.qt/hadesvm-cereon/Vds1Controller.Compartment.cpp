//
//  hadesvm-cereon/Vds1Controller.Compartment.cpp
//
//  hadesvm::cereon::Vds1Controller::_Compartment class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;

//////////
//  Construction/destruction
Vds1Controller::_Compartment::_Compartment(uint8_t number)
    :   _number(number),
        _videoMode(_Text32x24VideoMode),
        _page(0),
        _source(0),
        _destination(0),
        _length(0),
        _videoMemory(),
        _display(nullptr),
        _videoSignal(this),
        //  Resources
        _colorTable16
        {
            qRgb(0x00, 0x00, 0x00),
            qRgb(0x00, 0x00, 0x80),
            qRgb(0x00, 0x80, 0x00),
            qRgb(0x00, 0x80, 0x80),
            qRgb(0x80, 0x00, 0x00),
            qRgb(0x80, 0x00, 0x80),
            qRgb(0x80, 0x80, 0x00),
            qRgb(0xC0, 0xC0, 0xC0),
            qRgb(0x80, 0x80, 0x80),
            qRgb(0x00, 0x00, 0xFF),
            qRgb(0x00, 0xFF, 0x00),
            qRgb(0x00, 0xFF, 0xFF),
            qRgb(0xFF, 0x00, 0x00),
            qRgb(0xFF, 0x00, 0xFF),
            qRgb(0xFF, 0xFF, 0x00),
            qRgb(0xFF, 0xFF, 0xFF)
        }
{
    _regenerateVideoSignal();
}

//////////
//  Operations
void Vds1Controller::_Compartment::attachDisplay(Vds1Display * display)
{
    Q_ASSERT(display != nullptr);

    detachDisplay();    //  in case one was attached
    _display = display;
    Q_ASSERT(_display->_videoSignal == nullptr);
    _display->_videoSignal = &_videoSignal;
}

void Vds1Controller::_Compartment::detachDisplay()
{
    if (_display != nullptr)
    {   //  Detach
        Q_ASSERT(_display->_videoSignal == &_videoSignal);
        _display->_videoSignal = nullptr;
        _display = nullptr;
    }
}

void Vds1Controller::_Compartment::setVideoMode(uint8_t videoMode)
{
    if (videoMode <= 6)
    {   //  Valid
        _videoMode = videoMode;
        _page = 0;
        _regenerateVideoSignal();
    }
}

void Vds1Controller::_Compartment::setSource(uint16_t source)
{
    _source = source;
}

void Vds1Controller::_Compartment::setDestination(uint16_t destination)
{
    _destination = destination;
}

void Vds1Controller::_Compartment::setLength(uint16_t length)
{
    _length = length;
}

void Vds1Controller::_Compartment::reset()
{
    _videoMode = _Text32x24VideoMode;
    _page = 0;
    _source = 0;
    _destination = 0;
    _length = 0;
    memset(_videoMemory, 0, sizeof(_videoMemory));
    _regenerateVideoSignal();
}

uint8_t Vds1Controller::_Compartment::read()
{
    return _videoMemory[_source];
}

void Vds1Controller::_Compartment::write(uint8_t value)
{
    if (_videoMemory[_destination] != value)
    {
        _videoMemory[_destination] = value;
        _videoSignalNeedsRefreshing = true; //  TODO optimize - not every write changes the video signal
    }
}

void Vds1Controller::_Compartment::writeAndAdvance(uint8_t value)
{
    if (_videoMemory[_destination] != value)
    {
        _videoMemory[_destination] = value;
        _videoSignalNeedsRefreshing = true; //  TODO optimize - not every write changes the video signal
    }
    _destination++;
}

void Vds1Controller::_Compartment::move()
{
    uint16_t dst = _destination,
             src = _source;
    uint32_t cnt = static_cast<uint32_t>(_length) + 1;
    for (; cnt != 0; cnt--, dst++, src++)
    {
        if (_videoMemory[dst] != _videoMemory[src])
        {
            _videoMemory[dst] = _videoMemory[src];
            _videoSignalNeedsRefreshing = true; //  TODO optimize - not every write changes the video signal
        }
    }
}

//////////
//  Helpers
void Vds1Controller::_Compartment::_regenerateVideoSignal()
{
    switch (_videoMode)
    {
    case _Text32x24VideoMode:
        _regenerateText32x24VideoSignal();
        break;
    case _Text64x24VideoMode:
        _regenerateText64x24VideoSignal();
        break;
    case _Text40x24VideoMode:
        failure_with_message("Not yet implemented");
        break;
    case _Text80x24VideoMode:
        failure_with_message("Not yet implemented");
        break;
    case _Graph256x192VideoMode:
        failure_with_message("Not yet implemented");
        break;
    case _Graph512x192VideoMode:
        failure_with_message("Not yet implemented");
        break;
    case _Graph512x384VideoMode:
        failure_with_message("Not yet implemented");
        break;
    default:
        failure();
    }
}

void Vds1Controller::_Compartment::_regenerateText32x24VideoSignal()
{
    unsigned pageStartOffset = _page * 2048u;
    unsigned cellOffset = pageStartOffset;

    for (unsigned cy = 0; cy < 24; cy++)
    {
        for (unsigned cx = 0; cx < 32; cx++)
        {
            unsigned charCode = _videoMemory[cellOffset++];
            unsigned attr = _videoMemory[cellOffset++];
            unsigned foreColor = (attr & 0x0F),
                     bkColor = (attr >> 4);
            //  Where's out glyph ?
            const uint8_t * glyph = _videoMemory + (65536 - 8 * 256) + 8 * charCode;
            //  Generate pixels
            unsigned px = 16 * cx,
                py = 16 * cy;
            for (unsigned dy = 0; dy < 8; dy++)
            {
                uint8_t glyphLine = *(glyph++);
                for (unsigned dx = 0; dx < 8; dx++, glyphLine <<= 1)
                {
                    QRgb color = _colorTable16[(glyphLine & 0x80) ? foreColor : bkColor];
                    _videoSignal.setPixelColor(px + 2 * dx + 0, py + 2 * dy + 0, color);
                    _videoSignal.setPixelColor(px + 2 * dx + 0, py + 2 * dy + 1, color);
                    _videoSignal.setPixelColor(px + 2 * dx + 1, py + 2 * dy + 0, color);
                    _videoSignal.setPixelColor(px + 2 * dx + 1, py + 2 * dy + 1, color);
                }
            }
        }
    }
}

void Vds1Controller::_Compartment::_regenerateText64x24VideoSignal()
{
    unsigned pageStartOffset = _page * 4096u;
    unsigned cellOffset = pageStartOffset;

    for (unsigned cy = 0; cy < 24; cy++)
    {
        for (unsigned cx = 0; cx < 64; cx++)
        {
            unsigned charCode = _videoMemory[cellOffset++];
            unsigned attr = _videoMemory[cellOffset++];
            unsigned foreColor = (attr & 0x0F),
                     bkColor = (attr >> 4);
            //  Where's out glyph ?
            const uint8_t * glyph = _videoMemory + (65536 - 8 * 256) + 8 * charCode;
            //  Generate pixels
            unsigned px = 8 * cx,
                py = 16 * cy;
            for (unsigned dy = 0; dy < 8; dy++)
            {
                uint8_t glyphLine = *(glyph++);
                for (unsigned dx = 0; dx < 8; dx++, glyphLine <<= 1)
                {
                    QRgb color = _colorTable16[(glyphLine & 0x80) ? foreColor : bkColor];
                    _videoSignal.setPixelColor(px + dx, py + 2 * dy + 0, color);
                    _videoSignal.setPixelColor(px + dx, py + 2 * dy + 1, color);
                }
            }
        }
    }
}

//  End of hadesvm-cereon/Vds1Controller.Compartment.cpp
