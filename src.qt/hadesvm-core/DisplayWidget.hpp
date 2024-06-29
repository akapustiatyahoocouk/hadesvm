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
        //  Represents a generic VM component editor - a UI widget
        //  that can display configuration of a specific component
        //  and allows the user to modify that configuration.
        class HADESVM_CORE_PUBLIC DisplayWidget : public QWidget
        {
            Q_OBJECT
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(DisplayWidget)

            //////////
            //  Construction/destruction
        public:
            explicit DisplayWidget(QWidget * parent); //  TODO can we create the widget WITHOUT the parent? It'll be re-parented by VirtualApplianceWindow anyway
            virtual ~DisplayWidget();

            //////////
            //  Operations
        public:
            //  The user-readable short display name of this DisplayWidget.
            //  This is used e.g. to name a tab in which a DisplayWidget appears
            //  qithin a tabbed pane.
            virtual QString     displayName() const = 0;

            //  The "control" (context) menu for this DisplayWidget, used ehrn
            //  e.g. a tab representing thios DisplayWidget is right-clicked in
            //  a QTabWidget containing the VA's UI. The caller must NOT delete
            //  this QMenu. Returns nullptr if this DisplayWidget has no
            //  "control" (context) menu.
            virtual QMenu *     controlMenu() const = 0;
        };
    }
}

//  End of hadesvm-core/DisplayWidget.hpp
