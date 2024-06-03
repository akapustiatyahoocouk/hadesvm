//
//  hadesvm-kernel/Kernel.cpp
//
//  hadesvm::kernel::Kernel class implementation
//
//////////
#include "hadesvm-kernel/API.hpp"
using namespace hadesvm::kernel;

//////////
//  Construction/destruction
Kernel::Kernel(const QString & nodeName)
    :   _guard(),
        //  Configuration
        _nodeName(nodeName),
        _mountedFolders(),
        //  Runtime state
        _objects()
{
    Q_ASSERT(isValidNodeName(_nodeName));
}

Kernel::~Kernel()
{
}

//////////
//  hadesvm::core::Component
Kernel::Type * Kernel::type() const
{
    return Type::instance();
}

QString Kernel::displayName() const
{
    return Type::instance()->displayName();
}

void Kernel::serialiseConfiguration(QDomElement componentElement)
{
}

void Kernel::deserialiseConfiguration(QDomElement componentElement)
{
}

hadesvm::core::ComponentEditor * Kernel::createEditor(QWidget * parent)
{
    return new KernelEditor(parent, this);
}

//////////
//  Operations (configuration)
void Kernel::setNodeName(const QString & nodeName)
{
    Q_ASSERT(state() == State::Constructed);

    if (isValidNodeName(nodeName))
    {
        _nodeName = nodeName;
    }
}

QStringList Kernel::mountedFolderVolumeNames() const
{
    return QStringList(_mountedFolders.keys());
}

QString Kernel::mountedFolderPath(const QString & volumeName) const
{
    return _mountedFolders.contains(volumeName) ? _mountedFolders[volumeName] : "";
}

void Kernel::setMountedFolderPath(const QString & volumeName, const QString & path)
{
    Q_ASSERT(state() == State::Constructed);

    if (isValidVolumeName(volumeName))
    {
        _mountedFolders[volumeName] = path;
    }
}

void Kernel::removeMountedFolder(const QString & volumeName)
{
    Q_ASSERT(state() == State::Constructed);

    _mountedFolders.remove(volumeName);
}

//////////
//  Operations (validation)
bool Kernel::isValidNodeName(const QString & name)
{
    return isValidVolumeName(name);
}

bool Kernel::isValidVolumeName(const QString & name)
{
    if (name.length() == 0 || name.length() > 64)
    {
        return false;
    }
    for (QChar c : name)
    {
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
              (c >= '0' && c <= '9') || (c == '_' || c == '.')))
        {
            return false;
        }
    }
    if (name.startsWith(".") || name.endsWith("."))
    {
        return false;
    }
    return true;
}

//  End of hadesvm-kernel/Kernel.cpp
