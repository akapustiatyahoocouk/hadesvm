//
//  hadesvm-core/VirtualAppliance.cpp
//
//  hadesvm::core::VirtualAppliance class implementation
//
//////////
#include "hadesvm-core/API.hpp"
using namespace hadesvm::core;

namespace
{
    const QString VaDirectoryPrefix = "./";
}

//////////
//  Constants
const QString VirtualAppliance::PreferredExtension = ".hadesvm";

//////////
//  Construction/destruction
VirtualAppliance::VirtualAppliance(const QString & name, const QString & location,
                                   VirtualArchitecture * architecture)
    :   _state(State::Stopped),
        //  Configuration
        _name(name),
        _location(QFileInfo(location).absoluteFilePath()),
        _directory(QFileInfo(_location).absolutePath()),
        _architecture(architecture),
        _startAutomatically(false),
        _startFullScreen(false),
        _startMinimized(false),
        //  VA components
        _compatibleComponents(),
        _adaptedComponents(),
        _componentAdaptors(),
        //  Runtime state
        _stopRequested(false),
        _resetRequested(false),
        //  Runtime statistics
        _statisticsGuard(),
        _achievedClockFrequencyByClockedComponent()
{
    Q_ASSERT(_architecture != nullptr);
    _resetStatistics();
}

VirtualAppliance::~VirtualAppliance()
{
    //  Release and destroy adapted components
    while (!_adaptedComponents.isEmpty())
    {
        Component * component = _adaptedComponents[0];
        removeComponent(component); //  also destroys the adaptor
        delete component;
    }
    //  Release and destroy compatible components
    while (!_compatibleComponents.isEmpty())
    {
        Component * component = _compatibleComponents[0];
        removeComponent(component);
        delete component;
    }
}

//////////
//  Operations (general)
void VirtualAppliance::save() throws(VirtualApplianceException)
{
    //  Create & set up DOM document
    QDomDocument document;
    QDomProcessingInstruction pi = document.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"utf-8\"");
    document.appendChild(pi);

    //  Set up root element
    QDomElement rootElement = document.createElement(type()->mnemonic());
    rootElement.setAttribute("Name", name());
    rootElement.setAttribute("Architecture", architecture()->mnemonic());
    rootElement.setAttribute("Version", "1");
    rootElement.setAttribute("StartAutomatically", hadesvm::util::toString(_startAutomatically));
    rootElement.setAttribute("StartFullScreen", hadesvm::util::toString(_startFullScreen));
    rootElement.setAttribute("StartMinimized", hadesvm::util::toString(_startMinimized));
    document.appendChild(rootElement);

    //  Set up "components" element
    QDomElement componentsElement = document.createElement("Components");
    rootElement.appendChild(componentsElement);

    //  Set up component XML nodes
    for (auto component : _compatibleComponents)
    {
        QDomElement componentElement = document.createElement("Component");
        componentsElement.appendChild(componentElement);
        componentElement.setAttribute("Type", component->componentType()->mnemonic());
        component->serialiseConfiguration(componentElement);
        Q_ASSERT(componentElement.attribute("Type") == component->componentType()->mnemonic());
    }
    for (auto componentAdaptor : _componentAdaptors)
    {
        auto component = componentAdaptor->adaptedComponent();
        QDomElement componentElement = document.createElement("Component");
        componentsElement.appendChild(componentElement);
        componentElement.setAttribute("Type", component->componentType()->mnemonic());
        component->serialiseConfiguration(componentElement);
        Q_ASSERT(componentElement.attribute("Type") == component->componentType()->mnemonic());

        QDomElement adaptorElement = document.createElement("Adaptor");
        componentElement.appendChild(adaptorElement);
        adaptorElement.setAttribute("Type", componentAdaptor->componentAdaptorType()->mnemonic());
        componentAdaptor->serialiseConfiguration(adaptorElement);
        Q_ASSERT(adaptorElement.attribute("Type") == componentAdaptor->componentAdaptorType()->mnemonic());
    }

    //  Save!
    QFile file(location());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        throw VirtualApplianceException(file.errorString());
    }
    QTextStream stream(&file);
    stream << document.toString();
    file.close();
}

