<?php
/*
	Author: WPG2 Team
	Updated: 12:24 3/05/2008

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
* Include Container for WPG2 Options Maintenance
*/

/**
* Maintenance Options to Set WPG2 Paths
*
* @param NULL
* @return NULL
*/
function wpg2_pathadmin() {

//If we're setting options,
	if ( isset($_POST['action']) ) {
		if ($_POST['g2url']) {
			$ret = wpg2_validateg2url($_POST['g2url']);
			if ($ret) {
			// Return the Error
				?><div id="message" class="error"><p><strong><?php echo $ret; ?></strong></p></div><?php
			}
		} else {
			foreach ($_POST['g2_manualpath'] as $key=>$value){
				$wpg2_g2path[$key] = $value;
			}
			// Update G2 Paths & Validate
			if ($wpg2_g2path['g2_url']) {
				$wpg2base = 'wp-content'.DIRECTORY_SEPARATOR.'plugins'.DIRECTORY_SEPARATOR.'wpg2';
				@include_once(ABSPATH . $wpg2base . DIRECTORY_SEPARATOR . 'g2embeddiscoveryutilities.class');
				$wpg2_g2path['g2_url'] = G2EmbedDiscoveryUtilities::normalizeG2Uri($wpg2_g2path['g2_url']);
			}
			$doubleslash = DIRECTORY_SEPARATOR.DIRECTORY_SEPARATOR;
			$wpg2_g2path['g2_filepath'] = str_replace($doubleslash,DIRECTORY_SEPARATOR,$wpg2_g2path['g2_filepath']);
			$wpg2_g2path['g2_filepath'] = str_replace($doubleslash,DIRECTORY_SEPARATOR,$wpg2_g2path['g2_filepath']);
			$wpg2_g2path['g2_filepath'] = str_replace('/', DIRECTORY_SEPARATOR, $wpg2_g2path['g2_filepath']);
			$wpg2_g2path['wpg2_relativepath'] = str_replace($doubleslash,DIRECTORY_SEPARATOR,$wpg2_g2path['wpg2_relativepath']);
			$wpg2_g2path['wpg2_relativepath'] = str_replace($doubleslash,DIRECTORY_SEPARATOR,$wpg2_g2path['wpg2_relativepath']);

			update_option('wpg2_g2paths', $wpg2_g2path);
		}

	// Verify WPG2 Internal g2_embeduri setting & Revalidate
		wpg2_template_page();
		wpg2_validate('silent');

	}

	//Get Current Gallery2 Plug-in Options
	$wpg2_option = get_option('wpg2_options');
	$wpg2_g2path = get_option('wpg2_g2paths');

	// Validate Paths To Help Pin Point Errors.
	if (!$validate_err) {
		if (!$wpg2_g2path['g2_url']) {
			$valmessage = '<strong><font color="red">'.__('Validation Error: URL to your Gallery2 is Missing.','wpg2').'</strong></font>';
			$validate_err=1;
		}
	}

	if (!$validate_err) {
		if ($wpg2_g2path['g2_url']) {
			$ret = wpg2_validateuri($wpg2_g2path['g2_url'].'embed.php');
			if ($ret) {
				// Return the Error
				$valmessage = '<strong>'.__('Possible Validation Problem with Gallery2 URL:').$ret.'</strong>';
				$validate_warning=1;
			}
		}
	}

	if (!$validate_err) {
		if (!file_exists($wpg2_g2path['g2_filepath'].$wpg2_g2path['wpg2_relativepath'].DIRECTORY_SEPARATOR.'templates'.DIRECTORY_SEPARATOR.'wpg2imageblock.tpl' ) ) {
			$valmessage =  '<strong><font color="red">'.__('Validation Error: WPG2 plugin cannot validate the Gallery2 to WPG2 Relative Path ','wpg2').$wpg2_g2path['g2_filepath'].$wpg2_g2path['wpg2_relativepath'].DIRECTORY_SEPARATOR.'templates'.DIRECTORY_SEPARATOR.'wpg2imageblock.tpl</font></strong>';
			$validate_err=1;
		}
	}

	?>
	<div class="wrap">
	<?php

	echo '<h2>' . __('WPG2 Paths', 'wpg2') . '<font size="2">&nbsp;<a href="http://codex.gallery2.org/Integration:WPG2_Options_-_G2_Paths" rel="external">'. __('(Help)', 'wpg2').'</a></font></h2>';
	echo  $valmessage;
	echo '<form name="g2autopaths" method="post" action="">';
	echo '<input type="hidden" name="action" value="update" />';

	if ($validate_err && !$wpg2_g2path['g2_url']) {
// Turn On the Auto Configuration Option
		echo '<h3>'. __('WPG2 Automatic Configuration from Gallery2 URL', 'wpg2').'</h3>';
		echo '<table cellspacing="2" cellpadding="5" class="form-table"><tr><td></td><td>';
		echo '<tr><th valign="top" scope="row">'. __("Gallery2 URL:", "wpg2").'</th><td>';
		echo '<input name="g2url" type="text" id="g2url" value="" size="70" /><br />';
		echo __("Enter your Gallery2 URL by navigating to your Gallery2 Home Page and copy/paste the URL from the browser window into this field.", "wpg2");
		echo '<br /></td></tr></table>';
		?>
			<input type="hidden" id="g2_validated" name="wpg2_options[g2_validated]" value="No">
			<p class="submit">
			<input type="submit" name="Submit" value="<?php _e('Validate', 'wpg2') ?> &raquo;" />
			</p>
			</div><div class="wrap">
	<?php }

	?>
	<table cellspacing="2" cellpadding="5" class="form-table">
	<tr>
	<th valign="top" scope="row"><?php _e('Gallery2 URL:', 'wpg2') ?> </th>
	<td>
	<input name="g2_manualpath[g2_url]" type="text" id="g2_url" value="<?php echo $wpg2_g2path['g2_url']; ?>" size="70" /><br />
	<?php _e('URL to your Gallery2 Installation.', 'wpg2') ?><br /><?php _e('Example: http://www.domain.com/gallery2/', 'wpg2') ?><br />
	</td>
	</tr>
	<tr>
	<th valign="top" scope="row"><?php _e('URL to WPG2 Output:', 'wpg2') ?> </th>
	<td>
	<input name="g2_manualpath[g2_embeduri]" type="text" id="g2_embed" value="<?php echo $wpg2_g2path['g2_embeduri']; ?>" size="70" disabled /><br />
	</td>
	</tr>
	<tr>
	<th width="25%" valign="top" scope="row"><?php _e('Gallery2 File Path:', 'wpg2') ?> </th>
	<td>
	<input name="g2_manualpath[g2_filepath]" type="text" id="wpg2_g2path" value="<?php echo $wpg2_g2path['g2_filepath']; ?>" size="70" /><br />
	<?php _e('This is the File path to your Gallery2 installation.<br />Example: /home/username/public_html/gallery2/', 'wpg2') ?><br />
	</td>
	</tr>
	<tr>
	<th width="25%" valign="top" scope="row"><?php _e('Gallery2 to WPG2 Relative Path:', 'wpg2') ?> </th>
	<td>
	<input name="g2_manualpath[wpg2_relativepath]" type="text" id="wpg2_wpg2relativepath" value="<?php echo $wpg2_g2path['wpg2_relativepath']; ?>" size="70" /><br />
	<?php _e('Example: ../wordpress/wp-content/plugins/wpg2', 'wpg2') ?><br />
	</td>
	</tr>
	<tr>
	<th valign="top" scope="row"><?php _e('On Gallery Error Redirect to:', 'wpg2') ?> </th>
	<td>
	<input name="g2_manualpath[g2_errorredirect]" type="text" id="g2_errorredirect" value="<?php echo $wpg2_g2path['g2_errorredirect']; ?>" size="70" /><br />
	<?php _e('The page you want to redirect to if there is a Gallery login error.', 'wpg2') ?><br /><?php _e('Example: http://www.domain.com/index.php', 'wpg2') ?><br />
	</td>
	</tr>
	</table>
	<input type="hidden" id="g2_validated" name="wpg2_options[g2_validated]" value="No">
	<?php _e('<strong><br />Caution: Take care when editing any of these values, as incorrect values could return php errors or broken images.</strong>', 'wpg2'); ?>
	<p class="submit">
				<input type="submit" name="Submit" value="<?php _e('Save Changes', 'wpg2') ?>" />
	</p>
	</form>
	</div></div>
	<?php
}

