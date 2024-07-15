//
//  hadesvm-cereon/MemoryBusEditor.hpp
//
//  hadesvm-cereon editor for a MemoryBus component
//
//////////
#pragma once
#include "hadesvm-cereon/API.hpp"

namespace hadesvm
{
    namespace cereon
    {
        //////////
        //  The editor for a MemoryBus component
        namespace Ui { class MemoryBusEditor; }

        class HADESVM_CEREON_PUBLIC MemoryBusEditor final : public hadesvm::core::ComponentEditor
        {
            Q_OBJECT
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(MemoryBusEditor)

            //////////
            //  Construction/destruction
        public:
            explicit MemoryBusEditor(MemoryBus * memoryBus);
            virtual ~MemoryBusEditor();

            //////////
            //  hadesvm::core::ComponentEditor
        public:
            virtual void        loadComponentConfiguration() override;
            virtual bool        canSaveComponentConfiguration() const override;
            virtual void        saveComponentConfiguration() override;

            //////////
            //  Implementation
        private:
            MemoryBus *const    _memoryBus;

            //  Helpers
            hadesvm::core::ClockFrequency::Unit     _selectedClockFrequencyUnit() const;
            void                _setSelectedClockFrequencyUnit(hadesvm::core::ClockFrequency::Unit unit);

            //////////
            //  Controls & resources
        private:
            Ui::MemoryBusEditor *   _ui;

            //////////
            //  Signal handlers
        private slots:
            void                _onClockNumberOfUnitsLineEditTextChanged(QString);
            void                _onClockUnitComboBoxCurrentIndexChanged(int);
        };
    }
}

//  End of hadesvm-cereon/MemoryBusEditor.hpp
