//
//  hadesvm-cereon/Fdc1FloppyDriveStatsBarWidget.hpp
//
//  hadesvm-cereon StatusBarWidget for a Fdc1FloppyDrive component
//
//////////
#pragma once
#include "hadesvm-cereon/API.hpp"

namespace hadesvm
{
    namespace cereon
    {
        //////////
        //  The StatusBarWidget for a Fdc1FloppyDrive component
        namespace Ui { class Fdc1FloppyDriveStatusBarWidget; }

        class HADESVM_CEREON_PUBLIC Fdc1FloppyDriveStatusBarWidget final : public hadesvm::core::StatusBarWidget
        {
            Q_OBJECT
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(Fdc1FloppyDriveStatusBarWidget)

            //////////
            //  Construction/destruction
        public:
            explicit Fdc1FloppyDriveStatusBarWidget();
            virtual ~Fdc1FloppyDriveStatusBarWidget();

            //////////
            //  Controls & resources
        private:
            Ui::Fdc1FloppyDriveStatusBarWidget *    _ui;
        };
    }
}

//  End of hadesvm-cereon/Fdc1FloppyDriveStatsBarWidget.hpp