/**
* Maintenance Options for Sidebar Grid Block
*
* @param NULL
* @return NULL
*/
function wpg2_optionssidebargridblock() {

	//If we're setting options,
	if ( isset($_POST['action']) ) {
		$wpg2_option = get_option('wpg2_options');
		// Option Saving Time
		foreach ($_POST['g2_options'] as $key=>$value){
			$wpg2_option[$key] = $value;
		}

		update_option('wpg2_options', $wpg2_option);

		?><div id="message" class="updated fade"><p><strong><?php _e('Sidebar Grid Block options saved.', 'wpg2') ?></strong></p></div><?php
	}

	// Get WPG2 Option Settings
	$wpg2_option = get_option('wpg2_options');

	if ( $wpg2_option['g2_validated'] == "Yes" ) {

		?>
		<div class="wrap">
		<h2><?php _e('Sidebar Grid Block Options', 'wpg2') ?></font><font size="2">&nbsp;<a href="http://codex.gallery2.org/Integration:WPG2_Options_-_Sidebar_Grid" rel="external"><?php _e('(Help)', 'wpg2') ?></a></font></h2>

		<form name="sidebargridoptions" method="post" action="">
			<input type="hidden" name="action" value="update" />
			<table cellspacing="2" cellpadding="5" class="form-table">
			<tr>
			<th valign="top" scope="row"><?php _e('Number of Grid Blocks to Display:', 'wpg2') ?> </th>
			<td>
			<select name="g2_options[g2_sidebargridblockstodisplay]">
				<option label="<?php _e('None', 'wpg2') ?>" value=""<?php if ('' == $wpg2_option['g2_sidebargridblockstodisplay']){echo " selected";}?>><?php _e('None', 'wpg2') ?></option>
				<option label="<?php _e('One', 'wpg2') ?>" value="1"<?php if ('1' == $wpg2_option['g2_sidebargridblockstodisplay']){echo " selected";}?>><?php _e('One', 'wpg2') ?></option>
				<option label="<?php _e('Two', 'wpg2') ?>" value="2"<?php if ('2' == $wpg2_option['g2_sidebargridblockstodisplay']){echo " selected";}?>><?php _e('Two', 'wpg2') ?></option>
				<option label="<?php _e('Three', 'wpg2') ?>" value="3"<?php if ('3' == $wpg2_option['g2_sidebargridblockstodisplay']){echo " selected";}?>><?php _e('Three', 'wpg2') ?></option>
				<option label="<?php _e('Four', 'wpg2') ?>" value="4"<?php if ('4' == $wpg2_option['g2_sidebargridblockstodisplay']){echo " selected";}?>><?php _e('Four', 'wpg2') ?></option>
				<option label="<?php _e('Five', 'wpg2') ?>" value="5"<?php if ('5' == $wpg2_option['g2_sidebargridblockstodisplay']){echo " selected";}?>><?php _e('Five', 'wpg2') ?></option>
				<option label="<?php _e('Six', 'wpg2') ?>" value="6"<?php if ('6' == $wpg2_option['g2_sidebargridblockstodisplay']){echo " selected";}?>><?php _e('Six', 'wpg2') ?></option>
				<option label="<?php _e('Seven', 'wpg2') ?>" value="7"<?php if ('7' == $wpg2_option['g2_sidebargridblockstodisplay']){echo " selected";}?>><?php _e('Seven', 'wpg2') ?></option>
				<option label="<?php _e('Eight', 'wpg2') ?>" value="8"<?php if ('8' == $wpg2_option['g2_sidebargridblockstodisplay']){echo " selected";}?>><?php _e('Eight', 'wpg2') ?></option>
				<option label="<?php _e('Nine', 'wpg2') ?>" value="9"<?php if ('9' == $wpg2_option['g2_sidebargridblockstodisplay']){echo " selected";}?>><?php _e('Nine', 'wpg2') ?></option>
			</select></td></tr>
			<tr>
			<th valign="top" scope="row"><?php _e('Type of Grid Block:', 'wpg2') ?> </th>
			<td>
			<select name="g2_options[g2_sidebargridblockstype]">
				<option label="<?php _e('None', 'wpg2') ?>" value=""<?php if ('' == $wpg2_option['g2_sidebargridblockstype']){echo " selected";}?>><?php _e('None', 'wpg2') ?></option>
				<option label="<?php _e('Random Image', 'wpg2') ?>" value="randomImage"<?php if ('randomImage' == $wpg2_option['g2_sidebargridblockstype']){echo " selected";}?>><?php _e('Random Image', 'wpg2') ?></option>
				<option label="<?php _e('Recent Image', 'wpg2') ?>" value="recentImage"<?php if ('recentImage' == $wpg2_option['g2_sidebargridblockstype']){echo " selected";}?>><?php _e('Recent Image', 'wpg2') ?></option>
				<option label="<?php _e('Random Album', 'wpg2') ?>" value="randomAlbum"<?php if ('randomAlbum' == $wpg2_option['g2_sidebargridblockstype']){echo " selected";}?>><?php _e('Random Album', 'wpg2') ?></option>
				<option label="<?php _e('Recent Album', 'wpg2') ?>" value="recentAlbum"<?php if ('recentAlbum' == $wpg2_option['g2_sidebargridblockstype']){echo " selected";}?>><?php _e('Recent Album', 'wpg2') ?></option>
			</select>
			</td></tr>
			<tr>
			<th valign="top" scope="row"><?php _e('Thumbnail Size:', 'wpg2') ?> </th>
			<td>
			<input name="g2_options[g2_sidebargridblocksimgsize]" id="g2_sidebargridblocksimgsize" value="<?php echo $wpg2_option['g2_sidebargridblocksimgsize']; ?>" size="5" / ><br />
			</td></tr>
			<tr>
			<th valign="top" scope="row"><?php _e('Additional Options:', 'wpg2') ?> </th>
			<td>
			<input type="hidden" name="g2_options[g2_sidebargridblockinfo][0]" value="">
			<input name="g2_options[g2_sidebargridblockinfo][]" type="checkbox" id="g2_sidebargridblockinfo" value="heading" <?php if (in_array('heading',$wpg2_option['g2_sidebargridblockinfo'])){echo "checked ";}?>/> <?php _e('Heading', 'wpg2') ?><br />
			</td>
			</tr>
			</table>

			<p class="submit">
				<input type="submit" name="Submit" value="<?php _e('Save Changes', 'wpg2') ?>" />
			</p>
		</form>
		</div></div>
		<?php
		} else {
			?><div id="message" class="error"><p><strong><?php _e('Options not available until WPG2 Plugin is Validated.', 'wpg2') ?></strong></p></div>
			<?php
		}

}

