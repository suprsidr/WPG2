<?php
/*
	Author: WPG2 Team
	Updated: 20:47 27/04/2008

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
*/



function wpg2_widget_sidebarblock_display($args) {
	extract($args);
	$options = get_option('wpg2_widget_block');
	?>
		<?php echo $before_widget; ?>
			<?php echo $before_title . $options['title'] . $after_title; ?>
			<div>
				<?php print g2_sidebarimageblock($options['id']); ?>
			</div>
		<?php echo $after_widget; ?>
	<?php
}


/**
* WPG2 Sidebar Block Widget Output Display
*
* @param $title
* @return None
*/

function wpg2_widget_sidebargridblock_display($args) {
	extract($args);
	$options = get_option('wpg2_widget_gridblock');
	?>
		<?php echo $before_widget; ?>
			<?php echo $before_title . $options['title'] . $after_title; ?>
			<div>
				<?php
					if ($options['title'])
						print g2_sidebargridblock($options['blocktype'], $options['imgno'], $options['imgsize'], 'None');
					else
						print g2_sidebargridblock($options['blocktype'], $options['imgno']);
				?>
			</div>
		<?php echo $after_widget; ?>
	<?php
}

/**
* WPG2 Albumtree Widget Output Display
*
* @param $title
* @return None
*/

function wpg2_widget_albumtree_display($args) {
	extract($args);
	$options = get_option('wpg2_widget_albumtree');
	?>
		<?php echo $before_widget; ?>
			<?php echo $before_title . $options['title'] . $after_title; ?>
			<div>
				<?php
					print wpg2_make_html_album_tree();
				?>
			</div>
		<?php echo $after_widget; ?>
	<?php
}

/**
* Register WPG2 Sidebar Block Output Display
*
* @param $title
* @return None
*/

function wpg2_widget_sidebarblock_control() {
	$options = $newoptions = get_option('wpg2_widget_block');
	if ( $_POST["wpg2_widget_block_submit"] ) {
		$newoptions['title'] = strip_tags(stripslashes($_POST["wpg2_widget_block_title"]));
	}
	if ( $options != $newoptions ) {
		$options = $newoptions;
		update_option('wpg2_widget_block', $options);
	}
	$title = attribute_escape($options['title']);
	?>
		<strong><?php _e('Please Note: Gallery2 items to be displayed in this widget are configured in the WPG2 Sidebar Block Options Menu', 'wpg2')?></strong>
		<p style="text-align:left;margin-left:0px;"><label for="wpg2_widget_block_title"><?php _e('Title:'); ?> <input style="width: 408px;" id="blocktitle" name="wpg2_widget_block_title" type="text" value="<?php echo $title; ?>" /></label></p>
		<input type="hidden" id="wpg2_widget_block_submit" name="wpg2_widget_block_submit" value="1" />
	<?php
}

/**
* Register WPG2 Sidebar Block Widget Control Functions
*
* @param $title
* @return None
*/


function wpg2_widget_sidebargridblock_control() {
	$options = $newoptions = get_option('wpg2_widget_gridblock');
	$wpg2_option = get_option('wpg2_options');
	if ( $_POST["wpg2_widget_grid_submit"] ) {
		$newoptions['title'] = strip_tags(stripslashes($_POST["wpg2_widget_gridblock_title"]));
	}
	if ( $options != $newoptions ) {
		$options = $newoptions;
		update_option('wpg2_widget_gridblock', $options);
	}
	$title = attribute_escape($options['title']);
	?>
		<strong><?php _e('Please Note: Gallery2 items to be displayed in this widget are configured in the WPG2 Sidebar Grid Block Options Menu', 'wpg2')?></strong>
		<p style="text-align:left;margin-left:0px;"><label for="wpg2_widget_gridblock_title"><?php _e('Title:'); ?> <input style="width: 408px;" id="blocktitle" name="wpg2_widget_gridblock_title" type="text" value="<?php echo $title ?>" /></label></p>
		<input type="hidden" id="wpg2_widget_grid_submit" name="wpg2_widget_grid_submit" value="1" />
	<?php
}

