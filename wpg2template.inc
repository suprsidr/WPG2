<?php
/*
	Author: WordPress Gallery Team
	Updated: 14:34 24/05/2008

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
* Performs a Template Redirect to Output Gallery2 Content
*
* @param External Page Redirect
* @return None
*/
function wpg2_template() {

	global $g2data, $wpg2_wptitle, $wp_query;

	// Get WPG2 Option Settings
	$wpg2_option = get_option('wpg2_options');

	// Output Wordpress Page
	if (is_page($wpg2_option['g2_embedpageid'])) {

		if (function_exists('btev_trigger_error')) {
			$page_obj = $wp_query->get_queried_object();
			btev_trigger_error('WPG2 PAGE: '.$page_obj->ID.' TRIGGER', E_USER_NOTICE, __FILE__);
		}

		if ( $wpg2_option['g2_validated'] == "Yes" ) {

			// Get Gallery2 Paths & Plugin Settings
			$wpg2_g2path = get_option('wpg2_g2paths');
			$wpg2_option = get_option('wpg2_options');

			// Log into G2
			if (!defined('G2INIT')) {
				$ret = g2_login();
				if ($ret) {
					echo '<h2>' . __('Fatal G2 error', 'wpg2') . '</h2> ' . __("Here's the error from G2: ", 'wpg2') . $ret->getAsHtml();
					exit;
				}
			}

			// Remove the Magic Quotes That Wordpress Adds, it just confuses Gallery2
			$_POST = stripslashes_deep($_POST);
			$_GET = stripslashes_deep($_GET);
			$_COOKIE = stripslashes_deep($_COOKIE);
			$_SERVER = stripslashes_deep($_SERVER);

			// Set the WPG2 Template Page Control Flags
			define("G2PARTINIT", "True");
			define("WPG2PAGE", "True");

			// Pass through WP URL / Blog Name
			$site_url = get_option('home');
			$site_name = get_option('blogname');
			GalleryUtilities::putRequestVariable('wpg2_returnurl', $site_url);
			GalleryUtilities::putRequestVariable('wpg2_returnname', $site_name);

			// Switch on Lightbox
			if ($wpg2_option['wpg2_enableg2lightbox']) {
				GalleryUtilities::putRequestVariable('wpg2_lightbox', "active");
				GalleryUtilities::putRequestVariable('wpg2_lightboxsize', $wpg2_option['g2_lightboximgsize']);
			}

			// Grab Gallery2 Output
			$g2data = GalleryEmbed::handleRequest();

			if ($g2data['isDone']) {
				//Close Gallery Connection
				GalleryEmbed::done();

				// Add BTEV Event Message
				if (function_exists('btev_trigger_error')) {
					btev_trigger_error('WPG2 PAGE ALREADY ISDONE()', E_USER_NOTICE, __FILE__);
				}
				exit; // G2 has already sent output (redirect or binary data)
			}


			// Theme Switcher Hook
			if(!empty($_COOKIE['wp_ts2_'.COOKIEHASH])) {

				if ( file_exists( TEMPLATEPATH . '/wpg2header.php') ) {
						$wpg2_option['g2_externalheader'] = "Yes";
					} else {
						$wpg2_option['g2_externalheader'] = "No";
					}

				if ( file_exists( TEMPLATEPATH . '/wpg2footer.php') ) {
					$wpg2_option['g2_externalfooter'] = "Yes";
				} else {
					$wpg2_option['g2_externalfooter'] = "No";
				}
			}

			// Add G2 Title to Header - - Bug Fix #44
			add_filter ( 'wp_title', 'wp_outputpagetitle' );

			// How Should We Generate the WPG2 Page, Without WP Headers or With?
			if ($wpg2_option['g2_embedpagetype'] == 'gallery2' ) {
				echo $wpg2_option['g2_htmlheader'];
				$title = '<title>';
				$title .= get_option('blogname');
				echo $title.$wpg2_wptitle.'</title>';
				g2_addheader();
				g2_outputpagetitle();
				echo $wpg2_option['g2_htmlbody'];
			} else if ($wpg2_option['g2_embedpagetype'] == 'wordpress' ) {
				// Should we Disable the Header output and instead allow the wpg2header to control the Output?
				if ( $wpg2_option['g2_externalheader']!="Yes" ) {
					get_header();
					//Include any plug-in header content set in the plugin options
					echo stripslashes($wpg2_option['g2_header']);
						
				} else
					include (TEMPLATEPATH.'/wpg2header.php');
			}

			echo $g2data['bodyHtml'];	 //Display the gallery content

			// How Should We Generate the WPG2 Page, Without WP Footers or With?
			if ($wpg2_option['g2_embedpagetype'] == 'wordpress' ) {

				// Should we Disable the Header output and instead allow the wpg2footer to control the Output?
				if ($wpg2_option['g2_externalfooter']!="Yes" ) {
					//Include plug-in footer content
					echo stripslashes($wpg2_option['g2_footer']);
					//Include WP footer
				if ( file_exists( TEMPLATEPATH . '/footer.php') )
					load_template( TEMPLATEPATH . '/footer.php');
				else
					load_template( ABSPATH . 'wp-content/themes/default/footer.php');
				} else
					include (TEMPLATEPATH.'/wpg2footer.php');
			} else {	// Gallery2 Standalone Page?
				if ($wpg2_option['g2_embedpagetype'] == 'gallery2' ) {
					echo $wpg2_option['g2_htmlfooter'];
				}
			}

			// End Session
			GalleryEmbed::done();

			// Add BTEV Event Message
			if (function_exists('btev_trigger_error')) {
				btev_trigger_error('WPG2 PAGE ISDONE()', E_USER_NOTICE, __FILE__);
			}

		} else {
			// WPG2 has not been Validated..
			get_header();	
			echo '<div id="message" class="error"><p><strong>'.__('&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Sorry.. WPG2 Page can not be displayed until WPG2 Plugin has been Validated.', 'wpg2').'</strong></p></div>';
			get_footer();
		}

		// Terminate Processing
		exit;

	} 
}

?>