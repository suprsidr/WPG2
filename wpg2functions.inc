<?php
/*
	Author: WPG2 Team
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
********************************************************************************************************
											WP Hooks Functions
********************************************************************************************************
*/

/**
* WP action to add the WPG2 admin menus and submenus
*
* @param NULL
* @return NULL
*/
function wpg2_addwpmenus() {

	// Get WPG2 Options
	$wpg2_option = get_option('wpg2_options');

	if ( current_user_can('manage_options') ) {

	    add_menu_page(__('WPG2 Options - Validate','wpg2'), __('WPG2','wpg2'), 'manage_options', __FILE__, 'wpg2_validate');
		add_submenu_page(__FILE__, 'WPG2 Options - WPG2 Paths', __('WPG2 Paths','wpg2') , 'manage_options',  'wpg2_pathadmin', 'wpg2_pathadmin' );
		add_submenu_page(__FILE__, 'WPG2 Options - Gallery2 Rewrites', __('Gallery2 Rewrites', 'wpg2'), 'manage_options',  'wpg2_g2managerewrites', 'wpg2_g2managerewrites' );
		add_submenu_page(__FILE__, 'WPG2 Options - WPG2 Output', __('WPG2 Output', 'wpg2'), 'manage_options', 'wpg2_optionswpg2outputpage', 'wpg2_optionswpg2outputpage' );
		add_submenu_page(__FILE__, 'WPG2 Options - WPG2 Tags', __('WPG2 Tags', 'wpg2'), 'manage_options',  'wpg2_tagoptions', 'wpg2_tagoptions' );
		add_submenu_page(__FILE__, 'WPG2 Options - Sidebar Block', __('Sidebar Block', 'wpg2'), 'manage_options',  'wpg2_optionssidebarblock', 'wpg2_optionssidebarblock' );
		add_submenu_page(__FILE__, 'WPG2 Options - Sidebar Grid Block', __('Sidebar Grid Block', 'wpg2'), 'manage_options',  'wpg2_optionssidebargridblock', 'wpg2_optionssidebargridblock' );
		add_submenu_page(__FILE__, 'WPG2 Options - Lightbox', __('Lightbox', 'wpg2'), 'manage_options',  'wpg2_optionslightbox', 'wpg2_optionslightbox' );

		if (function_exists('g2image_plugin')) {
			add_submenu_page(__FILE__, 'G2IMAGE Options - Manage Options', __('G2Image', 'wpg2'), 'manage_options', 'g2imageoptions', 'g2imageoptions' );
		}

		if ($wpg2_option['g2_validated'] == "Yes" && current_user_can('edit_users')) {
			add_submenu_page('profile.php', 'WPG2 Gallery2 Users - Manage Users', __('Gallery2 Users', 'wpg2'), 'edit_users', 'wpg2/wpg2usersadmin.php' );
		}
	}
}

/*
********************************************************************************************************
  									WPG2 Output Helpers
********************************************************************************************************
*/

function wpg2_template_pagechanges($wpg2_editedpageid) {

	// Get WPG2 Options
	$wpg2_option = get_option('wpg2_options');

	if ($wpg2_option['g2_embedpageid'] == $wpg2_editedpageid) {

		// Grab New Page Title, Name
		$wpg2_option['wpg2_pagetitle'] = $_POST['post_title'];
		$wpg2_option['wpg2_pagename'] = $_POST['post_name'];
		update_option('wpg2_options', $wpg2_option);

		// Rebuild Everything
		wpg2_template_rewritechanges(TRUE);
		$ret = wpg2_g2rewritesvalidate('silent',$wpg2_option);

		// Add BTEV Event Message
		if (function_exists('btev_trigger_error')) {
			btev_trigger_error('WPG2 TEMPLATE WORDPRESS PAGE EDITED', E_USER_NOTICE, __FILE__);
		}

	}
}

/**
* Function Hook on Wordpress Page Deletes
* Sets WPG2 Output Page to the external Gallery2 main.php as WPG2 can no longer display output via internal Wordpress Page
*
* @param NULL
* @return NULL
*/
function wpg2_template_pagedeletes($wpg2_deletepageid) {

	// Get WPG2 Options
	$wpg2_option = get_option('wpg2_options');

	if ($wpg2_option['g2_embedpageid'] == $wpg2_deletepageid) {

		// Add BTEV Event Message
		if (function_exists('btev_trigger_error')) {
			btev_trigger_error('WPG2 TEMPLATE WORDPRESS PAGE ATTEMPTED DELETE', E_USER_NOTICE, __FILE__);
		}

		echo __('Sorry but you cannot delete this page, it is required by the WPG2 Plugin','wpg2');
		exit;
	}
}

/**
* Creates a Wordpress Page
*
* @param array string $WPG2 Page Title, string $WPG2 Page Name
* @return integer $createdpageid
*/
function wpg2_wppagecreate($wpg2_pagetitle='WPG2',$wpg2_pagename='wpg2') {

	global $user_ID;

	// Create New Gallery2 Output Page
	$wpg2_createdpageid = wp_insert_post(array(
				'post_author'		=> $user_ID,
				'post_title'		=> $wpg2_pagetitle,
				'post_name'		 	=> $wpg2_pagename,
				'post_status'		=> "publish",
				'post_type'         => "page",
				'comment_status'	=> "closed",
				'ping_status'		=> " ",
				'post_content'      => "WPG2 Internal Page used for displaying your Gallery2 Content.<br /><br />You can customise this page by changing the Page Title, Page Slug or altering the options in the WPG2 Output Tab.."
			));

	// Add BTEV Event Message
	if (function_exists('btev_trigger_error')) {
		btev_trigger_error('WPG2 ADD TEMPLATE WORDPRESS PAGE ('.$wpg2_createdpageid.')', E_USER_NOTICE, __FILE__);
	}

	return $wpg2_createdpageid;

}

