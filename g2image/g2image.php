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

// ====( Version Info )
$g2ic_version_text = '3.0.2';
$g2ic_version_array = array(3,0,2);

// ====( Initialization Code )
require_once('init.php');
session_start();
g2ic_get_request_and_session_options();
list($g2ic_album_info, $g2ic_gallery_items) = g2ic_get_gallery_items();
$g2ic_imginsert_options = g2ic_get_imginsert_selectoptions();

// ====( Main HTML Generation Code )
echo g2ic_make_html_header();

echo '        <table>' . "\n";
echo '            <tr>' . "\n";
echo '                <td width="200px" valign="top">' . "\n";

echo g2ic_make_html_album_tree($g2ic_options['root_album']);
echo '                </td>' . "\n";
echo '                <td valign="top">' . "\n";

echo '                    <div class="main">' . "\n";

if ($g2ic_options['wpg2_valid']) echo g2ic_make_html_wpg2_album_insert_button();
if ($g2ic_options['drupal_g2_filter']) echo g2ic_make_html_drupal_album_insert_button();

if (empty($g2ic_gallery_items)) {
	echo g2ic_make_html_empty_page();
}
else {
	$g2ic_page_navigation = g2ic_make_html_page_navigation();
	echo g2ic_make_html_display_options();
	echo g2ic_make_html_controls();
	print_r($g2ic_page_navigation);
	echo g2ic_make_html_image_navigation();
	print_r($g2ic_page_navigation);
}

echo g2ic_make_html_about($g2ic_version_text);

echo '                    </div>' . "\n";
echo '                </td>' . "\n";
echo '            </tr>' . "\n";
echo '        </table>' . "\n";
echo '    </form>' . "\n";
echo '</body>' . "\n\n";
echo '</html>';

$_SESSION['g2ic_last_album_visited'] = $g2ic_options['current_album'];

GalleryEmbed::done();

// ====( Functions - Alphabetical by Function Name)

/**
 * Get all of the Gallery2 items
 *
 * @return array $album_info Album Title and URL for the current album
 * @return array $gallery_items Sorted array of IDs and Titles for all Gallery2 Data Items in the current album
 */
function g2ic_get_gallery_items() {
	GLOBAL $gallery, $g2ic_options;

	$gallery_items = array();
	$item_mod_times = array();
	$item_orig_times = array();
	$item_create_times = array();
	$item_titles = array();
	$item_ids = array();
	$album_info = array();

	$urlGenerator =& $gallery->getUrlGenerator();

	list ($error,$albums) = GalleryCoreApi::loadEntitiesById(array($g2ic_options['current_album']));
	if(!$error) {
		foreach ($albums as $album) {
			$album_info['url'] = $urlGenerator->generateUrl(array('view' => 'core.ShowItem', 'itemId' => $album->getid()), array('forceFullUrl' => true));
			$album_info['title'] = $album->getTitle();
			list($error, $data_item_ids) = GalleryCoreApi::fetchChildDataItemIds($album);
			foreach ($data_item_ids as $data_item_id) {
				$item_ids[] = $data_item_id;
				list($error, $items) = GalleryCoreApi::loadEntitiesById(array($data_item_id));
				foreach ($items as $item) {
					$item_titles[] = $item->getTitle();
					$item_mod_times[] = $item->getModificationTimestamp( );
					$item_orig_times[] = $item->getOriginationTimestamp( );
					$item_create_times[] = $item->getOriginationTimestamp( );
				}
			}
		}
	}
	else {
		print T_('Error loading album entity');
	}

	// Sort directories and files
	$count_files = count($item_ids);

	if($count_files>0){
		switch ($g2ic_options['sortby']) {
			case 'title_asc' :
				array_multisort($item_titles, $item_ids);
				break;
			case 'title_desc' :
				array_multisort($item_titles, SORT_DESC, $item_ids);
				break;
			case 'orig_time_asc' :
				array_multisort($item_orig_times, $item_titles, $item_ids);
				break;
			case 'orig_time_desc' :
				array_multisort($item_orig_times, SORT_DESC, $item_titles, $item_ids);
				break;
			case 'mtime_asc' :
				array_multisort($item_mod_times, $item_titles, $item_ids);
				break;
			case 'mtime_desc' :
				array_multisort($item_mod_times, SORT_DESC, $item_titles, $item_ids);
		}
		for($i=0; $i<$count_files; $i++) {
			$gallery_items[$i] = array('title'=>$item_titles[$i],'id'=>$item_ids[$i]);
		}
	}

	return array($album_info, $gallery_items);

}

/**
 * Make the array of selection options for the "How to Insert?" select element
 *
 * @return array $imginsert_selectoptions The array of selection options for the "How to Insert?" select element
 */