/**
* Maintenance Options for Sidebar Block
*
* @param NULL
* @return NULL
*/
function wpg2_optionssidebarblock() {

	//If we're setting options,
	if ( isset($_POST['action']) ) {
		$wpg2_option = get_option('wpg2_options');
		// Option Saving Time
		foreach ($_POST['g2_options'] as $key=>$value){
			$wpg2_option[$key] = $value;
		}

		update_option('wpg2_options', $wpg2_option);

		?><div id="message" class="updated fade"><p><strong><?php _e('Sidebar Block options saved.', 'wpg2') ?></strong></p></div><?php

	}

	// Get WPG2 Option Settings
		$wpg2_option = get_option('wpg2_options');

	if ( $wpg2_option['g2_validated'] == "Yes" ) {

		// Get Current List of Gallery2 Frames
		$g2imageframes = g2_get_imageframes();

		?>
		<div class="wrap">
		<h2><?php _e('Sidebar Image Options', 'wpg2') ?></font><font size="2">&nbsp;<a href="http://codex.gallery2.org/Integration:WPG2_Options_-_Sidebar_Block" rel="external"><?php _e('(Help)', 'wpg2') ?></a></font></h2>
		<form name="sidebaroptions" method="post" action="">
			<input type="hidden" name="action" value="update" />
			<table cellspacing="2" cellpadding="5" class="form-table"><tr>
			<th valign="top" scope="row"><?php _e('Display Block(s):', 'wpg2') ?> </th>
			<td colspan=2>
				<input type="hidden" name="g2_options[g2_sidebarblock][0]" value="">
				<input name="g2_options[g2_sidebarblock][]" type="checkbox" id="g2_sidebarblock1" value="randomImage" <?php if (in_array('randomImage',$wpg2_option['g2_sidebarblock'])){echo "checked ";}?>/> <?php _e('Random Image', 'wpg2') ?><br />
				<input name="g2_options[g2_sidebarblock][]" type="checkbox" id="g2_sidebarblock2" value="recentImage" <?php if (in_array('recentImage',$wpg2_option['g2_sidebarblock'])){echo "checked ";}?>/> <?php _e('Recent Image', 'wpg2') ?><br />
				<input name="g2_options[g2_sidebarblock][]" type="checkbox" id="g2_sidebarblock3" value="viewedImage" <?php if (in_array('viewedImage',$wpg2_option['g2_sidebarblock'])){echo "checked ";}?>/> <?php _e('Viewed Image', 'wpg2') ?><br />
				<input name="g2_options[g2_sidebarblock][]" type="checkbox" id="g2_sidebarblock4" value="randomAlbum" <?php if (in_array('randomAlbum',$wpg2_option['g2_sidebarblock'])){echo "checked ";}?>/> <?php _e('Random Album', 'wpg2') ?><br />
				<input name="g2_options[g2_sidebarblock][]" type="checkbox" id="g2_sidebarblock5" value="recentAlbum" <?php if (in_array('recentAlbum',$wpg2_option['g2_sidebarblock'])){echo "checked ";}?>/> <?php _e('Recent Album', 'wpg2') ?><br />
				<input name="g2_options[g2_sidebarblock][]" type="checkbox" id="g2_sidebarblock6" value="viewedAlbum" <?php if (in_array('viewedAlbum',$wpg2_option['g2_sidebarblock'])){echo "checked ";}?>/> <?php _e('Viewed Album', 'wpg2') ?>
				</td><td><input name="g2_options[g2_sidebarblock][]" type="checkbox" id="g2_sidebarblock7" value="dailyImage" <?php if (in_array('dailyImage',$wpg2_option['g2_sidebarblock'])){echo "checked ";}?>/> <?php _e('Daily Image', 'wpg2') ?><br />
				<input name="g2_options[g2_sidebarblock][]" type="checkbox" id="g2_sidebarblock8" value="weeklyImage" <?php if (in_array('weeklyImage',$wpg2_option['g2_sidebarblock'])){echo "checked ";}?>/> <?php _e('Weekly Image', 'wpg2') ?><br />
				<input name="g2_options[g2_sidebarblock][]" type="checkbox" id="g2_sidebarblock9" value="monthlyImage" <?php if (in_array('monthlyImage',$wpg2_option['g2_sidebarblock'])){echo "checked ";}?>/> <?php _e('Monthly Image', 'wpg2') ?><br />
				<input name="g2_options[g2_sidebarblock][]" type="checkbox" id="g2_sidebarblock10" value="dailyAlbum" <?php if (in_array('dailyAlbum',$wpg2_option['g2_sidebarblock'])){echo "checked ";}?>/> <?php _e('Daily Album', 'wpg2') ?><br />
				<input name="g2_options[g2_sidebarblock][]" type="checkbox" id="g2_sidebarblock11" value="weeklyAlbum" <?php if (in_array('weeklyAlbum',$wpg2_option['g2_sidebarblock'])){echo "checked ";}?>/> <?php _e('Weekly Album', 'wpg2') ?><br />
				<input name="g2_options[g2_sidebarblock][]" type="checkbox" id="g2_sidebarblock12" value="monthlyAlbum" <?php if (in_array('monthlyAlbum',$wpg2_option['g2_sidebarblock'])){echo "checked ";}?>/> <?php _e('Monthly Album', 'wpg2') ?>
			</td></table>
			<table cellspacing="2" cellpadding="5" class="form-table"><tr>
			<th valign="top" scope="row"><?php _e('Additional Options:', 'wpg2') ?> </th>
			<td colspan=2>
			<input type="hidden" name="g2_options[g2_sidebarblockshow][0]" value="">
			<input name="g2_options[g2_sidebarblockshow][1]" type="checkbox" id="g2_sidebarblockshow1" value="title" <?php if (in_array('title',$wpg2_option['g2_sidebarblockshow'])){echo "checked ";}?>/> <?php _e('Title', 'wpg2') ?><br />
			<input name="g2_options[g2_sidebarblockshow][2]" type="checkbox" id="g2_sidebarblockshow2" value="date" <?php if (in_array('date',$wpg2_option['g2_sidebarblockshow'])){echo "checked ";}?>/> <?php _e('Date', 'wpg2') ?><br />
			<input name="g2_options[g2_sidebarblockshow][3]" type="checkbox" id="g2_sidebarblockshow3" value="views" <?php if (in_array('views',$wpg2_option['g2_sidebarblockshow'])){echo "checked ";}?>/> <?php _e('Views', 'wpg2') ?><br />
			</td><td>
			<input name="g2_options[g2_sidebarblockshow][4]" type="checkbox" id="g2_sidebarblockshow4" value="owner" <?php if (in_array('owner',$wpg2_option['g2_sidebarblockshow'])){echo "checked ";}?>/> <?php _e('Owner', 'wpg2') ?><br />
			<input name="g2_options[g2_sidebarblockshow][5]" type="checkbox" id="g2_sidebarblockshow5" value="heading" <?php if (in_array('heading',$wpg2_option['g2_sidebarblockshow'])){echo "checked ";}?>/> <?php _e('Heading', 'wpg2') ?><br />
			<input name="g2_options[g2_sidebarblockshow][6]" type="checkbox" id="g2_sidebarblockshow6" value="fullSize" <?php if (in_array('fullSize',$wpg2_option['g2_sidebarblockshow'])){echo "checked ";}?>/> <?php _e('FullSize', 'wpg2') ?><br />
			</td></table>
			<table cellspacing="2" cellpadding="5" class="form-table">			
			<tr>
			<th valign="top" scope="row"><?php _e('Image Size:', 'wpg2') ?> </th>
			<td>
			<input name="g2_options[g2_sidebarblockimgsize]" id="g2_sidebarblockimgsize" value="<?php echo $wpg2_option['g2_sidebarblockimgsize']; ?>" size="10" / ><br />
			</td>
			</tr>
			<tr>
			<th valign="top" scope="row"><?php _e('Item Frame:', 'wpg2') ?> </th>
			<td>
			<select name="g2_options[g2_sidebarblockimageframe]">
			<?php
				foreach ($g2imageframes as $key=>$g2frame){
					if ($wpg2_option['g2_sidebarblockimageframe'] == $key)
						echo '<option label="'.$g2frame.'" value="'.$key.'" selected >'.$g2frame.'</option>';
					else
						echo '<option label="'.$g2frame.'" value="'.$key.'">'.$g2frame.'</option>';
				}
			?>
			</select>
			</td>
			</tr>
			<tr>
			<th valign="top" scope="row"><?php _e('Album Frame:', 'wpg2') ?> </th>
			<td>
			<select name="g2_options[g2_sidebarblockalbumframe]">
			<?php
				foreach ($g2imageframes as $key=>$g2frame){
					if ($wpg2_option['g2_sidebarblockalbumframe'] == $key)
						echo '<option label="'.$g2frame.'" value="'.$key.'" selected >'.$g2frame.'</option>';
					else
						echo '<option label="'.$g2frame.'" value="'.$key.'">'.$g2frame.'</option>';
				}
			?>
			</select>
			</td>
			</tr>
			</table>
			<p class="submit">
				<input type="submit" name="Submit" value="<?php _e('Save Changes', 'wpg2') ?>" />
			</p>
		</form>
		</div></div>
		<?php
		} else {
			?><div id="message" class="error"><p><strong><?php _e('Options not available until WPG2 Plugin is Validated.', 'wpg2') ?></strong></p></div>
			<?php
		}

}

