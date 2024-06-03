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
Kernel::Kernel()
    :   _guard(),
        //  Configuration
        _nodeUuid(QUuid::createUuid()),
        _nodeName(QHostInfo::localHostName()),
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
    componentElement.setAttribute("UUID", _nodeUuid.toString());
    componentElement.setAttribute("Name", _nodeName);

    for (QString volumeName : _mountedFolders.keys())
    {
        componentElement.setAttribute("MountedFolder." + volumeName, _mountedFolders[volumeName]);
    }
}

void Kernel::deserialiseConfiguration(QDomElement componentElement)
{
    QUuid uuid(componentElement.attribute("UUID"));
    if (!uuid.isNull())
    {
        _nodeUuid = uuid;
    }

    QString nodeName = componentElement.attribute("Name");
    if (isValidNodeName(nodeName))
    {
        _nodeName = nodeName;
    }

    _mountedFolders.clear();
    QDomNamedNodeMap attributes = componentElement.attributes();
    for (int i = 0; i < attributes.count(); i++)
    {
        QDomAttr attribute = attributes.item(i).toAttr();
        if (!attribute.isNull() && attribute.name().startsWith("MountedFolder."))
        {
            QString volumeName = attribute.name().mid(14);
            QString path = attribute.value();
            if (isValidVolumeName(volumeName))
            {
                _mountedFolders[volumeName] = path;
            }
        }
    }
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
              (c >= '0' && c <= '9') || (c == '.' || c == '-')))
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