/**
* Register WPG2 Album Tree Widget Control Functions
*
* @param $title
* @return None
*/

function wpg2_widget_albumtree_control() {
	$options = $newoptions = get_option('wpg2_widget_albumtree');
	$wpg2_option = get_option('wpg2_options');
	if ( $_POST["wpg2_widget_albumtree_submit"] ) {
		$newoptions['title'] = strip_tags(stripslashes($_POST["wpg2_widget_albumtree_title"]));
	}
	if ( $options != $newoptions ) {
		$options = $newoptions;
		update_option('wpg2_widget_albumtree', $options);
	}
	$title = attribute_escape($options['title']);
	?>
		<p style="text-align:left;margin-left:0px;"><label for="wpg2_widget_albumtree_title"><?php _e('Title:'); ?> <input style="width: 408px;" id="blocktitle" name="wpg2_widget_albumtree_title" type="text" value="<?php echo $title ?>" /></label></p>
		<input type="hidden" id="wpg2_widget_albumtree_submit" name="wpg2_widget_albumtree_submit" value="1" />
	<?php
}

/**
* Register WPG2 Album Tree (with SimpleViewer) Widget Control Functions
*
* @param $title
* @return None
*/

function wpg2_widget_albumtree_simpleviewer_control() {
	$options = $newoptions = get_option('wpg2_widget_albumtree_simpleviewer');
	$wpg2_option = get_option('wpg2_options');
	if ( $_POST["wpg2_widget_albumtree_simpleviewer_submit"] ) {
		$newoptions['title'] = strip_tags(stripslashes($_POST["wpg2_widget_albumtree_simpleviewer_title"]));
	}
	if ( $options != $newoptions ) {
		$options = $newoptions;
		update_option('wpg2_widget_albumtree_simpleviewer', $options);
	}
	$title = attribute_escape($options['title']);
	?>
		<p style="text-align:left;margin-left:0px;"><label for="wpg2_widget_albumtree_simpleviewer_title"><?php _e('Title:'); ?> <input style="width: 408px;" id="blocktitle" name="wpg2_widget_albumtree_simpleviewer_title" type="text" value="<?php echo $title ?>" /></label></p>
		<input type="hidden" id="wpg2_widget_albumtree_simpleviewer_submit" name="wpg2_widget_albumtree_simpleviewer_submit" value="1" />
	<?php
}


/**
* Initialises the Wordpress Sidebar Widget Engine
*
* @param None
* @return None
*/

function wpg2_sidebar_widgets_init() {
	// Check for the required plugin functions.
	if ( !function_exists('register_sidebar_widget') )
		return;
		
	register_sidebar_widget('WPG2 Sbar Block', 'wpg2_widget_sidebarblock_display','','WPG2');
	register_widget_control('WPG2 Sbar Block', 'wpg2_widget_sidebarblock_control', 450, 90, 'WPG2');
	register_sidebar_widget('WPG2 Sbar Grid', 'wpg2_widget_sidebargridblock_display','','WPG2');
	register_widget_control('WPG2 Sbar Grid', 'wpg2_widget_sidebargridblock_control', 450, 90, 'WPG2');
	register_sidebar_widget('WPG2 Album Tree', 'wpg2_widget_albumtree_display','','WPG2');
	register_widget_control('WPG2 Album Tree', 'wpg2_widget_albumtree_control', 450, 90, 'WPG2');
	register_sidebar_widget('WPG2 Album Tree (with SimpleViewer)', 'wpg2_widget_albumtree_simpleviewer_display','','WPG2');
	register_widget_control('WPG2 Album Tree (with SimpleViewer)', 'wpg2_widget_albumtree_simpleviewer_control', 450, 90, 'WPG2');

}

?>