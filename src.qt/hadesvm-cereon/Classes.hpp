//
//  hadesvm-cereon/Classes.hpp
//
//  hadesvm-cereon forward declarations and typedefs
//
//////////

namespace hadesvm
{
    namespace cereon
    {
        class HADESVM_CEREON_PUBLIC Processor;
        class HADESVM_CEREON_PUBLIC ProcessorCore;
        class HADESVM_CEREON_PUBLIC IIoPort;
        class HADESVM_CEREON_PUBLIC IoBus;

        class HADESVM_CEREON_PUBLIC Kis1Keyboard;

        using ProcessorCoreList = QList<ProcessorCore*>;

        //  Adopt a few types from dependency components for cleaner reading
        using StandardComponentCategories = hadesvm::core::StandardComponentCategories;
        using MemorySize = hadesvm::core::MemorySize;
        using ComponentEditor = hadesvm::core::ComponentEditor;

        using ByteOrder = hadesvm::util::ByteOrder;
    }
}

//  End of hadesvm-cereon/Classes.hpp
