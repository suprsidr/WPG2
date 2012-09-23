<?php
/*
	Author: WPG2 Team
	Updated: 13:17 25/05/2008

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
 *Provide Embedded Gallery2 User Accounts with Create, Delete and Admin (de)Promoting Functions
 *
 * @param NULL
 * @return NULL
 */

// Grab Current WP User (Prevent Admin Changes)
global $user_ID;

if ( !current_user_can('edit_users') )
	die ( __('Cheatin&#8217; uh?', 'wpg2') );

if (function_exists('btev_trigger_error')) {
	btev_trigger_error('FUNCTION: WPG2 Manage G2 Users', E_USER_NOTICE, __FILE__);
}

// Initialize Gallery
	if (!defined('G2INIT')) {
		$ret = g2_login();
		if ($ret) {
			echo '<h2>' . __('Fatal G2 error:', 'wpg2') . '</h2>' . $ret;
			exit;
		}
	}

// Remove the g2_user cap
	if($_GET['duser_id'] != "" && $_GET['dg2user_id'] == "") {
		$usercap = new WP_User($_GET['duser_id']);
		if ($usercap->has_cap('gallery2_user')) {
				?><div id="message" class="updated fade"><p><strong><?php _e('Gallery2 User deleted and Wordpress Gallery2 User capability Revoked.', 'wpg2') ?></strong></p></div><?php
				$usercap->add_cap('gallery2_user', false);
				if (function_exists('btev_trigger_error')) {
					btev_trigger_error('G2USER capability removed from WP ID:'.$_GET['duser_id'], E_USER_NOTICE, __FILE__);
				}
				 g2_delete_user($_GET['duser_id']);
		}
	}

// add the g2_user cap
	if($_GET['auser_id'] != "" && $_GET['ag2user_id'] == "" ) {
		$usercap = new WP_User($_GET['auser_id']);
		if (!$usercap->has_cap('gallery2_user')) {
			$usercap->add_cap('gallery2_user', true);
			?><div id="message" class="updated fade"><p><strong><?php _e('Gallery2 User Created and Wordpress Gallery2 User capability Granted.', 'wpg2') ?></strong></p></div><?php
			if (function_exists('btev_trigger_error')) {
				btev_trigger_error('G2USER capability Granted to WP ID:'.$_GET['auser_id'], E_USER_NOTICE, __FILE__);
			}
		}
		g2_create_user($_GET['auser_id']);
	}

// Remove the g2_admin cap
	if($_GET['duser_id'] != "" && $_GET['dg2user_id'] != "" ) {
		$usercap = new WP_User($_GET['duser_id']);
		if ($usercap->has_cap('gallery2_admin')) {
			?><div id="message" class="updated fade"><p><strong><?php _e('Gallery2 admin capability Revoked.', 'wpg2') ?></strong></p></div><?php
			$usercap->add_cap('gallery2_admin', false);
			if (function_exists('btev_trigger_error')) {
				btev_trigger_error('G2ADMIN capability removed from WP ID:'.$_GET['duser_id'], E_USER_NOTICE, __FILE__);
			}
			$ret = g2_admin_user($_GET['duser_id']);
		}
	}

// Add the g2_admin cap
	if($_GET['auser_id'] != "" && $_GET['ag2user_id'] != "" ) {
		$usercap = new WP_User($_GET['auser_id']);
		if (!$usercap->has_cap('gallery2_admin')) {
			$usercap->add_cap('gallery2_admin', true);
			?><div id="message" class="updated fade"><p><strong><?php _e('Gallery2 admin capability Granted.', 'wpg2') ?></strong></p></div><?php
		// Add BTEV Event Message
			if (function_exists('btev_trigger_error')) {
				btev_trigger_error('G2ADMIN capability added to WP ID:'.$_GET['auser_id'], E_USER_NOTICE, __FILE__);
			}
			$ret = g2_admin_user($_GET['auser_id']);
		}
	}

// ---- END ACTIONS

// Renew a List of all externally Mapped Users in Gallery2
	list ($ret, $g2users) = GalleryEmbed::getExternalIdMap('externalId');
	if (!$ret) {
		foreach ($g2users as $g2user) {
			if ( $g2user['entityType'] == "GalleryUser" ) {
				$ret = GalleryCoreApi::removeMapEntry('ExternalIdMap', array('externalId' => $g2user['externalId'], 'entityType' => 'GalleryUser'));
			}
		}
	}

// Get a List of all current WP Users & Renew Mapping
	$wpusers = $wpdb->get_results("SELECT ID FROM $wpdb->users ORDER BY ID");
	foreach ($wpusers as $wpuser) {
		// Does the user have gallery2_user Role
	    $usercap = new WP_User($wpuser->ID);
		if ($usercap->has_cap('gallery2_user')) {
		 // Find User by UserName to Remap
            list ($ret, $g2user ) = GalleryCoreApi::fetchUserByUsername($usercap->user_login);
			if ($ret) // Secondary Find by Email Address
				list ($ret, $g2user ) = g2_fetchUserByUserEmail($usercap->user_email);	
			if (!$ret) { 
				list ($ret, $g2_results) = GalleryCoreApi::getMapEntry('ExternalIdMap', array('entityId'), array('entityId' => $g2user->id, 'entityType' => 'GalleryUser'));
				if (!($g2_result = $g2_results->nextResult())) {
					GalleryEmbed::addExternalIdMapEntry($wpuser->ID, $g2user->id, 'GalleryUser');
				} else
					g2_create_user($wpuser->ID);		
			}
		}
	}