VirtualAppliance * VirtualAppliance::load(const QString & location)
{
    QFileInfo fi(location);

    //  Load as XML DOM
    QDomDocument document;
    QFile file(fi.absoluteFilePath());
    if (!file.open(QIODevice::ReadOnly))
    {   //  OOPS!
        throw VirtualApplianceException(file.errorString());
    }
    if (!document.setContent(&file))
    {   //  OOPS!
        file.close();
        throw VirtualApplianceException(file.errorString());
    }
    file.close();

    //  Validate root XML element
    QDomElement rootElement = document.documentElement();
    VirtualApplianceType * type = nullptr;
    for (VirtualApplianceType * vat : VirtualApplianceType::all())
    {
        if (vat->mnemonic() == rootElement.tagName())
        {
            type = vat;
            break;
        }
    }
    if (type == nullptr)
    {
        throw VirtualApplianceException("Invalid virtual appliance type");
    }

    QString name = rootElement.attribute("Name");
    if (!isValidName(name))
    {
        throw VirtualApplianceException("Invalid virtual appliance name");
    }

    QString version = rootElement.attribute("Version");
    if (version != "1")
    {
        throw VirtualApplianceException("Invalid virtual appliance version");
    }

    VirtualArchitecture * architecture = nullptr;
    for (VirtualArchitecture * va : VirtualArchitecture::all())
    {
        if (va->mnemonic() == rootElement.attribute("Architecture"))
        {
            architecture = va;
            break;
        }
    }
    if (architecture == nullptr)
    {
        throw VirtualApplianceException("Invalid virtual appliance architecture");
    }

    //  Create an initially empty VA
    std::unique_ptr<VirtualAppliance> va{type->createVirtualAppliance(name, fi.absoluteFilePath(), architecture)};
    hadesvm::util::fromString(rootElement.attribute("StartAutomatically"), va->_startAutomatically);
    hadesvm::util::fromString(rootElement.attribute("StartFullScreen"), va->_startFullScreen);
    hadesvm::util::fromString(rootElement.attribute("StartMinimized"), va->_startMinimized);

    //  Process <Components> element
    for (QDomElement componentsElement = rootElement.firstChildElement("Components");
         !componentsElement.isNull();
         componentsElement = componentsElement.nextSiblingElement("Components"))
    {
        for (QDomElement componentElement = componentsElement.firstChildElement("Component");
             !componentElement.isNull();
             componentElement = componentElement.nextSiblingElement("Component"))
        {   //  Create component...
            QString componentTypeMnemonic = componentElement.attribute("Type");
            ComponentType * componentType = ComponentType::findByMnemonic(componentTypeMnemonic);
            if (componentType == nullptr)
            {
                throw VirtualApplianceException("Unsupported component type '" + componentTypeMnemonic + "'");
            }
            Component * component = componentType->createComponent();
            component->deserialiseConfiguration(componentElement);
            va->addComponent(component);
            //  Adaptor + properties ?
            ComponentAdaptor * adaptor = nullptr;
            for (auto ca : va->componentAdaptors())
            {
                if (ca->adaptedComponent() == component)
                {
                    adaptor = ca;
                    break;
                }
            }
            if (adaptor != nullptr)
            {
                QDomElement adaptorElement = componentElement.firstChildElement("Adaptor");
                if (!adaptorElement.isNull() && adaptorElement.attribute("Type") == adaptor->componentAdaptorType()->mnemonic())
                {
                    adaptor->deserialiseConfiguration(adaptorElement);
                }
            }
        }
    }

    //  Done
    return va.release();
}

