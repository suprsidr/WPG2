<?php
/*
	Author: WPG2 Team
	Updated: 116:25 23/03/2008

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
													WORDPRESS FUNCTIONS
********************************************************************************************************
*/

/**
* Adds g2image Options to WPG2 Options Sub Menu
*
* @param None
* @return None
*/
function g2imageoptions() {

	//If we're setting options,
	if ( isset($_POST['action']) ) {
		$g2_image = get_option('wpg2_g2ic');
		// Option Saving Time
		foreach ($_POST['g2_image'] as $key=>$value){
			$g2_image[$key] = $value;
		}

		update_option('wpg2_g2ic', $g2_image);

		?><div id="message" class="updated fade"><p><strong><?php _e('G2Image Page options saved.', 'wpg2') ?></strong></p></div><?php
	}

	// Get Gallery2 Option Settings
		$g2_image = get_option('wpg2_g2ic');
		$g2_option = get_option('wpg2_options');

	if ( $g2_option['g2_validated'] == "Yes" ) {
		?>
			<div class="wrap">
			<h2><?php _e('G2Image Popup Options', 'wpg2') ?></h2>
			<form name="g2imageoptions" method="post" action="">
			<input type="hidden" name="action" value="update" />
			<table cellspacing="2" cellpadding="5" class="form-table">
			<tr>
			<th valign="top" scope="row"><?php _e('Thumbnails per Page:', 'wpg2') ?> </th>
			<td>
			<input name="g2_image[g2ic_images_per_page]" id="g2ic_images_per_page" value="<?php if(isset($g2_image['g2ic_images_per_page'])){echo $g2_image['g2ic_images_per_page'];} else{echo "15";} ?>" size="10" / ><br />
			</td></tr>
			<tr>
			<th valign="top" scope="row"><?php _e('Default Image Sort Order:', 'wpg2') ?></th>
			<td>
			<select name="g2_image[g2ic_sortby]">
				<option value="title_asc"<?php if ($g2_image['g2ic_sortby']=="title_asc" ){ echo " selected";} ?>><?php _e('Gallery2 Title (A-z)', 'wpg2') ?></option>
				<option value="title_desc"<?php if ($g2_image['g2ic_sortby']=="title_desc" ){ echo " selected";} ?>><?php _e('Gallery2 Title (z-A)', 'wpg2') ?></option>
				<option value="orig_time_desc"<?php if ($g2_image['g2ic_sortby']=="orig_time_desc" ){ echo " selected";} ?>><?php _e('Origination Time (newest first)', 'wpg2') ?></option>
				<option value="orig_time_asc"<?php if ($g2_image['g2ic_sortby']=="orig_time_asc" ){ echo " selected";} ?>><?php _e('Origination Time (oldest first)', 'wpg2') ?></option>
				<option value="mtime_desc"<?php if ($g2_image['g2ic_sortby']=="mtime_desc" ){ echo " selected";} ?>><?php _e('Last Modification (newest first)', 'wpg2') ?></option>
				<option value="mtime_asc"<?php if ($g2_image['g2ic_sortby']=="mtime_asc" ){ echo " selected";} ?>><?php _e('Last Modification (oldest first)', 'wpg2') ?></option>
			</select> <br />
			</td>
			</tr>
			<tr>
			<th valign="top" scope="row"><?php _e('Default Display', 'wpg2') ?> </th>
			<td>
			<input name="g2_image[g2ic_display_filenames]" type="radio" id="g2ic_display_filenames_false" value="no" <?php if(!isset($g2_image['g2ic_display_filenames'])){echo "checked ";} elseif ($g2_image['g2ic_display_filenames']=="no"){echo "checked ";}?> /><?php _e('Thumbnails Only', 'wpg2')?>;&nbsp;&nbsp;
			<input name="g2_image[g2ic_display_filenames]" type="radio" id="g2ic_display_filenames_true" value="yes" <?php if ($g2_image['g2ic_display_filenames']=="yes"){echo "checked ";}?> /><?php _e('Thumbnails with Titles and Filenames', 'wpg2') ?>
			</td>
			</tr>
			<tr>
			<th valign="top" scope="row"><?php _e('Default Action:', 'wpg2') ?></th>
			<td>
			<?php _e('Choose the default "How to Insert" option.', 'wpg2') ?><br />
			<select name="g2_image[g2ic_default_action]">
				<option value="wpg2_image"<?php if ($g2_image['g2ic_default_action']=="wpg2_image" ){ echo " selected";} ?>><?php _e('WPG2 Tag', 'wpg2') ?></option>
				<option value="thumbnail_image"<?php if ($g2_image['g2ic_default_action']=="thumbnail_image" ){ echo " selected";} ?>><?php _e('Thumbnail with link to image', 'wpg2') ?></option>
				<option value="thumbnail_album"<?php if ($g2_image['g2ic_default_action']=="thumbnail_album" ){ echo " selected";} ?>><?php _e('Thumbnail with link to parent album', 'wpg2') ?></option>
				<option value="thumbnail_lightbox"<?php if ($g2_image['g2ic_default_action']=="thumbnail_lightbox" ){ echo " selected";} ?>><?php _e('Thumbnail with LightBox link to Fullsized Image', 'wpg2') ?></option>
				<option value="thumbnail_custom_url"<?php if ($g2_image['g2ic_default_action']=="thumbnail_custom_url" ){ echo " selected";} ?>><?php _e('Thumbnail with link to custom URL', 'wpg2') ?></option>
				<option value="thumbnail_only"<?php if ($g2_image['g2ic_default_action']=="thumbnail_only" ){ echo " selected";} ?>><?php _e('Thumbnail only - no link', 'wpg2') ?></option>
				<option value="fullsize_image"<?php if ($g2_image['g2ic_default_action']=="fullsize_image" ){ echo " selected";} ?>><?php _e('Fullsized image with link to Gallery page for image', 'wpg2') ?></option>
				<option value="fullsize_album"<?php if ($g2_image['g2ic_default_action']=="fullsize_album" ){ echo " selected";} ?>><?php _e('Fullsized image with link to parent album', 'wpg2') ?></option>
				<option value="fullsize_custom_url"<?php if ($g2_image['g2ic_default_action']=="fullsize_custom_url" ){ echo " selected";} ?>><?php _e('Fullsized image with link to custom URL', 'wpg2') ?></option>
				<option value="fullsize_only"<?php if ($g2_image['g2ic_default_action']=="fullsize_only" ){ echo " selected";} ?>><?php _e('Fullsized image only - no link', 'wpg2') ?></option>
				<option value="link_image"<?php if ($g2_image['g2ic_default_action']=="link_image" ){ echo " selected";} ?>><?php _e('Text link to image', 'wpg2') ?></option>
				<option value="link_album"<?php if ($g2_image['g2ic_default_action']=="link_album" ){ echo " selected";} ?>><?php _e('Text link to parent album', 'wpg2') ?></option>
			</select> <br />
			</td>
			</tr>
			<tr>
			<th valign="top" scope="row"><?php _e('Default Custom URL:', 'wpg2') ?> </th>
			<td>
			<input name="g2_image[g2ic_custom_url]" id="g2ic_custom_url" value="<?php if(isset($g2_image['g2ic_custom_url'])){echo $g2_image['g2ic_custom_url'];} else{echo "http://";} ?>" size="50" / ><br />
			</td>
			<tr>
			<th valign="top" scope="row"><?php _e('Default Image Class:', 'wpg2') ?></th>
			<td>
			<select name="g2_image[g2ic_default_alignment]">
				<option value="none"<?php if ($g2_image['g2ic_default_alignment']=="none" ){ echo " selected";} ?>><?php _e('None', 'wpg2') ?></option>
				<option value="g2image_normal"<?php if ($g2_image['g2ic_default_alignment']=="g2image_normal" ){ echo " selected";} ?>><?php _e('Normal', 'wpg2') ?></option>
				<option value="g2image_float_left"<?php if ($g2_image['g2ic_default_alignment']=="g2image_float_left" ){ echo " selected";} ?>><?php _e('Float Left', 'wpg2') ?></option>
				<option value="g2image_float_right"<?php if ($g2_image['g2ic_default_alignment']=="g2image_float_right" ){ echo " selected";} ?>><?php _e('Float Right', 'wpg2') ?></option>
				<option value="g2image_centered"<?php if ($g2_image['g2ic_default_alignment']=="g2image_centered" ){ echo " selected";} ?>><?php _e('Centered', 'wpg2') ?></option>
				<?php if(isset($g2_image['g2ic_custom_class_1'])&&($g2_image['g2ic_custom_class_1']!='not_used')){ ?>
				<option value="<?php echo $g2_image['g2ic_custom_class_1']; ?>"<?php if ($g2_image['g2ic_default_alignment']==$g2_image['g2ic_custom_class_1']){ echo " selected";} ?>><?php echo $g2_image['g2ic_custom_class_1']; ?></option>
				<?php } ?>
				<?php if(isset($g2_image['g2ic_custom_class_2'])&&($g2_image['g2ic_custom_class_2']!='not_used')){ ?>
				<option value="<?php echo $g2_image['g2ic_custom_class_2']; ?>"<?php if ($g2_image['g2ic_default_alignment']==$g2_image['g2ic_custom_class_2']){ echo " selected";} ?>><?php echo $g2_image['g2ic_custom_class_2']; ?></option>
				<?php } ?>
				<?php if(isset($g2_image['g2ic_custom_class_3'])&&($g2_image['g2ic_custom_class_3']!='not_used')){ ?>
				<option value="<?php echo $g2_image['g2ic_custom_class_3']; ?>"<?php if ($g2_image['g2ic_default_alignment']==$g2_image['g2ic_custom_class_3']){ echo " selected";} ?>><?php echo $g2_image['g2ic_custom_class_3']; ?></option>
				<?php } ?>
				<?php if(isset($g2_image['g2ic_custom_class_4'])&&($g2_image['g2ic_custom_class_4']!='not_used')){ ?>
				<option value="<?php echo $g2_image['g2ic_custom_class_4']; ?>"<?php if ($g2_image['g2ic_default_alignment']==$g2_image['g2ic_custom_class_4']){ echo " selected";} ?>><?php echo $g2_image['g2ic_custom_class_4']; ?></option>
				<?php } ?>
			</select> <br />
			<?php _e('This is the default CSS class applied to WPG2 tags and HTML thumbnails inserted using the G2Image popup window.', 'wpg2') ?><br />
			<?php _e('Custom classes will be available as options after entering them below and hitting the "Update Options" button.', 'wpg2') ?><br />
			</td>
			</tr>
			<tr>
			<th valign="top" scope="row"><?php _e('Default Class Mode', 'wpg2') ?> </th>
			<td>
			<input name="g2_image[g2ic_class_mode]" type="radio" id="g2ic_class_mode_img" value="img" <?php if(!isset($g2_image['g2ic_class_mode'])){echo "checked ";} elseif ($g2_image['g2ic_class_mode']=="img"){echo "checked ";}?> /><?php _e('Class in the img tag - &lt img class=... /&gt (Recommended)', 'wpg2') ?><br />
			<input name="g2_image[g2ic_class_mode]" type="radio" id="g2ic_class_mode_div" value="div" <?php if ($g2_image['g2ic_class_mode']=="div"){echo "checked ";}?> /><?php _e('Class in a div tag wrapper - &lt div class=...&gt&lt img ... /&gt&lt /div&gt', 'wpg2') ?>
			<?php _e('<br />NOTE: This setting only applies to images inserted as &lt;img&gt; tags, as WPG2 tag will always be wrapped with a div tag.', 'wpg2') ?><br />
			</td>
			</tr>
			<tr>
			<th valign="top" scope="row"><?php _e('Custom Classes:', 'wpg2') ?> </th>
			<td>
			<?php _e('Custom Classes (a valid class in your CSS or "not_used")', 'wpg2') ?>
			<table><tr><td colspan=2>
			<?php _e('1. ', 'wpg2') ?><input name="g2_image[g2ic_custom_class_1]" id="g2ic_custom_class_1" value="<?php if(isset($g2_image['g2ic_custom_class_1'])){echo $g2_image['g2ic_custom_class_1'];} else{echo "not_used";} ?>" size="30" / ><br />
			</td><td>
			<?php _e('3. ', 'wpg2') ?><input name="g2_image[g2ic_custom_class_3]" id="g2ic_custom_class_3" value="<?php if(isset($g2_image['g2ic_custom_class_3'])){echo $g2_image['g2ic_custom_class_3'];} else{echo "not_used";} ?>" size="30" / ><br />
			</td></tr>
			<tr><td colspan=2>
			<?php _e('2. ', 'wpg2') ?><input name="g2_image[g2ic_custom_class_2]" id="g2ic_custom_class_2" value="<?php if(isset($g2_image['g2ic_custom_class_2'])){echo $g2_image['g2ic_custom_class_2'];} else{echo "not_used";} ?>" size="30" / ><br />
			</td><td>
			<?php _e('4. ', 'wpg2') ?><input name="g2_image[g2ic_custom_class_4]" id="g2ic_custom_class_4" value="<?php if(isset($g2_image['g2ic_custom_class_4'])){echo $g2_image['g2ic_custom_class_4'];} else{echo "not_used";} ?>" size="30" / ><br />
			</td></tr></table></td></tr></table>
			<p class="submit">
				<input type="submit" name="Submit" value="<?php _e('Save Changes', 'wpg2') ?>" />
			</p>
		</form>
		</div>
		<?php
	} else {
		?><div id="message" class="error"><p><strong><?php _e('Options not available until WPG2 Plugin is Validated.', 'wpg2') ?></strong></p></div>
		<?php
	}

}

