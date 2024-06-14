//
//  hadesvm-cereon/ProcessorEditor.hpp
//
//  hadesvm-cereon editor for a Processor component
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
        namespace Ui { class ProcessorEditor; }

        class HADESVM_CEREON_PUBLIC ProcessorEditor final : public hadesvm::core::ComponentEditor
        {
            Q_OBJECT
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(ProcessorEditor)

            //////////
            //  Construction/destruction
        public:
            ProcessorEditor(QWidget * parent, Processor * processor);
            virtual ~ProcessorEditor();

            //////////
            //  hadesvm::core::ComponentEditor
        public:
            virtual void        loadComponentConfiguration() override;
            virtual bool        canSaveComponentConfiguration() const override;
            virtual void        saveComponentConfiguration() override;

            //////////
            //  Implementation
        private:
            Processor *const    _processor;

            //  Helpers
            hadesvm::core::ClockFrequency::Unit     _selectedClockFrequencyUnit() const;
            void                _setSelectedClockFrequencyUnit(hadesvm::core::ClockFrequency::Unit unit);

            //////////
            //  Controls & resources
        private:
            Ui::ProcessorEditor *   _ui;

            //////////
            //  Signal handlers
        private slots:
            void                _onIdLineEditTextChanged(QString);
            void                _onPrimaryCheckBoxToggled(bool);
            void                _onRestartAddressLineEditTextChanged(QString);
            void                _onClockNumberOfUnitsLineEditTextChanged(QString);
            void                _onClockUnitComboBoxCurrentIndexChanged(int);
            void                _onBigEndianRadioButtonClicked();
            void                _onLittleEndianRadioButtonClicked();
        };
    }
}

//  End of hadesvm-cereon/ProcessorEditor.hpp

