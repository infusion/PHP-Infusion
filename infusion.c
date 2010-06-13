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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "php.h"
#include "php_ini.h"
#include "php_infusion.h"
#include "ext/standard/php_smart_str.h"

ZEND_BEGIN_ARG_INFO(arginfo_is_ref, 0)
ZEND_ARG_INFO(1, var)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_typeof, 0)
ZEND_ARG_INFO(0, var)
ZEND_END_ARG_INFO()


/* {{{ infusion_functions[]
 *
 * Every user visible function must have an entry in infusion_functions[].
 */
static function_entry infusion_functions[] = {
   PHP_FE(isbit, NULL)
   PHP_FE(setbit, NULL)
   PHP_FE(invbit, NULL)
   PHP_FE(numbit, NULL)
   PHP_FE(msbit, NULL)

   PHP_FE(getint, NULL)
   PHP_FE(setint, NULL)

   PHP_FE(limit, NULL)
   PHP_FE(bound, NULL)

   PHP_FE(sgn, NULL)
   PHP_FE(gpp, NULL)
   PHP_FE(sigfig, NULL)
   PHP_FE(checksum, NULL)
   PHP_FE(bround, NULL)
   PHP_FE(xround, NULL)

   PHP_FE(strcut, NULL)
   PHP_FE(stroff, NULL)
   PHP_FE(truncate, NULL)
   PHP_FE(isuc, NULL)
   PHP_FE(islc, NULL)
   PHP_FE(xsprintf, NULL)
   PHP_FE(strcal, NULL)
   PHP_FE(strical, NULL)
   PHP_FE(strmap, NULL)

   PHP_FE(number_chop, NULL)
   PHP_FE(time_chop, NULL)

   PHP_FE(kimplode, NULL)

   PHP_FE(typeof, arginfo_typeof)
   PHP_FE(is_ref, arginfo_is_ref)

   {NULL, NULL, NULL}
};
/* }}} */

/* {{{ infusion_module_entry
 */
zend_module_entry infusion_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
   STANDARD_MODULE_HEADER,
#endif
   PHP_INFUSION_EXTNAME,
   infusion_functions,
   NULL,
   NULL,
   NULL,
   NULL,
   PHP_MINFO(infusion),
#if ZEND_MODULE_API_NO >= 20010901
   PHP_INFUSION_VERSION,
#endif
   STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_INFUSION
ZEND_GET_MODULE(infusion)
#endif

static inline void php_register_super_global(char *s, int len)
{
#ifdef ZEND_ENGINE_2
	if (zend_register_auto_global(s, len, NULL TSRMLS_CC) == SUCCESS) {
		zend_auto_global_disable_jit(s, len TSRMLS_CC);
#else
	if (zend_register_auto_global(s, len TSRMLS_CC) == SUCCESS) {
#endif
	}
}

static inline double php_intpow10(int power)
{
	static const double powers[] = {
		1e0,  1e1,  1e2,  1e3,  1e4,  1e5,  1e6,  1e7,
		1e8,  1e9,  1e10, 1e11, 1e12, 1e13, 1e14, 1e15,
		1e16, 1e17, 1e18, 1e19, 1e20, 1e21, 1e22
	};

	/* Not in lookup table */
	if (power < 0 || power > 22) {
		return pow(10.0, (double)power);
	}
	return powers[power];
}

static int array_reverse_data_compare(const void *a, const void *b TSRMLS_DC)
{
	Bucket *f;
	Bucket *s;
	zval result;
	zval *first;
	zval *second;

	f = *((Bucket **) a);
	s = *((Bucket **) b);

	first = *((zval **) f->pData);
	second = *((zval **) s->pData);

	if (numeric_compare_function(&result, first, second TSRMLS_CC) == FAILURE) {
		return 0;
	}

	if (Z_TYPE(result) == IS_DOUBLE) {
		if (Z_DVAL(result) < 0) {
			return 1;
		} else if (Z_DVAL(result) > 0) {
			return -1;
		} else {
			return 0;
		}
	}

	convert_to_long(&result);

	if (Z_LVAL(result) < 0) {
		return 1;
	} else if (Z_LVAL(result) > 0) {
		return -1;
	}

	return 0;
}

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(infusion)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "infusion support", "enabled");
	php_info_print_table_row(2, "infusion modules", "binary, math, string");
	php_info_print_table_row(2, "infusion version", PHP_INFUSION_VERSION);
	php_info_print_table_end();
}
/* }}} */