/**
* Updates a Wordpress Page
*
* @param array integer $post_ID, string $WPG2 Page Title, string $WPG2 Page Name
* @return NULL
*/
function wpg2_wppageupdate($wpg2_pageid,$wpg2_pagetitle,$wpg2_pagename) {

	global $user_ID;

	wp_insert_post(array(
				'ID'			=> $wpg2_pageid,
				'post_title'		=> $wpg2_pagetitle,
				'post_name'			=> $wpg2_pagename,
				'post_author'		=> $user_ID,
				'post_status'		=> "publish",
				'post_type'			=> "page",
				'comment_status'	=> "closed",
				'ping_status'		=> " ",
				'post_content'      => "WPG2 Internal Page used for displaying your Gallery2 Content.<br /><br />You can customise this page by changing the Page Title, Page Slug or altering the options in the WPG2 Output Tab.."
			));

	// Add BTEV Event Message
	if (function_exists('btev_trigger_error')) {
		btev_trigger_error('WPG2 UPDATE TEMPLATE WORDPRESS PAGE ('.$wpg2_createdpageid.')', E_USER_NOTICE, __FILE__);
	}

}

/**
* Function Hook to Update the Location of the WPG2 Output Template
* Dynamically Updates the G2URI
*
* @param NULL
* @return NULL
*/
function wpg2_template_page($wpg2safemode=null) {

	// SAFE Mode = No Permalinks, No Gallery2 Rewrite Rules are Active

	global $wp_rewrite;
	$wpg2_option = get_option('wpg2_options');
	$wpg2_g2path = get_option('wpg2_g2paths');
	$wpembedpageid = $wpg2_option['g2_embedpageid'];

	// Set the URI Path to WPG2 Page 
	$wppagestruct = $wp_rewrite->get_page_permastruct();
	if	( '' == $wppagestruct || $wpg2safemode ) {
		$wpg2_g2path['g2_embeduri'] = get_option('home') . "/?page_id=$wpembedpageid";
	} else {
		$wpg2_g2path['g2_embeduri'] = get_permalink($wpembedpageid);
	}

	if ($wpg2safemode && $wpg2_option['g2_rewriteactive'] == 'active' ) {
		wp_schedule_single_event( time(), 'wpg2_cron_g2rewritehook');
		// Add BTEV Event Message
		if (function_exists('btev_trigger_error')) {
			btev_trigger_error('WPG2 G2 REWRITE HOOK CRON SCHEDULED', E_USER_NOTICE, __FILE__);
		}
	}

	if ($wpg2safemode) {
		wp_schedule_single_event( time(), 'wpg2_cron_wprewritehook');
		// Add BTEV Event Message
		if (function_exists('btev_trigger_error')) {
			btev_trigger_error('WPG2 WP PERMALINK HOOK CRON SCHEDULED', E_USER_NOTICE, __FILE__);
		}
	}

	if ($wpg2_option['g2_rewriteactive'] != '' && $wpg2safemode)   {
		$wpg2_option['g2_rewriteactive'] = 'safe';
		// Update the Options
		update_option('wpg2_options', $wpg2_option);
	}

	update_option('wpg2_g2paths', $wpg2_g2path);

	// Add BTEV Event Message
	if (function_exists('btev_trigger_error')) {
		btev_trigger_error('WPG2 TEMPLATE PAGE SLUG VALIDATE', E_USER_NOTICE, __FILE__);
	}

}

/**
* Function Hook on Wordpress Rewrite Rule Changes
* Trim Ending /'s as Gallery2 Trims URI's prior to / resulting in broken 404 URLS
*
* @param NULL
* @return NULL
*/
function wpg2_trimpermalinkrules() {
	global $wp_rewrite;

	// Verify WP Permalink Rules do not have trailing slashes
	$permalink_structure = get_option('permalink_structure');
	$category_base = get_option('category_base');

	if (!empty($permalink_structure) || !empty($category_base) ) {
		if (!empty($permalink_structure)) {
			$permalink_structure = rtrim ($permalink_structure, "/");
			$wp_rewrite->set_permalink_structure($permalink_structure);
		}
	if (!empty($category_base)) {
		$category_base = rtrim ($category_base, "/");
		$wp_rewrite->set_category_base($category_base);
		}
	}

	// Add BTEV Event Message
	if (function_exists('btev_trigger_error')) {
		btev_trigger_error('WPG2 TRIM WP PERMALINK RULES', E_USER_NOTICE, __FILE__);
	}

}

