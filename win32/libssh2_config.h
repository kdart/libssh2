#ifndef LIBSSH2_CONFIG_H
#define LIBSSH2_CONFIG_H

#ifndef WIN32
#define WIN32
#endif

#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE 1
#endif

#define HAVE_LIBCRYPT32
#define HAVE_WINSOCK2_H
#define HAVE_IOCTLSOCKET
#define HAVE_SELECT
#define HAVE_SNPRINTF

#ifdef __MINGW32__
# define HAVE_UNISTD_H
# define HAVE_INTTYPES_H
# define HAVE_SYS_TIME_H
# define HAVE_GETTIMEOFDAY
#elif defined(_MSC_VER)
# if _MSC_VER < 1900
#  undef HAVE_SNPRINTF
#  if _MSC_VER < 1500
#   define vsnprintf _vsnprintf
#  endif
#  define strdup _strdup
#  define strncasecmp _strnicmp
#  define strcasecmp _stricmp
# endif
#else
# define strncasecmp strnicmp
# define strcasecmp stricmp
#endif

/* Enable newer diffie-hellman-group-exchange-sha1 syntax */
#define LIBSSH2_DH_GEX_NEW 1

#endif /* LIBSSH2_CONFIG_H */