/* Binary Extension */

/* {{{ proto bool isbit(int mask, int pos)
   Check if a bit-flag is set in the number */
PHP_FUNCTION(isbit)
{
	long bit, n;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &bit, &n) == FAILURE) {
		RETURN_NULL();
	}

	RETURN_BOOL(bit & (1 << n));
}
/* }}} */

/* {{{ proto int setbit(int mask, int pos[, init=1])
   Set a bit-flag in the number to 1 or another boolean */
PHP_FUNCTION(setbit)
{
	long bit, n;
	zend_bool init = 1;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll|b", &bit, &n, &init) == FAILURE) {
		RETURN_NULL();
	}

	RETURN_LONG(init ? bit | (1 << n) : bit & (~(1 << n)));
}
/* }}} */

/* {{{ proto int invbit(int mask, int pos)
   Toggle/invert a bit-flag in the number */
PHP_FUNCTION(invbit)
{
	long bit, n;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &bit, &n) == FAILURE) {
		RETURN_NULL();
	}

	RETURN_LONG(bit ^ (1 << n));
}
/* }}} */

/* {{{ proto int numbit(int mask)
   Count the number of bits set in the number */
PHP_FUNCTION(numbit)
{
	long n;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &n) == FAILURE) {
		RETURN_NULL();
	}

#if defined(__GNUC__) && (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4))
	RETURN_LONG(__builtin_popcount(n));
#else
	short c;
	for (c = 0; n; n&= n - 1, c++);

	RETURN_LONG(c);
#endif
}
/* }}} */

/* {{{ proto int msbit(int mask)
   Get the most significant bit of a number */
PHP_FUNCTION(msbit)
{
	static const long a[] = { 0x02, 0x0C, 0xF0, 0xFF00, 0xFFFF0000 };
	static const long b[] = { 0x01, 0x02, 0x04, 0x08, 0x10 };

	register long r = 0;
	long n;
	register short i;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &n) == FAILURE) {
		RETURN_NULL();
	}

	for (i = 4; i >= 0; i--) {
		if (n & a[i]) {
			n>>= b[i];
			r |= b[i];
		}
	}
	RETURN_LONG(r);
}
/* }}} */

/* {{{ proto int getint(int mask, int x, int y)
   Get a range of bits out of a number circumscribed by x and y */
PHP_FUNCTION(getint)
{
	long n, x, y;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lll", &n, &x, &y) == FAILURE) {
		RETURN_NULL();
	}
	RETURN_LONG((n >> x) & ((2 << (y - x)) - 1));
}
/* }}} */

/* {{{ proto int setint(int mask, int x, int y, int number)
   Set a number in a circumscribed range of x and y */
PHP_FUNCTION(setint)
{
	long n, x, y, m;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "llll", &n, &x, &y, &m) == FAILURE) {
		RETURN_NULL();
	}
	RETURN_LONG(n & (~(((2 << (y - x)) - 1) << x)) | (m << x));
}
/* }}} */

/* Math Extension */

/* {{{ proto int limit(mixed n, mixed max)
   Limits a number to a specified upper max value */