/**
* Maintenance Options for WPG2 Tags
*
* @param NULL
* @return NULL
*/
function wpg2_tagoptions() {

	//If we're setting options,
	if ( isset($_POST['action']) ) {
		$wpg2_option = get_option('wpg2_options');
		// Option Saving Time
		foreach ($_POST['wpg2_options'] as $key=>$value){
			$wpg2_option[$key] = $value;
		}

		update_option('wpg2_options', $wpg2_option);

		?><div id="message" class="updated fade"><p><strong><?php _e('WPG2 Tag options saved.', 'wpg2') ?></strong></p></div><?php
	}

	// Get Gallery2 Option Settings
		$wpg2_option = get_option('wpg2_options');

	if ( $wpg2_option['g2_validated'] == "Yes" ) {

		// Get Current List of Gallery2 Frames
		$g2imageframes = g2_get_imageframes();

		?>
		<div class="wrap">
		<h2><?php _e('WPG2 Tag Options', 'wpg2') ?></font><font size="2">&nbsp;<a href="http://codex.gallery2.org/Integration:WPG2_Options_-_G2_Rewrites" rel="external"><?php _e('(Help)', 'wpg2') ?></a></font></h2>
		<form name="wpg2tagoptions" method="post" action="">
			<input type="hidden" name="action" value="update" />
			<table width="100%" cellspacing="2" cellpadding="5" class="form-table">
			<tr>
			<th valign="top" scope="row"><?php _e('Default Image Size:', 'wpg2') ?> </th>
			<td>
			<input name="wpg2_options[g2_tagimgsize]" id="g2_postimgsize" value="<?php echo $wpg2_option['g2_tagimgsize']; ?>" size="10" / >
			</td>
			</tr>
			<tr>
			<th valign="top" scope="row"><?php _e('Item Frame:', 'wpg2') ?> </th>
			<td>
			<select name="wpg2_options[g2_tagimageframe]">
			<?php
				foreach ($g2imageframes as $key=>$g2frame){
					if ($wpg2_option['g2_tagimageframe'] == $key)
						echo '<option label="'.$g2frame.'" value="'.$key.'" selected >'.$g2frame.'</option>';
					else
						echo '<option label="'.$g2frame.'" value="'.$key.'">'.$g2frame.'</option>';
				}
			?>
			 </select>
			</td>
			</tr>
			<tr>
			<th valign="top" scope="row"><?php _e('Album Frame:', 'wpg2') ?> </th>
			<td>
			<select name="wpg2_options[g2_tagalbumframe]">
			<?php
				foreach ($g2imageframes as $key=>$g2frame){
					if ($wpg2_option['g2_tagalbumframe'] == $key)
						echo '<option label="'.$g2frame.'" value="'.$key.'" selected >'.$g2frame.'</option>';
					else
						echo '<option label="'.$g2frame.'" value="'.$key.'">'.$g2frame.'</option>';
				}
			?>
			 </select>
			</td>
			</tr>
			<th valign="top" scope="row"><?php _e('Optional Details:', 'wpg2') ?> </th>
			<td>
			<input type="hidden" name="wpg2_options[g2_tagblockshow][0]" value="">
						<input name="wpg2_options[g2_tagblockshow][]" type="checkbox" id="g2_postblockshow1" value="title" <?php if (in_array('title',$wpg2_option['g2_tagblockshow'])){echo "checked ";}?>/> <?php _e('Title', 'wpg2') ?>
			</td>
			</tr>
			</table>
			<p class="submit">
				<input type="submit" name="Submit" value="<?php _e('Save Changes', 'wpg2') ?>" />
			</p>
		</form>
		</div></div>
		<?php
		} else {
			?><div id="message" class="error"><p><strong><?php _e('Options not available until WPG2 Plugin is Validated.', 'wpg2') ?></strong></p></div>
			<?php
		}

}

