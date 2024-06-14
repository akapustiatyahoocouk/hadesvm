//
//  hadesvm-cereon/ResidentRomUnitEditor.hpp
//
//  hadesvm-cereon editor for a ResidentRomUnit component
//
//////////
#pragma once
#include "hadesvm-cereon/API.hpp"

namespace hadesvm
{
    namespace cereon
    {
        //////////
        //  The editor for a ResidentRomUnit component
        namespace Ui { class ResidentRomUnitEditor; }

        class HADESVM_CEREON_PUBLIC ResidentRomUnitEditor : public hadesvm::core::ComponentEditor
        {
            Q_OBJECT
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(ResidentRomUnitEditor)

            //////////
            //  Construction/destruction
        public:
            explicit ResidentRomUnitEditor(QWidget * parent, ResidentRomUnit * residentRomUnit);
            ~ResidentRomUnitEditor();

            //////////
            //  hadesvm::core::ComponentEditor
        public:
            virtual void        loadComponentConfiguration() override;
            virtual bool        canSaveComponentConfiguration() const override;
            virtual void        saveComponentConfiguration() override;

            //////////
            //  Implementation
        private:
            ResidentRomUnit *const _residentRomUnit;

            //  Helpers
            hadesvm::core::MemorySize::Unit _selectedMemorySizeUnit() const;
            void                _setSelectedMemorySizeUnit(hadesvm::core::MemorySize::Unit unit);

            //////////
            //  Controls & resources
        private:
            Ui::ResidentRomUnitEditor * _ui;

            //////////
            //  Signal handlers
        private slots:
            void                _onStartAddressLineEditTextChanged(QString);
            void                _onSizeNumberOfUnitsLineEditTextChanged(QString);
            void                _onSizeUnitComboBoxCurrentIndexChanged(int);
            void                _onContentLineEditTextChanged(QString);
            void                _onBrowsePushButtonClicked();
        };
    }
}

//  End of hadesvm-cereon/ResidentRomUnitEditor.hpp

