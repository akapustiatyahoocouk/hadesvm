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
        _name(name),
        _location(QFileInfo(location).absoluteFilePath()),
        _directory(QFileInfo(_location).absolutePath()),
        _architecture(architecture),
        _compatibleComponents(),
        _adaptedComponents(),
        _componentAdaptors(),
        //  Runtime state
        _stopRequested(false)
{
    Q_ASSERT(_architecture != nullptr);
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
    document.appendChild(rootElement);

    //  Set up "components" element
    QDomElement componentsElement = document.createElement("Components");
    rootElement.appendChild(componentsElement);

    //  Set up component XML nodes
    for (auto component : _compatibleComponents)
    {
        QDomElement componentElement = document.createElement("Component");
        componentsElement.appendChild(componentElement);
        componentElement.setAttribute("Type", component->type()->mnemonic());
        component->serialiseConfiguration(componentElement);
        Q_ASSERT(componentElement.attribute("Type") == component->type()->mnemonic());
    }
    for (auto componentAdaptor : _componentAdaptors)
    {
        auto component = componentAdaptor->adaptedComponent();
        QDomElement componentElement = document.createElement("Component");
        componentsElement.appendChild(componentElement);
        componentElement.setAttribute("Type", component->type()->mnemonic());
        component->serialiseConfiguration(componentElement);
        Q_ASSERT(componentElement.attribute("Type") == component->type()->mnemonic());

        QDomElement adaptorElement = document.createElement("Adaptor");
        componentElement.appendChild(adaptorElement);
        adaptorElement.setAttribute("Type", componentAdaptor->type()->mnemonic());
        componentAdaptor->serialiseConfiguration(adaptorElement);
        Q_ASSERT(adaptorElement.attribute("Type") == componentAdaptor->type()->mnemonic());
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
                if (!adaptorElement.isNull() && adaptorElement.attribute("Type") == adaptor->type()->mnemonic())
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
    if (!component->type()->isCompatibleWith(_architecture) &&
        !component->type()->isAdaptableTo(_architecture))
    {
        throw VirtualApplianceException("Component incompatible with virtual appliance architecture");
    }
    if (!component->type()->isCompatibleWith(type()))
    {
        throw VirtualApplianceException("Component incompatible with virtual appliance type");
    }
    //  ...and add the component directly to this VA or adapt it
    if (component->type()->isCompatibleWith(_architecture))
    {   //  Add directly
        _compatibleComponents.append(component);
    }
    else
    {   //  Adapt
        _adaptedComponents.append(component);
        ComponentAdaptorType * adaptorType = ComponentAdaptorType::find(component->type(), _architecture);
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

        _connectComponents();       //  may throw
        _initializeComponents();    //  may throw
        _startComponents();         //  may throw

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

    switch (_state)
    {
        case  State::Stopped:
            //  Nothing to do
            break;
        case State::Suspended:
            //  TODO implement
            Q_ASSERT(false);
            break;
        case State::Running:
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
            Q_ASSERT(false);
    }
}

bool VirtualAppliance::suspendable() const noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    for (Component * component : _compatibleComponents)
    {
        if (!component->suspendable())
        {
            return false;
        }
    }
    for (Component * component : _adaptedComponents)
    {
        if (!component->suspendable())
        {
            return false;
        }
    }
    for (ComponentAdaptor * componentAdaptor : _componentAdaptors)
    {
        if (!componentAdaptor->suspendable())
        {
            return false;
        }
    }
    return true;
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

//////////
//  Operations (runtime state)
void VirtualAppliance::requestStop()
{
    _stopRequested = true;
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
            component->initialize();
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

//  End of hadesvm-core/VirtualAppliance.cpp
