//
//  hadesvm-cereon/ResidentRomBlock.cpp
//
//  hadesvm::cereon::ResidentRomBlock class implementation
//
//////////
#include "hadesvm-cereon/API.hpp"
using namespace hadesvm::cereon;

//////////
//  Constants
const uint64_t   ResidentRomBlock::DefaultStartAddress = UINT64_C(0xFFFFFFFFFFF00000);
const hadesvm::core::MemorySize ResidentRomBlock::DefaultSize = hadesvm::core::MemorySize::megabytes(1);
const QString    ResidentRomBlock::DefailtContentFilePath = "./rom.bin";

//////////
//  Construction/destruction
ResidentRomBlock::ResidentRomBlock()
    :   ResidentMemoryBlock(DefaultStartAddress, DefaultSize),
        _contentFilePath(DefailtContentFilePath)
{
}

ResidentRomBlock::~ResidentRomBlock() noexcept
{
}

//////////
//  hadesvm::core::Component
QString ResidentRomBlock::displayName() const
{
    return hadesvm::util::toString(size()) +
           " ROM @ " +
           hadesvm::util::toString(startAddress(), "%016X");
}

void ResidentRomBlock::serialiseConfiguration(QDomElement componentElement) const
{
    ResidentMemoryBlock::serialiseConfiguration(componentElement);
    componentElement.setAttribute("ContentFilePath", _contentFilePath);
}

void ResidentRomBlock::deserialiseConfiguration(QDomElement componentElement)
{
    ResidentMemoryBlock::deserialiseConfiguration(componentElement);
    _contentFilePath = componentElement.attribute("ContentFilePath");
}

hadesvm::core::ComponentEditor * ResidentRomBlock::createEditor(QWidget * parent)
{
    return new ResidentRomBlockEditor(parent, this);
}

ResidentRomBlock::Ui * ResidentRomBlock::createUi()
{
    return nullptr;
}

//////////
//  hadesvm::core::Component (state management)
void ResidentRomBlock::initialize() throws(hadesvm::core::VirtualApplianceException)
{
    ResidentMemoryBlock::initialize();  //  may throw hadesvm::core::VirtualApplianceException

    //  Load ROM content...
    QString fileName = virtualAppliance()->toAbsolutePath(_contentFilePath);
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {   //  OOPS!
        ResidentMemoryBlock::deinitialize();
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
            ResidentMemoryBlock::storeByte(offset++, chunk[i]);
        }
        maxBytesToRead -= chunkSize;
    }
}

//////////
//  MemoryBlock
void ResidentRomBlock::storeByte(size_t /*offset*/, uint8_t /*value*/) throws(MemoryAccessError)
{
    throw MemoryAccessError::AccessDenied;
}

void ResidentRomBlock::storeHalfWord(size_t /*offset*/, uint16_t /*value*/, ByteOrder /*byteOrder*/) throws(MemoryAccessError)
{
    throw MemoryAccessError::AccessDenied;
}

void ResidentRomBlock::storeWord(size_t /*offset*/, uint32_t /*value*/, ByteOrder /*byteOrder*/) throws(MemoryAccessError)
{
    throw MemoryAccessError::AccessDenied;
}

void ResidentRomBlock::storeLongWord(size_t /*offset*/, uint64_t /*value*/, ByteOrder /*byteOrder*/) throws(MemoryAccessError)
{
    throw MemoryAccessError::AccessDenied;
}

//////////
//  Operations (configuration)
QString ResidentRomBlock::contentFilePath() const
{
    return _contentFilePath;
}

void ResidentRomBlock::setContentFilePath(const QString & contentFilePath)
{
    Q_ASSERT(state() == State::Constructed);

    //  TODO validate "contentFilePath"
    _contentFilePath = contentFilePath;
}

//////////
//  ResidentRomBlock::Type
HADESVM_IMPLEMENT_SINGLETON(ResidentRomBlock::Type)
ResidentRomBlock::Type::Type() {}
ResidentRomBlock::Type::~Type() {}

QString ResidentRomBlock::Type::mnemonic() const
{
    return "CereonResidentRomBlock";
}

QString ResidentRomBlock::Type::displayName() const
{
    return "Cereon Resident ROM Block";
}

hadesvm::core::ComponentCategory * ResidentRomBlock::Type::category() const
{
    return hadesvm::core::StandardComponentCategories::Memory;
}

bool ResidentRomBlock::Type::suspendable() const
{
    return true;
}

bool ResidentRomBlock::Type::isCompatibleWith(hadesvm::core::VirtualArchitecture * architecture) const
{
    return architecture == CereonWorkstationArchitecture::instance();
}

bool ResidentRomBlock::Type::isCompatibleWith(hadesvm::core::VirtualApplianceType * type) const
{
    return type == hadesvm::core::VirtualMachineType::instance();
}

ResidentRomBlock * ResidentRomBlock::Type::createComponent()
{
    return new ResidentRomBlock();
}

//  End of hadesvm-cereon/ResidentRomBlock.cpp
