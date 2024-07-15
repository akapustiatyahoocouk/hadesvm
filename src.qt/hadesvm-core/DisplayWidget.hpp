//
//  hadesvm-core/DisplayWidget.hpp
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
        //  Represents a generic UI widget that will represent a
        //  specific component at runtime by occupying the entire
        //  "screen" of a running VA.
        class HADESVM_CORE_PUBLIC DisplayWidget : public QWidget
        {
            Q_OBJECT
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(DisplayWidget)

            //////////
            //  Construction/destruction
        public:
            DisplayWidget(); //  It'll be re-parented by VirtualApplianceWindow anyway
            virtual ~DisplayWidget();

            //////////
            //  Operations
        public:
            //  The user-readable short display name of this DisplayWidget.
            //  This is used e.g. to name a tab in which a DisplayWidget appears
            //  within a tabbed pane.
            virtual QString     displayName() const = 0;

            //  The "control" (context) menu for this DisplayWidget, used ehrn
            //  e.g. a tab representing thios DisplayWidget is right-clicked in
            //  a QTabWidget containing the VA's UI. The caller must NOT delete
            //  this QMenu. Returns nullptr if this DisplayWidget has no
            //  "control" (context) menu.
            virtual QMenu *     controlMenu() const = 0;

            //  TODO document
            virtual QImage      currentImage() const = 0;
        };
    }
}

//  End of hadesvm-core/DisplayWidget.hpp
