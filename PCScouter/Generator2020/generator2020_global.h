#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(GENERATOR2020_LIB)
#  define GENERATOR2020_EXPORT Q_DECL_EXPORT
# else
#  define GENERATOR2020_EXPORT Q_DECL_IMPORT
# endif
#else
# define GENERATOR2020_EXPORT
#endif
