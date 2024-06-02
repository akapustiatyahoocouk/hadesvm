//
//  hadesvm-core/Classes.hpp
//
//  hadesvm-core forward declarations and typedefs
//
//////////

namespace hadesvm
{
    namespace core
    {
        class HADESVM_CORE_PUBLIC ComponentCategory;
        class HADESVM_CORE_PUBLIC ComponentType;
        class HADESVM_CORE_PUBLIC Component;
        class HADESVM_CORE_PUBLIC ComponentAdaptorType;
        class HADESVM_CORE_PUBLIC ComponentAdaptor;

        class HADESVM_CORE_PUBLIC VirtualArchitecture;
        class HADESVM_CORE_PUBLIC VirtualApplianceType;
        class HADESVM_CORE_PUBLIC VirtualMachineType;
        class HADESVM_CORE_PUBLIC RemoteTerminalType;

        class HADESVM_CORE_PUBLIC VirtualApplianceTemplate;
        class HADESVM_CORE_PUBLIC VirtualMachineTemplate;
        class HADESVM_CORE_PUBLIC RemoteTerminalTemplate;

        class HADESVM_CORE_PUBLIC VirtualAppliance;
        class HADESVM_CORE_PUBLIC VirtualMachine;
        class HADESVM_CORE_PUBLIC RemoteTerminal;

        using ComponentCategoryList = QList<ComponentCategory*>;
        using ComponentTypeList = QList<ComponentType*>;
        using ComponentList = QList<Component*>;
        using ComponentAdaptorTypeList = QList<ComponentAdaptorType*>;
        using ComponentAdaptorList = QList<ComponentAdaptor*>;

        using VirtualArchitectureList = QList<VirtualArchitecture*>;

        using VirtualApplianceTypeList = QList<VirtualApplianceType*>;
        using VirtualMachineTypeList = QList<VirtualMachineType*>;
        using RemoteTerminalTypeList = QList<RemoteTerminalType*>;

        using VirtualApplianceTemplateList = QList<VirtualApplianceTemplate*>;
        using VirtualMachineTemplateList = QList<VirtualMachineTemplate*>;
        using RemoteTerminalTemplateList = QList<RemoteTerminalTemplate*>;

        using VirtualApplianceList = QList<VirtualAppliance*>;
        using VirtualMachineList = QList<VirtualMachine*>;
        using RemoteTerminalList = QList<RemoteTerminal*>;
    }
}

//  End of hadesvm-core/Classes.hpp