PHP_FUNCTION(limit)
{
	zval *max, *num;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &num, &max) == FAILURE) {
		RETURN_NULL();
	}

	if (ZVAL_TYPE_P(num) == IS_DOUBLE || ZVAL_TYPE_P(max) == IS_DOUBLE) {
		convert_to_double(num);
		convert_to_double(max);

		RETURN_DOUBLE(Z_DVAL_P(num) > Z_DVAL_P(max) ? Z_DVAL_P(max) : Z_DVAL_P(num));
	} else {
		convert_to_long(num);
		convert_to_long(max);

		RETURN_DOUBLE(Z_LVAL_P(num) > Z_LVAL_P(max) ? Z_LVAL_P(max) : Z_LVAL_P(num));
	}
}
/* }}} */

/* {{{ proto int bound(mixed n, mixed min, mixed max)
   Limits a number to a specified lower min and a upper max value */
PHP_FUNCTION(bound)
{
	zval *num, *min, *max;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzz", &num, &min, &max) == FAILURE) {
		RETURN_NULL();
	}

	if (ZVAL_TYPE_P(num) == IS_DOUBLE || ZVAL_TYPE_P(min) == IS_DOUBLE || ZVAL_TYPE_P(max) == IS_DOUBLE) {
		convert_to_double(num);
		convert_to_double(min);
		convert_to_double(max);

		if (Z_DVAL_P(num) < Z_DVAL_P(min)) {
			RETURN_DOUBLE(Z_DVAL_P(min));
		} else if (Z_DVAL_P(num) > Z_DVAL_P(max)) {
			RETURN_DOUBLE(Z_DVAL_P(max));
		} else {
			RETURN_DOUBLE(Z_DVAL_P(num));
		}

	} else {
		convert_to_long(num);
		convert_to_long(min);
		convert_to_long(max);

		if (Z_LVAL_P(num) < Z_LVAL_P(min)) {
			RETURN_LONG(Z_LVAL_P(min));
		} else if (Z_LVAL_P(num) > Z_LVAL_P(max)) {
			RETURN_LONG(Z_LVAL_P(max));
		} else {
			RETURN_LONG(Z_LVAL_P(num));
		}
	}
}
/* }}} */

/* {{{ proto int sgn(mixed n)
   Returns the sign of a number */
PHP_FUNCTION(sgn)
{
	zval *x;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &x) == FAILURE) {
		RETURN_NULL();
	}

	if (ZVAL_TYPE_P(x) == IS_DOUBLE) {
		RETURN_LONG((0 < Z_DVAL_P(x)) - (Z_DVAL_P(x) < 0));
	}

	convert_to_long(x);
	RETURN_LONG((0 < Z_LVAL_P(x)) - (Z_LVAL_P(x) < 0));
}
/* }}} */

/* {{{ proto int gpp(int n)
   Returns the greatest proper power of an integer */
PHP_FUNCTION(gpp)
{
	long x;
	register long f = 1, n = 1, nn;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &x) == FAILURE) {
		RETURN_NULL();
	}

	if(x <= 0) {
		RETURN_LONG(1);
	}

	for (; ; n++) {

		if ((nn = n * n) >= x) {
			break;
		} if (0 == (x % nn)) {
			f = n;
		}
	}
	RETURN_LONG(f);
}
/* }}} */

/* {{{ proto int sigfig(double n, int figs)
   Calculates the significant figures of a number */
PHP_FUNCTION(sigfig)
{
	long figs;
	double value, pow_exp, pow_sig;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dl", &value, &figs) == FAILURE) {
		RETURN_NULL();
	}

	if (figs < 0 || figs > 10) RETURN_FALSE;

	pow_exp = php_intpow10((int)log10(value) + 1);
	pow_sig = php_intpow10(figs);

	RETURN_DOUBLE((int)((value / pow_exp) * pow_sig + 0.5) / pow_sig * pow_exp);
}
/* }}} */

/* {{{ proto int checksum(int n)
   Calculate the checksum of a number */
PHP_FUNCTION(checksum)
{
	register long s, t;
	long a;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &a) == FAILURE) {
		RETURN_NULL();
	}

	for (s=0; a; s+= a - (t = a / 10) * 10, a = t);

	RETURN_LONG(s);
}
/* }}} */

