//
//  hadesvm-core/StatusBarWidget.hpp
//
//  hadesvm-core display widget support
//
//////////
#pragma once
#include "hadesvm-core/API.hpp"

namespace hadesvm
{
    namespace core
    {
        //////////
        //  Represents a small widget that will appear at runtime in
        //  the VA's status bar.
        //  Normally, the comcrete subclass will set its sizeHint, size,
        //  minimumSize and maximumSize to some small fixed value (e.g.
        //  16x16 to accommodate a small icon representing the component
        //  in a VA UI's status bar
        class HADESVM_CORE_PUBLIC StatusBarWidget : public QWidget
        {
            Q_OBJECT
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(StatusBarWidget)

            //////////
            //  Construction/destruction
        public:
            StatusBarWidget(); //  It'll be re-parented by VirtualApplianceWindow anyway
            virtual ~StatusBarWidget();

            //////////
            //  Operations
        public:
            //  The user-readable short display name of this StatusBarWidget.
            //  This is used e.g. as a tooltip for the widget.
            virtual QString     displayName() const = 0;
        };
    }
}

//  End of hadesvm-core/StatusBarWidget.hpp
