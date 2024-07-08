//
//  hadesvm-gui/Preferences.hpp
//
//  hadesvm-gui application preferences
//
//////////
#include "hadesvm-gui/API.hpp"

namespace hadesvm
{
    namespace gui
    {
        //////////
        //  hadesvm-gui application preferences
        class Preferences final
        {
            HADESVM_UTILITY_CLASS(Preferences)

            //////////
            //  General
        public:

            //////////
            //  Start-up
        public:
            static bool     startMinimized();
            static void     setStartMinimized(bool startMinimized);
        };
    }
}

//  End of hadesvm-gui/Preferences.hpp