/**
* Maintenance Options for WPG2 Embedded Page
*
* @param NULL
* @return NULL
*/
function wpg2_optionswpg2outputpage() {

	global $wp_rewrite;

	//If we're setting options,
	if ( isset($_POST['action']) ) {
		$wpg2_option = get_option('wpg2_options');

		// Remove the Magic Quotes That Wordpress Adds, it screws up the ""
		$_POST = stripslashes_deep($_POST);

		// Option Saving Time
		foreach ($_POST['wpg2_options'] as $key=>$value){
			$wpg2_option[$key] = $value;
		}

		update_option('wpg2_options', $wpg2_option);

		?><div id="message" class="updated fade"><p><strong><?php _e('WPG2 Page, Style & CSS Options Saved.', 'wpg2') ?></strong></p></div><?php

	}


	// Get Gallery2 Option Settings
		$wpg2_option = get_option('wpg2_options');

	if ( $wpg2_option['g2_validated'] == "Yes" ) {
?>
		<div class="wrap">
		<h2><?php _e('WPG2 Page Style & CSS Options', 'wpg2') ?></font><font size="2">&nbsp;<a href="http://codex.gallery2.org/Integration:WPG2_Options_-_WPG2_Output" rel="external"><?php _e('(Help)', 'wpg2') ?></a></font></h2>
		<form name="g2options" method="post" action="">
			<input type="hidden" name="action" value="update" />
			<table cellspacing="2" cellpadding="5" class="form-table">
			<tr valign="top">
			<?php			
				if ($wpg2_option['g2_embedpagetype'] == 'gallery2' ) {
				?>
					<th scope="row"><?php _e('HTML &lt;HEAD&gt; Tag ', 'wpg2') ?> </th>
					<td>
					<textarea name="wpg2_options[g2_htmlheader]" cols="80" rows="4"><?php echo stripslashes($wpg2_option['g2_htmlheader']); ?></textarea><br / >	
					<?php _e('(HTML Elements to be dynamically added to create &lt;head&gt;)', 'wpg2') ?> 
					</td></tr>
					<tr valign="top">
					<th scope="row"><?php _e('HTML &lt;body&gt; Tag', 'wpg2') ?> </th>
					<td>
					<textarea name="wpg2_options[g2_htmlbody]" cols="80" rows="2"><?php echo stripslashes($wpg2_option['g2_htmlbody']); ?></textarea><br / >
					<?php _e('(HTML Elements to be dynamically added to create  &lt;body&gt;)', 'wpg2') ?>
					</td>
					</tr>
					<tr valign="top">
					<th scope="row"><?php _e('HTML &lt;/html&gt; Tag', 'wpg2') ?> </th>
					<td>
					<textarea name="wpg2_options[g2_htmlfooter]" cols="80" rows="2"><?php echo stripslashes($wpg2_option['g2_htmlfooter']); ?></textarea><br / >
					<?php _e('(HTML Elements to be dynamically added to create &lt;/html&gt;', 'wpg2') ?>
					</td>
					</tr>
			<?php			
				} else {
				?>
					<th scope="row"><?php _e('CSS Style Elements Header', 'wpg2') ?> </th>
					<td>
					<textarea name="wpg2_options[g2_cssheader]" cols="40" rows="2"><?php echo stripslashes($wpg2_option['g2_cssheader']); ?></textarea><br / >	
					<?php _e('(N/A when using Custom wpg2header)<br/>Stylesheet Elements to be dynamically added to the Wordpress Header prior to &lt;/head&gt;', 'wpg2') ?> 
					</td></tr>
					<tr valign="top">
					<th scope="row"><?php _e('Simple Style Elements Header', 'wpg2') ?> </th>
					<td>
					<textarea name="wpg2_options[g2_header]" cols="40" rows="2"><?php echo stripslashes($wpg2_option['g2_header']); ?></textarea><br / >
					<?php _e('(N/A when using Custom wpg2footer)<br/>Styling Elements to be dynamically added to the Wordpress Footer prior to &lt;/body&gt;', 'wpg2') ?>
					</td>
					</tr>
					<tr valign="top">
					<th scope="row"><?php _e('Simple Style Elements Footer', 'wpg2') ?> </th>
					<td>
					<textarea name="wpg2_options[g2_footer]" cols="40" rows="2"><?php echo stripslashes($wpg2_option['g2_footer']); ?></textarea><br / >
					<?php _e('(N/A when using Custom wpg2footer)<br/>Styling Elements to be dynamically added to the Wordpress Footer prior to &lt;/body&gt;', 'wpg2') ?>
					</td>
					</tr>
			<?php			
				} 
				?>
			</table>
			<p class="submit">
				<input type="submit" name="Submit" value="<?php _e('Save Changes', 'wpg2') ?>" />
			</p>
		</form>
		</div></div>
		<?php
		} else {
			?><div id="message" class="error"><p><strong><?php _e('Options not available until WPG2 Plugin is Validated.', 'wpg2') ?></strong></p></div>
			<?php
		}

}

