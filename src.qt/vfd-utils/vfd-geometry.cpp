//
//  vfd-geometry.cpp - VfdGeometry implementation
//
//////////
#include "main.hpp"

//////////
//  Construction/destruction/assignment
VfdGeometry::VfdGeometry()
    :   _cylinders(0),
        _heads(0),
        _sectors(0),
        _sectorSize(0)
{
}

VfdGeometry::VfdGeometry(unsigned int cylinders, unsigned int heads, unsigned int sectors, unsigned int sectorSize)
    :   _cylinders(cylinders),
        _heads(heads),
        _sectors(sectors),
        _sectorSize(sectorSize)
{
    assert(_cylinders > 0);
    assert(_heads > 0);
    assert(_sectors > 0);
    assert(_sectorSize > 0);
}

VfdGeometry::VfdGeometry(const VfdGeometry & src)
    :   _cylinders(src._cylinders),
        _heads(src._heads),
        _sectors(src._sectors),
        _sectorSize(src._sectorSize)
{
}

VfdGeometry & VfdGeometry::operator = (const VfdGeometry & src)
{
    _cylinders = src._cylinders;
    _heads = src._heads;
    _sectors = src._sectors;
    _sectorSize = src._sectorSize;
    return *this;
}

//////////
//  Operations
VfdGeometry VfdGeometry::forFormat(VfdFormat vfdFormat)
{
    switch (vfdFormat)
    {
        case VfdFormat_144M:
            return VfdGeometry(80, 2, 18, 512);
        case VfdFormat_Unknown:
            return VfdGeometry();
        default:
            assert(false);
            return VfdGeometry();
    }
}

//  End of vfd-geometry