/**
* Rebuilds Wordpress Ruleset.
*
* @param NULL
* @return NULL
*/
function wpg2_rebuildrewriterules() {
	global $wp_rewrite;

	// Get WPG2 Options
	$wpg2_option = get_option('wpg2_options');

	wpg2_trimpermalinkrules();

	// Verify Rewrite Rules
	$permalink_structure = get_option('permalink_structure');
	$category_base = get_option('category_base');

	if (!empty($permalink_structure) || !empty($category_base) ) {
		if (!empty($permalink_structure))
			$wp_rewrite->set_permalink_structure($permalink_structure);
		if (!empty($category_base))
			$wp_rewrite->set_category_base($category_base);
		$wp_rewrite->flush_rules();
	}

	// Add BTEV Event Message
	if (function_exists('btev_trigger_error')) {
		btev_trigger_error('WPG2 (CRON) REBUILD WP REWRITE RULES', E_USER_NOTICE, __FILE__);
	}

	// Verify WPG2 Internal g2_embeduri setting
		wpg2_template_page();
}

/**
* Function Hook on WP Cron Event
* Auto Rebuilds Both Wordpress and Gallery2 Rewrite Rules
*
* @param NULL
* @return NULL
*/
function wpg2_rebuildg2rewriterules() {
	// Get WPG2 Options & Reforce Rewrites to Active Again
	$wpg2_option = get_option('wpg2_options');
	$wpg2_option['g2_rewriteactive'] = 'active';
	update_option('wpg2_options', $wpg2_option);

	// Add BTEV Event Message
	if (function_exists('btev_trigger_error')) {
		btev_trigger_error('WPG2 (CRON) REBUILD G2 REWRITE RULES', E_USER_NOTICE, __FILE__);
	}

	$ret = wpg2_g2rewritesvalidate('silent',$wpg2_option);
	wpg2_rebuildrewriterules();
}


/**
* Function Hook on any updating of Wordpress Permalink Options
* Detactivates Gallery2 Rewrite Module and Forces WPG2 Template to Generate Long Style (SAFE) URL's
*
* @param NULL
* @return NULL
*/
function wpg2_template_rewritechanges($wpg2isinternal=null) {

	// Get WPG2 Options
	$wpg2_option = get_option('wpg2_options');

	if ( strstr( $_SERVER['PHP_SELF'], "options-permalink.php") && ($_POST) || $wpg2isinternal || strstr( $_SERVER['PHP_SELF'], "options-general.php") && ($_GET) ) {

		// Add BTEV Event Message
		if (function_exists('btev_trigger_error')) {
			btev_trigger_error('WPG2 WP PERMALINK RULES MAINTAINED', E_USER_NOTICE, __FILE__);
		}

		if ($wpg2_option['g2_rewriteactive'] == 'active') {
			// Reset Path to WPG2 Output back to Safe Option
			 wpg2_template_page('safe');

			if (!defined('G2INIT')) {
				$ret = g2_login();
				if ($ret) {
					echo '<h2>' . __('Fatal G2 error', 'wpg2') . '</h2> ' . __("Here's the error from G2: ", 'wpg2') . $ret->getAsHtml();
					exit;
				}
			}
			// Deactivate Rewrite Module as Wordpress Permalink Array will be wrong.
			list ($ret, $ignored) = GalleryCoreApi::deactivatePlugin('module', 'rewrite');

			// Add BTEV Event Message
			if (function_exists('btev_trigger_error')) {
				btev_trigger_error('WPG2 G2 REWRITE PLUGIN DEACTIVATED (SAFE MODE)', E_USER_NOTICE, __FILE__);
			}
	
			//Close Gallery Connection
			GalleryEmbed::done();

		} else {
			if (  strstr( $_SERVER['PHP_SELF'], "options-general.php") && ($_GET) ) {
				// Reset Path to WPG2 Output back to Safe Option
				 wpg2_template_page();

			}

		}
	}

}

/**
* Function Hook on Wordpress Rewrite Array
* Adds Gallery2 Rewrite Rules to Wordpress Rewrite Array
*
* @param array $Rules (Wordpress Rewrite Ruleset Array)
* @return NULL
*/
function wpg2_template_rules($wprules) {
	$wpg2_option = get_option('wpg2_options');

	if ($wpg2_option['g2_rewriteactive']) {
		if (!defined('G2INIT')) {
			$ret = g2_login();
			if ($ret) {
				echo '<h2>' . __('Fatal G2 error', 'wpg2') . '</h2> ' . __("Here's the error from G2: ", 'wpg2') . $ret->getAsHtml();
				exit;
			}
		}

		$wpg2_g2path = get_option('wpg2_g2paths');
		$siteUrl = get_option('home');

		list ($ret, $activeRules) = GalleryCoreApi::getPluginParameter('module', 'rewrite', 'activeRules');
		if (!$ret) {
			$activeRules = unserialize($activeRules);

			GalleryCoreApi::requireOnce('modules/rewrite/classes/RewriteHelper.class');
			$regexRules = array();
			$shortUrls = array();
			list ($ret, $parser) = RewriteHelper::getRewriteParser();
			if (!$ret) {
				if (!empty($activeRules)) {
					list ($ret, $code, $regexRules, $shortUrls, $errorId) =
					RewriteHelper::parseActiveRules($activeRules, $parser);
					if ($ret) {
						die;
					}
					if ($code != REWRITE_STATUS_OK) {
						die;
					}
				}
			}
			/* Substitute with what the Gallery URL generator would generate */
			$galleryUrlGenerator = new GalleryUrlGenerator();
			$galleryUrlGenerator->init($siteUrl . '/index.php?page_id='.$wpg2_option['g2_embedpageid'], $wpg2_g2path['g2_url']);
			$wpDirectory = $galleryUrlGenerator->_path[0];
			foreach ($regexRules as $ruleId => $regexRule) {

				/* Substitution */
				$params = $regexRule['queryString'];
				foreach ($regexRule['keywords'] as $reference => $name) {
				if (empty($name)) {
				    continue;
				}
				/* $N are RewriteRule backreferences, %N are RewriteCond backreferences */
				$params[$name] = '%' . $reference;
				}
				$regexRule['substitution'] = $galleryUrlGenerator->generateUrl($params, $regexRule['options']);
				$regexRules[$ruleId] = $regexRule;
			}
			foreach ($regexRules as $rule) {
				$wpg2rewriterule = $rule['pattern'];
				if (strlen($wpg2rewriterule) > 0) {
					if ($wpDirectory != '/') {
						$newrules[$wpg2rewriterule] = preg_replace($wpDirectory,'',$rule['substitution']);
						$newrules[$wpg2rewriterule] = preg_replace('/\/\//','',$newrules[$wpg2rewriterule]);
					}
					else {
						$newrules[$wpg2rewriterule] = preg_replace('/^\//','',$rule['substitution']);
					}
				}
				// Add BTEV Event Message
				if (function_exists('btev_trigger_error')) {
					btev_trigger_error('WPG2 ADDED PERMALINK RULE '.$wpg2rewriterule.'=='.$newrules[$wpg2rewriterule].'', E_USER_NOTICE, __FILE__);
				}
			}

		}
	}

	$newrules = array_merge($newrules,$wprules);
	return $newrules;
}

