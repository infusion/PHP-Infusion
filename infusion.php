<?php
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



/* Binary Extension */

function isbit($mask, $n);

function setbit($mask, $n[, $init=1]);

function invbit($mask, $n);

function numbit($mask);

function msbit($mask);

function getint($mask, $x, $y);

function setint($mask, $x, $y, $num);

/* Logical Extension */

function between($num, $a, $b);

function limit($num, $max);

function bound($num, $min, $max);

/* Mathematical Extension */

function sgn($num);

function gpp($num);

function sigfig($num, $figs);

function checksum($num);

function bround($num, $base);

function xround($num);

/* String Extension */

function strcut($str, $num);

function stroff($str, $off);

function truncate($str, $num[, $x='...']);

function isuc($str[, $pos=0]);

function islc($str[, $pos=0]);

function xsprintf($format, $callback, $delim);

function strcal($format, $str[, $len=-1]);

function strical($format, $str[, $len=-1]);

function strmap($str, $replace);

/* Chop Extension */

function number_chop($num, $parts);

function time_chop($time[, $format=2, $is_array=0]);

/* Array Extension */

function kimplode($array);

/* Variable Extension */

function typeof($var);

function is_ref($var);


?>
