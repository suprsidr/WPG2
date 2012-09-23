<?php
/*
   Copyright (c) 2005 Steven Armstrong <sa at c-area dot ch>
    Modified for Gallery 2 Image Chooser by Kirk Steffensen
    Version 3.0.2 - updated 01 OCT 2007

   Drop in replacement for native gettext.

   This file is part of PHP-gettext.

   PHP-gettext is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   PHP-gettext is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with PHP-gettext; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

if (!class_exists('CachedFileReader')) {
	require('streams.php');
}
if (!class_exists('gettext_reader')){
	require('gettext.php');
}

// Variables

global $text_domains, $default_domain, $CURRENTLOCALE;
$text_domains = array();
$default_domain = 'default';
$CURRENTLOCALE = '';

// Utility functions

/**
 * Utility function to get a StreamReader for the given text domain.
 */
function _get_reader($domain=null, $enable_cache=true) {
	global $text_domains, $default_domain, $CURRENTLOCALE;
	if (!isset($domain)) $domain = $default_domain;
	if (!isset($text_domains[$domain]->l10n)) {
		// get the current locale
		$locale = $CURRENTLOCALE;
		$p = isset($text_domains[$domain]->path) ? $text_domains[$domain]->path : './';
		$path = $p . "$locale.mo";
		if (file_exists($path)) {
			$input = new FileReader($path);
		}
		else {
			$input = null;
		}
		$text_domains[$domain]->l10n = new gettext_reader($input, $enable_cache);
	}
	return $text_domains[$domain]->l10n;
}

/**
 * Get the codeset for the given domain.
 */
function _get_codeset($domain=null) {
	global $text_domains, $default_domain;
	if (!isset($domain)) $domain = $default_domain;
	return (isset($text_domains[$domain]->codeset))? $text_domains[$domain]->codeset : ini_get('mbstring.internal_encoding');
}

/**
 * Convert the given string to the encoding set by bind_textdomain_codeset.
 */
function _encode($text) {

	// If the mbstring extension for PHP is not enabled, mb_detect_encoding and mb_convert_encoding
	// are not available.  If you are having problems with character encodings not working on your
	// translation, please check with your webhost about how to enable mbstring for your PHP instance.
	// See http://www.php.net/mbstring for more information.

	if (function_exists('mb_detect_encoding')) {
		$source_encoding = mb_detect_encoding($text);
	}
	else {
		$source_encoding = 'UTF-8';
	}

	$target_encoding = _get_codeset();
	if (($source_encoding != $target_encoding) && (function_exists('mb_convert_encoding'))) {
		return mb_convert_encoding($text, $target_encoding, $source_encoding);
	}
	else {
		return $text;
	}
}

// Custom implementation of the standard gettext related functions

/**
 * Sets a requested locale, if needed emulates it.
 */
function _setlocale($category, $locale) {
	global $CURRENTLOCALE;
	if ($locale == '') // emulate variable support
		$CURRENTLOCALE = getenv('LANG');
	else
		$CURRENTLOCALE = $locale;
        return $CURRENTLOCALE;
}

/**
 * Sets the path for a domain.
 */
function _bindtextdomain($domain, $path) {
	global $text_domains;
	// ensure $path ends with a slash
	if ($path[strlen($path) - 1] != '/') $path .= '/';
	elseif ($path[strlen($path) - 1] != '\\') $path .= '\\';
	$text_domains[$domain]->path = $path;
}

/**
 * Specify the character encoding in which the messages from the DOMAIN message catalog will be returned.
 */
function _bind_textdomain_codeset($domain, $codeset) {
	global $text_domains;
	$text_domains[$domain]->codeset = $codeset;
}

/**
 * Sets the default domain.
 */
function _textdomain($domain) {
	global $default_domain;
	$default_domain = $domain;
}

/**
 * Lookup a message in the current domain.
 */
function _gettext($msgid) {
	$l10n = _get_reader();
	//return $l10n->translate($msgid);
	return _encode($l10n->translate($msgid));
}

// Wrappers to use if the standard gettext functions are available, but the current locale is not supported by the system.
// Use the standard impl if the current locale is supported, use the custom impl otherwise.

function T_setlocale($category, $locale) {
	return _setlocale($category, $locale);
}

function T_bindtextdomain($domain, $path) {
	return _bindtextdomain($domain, $path);
}

function T_bind_textdomain_codeset($domain, $codeset) {
	return _bind_textdomain_codeset($domain, $codeset);
}

function T_textdomain($domain) {
	return _textdomain($domain);
}

function T_($msgid) {
	return _gettext($msgid);
}
?>