//////////
//  Operations (configuration)
bool VirtualAppliance::isValidName(const QString & name)
{
    return name.length() > 0 &&
           name.length() <= 128 &&
           name.length() == name.trimmed().length();
}

bool VirtualAppliance::isValidLocation(const QString & location)
{
    return location.length() > 0 &&
           QFileInfo(location).absoluteFilePath() == location;
}

QString VirtualAppliance::name() const
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    return _name;
}

void VirtualAppliance::setName(const QString & name)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (isValidName(name))
    {
        _name = name;
    }
}

QString VirtualAppliance::location() const
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    return _location;
}

QString VirtualAppliance::directory() const
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    return _directory;
}

VirtualArchitecture * VirtualAppliance::architecture() const
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    return _architecture;
}

bool VirtualAppliance::startAutomatically() const
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    return _startAutomatically;
}

void VirtualAppliance::setStartAutomatically(bool startAutomatically)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    _startAutomatically = startAutomatically;
}

bool VirtualAppliance::startFullScreen() const
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    return _startFullScreen;
}

void VirtualAppliance::setStartFullScreen(bool startFullScreen)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    _startFullScreen = startFullScreen;
    if (_startFullScreen)
    {
        _startMinimized = false;
    }
}

bool VirtualAppliance::startMinimized() const
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    return _startMinimized;
}

void VirtualAppliance::setStartMinimized(bool startMinimized)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    _startMinimized = startMinimized;
    if (_startMinimized)
    {
        _startFullScreen = false;
    }
}

QString VirtualAppliance::toRelativePath(const QString & path)
{
    QDir baseDir(_directory);
    QString relativePath = baseDir.relativeFilePath(path);
    if (!relativePath.contains("/") && !relativePath.contains("\\") && relativePath != ".")
    {
        relativePath = VaDirectoryPrefix + relativePath;
    }
    else if (relativePath.contains("..") && QFileInfo(path).isAbsolute())
    {
        relativePath = path;
    }
    qDebug() << path << " -> " << relativePath;   //  TODO kill off
    return relativePath;
}

QString VirtualAppliance::toAbsolutePath(const QString & path)
{
    QDir baseDir(_directory);

    QString absolutePath = path;
    if (absolutePath == ".")
    {
        absolutePath = _directory;
    }
    else if (absolutePath.startsWith(VaDirectoryPrefix))
    {
        absolutePath = absolutePath.mid(VaDirectoryPrefix.length());
    }
    absolutePath = baseDir.absoluteFilePath(absolutePath);

    qDebug() << path << " -> " << absolutePath;   //  TODO kill off
    return absolutePath;
}

//////////
//  Operations (VA components)
ComponentList VirtualAppliance::compatibleComponents() const
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    return _compatibleComponents;
}

ComponentList VirtualAppliance::adaptedComponents() const
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    return _adaptedComponents;
}

ComponentList VirtualAppliance::components() const
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    ComponentList result;
    result.append(_compatibleComponents);
    result.append(_adaptedComponents);
    return result;
}

ComponentAdaptorList VirtualAppliance::componentAdaptors() const
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    return _componentAdaptors;
}

void VirtualAppliance::addComponent(Component * component) throws(VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());
    Q_ASSERT(component != nullptr);
    Q_ASSERT(component->virtualAppliance() == nullptr);

    //  Ensure component is suitable for this VA
    if (!component->componentType()->isCompatibleWith(_architecture) &&
        !component->componentType()->isAdaptableTo(_architecture))
    {
        throw VirtualApplianceException("Component incompatible with virtual appliance architecture");
    }
    if (!component->componentType()->isCompatibleWith(type()))
    {
        throw VirtualApplianceException("Component incompatible with virtual appliance type");
    }
    //  ...and add the component directly to this VA or adapt it
    if (component->componentType()->isCompatibleWith(_architecture))
    {   //  Add directly
        _compatibleComponents.append(component);
    }
    else
    {   //  Adapt
        _adaptedComponents.append(component);
        ComponentAdaptorType * adaptorType = ComponentAdaptorType::find(component->componentType(), _architecture);
        Q_ASSERT(adaptorType != nullptr);
        ComponentAdaptor * adaptor = adaptorType->createComponentAdaptor(component);
        Q_ASSERT(adaptor != nullptr);
        _componentAdaptors.append(adaptor);
        adaptor->_virtualAppliance = this;
    }
    component->_virtualAppliance = this;
}