/**
* Sets g2image Defaults
*
* @param None
* @return None
*/
function g2image_setdefaults() {
	$g2_image['g2ic_images_per_page'] = 15;
	$g2_image['g2ic_display_filenames'] = "no";
	$g2_image['g2ic_default_alignment'] = 'none';
	$g2_image['g2ic_custom_class_1'] = 'not_used';
	$g2_image['g2ic_custom_class_2'] = 'not_used';
	$g2_image['g2ic_custom_class_3'] = 'not_used';
	$g2_image['g2ic_custom_class_4'] = 'not_used';
	$g2_image['g2ic_custom_url'] = 'http://';
	$g2_image['g2ic_class_mode'] = 'img';
	$g2_image['g2ic_click_mode'] = 'one_click_insert';
	$g2_image['g2ic_click_mode_variable'] = "yes";
	$g2_image['g2ic_wpg2id_tags'] = 'yes';
	$g2_image['g2ic_default_action'] = 'wpg2_image';
	$g2_image['g2ic_sortby'] = 'title_asc';

	// Get WPG2 Archive IF Exists and repopulate
	$g2ic_archive = get_option('wpg2_g2ic_30_archive');

	if (!empty($g2ic_archive)) {
		foreach ($g2ic_archive as $key=>$value){
			$g2_image[$key] = $value;
		}
//		delete_option('wpg2_g2ic_30_archive');
	}

	update_option('wpg2_g2ic', $g2_image);

}