/* {{{ proto int bround(int n, int b)
   Round to the next multiple of a base */
PHP_FUNCTION(bround)
{
	long n, b;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &n, &b) == FAILURE) {
		RETURN_NULL();
	}

	if (0 == b) {
		RETURN_FALSE;
	}
	RETURN_LONG(n + b - n % b);
}
/* }}} */

/* {{{ proto int xround(int n)
   Round to the next power of 10 */
PHP_FUNCTION(xround)
{
	static const long map[] = {
		1000000000,
		100000000,
		10000000,
		1000000,
		100000,
		10000,
		1000,
		100,
		10,
		1};

	long n, i;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &n) == FAILURE) {
		RETURN_NULL();
	}

	for (i=1; i < 10; i++) {
		if (n > map[i]) {
			RETURN_LONG(map[i - 1]);
		}
	}
	RETURN_LONG(1);
}
/* }}} */

/* String Extension */

/* {{{ proto string strcut(string s, int max)
   Cuts a string if it's longer then a max value */
PHP_FUNCTION(strcut)
{
	char *str;
	long len, max;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &str, &len, &max) == FAILURE) {
		RETURN_NULL();
	}

	if (len > max) {
		if (max <= 0) {
			RETURN_NULL();
		}
		char *tmp = emalloc(max + 1);

		memcpy(tmp, str, max);

		tmp[max] = 0;

		RETURN_STRINGL(tmp, max, 0);
	} else {
		RETURN_STRINGL(str, len, 1);
	}
}
/* }}} */

/* {{{ proto string stroff(string s, int off)
   Offsets a string by a given number */
PHP_FUNCTION(stroff)
{
	char *str;
	long len, off;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &str, &len, &off) == FAILURE) {
		RETURN_NULL();
	}

	if (off >= len || off < 0) {
		RETURN_NULL();
	} else {

		char *tmp = emalloc(len - off + 1);

		memcpy(tmp, str + off, len - off);

		tmp[len - off] = 0;

		RETURN_STRINGL(tmp, len - off, 0);
	}
}
/* }}} */

/* {{{ proto string truncate(string s, int max[, c="..."])
   Cuts a string if it's longer then a max value and appends a given string */
PHP_FUNCTION(truncate)
{
	char *str, *c = "...";
	long sl, cl = 3, max;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl|s", &str, &sl, &max, &c, &cl) == FAILURE) {
		RETURN_NULL();
	}

	if (sl > max) {
		char *ret = emalloc(max + cl + 1);
		memcpy(ret, str, max);
		memcpy(ret + max, c, cl + 1);
		RETURN_STRINGL(ret, max + cl, 0);
	} else {
		RETURN_STRINGL(str, sl, 1);
	}
}
/* }}} */

/* {{{ proto bool isuc(string s[, int pos=0])
   Check if the specified character is upper-case */
PHP_FUNCTION(isuc)
{
	char *str;
	long len, pos = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &str, &len, &pos) == FAILURE) {
		RETURN_NULL();
	}

	if (pos >= len) {
		RETURN_FALSE;
	}

	RETURN_BOOL(str[pos] >= 'A' && str[pos] <= 'Z');
}
/* }}} */

/* {{{ proto bool islc(string s[, int pos=0])
   Check if the specified character is lower-case */
PHP_FUNCTION(islc)
{
	char *str;
	long len, pos = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &str, &len, &pos) == FAILURE) {
		RETURN_NULL();
	}

	if (pos >= len) {
		RETURN_FALSE;
	}

	RETURN_BOOL(str[pos] >= 'a' && str[pos] <= 'z');
}
/* }}} */

/* {{{ proto string xsprintf(string format, callback c, char s)
   A more intelligent sprintf where you can specify the search token and a callback function */
