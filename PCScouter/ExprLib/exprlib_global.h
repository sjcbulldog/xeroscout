#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(EXPRLIB_LIB)
#  define EXPRLIB_EXPORT Q_DECL_EXPORT
# else
#  define EXPRLIB_EXPORT Q_DECL_IMPORT
# endif
#else
# define EXPRLIB_EXPORT
#endif