/**
* Stores G2IC Defaults on Deactivation
*
* @param None
* @return None
*/
function g2image_deactivation() {

	// Get Gallery2 IC Option Settings
	$g2_image = get_option('wpg2_g2ic');

	$g2ic_archive['g2ic_images_per_page'] = $g2_image['g2ic_images_per_page'];
	$g2ic_archive['g2ic_display_filenames'] = $g2_image['g2ic_display_filenames'];
	$g2ic_archive['g2ic_default_alignment'] = $g2_image['g2ic_default_alignment'];
	$g2ic_archive['g2ic_custom_class_1'] = $g2_image['g2ic_custom_class_1'];
	$g2ic_archive['g2ic_custom_class_2'] = $g2_image['g2ic_custom_class_2'];
	$g2ic_archive['g2ic_custom_class_3'] = $g2_image['g2ic_custom_class_3'];
	$g2ic_archive['g2ic_custom_class_4'] = $g2_image['g2ic_custom_class_4'];
	$g2ic_archive['g2ic_custom_url'] = $g2_image['g2ic_custom_url'];
	$g2ic_archive['g2ic_class_mode'] = $g2_image['g2ic_class_mode'];
	$g2ic_archive['g2ic_click_mode'] = $g2_image['g2ic_click_mode'];
	$g2ic_archive['g2ic_click_mode_variable'] = $g2_image['g2ic_click_mode_variable'];
	$g2ic_archive['g2ic_wpg2id_tags'] = $g2_image['g2ic_wpg2id_tags'];
	$g2ic_archive['g2ic_default_action'] = $g2_image['g2ic_default_action'];
	$g2ic_archive['g2ic_sortby'] = $g2_image['g2ic_sortby'];

	// Save the WPG2 Options into WPG2 Archive
	update_option('wpg2_g2ic_30_archive', $g2ic_archive);

	// Delete GIC Option
	delete_option('wpg2_g2ic');


}
/*
********************************************************************************************************
																TINYMCE
********************************************************************************************************
*/

