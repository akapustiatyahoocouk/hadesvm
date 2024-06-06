//
//  hadesvm-kernel/SystemInterfaces.cpp
//
//  hadesvm::kernel system interface and method names
//
//////////
#include "hadesvm-kernel/API.hpp"
using namespace hadesvm::kernel;

//////////
//  SystemInterfaces::IUnknown
const QString SystemInterfaces::IUnknown::Name = "system::IUnknown";
const QString SystemInterfaces::IUnknown::QueryInterface = "system::IUnknown.QueryInterface()";
const QString SystemInterfaces::IUnknown::HandleOpen = "system::IUnknown.HandleOpen()";
const QString SystemInterfaces::IUnknown::HandleClosed = "system::IUnknown.HandleClosed()";

//  End of hadesvm-kernel/SystemInterfaces.cpp
