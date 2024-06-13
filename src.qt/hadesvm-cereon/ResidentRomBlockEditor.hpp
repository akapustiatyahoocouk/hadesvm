//
//  hadesvm-cereon/ResidentRomBlockEditor.hpp
//
//  hadesvm-cereon editor for a ResidentRomBlock component
//
//////////
#pragma once
#include "hadesvm-cereon/API.hpp"

namespace hadesvm
{
    namespace cereon
    {
        //////////
        //  The editor for a ResidentRomBlock component
        namespace Ui { class ResidentRomBlockEditor; }

        class HADESVM_CEREON_PUBLIC ResidentRomBlockEditor : public hadesvm::core::ComponentEditor
        {
            Q_OBJECT
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(ResidentRomBlockEditor)

            //////////
            //  Conbstruction/destruction
        public:
            explicit ResidentRomBlockEditor(QWidget * parent, ResidentRomBlock * residentRomBlock);
            ~ResidentRomBlockEditor();

            //////////
            //  hadesvm::core::ComponentEditor
        public:
            virtual void        loadComponentConfiguration() override;
            virtual bool        canSaveComponentConfiguration() const override;
            virtual void        saveComponentConfiguration() override;

            //////////
            //  Implementation
        private:
            ResidentRomBlock *const _residentRomBlock;

            //  Helpers
            hadesvm::core::MemorySize::Unit _selectedMemorySizeUnit() const;
            void                _setSelectedMemorySizeUnit(hadesvm::core::MemorySize::Unit unit);

            //////////
            //  Controls & resources
        private:
            Ui::ResidentRomBlockEditor *    _ui;

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

//  End of hadesvm-cereon/ResidentRomBlockEditor.hpp

