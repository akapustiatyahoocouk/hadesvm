//
//  hadesvm-cereon/Kis1KayboardEditor.hpp
//
//  hadesvm-cereon editor for a Kis1Kayboard component
//
//////////
#pragma once
#include "hadesvm-cereon/API.hpp"

namespace hadesvm
{
    namespace cereon
    {
        //////////
        //  The editor for a Kis1Keyboard component
        namespace Ui { class Kis1KeyboardEditor; }

        class HADESVM_CEREON_PUBLIC Kis1KeyboardEditor final : public hadesvm::core::ComponentEditor
        {
            Q_OBJECT
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Kis1KeyboardEditor)

            //////////
            //  Construction/destruction
        public:
            explicit Kis1KeyboardEditor(Kis1Keyboard * kis1Keyboard);
            virtual ~Kis1KeyboardEditor();

            //////////
            //  hadesvm::core::ComponentEditor
        public:
            virtual void        loadComponentConfiguration() override;
            virtual bool        canSaveComponentConfiguration() const override;
            virtual void        saveComponentConfiguration() override;

            //////////
            //  Implementation
        private:
            Kis1Keyboard *const _kis1Keyboard;
            QList<Kis1KeyboardLayout*>  _keyboardLayouts;

            //////////
            //  Controls & resources
        private:
            Ui::Kis1KeyboardEditor *    _ui;

            //////////
            //  Signal handlers
        private slots:
            void                _onControllerStatePortLineEditTextChanged(QString);
        };
    }
}

//  End of hadesvm-cereon/Kis1KayboardEditor.hpp