function g2ic_get_imginsert_selectoptions(){
	GLOBAL $g2ic_options;

	$message = array();
	$imginsert_selectoptions = array();

	// These are the universal image insertion options
	$message['thumbnail_image'] = T_('Thumbnail with link to image');
	$message['thumbnail_album'] = T_('Thumbnail with link to parent album');
	$message['thumbnail_lightbox'] = T_('Thumbnail with LightBox link to Fullsized Image');
	$message['thumbnail_custom_url'] = T_('Thumbnail with link to custom URL (from text box below)');
	$message['thumbnail_only'] = T_('Thumbnail only - no link');
	$message['fullsize_image'] = T_('Fullsized image with link to Gallery page for image');
	$message['fullsize_only'] = T_('Fullsized image only - no link');
	$message['link_image'] = T_('Text link to image');
	$message['link_album'] = T_('Text link to parent album');

	// If using the WPG2 or Drupal Gallery Module append '(HTML)' to the end of the universal messages
	if ($g2ic_options['wpg2_valid'] || $g2ic_options['drupal_g2_filter']) {
		foreach($message as $key => $text) {
			$message[$key] = $text . ' (' . T_('HTML') . ')';
		}
	}

	// These are CMS-specific image insertion options
	$message['wpg2_image'] = T_('WPG2 tag of image');
	$message['drupal_g2_filter'] = T_('Drupal Gallery2 Module filter tag');

	// Make the universal message array
	$imginsert_selectoptions = array(
		'thumbnail_image' => array(
			'text'  => $message['thumbnail_image'] ),
		'thumbnail_album' => array(
			'text'  => $message['thumbnail_album'] ),
		'thumbnail_lightbox' => array(
			'text'  => $message['thumbnail_lightbox'] ),
		'thumbnail_custom_url' => array(
			'text'  => $message['thumbnail_custom_url'] ),
		'thumbnail_only' => array(
			'text'  => $message['thumbnail_only'] ),
		'fullsize_image' => array(
			'text'  => $message['fullsize_image'] ),
		'fullsize_only' => array(
			'text'  => $message['fullsize_only'] ),
		'link_image' => array(
			'text'  => $message['link_image'] ),
		'link_album'  => array(
			'text'  => $message['link_album'] ),
	);
	// If using the WPG2 or Drupal Gallery Module append their array to the beginning of the universal array
	if ($g2ic_options['wpg2_valid']) {
		$imginsert_selectoptions = array(
			'wpg2_image' => array(
				'text'  => $message['wpg2_image'] ),
		) + $imginsert_selectoptions;
	}
	if($g2ic_options['drupal_g2_filter'])	{
		$imginsert_selectoptions = array(
			'drupal_g2_filter' => array(
				'text' => $message['drupal_g2_filter'] ),
		) + $imginsert_selectoptions;
	}

	$imginsert_selectoptions[$g2ic_options['default_action']]['selected'] = TRUE;
	return $imginsert_selectoptions;
}

/**
 * Get info about an item from Gallery2
 *
 * @param int $item_id The Gallery2 ID of the item
 * @return array $item_info The array of information about the item
 */
function g2ic_get_item_info($item_id) {
	global $gallery;

	$urlGenerator =& $gallery->getUrlGenerator();

	list ($error,$items) = GalleryCoreApi::loadEntitiesById(array($item_id));
	if(!$error) {
		foreach ($items as $item) {
			$item_info['id'] = $item_id;
			$item_info['title'] = $item->getTitle();
			$item_info['description'] = $item->getDescription();
			$item_info['summary'] = $item->getSummary();
			$item_info['fullsize_img'] = $urlGenerator->generateUrl(array('view' => 'core.DownloadItem', 'itemId' => $item->getid()), array('forceFullUrl' => true));
			list($error, $thumbnails) = GalleryCoreApi::fetchThumbnailsByItemIds(array($item->getid()));
			if(!$error) {
				foreach($thumbnails as $thumbnail) {
					$item_info['thumbnail_src'] = $urlGenerator->generateUrl(array('view' => 'core.DownloadItem', 'itemId' => $thumbnail->getid()), array('forceFullUrl' => true));
					$item_info['image_url'] = $urlGenerator->generateUrl(array('view' => 'core.ShowItem', 'itemId' => $item->getid()), array('forceFullUrl' => true));
					$item_info['thumbnail_width'] = $thumbnail->getWidth();
					$item_info['thumbnail_height'] = $thumbnail->getHeight();
				}
			}
			else {
				print T_('Error loading thumbnails');
			}
		}
	}
	else {
		print T_('Error loading album items');
	}

	if(empty($item_info['summary']))
		$item_info['summary'] = $item_info['title'];
	if(empty($item_info['description']))
		$item_info['description'] = $item_info['summary'];

	return $item_info;
}

/**
 * Get all of the options set in $_REQUEST and/or $_SESSION
 */