void VirtualAppliance::removeComponent(Component * component)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());
    Q_ASSERT(component != nullptr);

    if (_compatibleComponents.contains(component))
    {
        Q_ASSERT(component->virtualAppliance() == this);
        _compatibleComponents.removeOne(component);
        component->_virtualAppliance = nullptr;
    }
    else if (_adaptedComponents.contains(component))
    {
        Q_ASSERT(component->virtualAppliance() == this);
        ComponentAdaptor * adaptor = nullptr;
        for (auto ca : _componentAdaptors)
        {
            if (ca->adaptedComponent() == component)
            {
                adaptor = ca;
                break;
            }
        }
        Q_ASSERT(adaptor != nullptr);
        //  Release the adaptor...
        _componentAdaptors.removeOne(adaptor);
        adaptor->_virtualAppliance = nullptr;
        delete adaptor;
        //  ...and the component
        _adaptedComponents.removeOne(component);
        component->_virtualAppliance = nullptr;
    }
    else
    {
        Q_ASSERT(component->virtualAppliance() != this);
    }
}

//////////
//  Operations (state management)
VirtualAppliance::State VirtualAppliance::state() const
{
    return _state;
}

void VirtualAppliance::testConfiguration() throws(VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Stopped)
    {   //  Can't
        return;
    }

    try
    {
        _connectComponents();       //  may throw
        _initializeComponents();    //  may throw

        architecture()->validateVirtualAppliance(this); //  may throw

        _deinitializeComponents();
        _disconnectComponents();
    }
    catch (...)
    {
        _deinitializeComponents();
        _disconnectComponents();
        throw;
    }
}

void VirtualAppliance::start() throws(VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Stopped)
    {   //  Can't
        return;
    }

    try
    {
        _stopRequested = false;
        _resetRequested = false;

        _connectComponents();       //  may throw
        _initializeComponents();    //  may throw
        _startComponents();         //  may throw

        _resetStatistics();

        _workerThread = new _WorkerThread(this);
        _workerThread->start(); //  may choose to stop prematurely!

        _state = State::Running;
    }
    catch (...)
    {
        _stopComponents();
        _deinitializeComponents();
        _disconnectComponents();
        throw;
    }
}

void VirtualAppliance::stop() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    _stopRequested = false;
    _resetRequested = false;

    switch (_state)
    {
        case  State::Stopped:
            //  Nothing to do
            break;
        case State::Suspended:
            //  TODO implement
            failure();
            break;
        case State::Running:
            _workerThread->requestStop();
            _workerThread->wait(15 * 1000); //  wait 15 seconds...
            if (_workerThread->isRunning())
            {   //  ...then force-kill it as a last resort
                _workerThread->terminate();
                _workerThread->wait(ULONG_MAX);
            }
            delete _workerThread;
            _workerThread = nullptr;

            _stopComponents();
            _deinitializeComponents();
            _disconnectComponents();
            _state = State::Stopped;
            try
            {   //  While VA was running, some floppy images may have been
                //  mounted, optional devices connected, etc. - so save VA
                //  configuration
                save();
            }
            catch (...)
            {   //  OOPS! Suppress, though TODO but log ?
            }
            break;
        default:
            failure();
    }
}

void VirtualAppliance::suspend() throws(VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    throw VirtualApplianceException("Not yet implemented");
}

void VirtualAppliance::resume() throws(VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    throw VirtualApplianceException("Not yet implemented");
}

