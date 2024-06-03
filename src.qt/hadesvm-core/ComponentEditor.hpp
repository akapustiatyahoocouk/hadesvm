//
//  hadesvm-core/ComponentEditor.hpp
//
//  hadesvm-core component editor support
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
        class HADESVM_CORE_PUBLIC ComponentEditor : public QWidget
        {
            Q_OBJECT
            HADESVM_CANNOT_ASSIGN_OR_COPY_CONSTRUCT(ComponentEditor)

            //////////
            //  Construction/destruction
        public:
            explicit ComponentEditor(QWidget * parent);
            virtual ~ComponentEditor();

            //////////
            //  Operations
        public:
            //  True if the current contents of the editor's controls
            //  can be saved to the configuration of the edited component.
            virtual bool        canSaveComponentConfiguration() const = 0;

            //////////
            //  Signals emitted
        signals:
            //  Raised when the content of some comtrol(s) within
            //  the editor changes
            void                contentChanged();
        };
    }
}

//  End of hadesvm-core/ComponentEditor.hpp
