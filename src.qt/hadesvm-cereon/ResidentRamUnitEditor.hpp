//
//  hadesvm-cereon/ResidentRamUnitEditor.hpp
//
//  hadesvm-cereon editor for a ResidentRamUnit component
//
//////////
#pragma once
#include "hadesvm-cereon/API.hpp"

namespace hadesvm
{
    namespace cereon
    {
        //////////
        //  The editor for a ResidentRamUnit component
        namespace Ui { class ResidentRamUnitEditor; }

        class HADESVM_CEREON_PUBLIC ResidentRamUnitEditor final : public hadesvm::core::ComponentEditor
        {
            Q_OBJECT
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(ResidentRamUnitEditor)

            //////////
            //  Construction/destruction
        public:
            explicit ResidentRamUnitEditor(ResidentRamUnit * residentRamUnit);
            virtual ~ResidentRamUnitEditor();

            //////////
            //  hadesvm::core::ComponentEditor
        public:
            virtual void        loadComponentConfiguration() override;
            virtual bool        canSaveComponentConfiguration() const override;
            virtual void        saveComponentConfiguration() override;

            //////////
            //  Implementation
        private:
            ResidentRamUnit *const _residentRamUnit;

            //  Helpers
            hadesvm::core::MemorySize::Unit _selectedMemorySizeUnit() const;
            void                _setSelectedMemorySizeUnit(hadesvm::core::MemorySize::Unit unit);

            //////////
            //  Controls & resources
        private:
            Ui::ResidentRamUnitEditor * _ui;

            //////////
            //  Signal handlers
        private slots:
            void                _onStartAddressLineEditTextChanged(QString);
            void                _onSizeNumberOfUnitsLineEditTextChanged(QString);
            void                _onSizeUnitComboBoxCurrentIndexChanged(int);
        };
    }
}

//  End of hadesvm-cereon/ResidentRamUnitEditor.hpp
