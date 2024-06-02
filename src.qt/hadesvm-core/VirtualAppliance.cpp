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
VirtualAppliance::VirtualAppliance(const QString & name, const QString & location)
    :   _stateGuard(),
        _name(name),
        _location(QFileInfo(location).absoluteFilePath()),
        _directory(QFileInfo(_location).absolutePath()),
        _components()

{
}

VirtualAppliance::~VirtualAppliance()
{
}

//////////
//  Operations
VirtualAppliance::State VirtualAppliance::state() const
{
    QMutexLocker lock(&_stateGuard);
    return _state;
}

QString VirtualAppliance::name() const
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    return _name;
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

ComponentList VirtualAppliance::components() const
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    return _components;
}

void VirtualAppliance::addComponent(Component * component) throws(VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());
    Q_ASSERT(component != nullptr);
    Q_ASSERT(component->virtualAppliance() == nullptr);

    _components.append(component);
    component->_virtualAppliance = this;
}

void VirtualAppliance::removeComponent(Component * component)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());
    Q_ASSERT(component != nullptr);

    if (_components.contains(component))
    {
        Q_ASSERT(component->virtualAppliance() == this);
        _components.removeOne(component);
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

    for (Component * component : _components)
    {
        if (!component->suspendable())
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
    QDomDocument document;

    //  Set up root element
    QDomElement rootElement = document.createElement(type()->mnemonic());
    rootElement.setAttribute("Name", name());
    rootElement.setAttribute("Version", "1");
    document.appendChild(rootElement);

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

//  End of hadesvm-core/VirtualAppliance.cpp
