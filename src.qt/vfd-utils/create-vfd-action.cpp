//
//  create-vfd-action.cpp - "create VFD" action
//
//////////
#include "main.hpp"

//////////
//  Action
bool CreateVfdAction::execute(Vfd *& currentVfd)
{
    //  Create a new VFD
    Vfd * newVfd = new Vfd(_vfdFileName, _vfdFormat);
    //  If there was a previous VFD, we need to close it...
    delete currentVfd;  //  "delete NULL;" is safe!
    //  ...and set up the new VFD as the "current" one
    currentVfd = newVfd;
    return true;    //  TODO always ?
}

//  End of create-vfd-action.cpp
