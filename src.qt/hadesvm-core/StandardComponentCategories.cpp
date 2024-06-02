//
//  hadesvm-core/StandardComponentCategories.cpp
//
//  hadesvm::core::StandardComponentCategories class implementation
//
//////////
#include "hadesvm-core/API.hpp"
using namespace hadesvm::core;

namespace
{
    class ProcessorsComponentCategory final : public ComponentCategory
    {
        HADESVM_DECLARE_SINGLETON(ProcessorsComponentCategory)

        //////////
        //  hadesvm::util::StockObject
    public:
        virtual QString     mnemonic() const override;
        virtual QString     displayName() const override;
    };

    HADESVM_IMPLEMENT_SINGLETON(ProcessorsComponentCategory)
    ProcessorsComponentCategory::ProcessorsComponentCategory() {}
    ProcessorsComponentCategory::~ProcessorsComponentCategory() {}
    QString ProcessorsComponentCategory::mnemonic() const { return "Processors"; }
    QString ProcessorsComponentCategory::displayName() const { return "Processors"; }

    class MemoryComponentCategory final : public ComponentCategory
    {
        HADESVM_DECLARE_SINGLETON(MemoryComponentCategory)

        //////////
        //  hadesvm::util::StockObject
    public:
        virtual QString     mnemonic() const override;
        virtual QString     displayName() const override;
    };

    HADESVM_IMPLEMENT_SINGLETON(MemoryComponentCategory)
    MemoryComponentCategory::MemoryComponentCategory() {}
    MemoryComponentCategory::~MemoryComponentCategory() {}
    QString MemoryComponentCategory::mnemonic() const { return "Memory"; }
    QString MemoryComponentCategory::displayName() const { return "Memory"; }

    class StorageComponentCategory final : public ComponentCategory
    {
        HADESVM_DECLARE_SINGLETON(StorageComponentCategory)

        //////////
        //  hadesvm::util::StockObject
    public:
        virtual QString     mnemonic() const override;
        virtual QString     displayName() const override;
    };

    HADESVM_IMPLEMENT_SINGLETON(StorageComponentCategory)
    StorageComponentCategory::StorageComponentCategory() {}
    StorageComponentCategory::~StorageComponentCategory() {}
    QString StorageComponentCategory::mnemonic() const { return "Storage"; }
    QString StorageComponentCategory::displayName() const { return "Storage"; }

    class DevicesComponentCategory final : public ComponentCategory
    {
        HADESVM_DECLARE_SINGLETON(DevicesComponentCategory)

        //////////
        //  hadesvm::util::StockObject
    public:
        virtual QString     mnemonic() const override;
        virtual QString     displayName() const override;
    };

    HADESVM_IMPLEMENT_SINGLETON(DevicesComponentCategory)
    DevicesComponentCategory::DevicesComponentCategory() {}
    DevicesComponentCategory::~DevicesComponentCategory() {}
    QString DevicesComponentCategory::mnemonic() const { return "Devices"; }
    QString DevicesComponentCategory::displayName() const { return "Devices"; }

    class SoftwareComponentCategory final : public ComponentCategory
    {
        HADESVM_DECLARE_SINGLETON(SoftwareComponentCategory)

        //////////
        //  hadesvm::util::StockObject
    public:
        virtual QString     mnemonic() const override;
        virtual QString     displayName() const override;
    };

    HADESVM_IMPLEMENT_SINGLETON(SoftwareComponentCategory)
    SoftwareComponentCategory::SoftwareComponentCategory() {}
    SoftwareComponentCategory::~SoftwareComponentCategory() {}
    QString SoftwareComponentCategory::mnemonic() const { return "Software"; }
    QString SoftwareComponentCategory::displayName() const { return "Software"; }

    class MiscellaneousComponentCategory final : public ComponentCategory
    {
        HADESVM_DECLARE_SINGLETON(MiscellaneousComponentCategory)

        //////////
        //  hadesvm::util::StockObject
    public:
        virtual QString     mnemonic() const override;
        virtual QString     displayName() const override;
    };

    HADESVM_IMPLEMENT_SINGLETON(MiscellaneousComponentCategory)
    MiscellaneousComponentCategory::MiscellaneousComponentCategory() {}
    MiscellaneousComponentCategory::~MiscellaneousComponentCategory() {}
    QString MiscellaneousComponentCategory::mnemonic() const { return "Miscellaneous"; }
    QString MiscellaneousComponentCategory::displayName() const { return "Miscellaneous"; }
}

//////////
//  Constants
ComponentCategory *const StandardComponentCategories::Processors = ProcessorsComponentCategory::instance();
ComponentCategory *const StandardComponentCategories::Memory = MemoryComponentCategory::instance();
ComponentCategory *const StandardComponentCategories::Storage = StorageComponentCategory::instance();
ComponentCategory *const StandardComponentCategories::Devices = DevicesComponentCategory::instance();
ComponentCategory *const StandardComponentCategories::Software = SoftwareComponentCategory::instance();
ComponentCategory *const StandardComponentCategories::Miscellaneous = MiscellaneousComponentCategory::instance();

//////////
//  Operations
ComponentCategoryList StandardComponentCategories::all()
{
    ComponentCategoryList result;

    result.append(ProcessorsComponentCategory::instance());
    result.append(MemoryComponentCategory::instance());
    result.append(StorageComponentCategory::instance());
    result.append(DevicesComponentCategory::instance());
    result.append(SoftwareComponentCategory::instance());
    result.append(MiscellaneousComponentCategory::instance());

    return result;
}

//  End of hadesvm-core/StandardComponentCategories.cpp
