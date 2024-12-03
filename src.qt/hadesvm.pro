TEMPLATE = subdirs

SUBDIRS += \
    hadesvm-cereon \
    hadesvm-core \
    hadesvm-gui \
    hadesvm-ibm3x0 \
    hadesvm-ibmhfp \
    hadesvm-ieee754 \
    hadesvm-kernel \
    hadesvm-util \
    vfd-utils

hadesvm-gui.depends = hadesvm-ibm3x0 hadesvm-cereon hadesvm-kernel hadesvm-core hadesvm-util
hadesvm-kernel.depends = hadesvm-core hadesvm-util
hadesvm-ibm3x0.depends = hadesvm-core hadesvm-ibmhfp hadesvm-util
hadesvm-cereon.depends = hadesvm-core hadesvm-ieee754 hadesvm-util
hadesvm-core.depends = hadesvm-util
hadesvm-ieee754.depends = hadesvm-util
hadesvm-ibmhfp.depends = hadesvm-util