// Get a List of all current WP Users
	$wpusers = $wpdb->get_results("SELECT ID FROM $wpdb->users ORDER BY ID");
	$cnt = 0;
	foreach ($wpusers as $wpuser) {
		$wparray[$cnt] = $wpuser->ID;
		$cnt++;
	}

// Get G2 Mapping
	list ($ret, $g2users) = GalleryEmbed::getExternalIdMap('entityId');
	if ($ret) {
		echo __('Fatal G2 error:', 'wpg2') . $ret->getAsHtml();
		exit;
	}

	foreach ($g2users as $g2user) {
		if ( $g2user['entityType'] == "GalleryUser" ) {
			$ret = GalleryCoreApi::removeMapEntry( 'ExternalIdMap', array('externalId' => $g2array[$entity], 'entityType' => 'GalleryUser'));
			$g2entityarray[$g2user['externalId']] = $g2user['entityId'];
		}
	}

	echo '<div class="wrap">';

// Wordpress Accounts Mapped to G2

	echo '<h2>' . __('Wordpress Users with Gallery2 admin accounts', 'wpg2') . '</h2>';
	echo '<table cellpadding="3" cellspacing="3">';
	echo '<tr><th>' . __('WP ID', 'wpg2') . '</th>';
	echo '<th>' . __('G2 ID', 'wpg2') . '</th>';
	echo '<th>' . __('User Name', 'wpg2') . '</th>';
	echo '<th>' . __('Nickname', 'wpg2') . '</th>';
	echo '<th>' . __('Name', 'wpg2') . '</th>';
	echo '<th>' . __('Email', 'wpg2') . '</th>';
	echo '<th>' . __('WP<>G2 Password Encryption', 'wpg2') . '</th>';
	echo '<th>' . __('Action', 'wpg2') . '</th>';
	echo '<th>&nbsp;</th>';
	echo '</tr>';
	$style = '';
	echo '<FORM METHOD="POST" ACTION="">';
	foreach ($wpusers as $wpuser) {
		//	Get WP & G2 Member Information
		$userdata = new WP_User($wpuser->ID);
		$wpuserid = $wpuser->ID;
		if ($userdata->has_cap('gallery2_user') && $userdata->has_cap('gallery2_admin') && $g2entityarray[$wpuserid] != '' ) {
			// Output WP Infomation
			if ( strlen($userdata->user_pass) > 32 )
				$wpg2_passwordhash = '<font color="red">Incompatible</font>';
			else
				$wpg2_passwordhash = 'Compatible';
			$email = $userdata->user_email;
			$style = ('class="alternate"' == $style) ? '' : 'class="alternate"';
			echo "";
			echo "<tr $style>";
			echo '<td align="center">' . $userdata->ID . '</td>';
			echo '<td align="center">' . $g2entityarray[$wpuserid] . '</td>';
			echo '<td align="center">' . $userdata->user_login . '</td>';
			echo '<td align="center">' . $userdata->nickname . '</td>';
			echo '<td align="center">' . $userdata->user_firstname .' ' . $userdata->user_lastname . '</td>';
			echo '<td align="center"><a href="mailto:' . $email . '" title="' . __('e-mail: ', 'wpg2') . $email . '">' . $email . '</a></td>';

			echo '<td align="center">'.$wpg2_passwordhash.'</td>';
			if ( current_user_can('gallery2_admin') && ($user_ID != $wpuserid ))
				echo '<td align="center"><a href="profile.php?page=wpg2/wpg2usersadmin.php&duser_id=' . $userdata->ID . '&dg2user_id=' . $g2entityarray[$wpuserid] . '">' . __('Revoke G2 Admin', 'wpg2') . '</a></td>';
			else
				echo '<td align="center">NA</td>';

			echo '</tr>';
		}
	}
	echo '</table><br />';

	echo '<h2>' . __('Wordpress Users without Gallery2 admin accounts', 'wpg2') . '</h2>';
	echo '<table cellpadding="3" cellspacing="3" >';
	echo '<tr><th>' . __('WP ID', 'wpg2') . '</th>';
	echo '<th>' . __('G2 ID', 'wpg2') . '</th>';
	echo '<th>' . __('User Name', 'wpg2') . '</th>';
	echo '<th>' . __('Nickname', 'wpg2') . '</th>';
	echo '<th>' . __('Name', 'wpg2') . '</th>';
	echo '<th>' . __('Email', 'wpg2') . '</th>';
	echo '<th>' . __('WP<>G2 Password Encryption', 'wpg2') . '</th>';
	echo '<th>' . __('Action', 'wpg2') . '</th>';
	echo '<th>&nbsp;</th>';
	echo '</tr>';
	$style = '';

	foreach ($wpusers as $wpuser) {
		//	Get WP & G2 Member Information
		$userdata = new WP_User($wpuser->ID);
		$wpuserid = $wpuser->ID;
		if ($userdata->has_cap('gallery2_user') && !$userdata->has_cap('gallery2_admin') && $g2entityarray[$wpuserid] != '' ) {
			// Output WP Infomation
			$email = $userdata->user_email;
			if ( strlen($userdata->user_pass) > 32 )
				$wpg2_passwordhash = '<font color="red">Incompatible</font>';
			else
				$wpg2_passwordhash = 'Compatible';
			$style = ('class="alternate"' == $style) ? '' : 'class="alternate"';
			echo "";
			echo "<tr $style>";
			echo '<td align="center">' . $userdata->ID . '</td>';
			echo '<td align="center">' . $g2entityarray[$wpuserid] . '</td>';
			echo '<td align="center">' . $userdata->user_login . '</td>';
			echo '<td align="center">' . $userdata->nickname . '</td>';
			echo '<td align="center">' . $userdata->user_firstname .' ' . $userdata->user_lastname . '</td>';
			echo '<td align="center"><a href="mailto:' . $email . '" title="' . __('e-mail: ', 'wpg2') . $email . '">' . $email . '</a></td>';
			echo '<td align="center">'.$wpg2_passwordhash.'</td>';
			if ( current_user_can('gallery2_admin') ) {
				echo '<td align="center"><a href="profile.php?page=wpg2/wpg2usersadmin.php&auser_id=' . $userdata->ID . '&ag2user_id=' . $g2entityarray[$wpuserid] . '">' . __('Grant G2 Admin', 'wpg2') . '</a>';
				echo ' / <a href="profile.php?page=wpg2/wpg2usersadmin.php&duser_id=' . $userdata->ID . '">' . __('Revoke G2 User', 'wpg2') . '</a></td>';

			} else {
				echo '<td align="center"><a href="profile.php?page=wpg2/wpg2usersadmin.php&duser_id=' . $userdata->ID . '">' . __('Revoke G2 User', 'wpg2') . '</a></td>';
			}
			echo '</tr>';
		}
	}

	echo '</table><br />';

