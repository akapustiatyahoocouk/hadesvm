#include <QtCore/qglobal.h>

#if defined(HADESVM_IBM3X0_LIBRARY)
#define HADESVM_IBM3X0_EXPORT Q_DECL_EXPORT
#else
#define HADESVM_IBM3X0_EXPORT Q_DECL_IMPORT
#endif