void VirtualAppliance::reset()
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    QMessageBox::critical(nullptr, "OOPS!", "Not yet implemented");    //  reset() must NOT throw!
}

//////////
//  Operations (runtime state)
void VirtualAppliance::requestStop()
{
    _stopRequested = true;
}

void VirtualAppliance::requestReset()
{
    _resetRequested = true;
}

//////////
//  Operations (runtime statistics)
void VirtualAppliance::recordAchievedClockFrequency(IClockedComponent * component, const ClockFrequency & clockFrequency)
{
    Q_ASSERT(component != nullptr);

    QMutexLocker lock(&_statisticsGuard);
    if (component->virtualAppliance() == this)
    {
        _achievedClockFrequencyByClockedComponent[component] = clockFrequency;
        /*  TODO kill off - this is debug code
        qDebug() << component->displayName()
                 << " is running at "
                 << clockFrequency.displayForm();
        */
    }
}

void VirtualAppliance::getRuntimeStatistics(QMap<IClockedComponent*, ClockFrequency> & achievedClockFrequencyByClockedComponent)
{
    QMutexLocker lock(&_statisticsGuard);

    achievedClockFrequencyByClockedComponent.clear();
    achievedClockFrequencyByClockedComponent.insert(_achievedClockFrequencyByClockedComponent);
}

//////////
//  Implementation helpers
void VirtualAppliance::_connectComponents() throws(VirtualApplianceException)
{
    try
    {
        for (auto component : _compatibleComponents)
        {
            Q_ASSERT(component->state() == Component::State::Constructed);
            component->connect();
            Q_ASSERT(component->state() == Component::State::Connected);
        }
        for (auto component : _adaptedComponents)
        {
            Q_ASSERT(component->state() == Component::State::Constructed);
            component->connect();
            Q_ASSERT(component->state() == Component::State::Connected);
        }
        for (auto componentAdaptor : _componentAdaptors)
        {
            Q_ASSERT(componentAdaptor->state() == ComponentAdaptor::State::Constructed);
            componentAdaptor->connect();
            Q_ASSERT(componentAdaptor->state() == ComponentAdaptor::State::Connected);
        }
    }
    catch (...)
    {
        for (auto componentAdaptor : _componentAdaptors)
        {
            if (componentAdaptor->state() == ComponentAdaptor::State::Connected)
            {
                componentAdaptor->disconnect();
                Q_ASSERT(componentAdaptor->state() == ComponentAdaptor::State::Constructed);
            }
        }
        for (auto component : _adaptedComponents)
        {
            if (component->state() == Component::State::Connected)
            {
                component->disconnect();
                Q_ASSERT(component->state() == Component::State::Constructed);
            }
        }
        for (auto component : _compatibleComponents)
        {
            if (component->state() == Component::State::Connected)
            {
                component->disconnect();
                Q_ASSERT(component->state() == Component::State::Constructed);
            }
        }
        throw;
    }
}

void VirtualAppliance::_initializeComponents() throws(VirtualApplianceException)
{
    try
    {
        for (auto component : _compatibleComponents)
        {
            Q_ASSERT(component->state() == Component::State::Connected);
            qDebug() << "Initializing " << component->displayName();
            component->initialize();
            qDebug() << "Done initializing " << component->displayName();
            Q_ASSERT(component->state() == Component::State::Initialized);
        }
        for (auto component : _adaptedComponents)
        {
            Q_ASSERT(component->state() == Component::State::Connected);
            component->initialize();
            Q_ASSERT(component->state() == Component::State::Initialized);
        }
        for (auto componentAdaptor : _componentAdaptors)
        {
            Q_ASSERT(componentAdaptor->state() == ComponentAdaptor::State::Connected);
            componentAdaptor->initialize();
            Q_ASSERT(componentAdaptor->state() == ComponentAdaptor::State::Initialized);
        }
    }
    catch (...)
    {
        for (auto componentAdaptor : _componentAdaptors)
        {
            if (componentAdaptor->state() == ComponentAdaptor::State::Initialized)
            {
                componentAdaptor->deinitialize();
                Q_ASSERT(componentAdaptor->state() == ComponentAdaptor::State::Connected);
            }
        }
        for (auto component : _adaptedComponents)
        {
            if (component->state() == Component::State::Initialized)
            {
                component->deinitialize();
                Q_ASSERT(component->state() == Component::State::Connected);
            }
        }
        for (auto component : _compatibleComponents)
        {
            if (component->state() == Component::State::Initialized)
            {
                component->deinitialize();
                Q_ASSERT(component->state() == Component::State::Connected);
            }
        }
        throw;
    }
}

