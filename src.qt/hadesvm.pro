TEMPLATE = subdirs

SUBDIRS += \
    hadesvm-core \
    hadesvm-gui \
    hadesvm-ibmhfp \
    hadesvm-ieee754 \
    hadesvm-kernel \
    hadesvm-util \
    vfd-utils

hadesvm-gui.depends = hadesvm-kernel hadesvm-core hadesvm-util
hadesvm-kernel.depends = hadesvm-core hadesvm-util
hadesvm-core.depends = hadesvm-util
hadesvm-ieee754.depends = hadesvm-util
hadesvm-ibmhfp.depends = hadesvm-util
