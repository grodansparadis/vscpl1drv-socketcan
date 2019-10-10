dnl Autoconf settings for vscpl1drv-socketcan
dnl $Id: configure.in,v 1.5 2017/10/13 11:22:06 akhe Exp $

AC_INIT( [vscpl1drv-socketcan], [14.0.0], [akhe@grodansparadis.com])

CONFIGURE_LINE="$0 $*"
CODENAME="Ant"

AC_CONFIG_MACRO_DIR(m4)

AC_PREREQ(2.57)
AC_CONFIG_SRCDIR([linux/socketcanobj.cpp])
AC_CONFIG_AUX_DIR(autotools)
AC_CONFIG_HEADERS([config.h])

AX_CHECK_ENABLE_DEBUG()

# Checks for programs.
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_YACC
AC_PROG_CC
AC_PROG_CPP
AC_PROG_LEX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB

# Check for needed libraries.
AC_CHECK_LIB([dl], [dlsym])
AC_CHECK_LIB([pthread], [pthread_create])
AC_CHECK_LIB([m],[sin])

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([OS.h arpa/inet.h fcntl.h inttypes.h limits.h locale.h netdb.h netinet/in.h sgtty.h stddef.h stdint.h stdlib.h string.h strings.h sys/file.h sys/ioctl.h sys/socket.h sys/time.h sys/timeb.h syslog.h termios.h unistd.h openssl/md5.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM
AC_C_VOLATILE


# Checks for library functions.
AC_FUNC_FORK
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_REALLOC
AC_FUNC_SELECT_ARGTYPES
AC_FUNC_SETVBUF_REVERSED
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_FUNC_STRFTIME
AC_FUNC_STRTOD
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([alarm bzero dup2 floor gethostbyname gethostname gethrtime gettimeofday inet_ntoa memchr memmove memset mkfifo pow select setlocale socket sqrt strcasecmp strchr strcspn strdup strerror strncasecmp strpbrk strrchr strspn strstr strtol strtoul])

dnl bigendian check
AC_C_BIGENDIAN(
     [AC_DEFINE([WORDS_BIGENDIAN], 1, [Define for endiness.])],
     [AC_DEFINE([WORDS_BIGENDIAN], 0, [Define for endiness.])])

dnl compile 32-bit/64-bit
AC_CHECK_SIZEOF([long])
AS_IF([test "$ac_cv_sizeof_long" -eq 8],
    [AC_DEFINE([OSBIT], 64, [Define to the size of a long in bits.])],
    [AC_DEFINE([OSBIT], 32, [Define to the size of a long in bits.])])

AC_SUBST(VSCPDIR)

dnl
dnl Project base installation folder
dnl

AC_ARG_VAR(VSCPDIR, Set base folder for installation (/srv/vscp) )
if test "${VSCPDIR+set}" = set; then
  echo "variable \$VSCPDIR is set to: $VSCPDIR"
else
  VSCPDIR="/srv/vscp"
  echo "variable \$VSCPDIR is set to: $VSCPDIR"
fi

dnl
dnl Project Sub directories
dnl

PROJ_SUBDIRS="linux "

AC_SUBST(PROJ_SUBDIRS)

dnl
dnl Construct Makefiles
dnl

AC_CONFIG_FILES([Makefile
         m4/Makefile
         linux/Makefile
])

EXTRALIBS="${EXTRALIBS} ${LIBS}"

dnl Generate makefiles
AC_OUTPUT

dnl Output some debugging information
echo   "LIBS          : ${LIBS}"
echo   "EXTRALIBS     : ${EXTRALIBS}"
echo   "EXTRACPPFLAGS : ${CPPFLAGS}"
echo   "EXTRACXXFLAGS : ${CXXFLAGS}"
echo   "EXTRACFLAGS   : ${CFLAGS}"
echo   "PROJ_SUBDIRS  : ${PROJ_SUBDIRS}"

dnl Give user some feedback
echo
echo   "           VSCP configuration"
echo   "--------------------------------------------"
echo   "VSCP version          : `sh echo-version` (${PACKAGE_VERSION})"
echo   "System                : `uname -a` "
echo   "VSCP install dir      : ${VSCPDIR}"

dnl echo   "${ax_enable_debug}"
if test "${ax_enable_debug}" = "yes"
then
   echo "build flavour         : debug "
else
   echo "build flavour         : release "
fi

dnl vi: set et sts=4 sw=4 com=\:dnl
