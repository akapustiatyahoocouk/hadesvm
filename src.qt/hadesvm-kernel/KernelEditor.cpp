//
//  hadesvm-kernel/KernelEditor.cpp
//
//  hadesvm::kernel::KernelEditor class implementation
//
//////////
#include "hadesvm-kernel/API.hpp"
using namespace hadesvm::kernel;
#include "ui_KernelEditor.h"

//////////
//  Construction/destruction
KernelEditor::KernelEditor(QWidget * parent, Kernel * kernel)
    :   hadesvm::core::ComponentEditor(parent),
        //  Implementation
        _kernel(kernel),
        //  Controls & resources
        _ui(new Ui::KernelEditor)
{
    Q_ASSERT(_kernel != nullptr);
    _ui->setupUi(this);
}

KernelEditor::~KernelEditor()
{
    delete _ui;
}

//////////
//  hadesvm::core::ComponentEditor
bool KernelEditor::canSaveComponentConfiguration() const
{
    return true;
}

//  End of hadesvm-kernel/KernelEditor.cpp