function g2ic_get_request_and_session_options(){

	global $g2ic_options;

	// Get the root album

	// Check for G2 Core API >= 7.5.  getDefaultAlbumId only available at 7.5 or above
	if (GalleryUtilities::isCompatibleWithApi(array(7,5), GalleryCoreApi::getApiVersion())) {
		list($error, $g2ic_options['root_album']) = GalleryCoreApi::getDefaultAlbumId();
	}
	// Otherwise use a Gallery2 2.1 method to get the root album
	else {
		list($error, $g2ic_options['root_album']) = GalleryCoreApi::getPluginParameter('module', 'core', 'id.rootAlbum');
	}

	g2ic_magic_quotes_remove($_REQUEST);

	// Is this a TinyMCE window?
	if(isset($_REQUEST['g2ic_tinymce'])){
		$g2ic_options['tinymce'] = $_REQUEST['g2ic_tinymce'];
		$_SESSION['g2ic_tinymce'] = $_REQUEST['g2ic_tinymce'];
	}
	else if (isset($_SESSION['g2ic_tinymce']))
		$g2ic_options['tinymce'] = $_SESSION['g2ic_tinymce'];
	else $g2ic_options['tinymce'] = 0;

	// Get the form name (if set) for insertion (not TinyMCE or FCKEditor)
	if(isset($_REQUEST['g2ic_form'])){
		$g2ic_options['form'] = $_REQUEST['g2ic_form'];
		$_SESSION['g2ic_form'] = $_REQUEST['g2ic_form'];
	}
	else if (isset($_SESSION['g2ic_form']))
		$g2ic_options['form'] = $_SESSION['g2ic_form'];
	else $g2ic_options['form'] = '';

	// Get the field name (if set) for insertion (not TinyMCE or FCKEditor)
	if(isset($_REQUEST['g2ic_field'])){
		$g2ic_options['field'] = $_REQUEST['g2ic_field'];
		$_SESSION['g2ic_field'] = $_REQUEST['g2ic_field'];
	}
	else if (isset($_SESSION['g2ic_field']))
		$g2ic_options['field'] = $_SESSION['g2ic_field'];
	else $g2ic_options['field'] = '';

	// Get the last album visited
	if(isset($_SESSION['g2ic_last_album_visited'])) {
		$g2ic_options['last_album'] = $_SESSION['g2ic_last_album_visited'];
	}
	else {
		$g2ic_options['last_album'] = $g2ic_options['root_album'];
	}

	// Get the current album
	if(IsSet($_REQUEST['current_album'])){
		$g2ic_options['current_album'] = $_REQUEST['current_album'];
	}
	else {
		$g2ic_options['current_album'] = $g2ic_options['last_album'];
	}

	// Get the current page
	if (isset($_REQUEST['g2ic_page']) and is_numeric($_REQUEST['g2ic_page'])) {
		$g2ic_options['current_page'] = floor($_REQUEST['g2ic_page']);
	}
	else {
		$g2ic_options['current_page'] = 1;
	}

	// Get the current sort method
	if(IsSet($_REQUEST['sortby']))
		$g2ic_options['sortby'] = $_REQUEST['sortby'];

	// Determine whether to display the titles or keep them hidden
	if(IsSet($_REQUEST['display']))
		if ($_REQUEST['display'] == 'filenames')
			$g2ic_options['display_filenames'] = TRUE;

	// Determine how many images to display per page
	if(IsSet($_REQUEST['images_per_page']))
		$g2ic_options['images_per_page'] = $_REQUEST['images_per_page'];

	return;
}

/**
 * Remove "Magic Quotes"
 *
 * @param array &$array POST or GET with magic quotes
 */
function g2ic_magic_quotes_remove(&$array) {
	if(!get_magic_quotes_gpc())
		return;
	foreach($array as $key => $elem) {
		if(is_array($elem))
			g2ic_magic_quotes_remove($elem);
		else
			$array[$key] = stripslashes($elem);
	}
}

/**
 * Creates the "About" alert HTML
 *
 * @return string $html The "About" alert HTML
 */
function g2ic_make_html_about($version){
	global $g2ic_options, $g2ic_album_info;

	$html = '<div class="about_button">' . "\n"
	. '    <input type="button" onclick="alert(\'' . T_('Gallery2 Image Chooser') . '\n' . T_('Version') . ' ' . $version
	. '\n' . T_('Documentation:') .  ' http://g2image.steffensenfamily.com/\')" '
	. 'value="' . T_('About G2Image') . '"/>' . "\n"
	. '    <input type="hidden" name="current_album" value="' . $g2ic_options['current_album'] . '">' . "\n"
	. '    <input type="hidden" name="album_name" value="' . $g2ic_album_info['title'] . '" />' . "\n"
	. '    <input type="hidden" name="album_url" value="' . $g2ic_album_info['url'] . '" />' . "\n"
	. '    <input type="hidden" name="g2ic_page" value="' . $g2ic_options['current_page'] . '" />' . "\n"
	. '    <input type="hidden" name="class_mode" value="' . $g2ic_options['class_mode'] . '" />' . "\n"
	. '    <input type="hidden" name="g2ic_form" value="' . $g2ic_options['form'] . '" />' . "\n"
	. '    <input type="hidden" name="g2ic_field" value="' . $g2ic_options['field'] . '" />' . "\n"
	. '    <input type="hidden" name="g2ic_wpg2_valid" value="' . $g2ic_options['wpg2_valid'] . '" />' . "\n"
	. '    <input type="hidden" name="drupal_g2_filter" value="' . $g2ic_options['drupal_g2_filter'] . '" />' . "\n"
	. '    <input type="hidden" name="drupal_filter_prefix" value="' . $g2ic_options['drupal_g2_filter_prefix'] . '" />' . "\n"
	. '</div>' . "\n";

	return $html;
}

