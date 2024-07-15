//
//  hadesvm-cereon/Fdc1FloppyDriveEditor.hpp
//
//  hadesvm-cereon editor for a Fdc1FloppyDrive component
//
//////////
#pragma once
#include "hadesvm-cereon/API.hpp"

namespace hadesvm
{
    namespace cereon
    {
        //////////
        //  The editor for a Kis1Controller component
        namespace Ui { class Fdc1FloppyDriveEditor; }

        class HADESVM_CEREON_PUBLIC Fdc1FloppyDriveEditor : public hadesvm::core::ComponentEditor
        {
            Q_OBJECT
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Fdc1FloppyDriveEditor)

            //////////
            //  Construction/destruction
        public:
            explicit Fdc1FloppyDriveEditor(Fdc1FloppyDrive * fdc1FloppyDrive);
            virtual ~Fdc1FloppyDriveEditor();

            //////////
            //  hadesvm::core::ComponentEditor
        public:
            virtual void        loadComponentConfiguration() override;
            virtual bool        canSaveComponentConfiguration() const override;
            virtual void        saveComponentConfiguration() override;

            //////////
            //  Implementation
        private:
            Fdc1FloppyDrive *const  _fdc1FloppyDrive;

            //////////
            //  Controls & resources
        private:
            Ui::Fdc1FloppyDriveEditor * _ui;

            //////////
            //  Signal handlers
        private slots:
            void                _onDiskImagePathLineEditTextChanged(QString);
            void                _onBrowsePushButtonClicked();
        };
    }
}

//  End of hadesvm-cereon/Fdc1FloppyDriveEditor.hpp
