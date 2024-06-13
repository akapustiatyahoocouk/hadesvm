//
//  hadesvm-gui/AboutDialog.hpp
//
//  hadesvm-gui the modal "About" dialog
//
//////////
#pragma once
#include "hadesvm-gui/API.hpp"

namespace hadesvm
{
    namespace gui
    {
        //////////
        //  The model "About" dialog
        namespace Ui { class AboutDialog; }

        class AboutDialog final : public QDialog
        {
            Q_OBJECT
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(AboutDialog)

            //////////
            //  Construction/destruction
        public:
            explicit AboutDialog(QWidget * parent = nullptr);
            virtual ~AboutDialog();

            //////////
            //  Controls & resources
        private:
            Ui::AboutDialog *   _ui;
        };
    }
}

//  End of hadesvm-gui/AboutDialog.hpp