void VirtualAppliance::_startComponents() throws(VirtualApplianceException)
{
    try
    {
        for (auto component : _compatibleComponents)
        {
            Q_ASSERT(component->state() == Component::State::Initialized);
            component->start();
            Q_ASSERT(component->state() == Component::State::Running);
        }
        for (auto component : _adaptedComponents)
        {
            Q_ASSERT(component->state() == Component::State::Initialized);
            component->start();
            Q_ASSERT(component->state() == Component::State::Running);
        }
        for (auto componentAdaptor : _componentAdaptors)
        {
            Q_ASSERT(componentAdaptor->state() == ComponentAdaptor::State::Initialized);
            componentAdaptor->start();
            Q_ASSERT(componentAdaptor->state() == ComponentAdaptor::State::Running);
        }
    }
    catch (...)
    {
        for (auto componentAdaptor : _componentAdaptors)
        {
            if (componentAdaptor->state() == ComponentAdaptor::State::Running)
            {
                componentAdaptor->stop();
                Q_ASSERT(componentAdaptor->state() == ComponentAdaptor::State::Initialized);
            }
        }
        for (auto component : _adaptedComponents)
        {
            if (component->state() == Component::State::Running)
            {
                component->stop();
                Q_ASSERT(component->state() == Component::State::Initialized);
            }
        }
        for (auto component : _compatibleComponents)
        {
            if (component->state() == Component::State::Running)
            {
                component->stop();
                Q_ASSERT(component->state() == Component::State::Initialized);
            }
        }
        throw;
    }
}

void VirtualAppliance::_stopComponents()
{
    for (auto componentAdaptor : _componentAdaptors)
    {
        if (componentAdaptor->state() == ComponentAdaptor::State::Running)
        {
            componentAdaptor->stop();
            Q_ASSERT(componentAdaptor->state() == ComponentAdaptor::State::Initialized);
        }
    }
    for (auto component : _adaptedComponents)
    {
        if (component->state() == Component::State::Running)
        {
            component->stop();
            Q_ASSERT(component->state() == Component::State::Initialized);
        }
    }
    for (auto component : _compatibleComponents)
    {
        if (component->state() == Component::State::Running)
        {
            component->stop();
            Q_ASSERT(component->state() == Component::State::Initialized);
        }
    }
}

void VirtualAppliance::_deinitializeComponents()
{
    for (auto componentAdaptor : _componentAdaptors)
    {
        Q_ASSERT(componentAdaptor->state() != ComponentAdaptor::State::Running);
        if (componentAdaptor->state() == ComponentAdaptor::State::Initialized)
        {
            componentAdaptor->deinitialize();
            Q_ASSERT(componentAdaptor->state() == ComponentAdaptor::State::Connected);
        }
    }
    for (auto component : _adaptedComponents)
    {
        Q_ASSERT(component->state() != Component::State::Running);
        if (component->state() == Component::State::Initialized)
        {
            component->deinitialize();
            Q_ASSERT(component->state() == Component::State::Connected);
        }
    }
    for (auto component : _compatibleComponents)
    {
        Q_ASSERT(component->state() != Component::State::Running);
        if (component->state() == Component::State::Initialized)
        {
            component->deinitialize();
            Q_ASSERT(component->state() == Component::State::Connected);
        }
    }
}

