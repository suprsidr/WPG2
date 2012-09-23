<?php
/*
	Author: WPG2 Team
	Updated: 12:09 3/05/2008

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
* Performs a Series of Checks to Ensure WPG2 will function within the Wordpress/Gallery2 Environment
*
* @param NULL
* @return string $html The album tree HTML
*/
function wpg2_validate($wpg2_outputtype) {

	global $wp_db_version;

	//If we're setting options,
	if ( isset($_POST['action']) ) {
		$wpg2_option = get_option('wpg2_options');

		// Option Saving Time
		foreach ($_POST['wpg2_options'] as $key=>$value){
			$wpg2_option[$key] = $value;
		}

		update_option('wpg2_options', $wpg2_option);
	}

	// Initialisation
		$validate_err=0;
		$validate_warning=0;
		//Get Current Gallery2 Plug-in Options
		$wpg2_option = get_option('wpg2_options');
		$wpg2_g2path = get_option('wpg2_g2paths');

	// Do We Have Paths?
	if (empty($wpg2_g2path['g2_filepath']) || empty($wpg2_g2path['wpg2_relativepath']) ) {
		$ret = wpg2_simple_find_paths();

		if (!$ret) {
			// Clean up Permalink Structure
			wpg2_trimpermalinkrules();
			// Verify WPG2 Internal g2_embeduri setting
			wpg2_template_page();
			$wpg2_g2path = get_option('wpg2_g2paths');
		}
	}

	// Test Wordpress Version
		$environment_html = '<strong>';
		$environment_html .=  __('1. Wordpress Compatible? - [' , 'wpg2');
		if ($wp_db_version >=$wpg2_option['g2_wpversion_low']) {
			$environment_html .= '<font color="green">' . __('	Success', 'wpg2') . '</font>';
		} else {
			define("WPVERSIONERR", "True");
			$environment_html .= '<font color="red">' . __('Failed', 'wpg2') . '</font>';
			$validate_err=1;
		}
		$environment_html .= '] <br />';


	// Test Gallery2 URL
		if (!$validate_err) {
			$environment_html .=  __('2. URI to Gallery2 Ok? - [' , 'wpg2');
			$ret = wpg2_validateuri($wpg2_g2path['g2_url'].'embed.php');
			if (!$ret) {
				$environment_html .= '<font color="green">' . __('Success', 'wpg2') . '</font>';
			} else {
				define("G2EMBEDERR", "True");
				$validate_warning=1;
				$reterr = $ret;
				$environment_html .= '<font color="orange">' . __('Warning', 'wpg2') . '</font>';
			}
			$environment_html .= '] <br />';
		} 

	// Gallery Ok?
		if (!$validate_err) {
			$environment_html .= __('3. Gallery2 can be located in G2 File Path? - [', 'wpg2');
			if (file_exists( $wpg2_g2path['g2_filepath'].'embed.php' ) )
				$environment_html .= '<font color="green">' . __('Success', 'wpg2').'</font>';
			else {
				$environment_html .= '<font color="red">' . __('Failed', 'wpg2').'</font>';
				define("G2PATHERR", "True");
				$validate_err=1;
			}
			$environment_html .= '] <br />';
		}

	// Test Initialise G2
		if (!$validate_err) {
			$environment_html .= __('4. Gallery2 can be initialised? - [', 'wpg2');
			if (!defined('G2INIT')) {
				$ret = g2_init();
				if (!$ret)
					$environment_html .= '<font color="green">' . __('Success', 'wpg2') . '</font>';
				else {
					define("G2INITERR", "True");
					$environment_html .= '<font color="red">' . __('Failed', 'wpg2') . '-><font>';
					$validate_err=1;
				}
			} else {		
				$environment_html .= '<font color="green">' . __('Success', 'wpg2') . '</font>';
			}
			$environment_html .= '] <br />';
		} 

	// Test G2 Version
		if (!$validate_err) {
			$environment_html .= __('5. Gallery2 Version Compatible? - [' , 'wpg2');
			list ($ret, $core) = GalleryCoreApi::loadPlugin('module', 'core', true);
	        $g2version = $core->getInstalledVersions();
			$g2version = substr($g2version['gallery'],0,3);
			if ($wpg2_option['g2_version_low'] <= $g2version)
				$environment_html .= '<font color="green">' . __('Success', 'wpg2') . '</font>';
			else {
				$environment_html .= '<font color="red">' . __('Failed', 'wpg2') . '</font>';
				define("G2VERERR", "True");
				$validate_err=1;
			}
			$environment_html .= '] <br />';
		} 

	// ImageBlock Active?
		if (!$validate_err) {
			$environment_html .= __('6. Gallery2 Module: ImageBlock Active? - [' , 'wpg2');
			$g2_image_block_active = 0;
			$g2_moduleid = 'imageblock';
			list ($ret, $g2_modulestatus ) = GalleryCoreApi::fetchPluginStatus('module');
			if (!$ret) {
				if (isset($g2_modulestatus[$g2_moduleid]) && !empty($g2_modulestatus[$g2_moduleid]['active']) && $g2_modulestatus[$g2_moduleid]['active'])
					$environment_html .= '<font color="green">' . __('Success', 'wpg2') . '</font>';
				else {
					$environment_html .= '<font color="red">' . __('Failed', 'wpg2') . '</font>';
					define("G2IMAGEBLOCKERR", "True");
					$validate_err=1;
				}
			}
			$environment_html .= '] <br />';
		} 

	// Test ImageBlock Version
		if (!$validate_err) {
			$environment_html .= __('7. Gallery2 Module: ImageBlock Version Compatible? - [', 'wpg2');
			GalleryCoreApi::requireOnce('modules/imageblock/module.inc');
			GalleryCoreApi::requireOnce('modules/core/classes/GalleryRepositoryUtilities.class');
			$plugin = new ImageBlockModule;
			$version = $plugin->getVersion();
			$version_comparison = GalleryRepositoryUtilities::compareRevisions($version,$wpg2_option['g2_imageblock_version_low']);
			if ($version_comparison != 'older') {
				$environment_html .= '<font color="green">' . __('Success', 'wpg2') . '</font>';
			} else {
				define("G2IMAGEBLOCKVERSIONERR", "True");
				$validate_warning=1;
				$reterr = $ret;
				$environment_html .= '<font color="orange">' . __('Warning', 'wpg2') . '</font>';
			}
			$environment_html .= '] <br />';
		} 

	// ImageFrame Active?
		if (!$validate_err) {
			$environment_html .=  __('8. Gallery2 Module: ImageFrame Active? - [' , 'wpg2');
			$g2_image_block_active = 0;
			$g2_moduleid = 'imageframe';
			if (!$ret) {
				if (isset($g2_modulestatus[$g2_moduleid]) && !empty($g2_modulestatus[$g2_moduleid]['active']) && $g2_modulestatus[$g2_moduleid]['active'])
					$environment_html .= '<font color="green">' . __('Success', 'wpg2') . '</font>';
				else {
					$environment_html .= '<font color="red">' . __('Failed', 'wpg2') . '</font>';
					define("G2IMAGEFRAMEERR", "True");
					$validate_err=1;
				}
			}
			$environment_html .= '] <br />';
		}

	// ImageFrame can be found in Relative Path?
		if (!$validate_err) {
			$environment_html .=  __('9. Gallery2 to WPG2 Relative Path Validate? - [' , 'wpg2');
			if (file_exists($wpg2_g2path['g2_filepath'].$wpg2_g2path['wpg2_relativepath'].DIRECTORY_SEPARATOR.'templates'.DIRECTORY_SEPARATOR.'wpg2imageblock.tpl' ) ) {
					$environment_html .= '<font color="green">' . __('Success', 'wpg2') . '</font>';
			} else {
					$environment_html .= '<font color="red">' . __('Failed', 'wpg2') . '</font>';
					define("G2RELPATHERR", "True");
					$validate_err=1;
			}
			$environment_html .= '] <br /></strong></td></tr>';
		}


	// End Validation Checks
		if (!$validate_err) {
			$environment_html .=  '<tr><td><font size="4">' . __('WPG2 Page Generation Rules: ', 'wpg2') . '&nbsp;</font><a href="'.$wpg2_g2path['g2_embeduri'].'">'.'(View WPG2 Output Page &raquo;)</a><strong><br />';

		// WPG2 Page Output Type?
			$environment_html .= __('1. WPG2 Output Style? - [', 'wpg2');
			if ( $wpg2_option['g2_embedpagetype'] == 'wordpress' ) {
				$environment_html .= '<font color="green">' . __('Look like a Wordpress Page', 'wpg2'). '</font>';
			} else {
				$environment_html .= '<font color="green">' . __('Look like a Gallery2 Page', 'wpg2'). '</font>';
			}
			$environment_html .= '] <br />';

		// WPG2 / Gallery Rewrites Active?
			$environment_html .= __('2. WPG2 Gallery2 Rewrites Status? - [', 'wpg2');
			if ($wpg2_option['g2_rewriteactive'] == 'active') {
				$environment_html .= '<font color="green">' . __('On', 'wpg2'). '</font>';
			} else {
				if ($wpg2_option['g2_rewriteactive'] == 'safe')
					$environment_html .= '<font color="green">' . __('Off [Safe]', 'wpg2'). '</font>';
				else
					$environment_html .= '<font color="green">' . __('Off', 'wpg2'). '</font>';
			}
			$environment_html .= '] <br />';

		// Custom Header?
			$environment_html .= __('3. Embedded Page is using what Header type? - [', 'wpg2');
			if ( file_exists( TEMPLATEPATH . '/wpg2header.php') ) {
				$environment_html .= '<font color="green">' . __('WPG2HEADER', 'wpg2'). '</font>';
				$wpg2_option['g2_externalheader'] = "Yes";
			} else {
				if ($wpg2_option['g2_embedpagetype'] != 'gallery2' )
					define('WPSIMPLEHEADER',"True");
				$environment_html .= '<font color="green">' . __('Simple', 'wpg2'). '</font>';
				$wpg2_option['g2_externalheader'] = "No";
			}
			$environment_html .= '] <br />';

		// Custom Footer?
			$environment_html .= __('4. Embedded Page is using what Footer type? - [', 'wpg2');
			if ( file_exists( TEMPLATEPATH . '/wpg2footer.php') ) {
				$environment_html .= '<font color="green">' . __('WPG2FOOTER', 'wpg2'). '</font>';
				$wpg2_option['g2_externalfooter'] = "Yes";
			} else {
				$environment_html .= '<font color="green">' . __('Simple', 'wpg2'). '</font>';
				$wpg2_option['g2_externalfooter'] = "No";
			}
			$environment_html .= '] <br />';

		// External WPG2 CSS
			$environment_html .= __('5. Using Which WPG2 Stylesheet? - [', 'wpg2');
			if ( file_exists( TEMPLATEPATH . '/wpg2.css') ) {
				$environment_html .= '<font color="green">' . __('External', 'wpg2'). '</font>';
				$wpg2_option['g2_externalcss'] = "Yes";
			} else {
				$environment_html .= '<font color="green">' . __('Internal', 'wpg2'). '</font>';
				$wpg2_option['g2_externalcss'] = "";
			}
			$environment_html .= '] </strong></td></tr>';
		}
		
		$environment_html .= '</table>';

		if ($validate_err)
			$wpg2_option['g2_validated'] = "No";
		else {
			if (!$wpg2_option['g2_rewriteactive'] == 'active') {
				$g2_moduleid = 'rewrite';
				list ($ret, $g2_modulestatus ) = GalleryCoreApi::fetchPluginStatus('module');
				if (!$ret && isset($g2_modulestatus[$g2_moduleid]) && !empty($g2_modulestatus[$g2_moduleid]['active']) && $g2_modulestatus[$g2_moduleid]['active']) {
				// Deactivate Rewrite Module as Wordpress Permalink Array will be wrong.
					list ($ret, $ignored) = GalleryCoreApi::deactivatePlugin('module', 'rewrite');
					if (!$ret)
						define("G2REWRITEDEACTIVATED", "True");
				}
			}
			$wpg2_option['g2_validated'] = "Yes";
			// Close Gallery Connection
			GalleryEmbed::done();
		}

	// Update the Options
		update_option('wpg2_options', $wpg2_option);

	// Add BTEV Event Message
	if (function_exists('btev_trigger_error')) {
		btev_trigger_error('WPG2 VALIDATION ('.$wpg2_option['g2_validated'].') ', E_USER_NOTICE, __FILE__);
	}

	//Output Status
	if ($wpg2_outputtype != "silent") {

		echo '<div class="wrap">';
        echo '<h2>'. __('WPG2 Dashboard').'</font><font size="2">&nbsp;<a href="http://codex.gallery2.org/Integration:WPG2_Options_-_Validation" rel="external">'. __('(Help)', 'wpg2').'</a></font></h2>';
		?>
			<form name="g2options" method="post" action="">
				<table cellspacing="1" cellpadding="5" class="form-table">
				<input type="hidden" name="action" value="update" />
				<tr valign="top">
				<td><strong><?php _e('Should Gallery2 Page(s) look like a Wordpress Page?', 'wpg2') ?>&nbsp; 
				<?php _e('Yes', 'wpg2') ?> 
				<input name="wpg2_options[g2_embedpagetype]" type="radio" value="wordpress" <?php if ($wpg2_option['g2_embedpagetype'] == 'wordpress'){echo "checked ";}?> />
				<?php _e('&nbsp;&nbsp;No', 'wpg2') ?>
				<input name="wpg2_options[g2_embedpagetype]" type="radio" value="gallery2" <?php if ($wpg2_option['g2_embedpagetype'] == 'gallery2'){echo "checked ";}?> />
				</strong>&nbsp&nbsp
				<input type="submit" name="Submit" value="<?php _e('Change', 'wpg2') ?>" />
				</td></tr>
				</table>
			</form>
		<?php
		echo "<table cellpadding=1 cellspacing=0 class='form-table'>";
		if ($validate_err){
			echo '<tr><td><font size="4">' . __('WPG2 Validation Check: ', 'wpg2') . '<font color="red">' . __('Failed', 'wpg2') . '</font>&nbsp;<a href="http://codex.gallery2.org/Integration:WPG2_Options_-_Validation" rel="external">'. __('(Help)', 'wpg2').'</a></font><br />';
			echo $environment_html;
		} else {
			echo '<tr><td><font size="4">' . __('WPG2 Validation Check: ', 'wpg2') . '<font color="green">' . __('Valid', 'wpg2') . '</font>&nbsp;<a href="http://codex.gallery2.org/Integration:WPG2_Options_-_Validation" rel="external">'. __('(Help)', 'wpg2').'</a></font><br />';
			echo $environment_html;
		}

		if (defined('WPVERSIONERR')) {
			echo '<div id = "wpversion_form">';
			echo '<br /><h2>' . __('Error: Wordpress Build is NOT Compatible', 'wpg2').'</h2>'. __(' WPG2 ','wpg2').$wpg2_option['g2_wpg2version'];
			_e(' requires a minimum Wordpress build of ', 'wpg2');
			echo $wpg2_option['g2_wpversion_low'];
			_e (' and has detected your Wordpress build ', 'wpg2');
			echo $wp_db_version;
			_e (' does not meet the minimum build requirement.', 'wpg2');
			echo '<br /><br /></div>';
		}

		if (defined('G2EMBEDERR')) {
			echo '<div id = "g2embed_form">';
			echo '<br /><h3>'. __('Warning: Possible invalid URL to Gallery2', 'wpg2').'</h3>'. __('Possible Validation Problem of Gallery2 URL.  The error returned reads: "', 'wpg2').$reterr.'"<br /><br />';
			echo '</div>';
		}

		if (defined('G2PATHERR')) {
			echo '<div id = "g2path_form">';
			echo '<br /><h2>'. __('Error: embed.php cannot be located in the specified File Path.', 'wpg2').'</h2>';
			_e('WPG2 plugin cannot locate the Gallery2 installation.<br /><br />Upon activation, WPG2 automatically looks for your Gallery2 installation in (Domain URL)/gallery2/, (Domain URL)/gallery/, (Domain URL)/photos/, (WordPress URL)/gallery2/, (WordPress URL)/gallery/, and (WordPress URL)/photos/.  If your Gallery2 installation is in another directory, please go to the "WPG2 Paths" tab and enter your Gallery2 URL. ', 'wpg2');
			echo ($wpg2_g2path['g2_filepath']). __('embed.php', 'wpg2').'<br /><br />';
			echo '</div>';
		}


		if (defined('G2INITERR')) {
			echo '<div id = "g2init_form">';
			echo '<br /><h2>'. __('Error: Gallery2 cannot be initialised', 'wpg2').'</h2>'. __('Check to ensure Gallery2 is correctly working and revalidate.', 'wpg2').'<br />';
			echo $ret.'<br /><br />';
			echo '</div>';
		}

		if (defined('G2VERERR')) {
			echo '<div id = "g2version_form">';
			echo '<br /><h2>'. __('Error: Gallery2 version is incompatible','wpg2').'</h2>'. __('WPG2 plugin requires at least Gallery2 version ', 'wpg2');
			echo $wpg2_option['g2_version_low']. __(', Gallery2 reports you are running version ','wpg2');
			echo $g2version.'<br /><br />';
			echo '</div>';
		}

		if (defined('G2IMAGEBLOCKERR')) {
			echo '<div id = "g2imageblock_form">';
			echo '<br /><h2>'. __('Error: Gallery2 Imageblock plugin is not activated','wpg2').'</h2>'. __('Ensure the Gallery2 ImageBlock is installed / activated via the Gallery2 Site Admin -> Plugin Panel.', 'wpg2').'<br /><br />';
			echo $ret;
			echo '</div>';
		}

		if (defined('G2IMAGEBLOCKVERSIONERR')) {
			echo '<div id = "g2imageblock_form">';
			echo '<br /><h3>'. __('Warning: Gallery2 Imageblock plugin is an old version.','wpg2').'</h3>'. __('Please update to the most recent version from your Gallery2 page: Site Admin -> Plugins.<br />If you do not upgrade the ImageBlock plugin, you will not be able to use the size option with WPG2 tags.', 'wpg2').'<br /><br />';
			echo $ret;
			echo '</div>';
		}

		if (defined('G2IMAGEFRAMEERR')) {
			echo '<div id = "g2imageframe_form">';
			echo '<br /><h2>'. __('Error: Gallery2 Imageframe plugin is not activated','wpg2').'</h2>'. __('Ensure the Gallery2 ImageFrame is installed / activated via the Gallery2 Site Admin -> Plugin Panel.', 'wpg2').'<br /><br />';
			echo $ret;
			echo '</div>';
		}

		if (defined('G2RELPATHERR')) {
			echo '<div id = "g2path_form">';
			echo '<br /><h2>'. __('Error: Gallery2 to WPG2 relative Path cannot be validated','wpg2').'</h2>'. __('The file wpg2imageblock.tpl must be within this path ', 'wpg2');
			echo $wpg2_g2path['g2_filepath'].$wpg2_g2path['wpg2_relativepath'].DIRECTORY_SEPARATOR.'templates'.DIRECTORY_SEPARATOR.'wpg2imageblock.tpl'.__(' as specified in the <strong>Gallery2 to WPG2 Relative Path</strong> in WPG2 Paths Tab','wpg2').'<br /><br />';
			echo '</div>';
		}

		if (defined('G2REWRITEDEACTIVATED')) {
			echo '<div id = "simpleheader_form">';
			echo '<br /><h3>'. __('Warning: Gallery2 Rewrite Plugin was deactived','wpg2').'</h3>'. __('WPG2 has automatically deactivated the Gallery2 Rewrite Plugin, to reactive, enable Gallery2 Rewrites in the WPG2 Gallery2 Rewrites Tab.','wpg2').'<br /><br />';
			echo '</div>';
		}

		if (defined('WPSIMPLEHEADER')) {
			echo '<div id = "simpleheader_form">';
			echo '<h3>'. __('Warning: Using the Simple Header','wpg2').'</h3>'. __('WPG2 will generate your Wordpress header using the CSS Elements specified in the WPG2 Output options. For some Wordpress themes this will incorrectly distort the output from Gallery2 and instead you may need to create your own WPG2 Header.','wpg2').'<br /><br />';
			echo '</div>';
		}

		echo '</div></div>';

	}

}

?>