PHP_FUNCTION(xsprintf)
{
	int i, flen, last, ts;

	zval *function,
	**args[1],
	*retval,
	*ptr;

	char *format,
	*search;

	smart_str token = {0};
	smart_str result = {0};

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "szs", &format, &flen, &function, &search, &last) == FAILURE) {
		RETURN_NULL();
	}

	if (last < 1) {
		RETURN_NULL();
	}

	MAKE_STD_ZVAL(ptr);

	last = 0;

	for (i = 0; i < flen; i++) {

		if (format[i] == search[0]) {

			smart_str_appendl(&result, format + last, i - last);

			if (format[i + 1] != search[0]) {

				for (ts = i++; i < flen; i++) {
					switch (format[i]) {
					case 'a' ... 'z':
					case 'A' ... 'Z':

						token.len = 0;
						smart_str_appendl(&token, format + ts + 1, i - ts);
						smart_str_0(&token);

						ZVAL_STRINGL(ptr, token.c, token.len, 0);
						args[0] =&ptr;

						if (call_user_function_ex(EG(function_table), NULL, function, &retval, 1, args, 0, NULL TSRMLS_CC) == SUCCESS) {

							convert_to_string_ex(&retval);
							smart_str_appendl(&result, Z_STRVAL_P(retval), Z_STRLEN_P(retval));
							zval_ptr_dtor(&retval);
						}
						break;

					default:
						continue;
					}
					break;
				}

				last = 1+i;
			} else {
				last = ++i;
			}
		}
	}
	if (0 < flen - last) {
		smart_str_appendl(&result, format + last, flen - last);
	}

	FREE_ZVAL(ptr);
	smart_str_free(&token);
	RETURN_STRINGL(result.c, result.len, 0);
}
/* }}} */

/* {{{ proto bool strcal(string format, string s[, int len])
   String calibration to check, if the string is in a given format with a simple regexp format */