/**
 * Creates the album tree HTML
 *
 * @return string $html The album tree HTML
 */
function g2ic_make_html_album_tree($root_album){

	// Album navigation

	$html = '<div class="dtree">' . "\n"
	. '    <p><a href="javascript: d.openAll();">' . T_('Expand all') . '</a> | <a href="javascript: d.closeAll();">' . T_('Collapse all') . '</a></p>' . "\n"
	. '    <script type="text/javascript">' . "\n"
	. '        <!--' . "\n"
	. '        d = new dTree("d");' . "\n";
	$parent = -1;
	$node = 0;
	$html .= g2ic_make_html_album_tree_branches($root_album, $parent, $node);
	$html .= '        document.write(d);' . "\n"
	. '        //-->' . "\n"
	. '    </script>' . "\n"
	. '</div>' . "\n\n";

	return $html;
}

/**
 * Generates album hierarchy as d.add entites of dtree
 *
 * @param int $current_album id of current album
 * @param int $parent node of the parent album
 */
function g2ic_make_html_album_tree_branches($current_album, $parent, &$node) {
	global $g2ic_options;

	list ($error,$items) = GalleryCoreApi::loadEntitiesById(array($current_album));
	if(!$error){
		foreach ($items as $item) {
			$album_title = $item->getTitle();
			if(empty($album_title)) {
				$album_title = $item->getPathComponent();
			}
		}
		$html = '        d.add(' . $node . ',' . $parent . ',"' . $album_title . '","'
		. '?current_album=' . $current_album . '&sortby=' . $g2ic_options['sortby']
		. '&images_per_page=' . $g2ic_options['images_per_page'] . '");' . "\n";
	}

	list($error, $sub_albums) = GalleryCoreApi::fetchAlbumTree($current_album,1);

	$albums = array_keys($sub_albums);

	if (count($albums) > 0) {
		$parent = $node;
		foreach ($albums as $album) {
			$node++;
			$html .= g2ic_make_html_album_tree_branches($album, $parent, $node);
		}
	}

	return $html;
}

/**
 * Creates the alignment selection HTML
 *
 * @return string $html The alignment selection HTML
 */
function g2ic_make_html_alignment_select(){
	GLOBAL $g2ic_options;

	// array for output
	$align_options = array('none' => array('text' => T_('None')),
		'g2image_normal' => array('text' => T_('Normal')),
		'g2image_float_left' => array('text' => T_('Float Left')),
		'g2image_float_right' => array('text' => T_('Float Right')),
		'g2image_centered' => array('text' => T_('Centered')));

	if ($g2ic_options['custom_class_1'] != 'not_used'){
		$align_options = array_merge($align_options, array($g2ic_options['custom_class_1'] => array('text' => $g2ic_options['custom_class_1'])));
	}

	if ($g2ic_options['custom_class_2'] != 'not_used'){
		$align_options = array_merge($align_options, array($g2ic_options['custom_class_2'] => array('text' => $g2ic_options['custom_class_2'])));
	}

	if ($g2ic_options['custom_class_3'] != 'not_used'){
		$align_options = array_merge($align_options, array($g2ic_options['custom_class_3'] => array('text' => $g2ic_options['custom_class_3'])));
	}

	if ($g2ic_options['custom_class_4'] != 'not_used'){
		$align_options = array_merge($align_options, array($g2ic_options['custom_class_4'] => array('text' => $g2ic_options['custom_class_4'])));
	}

	$align_options[$g2ic_options['default_alignment']]['selected'] = TRUE;

	$html = g2ic_make_html_select('alignment',$align_options);

	return $html;
}

/**
 * Create the HTML for the image controls
 *
 * @return string $html The HTML for the image controls
 */
