//
//  hadesvm-cereon/Cmos1Editor.hpp
//
//  hadesvm-cereon editor for a CMos1 component
//
//////////
#pragma once
#include "hadesvm-cereon/API.hpp"

namespace hadesvm
{
    namespace cereon
    {
        //////////
        //  The editor for a Cmos1 component
        namespace Ui { class Cmos1Editor; }

        class HADESVM_CEREON_PUBLIC Cmos1Editor final : public hadesvm::core::ComponentEditor
        {
            Q_OBJECT
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Cmos1Editor)

            //////////
            //  Construction/destruction
        public:
            Cmos1Editor(QWidget * parent, Cmos1 * cmos1);
            virtual ~Cmos1Editor();

            //////////
            //  hadesvm::core::ComponentEditor
        public:
            virtual void        loadComponentConfiguration() override;
            virtual bool        canSaveComponentConfiguration() const override;
            virtual void        saveComponentConfiguration() override;

            //////////
            //  Implementation
        private:
            Cmos1 *const        _cmos1;

            //  Helpers
            hadesvm::core::ClockFrequency::Unit     _selectedClockFrequencyUnit() const;
            void                _setSelectedClockFrequencyUnit(hadesvm::core::ClockFrequency::Unit unit);

            hadesvm::core::TimeInterval::Unit       _selectedReadDelayUnit() const;
            void                _setSelectedReadDelayUnit(hadesvm::core::TimeInterval::Unit unit);

            hadesvm::core::TimeInterval::Unit       _selectedWriteDelayUnit() const;
            void                _setSelectedWriteDelayUnit(hadesvm::core::TimeInterval::Unit unit);

            //////////
            //  Controls & resources
        private:
            Ui::Cmos1Editor *   _ui;

            //////////
            //  Signal handlers
        private slots:
            void                _onClockNumberOfUnitsLineEditTextChanged(QString);
            void                _onClockUnitComboBoxCurrentIndexChanged(int);
            void                _onReadDelayNumberOfUnitsLineEditTextChanged(QString);
            void                _onReadDelayUnitComboBoxCurrentIndexChanged(int);
            void                _onWriteDelayNumberOfUnitsLineEditTextChanged(QString);
            void                _onWriteDelayUnitComboBoxCurrentIndexChanged(int);
            void                _onStatePortLineEditTextChanged(QString);
            void                _onAddressPortLineEditTextChanged(QString);
            void                _onDataPortLineEditTextChanged(QString);
            void                _onInterruptMaskPortLineEditTextChanged(QString);
            void                _onBrowsePushButtonClicked();
        };
    }
}

//  End of hadesvm-cereon/Cmos1Editor.hpp
