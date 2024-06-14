//
//  hadesvm-cereon/IoBusEditor.hpp
//
//  hadesvm-cereon editor for a IoBus component
//
//////////
#pragma once
#include "hadesvm-cereon/API.hpp"

namespace hadesvm
{
    namespace cereon
    {
        //////////
        //  The editor for a IoBus component
        namespace Ui { class IoBusEditor; }

        class HADESVM_CEREON_PUBLIC IoBusEditor final : public hadesvm::core::ComponentEditor
        {
            Q_OBJECT
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(IoBusEditor)

            //////////
            //  Construction/destruction
        public:
            IoBusEditor(QWidget * parent, IoBus * ioBus);
            virtual ~IoBusEditor();

            //////////
            //  hadesvm::core::ComponentEditor
        public:
            virtual void        loadComponentConfiguration() override;
            virtual bool        canSaveComponentConfiguration() const override;
            virtual void        saveComponentConfiguration() override;

            //////////
            //  Implementation
        private:
            IoBus *const        _ioBus;

            //  Helpers
            hadesvm::core::ClockFrequency::Unit     _selectedClockFrequencyUnit() const;
            void                _setSelectedClockFrequencyUnit(hadesvm::core::ClockFrequency::Unit unit);

            //////////
            //  Controls & resources
        private:
            Ui::IoBusEditor *   _ui;

            //////////
            //  Signal handlers
        private slots:
            void                _onClockNumberOfUnitsLineEditTextChanged(QString);
            void                _onClockUnitComboBoxCurrentIndexChanged(int);
        };
    }
}

//  End of hadesvm-cereon/IoBusEditor.hpp