/**
* Function Hook on Wordpress Header
* Inserts the Lightbox CSS & Javascripts to the Wordpress Page Header
*
* @param NULL
* @return NULL
*/
function wpg2_lightboxheader() {

	$wpg2_option = get_option('wpg2_options');

	// Is Gallery2 Outputting & Lightbox G2 Support is Active?
	if (defined('G2NOHEADERPAGE') && $wpg2_option['wpg2_effectscript'] == 'none' )
		return;

	//Output Lightbox
	if ($wpg2_option['wpg2_effectscript'] == 'lightbox') {
		$lightbox_scriptpath = get_bloginfo('wpurl')."/wp-content/plugins/wpg2/js/";
		$lightbox_csspath = get_bloginfo('wpurl')."/wp-content/plugins/wpg2/css/";
		$lightbox_imgpath = get_bloginfo('wpurl')."/wp-content/plugins/wpg2/img/";
		$lightboxscript.= '<script language="JavaScript" type="text/javascript"> lb_path = "'.$lightbox_imgpath.'"; </script>'."\n";
		$lightboxscript.= "<script type=\"text/javascript\" src=\"".$lightbox_scriptpath."prototype.js\"></script>\n";
		$lightboxscript.= "<script type=\"text/javascript\" src=\"".$lightbox_scriptpath."scriptaculous.js?load=effects\"></script>\n";
		$lightboxscript.= "<script type=\"text/javascript\" src=\"".$lightbox_scriptpath."lightbox.js\"></script>\n";
		$lightboxscript.= "<link rel=\"stylesheet\" href=\"".$lightbox_csspath."lightbox.css\" type=\"text/css\" media=\"screen\" />\n";
		print($lightboxscript);
	}

	//Output Slimbox
	if ($wpg2_option['wpg2_effectscript'] == 'slimbox') {
		$lightbox_scriptpath = get_bloginfo('wpurl')."/wp-content/plugins/wpg2/g2image/jscripts/";
		$lightbox_csspath = get_bloginfo('wpurl')."/wp-content/plugins/wpg2/g2image/css/";
		$lightbox_imgpath = get_bloginfo('wpurl')."/wp-content/plugins/wpg2/images/";
		$lightboxscript.= "<script type=\"text/javascript\" src=\"".$lightbox_scriptpath."mootools.js\"></script>\n";
		$lightboxscript.= "<script type=\"text/javascript\" src=\"".$lightbox_scriptpath."slimbox.js\"></script>\n";
		$lightboxscript.= "<link rel=\"stylesheet\" href=\"".$lightbox_csspath."slimbox.css\" type=\"text/css\" media=\"screen\" />\n";
		print($lightboxscript);
	}

	// Add BTEV Event Message
	if (function_exists('btev_trigger_error')) {
		btev_trigger_error('WPG2 ADD '.$$wpg2_option['wpg2_effectscript'].' TO WP HEADER', E_USER_NOTICE, __FILE__);
	}

}

/**
* Function Hook on Wordpress Admin Header
* Inserts the Help JS into Wordpress Page Header
*
* @param NULL
* @return NULL
*/
function wpg2_adminjsheader() {

	if ( strstr( $_SERVER['PHP_SELF'], "admin.php")) {
		$wpg2help = get_bloginfo('wpurl').'/wp-content/plugins/wpg2/js/helpwindow.js';
		echo '<script type="text/javascript" src="'.$wpg2help.'"></script>';
	}
}


