<?php
/*
	Author: WPG2 Team
	Updated: 17:27 19/04/2008

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
*/

/**
* Option to Enable Gallery2 Rewrite Module
*
* @param NULL
* @return NULL
*/
function wpg2_g2managerewrites() {

	global $wp_rewrite;

	//If we're setting options
	if ( isset($_POST['action']) ) {
		$wpg2_option = get_option('wpg2_options');
		// Option Saving Time
		foreach ($_POST['g2_options'] as $key=>$value){
			$wpg2_option[$key] = $value;
		}
		update_option('wpg2_options', $wpg2_option);

		if ($wpg2_option['g2_rewriteactive'] == 'active') {
		// Add BTEV Event Message
		if (function_exists('btev_trigger_error')) {
			btev_trigger_error('WPG2 G2 REWRITE PLUGIN ACTIVATED', E_USER_NOTICE, __FILE__);
		}
		// Rebuild Rules
			wpg2_rebuildrewriterules();
			?><div id="message" class="updated fade"><p><strong><?php _e('Gallery2 Rewrite Module Activated.', 'wpg2') ?></strong></p></div><?php
		}
	}

	// Get WPG2 Option Settings
	$wpg2_option = get_option('wpg2_options');

	if ( $wpg2_option['g2_validated'] == 'Yes' ) {

		?>
		<div class="wrap">
		<h2><?php _e('Gallery2 Rewrites', 'wpg2') ?></font><font size="2">&nbsp;<a href="http://codex.gallery2.org/Integration:WPG2_Options_-_G2_Rewrites" rel="external"><?php _e('(Help)', 'wpg2') ?></a></font></h2>
		<form name="gallery2rewrites" method="post" action="">
		<input type="hidden" name="action" value="update" />
		<?php

		$ret = wpg2_g2rewritesvalidate('verbose',$wpg2_option);

		if ($wpg2_option['g2_rewriteactive'] == '' && !$ret)   {
			$wpg2_option['g2_rewriteactive'] = 'safe';
			// Update the Options
			update_option('wpg2_options', $wpg2_option);
		}


		// Stop on Failure, otherwise Allow Activation
		if (!$ret) {
			?>
				<h2><?php _e('Gallery2 Rewrite Plugin:', 'wpg2') ?>
				<select name="g2_options[g2_rewriteactive]">
				<?php if ($wpg2_option['g2_rewriteactive'] == 'active') {
					?><option label="<?php _e('rewrite_active', 'wpg2') ?>" value="active"<?php if ('active' == $wpg2_option['g2_rewriteactive']){echo " selected";}?>><?php _e('On', 'wpg2') ?></option>
				<?php } if ($wpg2_option['g2_rewriteactive'] == 'safe') {
					?><option label="<?php _e('rewrite_active', 'wpg2') ?>" value="active"<?php if ('active' == $wpg2_option['g2_rewriteactive']){echo " selected";}?>><?php _e('On', 'wpg2') ?></option>
				<?php } if ($wpg2_option['g2_rewriteactive'] == 'safe') {
					?><option label="<?php _e('rewrite_disabled', 'wpg2') ?>" value="safe"<?php if ('safe' == $wpg2_option['g2_rewriteactive']){echo " selected";}?>><?php _e('Off [Safe]', 'wpg2') ?></option>
				<?php } if ($wpg2_option['g2_rewriteactive'] != 'safe') {
					?><option label="<?php _e('rewrite_disabled', 'wpg2') ?>" value=""<?php if ('' == $wpg2_option['g2_rewriteactive']){echo " selected";}?>><?php _e('Off', 'wpg2') ?></option>
				<?php } ?>
				</select></h2>
				<?php
				echo '<h3>'. __('Warning: Do not Activate until you have read the following: ','wpg2').'</h3>'. __("Gallery2 simplified URLs may not correctly function in your hosting environment.  If you experience any problems (EG: 404 Errors) when navigating to your photos or albums, then please deactivate this setting.", 'wpg2').'<br /><br />'. __('<strong>NOTE: </strong>You cannot use the Gallery2 "Site Admin" URL rewrite rule with WPG2.  It does not work correctly in embedded mode and will break your "Site Admin" link in the embedded gallery.  Please ensure that the "Site Admin" rule is off.', 'wpg2');
				?>
				<br /><br />
				<p class="submit">
				<input type="submit" name="Submit" value="<?php _e('Save Changes', 'wpg2') ?>" />
				</p>
			</form>
			</div>
			<?php
		}
	} else {
		?><div id="message" class="error"><p><strong><?php _e('Options not available until WPG2 Plugin is Validated.', 'wpg2') ?></strong></p></div>
		<?php
	}

}

