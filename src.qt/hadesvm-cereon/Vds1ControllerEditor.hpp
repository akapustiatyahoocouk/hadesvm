//
//  hadesvm-cereon/Vds1ControllerEditor.hpp
//
//  hadesvm-cereon editor for a Vds1Controller component
//
//////////
#pragma once
#include "hadesvm-cereon/API.hpp"

namespace hadesvm
{
    namespace cereon
    {
        //////////
        //  The editor for a Vds1Controller component
        namespace Ui { class Vds1ControllerEditor; }

        class HADESVM_CEREON_PUBLIC Vds1ControllerEditor final : public hadesvm::core::ComponentEditor
        {
            Q_OBJECT
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Vds1ControllerEditor)

            //////////
            //  Construction/destruction
        public:
            Vds1ControllerEditor(QWidget * parent, Vds1Controller * vds1Controller);
            virtual ~Vds1ControllerEditor();

            //////////
            //  hadesvm::core::ComponentEditor
        public:
            virtual void        loadComponentConfiguration() override;
            virtual bool        canSaveComponentConfiguration() const override;
            virtual void        saveComponentConfiguration() override;

            //////////
            //  Implementation
        private:
            Vds1Controller *const   _vds1Controller;

            //  Helpers

            //////////
            //  Controls & resources
        private:
            Ui::Vds1ControllerEditor *  _ui;

            //  Helpers
            hadesvm::core::ClockFrequency::Unit     _selectedClockFrequencyUnit() const;
            void                _setSelectedClockFrequencyUnit(hadesvm::core::ClockFrequency::Unit unit);

            //////////
            //  Signal handlers
        private slots:
            void                _onStatePortLineEditTextChanged(QString);
            void                _onCommandPortLineEditTextChanged(QString);
            void                _onDataPortLineEditTextChanged(QString);
            void                _onClockNumberOfUnitsLineEditTextChanged(QString);
            void                _onClockUnitComboBoxCurrentIndexChanged(int);
        };
    }
}

//  End of hadesvm-cereon/Vds1ControllerEditor.hpp
