//
//  hadesvm-gui/Preferences.cpp
//
//  hadesvm::gui::Preferences class implementation
//
//////////
#include "hadesvm-gui/API.hpp"
using namespace hadesvm::gui;

static QSettings settings;

//////////
//  Start-up
bool Preferences::startMinimized()
{
    return settings.value("StartMinimized", false).toBool();
}

void Preferences::setStartMinimized(bool startMinimized)
{
    settings.setValue("StartMinimized", startMinimized);
}

//  End of hadesvm-gui/Preferences.cpp
