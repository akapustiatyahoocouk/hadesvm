//
//  hadesvm-gui/PreferencesDialog.hpp
//
//  hadesvm-gui the modal "Preferences" dialog
//
//////////
#pragma once
#include "hadesvm-gui/API.hpp"

namespace hadesvm
{
    namespace gui
    {
        //////////
        //  The modal "Preferences" dialog
        namespace Ui { class PreferencesDialog; }

        class PreferencesDialog final : public QDialog
        {
            Q_OBJECT
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(PreferencesDialog)

            //////////
            //  Construction/destruction
        public:
            explicit PreferencesDialog(QWidget * parent = nullptr);
            ~PreferencesDialog();

            //////////
            //  Controls & resources
        private:
            Ui::PreferencesDialog * _ui;

            //////////
            //  Signal handlers
        private slots:
            void                _accept();
        };
    }
}

//  End of hadesvm-gui/PreferencesDialog.hpp
