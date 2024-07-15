//
//  hadesvm-cereon/Kis1ControllerEditor.hpp
//
//  hadesvm-cereon editor for a Kis1Controller component
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
        namespace Ui { class Kis1ControllerEditor; }

        class HADESVM_CEREON_PUBLIC Kis1ControllerEditor final : public hadesvm::core::ComponentEditor
        {
            Q_OBJECT
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Kis1ControllerEditor)

            //////////
            //  Construction/destruction
        public:
            explicit Kis1ControllerEditor(Kis1Controller * kis1Controller);
            virtual ~Kis1ControllerEditor();

            //////////
            //  hadesvm::core::ComponentEditor
        public:
            virtual void        loadComponentConfiguration() override;
            virtual bool        canSaveComponentConfiguration() const override;
            virtual void        saveComponentConfiguration() override;

            //////////
            //  Implementation
        private:
            Kis1Controller *const   _kis1Controller;

            //  Helpers
            hadesvm::core::ClockFrequency::Unit     _selectedClockFrequencyUnit() const;
            void                _setSelectedClockFrequencyUnit(hadesvm::core::ClockFrequency::Unit unit);

            //////////
            //  Controls & resources
        private:
            Ui::Kis1ControllerEditor *  _ui;

            //////////
            //  Signal handlers
        private slots:
            void                _onStatePortLineEditTextChanged(QString);
            void                _onCurrentDevicePortLineEditTextChanged(QString);
            void                _onInterruptMaskPortLineEditTextChanged(QString);
            void                _onInputSourcePortLineEditTextChanged(QString);
            void                _onDeviceStatePortLineEditTextChanged(QString);
            void                _onDataInPortLineEditTextChanged(QString);
            void                _onLayoutPortLineEditTextChanged(QString);
            void                _onClockNumberOfUnitsLineEditTextChanged(QString);
            void                _onClockUnitComboBoxCurrentIndexChanged(int);
        };
    }
}

//  End of hadesvm-cereon/Kis1ControllerEditor.hpp