void VirtualAppliance::_disconnectComponents()
{
    for (auto componentAdaptor : _componentAdaptors)
    {
        Q_ASSERT(componentAdaptor->state() != ComponentAdaptor::State::Running &&
                 componentAdaptor->state() != ComponentAdaptor::State::Initialized);
        if (componentAdaptor->state() == ComponentAdaptor::State::Connected)
        {
            componentAdaptor->disconnect();
            Q_ASSERT(componentAdaptor->state() == ComponentAdaptor::State::Constructed);
        }
    }
    for (auto component : _adaptedComponents)
    {
        Q_ASSERT(component->state() != Component::State::Running &&
                 component->state() != Component::State::Initialized);
        if (component->state() == Component::State::Connected)
        {
            component->disconnect();
            Q_ASSERT(component->state() == Component::State::Constructed);
        }
    }
    for (auto component : _compatibleComponents)
    {
        Q_ASSERT(component->state() != Component::State::Running &&
                 component->state() != Component::State::Initialized);
        if (component->state() == Component::State::Connected)
        {
            component->disconnect();
            Q_ASSERT(component->state() == Component::State::Constructed);
        }
    }
}

void VirtualAppliance::_resetStatistics()
{
    QMutexLocker lock(&_statisticsGuard);

    _achievedClockFrequencyByClockedComponent.clear();
}

//////////
//  VirtualAppliance::_FrequencyDivider
VirtualAppliance::_FrequencyDivider::_FrequencyDivider(unsigned inputTicks, unsigned outputTicks,
                                                       IClockedComponent * drivenComponent)
    :   _inputTicks(inputTicks),
        _outputTicks(outputTicks),
        _drivenComponent(drivenComponent),
        //  Worker data for Bresenham's line algorithm
        _dx(inputTicks - 1),
        _dy(outputTicks - 1),
        _2dx(2 * _dx),
        _2dy(2 * _dy),
        _d(2 * _dy - _dx),
        _x(0),
        _y(0)
{
    Q_ASSERT(_inputTicks >= 1 && _inputTicks <= 1000000000);
    Q_ASSERT(_outputTicks >= 1 && _outputTicks <= 1000000000);
    Q_ASSERT(_outputTicks <= _inputTicks);
    Q_ASSERT(_drivenComponent != nullptr);
}

ClockFrequency VirtualAppliance::_FrequencyDivider::clockFrequency() const noexcept
{
    return _drivenComponent->clockFrequency();
}

void VirtualAppliance::_FrequencyDivider::onClockTick() noexcept
{
    _x++;
    if (_d > 0)
    {
        _y++;
        _drivenComponent->onClockTick();
        _d -= _2dx;
    }
    _d += _2dy;
    if (_x >= _dx)
    {   //  Start over
        _x = _y = 0;
        _d = 2 * _dy - _dx;
    }
}

//////////
//  VirtualAppliance::_WorkerThread
VirtualAppliance::_WorkerThread::_WorkerThread(VirtualAppliance * virtualAppliance)
    :   _virtualAppliance(virtualAppliance),
        _stopRequested(false),
        _maxClockFrequency(),   //  0hz
        _frequencyDividers(),
        _tickTargets()
{
    //  Which components/adapters will be "clock ticked" by the VA's
    //  own _WorkerThread? These are all that implement IClockedComponentAspect
    //  but do NOT implement IActiveComponentAspect
    for (auto cc : virtualAppliance->componentsImplementing<IClockedComponent>())
    {
        if (dynamic_cast<IActiveComponent*>(cc) == nullptr)
        {
            _tickTargets.append(cc);    //  can end up empty!
        }
    }

    for (auto tickTarget : _tickTargets)
    {
        _maxClockFrequency = qMax(_maxClockFrequency, tickTarget->clockFrequency());
    }

    //  Those tick targets that must tick at a frequency LOWER than
    //  maxClockFrequency require _FrequencyDividers
    for (qsizetype i = 0; i < _tickTargets.count(); i++)
    {
        IClockedComponent * tickTarget = _tickTargets[i];
        if (tickTarget->clockFrequency() < _maxClockFrequency)
        {
            _FrequencyDivider * frequencyDivider =
                new _FrequencyDivider(
                    static_cast<unsigned>(qMax(qMin(_maxClockFrequency.toHz(), 1000000000u), 1u)),
                    static_cast<unsigned>(qMax(qMin(tickTarget->clockFrequency().toHz(), 1000000000u), 1u)),
                    tickTarget);
            _frequencyDividers.append(frequencyDivider);
            _tickTargets[i] = frequencyDivider;
        }
    }
}

