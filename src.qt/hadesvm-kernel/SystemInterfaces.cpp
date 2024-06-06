//
//  hadesvm-kernel/SystemInterfaces.cpp
//
//  hadesvm::kernel system interface and service names
//
//////////
#include "hadesvm-kernel/API.hpp"
using namespace hadesvm::kernel;

//////////
//  IUnknown
const QString IUnknown::Name = "system::IUnknown";
const QString IUnknown::QueryInterface = "system::IUnknown.QueryInterface()";
const QString IUnknown::HandleClosed = "system::IUnknown.HandleClosed()";

//  End of hadesvm-kernel/SystemInterfaces.cpp