function g2ic_make_html_controls(){
	global $gallery, $g2ic_imginsert_options, $g2ic_options;

	// "How to insert:" radio buttons
	$html = "        <fieldset>\n"
	. '            <legend>' . T_('Insertion Options') . '</legend>' . "\n"
	. '            <label for="alignment">' . T_('How to Insert Image') . '</label>' . "\n"
	. g2ic_make_html_select('imginsert', $g2ic_imginsert_options, 'toggleTextboxes();')
	. '            <br />' . "\n"
	. '            <br />' . "\n"

	// "Custom URL" textbox
	. '            <div name="custom_url_textbox"';
	if ($g2ic_options['default_action'] == 'thumbnail_custom_url') {
		$html .= ' class="displayed_textbox"';
	}
	else {
		$html .= 'class="hidden_textbox"';
	}
	$html .= '>' . "\n"
	. '            <label for="custom_url">' . T_('Custom URL') . '<br /></label>' . "\n"
	. '            <input type="text" name="custom_url" size="84" maxlength="150" value="' . $g2ic_options['custom_url'] . '" />' . "\n"
	. '            <br />' . "\n"
	. '            <br />' . "\n"
	. '            </div>' . "\n"

	// "Link Text" textbox
	. '            <div name="link_text_textbox"';
	if (($g2ic_options['default_action'] == 'link_image') || ($g2ic_options['default_action'] == 'link_album')){
		$html .= ' class="displayed_textbox"';
	}
	else {
		$html .= 'class="hidden_textbox"';
	}
	$html .= '>' . "\n"
	. '            <label for="link_text">' . T_('Text for text link') . '<br /></label>' . "\n"
	. '            <input type="text" name="link_text" size="84" maxlength="150" value="" />' . "\n"
	. '            <br />' . "\n"
	. '            <br />' . "\n"
	. '            </div>' . "\n"

	// "LightBox Group" textbox
	. '            <div name="lightbox_group_textbox"';
	if ($g2ic_options['default_action'] == 'thumbnail_lightbox'){
		$html .= ' class="displayed_textbox"';
	}
	else {
		$html .= 'class="hidden_textbox"';
	}
	$html .= '>' . "\n"
	. '            <label for="lightbox_group">' . T_('LightBox Group (Leave blank to not group with other images)') . '<br /></label>' . "\n"
	. '            <input type="text" name="lightbox_group" size="84" maxlength="150" value="g2image" />' . "\n"
	. '            <br />' . "\n"
	. '            <br />' . "\n"
	. '            </div>' . "\n";

	// WPG2 tag "size" box.

	if ($g2ic_options['wpg2_valid']) {
		GalleryCoreApi::requireOnce('modules/imageblock/module.inc');
		GalleryCoreApi::requireOnce('modules/core/classes/GalleryRepositoryUtilities.class');
		$plugin = new ImageBlockModule;
		$version = $plugin->getVersion();
		$version_comparison = GalleryRepositoryUtilities::compareRevisions($version,'1.0.9');
		if ($version_comparison != 'older') {
			$html .= '            <div name="wpg2_tag_size_textbox"';
			if ($g2ic_options['default_action'] == 'wpg2_image'){
				$html .= ' class="displayed_textbox"';
			}
			else {
				$html .= 'class="hidden_textbox"';
			}
			$html .= '>' . "\n"
			. '            <label for="wpg2_tag_size">' . T_('WPG2 tag "size" attribute (Leave blank for the default size of: ') . $g2ic_options['wpg2_tag_size']. 'px)' . '<br /></label>' . "\n"
			. '            <input type="text" name="wpg2_tag_size" size="84" maxlength="150" value="" />' . "\n"
			. '            <br />' . "\n"
			. '            <br />' . "\n"
			. '            </div>' . "\n";
		}
		else {
			$html .= '            <input type="hidden" name="wpg2_tag_size" value="" />' . "\n";
		}
	}

	// Drupal G2 Filter "exactsize" attribute box.

	if ($g2ic_options['drupal_g2_filter']) {
		GalleryCoreApi::requireOnce('modules/imageblock/module.inc');
		GalleryCoreApi::requireOnce('modules/core/classes/GalleryRepositoryUtilities.class');
		$plugin = new ImageBlockModule;
		$version = $plugin->getVersion();
		$version_comparison = GalleryRepositoryUtilities::compareRevisions($version,'1.0.9');
		if ($version_comparison != 'older') {
			$html .= '            <div name="drupal_exactsize_textbox"';
			if ($g2ic_options['default_action'] == 'drupal_g2_filter'){
				$html .= ' class="displayed_textbox"';
			}
			else {
				$html .= 'class="hidden_textbox"';
			}
			$html .= '>' . "\n"
			. '            <label for="drupal_exactsize">' . T_('Drupal G2 Filter "exactsize" attribute (Leave blank for no exactsize attribute)') . '<br /></label>' . "\n"
			. '            <input type="text" name="drupal_exactsize" size="84" maxlength="150" value="" />' . "\n"
			. '            <br />' . "\n"
			. '            <br />' . "\n"
			. '            </div>' . "\n";
		}
		else {
			$html .= '            <input type="hidden" name="drupal_exactsize" value="" />' . "\n";
		}
	}

	// Alignment selection
	$html .= '            <label for="alignment">' . T_('G2Image Alignment Class') . '</label>' . "\n"
	. g2ic_make_html_alignment_select()
	. "        </fieldset>\n\n";

	// "Insert" button
	$html .=  "        <fieldset>\n"
	. '            <legend>' . T_('Press button to insert checked image(s)') . '</legend>' . "\n"
	. "            <input disabled type='button'\n"
	. "            name='insert_button'\n"
	. '            onclick="insertItems();"' . "\n"
	. '            value="' . T_('Insert') . '"' . "\n"
	. '            />' . "\n"
	. '            <a href="javascript: checkAllImages();">' . T_('Check all') . '</a> | <a href="javascript: uncheckAllImages();">' . T_('Uncheck all') . '</a>' . "\n"
	. "        </fieldset>\n\n";

	return $html;
}

