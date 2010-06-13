/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_0.txt.                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Robert Eisele <robert@xarg.org>                              |
  | Site: http://www.xarg.org/project/php-infusion/                      |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_INFUSION_H
#define PHP_INFUSION_H

#define PHP_INFUSION_VERSION "1.1"
#define PHP_INFUSION_EXTNAME "infusion"

#ifdef ZTS
#  include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(infusion);
PHP_MINFO_FUNCTION(infusion);

static inline void php_register_super_global(char *s, int len);
static inline double php_intpow10(int power);
static int array_reverse_data_compare(const void *a, const void *b TSRMLS_DC);

PHP_FUNCTION(isbit);
PHP_FUNCTION(setbit);
PHP_FUNCTION(invbit);
PHP_FUNCTION(numbit);
PHP_FUNCTION(msbit);

PHP_FUNCTION(getint);
PHP_FUNCTION(setint);

PHP_FUNCTION(limit);
PHP_FUNCTION(bound);

PHP_FUNCTION(sgn);
PHP_FUNCTION(gpp);
PHP_FUNCTION(sigfig);
PHP_FUNCTION(checksum);
PHP_FUNCTION(bround);
PHP_FUNCTION(xround);

PHP_FUNCTION(strcut);
PHP_FUNCTION(stroff);
PHP_FUNCTION(truncate);
PHP_FUNCTION(isuc);
PHP_FUNCTION(islc);
PHP_FUNCTION(xsprintf);
PHP_FUNCTION(strcal);
PHP_FUNCTION(strical);
PHP_FUNCTION(strmap);

PHP_FUNCTION(number_chop);
PHP_FUNCTION(time_chop);

PHP_FUNCTION(kimplode);

PHP_FUNCTION(typeof);
PHP_FUNCTION(is_ref);

extern zend_module_entry infusion_module_entry;
#define phpext_infusion_ptr &infusion_module_entry

#endif
