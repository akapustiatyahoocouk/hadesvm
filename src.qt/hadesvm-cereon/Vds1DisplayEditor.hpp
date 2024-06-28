//
//  hadesvm-cereon/Vds1DisplayEditor.hpp
//
//  hadesvm-cereon editor for a Vds1Display component
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
        namespace Ui { class Vds1DisplayEditor; }

        class HADESVM_CEREON_PUBLIC Vds1DisplayEditor final : public hadesvm::core::ComponentEditor
        {
            Q_OBJECT
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Vds1DisplayEditor)

            //////////
            //  Construction/destruction
        public:
            Vds1DisplayEditor(QWidget * parent, Vds1Display * vds1Display);
            ~Vds1DisplayEditor();

            //////////
            //  hadesvm::core::ComponentEditor
        public:
            virtual void        loadComponentConfiguration() override;
            virtual bool        canSaveComponentConfiguration() const override;
            virtual void        saveComponentConfiguration() override;

            //////////
            //  Implementation
        private:
            Vds1Display *const  _vds1Display;

            //////////
            //  Controls & resources
        private:
            Ui::Vds1DisplayEditor * _ui;

            //////////
            //  Signal handlers
        private slots:
            void                _onControllerStatePortLineEditTextChanged(QString);
        };
    }
}

//  End of hadesvm-cereon/Vds1DisplayEditor.hpp