/**
 * Creates the HTML for the "Display Options" box
 *
 * @return string $html The HTML for the "Display Options" box
 */
function g2ic_make_html_display_options(){
	global $g2ic_options;

	$images_per_page_options = array(10,20,30,40,50,60,9999);

	if (!in_array($g2ic_options['images_per_page'],$images_per_page_options)){
		array_push($images_per_page_options,$g2ic_options['images_per_page']);
		sort($images_per_page_options);
	}

	// array for output
	$sortoptions = array('title_asc' => array('text' => T_('Gallery2 Title (A-z)')),
		'title_desc' => array('text' => T_('Gallery2 Title (z-A)')),
		'orig_time_desc' => array('text' => T_('Origination Time (Newest First)')),
		'orig_time_asc' => array('text' => T_('Origination Time (Oldest First)')),
		'mtime_desc' => array('text' => T_('Last Modification (Newest First)')),
		'mtime_asc' => array('text' => T_('Last Modification (Oldest First)')));

	$sortoptions[$g2ic_options['sortby']]['selected'] = TRUE;

	$html = "<div>\n"
	. "    <fieldset>\n"
	. '        <legend>' . T_('Display Options') . '</legend>' . "\n"
	. '            ' . T_('Sorted by:') . "\n"
	. g2ic_make_html_select('sortby',$sortoptions,'document.forms[0].submit();')
	. '            ' . T_('Per Page:') . "\n"
	. '            <select name="images_per_page" onchange="document.forms[0].submit();">' . "\n";

	for($i=0;$i<count($images_per_page_options);$i++){
		$html .= '                <option value="' . $images_per_page_options[$i] . '"';
		if ($images_per_page_options[$i] == $g2ic_options['images_per_page'])
			$html .= " selected='selected'";
		$html .= '>';
		if ($images_per_page_options[$i] == 9999)
			$html .= T_('All');
		else
			$html .= $images_per_page_options[$i];
		$html .= "</option>\n";
	}

	$html .=	"            </select>\n"
	. '            <br />' . "\n";

	$html .= '            <input type="radio" name="display" value="thumbnails"';
	if (!$g2ic_options['display_filenames'])
		$html .= ' checked="checked"' . "\n";
	else
		$html .= "\n";
	$html .= "            onclick='showThumbnails()'"
	.  '>' . T_('Thumbnails') . '</input>' . "\n";

	$html .= '            <input type="radio" name="display" value="filenames"';
	if ($g2ic_options['display_filenames'])
		$html .= ' checked="checked"' . "\n";
	else
		$html .= "\n";
	$html .= "            onclick='showFileNames()'"
	.  '>' . T_('Thumbnails with info') . '</input>' . "\n";

	$html .= "    </fieldset>\n"
	. "</div>\n\n";

	return $html;
}

/**
 * Creates the HTML for inserting an album Drupal Filter Tag
 *
 * @return string $html The HTML for for inserting an album Drupal Filter Tag
 */
function g2ic_make_html_drupal_album_insert_button(){

	GLOBAL $g2ic_options, $g2ic_album_info, $g2ic_gallery_items;
	$html = '';

	// Create the form
	$html .= "<div>\n"
	. "    <fieldset>\n"
	. '        <legend>' . T_('Insert a Drupal G2 Filter tag for the current album:') . ' ' . $g2ic_album_info['title'] . '</legend>' . "\n";

	if (empty($g2ic_gallery_items)) {
		$html .= '            ' . T_('G2Image Alignment Class') . ' ' . "\n"
		. g2ic_make_html_alignment_select()
		. '            <br />' . "\n";
	}

	// "Insert" button
	$html .= "            <input type='button'\n"
	. "            onclick='insertDrupalFilter()'\n"
	. '            value="' . T_('Insert') . '"' . "\n"
	. "            />\n";

	if (!empty($g2ic_gallery_items)) {
		$html .= '            ' . T_('Set the Alignment Class in "Insertion Options" below') . ' ' . "\n";
	}

	$html .= '            <input type="hidden" name="drupal_image_id" value="' . $g2ic_options['current_album'] . '" />' . "\n"
	. "    </fieldset>\n"
	. "</div>\n\n";

	return $html;
}

/**
 * Make the HTML for the "No photos in this album" message
 *
 * @return string $html The HTML for the "No photos in this album" message
 */
function g2ic_make_html_empty_page() {

	$html = '<p><strong>' . T_('There are no photos in this album.<br /><br />Please pick another album from the navigation options above.') . '</strong></p>' . "\n\n";

	return $html;
}

/**
 * Make the header
 *
 * @return string $html The HTML for the header
 */
