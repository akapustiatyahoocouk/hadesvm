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

        class HADESVM_KERNEL_PUBLIC KernelEditor final : public hadesvm::core::ComponentEditor
        {
            Q_OBJECT
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(KernelEditor)

            //////////
            //  Construction/destruction
        public:
            explicit KernelEditor(Kernel * kernel);
            virtual ~KernelEditor();

            //////////
            //  hadesvm::core::ComponentEditor
        public:
            virtual void        loadComponentConfiguration() override;
            virtual bool        canSaveComponentConfiguration() const override;
            virtual void        saveComponentConfiguration() override;

            //////////
            //  Implementation
        private:
            Kernel *const       _kernel;

            //  Helpers
            void                _refresh();
            void                _addMountedFolder(const QString & volumeName, const QString & path);
            void                _deleteMountedFolder(const QString & volumeName);

            //////////
            //  Controls & resources
        private:
            Ui::KernelEditor *  _ui;

            //////////
            //  Signal handlers:
        private slots:
            void                _onNodeNameTextChanged(QString);
            void                _onVolumesListWidgetCurrentRowChanged(int);
            void                _onAddVolumePushButtonClicked();
            void                _onModifyVolumePushButtonClicked();
            void                _onDeleteVolumePushButtonClicked();
        };
    }
}

//  End of hadesvm-kernel/KernelEditor.hpp
