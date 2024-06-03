//
//  hadesvm-kernel/KernelEditor.hpp
//
//  hadesvm-kernel editor for a Kernel component
//
//////////
#pragma once
#include "hadesvm-kernel/API.hpp"

namespace hadesvm
{
    namespace kernel
    {
        //////////
        //  The editor for a Kernel component
        namespace Ui { class KernelEditor; }

        class HADESVM_KERNEL_PUBLIC KernelEditor : public hadesvm::core::ComponentEditor
        {
            Q_OBJECT
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(KernelEditor)

            //////////
            //  Construction/destruction
        public:
            KernelEditor(QWidget * parent, Kernel * kernel);
            virtual ~KernelEditor();

            //////////
            //  hadesvm::core::ComponentEditor
        public:
            virtual bool        canSaveComponentConfiguration() const override;

            //////////
            //  Implementation
        private:
            Kernel *const   _kernel;

            //////////
            //  Controls & resources
        private:
            Ui::KernelEditor *      _ui;
        };
    }
}

//  End of hadesvm-kernel/KernelEditor.hpp