/**
* Adds g2image to the TinyMCE button bar
*
* @param string $buttons the buttons string from the WP filter
* @return string the appended buttons string
*/

function g2image_wp_extended_editor_mce_buttons($buttons) {
	array_push($buttons, 'separator', 'g2image');
	return $buttons;
}

/**
* Adds g2image to the TinyMCE button Toolbar 
*
* @param null
* @return null
*/
function g2image_addbuttons() {
   // Don't bother doing this stuff if the current user lacks permissions
   if ( ! current_user_can('edit_posts') && ! current_user_can('edit_pages') )
     return;
 
   // Add only in Rich Editor mode
   if ( get_user_option('rich_editing') == 'true') {
     add_filter("mce_external_plugins", "g2image_plugin");
     add_filter('mce_buttons', 'g2image_wp_extended_editor_mce_buttons');
   }
}

/**
* Adds g2image to the TinyMCE plugins list
*
* @param string $plugins the buttons string from the WP filter
* @return string the appended plugins string
*/
function g2image_plugin($plugin_array) {
   $plugin_array['g2image'] = get_bloginfo('wpurl').'/wp-content/plugins/wpg2/g2image/editor_plugin.js';
   return $plugin_array;
}

/**
* Adds Additional WPG2 Specific Elements to the TinyMCE valid elements list
*
* @param string $valid_elements the valid elements string from the WP filter
* @return string the appended valid elements string
*/

