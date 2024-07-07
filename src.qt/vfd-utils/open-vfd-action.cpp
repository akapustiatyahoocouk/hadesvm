//
//  open-vfd_action.cpp - "open VFD" action
//
//////////
#include "main.hpp"

//////////
//  Action
bool OpenVfdAction::execute(Vfd *& currentVfd)
{
    //  OPen a VFD
    Vfd * newVfd = new Vfd(_vfdFileName);
    //  If there was a previous VFD, we need to close it...
    delete currentVfd;  //  "delete NULL;" is safe!
    //  ...and set up the new VFD as the "current" one
    currentVfd = newVfd;

    return true;    //  TODO always ?
}

//  End of open-vfd_action.cpp