/**
* Function Hook on Wordpress Header
* WP action to add the Gallery2 image frame style sheets to the WP header
*
* @param NULL
* @return NULL
*/
function g2_addheader() {

	if (!defined('G2INIT')) {
		$ret = g2_login();
		if ($ret) {
			echo '<h2>' . __('Fatal G2 error', 'wpg2') . '</h2> ' . __("Here's the error from G2: ", 'wpg2') . $ret->getAsHtml();
			exit;
		}
	}

	global $g2data;

	// Get WPG2 Option Settings
	$wpg2_option = get_option('wpg2_options');

	// Get Gallery2 Paths
	$wpg2_g2path = get_option('wpg2_g2paths');

	if ($wpg2_option['g2_externalheader'] && $wpg2_option['g2_embedpagetype'] != 'gallery2' ) {
		echo '<style type="text/css">'.$wpg2_option['g2_cssheader'].'</style>'."\n";
	}

	if ($wpg2_option['g2_externalcss'])
		$wpg2_csspath = get_bloginfo('stylesheet_directory').'/';
	else
		$wpg2_csspath = get_bloginfo('wpurl')."/wp-content/plugins/wpg2/css/";

	if (!defined('WPG2PAGE')) {
		// Add Album Tree JS
		echo '<script language="javascript" type="text/javascript" src="'.get_bloginfo('wpurl').'/wp-content/plugins/wpg2/js/dtree.js"></script>'."\n";
		// Add Gallery2 CSS For Frames Into WP Header for (Pure WP Pages Only)
		$site_url = trailingslashit(get_option('siteurl'));
		$framenames = array('none');
		$cssoutput = '<link rel="stylesheet" type="text/css" href="'.$wpg2_g2path['g2_url'].'main.php?g2_view=imageframe.CSS&amp;g2_frames=';
		$frameoptions = array('wpg2_sidebargridblocksimageframe','g2_sidebarblockimageframe','g2_sidebarblockalbumframe','g2_tagalbumframe','g2_tagimageframe');
		foreach($frameoptions as $frameoption) {
			$framename = $wpg2_option[$frameoption];
			if($framename && !in_array($framename, $framenames)) {
					if ($cssappendoutput)
						$cssoutput .= '%7C';
					$framenames[] = $framename;
					$cssoutput .= $framename;
					$cssappendoutput = true;
			}
		}
		// Check for None Frame Type (That is No Frames being Set)
		if (empty($cssappendoutput))
			$cssoutput .= 'none';
		// Echo CSS Ouput
		echo $cssoutput."\"/>\n";
	} else {
		// Merge Gallery2 HEAD Outputs Into WP Header 
		if (isset($g2data['headHtml'])) {
			list($g2_title, $g2_css, $g2_javascript) = GalleryEmbed::parseHead($g2data['headHtml']);
			foreach ($g2_css as $wpg2_css) { echo "$wpg2_css \r\n"; }
			foreach ($g2_javascript as $wpg2_javascript) { echo "$wpg2_javascript \r\n"; }
		}
	}
		
	echo '<link rel="stylesheet" type="text/css" media="screen" href="'.$wpg2_csspath.'wpg2.css"/>'."\n";

	// Add BTEV Event Message
	if (function_exists('btev_trigger_error')) {
		btev_trigger_error('WPG2 ADDED G2 HEADER TO WP HEADER', E_USER_NOTICE, __FILE__);
	}

}

/**
* Control the Output of Page Title in the WPG2 Page - // Fix Bug #42
*
* @param NULL
* @return NULL
*/
function g2_outputpagetitle() {

	if (!defined('G2INIT')) {
		// Gallery2 has not yet started, return!
		return;
	}

	global $g2data, $wpg2_wptitle;
	list($g2_title, $g2_css, $g2_javascript) = GalleryEmbed::parseHead($g2data['headHtml']);
		
	// Set WP Title
	$site_name = get_option('blogname');
	$page_name = wp_title(false, false);
	$page_name = ltrim($page_name);

// Add Gallery2 Page to Page Title	
		if ($page_name == $g2_title)
			$wpg2_wptitle .= ' &raquo; '.$g2_title; 		 
		 else
			$wpg2_wptitle .= ' &raquo; '.$g2_title; 

	// Add BTEV Event Message
	if (function_exists('btev_trigger_error')) {
		btev_trigger_error('WPG2 SET GLOBAL WPTITLE TO '.$wpg2_wptitle, E_USER_NOTICE, __FILE__);
	}

}

/**
* Changes the Page Title in the WPG2 Page - // Fix Bug #59
*
* @param NULL
* @return NULL
*/
function wp_outputpagetitle($title) {

	global $wpg2_wptitle;
	$title = $wpg2_wptitle;

	return $title;


}


/*
********************************************************************************************************
											WPG2 Validation Helpers
********************************************************************************************************
*/