function g2image_wp_extended_editor_mce_valid_elements($valid_elements) {
	$valid_elements .= 'wpg2,wpg2id,div[class]';
	return $valid_elements;
}

/**
* Adds WPG2 Button to Wordpress Selected (Typically Write) Screens
*
* Javascript appended to the bottom of the "Write Post" or "Write Page" admin pages for the WPG2 quicktag.
*/

function g2image_callback() {

   $g2image_url = get_bloginfo('wpurl').'/wp-content/plugins/wpg2/g2image/';

 // Only add the javascript to post.php, post-new.php, page.php, page-new.php, or bookmarklet.php pages
  if (strpos($_SERVER['REQUEST_URI'], 'post.php') ||
      strpos($_SERVER['REQUEST_URI'], 'post-new.php') ||
      strpos($_SERVER['REQUEST_URI'], 'page.php') ||
      strpos($_SERVER['REQUEST_URI'], 'page-new.php') ||
      strpos($_SERVER['REQUEST_URI'], 'bookmarklet.php')) {

?>

<script language="JavaScript" type="text/javascript"><!--

	var g2_toolbar = document.getElementById("ed_toolbar");
<?php
g2image_edit_insert_button(__('WPG2', 'wpg2'), 'g2_open', __('Gallery2 Image Chooser', 'g2image'));
?>
	function g2_open() {
		var form = 'post';
		var field = 'content';
		var url = '<?php echo $g2image_url; ?>g2image.php?g2ic_form='+form+'&g2ic_field='+field+'&g2ic_tinymce=0';
		var name = 'g2image';
		var w = 800;
		var h = 600;
		var valLeft = (screen.width) ? (screen.width-w)/2 : 0;
		var valTop = (screen.height) ? (screen.height-h)/2 : 0;
		var features = 'width='+w+',height='+h+',left='+valLeft+',top='+valTop+',resizable=1,scrollbars=1';
		var g2imageWindow = window.open(url, name, features);
		g2imageWindow.focus();
	}

//--></script>

<?php
	}
}

if(!function_exists('g2image_edit_insert_button')) {

	//edit_insert_button: Inserts a button into the editor
	function g2image_edit_insert_button($caption, $js_onclick, $title = '') {
	?>

	if(g2_toolbar){
		var theButton = document.createElement('input');
		theButton.type = 'button';
		theButton.value = '<?php echo $caption; ?>';
		theButton.onclick = <?php echo $js_onclick; ?>;
		theButton.className = 'ed_button';
		theButton.title = "<?php echo $title; ?>";
		theButton.id = "<?php echo "ed_{$caption}"; ?>";
		g2_toolbar.appendChild(theButton);
	}

	<?php
	}
}

?>