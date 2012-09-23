<?php
/*
    Gallery 2 Image Chooser
    Version 3.0.2 - updated 01 OCT 2007
    Documentation: http://g2image.steffensenfamily.com/

    Author: Kirk Steffensen with inspiration, code snipets,
        and assistance as listed in CREDITS.HTML

    Released under the GPL version 2.
    A copy of the license is in the root folder of this plugin.

    See README.HTML for installation info.
    See CHANGELOG.HTML for a history of changes.
*/

require_once('config.php');

// ====( Initialize Variables )=================================
$g2ic_options = array();
$g2ic_options['current_page'] = 1;
$g2ic_options['wpg2_valid'] = FALSE;
$g2ic_wp_rel_path = '';
$g2ic_base_path = str_repeat("../", substr_count(dirname($_SERVER['PHP_SELF']), "/"));

// Convert the variables from config.php to $g2ic_options array items.
// Kept the original variable names in config.php for backwards compatibility with
// some integrations that overwrite config.php to set these variables.
$g2ic_options['images_per_page'] = $g2ic_images_per_page;
$g2ic_options['display_filenames'] = $g2ic_display_filenames;
$g2ic_options['default_alignment'] = $g2ic_default_alignment;
$g2ic_options['custom_class_1'] = $g2ic_custom_class_1;
$g2ic_options['custom_class_2'] = $g2ic_custom_class_2;
$g2ic_options['custom_class_3'] = $g2ic_custom_class_3;
$g2ic_options['custom_class_4'] = $g2ic_custom_class_4;
$g2ic_options['custom_url'] = $g2ic_custom_url;
$g2ic_options['class_mode'] = $g2ic_class_mode;
$g2ic_options['default_action'] = $g2ic_default_action;
$g2ic_options['sortby'] = $g2ic_sortby;
$g2ic_options['drupal_g2_filter'] = $g2ic_drupal_g2_filter;
$g2ic_options['drupal_g2_filter_prefix'] = $g2ic_drupal_g2_filter_prefix;

// ==============================================================
// WPG2 validation
// ==============================================================

// Determine if in a WordPress installation by checking for wp-config.php
for ($count = 1; $count <= 7; $count++) {
	$g2ic_wp_rel_path = $g2ic_wp_rel_path.'../';
	if (file_exists($g2ic_wp_rel_path . 'wp-config.php')) {
		require_once($g2ic_wp_rel_path.'wp-config.php');
		require_once($g2ic_wp_rel_path.'wp-admin/admin.php');
		$wpg2_g2ic = get_option('wpg2_g2ic');
		$wpg2_g2paths = get_option('wpg2_g2paths');
		$wpg2_options = get_option('wpg2_options');
		$g2ic_language = get_locale();

		// Assume WPG2 is active, or we wouldn't be here.
		$g2ic_options['wpg2_valid'] = TRUE;
		$g2ic_embedded_mode = TRUE;
		$g2ic_use_full_path = TRUE;

		// Call g2ic_init with WPG2 URI info
		$g2ic_embed_uri = $wpg2_g2paths['g2_embeduri'];
		$g2ic_gallery2_uri = $wpg2_g2paths['g2_url'];
		$g2ic_gallery2_path = $wpg2_g2paths['g2_filepath'];

		// Get the configurations options from the WPG2 admin panel
		if(isset($wpg2_options['g2_tagimgsize']))
			$g2ic_options['wpg2_tag_size'] = $wpg2_options['g2_tagimgsize'];
		if(isset($wpg2_g2ic['g2ic_images_per_page']))
			$g2ic_options['images_per_page'] = $wpg2_g2ic['g2ic_images_per_page'];
		if(isset($wpg2_g2ic['g2ic_display_filenames'])){
			if($wpg2_g2ic['g2ic_display_filenames']=='yes')
				$g2ic_options['display_filenames'] = TRUE;
			else
				$g2ic_options['display_filenames'] = FALSE;
		}
		if(isset($wpg2_g2ic['g2ic_default_alignment']))
			$g2ic_options['default_alignment'] = $wpg2_g2ic['g2ic_default_alignment'];
		if(isset($wpg2_g2ic['g2ic_custom_class_1']))
			$g2ic_options['custom_class_1'] = $wpg2_g2ic['g2ic_custom_class_1'];
		if(isset($wpg2_g2ic['g2ic_custom_class_2']))
			$g2ic_options['custom_class_2'] = $wpg2_g2ic['g2ic_custom_class_2'];
		if(isset($wpg2_g2ic['g2ic_custom_class_3']))
			$g2ic_options['custom_class_3'] = $wpg2_g2ic['g2ic_custom_class_3'];
		if(isset($wpg2_g2ic['g2ic_custom_class_4']))
			$g2ic_options['custom_class_4'] = $wpg2_g2ic['g2ic_custom_class_4'];
		if(isset($wpg2_g2ic['g2ic_custom_url']))
			$g2ic_options['custom_url'] = $wpg2_g2ic['g2ic_custom_url'];
		if(isset($wpg2_g2ic['g2ic_class_mode']))
			$g2ic_options['class_mode'] = $wpg2_g2ic['g2ic_class_mode'];
		if(isset($wpg2_g2ic['g2ic_sortby']))
			$g2ic_options['sortby'] = $wpg2_g2ic['g2ic_sortby'];
		if(isset($wpg2_g2ic['g2ic_default_action'])) {
			// For backwards compatibility with old option value in WPG2 G2Image Options tab
			if ($wpg2_g2ic['g2ic_default_action'] == 'wpg2')
				$g2ic_options['default_action'] = 'wpg2_image';
			else
				$g2ic_options['default_action'] = $wpg2_g2ic['g2ic_default_action'];
		}
		else
			$g2ic_options['default_action'] = 'wpg2_image';

		break;
	}
}

