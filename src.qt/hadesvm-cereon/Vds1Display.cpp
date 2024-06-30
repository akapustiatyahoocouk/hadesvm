//
//  hadesvm-cereon/Vds1Display.cpp
//
//  hadesvm::cereon::Vds1Display class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;

//////////
//  Constants
const uint16_t   Vds1Display::DefaultControllerStatePortAddress = 0x0200;
const uint8_t    Vds1Display::DefaultControllerCompartmentNumber = 0;

//////////
//  Construction/destruction
Vds1Display::Vds1Display()
    :   //  Configuration
        _controllerStatePortAddress(DefaultControllerStatePortAddress),
        _controllerCompartmentNumber(DefaultControllerCompartmentNumber)
{
}

Vds1Display::~Vds1Display() noexcept
{
}

//////////
//  hadesvm::core::Component
QString Vds1Display::displayName() const
{
    return Type::instance()->displayName() +
           " @ " +
           hadesvm::util::toString(_controllerStatePortAddress, "%04X") +
           ":" +
           hadesvm::util::toString(_controllerCompartmentNumber);
}

void Vds1Display::serialiseConfiguration(QDomElement componentElement) const
{
    componentElement.setAttribute("ControllerStatePortAddress", hadesvm::util::toString(_controllerStatePortAddress, "%04X"));
    componentElement.setAttribute("ControllerCompartmentNumber", hadesvm::util::toString(_controllerCompartmentNumber));

    switch (_stretchMode)
    {
        case StretchMode::NoStretch:
            componentElement.setAttribute("StretchMode", "NoStretch");
            break;
        case StretchMode::IntegralStretch:
            componentElement.setAttribute("StretchMode", "IntegralStretch");
            break;
        case StretchMode::Fill:
            componentElement.setAttribute("StretchMode", "Fill");
            break;
        default:
            failure();
    }
}

void Vds1Display::deserialiseConfiguration(QDomElement componentElement)
{
    uint16_t controllerStatePortAddress = 0;
    if (hadesvm::util::fromString(componentElement.attribute("ControllerStatePortAddress"), "%X", controllerStatePortAddress))
    {
        _controllerStatePortAddress = controllerStatePortAddress;
    }

    uint8_t controllerCompartmentNumber = 0;
    if (hadesvm::util::fromString(componentElement.attribute("ControllerCompartmentNumber"), controllerCompartmentNumber))
    {
        _controllerCompartmentNumber = controllerCompartmentNumber;
    }

    if (componentElement.attribute("StretchMode") == "NoStretch")
    {
        _stretchMode = StretchMode::NoStretch;
    }
    else if (componentElement.attribute("StretchMode") == "IntegralStretch")
    {
        _stretchMode = StretchMode::IntegralStretch;
    }
    else if (componentElement.attribute("StretchMode") == "Fill")
    {
        _stretchMode = StretchMode::Fill;
    }
}

hadesvm::core::ComponentEditor * Vds1Display::createEditor(QWidget * parent)
{
    return new Vds1DisplayEditor(parent, this);
}

Vds1Display::Ui * Vds1Display::createUi()
{
    return new Ui(this);
}

//////////
//  Operations (component configuration)
void Vds1Display::setControllerStatePortAddress(uint16_t controllerStatePortAddress)
{
    Q_ASSERT(_state == State::Constructed);

    //  TODO validate "controllerStatePortAddress"
    _controllerStatePortAddress = controllerStatePortAddress;
}

void Vds1Display::setControllerCompartmentNumber(uint8_t controllerCompartmentNumber)
{
    Q_ASSERT(_state == State::Constructed);

    //  TODO validate "controllerCompartmentNumber"
    _controllerCompartmentNumber = controllerCompartmentNumber;
}

void Vds1Display::setStretchMode(StretchMode stretchMode)
{
    //  Can be changed at runtime!!!
    _stretchMode = stretchMode;
}

//////////
//  hadesvm::core::Component (state management)
Vds1Display::State Vds1Display::state() const noexcept
{
    return _state;
}

