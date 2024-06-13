//
//  hadesvm-cereon/ResidentRamBlockEditor.hpp
//
//  hadesvm-cereon editor for a ResidentRamBlock component
//
//////////
#pragma once
#include "hadesvm-cereon/API.hpp"

namespace hadesvm
{
    namespace cereon
    {
        //////////
        //  The editor for a ResidentRamBlock component
        namespace Ui { class ResidentRamBlockEditor; }

        class HADESVM_CEREON_PUBLIC ResidentRamBlockEditor final : public hadesvm::core::ComponentEditor
        {
            Q_OBJECT
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(ResidentRamBlockEditor)

            //////////
            //  Conbstruction/destruction
        public:
            ResidentRamBlockEditor(QWidget * parent, ResidentRamBlock * residentRamBlock);
            virtual ~ResidentRamBlockEditor();

            //////////
            //  hadesvm::core::ComponentEditor
        public:
            virtual void        loadComponentConfiguration() override;
            virtual bool        canSaveComponentConfiguration() const override;
            virtual void        saveComponentConfiguration() override;

            //////////
            //  Implementation
        private:
            ResidentRamBlock *const _residentRamBlock;

            //  Helpers
            hadesvm::core::MemorySize::Unit _selectedMemorySizeUnit() const;
            void                _setSelectedMemorySizeUnit(hadesvm::core::MemorySize::Unit unit);

            //////////
            //  Controls & resources
        private:
            Ui::ResidentRamBlockEditor *    _ui;

            //////////
            //  Signal handlers
        private slots:
            void                _onStartAddressLineEditTextChanged(QString);
            void                _onSizeNumberOfUnitsLineEditTextChanged(QString);
            void                _onSizeUnitComboBoxCurrentIndexChanged(int);
        };
    }
}

//  End of hadesvm-cereon/ResidentRamBlockEditor.hpp