PHP_FUNCTION(strcal)
{
	long vl, hl, len = -1, i, j;
	char *val, *hash;
	short match;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|l", &val, &vl, &hash, &hl, &len) == FAILURE) {
		RETURN_NULL();
	}

	if (-1 == len) {
		len = hl;
	} else if (len != hl) {
		RETURN_FALSE;
	}

	for (i = 0; i < len; i++) {
		for (j = 0, match = 0; j < vl; j++) {
			if (j < (vl-2) && val[j+1] == '-') {
				if (hash[i] >= val[j] && hash[i] <= val[j+2]) {
					match = 1;
					break;
				}
				j+= 2;
			} else if (hash[i] == val[j]) {
				match = 1;
				break;
			}
		}
		if (!match) RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool strcal(string format, string s[, int len])
   String calibration without care about upper and lower case to check, if the string is in a given format with a simple regexp format */
PHP_FUNCTION(strical)
{
	long vl, hl, len = -1, i, j;
	char *val, *hash, c;
	short match;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|l", &val, &vl, &hash, &hl, &len) == FAILURE) {
		RETURN_NULL();
	}

	if (-1 == len) {
		len = hl;
	} else if (len != hl) {
		RETURN_FALSE;
	}

	for (i = 0; i < len; i++) {
		c = tolower(hash[i]);
		for (j = 0, match = 0; j < vl; j++) {
			if (j < (vl-2) && val[j+1] == '-') {
				if (c >= val[j] && c <= val[j+2]) {
					match = 1;
					break;
				}
				j+= 2;
			} else if (c == val[j]) {
				match = 1;
				break;
			}
		}
		if (!match) RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

#define YYCTYPE         char
#define YYCURSOR        str
#define YYLIMIT         end
#define YYMARKER        marker
#define YYFILL(n)       { smart_str_appendl(&ret_ptr, YYMARKER, end - YYMARKER); RETURN_STRINGL(ret_ptr.c, ret_ptr.len, 0); }

/* {{{ proto string strmap(string s, array tokens)
   Brings a simple template parser to PHP */
PHP_FUNCTION(strmap)
{
	char *str;
	int str_len;

	zval *zarray, **zvalue;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sa", &str, &str_len, &zarray) == FAILURE) {
		RETURN_NULL();
	}

	char *end = str + str_len;
	char *marker = str;

	smart_str ret_ptr = {0};

	register YYCTYPE yych;

start:
	if ((YYLIMIT - YYCURSOR) < 2) YYFILL(2);
	yych = *YYCURSOR;
	switch (yych) {
		case '{':               goto yy2;
		default:                ++YYCURSOR;
		goto start;
	}
yy2:
	smart_str_appendl(&ret_ptr, YYMARKER, str - YYMARKER);

	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
		case '0' ... '9':       goto yy5;
		case 'a' ... 'z':
		case 'A' ... 'Z':
		case '_':               goto yy8;
		default:                goto start;
	}
yy5:
	++YYCURSOR;
	if (YYLIMIT <= YYCURSOR) YYFILL(1);
	yych = *YYCURSOR;
	switch (yych) {
		case '0' ... '9':       goto yy5;
		case 'a' ... 'z':
		case 'A' ... 'Z':
		case '_':               goto yy8;
		case '}':               goto yy12;
		default:                YYCURSOR = YYMARKER;
		--YYMARKER;
		goto start;
	}
yy8:
	++YYCURSOR;
	if (YYLIMIT <= YYCURSOR) YYFILL(1);
	yych = *YYCURSOR;
	switch (yych) {
		case 'a' ... 'z':
		case 'A' ... 'Z':
		case '0' ... '9':
		case '_':               goto yy8;
		case '}':               goto yy10;
		default:                YYCURSOR = YYMARKER;
		--YYMARKER;
		goto start;
	}
yy10:
	*str = 0;
	if (zend_hash_find(Z_ARRVAL_P(zarray), YYMARKER, str - YYMARKER + 1, (void**)&zvalue) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Undefined index: %s", YYMARKER);
	} else {
		convert_to_string(*zvalue);
		smart_str_appendl(&ret_ptr, Z_STRVAL_PP(zvalue), Z_STRLEN_PP(zvalue));
	}
	*str = '}';
	YYMARKER = ++YYCURSOR;
	goto start;

yy12:
	*str = 0;
	if (zend_hash_index_find(Z_ARRVAL_P(zarray), atol(YYMARKER), (void**)&zvalue) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Undefined offset: %s", YYMARKER);
	} else {
		convert_to_string(*zvalue);
		smart_str_appendl(&ret_ptr, Z_STRVAL_PP(zvalue), Z_STRLEN_PP(zvalue));
	}
	*str = '}';
	YYMARKER = ++YYCURSOR;
	goto start;
}
/* }}} */

#undef YYCTYPE
#undef YYCURSOR
#undef YYLIMIT
#undef YYMARKER
#undef YYFILL


/* {{{ proto int number_chop(int number, array parts)
   Chops a number as big as possible into parts */
PHP_FUNCTION(number_chop)
{
	zval *parts, **tmp;
	HashTable *target;

	long part_count, p = 0, c = 0, t = 0, n;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "la", &n, &parts) == FAILURE) {
		RETURN_NULL();
	}

	part_count = Z_ARRVAL_P(parts)->nNumOfElements;

	if(part_count == 0) {
		RETURN_NULL();
	}

	/* I think working inplace is okay here */
	target = HASH_OF(parts);
	if(zend_hash_sort(target, zend_qsort, array_reverse_data_compare, 1 TSRMLS_CC) == FAILURE) {
		RETURN_NULL();
	}

	if(zend_hash_index_find(target, part_count - 1, (void**)&tmp) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Only pass a indexed array");
		RETURN_NULL();
	}

	if(Z_LVAL_PP(tmp) <= 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Only pass numbers greater 0 with the array, found smallest value: %d", Z_LVAL_PP(tmp));
		RETURN_NULL();
	}

	array_init(return_value);

	while(n > 0) {

		if(t == 0) {

			if(p == part_count) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Overflow detected. Add 1 as the smallest value to the array to avoid this");
				RETURN_NULL();
			}

			if(zend_hash_index_find(target, p, (void**)&tmp) == FAILURE) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Undefined index: %d", p);
				RETURN_NULL();
			}

			t = Z_LVAL_PP(tmp);

		} if(t == 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Only pass unsigned parts with the array, found smallest value: %d", t);
			RETURN_NULL();
		}

		if(n >= t) {
			c++;
			if((n-= t) < t) {
				add_index_long(return_value, t, c);
				c = t = 0;
				p++;
			}
		} else {
			p++;
			t = 0;
		}
	}
}
/* }}} */

