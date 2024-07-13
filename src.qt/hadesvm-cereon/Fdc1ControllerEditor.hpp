//
//  hadesvm-cereon/Fdc1ControllerEditor.hpp
//
//  hadesvm-cereon editor for a Fdc1Controller component
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
        namespace Ui { class Fdc1ControllerEditor; }

        class HADESVM_CEREON_PUBLIC Fdc1ControllerEditor final : public hadesvm::core::ComponentEditor
        {
            Q_OBJECT

            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Fdc1ControllerEditor)

            //////////
            //  Construction/destruction
        public:
            Fdc1ControllerEditor(QWidget * parent, Fdc1Controller * fdc1Controller);
            virtual ~Fdc1ControllerEditor();

            //////////
            //  hadesvm::core::ComponentEditor
        public:
            virtual void        loadComponentConfiguration() override;
            virtual bool        canSaveComponentConfiguration() const override;
            virtual void        saveComponentConfiguration() override;

            //////////
            //  Implementation
        private:
            Fdc1Controller *const   _fdc1Controller;

            //  Helpers
            hadesvm::core::ClockFrequency::Unit     _selectedClockFrequencyUnit() const;
            void                _setSelectedClockFrequencyUnit(hadesvm::core::ClockFrequency::Unit unit);

            //////////
            //  Controls & resources
        private:
            Ui::Fdc1ControllerEditor *  _ui;

            //////////
            //  Signal handlers
        private slots:
            void                _onStateAndControlPortLineEditTextChanged(QString);
            void                _onDataPortLineEditTextChanged(QString);
            void                _onInterruptMaskPortLineEditTextChanged(QString);
            void                _onClockNumberOfUnitsLineEditTextChanged(QString);
            void                _onClockUnitComboBoxCurrentIndexChanged(int);
        };
    }
}

//  End of hadesvm-cereon/Fdc1ControllerEditor.hpp