/**
* Tries to Guess the Gallery2 URL Location on WPG2 Plugin Activation
*
* @param NULL
* @return $ret
*/
function wpg2_simple_find_paths() {

	// See if we can guess the Gallery2 URL

	$site_url = trailingslashit(get_option('siteurl'));
	$g2url = $site_url.'gallery2';
	$ret = wpg2_validateg2url($g2url,$wpg2relpath);
	if ($ret) {
		$g2url = $site_url.'gallery';
		$ret = wpg2_validateg2url($g2url,$wpg2relpath);
	}
	if ($ret) {
		$g2url = $site_url.'photos';
		$ret = wpg2_validateg2url($g2url,$wpg2relpath);
	}
	if ($ret) {
		$domain_url = rtrim($site_url, '/');
		$domain_url_subset = strlen($domain_url) - strpos(strrev($domain_url), '/');
		$domain_url = substr($domain_url,0,$domain_url_subset);
		$g2url = $domain_url."gallery2";
		$ret = wpg2_validateg2url($g2url,$wpg2relpath);
	}
	if ($ret) {
		$g2url = $domain_url."gallery";
		$ret = wpg2_validateg2url($g2url,$wpg2relpath);
	}
	if ($ret) {
		$g2url = $domain_url."photos";
		$ret = wpg2_validateg2url($g2url,$wpg2relpath);
	}
	// Failed .. Default Paths
	if ($ret) {

		// Get Plugin Base
		$wpg2base = dirname(__FILE__) . '/';
		require_once($wpg2base . 'g2embeddiscoveryutilities.class');
		
		// Set up the URL's and Paths
		$site_url = trailingslashit(get_option('siteurl'));
		$embedUri = G2EmbedDiscoveryUtilities::normalizeG2Uri($site_url);

		// Add BTEV Event Message
		if (function_exists('btev_trigger_error')) {
			btev_trigger_error('WPG2 FIND SIMPLE G2 PATHS FAILED', E_USER_NOTICE, __FILE__);
		}

		// Set the WPG2 Paths
		$wpg2_g2path['g2_url'] = '';
		$wpg2_g2path['g2_filepath'] = '';
		$wpg2_g2path['g2_embeduri'] = '';
		$wpg2_g2path['wpg2_relativepath'] = '..'.DIRECTORY_SEPARATOR.dirname(__FILE__); ; 
		$wpg2_g2path['g2_errorredirect'] = get_option( 'home' );
		$wpg2_g2path['g2_errorredirect'] .= "/wp-login.php";
		update_option('wpg2_g2paths', $wpg2_g2path);
	}

	return $ret;

}

/**
* Function Hook on Wordpress Theme Change
* Revalidates what Wordpress Header / Footer is being used in the new theme.
*
* @param NULL
* @return NULL
*/
function wpg2_themeswitchvalidate() {
		wpg2_validate('silent');
}

/**
* Sets up the WPG2 Plugin defaults, adds any user capabilities.
*
* @param NULL
* @return NULL
*/
function wpg2_pluginactivate() {

// Load the WPG2 translation files
	load_plugin_textdomain('wpg2', 'wp-content/plugins/wpg2/locale');

	// Get WPG2 Option Settings
	$wpg2_option = get_option('wpg2_options');

	// Has WPG2 Plugin Not been correctly deactivated as Options should not yet exist.
	if ($wpg2_option['g2_embedpageid']) {
		// Add BTEV Event Message
		if (function_exists('btev_trigger_error')) {
			btev_trigger_error('WPG2 PLUGIN WAS NOT DEACTIVATED', E_USER_NOTICE, __FILE__);
		}
		wpg2_plugindeactivate();
		$wpg2_option = '';
	}

	global $user_ID;

	// Embedded Page Defaults
	$wpg2_option['g2_externalheader'] = 'No';
	$wpg2_option['g2_externalfooter'] = 'No';
	$wpg2_option['g2_header'] = '<div id="content" class="g2_column">';
	$wpg2_option['g2_footer'] = "</div>";
	$wpg2_option['g2_cssheader'] = '.g2_column {width: 738px;margin: 0px 1px 0px 12px;}';
	$wpg2_option['g2_htmlheader'] = '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">';
	$wpg2_option['g2_htmlheader'] .= "\n".'<html lang="en-US" xmlns="http://www.w3.org/1999/xhtml">';
	$wpg2_option['g2_htmlheader'] .= "\n".'<head>';
	$wpg2_option['g2_htmlfooter'] = '</body></html>';
	$wpg2_option['g2_htmlbody'] = '</head><body class="gallery">';
	$wpg2_option['g2_embedpageid'] = '';
	$wpg2_option['g2_embedpagetype'] = 'gallery2';
	$wpg2_option['wpg2_pagetitle'] = 'WPG2';
	$wpg2_option['wpg2_pagename'] = 'wpg2';
	$wpg2_option['g2_rewriteactive'] = '';

	//Sidebar Grid Block Defaults
	$wpg2_option['g2_sidebargridblockshow'][1] = 'heading';
	$wpg2_option['g2_sidebargridblockstodisplay'] = '';
	$wpg2_option['g2_sidebargridblocksimgsize'] = '85';
	$wpg2_option['g2_sidebargridblockstype'] = '';
	$wpg2_option['g2_sidebargridblockinfo'][1] = '';

	// Sidebar Block Defaults
	$wpg2_option['g2_sidebarblock'][0] = '';
	$wpg2_option['g2_sidebarblockinfo'][1] = 'heading';
	$wpg2_option['g2_sidebarblockshow'][0] = '';
	$wpg2_option['g2_sidebarblockimgsize'] = '150';
	$wpg2_option['g2_sidebarblockimageframe'] = 'None';
	$wpg2_option['g2_sidebarblockalbumframe'] = 'None';

	// WPG2 Tag Defaults
	$wpg2_option['g2_tagimgsize'] = '150';
	$wpg2_option['g2_tagblockshow'] = array(1);
	$wpg2_option['g2_tagalbumframe'] = 'None';
	$wpg2_option['g2_tagimageframe'] = 'None';

	// Lightbox Defaults
	$wpg2_option['wpg2_effectscript'] = 'none';
	$wpg2_option['wpg2_enableg2lightbox'] = '';
	$wpg2_option['wpg2_enablesidebarlightbox'] = '';
	$wpg2_option['wpg2_enabletagslightbox'] = '';
	$wpg2_option['g2_lightboximgsize'] = '300';

	// Get WPG2 Archive IF Exists and repopulate
	$wpg2_archive = get_option('wpg2_30_archive');
	if (!empty($wpg2_archive)) {
		foreach ($wpg2_archive as $key=>$value){
			if (!empty($value)) {
				$wpg2_option[$key] = $value;
			}
		}
		delete_option('wpg2_30_archive');
	}

	// WPG2 Page Has Not been Created
	if ($wpg2_option['g2_embedpageid'] == '') {
		$wpg2_option['g2_embedpageid'] = wpg2_wppagecreate($wpg2_option['wpg2_pagetitle'],$wpg2_option['wpg2_pagename']);
		// Verify WPG2 Internal g2_embeduri setting & Revalidate
		wpg2_template_page();
	}

	update_option('wpg2_options', $wpg2_option);

	if (function_exists('g2image_plugin')) {
		g2image_setdefaults();
		}

	// Get Gallery2 Paths Settings
	$wpg2_g2path = get_option('wpg2_g2paths');

	// Do We Have Paths, If So, Build Output Page Paths
	if (!$wpg2_g2paths['g2_filepath']) {
		wpg2_template_page();
	}

	// Get Gallery2 OLD Option Settings
	$g2_option = get_option('g2_options');

	// Upgrading from WPG2 Releases Prior to 2.2
	if ($g2_option['g2_filepath']) {

		// Clean Up Old Pre 2.2 Settings
		delete_option('g2_options');

	}

	// WPG2 Versions Requirements
	$wpg2_option['g2_version_low'] = '2.2';
	$wpg2_option['g2_imageblock_version_low'] = '1.0.9';
	$wpg2_option['g2_wpversion_low'] = "7558";

	// Add Security Groups
	$wp_roles = new WP_Roles();
	$wp_roles->add_cap('administrator','gallery2_admin');
	$wp_roles->add_cap('administrator','gallery2_user');
	$wp_roles->add_cap('editor','gallery2_user');
	$wp_roles->add_cap('author','gallery2_user');
    $wp_roles->add_cap('subscriber','gallery2_user');
	$wp_roles->add_cap('contributor','gallery2_user');

	$wp_roles->add_cap('author','Unfiltered Html');
	$wp_roles->add_cap('editor','Unfiltered Html');
	$wp_roles->add_cap('contributor','Unfiltered Html');
	$wp_roles->add_cap('administrator','Unfiltered Html');

	update_option('wpg2_options', $wpg2_option);

	// Add BTEV Event Message
	if (function_exists('btev_trigger_error')) {
		btev_trigger_error('WPG2 PLUGIN ACTIVATED', E_USER_NOTICE, __FILE__);
	}

}

