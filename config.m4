PHP_ARG_ENABLE(infusion, for Infusion support, [  --enable-infusion          Enable Infusion support])

if test "$PHP_INFUSION" != "no"; then
  AC_DEFINE(HAVE_INFUSION, 1, [ ])
  PHP_NEW_EXTENSION(infusion, infusion.c, $ext_shared)
fi