// Wordpress Accounts Not mapped to G2

	echo '<h2>' . __('Wordpress Users without Gallery2 accounts', 'wpg2') . '</h2>';
	echo '<table cellpadding="3" cellspacing="3" >';
	echo '<tr><th>' . __('WP ID', 'wpg2') . '</th>';
	echo '<th>' . __('G2 ID', 'wpg2') . '</th>';
	echo '<th>' . __('User Name', 'wpg2') . '</th>';
	echo '<th>' . __('Nickname', 'wpg2') . '</th>';
	echo '<th>' . __('Name', 'wpg2') . '</th>';
	echo '<th>' . __('Email', 'wpg2') . '</th>';
	echo '<th>' . __('WP<>G2 Password Encryption', 'wpg2') . '</th>';
	echo '<th>' . __('Action', 'wpg2') . '</th>';
	echo '<th>&nbsp;</th>';
	echo '</tr>';
	$style = '';

	foreach ($wpusers as $wpuser) {
		//	Get WP & G2 Member Information
		$wpuserid = $wpuser->ID;
		$userdata = new WP_User($wpuser->ID);
		if (!$userdata->has_cap('gallery2_user') || $g2entityarray[$wpuserid] == '' ) {
			// Output WP Infomation
			$email = $userdata->user_email;
			if ( strlen($userdata->user_pass) > 32 )
				$wpg2_passwordhash = '<font color="red">Incompatible</font>';
			else
				$wpg2_passwordhash = 'Compatible';
			$style = ('class="alternate"' == $style) ? '' : 'class="alternate"';
			echo "";
			echo "<tr $style>";
			echo '<td align="center">' . $userdata->ID . '</td>';
			echo '<td align="center">' . __('NA', 'wpg2') . '</td>';
			echo '<td align="center">' . $userdata->user_login . '</td>';
			echo '<td align="center">' . $userdata->nickname . '</td>';
			echo '<td align="center">' . $userdata->user_firstname  . ' ' . $userdata->user_lastname . '</td>';
			echo '<td align="center"><a href="mailto:' . $email . '" title="' . __('e-mail: ', 'wpg2') . $email . '">' . $email . '</a></td>';
			echo '<td align="center">'.$wpg2_passwordhash.'</td>';
			if ( strlen($userdata->user_pass) > 32 )
				echo '<td align="center">'.__('NA','wpg2').'</td>';
			else
				echo '<td align="center"><a href="profile.php?page=wpg2/wpg2usersadmin.php&auser_id=' . $userdata->ID . '">' . __('Grant G2 User', 'wpg2') . '</a></td>';
			echo '</tr>';
		}
	}

	echo '</form></table>';
	echo __('<br />NOTE: Due to password encryption changes in Wordpress 2.5, passwords are not initially compatible with Gallery2.<br />WPG2 can only make the Wordpress password compatible with Gallery2 after you have logged off the wordpress user and logged back in.','wpg2');
	echo '</div>';

?>