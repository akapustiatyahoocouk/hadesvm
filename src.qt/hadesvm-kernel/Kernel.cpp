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
    :   //  State
        _state(State::Constructed),
        //  Configuration
        _nodeUuid(QUuid::createUuid()),
        _nodeName(QHostInfo::localHostName()),
        _mountedFolders(),
        //  Runtime state
        _runtimeStateGuard(),
        //  Primary object maps
        _oidGenerator(),
        _liveObjects(),
        _deadObjects(),
        //  Secondary object maps
        _localNode(nullptr),
        _deviceManagerProcess(nullptr)
{
    Q_ASSERT(isValidNodeName(_nodeName));
}

Kernel::~Kernel()
{
    for (Object * object : _liveObjects.values())
    {
        object->_referenceCount = 0;    //  else "delete" will assert
        delete object;
    }
    for (Object * object : _deadObjects.values())
    {
        object->_referenceCount = 0;    //  else "delete" will assert
        delete object;
    }
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
//  hadesvm::core::Component (state management)
Kernel::State Kernel::state() const noexcept
{
    return _state;
}

void Kernel::connect() throws(hadesvm::core::VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Constructed)
    {   //  OOPS!
        throw hadesvm::core::VirtualApplianceException(displayName() + " is not in Constructed state");
    }

    //  TODO discover devices that Kernel will manage

    //  Done
    _state = State::Connected;
}

void Kernel::initialize() throws(hadesvm::core::VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Connected)
    {   //  OOPS!
        throw hadesvm::core::VirtualApplianceException(displayName() + " is not in Connected state");
    }

    QMutexLocker lock(&_runtimeStateGuard);

    //  Start with the local node
    _localNode = new LocalNode(this, _nodeUuid, _nodeName);
    _localNode->incrementReferenceCount();  //  we've just created a new reference to "_localNode"

    //  Create Device objects representing mounted folders
    for (auto [volumeName, path] : _mountedFolders.asKeyValueRange())
    {
        QDir baseDir(virtualAppliance()->directory());
        path = QFileInfo(baseDir, path).canonicalFilePath();
        if (_localNode->findDeviceByName(volumeName) != nullptr)
        {   //  OOPS!
            throw hadesvm::core::VirtualApplianceException(
                "Device '" + volumeName +
                "' already exists for node '" + _localNode->name() + "'");
        }
    }

    //  TODO

    //  Create Device objects representing physical devices
    for (IDeviceComponent * deviceComponent : virtualAppliance()->componentsImplementing<IDeviceComponent>())
    {
        if (_localNode->findDeviceByName(deviceComponent->deviceName()) != nullptr)
        {   //  OOPS!
            throw hadesvm::core::VirtualApplianceException(
                "Device '" + deviceComponent->deviceName() +
                "' already exists for node '" + _localNode->name() + "'");
        }
        new PhysicalDevice(this, _localNode, deviceComponent->deviceName(), deviceComponent);
    }

    //  Create "device manager" system process


    //  TODO implement
    _deviceManagerProcess = new Process(this, nullptr, "device manager");
    _deviceManagerProcess->incrementReferenceCount();   //  we've just created a new reference to "_localNode"
    new DeviceManagerMainThread(this, _deviceManagerProcess);

    //  Done
    _state = State::Initialized;
}

void Kernel::start() throws(hadesvm::core::VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Initialized)
    {   //  OOPS!
        throw hadesvm::core::VirtualApplianceException(displayName() + " is not in Initialized state");
    }
    //  TODO implement
}

void Kernel::stop() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Running)
    {   //  Nothing to do
        return;
    }
    //  TODO implement
}

void Kernel::deinitialize() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Initialized)
    {   //  Nothing to do
        return;
    }

    delete _localNode;
    _localNode = nullptr;
    //  TODO implement
}

void Kernel::disconnect() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Connected)
    {   //  Nothing to do
        return;
    }
    //  TODO implement
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
//  Operations (Node)
Node * Kernel::findNodeByUuid(const QUuid & uuid) const
{
    Q_ASSERT(isLockedByCurrentThread());

    return _nodesByUuid.contains(uuid) ? _nodesByUuid[uuid] : nullptr;
}

//////////
//  Operations (validation)
bool Kernel::isValidNodeName(const QString & name)
{
    return isValidVolumeName(name); //  for now
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

bool Kernel::isValidDeviceName(const QString & name)
{
    return isValidVolumeName(name); //  for now
}

//////////
//  Operations (runtime state)
bool Kernel::isLockedByCurrentThread() const
{
    return _runtimeStateGuard.lockingThread() == QThread::currentThread();
}

//////////
//  Implementation helpers
Oid Kernel::_generateUniqueOid()
{
    Q_ASSERT(isLockedByCurrentThread());

    for (; ; )
    {
        Oid oid = _oidGenerator.generate();
        if (oid != 0 && !_liveObjects.contains(oid) && !_deadObjects.contains(oid))
        {
            return oid;
        }
    }
}

//  End of hadesvm-kernel/Kernel.cpp