void Vds1Display::connect() throws(hadesvm::core::VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Constructed)
    {   //  OOPS! Can't
        return;
    }

    //  If a VDS1 controller does not exist at _controllerStatePortAddress
    //  or has less than _controllerCompartmentNumber compartments, it's an error
    QList<Vds1Controller*> vdsControllers;
    for (auto vdsController : virtualAppliance()->componentsImplementing<Vds1Controller>())
    {
        if (vdsController->statePortAddress() == _controllerStatePortAddress)
        {
            vdsControllers.append(vdsController);
        }
    }
    if (vdsControllers.count() == 0)
    {
        throw hadesvm::core::VirtualApplianceException("TODO proper error message");
    }
    if (vdsControllers.count() > 1)
    {
        throw hadesvm::core::VirtualApplianceException("TODO proper error message");
    }
    if (_controllerCompartmentNumber >= vdsControllers[0]->numberOfCompartments())
    {
        throw hadesvm::core::VirtualApplianceException("TODO proper error message");
    }

    //  TODO what if there are multiple VDS1/KIS1 controllers ?
    for (Kis1Keyboard * kis1Keyboard : virtualAppliance()->componentsImplementing<Kis1Keyboard>())
    {
        if (kis1Keyboard->controllerCompartmentNumber() == controllerCompartmentNumber())
        {   //  This one!
            _kis1Keyboard = kis1Keyboard;
            break;
        }
    }

    _state = State::Connected;
}

void Vds1Display::initialize() throws(hadesvm::core::VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Connected)
    {   //  OOPS! Can't
        return;
    }

    _state = State::Initialized;
}

void Vds1Display::start() throws(hadesvm::core::VirtualApplianceException)
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Initialized)
    {   //  OOPS! Can't
        return;
    }

    _state = State::Running;
}

void Vds1Display::stop() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Running)
    {   //  OOPS! Can't
        return;
    }

    _state = State::Initialized;
}

void Vds1Display::deinitialize() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Initialized)
    {   //  OOPS! Can't
        return;
    }

    _state = State::Connected;
}

void Vds1Display::disconnect() noexcept
{
    Q_ASSERT(QApplication::instance()->thread() == QThread::currentThread());

    if (_state != State::Connected)
    {   //  OOPS! Can't
        return;
    }

    _kis1Keyboard = nullptr;

    _state = State::Constructed;
}

//////////
//  hadesvm::cereon::Vds1Display::Type
HADESVM_IMPLEMENT_SINGLETON(Vds1Display::Type)
Vds1Display::Type::Type() {}
Vds1Display::Type::~Type() {}

QString Vds1Display::Type::mnemonic() const
{
    return "CereonVds1Display";
}

QString Vds1Display::Type::displayName() const
{
    return "Cereon VDS1 display";
}

hadesvm::core::ComponentCategory * Vds1Display::Type::category() const
{
    return hadesvm::core::StandardComponentCategories::UiDevices;
}

bool Vds1Display::Type::isCompatibleWith(hadesvm::core::VirtualArchitecture * architecture) const
{
    return architecture == CereonWorkstationArchitecture::instance();
}

bool Vds1Display::Type::isCompatibleWith(hadesvm::core::VirtualApplianceType * type) const
{
    return type == hadesvm::core::VirtualMachineType::instance();
}

Vds1Display * Vds1Display::Type::createComponent()
{
    return new Vds1Display();
}

//////////
//  Vds1Display::Ui
Vds1Display::Ui::Ui(Vds1Display * vds1Display)
    :   _vds1Display(vds1Display),
        _vds1DisplayWidget(new Vds1DisplayWidget(nullptr, vds1Display)),
        _displayWidgets{_vds1DisplayWidget}
{
    Q_ASSERT(_vds1Display != nullptr);
}

Vds1Display::Ui::~Ui()
{
    _vds1DisplayWidget->_canDestruct = true;
    delete _vds1DisplayWidget;
}

//  End of hadesvm-cereon/Vds1Display.cpp
