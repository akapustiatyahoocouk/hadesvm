//
//  hadesvm-core/VirtualAppliance.cpp
//
//  hadesvm::core::VirtualAppliance class implementation
//
//////////
#include "hadesvm-core/API.hpp"
using namespace hadesvm::core;

//////////
//  Constants
const QString VirtualAppliance::PreferredExtension = ".hadesvm";

//////////
//  Construction/destruction
VirtualAppliance::VirtualAppliance(const QString & name, const QString & location,
                                   VirtualArchitecture * architecture)
    :   _stateGuard(),
        _name(name),
        _location(QFileInfo(location).absoluteFilePath()),
        _directory(QFileInfo(_location).absolutePath()),
        _architecture(architecture)
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
//  Operations
VirtualAppliance::State VirtualAppliance::state() const
{
    QMutexLocker lock(&_stateGuard);
    return _state;
}

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

void VirtualAppliance::start() throws(VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());
}

void VirtualAppliance::stop() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());
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
}

void VirtualAppliance::resume() throws(VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());
}

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

//  End of hadesvm-core/VirtualAppliance.cpp