/* {{{ proto int time_chop(int time, mixed format=null, bool as_array=0)
   Chops a time value and returns as format string or as array */
PHP_FUNCTION(time_chop)
{
	static long date[] = { 31536000, 2592000, 604800, 86400, 3600, 60,  1 };
	static long data[] = {        0,       0,      0,     0,    0,  0,  0 };

	const char code[] = "ynwdhms";
	const char *ndx[] = {"years", "months", "weeks", "days", "hours", "minutes", "seconds"};

	zend_bool is_array = 0;

	zval *_format;
	char *format;
	long count, time, i;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|zb", &time, &_format, &is_array) == FAILURE) {
		RETURN_NULL();
	}

	if (time > 1000000000) {
		time = (long)sapi_get_request_time(TSRMLS_C) - time;
	}

	if (ZEND_NUM_ARGS() == 1) {
		format = NULL;
		count  = 2;
	} else switch (Z_TYPE_P(_format)) {
		case IS_STRING:
			if (!is_numeric_string(Z_STRVAL_P(_format), Z_STRLEN_P(_format), NULL, NULL, 0)) {
				format = Z_STRVAL_P(_format);
				count  = 7;
				break;
			}
		/* Fall */

		case IS_LONG:
		case IS_DOUBLE:
			convert_to_long(_format);
			format = NULL;
			count  = Z_LVAL_P(_format);
			break;

		case IS_NULL:
			format = NULL;
			count  = 2;
			break;

		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Passed a not supported type for format");
			RETURN_NULL();
	}

	if (is_array) {

		array_init(return_value);

		long tmp;

		for (i = 0; i < 7 && time > 0; i++) {

			if (NULL != format && NULL == strchr(format, code[i]) || 0 == (tmp = time / date[i])) {
				continue;
			}

			time-= tmp * date[i];
			add_assoc_long(return_value, ndx[i], tmp);
		}

	} else {

		long j = 0;
		long c = 1;

		smart_str str = {0};

		for (i = 0; i < 7 && time > 0; i++) {

			if(NULL != format && NULL == strchr(format, code[i]) || 0 == (data[i] = time / date[i])) {
				continue;
			}

			time-= data[i] * date[i];
			j++;
		}

		for (i = 0; i < 7 && c <= count; i++) if (data[i] > 0) {

			if (c > 1) {
				if (c == j || c == count) {
					smart_str_appendl(&str, " {14} ", 6);
				} else {
					smart_str_appendl(&str, ", ", 2);
				}
			}

			if (data[i] == 1) {
				smart_str_appendc(&str, '{');
				smart_str_append_long(&str, i);
				smart_str_appendc(&str, '}');
			} else {
				smart_str_append_long(&str, data[i]);
				smart_str_appendl(&str, " {", 2);
				smart_str_append_long(&str, i + 7);
				smart_str_appendc(&str, '}');
			}
			c++;
		}
		RETURN_STRINGL(str.c, str.len, 0)
	}
}
/* }}} */

/* {{{ proto string kimplode([string glue,] array pieces)
	Joins array keys placing glue string between items and return one string */