/**
* Maintenance Options for Lightbox Settings
*
* @param NULL
* @return NULL
*/
function wpg2_optionslightbox() {

	//If we're setting options,
	if ( isset($_POST['action']) ) {
		$wpg2_option = get_option('wpg2_options');
		// Option Saving Time
		foreach ($_POST['wpg2_options'] as $key=>$value){
			$wpg2_option[$key] = $value;
		}

		update_option('wpg2_options', $wpg2_option);

		?>		<div id="message" class="updated fade"><p><strong><?php _e('Lightbox options saved.', 'wpg2') ?></strong></p></div><?php
	}

	// Get Gallery2 Option Settings
		$wpg2_option = get_option('wpg2_options');

	if ( $wpg2_option['g2_validated'] == "Yes" ) {
		?>
		<div class="wrap">
		<h2><?php _e('Lightbox Options', 'wpg2') ?></font><font size="2">&nbsp;<a href="http://codex.gallery2.org/Integration:WPG2_Options_-_Lightbox" rel="external"><?php _e('(Help)', 'wpg2') ?></a></font></h2>
		<form name="g2lightboxoptions" method="post" action="">
			<input type="hidden" name="action" value="update" />
			<table cellspacing="2" cellpadding="5" class="form-table">
			<tr>
			<th valign="top" scope="row"><?php _e('Internal Lightbox Script?', 'wpg2') ?> </th>
			<td>
			<select name="wpg2_options[wpg2_effectscript]">
				<option label="<?php _e('None', 'wpg2') ?>" value="none"<?php if ($wpg2_option['wpg2_effectscript'] == "none" ){echo " selected";}?>><?php _e('None', 'wpg2')?></option>
				<option label="<?php _e('Slimbox', 'wpg2') ?>" value="slimbox"<?php if ($wpg2_option['wpg2_effectscript'] == "slimbox" ){echo " selected";}?>><?php _e('Slimbox', 'wpg2')?></option>
				<option label="<?php _e('Lightbox', 'wpg2') ?>" value="lightbox"<?php if ($wpg2_option['wpg2_effectscript'] == "lightbox" ){echo " selected";}?>><?php _e('Lightbox', 'wpg2')?></option>
			</select></td></tr>
			<tr>
			<th valign="top" scope="row"><?php _e('Should WPG2 Tags have Lightbox Support?', 'wpg2') ?> </th>
			<td>
			<select name="wpg2_options[wpg2_enabletagslightbox]">
				<option label="<?php _e('Yes', 'wpg2') ?>" value="enable"<?php if ($wpg2_option['wpg2_enabletagslightbox'] == "enable" ){echo " selected";}?>><?php _e('Yes', 'wpg2')?></option>
				<option label="<?php _e('No', 'wpg2') ?>" value=""<?php if ($wpg2_option['wpg2_enabletagslightbox'] == "" ){echo " selected";}?>><?php _e('No', 'wpg2')?></option>
			</select></td></tr>
			<tr>
			<th valign="top" scope="row"><?php _e('Should Sidebar Images have Lightbox Support?', 'wpg2') ?> </th>
			<td>
			<select name="wpg2_options[wpg2_enablesidebarlightbox]">
				<option label="<?php _e('Yes', 'wpg2') ?>" value="enable"<?php if ($wpg2_option['wpg2_enablesidebarlightbox'] == "enable" ){echo " selected";}?>><?php _e('Yes', 'wpg2')?></option>
				<option label="<?php _e('No', 'wpg2') ?>" value=""<?php if ($wpg2_option['wpg2_enablesidebarlightbox'] == "" ){echo " selected";}?>><?php _e('No', 'wpg2')?></option>
			</select></td></tr>
			<tr>
			<th valign="top" scope="row"><?php _e('Should Gallery2 have Lightbox Support?', 'wpg2') ?> </th>
			<td>
			<select name="wpg2_options[wpg2_enableg2lightbox]">
				<option label="<?php _e('Yes', 'wpg2') ?>" value="enable"<?php if ($wpg2_option['wpg2_enableg2lightbox'] == "enable" ){echo " selected";}?>><?php _e('Yes', 'wpg2')?></option>
				<option label="<?php _e('No', 'wpg2') ?>" value=""<?php if ($wpg2_option['wpg2_enableg2lightbox'] == "" ){echo " selected";}?>><?php _e('No', 'wpg2')?></option>
			</select><?php _e(' <-- Only Valid when using Gallery2 Lightbox Enabled Theme', 'wpg2')?></td></tr>
			<tr>
			<th valign="top" scope="row"><?php _e('Lightbox Image Minimum Height:', 'wpg2') ?> </th>
			<td>
			<input name="wpg2_options[g2_lightboximgsize]" id="g2_lightboximgsize" value="<?php echo $wpg2_option['g2_lightboximgsize']; ?>" size="3" / >
			</td></tr>
			</table>
			<p class="submit">
				<input type="submit" name="Submit" value="<?php _e('Save Changes', 'wpg2') ?>" />
			</p>
		</form>
		</div></div>
		<?php
		} else {
			?><div id="message" class="error"><p><strong><?php _e('Options not available until WPG2 Plugin is Validated.', 'wpg2') ?></strong></p></div>
			<?php
		}
}
?>