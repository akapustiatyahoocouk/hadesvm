//
//  hadesvm-kernel/MountedFolderDialog.hpp
//
//  hadesvm-kernel add/modify mounted folder dialog
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
        namespace Ui { class MountedFolderDialog;}

        class HADESVM_KERNEL_PUBLIC MountedFolderDialog final : public QDialog
        {
            Q_OBJECT
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(MountedFolderDialog)

            //////////
            //  Construction/destruction
        public:
            MountedFolderDialog(QWidget * parent, const QString & volumeName, const QString & path);
            virtual ~MountedFolderDialog();

            //////////
            //  Operations
        public:
            QString         volumeName() const { return _volumeName; }
            QString         path() const { return _path; }

            //////////
            //  Implementation
        private:
            QString         _volumeName;
            QString         _path;

            //  Helpers
            void            _refresh();

            //////////
            //  Controls & resources
        private:
            Ui::MountedFolderDialog *   _ui;

            //////////
            //  Signal handlers
        private slots:
            void            _onLineEditTextChanged(QString);
            void            _onBrowsePushButtonClicked();
            void            _onOk();
            void            _onCancel();
        };
    }
}

//  End of hadesvm-kernel/MountedFolderDialog.hpp