/**
* Verify Gallery2 Rewrite Rules Should Function within the Wordpress Environment
*
* @param array WPG2 Options
* @param string validation verbose Setting
* @return int validation result
*/
function wpg2_g2rewritesvalidate($wpg2_outputtype, $wpg2_option ) {

	global $wp_rewrite;

	// Initialisation
		$validate_err=0;

// WP Permalinks Active?
	$environment_html = '<strong>';
	$environment_html .=  __('1. Wordpress Permalinks Active? - [' , 'wpg2');
	$wppagestruct = $wp_rewrite->get_page_permastruct();
	if	( '' != $wppagestruct )
		$environment_html .= '<font color="green">' . __('Yes', 'wpg2') . '</font>';
	else {
		$environment_html .= '<font color="red">' . __('No', 'wpg2') . '</font>';
		$validate_err = 1;
		define("WPNOPERMALINKS", "True");
	}
	$environment_html .= '] <br />';

	// Verify WPG2 Internal g2_embeduri setting
	wpg2_template_page();

	if (!defined('G2INIT')) {
		$ret = g2_login();
		if ($ret) {
			echo '<h2>' . __('Fatal G2 error', 'wpg2') . '</h2> ' . __("Here's the error from G2: ", 'wpg2') . $ret->getAsHtml();
			exit;
		}
	}

// Does the Rewrite Plugin Exist?
	if (!$validate_err) {
		$environment_html .= __('2. Is Gallery2 Rewrite Plugin Loaded? - [' , 'wpg2');
		list ($ret, $g2_modulestatus ) = GalleryCoreApi::fetchPluginStatus('module');
		if (!$ret) {
			if (isset($g2_modulestatus['rewrite']))
				$environment_html .= '<font color="green">' . __('Yes', 'wpg2') . '</font>';
			else {
				$environment_html .= '<font color="red">' . __('No', 'wpg2') . '</font>';
				$validate_err = 1;
				define("G2REWRITEMODULEMISSING", "True");
			}
		} else {
			$environment_html .= '<font color="red">' . __('No', 'wpg2') . '</font>';
			$validate_err = 1;
		}
		$environment_html .= '] <br />';
	}

// Is the Rewrite Plugin WP Paths able to be Set?
	if (!$validate_err) {
		$environment_html .= __('3. Can Gallery2 Rewrite WP Paths be Set? - [' , 'wpg2');
		$ret = g2_setrewriteepath(); // Set the WP Path
		if (!$ret) {
			$environment_html .= '<font color="green">' . __('Yes', 'wpg2') . '</font>';
		} else {
			$environment_html .= '<font color="red">' . __('No', 'wpg2') . '</font>';
			$validate_err = 1;
		}
		$environment_html .= '] <br />';
	}

// Check For Pathinfo
	if (!$validate_err) {
		$environment_html .=  __('4. Is Gallery2 Rewrite using Apache Mod_Rewrite? - [' , 'wpg2');
		GalleryCoreApi::requireOnce('modules/rewrite/classes/RewriteHelper.class');
		list ($ret, $rewriteParser) = RewriteHelper::getRewriteParser();
		if (!$ret) {
			if ('modrewrite' == $rewriteParser->getParserId())
					$environment_html .= '<font color="green">' . __('Yes', 'wpg2') . '</font>';
			else {
				$environment_html .= '<font color="red">' . __('No', 'wpg2') . '</font>';
				define("G2REWRITEPATHINFO", "True");
				$validate_err=1;
			}
		} else {
			$environment_html .= '<font color="red">' . __('NA', 'wpg2') . '</font>';
			define("G2REWRITEMODULEERR", "True");
			$validate_err = 1;
		}
		$environment_html .= '] <br />';
	}

// Gallery2 HTACCESS Update
	if (!$validate_err) {
		$environment_html .= __('5. Wordpress .htaccess can be updated? - [' , 'wpg2');
		$errreturn = g2_verifyhtaccess();
		if ($errreturn) {
			$environment_html .= '<font color="red">' . __('No', 'wpg2') . '</font>';
			$errstr = __('Failed to update because ', 'wpg2').$errreturn;
			define("G2REWRITERULESERR", "True");
			$validate_err=1;
		} else {
			$environment_html .= '<font color="green">' . __('Yes', 'wpg2') . '</font>';
		}
	}

	$environment_html .= "] </td></tr>";
	$environment_html .= "</table><br/><br/>";

	if ($validate_err || $wpg2_option['g2_rewriteactive'] != 'active') {
		// Check G2 Rewrite Module, it should be deactivated
		$g2_moduleid = 'rewrite';
		list ($ret, $g2_modulestatus ) = GalleryCoreApi::fetchPluginStatus('module');
		if (isset($g2_modulestatus[$g2_moduleid]) && !empty($g2_modulestatus[$g2_moduleid]['active']) && $g2_modulestatus[$g2_moduleid]['active']) {
			list ($ret, $ignored) = GalleryCoreApi::deactivatePlugin('module', 'rewrite');
		}
	} else {
		$ret = g2_activaterewrites();
	}

	//Close Gallery Connection
	GalleryEmbed::done();

	//Output Status
	if ($wpg2_outputtype != "silent") {

		echo "<table cellpadding=1 cellspacing=0 class='form-table' >";
		if ($validate_err) {
			echo '<tr><td><font size="4">' . __('Rewrite Validation Check: ', 'wpg2') . '<font color="red">' . __('Failed', 'wpg2') . '</font>&nbsp;<a href="http://codex.gallery2.org/Integration:WPG2_Options_-_G2_Rewrites" rel="external">'. __('(Help)', 'wpg2').'</a></font><br />';
			// Add BTEV Event Message
			if (function_exists('btev_trigger_error')) {
				btev_trigger_error('WPG2 G2 REWRITE PLUGIN VALIDATION FAILED', E_USER_NOTICE, __FILE__);
			}
		}
		else {
			echo '<tr><td><font size="4">' . __('Rewrite Validation Check: ', 'wpg2') . '<font color="green">' . __('Valid', 'wpg2') . '</font>&nbsp;<a href="http://codex.gallery2.org/Integration:WPG2_Options_-_G2_Rewrites" rel="external">'. __('(Help)', 'wpg2').'</a></font><br />';
			// Add BTEV Event Message
			if (function_exists('btev_trigger_error')) {
				btev_trigger_error('WPG2 G2 REWRITE PLUGIN VALIDATION PASSED', E_USER_NOTICE, __FILE__);
			}
		}
		echo $environment_html;

		if (defined('WPNOPERMALINKS')) {
			echo '<div id = "g2rewrite_form">';
			echo '<h2>'. __('Error: Wordpress Permalinks is not Active','wpg2').'</h2>'. __("In order to support Gallery2 Rewrite rules, you must have a Wordpress Permalinks Active, please Configure Wordpress Permalinks from the Wordpres Options Panel.", 'wpg2').'<br /><br />';
			echo $ret;
			echo '</div>';
		}

		if (defined('G2REWRITEMODULEMISSING')) {
			echo '<div id = "g2rewriterules_form">';
			echo '<h2>'. __('Error: Gallery2 Rewrite Plugin is not installed','wpg2').'</h2>'. __("You have not installed the Gallery2 Rewrite Plugin, please download/install the rewrite plugin from the Gallery2 Site Admin Plugin Get More Plugins Panel.", 'wpg2').'<br /><br />';
			echo '</div>';
		}


		if (defined('G2REWRITEMODULEERR')) {
			echo '<div id = "g2rewriterules_form">';
			echo '<h2>'. __('Error: Gallery2 Rewrite Plugin is not Installed/Configured','wpg2').'</h2>'. __("Please install/configure the Gallery2 Rewrite Plugin from the Gallery2 Site Admin Plugin Panel.", 'wpg2').'<br /><br />';
			echo '</div>';
		}

		if (defined('G2REWRITERULESERR')) {
			echo '<div id = "g2rewriterules_form">';
			echo '<h2>'. __('Error: Gallery2 Rewrite Rules Failed to be updated','wpg2').'</h2>'. $errstr.'<br /><br />';
			echo $ret;
			echo '</div>';
		}

		if (defined('G2REWRITEPATHINFO')) {
			echo '<div id = "g2rewrite_form">';
			echo '<h2>'. __('Error: Gallery2 Rewrite is not using mod_rewrite','wpg2').'</h2>'. __("Gallery2 Rewrite module is not using Apache mod_rewrite and cannot be supported within the Wordpress Environment, please configure the Gallery2 Rewrite Module to use Apache Mod_Write.", 'wpg2').'<br /><br />';
			echo $ret;
			echo '</div>';
		}

	}

	return $validate_err;

}

?>