// ==============================================================
// NOTE for developers:
// If you are developing an embedded application for Gallery2 and want to use
// this plugin for accessing Gallery2, this is where you'll need
// to validate that your ebedded page exists and then get the values from your
// embedded application $g2ic_gallery2_path, $g2ic_gallery2_uri, and
// $g2ic_embed_uri.  Descriptions of these variables are in config.php
//
// You'll also need to set $g2ic_embedded_mode to TRUE.
//
// If you use the full directory path for $g2ic_gallery_path, you'll need to set
// $g2ic_use_full_path to TRUE.  If you use a path relative to the root web page,
// you'll need to set $g2ic_use_full_path to FALSE.
//
// If your embedded application sets its own localization, you'll need to set the
// language in $g2ic_language.  If you need to load the language file for any
// initialization messages (as in the WPG2 code above), you'll need to load it
// before those messages appear.  If you do load it in your initialization
// sequence, set $g2ic_language_loaded to TRUE, so that the language pack won't
// get loaded again.
//
// See http://g2image.steffensenfamily.com for more details.
//
// ==============================================================

// Determine gettext locale
if (file_exists('./langs/' . $g2ic_language . '.mo')) {
	$locale = $g2ic_language;
}
else {
	$locale = 'en';
}

// gettext setup
require_once('gettext.inc');
T_setlocale(LC_ALL, $locale);

// Set the text domain as 'default'
T_bindtextdomain('default', 'langs');
T_bind_textdomain_codeset('default', 'UTF-8');
T_textdomain('default');

if(!$g2ic_embedded_mode)
	$g2ic_gallery2_uri = '/' . $g2ic_gallery2_path . 'main.php';

if(!$g2ic_use_full_path)
	$g2ic_gallery2_path = $g2ic_base_path.$g2ic_gallery2_path;

if(file_exists($g2ic_gallery2_path.'embed.php')) {
	require_once($g2ic_gallery2_path.'embed.php');

	if ($g2ic_embedded_mode){
		$g2ic_embed_options['g2Uri'] = $g2ic_gallery2_uri;
		$g2ic_embed_options['embedUri'] = $g2ic_embed_uri;
		g2ic_init($g2ic_embed_options,$g2ic_embedded_mode);
	}
	else{
		$g2ic_embed_options['g2Uri'] = $g2ic_gallery2_uri;
		g2ic_init($g2ic_embed_options,$g2ic_embedded_mode);
	}
}
// Else die on a fatal error
else {
	print g2ic_make_html_header();
	print T_('<h3>Fatal Gallery2 Error: Cannot activate the Gallery2 Embedded functions.</h3><br />For WordPress users, Validate WPG2 in the Options Admin panel.<br /><br />For other platforms, please verify your Gallery2 path in config.php.');
	print '</body>' . "\n\n";
	print '</html>';
	die;
}

/**
 * Initialize the emedded functions of Gallery2
 *
 * Exit on Fatal Error
 *
 * @param array $option The GalleryEmbed options array
 * @param boolean $embedded_mode Whether to perform embedded GalleryEmbed init, or standalone init.
 */
function g2ic_init($option, $embedded_mode) {

	// Initialise GalleryAPI
	if ($embedded_mode){
		$error = GalleryEmbed::init( array(
			'g2Uri' => $option['g2Uri'],
			'embedUri' => $option['embedUri'],
			'fullInit' => true)
		);
	}
	else{
		$error = GalleryEmbed::init( array(
			'g2Uri' => $option['g2Uri'],
			'embedUri' => $option['g2Uri'],
			'fullInit' => true)
		);
	}
	if ($error) {
		print g2ic_make_html_header();
		print T_('<h3>Fatal Gallery2 error:</h3><br />Here\'s the error from G2:') . ' ' . $error->getAsHtml() . "\n";
		print "</body>\n\n";
		print "</html>";
		die;
	}

	return;
}
?>
