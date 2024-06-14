//
//  hadesvm-cereon/ResidentRomUnit.cpp
//
//  hadesvm::cereon::ResidentRomUnit class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;

//////////
//  Constants
const uint64_t   ResidentRomUnit::DefaultStartAddress = UINT64_C(0xFFFFFFFFFFF00000);
const hadesvm::core::MemorySize ResidentRomUnit::DefaultSize = hadesvm::core::MemorySize::megabytes(1);
const QString    ResidentRomUnit::DefailtContentFilePath = "./rom.bin";

//////////
//  Construction/destruction
ResidentRomUnit::ResidentRomUnit()
    :   ResidentMemoryUnit(DefaultStartAddress, DefaultSize),
        _contentFilePath(DefailtContentFilePath)
{
}

ResidentRomUnit::~ResidentRomUnit() noexcept
{
}

//////////
//  hadesvm::core::Component
QString ResidentRomUnit::displayName() const
{
    return hadesvm::util::toString(size()) +
           " ROM @ " +
           hadesvm::util::toString(startAddress(), "%016X");
}

void ResidentRomUnit::serialiseConfiguration(QDomElement componentElement) const
{
    ResidentMemoryUnit::serialiseConfiguration(componentElement);
    componentElement.setAttribute("ContentFilePath", _contentFilePath);
}

void ResidentRomUnit::deserialiseConfiguration(QDomElement componentElement)
{
    ResidentMemoryUnit::deserialiseConfiguration(componentElement);
    _contentFilePath = componentElement.attribute("ContentFilePath");
}

hadesvm::core::ComponentEditor * ResidentRomUnit::createEditor(QWidget * parent)
{
    return new ResidentRomUnitEditor(parent, this);
}

ResidentRomUnit::Ui * ResidentRomUnit::createUi()
{
    return nullptr;
}

//////////
//  hadesvm::core::Component (state management)
void ResidentRomUnit::initialize() throws(hadesvm::core::VirtualApplianceException)
{
    ResidentMemoryUnit::initialize();  //  may throw hadesvm::core::VirtualApplianceException

    //  Load ROM content...
    QString fileName = virtualAppliance()->toAbsolutePath(_contentFilePath);
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {   //  OOPS!
        ResidentMemoryUnit::deinitialize();
        throw hadesvm::core::VirtualApplianceException("Could not read " + fileName);
    }
    QDataStream in(&file);

    uint64_t maxBytesToRead = this->size().toBytes();
    char chunk[4096];
    size_t offset = 0;
    while (maxBytesToRead > 0)
    {
        qint64 chunkSize = in.readRawData(chunk, sizeof(chunk));
        if (chunkSize <= 0)
        {   //  EOF
            break;
        }
        for (int i = 0; i < chunkSize; i++)
        {
            ResidentMemoryUnit::storeByte(offset++, chunk[i]);
        }
        maxBytesToRead -= chunkSize;
    }
}

//////////
//  IMemoryBlock
void ResidentRomUnit::storeByte(size_t /*offset*/, uint8_t /*value*/) throws(MemoryAccessError)
{
    throw MemoryAccessError::AccessDenied;
}

void ResidentRomUnit::storeHalfWord(size_t /*offset*/, uint16_t /*value*/, ByteOrder /*byteOrder*/) throws(MemoryAccessError)
{
    throw MemoryAccessError::AccessDenied;
}

void ResidentRomUnit::storeWord(size_t /*offset*/, uint32_t /*value*/, ByteOrder /*byteOrder*/) throws(MemoryAccessError)
{
    throw MemoryAccessError::AccessDenied;
}

void ResidentRomUnit::storeLongWord(size_t /*offset*/, uint64_t /*value*/, ByteOrder /*byteOrder*/) throws(MemoryAccessError)
{
    throw MemoryAccessError::AccessDenied;
}

//////////
//  Operations (configuration)
QString ResidentRomUnit::contentFilePath() const
{
    return _contentFilePath;
}

void ResidentRomUnit::setContentFilePath(const QString & contentFilePath)
{
    Q_ASSERT(state() == State::Constructed);

    //  TODO validate "contentFilePath"
    _contentFilePath = contentFilePath;
}

//////////
//  ResidentRomUnit::Type
HADESVM_IMPLEMENT_SINGLETON(ResidentRomUnit::Type)
ResidentRomUnit::Type::Type() {}
ResidentRomUnit::Type::~Type() {}

QString ResidentRomUnit::Type::mnemonic() const
{
    return "CereonResidentRomUnit";
}

QString ResidentRomUnit::Type::displayName() const
{
    return "Cereon Resident ROM Unit";
}

hadesvm::core::ComponentCategory * ResidentRomUnit::Type::category() const
{
    return hadesvm::core::StandardComponentCategories::Memory;
}

bool ResidentRomUnit::Type::suspendable() const
{
    return true;
}

bool ResidentRomUnit::Type::isCompatibleWith(hadesvm::core::VirtualArchitecture * architecture) const
{
    return architecture == CereonWorkstationArchitecture::instance();
}

bool ResidentRomUnit::Type::isCompatibleWith(hadesvm::core::VirtualApplianceType * type) const
{
    return type == hadesvm::core::VirtualMachineType::instance();
}

ResidentRomUnit * ResidentRomUnit::Type::createComponent()
{
    return new ResidentRomUnit();
}

//  End of hadesvm-cereon/ResidentRomUnit.cpp