function g2ic_make_html_header(){
	global $g2ic_options;
	$html = '<html xmlns="http://www.w3.org/1999/xhtml">' . "\n"
	. '<head>' . "\n"
	. '    <title>' . T_('Gallery2 Image Chooser') . '</title>' . "\n"
	. '    <link rel="stylesheet" href="css/g2image.css" type="text/css" />' . "\n"
	. '    <link rel="stylesheet" href="css/dtree.css" type="text/css" />' . "\n"
	. '    <link rel="stylesheet" href="css/lightbox.css" type="text/css" />' . "\n"
	. '    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />' . "\n";
	if($g2ic_options['tinymce'] && $g2ic_options['wpg2_valid']) {
		$html .= "    <script language='javascript' type='text/javascript' src='../../../../wp-includes/js/tinymce/tiny_mce_popup.js'></script>\n";
	}
	elseif($g2ic_options['tinymce'] && !$g2ic_options['wpg2_valid']) {
		$html .= "    <script language='javascript' type='text/javascript' src='../../tiny_mce_popup.js'></script>\n";
	}
	$html .= '    <script language="javascript" type="text/javascript" src="jscripts/functions.js"></script>' . "\n"
	. '    <script language="javascript" type="text/javascript" src="jscripts/dtree.js"></script>' . "\n"
	. '    <script language="javascript" type="text/javascript" src="jscripts/prototype.js"></script>' . "\n"
	. '    <script language="javascript" type="text/javascript" src="jscripts/scriptaculous.js?load=effects"></script>' . "\n"
	. '    <script language="javascript" type="text/javascript" src="jscripts/lightbox.js"></script>' . "\n"
	. '</head>' . "\n\n"
	. '<body id="g2image">' . "\n\n"
	. '    <form method="post">' . "\n";

	return $html;
}

/**
 * Make the HTML for the image block
 *
 * @return string $html The HTML for the image block
 */
function g2ic_make_html_image_navigation(){
	global $g2ic_gallery_items, $g2ic_options;

	reset($g2ic_gallery_items);

	$html = '';

	foreach($g2ic_gallery_items as $key => $item) {

		$image_id = $item['id'];

		if (!(($g2ic_options['current_page']-1)*$g2ic_options['images_per_page'] <= $key)) // Haven't gotten there yet
			continue;
		else if (!($key < $g2ic_options['current_page']*$g2ic_options['images_per_page']))
			break; // Have gone past the range for this page

		if ($g2ic_options['display_filenames']){
			$html .=  "<div class='title_imageblock'>\n";
		}
		else {
			$html .=  "<div class='thumbnail_imageblock'>\n";
		}

		$item_info = g2ic_get_item_info($image_id);

		$html .= g2ic_make_html_img($item_info) . "\n";

		if ($g2ic_options['display_filenames'])
			$html .= '    <div class="displayed_title">' . "\n";
		else
			$html .= '    <div class="hidden_title">' . "\n";

		$html .= '        ' . T_('Title:') . ' ' . htmlspecialchars($item_info['title']) . '<br />' . "\n"
		. '        ' . T_('Summary:') . ' ' . htmlspecialchars($item_info['summary']) . '<br />' . "\n"
		. '        ' . T_('Description:') . ' ' . htmlspecialchars($item_info['description']) . '<br />' . "\n";

		$html .=  "    </div>\n\n";

		if ($g2ic_options['display_filenames']){
			$html .=  "    <div class='active_placeholder'>\n"
			. "    </div>\n\n";
		}
		else {
			$html .=  "    <div class='inactive_placeholder'>\n"
			. "    </div>\n\n";
		}

		// hidden fields
		$html .= '    <input type="hidden" name="thumbnail_src" value="' . $item_info['thumbnail_src'] . '" />' . "\n"
		. '    <input type="hidden" name="fullsize_img" value="' . $item_info['fullsize_img'] . '" />' . "\n"
		. '    <input type="hidden" name="item_title" value="' . $item_info['title'] . '" />' . "\n"
		. '    <input type="hidden" name="item_summary" value="' . $item_info['summary'] . '" />' . "\n"
		. '    <input type="hidden" name="item_description" value="' . $item_info['description'] . '" />' . "\n"
		. '    <input type="hidden" name="image_url" value="' . $item_info['image_url'] . '" />' . "\n"
		. '    <input type="hidden" name="image_id" value="' . $image_id . '" />' . "\n"
		. '    <input type="hidden" name="thumbw" value="' . $item_info['thumbnail_width'] . '" />' . "\n"
		. '    <input type="hidden" name="thumbh" value="' . $item_info['thumbnail_height'] . '" />' . "\n"
		. '</div>' . "\n";
	}
	return $html;
}

/**
 * Make the HTML for an individual image
 *
 * @param array $item_info Information on the image
 * @return string $html The HTML for an individual image
 */
