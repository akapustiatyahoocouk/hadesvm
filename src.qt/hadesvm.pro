TEMPLATE = subdirs

SUBDIRS += \
    hadesvm-cereon \
    hadesvm-core \
    hadesvm-gui \
    hadesvm-ibmhfp \
    hadesvm-ieee754 \
    hadesvm-kernel \
    hadesvm-util \
    vfd-utils

hadesvm-gui.depends = hadesvm-cereon hadesvm-kernel hadesvm-core hadesvm-util
hadesvm-kernel.depends = hadesvm-core hadesvm-util
hadesvm-cereon.depends = hadesvm-core hadesvm-ieee754 hadesvm-util
hadesvm-core.depends = hadesvm-util
hadesvm-ieee754.depends = hadesvm-util
hadesvm-ibmhfp.depends = hadesvm-util