PHP_FUNCTION(kimplode)
{
	zval **arg1 = NULL, **arg2 = NULL, *delim, *arr;

	zval         **tmp;
	HashPosition   pos;
	smart_str      implstr = {0};
	int            numelems, i = 0;
	zval tmp_val;
	int str_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Z|Z", &arg1, &arg2) == FAILURE) {
		return;
	}

	if (arg2 == NULL) {
		if (Z_TYPE_PP(arg1) != IS_ARRAY) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Argument must be an array");
			return;
		}

		MAKE_STD_ZVAL(delim);

		ZVAL_STRINGL(delim, "", sizeof("") - 1, 0);

		SEPARATE_ZVAL(arg1);
		arr = *arg1;
	} else {
		if (Z_TYPE_PP(arg1) == IS_ARRAY) {
			arr = *arg1;
			convert_to_string_ex(arg2);
			delim = *arg2;
		} else if (Z_TYPE_PP(arg2) == IS_ARRAY) {
			arr = *arg2;
			convert_to_string_ex(arg1);
			delim = *arg1;
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid arguments passed");
			return;
		}
	}

	numelems = zend_hash_num_elements(Z_ARRVAL_P(arr));

	if (numelems == 0) {
		RETURN_EMPTY_STRING();
	}

	for (zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(arr), &pos);
		zend_hash_get_current_data_ex(Z_ARRVAL_P(arr), (void **) &tmp, &pos) == SUCCESS;
		zend_hash_move_forward_ex(Z_ARRVAL_P(arr), &pos)
	) {

		char *key;
		int key_len;
		long index;

		if (zend_hash_get_current_key_ex(Z_ARRVAL_P(arr), &key, &key_len, &index, 0, &pos) == HASH_KEY_IS_STRING) {
			smart_str_appendl(&implstr, key, key_len - 1);
		} else {
			char stmp[MAX_LENGTH_OF_LONG + 1];
			str_len = slprintf(stmp, sizeof(stmp), "%ld", index);
			smart_str_appendl(&implstr, stmp, str_len);
		}

		if (++i != numelems) {
			smart_str_appendl(&implstr, Z_STRVAL_P(delim), Z_STRLEN_P(delim));
		}
	}
	smart_str_0(&implstr);

	if (arg2 == NULL) {
		FREE_ZVAL(delim);
	}

	if (implstr.len) {
		RETURN_STRINGL(implstr.c, implstr.len, 0);
	} else {
		smart_str_free(&implstr);
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ proto int typeof(mixed val)
   Returns the type of a number as integer */
PHP_FUNCTION(typeof)
{
	zval *arg;
	char *type_name;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &arg) == FAILURE) {
		RETURN_NULL();
	}

	switch(Z_TYPE_P(arg)) {
		case IS_NULL:
		case IS_BOOL:
		case IS_LONG:
		case IS_DOUBLE:
		case IS_STRING:
		case IS_ARRAY:
		case IS_OBJECT:
			RETURN_LONG(Z_TYPE_P(arg));
		case IS_RESOURCE:
			type_name = zend_rsrc_list_get_rsrc_type(Z_LVAL_P(arg) TSRMLS_CC);
			if (type_name) {
				RETURN_LONG(Z_TYPE_P(arg));
			}
		default:
			RETURN_LONG(-1);
	}
}
/* }}} */

/* {{{ proto bool is_ref(mixed val)
   Check if there is a reference on a variable */
PHP_FUNCTION(is_ref)
{
	zval *var = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &var) == FAILURE) {
		RETURN_NULL();
	}

	if (var) {
		RETURN_BOOL(Z_REFCOUNT_P(var) > 2);
	}
	RETURN_FALSE;
}
/* }}} */

/*
* Local variables:
* tab-width: 4
* c-basic-offset: 4
* End:
* vim600: noet sw=4 ts=4 fdm=marker
* vim<600: noet sw=4 ts=4
*/