VirtualAppliance::_WorkerThread::~_WorkerThread()
{
    for (auto frequencyDivider : _frequencyDividers)
    {
        delete frequencyDivider;
    }
}

void VirtualAppliance::_WorkerThread::run()
{
    if (_tickTargets.isEmpty())
    {   //  Nothing to tick!
        return;
    }

    uint64_t requiredClockFrequencyHz = _maxClockFrequency.toHz();
    uint64_t requiredNsPerTick = 1000000000 / requiredClockFrequencyHz;
    unsigned ticksBetweenDelayAdjustment = static_cast<unsigned>(qMin(qMax(1u, requiredClockFrequencyHz / 10), UINT_MAX));
    unsigned delayPerTickNs = 0;
    uint64_t accumulatedDelayNs = 0;

    QElapsedTimer elapsedTimer;
    while (!_stopRequested)
    {
        //  Execute a bunch of instructions...
        elapsedTimer.restart();
        for (unsigned n = 0; n < ticksBetweenDelayAdjustment; n++)
        {
            for (qsizetype i = 0; i < _tickTargets.count(); i++)
            {
                _tickTargets[i]->onClockTick();
            }
            accumulatedDelayNs += delayPerTickNs;
        }
        uint64_t accumulatedDelayMs = accumulatedDelayNs / 1000000;
        accumulatedDelayNs -= 1000000 * accumulatedDelayMs;
        if (accumulatedDelayMs > 0)
        {
            msleep(static_cast<unsigned long>(accumulatedDelayMs));
        }

        qint64 idealNsElapsed = requiredNsPerTick * ticksBetweenDelayAdjustment;
        qint64 actualNsElapsed = elapsedTimer.nsecsElapsed();

        static int n = 0;
        if (n++ >= 100)
        {
            n = 0;
            //TODO kill off qDebug() << idealNsElapsed << " / " << actualNsElapsed << " / " << delayPerTickNs;

            qint64 actualNsPerTick = actualNsElapsed / ticksBetweenDelayAdjustment;;
            qint64 actualClockFrequencyHz = 1000000000 / actualNsPerTick;
            //TODO kill off qDebug() << "VA._WorkerThread Running at " << actualClockFrequencyHz << " Hz, delayPerTickNs = " << delayPerTickNs;
            //  Record!
            for (qsizetype i = 0; i < _tickTargets.count(); i++)
            {
                _virtualAppliance->recordAchievedClockFrequency(
                    _tickTargets[i],
                    ClockFrequency::hertz(_tickTargets[i]->clockFrequency().toHz() * actualClockFrequencyHz / requiredClockFrequencyHz));
            }
        }

        if (actualNsElapsed < idealNsElapsed)
        {   //  Going too fast!
            delayPerTickNs++;
        }
        else if (actualNsElapsed > idealNsElapsed && delayPerTickNs > 0)
        {   //  Going too slow!
            delayPerTickNs--;
        }
        //qDebug() << actualNsElapsed;
        //  ...and see how long it actually took to execute the binch

        //usleep(1);
    }
}

//  End of hadesvm-core/VirtualAppliance.cpp
