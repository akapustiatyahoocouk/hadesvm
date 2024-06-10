TEMPLATE = subdirs

SUBDIRS += \
    hadesvm-core \
    hadesvm-gui \
    hadesvm-kernel \
    hadesvm-util \
    vfd-utils

hadesvm-gui.depends = hadesvm-kernel hadesvm-core hadesvm-util
hadesvm-kernel.depends = hadesvm-core hadesvm-util
hadesvm-core.depends = hadesvm-util