/**
* Removes all the WPG2 settings / Caps.
*
* @param NULL
* @return NULL
*/
function wpg2_plugindeactivate() {

// Get Gallery2 Option Settings
	$wpg2_option = get_option('wpg2_options');

// Remove These Filters as WP is incorrectly firing WPG2 After Deactivation
	remove_action('template_redirect', 'wpg2_template');
	remove_action('rewrite_rules_array', 'wpg2_template_rules');
	remove_filter('generate_rewrite_rules', 'wpg2_trimpermalinkrules');
	remove_action('admin_menu', 'wpg2_template_rewritechanges');
	// Filter for WP Page Changes
	remove_action('edit_page_form', 'wpg2_template_pagechanges');
	remove_action('delete_post', 'wpg2_template_pagedeletes');


	// Check to see if this is NOT a Mass Deactivation Event (Wordpress Upgrade) thus leave WPG2 Options Intact!
	 if ($_GET['action'] != 'deactivate-all') {

		// Restore Security Groups
		$wp_roles = new WP_Roles();
		$wp_roles->remove_cap('administrator','gallery2_user');
		$wp_roles->remove_cap('administrator','gallery2_admin');
		$wp_roles->remove_cap('contributor','gallery2_user');
		$wp_roles->remove_cap('editor','gallery2_user');
		$wp_roles->remove_cap('author','gallery2_user');
	    $wp_roles->remove_cap('subscriber','gallery2_user');
		$wp_roles->remove_cap('contributor','gallery2_user');

		$wp_roles->remove_cap('author','Unfiltered Html');
		$wp_roles->remove_cap('editor','Unfiltered Html');
		$wp_roles->remove_cap('contributor','Unfiltered Html');

	// Save Settings into an archive Array

		// Embedded Page Defaults
		$wpg2_archive['g2_externalheader'] = $wpg2_option['g2_externalheader'];
		$wpg2_archive['g2_externalfooter'] = $wpg2_option['g2_externalfooter'];
		$wpg2_archive['g2_header'] = $wpg2_option['g2_header'];
		$wpg2_archive['g2_footer'] = $wpg2_option['g2_footer'];
		$wpg2_archive['g2_embedpagetype'] = $wpg2_option['g2_embedpagetype'];
		$wpg2_archive['wpg2_pagetitle'] = $wpg2_option['wpg2_pagetitle'];
		$wpg2_archive['wpg2_pagename'] = $wpg2_option['wpg2_pagename'];

		//Sidebar Grid Block Defaults
		$wpg2_archive['g2_sidebargridblockshow'] = $wpg2_option['g2_sidebargridblockshow'];
		$wpg2_archive['g2_sidebargridblockstodisplay'] = $wpg2_option['g2_sidebargridblockstodisplay'];
		$wpg2_archive['g2_sidebargridblocksimgsize'] = $wpg2_option['g2_sidebargridblocksimgsize'];
		$wpg2_archive['g2_sidebargridblockstype'] = $wpg2_option['g2_sidebargridblockstype'];
		$wpg2_archive['g2_sidebargridblockinfo'] = $wpg2_option['g2_sidebargridblockinfo'];

		// Sidebar Block Defaults
		$wpg2_archive['g2_sidebarblock'] = $wpg2_option['g2_sidebarblock'];
		$wpg2_archive['g2_sidebarblockinfo'] = $wpg2_option['g2_sidebarblockinfo'];
		$wpg2_archive['g2_sidebarblockshow'] = $wpg2_option['g2_sidebarblockshow'];
		$wpg2_archive['g2_sidebarblockimgsize'] = $wpg2_option['g2_sidebarblockimgsize'];
		$wpg2_archive['g2_sidebarblockimageframe'] = $wpg2_option['g2_sidebarblockimageframe'];
		$wpg2_archive['g2_sidebarblockalbumframe'] = $wpg2_option['g2_sidebarblockalbumframe'];

		// CSS Defaults
		$wpg2_archive['g2_cssheader'] = $wpg2_option['g2_cssheader'];

		// WPG2 Tag Defaults
		$wpg2_archive['g2_tagimgsize'] = $wpg2_option['g2_tagimgsize'];
		$wpg2_archive['g2_tagblockshow'] = $wpg2_option['g2_tagblockshow'];
		$wpg2_archive['g2_tagalbumframe'] = $wpg2_option['g2_tagalbumframe'];
		$wpg2_archive['g2_tagimageframe'] = $wpg2_option['g2_tagimageframe'];

		// Lightbox Defaults
		$wpg2_archive['wpg2_effectscript'] = $wpg2_option['wpg2_effectscript'];
		$wpg2_archive['wpg2_enablelightbox'] = $wpg2_option['wpg2_enablelightbox'];
		$wpg2_archive['wpg2_enableg2lightbox'] = $wpg2_option['wpg2_enableg2lightbox'];
		$wpg2_archive['wpg2_enablesidebarlightbox'] = $wpg2_option['wpg2_enablesidebarlightbox'];
		$wpg2_archive['wpg2_enabletagslightbox'] = $wpg2_option['wpg2_enabletagslightbox'];

		// Save the WPG2 Options into WPG2 Archive
		update_option('wpg2_30_archive', $wpg2_archive);

		// Remove ALL WPG2 Options
		delete_option('wpg2_g2paths');
		delete_option('wpg2_options');

		// Delete the WPG2 Page
		if ( $wpg2_option['g2_embedpageid'] != '' )
			wp_delete_post($wpg2_option['g2_embedpageid']);

		// Add BTEV Event Message
		if (function_exists('btev_trigger_error')) {
			btev_trigger_error('WPG2 DELETE TEMPLATE PAGE ('.$wpg2_option['g2_embedpageid'].')', E_USER_NOTICE, __FILE__);
		}


		if (function_exists('g2image_plugin')) {
			g2image_deactivation();
		}

		// Add BTEV Event Message
		if (function_exists('btev_trigger_error')) {
			btev_trigger_error('WPG2 PLUGIN DEACTIVATED', E_USER_NOTICE, __FILE__);
		}

	 } else {
		// Add BTEV Event Message
		if (function_exists('btev_trigger_error')) {
			btev_trigger_error('WPG2 PLUGIN DEACTIVATED (MASS WP DEACTIVATE ALL)', E_USER_NOTICE, __FILE__);
		}
	}

}

/*
********************************************************************************************************
											WP Plugable Functions
********************************************************************************************************
*/
// Revert WP Password Changes until Gallery2 Changes Can be Implemented.

if ( ! function_exists('wp_check_password') ):
	function wp_check_password($password, $hash, $user_id = '') {
		// If the hash was updated to the new hash before this plugin
		// was installed, rehash as md5.
		if ( strlen($hash) > 32 ) {
			global $wp_hasher;
			if ( empty($wp_hasher) ) {
				require_once( ABSPATH . 'wp-includes/class-phpass.php');
				$wp_hasher = new PasswordHash(8, TRUE);
			}
			$check = $wp_hasher->CheckPassword($password, $hash);
			if ( $check && $user_id ) {
				// Rehash using new hash.
				wp_set_password($password, $user_id);
				$user = get_userdata($user_id);
				$hash = $user->user_pass;
			}

			return apply_filters('check_password', $check, $password, $hash, $user_id);
		}

		$check = ( $hash == md5($password) );

		return apply_filters('check_password', $check, $password, $hash, $user_id);
	}
endif;

if ( !function_exists('wp_hash_password') ):
	function wp_hash_password($password) {
		return md5($password);
	}
endif;


?>