function g2ic_make_html_img($item_info) {
	global $g2ic_options;

	$html = '';

	// ---- image code
	$html .= '    <div style="background:#F0F0EE url(' . $item_info['thumbnail_src'] . '); width:'
	. $item_info['thumbnail_width'] . 'px; height:' . $item_info['thumbnail_height'] . 'px; float: left;">' . "\n"
	. '        <input type="checkbox" name="images" onclick="activateInsertButton();"/>' . "\n";

	$html .= '        <a title="' . $item_info['title'] .  '" rel="lightbox[g2image]" href="'
	. $item_info['fullsize_img'] . '">' . "\n"
	. '        <img src="images/magnifier.gif" border="0"></a>' . "\n"
	. '    </div>' . "\n";

	return $html;

}

/**
 * Make the HTML for navigating multiple pages of images
 *
 * @return string $html The HTML for navigating multiple pages of images
 */
function g2ic_make_html_page_navigation() {
	global $g2ic_gallery_items, $g2ic_options;

	// ---- navigation for pages of images
	$pages = ceil(count($g2ic_gallery_items)/$g2ic_options['images_per_page']);
	if ($g2ic_options['current_page'] > $pages)
		$g2ic_options['current_page'] = $pages;

	$pagelinks = array();
	for ($count = 1; $count <= $pages; $count++) {
		if ($g2ic_options['current_page'] == $count) {
			$pagelinks[] = '        <strong>' . $count . '</strong>';
		}
		else {
			$pagelinks[] = '        <a href="?g2ic_page=' . $count
			. '&sortby=' . $g2ic_options['sortby'] . '&current_album=' . $g2ic_options['current_album']
			. '&images_per_page='  . $g2ic_options['images_per_page'] . '">' . $count . '</a>';
		}
	}

	if (count($pagelinks) > 1) {
		$html = '<div>' . "\n"
		. '    <fieldset>' . "\n"
		. '        <legend>' . T_('Page Navigation:') . '</legend>' . "\n"
		. '        ' . T_('Page:') . ' '. "\n"
		. implode("     - \n", $pagelinks) . "\n"
		. '    </fieldset>' . "\n"
		. '</div>' . "\n\n";
	}
	else {
		$html = "";
	}

	return $html;
}

/**
 * Creates HTML for a select element
 *
 * The array $options should contain values and descriptions:
 * array(
 * 	'value' => array(
 * 	'text'     => 'Description',
 * 	'selected' => (TRUE|FALSE),
 * 	),
 * 	...
 * )
 *
 * @param string $name The name for the select element
 * @param array $options The array of options attributes for the select element
 * @param string $onchange (optional) The string that will be exectuted when the user changes options
 * @return string $html The HTML for for select element
 */
function g2ic_make_html_select($name,$options,$onchange=null) {
	$html = '            <select name="' . $name . '" size="1" ';
	if($onchange) {
		$html .= 'onchange="' . $onchange . '" ';
	}
	$html .= '>' . "\n";
	foreach ($options as $value => $option) {
		$html .= "                <option value='{$value}'";
		if (isset($option['selected']) and $option['selected'])
			$html .= " selected='selected'";
		$html .= ">{$option['text']}</option>\n";
	}
	$html .= "            </select>\n";

	return $html;
}

/**
 * Creates the HTML for inserting an album WPG2 Tag
 *
 * @return string $html The HTML for for inserting an album WPG2 Tag
 */
function g2ic_make_html_wpg2_album_insert_button(){

	GLOBAL $g2ic_options, $g2ic_gallery_items;
	$html = '';

	$album_info = g2ic_get_item_info($g2ic_options['current_album']);

	// Create the form
	$html .= "<div>\n"
	. "    <fieldset>\n"
	. '        <legend>' . T_('Insert a WPG2 tag for the current album:') . ' ' . $album_info['title'] . '</legend>' . "\n";

	if (empty($g2ic_gallery_items)) {
		$html .= '            ' . T_('G2Image Alignment Class') . ' ' . "\n"
		. g2ic_make_html_alignment_select()
		. '            <br />' . "\n";
	}

	// "Insert" button
	$html .= '            <input type="button"' . "\n"
	. '            onclick="insertWpg2Tag()"' . "\n"
	. '            value="' . T_('Insert') . '"' . "\n"
	. '            />' . "\n";

	if (!empty($g2ic_gallery_items)) {
		$html .= '            ' . T_('Set the Alignment Class in "Insertion Options" below') . ' ' . "\n";
	}

	$html .= '            <input type="hidden" name="wpg2_id" value="' . $g2ic_options['current_album'] . '" />' . "\n"
	. '            <input type="hidden" name="wpg2_summary" value="' . $album_info['summary'] . '" />' . "\n"
	. '            <input type="hidden" name="wpg2_thumbnail" value="' . $album_info['thumbnail_src'] . '" />' . "\n"
	. '            <input type="hidden" name="wpg2_thumbw" value="' . $album_info['thumbnail_width'] . '" />' . "\n"
	. '            <input type="hidden" name="wpg2_thumbh" value="' . $album_info['thumbnail_height'] . '" />' . "\n"
	. '    </fieldset>' . "\n"
	. '</div>' . "\n\n";

	return $html;
}
?>
