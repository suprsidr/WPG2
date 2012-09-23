<?php
/*
Plugin Name: WPG2
Plugin URI: http://www.wpg2.org
Description: Embeds Gallery2 within Wordpress to share photos, videos and any other Gallery2 content seamlessly into your Blog & Sidebar Content.<br/> <a href="http://codex.gallery2.org/Integration:WPG2">Documentation</a>, <a href="http://gallery.menalto.com/forum/81">Support Forums</a>, <a href="http://codex.gallery2.org/Integration:WPG2_Release_Notes">Change Logs</a>
Version: 3.0.7
Author: <a href="http://www.ozgreg.com/">Ozgreg</a> and <a href="http://codex.gallery2.org/Integration:WPG2_Team">WPG2 Team</a>
Author URI: http://www.ozgreg.com/
*/

/*
Updated: 15:32 19/07/2008

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

/*
********************************************************************************************************
											Plugin Initalisation
********************************************************************************************************
*/

// To Trap Errors
// Remove @ from Includes 

// Get Gallery2 Option Settings
$wpg2_option = get_option('wpg2_options');

// Get Plugin Base
$wpg2base = 'wp-content'.DIRECTORY_SEPARATOR.'plugins'.DIRECTORY_SEPARATOR.'wpg2';

// Add WPG2 Validate Functions
	@include_once(ABSPATH . $wpg2base . DIRECTORY_SEPARATOR . 'wpg2validate.inc');

// Add WPG2 Functions
	@include_once(ABSPATH . $wpg2base . DIRECTORY_SEPARATOR . 'wpg2embed.inc');

// Add G2Image Functions
	@include_once(ABSPATH . $wpg2base . DIRECTORY_SEPARATOR . 'g2imageplugin.inc');

// Add WPG2 Widgets
	@include_once(ABSPATH . $wpg2base . DIRECTORY_SEPARATOR . 'wpg2widgets.inc');

// Add WPG2 Template Functions
	@include_once(ABSPATH . $wpg2base . DIRECTORY_SEPARATOR . 'wpg2template.inc');

// Add WPG2 Option Page Functions
	@include_once(ABSPATH . $wpg2base . DIRECTORY_SEPARATOR . 'wpg2options.inc');

// Add WPG2 Functions
	@include_once(ABSPATH . $wpg2base . DIRECTORY_SEPARATOR . 'wpg2functions.inc');

// Add WPG2 Rewrite Validation
	@include_once(ABSPATH . $wpg2base . DIRECTORY_SEPARATOR . 'wpg2managerewrites.inc');

/*
********************************************************************************************************
											Link WPG2 to WP Hooks
********************************************************************************************************
*/

//Activate & Deactivate Plugin Functions
add_action('activate_' . plugin_basename(__FILE__), 'wpg2_pluginactivate');
add_action('deactivate_' . plugin_basename(__FILE__), 'wpg2_plugindeactivate');

// Add WP Menus
add_action('admin_menu', 'wpg2_addwpmenus');
add_action('user_menu', 'wpg2_addwpmenus');

// Add Filter for Gallery2 Output
add_action('template_redirect', 'wpg2_template');

// Filter for WP Page Changes
add_action('delete_post', 'wpg2_template_pagedeletes');
add_action('save_post', 'wpg2_template_pagechanges');

// Add G2 Title to Header
add_action ( 'get_header', 'g2_outputpagetitle' );

// Hook into WP Templates
if ($wpg2_option['g2_validated'] == "Yes" && $wpg2_option['g2_embedpageid'] != "" ) {

// Wordpress Rewrite Hooks - Add Filter To insert additional Gallery2  Rewrite ruleset.
	if ($wpg2_option['g2_rewriteactive'] != '') {
		add_filter('rewrite_rules_array', 'wpg2_template_rules');
	}
	add_filter('generate_rewrite_rules', 'wpg2_trimpermalinkrules');
	add_action('admin_menu', 'wpg2_template_rewritechanges'); // <-- Additional Trap to put WPG2 into Safe Mode due to Filter not firing when deactiving permalinks

}

// Activate when WPG2 is Validated
if ( $wpg2_option['g2_validated'] == "Yes" ) {
	// Add User Sync Functions
	add_action('delete_user', 'g2_delete_user');
	add_action('profile_update', 'g2_update_user');
	add_action('user_register', 'g2_create_user');

	// Switching Theme's
	add_action('switch_theme', 'wpg2_themeswitchvalidate');

	// Logout
	add_action('wp_logout', 'g2_logout');

	// Lightbox
	if ($wpg2_option['wpg2_enablelightbox'])
		add_action('wp_head', 'wpg2_lightboxheader');

	// Add G2 Header Elements
	add_action('wp_head', 'g2_addheader');

	// Add WP Menus
	add_action('user_menu', 'wpg2_addwpmenus');

	// Add Cron Hook For WP Revalidation
	add_action('wpg2_cron_wprewritehook','wpg2_rebuildrewriterules');

	// Add Cron Hook For WP+G2 Revalidation
	add_action('wpg2_cron_g2rewritehook','wpg2_rebuildg2rewriterules');

	// Add Admin WPG2 Help JS
	add_action('admin_notices', 'wpg2_adminjsheader');

	// Add  WPG2 is_done on WP Footer Execution
	add_action('admin_footer', 'g2_isdone');	
	add_action('wp_footer', 'g2_isdone');
}

// WPG2 Widgets
add_action('widgets_init', 'wpg2_sidebar_widgets_init', 0);

// Filter for WPG2 Tags - Blog - G2WP Path in post
add_filter('the_content', 'g2_imagebypathinpost', 0);

// Filter for WPG2 Tags - Category - G2WP Path in post
add_filter('the_excerpt', 'g2_imagebypathinpost', 0);

// Filter for WPG2 Tags - Blog Excerpt -G2WP Path in post
add_filter('excerpt_save_pre', 'g2_imagebypathinpost', 0);

// Filter for WPG2 Tags - Blog - G2WP ID in post
add_filter('the_content', 'g2_imagebyidinpost', 0);

// Filter for WPG2 Tags - Category - G2WP ID in post
add_filter('the_excerpt', 'g2_imagebyidinpost', 0);
// Filter for WPG2 Tags - Comments - G2WP ID in post
add_filter('comment_text', 'g2_imagebyidinpost', 0);

// G2Image Functions
if (function_exists('g2image_plugin')) {

	$g2_image = get_option('g2ic');

	// Activate when WPG2 is Validated
	if ( $wpg2_option['g2_validated'] == "Yes" ) {

		// Add Tags to valid MCEEditor Tags
		add_filter('mce_valid_elements', 'g2image_wp_extended_editor_mce_valid_elements', 0);

		if (isset($g2_image['wp_nomcebutton']) && ($g2_image['wp_nomcebutton'] == 'disabled')) {
			// Skip Activation of the TinyMCEG2image Button
		} else {
			// G2 Image Filters - Visual Editor
			add_action('init', 'g2image_addbuttons');
		}

		// G2 Image Filters - Non Visual Editor
		add_filter('admin_footer', 'g2image_callback');

	}
}

?>