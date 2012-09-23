<?php
/*
	Author: WPG2 Team
	Updated: 21:45 9/06/2008

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
													G2 Session Handling
********************************************************************************************************
*/

/**
* Initialize Gallery; must be called before most GalleryEmbed methods can be used.
* This method should only be called once during the lifetime of the request.
*
* @param NULL
* @return string GalleryStatus
*/

function g2_init() {

	// Get WPG2 Option Settings
	$wpg2_option = get_option('wpg2_options');
	$wpg2_g2path = get_option('wpg2_g2paths');

	@include_once($wpg2_g2path['g2_filepath'].'embed.php');

	if (!defined('G2PARTINIT')) {
		// Initialise GalleryAPI
			$ret = GalleryEmbed::init( array(
						'embedUri' => $wpg2_g2path['g2_embeduri'],
						'g2Uri' => $wpg2_g2path['g2_url'],
						'loginRedirect' => $wpg2_g2path['g2_errorredirect'],
						'fullInit' => true)
					);
	} else {
		// Initialise GalleryAPI
			$ret = GalleryEmbed::init( array(
						'embedUri' => $wpg2_g2path['g2_embeduri'],
						'g2Uri' => $wpg2_g2path['g2_url'],
						'loginRedirect' => $wpg2_g2path['g2_errorredirect'])
				);
	}

	if ($ret) {
		if (function_exists('btev_trigger_error')) {
			btev_trigger_error('WPG2 FAILED TO INITALISE G2', E_USER_ERROR, __FILE__);
		}
		$ret->getAsHtml();
		return $ret;
	} else {
		// Declate G2 Init so we do not do it again..
		define("G2INIT", "True");
		if (function_exists('btev_trigger_error')) {
			btev_trigger_error('WPG2 INITALISED G2', E_USER_NOTICE, __FILE__);
		}
		return $ret;
	}
}

/*
********************************************************************************************************
											G2 User Handling
********************************************************************************************************
*/

/**
* Logs in the externally mapped Gallery2 User Account
*
* @param NULL
* @return string GalleryStatus
*/

function g2_login() {
	// Get WPG2 Option Settings
	$wpg2_option = get_option('wpg2_options');
	global $user_ID;

// Initalise the WP Capacitiy Class
	$userrole = new WP_User($user_ID);

	if (!defined('G2INIT')) {
		$ret = g2_init();
		if ($ret) {
			echo '<h2>' . __('Fatal G2 error', 'wpg2') . '</h2> ' . __("Here's the error from G2: ", 'wpg2') . $ret->getAsHtml();
			exit;
		}
	} 

// Set G2 Session Handling  = Guest
	$wpg2guest = "True";

//	Try to Login with Current WP User Mapping via G2 ExternalID if user has Gallery2_user
	if ($user_ID) {
		if ($userrole->has_cap('gallery2_user')) {
			$ret = GalleryEmbed::isExternalIdMapped($user_ID, 'GalleryUser');
			if ($ret) {
				g2_create_user($user_ID);
			}
			// Check to Ensure the Galley2 User has actually been created, this may not be the case in WP 2.5
			$ret = GalleryEmbed::isExternalIdMapped($user_ID, 'GalleryUser');
			if (!$ret) { 	
				$ret = GalleryEmbed::checkActiveUser($user_ID);
				if ($ret) { // Fatal Return External ID Mapping Must be Corrupted - Remove and Retry
					// Add BTEV Event Message
					if (function_exists('btev_trigger_error')) {
						btev_trigger_error('G2 INVALID EXTERNAL MAP TO ID:'.$user_ID, E_USER_ERROR, __FILE__);
					}
					$ret = GalleryCoreApi::removeMapEntry('ExternalIdMap', array('externalId' => $user_ID, 'entityType' => 'GalleryUser'));
				} else {
					// No Longer Guest
					$wpg2guest = '';
				}
			}
		}
	} 

	if ($wpg2guest) {	
		$ret = GalleryEmbed::checkActiveUser('');	
		// Gallery2 Session Handling - Stop Temp ID's Fix  Bug #43
		global $gallery;
		$g2_session =&$gallery->getSession();
		$g2_session->doNotUseTempId();
		// Add BTEV Event Message
		if (function_exists('btev_trigger_error')) {
				btev_trigger_error('WPG2 LOGGED IN WP:'.$user_ID.' AS G2 GUEST', E_USER_NOTICE, __FILE__);
		}
	} else {
		global $gallery;
		$g2user = $gallery->getActiveUser();
		// Add BTEV Event Message
		if (function_exists('btev_trigger_error')) {
			btev_trigger_error('WPG2 LOGGED IN WP:'.$user_ID.' G2 ID:'.$g2user->id, E_USER_NOTICE, __FILE__);
		}
		// Do the Session Thing!
		$g2_session =&$gallery->getSession();
		$idInSession = $g2_session->get('embed.id.externalUser');
	
		if ($idInSession === $user_ID) {
			// Add BTEV Event Message
			if (function_exists('btev_trigger_error')) {
				btev_trigger_error('WPG2 G2 SESSION RESUMED', E_USER_NOTICE, __FILE__);		    
			}
		} else {
			$g2_session->start();
			$g2_session->doNotUseTempId();
			// Add BTEV Event Message
			if (function_exists('btev_trigger_error')) {
				btev_trigger_error('WPG2 G2 SESSION CREATED', E_USER_NOTICE, __FILE__);		    
			}
		}
		// Set G2 SiteAdminSession (if required)		
		list ($ret, $isAdmin) = GalleryCoreApi::isUserInSiteAdminGroup($g2user->id);
		if ($isAdmin) {
			$g2_session->put('session.siteAdminActivityTimestamp', time());
		}
	}

	return $ret;

}


/**
* Logs Out the Externally Mapped Gallery2 User
*
* @param NULL
* @return NULL
*/
function g2_logout() {

	global $user_ID;

	// Get WPG2 Option Settings
	$wpg2_g2path = get_option('wpg2_g2paths');

	// Test if Plugin has been validated.
	if ($wpg2_g2path['g2_validated'] == "Yes" ) {
		@include_once($g2_['g2_filepath'].'embed.php');

		$ret = GalleryEmbed::logout( array(
				'embedUri' => $wpg2_g2path['g2_embed'])
			);

	}

	// Add BTEV Event Message
	if (function_exists('btev_trigger_error')) {
		btev_trigger_error('WPG2 (WP:'.$user_ID.') G2 LOGOFF', E_USER_NOTICE, __FILE__);
	}

}

/**
* Creates an externally mapped Gallery2 User account.
*
* @param Internal Wordpress User ID
* @return NULL
*/
function g2_create_user($wpuser_id) {

	if (!defined('G2INIT')) {
		$ret = g2_login();
		if ($ret) {
			echo '<h2>' . __('Fatal G2 error', 'wpg2') . '</h2> ' . __("Here's the error from G2: ", 'wpg2') . $ret->getAsHtml();
			exit;
		}
	}

// Does the user have gallery2_user Role
    $usercap = new WP_User($wpuser_id);
    if ($usercap->has_cap('gallery2_user')) {
     // Check to Make Sure User Doesn't already have an External ID
		$g2_mapped = null;
		list ($ret, $g2_results) = GalleryCoreApi::getMapEntry('ExternalIdMap', array('entityId'), array('externalId' => $wpuser_id, 'entityType' => 'GalleryUser'));
		if (!($g2_result = $g2_results->nextResult())) {
            list ($ret, $g2user ) = GalleryCoreApi::fetchUserByUsername($usercap->user_login);
//			if ($ret) // Secondary Find by Email Address
//				list ($ret, $g2user ) = g2_fetchUserByUserEmail($usercap->user_email);	
			if (!$ret) {
				list ($ret, $g2_results) = GalleryCoreApi::getMapEntry('ExternalIdMap', array('entityId'), array('entityId' => $g2user->id, 'entityType' => 'GalleryUser'));
				if (!($g2_result = $g2_results->nextResult())) {
	                GalleryEmbed::addExternalIdMapEntry($wpuser_id, $g2user->id, 'GalleryUser');
					$g2_mapped = 1;
					// Add BTEV Event Message
					if (function_exists('btev_trigger_error')) {
						btev_trigger_error('WPG2 MAPPED WP:'.$wpuser_id.' TO G2 ID:'.$g2user->id, E_USER_NOTICE, __FILE__);
					}
				}
			}
			if (!$g2_mapped ) {
				// Add BTEV Event Message
				if (function_exists('btev_trigger_error')) {
					btev_trigger_error('WPG2 COULD NOT FIND G2 USER:'.$usercap->user_login, E_USER_NOTICE, __FILE__);
				}
				// User has not been mapped, if WP password been hashed, then skip update.
				if ( strlen($usercap->user_pass) > 32 ) {
					// Add BTEV Event Message
					if (function_exists('btev_trigger_error')) {
						btev_trigger_error('WPG2 G2 USER (WP:'.$wpuser_id.') COULD NOT BE CREATED DUE TO PWD HASHING', E_USER_ERROR, __FILE__);
						$ret = '';
					}
				} else {
					$ret = GalleryEmbed::createUser( $usercap->ID, array ( 'username' => $usercap->user_login, 'email' => $usercap->user_email, 'fullname' => $usercap->user_nicename,
							'hashedpassword' => $usercap->user_pass, 'hashmethod' => 'md5'));
					if ($ret) {
						echo '<h2>' . __('Fatal G2 error', 'wpg2') . '</h2> ' . __("Here's the error from G2: ", 'wpg2') . $ret->getAsHtml();
						exit;
					}
					// Add BTEV Event Message
					if (function_exists('btev_trigger_error')) {
						btev_trigger_error('WPG2 CREATED G2 USER  FOR WP:'.$wpuser_id, E_USER_NOTICE, __FILE__);
					}
						$ret = g2_admin_user($wpuser_id);
					}
			}
		}
	} else {
				// Add BTEV Event Message
				if (function_exists('btev_trigger_error')) {
					btev_trigger_error('WPG2 (WP:'.$wpuser_id.') NO G2USER CAP', E_USER_ERROR, __FILE__);
				}
	}

	if ($ret) {
        echo '<h2>' . __('Fatal G2 error', 'wpg2') . '</h2> ' . __("Here's the error from G2: ", 'wpg2') . $ret->getAsHtml();
        exit;
    }

	return $ret;

}

/**
* Deletes a externally mapped Gallery2 User Accout
*
* @param int Wordpress User ID
* @return string GalleryStatus
*/
function g2_delete_user($wpuser_id) {

	if (!defined('G2INIT')) {
		$ret = g2_login();
		if ($ret) {
			echo '<h2>' . __('Fatal G2 error', 'wpg2') . '</h2> ' . __("Here's the error from G2: ", 'wpg2') . $ret->getAsHtml();
			exit;
		}
	}

	// Check to Make Sure User Does already Exist
	list ($ret, $g2_results) = GalleryCoreApi::getMapEntry('ExternalIdMap', array('entityId'), array('externalId' => $wpuser_id, 'entityType' => 'GalleryUser'));
	if ($g2_result = $g2_results->nextResult()) {
	   $ret = GalleryEmbed::deleteUser($wpuser_id);
		if ($ret) {
			echo '<h2>' . __('Fatal G2 error', 'wpg2') . '</h2> ' . __("Here's the error from G2: ", 'wpg2') . $ret->getAsHtml();
			exit;
		}
		// Add BTEV Event Message
		if (function_exists('btev_trigger_error')) {
			btev_trigger_error('WPG2 DELETED G2 USER ('.$wpuser_id.') ', E_USER_NOTICE, __FILE__);
		}
	} else {
		$ret = "";
		// Add BTEV Event Message
		if (function_exists('btev_trigger_error')) {
			btev_trigger_error('WPG2 FOUND NO G2 USER TO DELETE', E_USER_NOTICE, __FILE__);
		}
	}

	return $ret;

}

/**
* Updates a externally mapped Gallery2 User Account
*
* @param int Wordpress User ID
* @return string GalleryStatus
*/
function g2_update_user($wpuser_id) {

	if (!defined('G2INIT')) {
		$ret = g2_login();
		if ($ret) {
			echo '<h2>' . __('Fatal G2 error', 'wpg2') . '</h2> ' . __("Here's the error from G2: ", 'wpg2') . $ret->getAsHtml();
			exit;
		}
	}

	// Does the user have gallery2_user Role
    $usercap = new WP_User($wpuser_id);
    if ($usercap->has_cap('gallery2_user')) {
		// Has password been hashed, then skip update.
		if ( strlen($usercap->user_pass) > 32 ) {
			// Add BTEV Event Message
			if (function_exists('btev_trigger_error')) {
				btev_trigger_error('WPG2 WP USER ('.$usercap->id.') SKIPPED DUE TO HASHING', E_USER_ERROR, __FILE__);
				$ret = '';
			}
		} else {
	     // Check to Make Sure User Does already Exist
			$ret = GalleryEmbed::isExternalIdMapped($wpuser_id, 'GalleryUser');
			if ($ret) {
		        $ret = GalleryEmbed::updateUser( $wpuser_id, array ( 'username' => $usercap->user_login, 'email' => $usercap->user_email, 'fullname' => $usercap->user_nicename,
	                    'hashedpassword' => $usercap->user_pass, 'hashmethod' => 'md5'));
				// Add BTEV Event Message
				if (function_exists('btev_trigger_error')) {
					btev_trigger_error('WPG2 G2 USER (WP:'.$wpuser_id.') UPDATED', E_USER_NOTICE, __FILE__);
				}
				// Check if User should be Admin
				$ret = g2_admin_user($wpuser_id);
	        } else {
				$ret = g2_create_user($wpuser_id);
				if ($ret) {
					echo '<h2>' . __('Fatal G2 error', 'wpg2') . '</h2> ' . __("Here's the error from G2: ", 'wpg2') . $ret->getAsHtml();
					exit;
				}
			}
		}
	} 

}

/**
* Promotes a externally mapped Gallery2 user into the Gallery2 Admin Group
*
* @param int wordpress User ID
* @return string GalleryStatus
*/
function g2_admin_user($wpuser_id) {

	if (!defined('G2INIT')) {
		$ret = g2_login();
		if ($ret) {
			echo '<h2>' . __('Fatal G2 error', 'wpg2') . '</h2> ' . __("Here's the error from G2: ", 'wpg2') . $ret->getAsHtml();
			exit;
		}
	}

    $usercap = new WP_User($wpuser_id);
    if ($usercap->has_cap('gallery2_admin')) {
	 // Check to Make Sure User Does already Exist
		list ($ret, $g2_results) = GalleryCoreApi::getMapEntry('ExternalIdMap', array('entityId'), array('externalId' => $wpuser_id, 'entityType' => 'GalleryUser'));
		if ($g2_result = $g2_results->nextResult()) {
			list ($ret, $g2user) = GalleryCoreApi::loadEntityByExternalId($wpuser_id, 'GalleryUser');
			if ($ret) {
				echo '<h2>' . __('Fatal G2 error', 'wpg2') . '</h2> ' . __("Here's the error from G2: ", 'wpg2') . $ret->getAsHtml();
				exit;
			}

			list ($ret, $isAdmin) = GalleryCoreApi::isUserInSiteAdminGroup($g2user->id);
			if (!$isAdmin) {
			   list ($ret, $siteAdminGroupId) = GalleryCoreApi::getPluginParameter('module', 'core', 'id.adminGroup');
			   $ret = GalleryCoreApi::addUserToGroup($g2user->id, $siteAdminGroupId);
				if ($ret) {
					echo '<h2>' . __('Fatal G2 error', 'wpg2') . '</h2> ' . __("Here's the error from G2: ", 'wpg2') . $ret->getAsHtml();
					exit;
				}
				// Add BTEV Event Message
				if (function_exists('btev_trigger_error')) {
					btev_trigger_error('WPG2 ADD WP USER ('.$wpuser_id.') TO G2 ADMIN ', E_USER_NOTICE, __FILE__);
				}
			}
		}
	} else {
		list ($ret, $g2_results) = GalleryCoreApi::getMapEntry('ExternalIdMap', array('entityId'), array('externalId' => $wpuser_id, 'entityType' => 'GalleryUser'));
		if ($g2_result = $g2_results->nextResult()) {
			list ($ret, $g2user) = GalleryCoreApi::loadEntityByExternalId($wpuser_id, 'GalleryUser');
			if ($ret) {
				echo '<h2>' . __('Fatal G2 error', 'wpg2') . '</h2> ' . __("Here's the error from G2: ", 'wpg2') . $ret->getAsHtml();
				exit;
			}
			list ($ret, $isAdmin) = GalleryCoreApi::isUserInSiteAdminGroup($g2user->id);
			if ($isAdmin) {
			   list ($ret, $siteAdminGroupId) = GalleryCoreApi::getPluginParameter('module', 'core', 'id.adminGroup');
			   $ret = GalleryCoreApi::removeUserFromGroup($g2user->id, $siteAdminGroupId);
				if ($ret) {
					echo '<h2>' . __('Fatal G2 error', 'wpg2') . '</h2> ' . __("Here's the error from G2: ", 'wpg2') . $ret->getAsHtml();
					exit;
				}
				// Add BTEV Event Message
				if (function_exists('btev_trigger_error')) {
					btev_trigger_error('WPG2 REMOVED WP USER ('.$wpuser_id.') FROM G2 ADMIN ', E_USER_NOTICE, __FILE__);
				}
			}
		} else // Reset Return Because G2<>WP Users Are In Sync
			$ret = "";

	}

	return $ret;

}

/**
* Returns Gallery2 User by Email Address
*
* @param string WP User email
* @return string GalleryStatus
 * @see GalleryCoreApi::fetchUserByUserName
 */

function g2_fetchUserByUserEmail($userEmail=null) {
	global $gallery;

	$query = '
	SELECT
	  [GalleryUser::id]
	FROM
	  [GalleryUser]
	WHERE
	  [GalleryUser::email] = ?
	';
	list ($ret, $searchResults) = $gallery->search($query, array($userEmail));
	if ($ret) {
		return array($ret, null);
	}

	if ($searchResults->resultCount() == 0) {
		return array(GalleryCoreApi::error(ERROR_MISSING_OBJECT), null);
	} else {
		$result = $searchResults->nextResult();
		$id = $result[0];
		list ($ret, $user) = GalleryCoreApi::loadEntitiesById($id, 'GalleryUser');
		if ($ret) {
			return array($ret, null);
		}
		return array(null, $user);
	}
}

/**
* Closes the Gallery2 Embedded Session
*
* @param none
* @return none
*/
function g2_isdone() {

	if (defined('G2INIT') && !defined("WPG2PAGE")) {
		GalleryEmbed::done();
		// Add BTEV Event Message
		if (function_exists('btev_trigger_error')) {
			btev_trigger_error('WPG2 ISDONE()', E_USER_NOTICE, __FILE__);
		}
	}
}


/*
********************************************************************************************************
																G2 Validation
********************************************************************************************************
*/

/**
* Validates if Gallery is located at the supplied URL
*
* @param string url
* @return string error Code
*/
function wpg2_validateg2url($g2url, $wpg2relpath=null) {

	// Include the Gallery2 Validation Tools
	$wpg2base = 'wp-content'.DIRECTORY_SEPARATOR.'plugins'.DIRECTORY_SEPARATOR.'wpg2';

	@include_once(ABSPATH . $wpg2base . DIRECTORY_SEPARATOR. 'g2embeddiscoveryutilities.class');

	// Set up the URL's and Paths
	$site_url = trailingslashit(get_option('siteurl'));
	$g2Uri = G2EmbedDiscoveryUtilities::normalizeG2Uri($g2url);
	$embedUri = G2EmbedDiscoveryUtilities::normalizeG2Uri($site_url);
	$file_path = ABSPATH;
	// Try the first method of getting embedPhpPath
	list ($success, $embedPhpPath, $errorString) = G2EmbedDiscoveryUtilities::getG2EmbedPathByG2Uri($g2Uri);
	if (!$success) {
	// Try the second method of getting embedPhpPath
		list ($success, $embedPhpPath, $errorString) = G2EmbedDiscoveryUtilities::getG2EmbedPathByG2UriEmbedUriAndLocation($g2Uri, $embedUri, $file_path);
	}
	if ($success){
		// Set the Plugin Paths
		$wpg2_g2path['g2_filepath'] = rtrim ($embedPhpPath, "embed.php");

		// Clean up File Structure..
		$slash = DIRECTORY_SEPARATOR;
		$doubleslash = DIRECTORY_SEPARATOR.DIRECTORY_SEPARATOR;
		$wpg2_g2path['g2_filepath'] = str_replace($doubleslash,$slash,$wpg2_g2path['g2_filepath']);
		$wpg2_g2path['g2_filepath'] = str_replace($doubleslash,$slash,$wpg2_g2path['g2_filepath']);
		$wpg2_g2path['g2_filepath'] = str_replace('/', DIRECTORY_SEPARATOR, $wpg2_g2path['g2_filepath']);

		$wpg2_g2path['g2_url'] = str_replace("//","/",$g2Uri);
		$wpg2_g2path['g2_url'] = str_replace('http:/', 'http://',$wpg2_g2path['g2_url']);

		$wpg2_g2path['g2_errorredirect'] = get_option( 'home' );
		$wpg2_g2path['g2_errorredirect'] .= "/wp-login.php";

		$wpg2relpath = dirname($wpg2_g2path['g2_filepath']);

		if (file_exists($wpg2relpath.DIRECTORY_SEPARATOR.'wp-login.php')) {
			$wpg2relpath = '..'.DIRECTORY_SEPARATOR.$wpg2base;
		} else {
			$wpg2relpath = str_replace($wpg2relpath,'',ABSPATH); 
			$wpg2relpath = '..'.$wpg2relpath.$wpg2base;
		}

		$wpg2_g2path['wpg2_relativepath'] = $wpg2relpath;
		
		// Add BTEV Event Message
		if (function_exists('btev_trigger_error')) {
			btev_trigger_error('WPG2 SET G2 URL TO '.$wpg2_g2path['g2_url'], E_USER_NOTICE, __FILE__);
			btev_trigger_error('WPG2 SET G2 FILE PATH TO '.$wpg2_g2path['g2_filepath'], E_USER_NOTICE, __FILE__);
			btev_trigger_error('WPG2 SET G2TOWPG2 FILE PATH TO '.$wpg2_g2path['wpg2_relativepath'], E_USER_NOTICE, __FILE__);
		}

		// Attempt to Provide a Default Relative Path
		if ($wpg2relpath == NULL) {
			$wpg2_g2path['wpg2_relativepath'] = '..'.DIRECTORY_SEPARATOR.ABSPATH.$wpg2base; 
		}

		// Update G2 Paths
		update_option('wpg2_g2paths', $wpg2_g2path);

		return false;
	}
	else {
		// Add BTEV Event Message
		if (function_exists('btev_trigger_error')) {
			btev_trigger_error('WPG2 FAILED TO LOCATE G2 AT URL '.$g2Uri, E_USER_NOTICE, __FILE__);
		}
		return $errorString;
	}
}

/**
* Verifies if the Supplied URI is in a W3C Valid Format
*
* @param string url
* @return string error
*/
function wpg2_validateuri($wpg2uri) {

	// Include the Gallery2 Validation Tools
	$slash = DIRECTORY_SEPARATOR;
	$wpg2base = 'wp-content'.$slash.'plugins'.$slash.'wpg2';

	@include_once(ABSPATH . $wpg2base . $slash . 'g2embeddiscoveryutilities.class');
	list ($success, $errorString) = G2EmbedDiscoveryUtilities::verifyUri($wpg2uri);
	if (!$success) {
		// Return Error
		$ret =  $errorString;
	}

	return $ret;
}

/**
* Sets Gallery2 Embedded Rewrite Configuration by suppling the current values of Wordpress Directory and Wordpress HTACCESS File Location
*
* @param string url
* @return string GalleryStatus
*/
function g2_setrewriteepath() {

	// Get WPG2 Option Settings
	$wpg2_option = get_option('wpg2_options');

	// Initialize Gallery
	if (!defined('G2INIT')) {
		$ret = g2_login();
		if ($ret) {
			echo '<h2>' . __('Fatal G2 error', 'wpg2') . '</h2> ' . __("Here's the error from G2: ", 'wpg2') . $ret->getAsHtml();
			exit;
		}
	}

	/*
	 * Load the plugin even if it's deactivated since we only need it for setting the htaccess path as it's possible that this is getting called during activation when we won't be able to properly set the embedded htaccess path.
	 */
	list ($ret, $module) = GalleryCoreApi::loadPlugin('module', 'rewrite', true);
	if (!$ret) {
		// Find the File Path of WP
		$wphome = get_option( 'home' );
		if ( $wphome != '' && $wphome != get_option( 'siteurl' ) ) {
		  $file_path = parse_url( $wphome );
		  $file_path = $file_path['path'];
		  $wproot = str_replace( $_SERVER["PHP_SELF"], '', $_SERVER["SCRIPT_FILENAME"] );
		  $embeddedHtaccess = trailingslashit( $wproot.$file_path );
		} else {
		 $embeddedHtaccess = ABSPATH;
		}

		// Find the URL of WP
		$site_url = trailingslashit(get_option('home'));
		$site_path = str_replace(('https://' . $_SERVER['HTTP_HOST']),'', $site_url);
		$embeddedLocation = str_replace(('http://' . $_SERVER['HTTP_HOST']),'', $site_url);

		// Commit The Values - Do not trap Failures..
		$ret = GalleryCoreApi::setPluginParameter('module', 'rewrite', 'modrewrite.embeddedHtaccess', $embeddedHtaccess);
		$ret = GalleryCoreApi::setPluginParameter('module', 'rewrite', 'modrewrite.embeddedLocation', $embeddedLocation);
	}

	// Add BTEV Event Message
	if (function_exists('btev_trigger_error')) {
		btev_trigger_error('WPG2 SET G2 REWRITE HTACCESS '.$embeddedHtaccess, E_USER_NOTICE, __FILE__);
		btev_trigger_error('WPG2 SET G2 REWRITE LOCATION '.$embeddedLocation, E_USER_NOTICE, __FILE__);
	}

	return $ret;

}


/**
* Completes Gallery2 Embedded Rewrite Configuration by suppling the current values of
* Wordpress Directory and Wordpress HTACCESS File Location
*
* @param string url
* @return string GalleryStatus
*/
function g2_verifyhtaccess() {

	// Find the File Path of WP
	$wphome = get_option( 'home' );
	if ( $wphome != '' && $wphome != get_option( 'siteurl' ) ) {
	  $file_path = parse_url( $wphome );
	  $file_path = $file_path['path'];
	  $wproot = str_replace( $_SERVER["PHP_SELF"], '', $_SERVER["SCRIPT_FILENAME"] );
	  $file_path = trailingslashit( $wproot.$file_path );
	} else {
	  $file_path = ABSPATH;
	}

	// Check if .htaccess exists.  If not and path is writable, create it.
	if (!file_exists($file_path . '.htaccess')) {
		if(is_writable($file_path)) {
			$f = fopen($file_path . '.htaccess', 'w');
			fclose($f);
		}
		// If path is not writable, generate "WordPress Path Not Writable Error"
		else {
			// Add BTEV Event Message
			if (function_exists('btev_trigger_error')) {
				btev_trigger_error('WPG2 VERIFY HTACCESS FAILED NO CREATE ACCESS', E_USER_NOTICE, __FILE__);
			}
			return (__('There is no .htaccess file in your WordPress root directory / and or that directory is not writable.  Please create a writeable .htaccess in that directory.', 'wpg2'));
		}
	}

	if (file_exists($file_path . '.htaccess'))   {
		if (!is_writable($file_path . '.htaccess')) {
			// Add BTEV Event Message
			if (function_exists('btev_trigger_error')) {
				btev_trigger_error('WPG2 VERIFY HTACCESS FAILED NO WRITE ACCESS', E_USER_NOTICE, __FILE__);
			}
			return (__('The .htaccess file in your WordPress root directory (where wp-config.php is located) is not writable.  Please CHMOD it to 644 (or 666 if 644 does not work).', 'wpg2'));
		}
	}

	// Add BTEV Event Message
	if (function_exists('btev_trigger_error')) {
		btev_trigger_error('WPG2 VERIFIED HTACCESS', E_USER_NOTICE, __FILE__);
	}

	return(null);

}

/**
* Activates Gallery2 Rewrite  Module
*
* @param string url
* @return string GalleryStatus
*/
function g2_activaterewrites() {

	// Get WPG2 Option Settings
	$wpg2_option = get_option('wpg2_options');

	// Initialize Gallery
	if (!defined('G2INIT')) {
		$ret = g2_login();
		if ($ret) {
			echo '<h2>' . __('Fatal G2 error', 'wpg2') . '</h2> ' . __("Here's the error from G2: ", 'wpg2') . $ret->getAsHtml();
			exit;
		}
	}

	// Turn on Rewrite Module
	list ($ret, $ignored) = GalleryCoreApi::activatePlugin('module', 'rewrite');

	// Get the Gallery2 Rewrite Configuration
	list ($ret, $rewriteApi) = GalleryCoreApi::newFactoryInstance('RewriteApi');
		if ($ret) {
			echo '<h2>' . __('Fatal G2 error', 'wpg2') . '</h2> ' . __("Here's the error from G2: ", 'wpg2') . $ret->getAsHtml();
			exit;
		}

    // Load the G2 Rewrite Values
	list ($ret, $params) = $rewriteApi->fetchEmbedConfig();

	// Save the G2 rewrite Values
	list ($ret, $code, $err) = $rewriteApi->saveEmbedConfig($params);

	if ( $code > 0 ) {
		list ($ret, $errstr) = $err;
		$errstr =  $code." - ".$errstr;
		// Add BTEV Event Message
		if (function_exists('btev_trigger_error')) {
			btev_trigger_error('WPG2 FAILED TO ACTIVE G2 REWRITE PLUGIN. RETURNED '.$errstr, E_USER_NOTICE, __FILE__);
		}
		return ($errstr);
	}
	else {
		// Add BTEV Event Message
		if (function_exists('btev_trigger_error')) {
			btev_trigger_error('WPG2 ACTIVATED G2 REWRITE PLUGIN', E_USER_NOTICE, __FILE__);
		}
		return (NULL);
	}
}


/*
********************************************************************************************************
											Gallery2 ImageBlock Handling
********************************************************************************************************
*/

/**
* Include image from Gallery2 from WPG2 Tag
*
* @param string $g2inputid Gallery2 Item path relative to root Gallery2 Data directory
* @param integer $g2itemsize Item Size in pixels.  Defaults to null if not included in GET parameters.
* @return string HTML for img tag
*/
function g2_tagimageblock( $g2inputstr ) {

	// Get WPG2 Option Settings
	$wpg2_option = get_option('wpg2_options');

	global $post;

	if ($wpg2_option['g2_validated'] == "Yes") {

		// Initialize Gallery
		if (!defined('G2INIT')) {
			$ret = g2_login();
			if ($ret) {
				echo '<h2>' . __('Fatal G2 error', 'wpg2') . '</h2> ' . __("Here's the error from G2: ", 'wpg2') . $ret->getAsHtml();
				exit;
			}
		}

		// Check for the Item Size | parameter & Clean up Strings..
		$g2itempos = strpos ($g2inputstr, '|');

		if ($g2itempos) {
			$g2itemsize = substr ($g2inputstr, $g2itempos+1);
			$g2itemid = substr ($g2inputstr, 0, $g2itempos);
		} else {
			$g2itemsize = $wpg2_option['g2_postimgsize'];
			$g2itemid = $g2inputstr;
		}

		if ( !is_numeric($g2itemid) ) {
			// Make Sure Item Path does not contain a + as it should instead be a space
			$g2itemid = str_replace ("+", " ", $g2itemid);

			// Get the Image ID
			list ($ret, $g2itemid) = GalleryCoreAPI::fetchItemIdByPath($g2itemid);
			if ($ret) {
				$img = '* ' . __('WPG2 CANNOT LOCATE GALLERY2 ITEM BY '.$g2itemid, 'wpg2') . ' *';
				return $img;
			}
		} else {
				list ($ret, $g2item) = GalleryCoreApi::loadEntitiesById($g2itemid);
			if ($ret) {
				$img = '* ' . __('WPG2 CANNOT LOCATE GALLERY2 ITEM ID '.$g2itemid, 'wpg2') . ' *';
				return $img;
			}
		}

		// Build the Image Block
		$blockoptions['blocks'] = 'specificItem';
		$blockoptions['show'] = 'none';
		$blockoptions['itemId'] = $g2itemid;

		// Assign Show Details
		if ( $wpg2_option['g2_tagblockshow'] ) {
			if ( count($wpg2_option['g2_tagblockshow']) > 1 )
				$blockoptions['show'] = $wpg2_option['g2_tagblockshow'][1].'|'.$wpg2_option['g2_tagblockshow'][2] ;
			else
				$blockoptions['show'] = $wpg2_option['g2_tagblockshow'][1];
		} else
				$blockoptions['show'] = 'none';

		// Assign maxSize
		if ($g2itemsize)
			$blockoptions['exactSize'] = $g2itemsize;
		else
			if ( $wpg2_option['g2_tagimgsize'] )
				$blockoptions['exactSize'] = $wpg2_option['g2_tagimgsize'];

		// Assign Item Frame Style
		if ($wpg2_option['g2_tagimageframe'])
			$blockoptions['itemFrame']  = $wpg2_option['g2_tagimageframe'];

		// Assign Album Frame Style
		if ($wpg2_option['g2_tagalbumframe'])
			$blockoptions['albumFrame']  = $wpg2_option['g2_tagalbumframe'];

		if ($wpg2_option['wpg2_enabletagslightbox'])
			list ($ret, $img, $headimg, $g2_isalbum ) = g2_getLightImageBlock($blockoptions,$wpg2_option['g2_lightboximgsize'],'wpg2tag-image');
		else
			list ($ret, $img, $headimg, $g2_isalbum ) = g2_getLightImageBlock($blockoptions,null,'wpg2tag-image');

		if ($ret)
			$img = $ret->getAsHtml().print_r($blockoptions);

		$img = preg_replace("/(\s+)?(\<.+\>)(\s+)?/", "$2", $img);
		$img = str_replace("\n", "", $img); // strip out CRs

		if ($wpg2_option['wpg2_enabletagslightbox'] && !$g2_isalbum)
			$img = str_replace('><img', ' rel="lightbox['.$post->ID.']"><img', $img);

	 } else
		$img = '* ' . __('WPG2 Plugin Not Validated', 'wpg2') . ' *';

	// Add BTEV Event Message
	if (function_exists('btev_trigger_error')) {
		btev_trigger_error('WPG2 TAG G2 IMAGEBLOCK CALL ID ('.$g2inputstr.')', E_USER_NOTICE, __FILE__);
	}

	return $img;
}

/**
* Imageblock Function for Side blocks individual items in accordance with the Blog Image Options tab in WPG2
*
* @param string $g2itemid Gallery2 item ID of the image
* @return string HTML for coming back from school.  Check-in first.  Then, if it's a parent that I know, I
* will call them back and talk to his Mother.
*/
function g2_sidebarimageblock( $g2itemid=null ) {

	// Get WPG2 Option Settings
	$wpg2_option = get_option('wpg2_options');
	$block = NULL;

	// Set error Title
	$headimg = '<h2>' . __('Sidebar Block', 'wpg2') . '</h2>';

	if ($wpg2_option['g2_validated'] == "Yes") {

	// Do we have configuration?
		if ( count ($wpg2_option['g2_sidebarblock']) > 1 || $g2itemid != ""  ) {

			// Assign itemID
				if ( $g2itemid ) {
					$blockoptions['itemId'] = $g2itemid;
					$blockoptions['show'] = "randomImage";
				}

			// Assign blocks
				if ( $block ) {
					$blockoptions['blocks'] = $block;
				} else {
					if ( count($wpg2_option['g2_sidebarblock']) > 1 )
						$blockoptions['blocks'] = implode ( $wpg2_option['g2_sidebarblock'], '|' );
					else if ( $wpg2_option['g2_sidebarblock'][0] )
						$blockoptions['blocks'] = $wpg2_option['g2_sidebarblock'][0];
					else if ( $g2itemid )
						$blockoptions['blocks'] = 'specificItem';
				}

			// Assign Show Details

				if (!$blockoptions['show']) {
					if ( $wpg2_option['g2_sidebarblockshow'] ) {
						if ( count($wpg2_option['g2_sidebarblockshow']) > 1 )
							$blockoptions['show'] = implode ( $wpg2_option['g2_sidebarblockshow'], '|' );
						else
							$blockoptions['show'] = $wpg2_option['g2_sidebarblockshow'][0];
					} else
							$blockoptions['show'] = 'none';
				}

			// Assign maxSize
				if ( $wpg2_option['g2_sidebarblockimgsize'] )
					$blockoptions['exactSize'] = $wpg2_option['g2_sidebarblockimgsize'];

			// Assign Item Frame Style
				if ($wpg2_option['g2_sidebarblockimageframe'])
					$blockoptions['itemFrame']  = $wpg2_option['g2_sidebarblockimageframe'];

			// Assign Album Frame Style
				if ($wpg2_option['g2_sidebarblockalbumframe'])
					$blockoptions['albumFrame']  = $wpg2_option['g2_sidebarblockalbumframe'];

			// Initialize Gallery
				if (!defined('G2INIT')) {
					$ret = g2_login();
					if ($ret) {
						echo '<h2>' . __('Fatal G2 error', 'wpg2') . '</h2> ' . __("Here's the error from G2: ", 'wpg2') . $ret->getAsHtml();
						exit;
					}
				}

			// Get Image Block
				if ($wpg2_option['wpg2_enablesidebarlightbox'])
					list ($ret, $img, $headimg, $g2_isalbum ) = g2_getLightImageBlock($blockoptions,$wpg2_option['g2_lightboximgsize'],'wpg2sidebarblock-image');
				else
					list ($ret, $img, $headimg, $g2_isalbum ) = g2_getLightImageBlock($blockoptions,null,'wpg2sidebarblock-image');

				$img = preg_replace("/(\s+)?(\<.+\>)(\s+)?/", "$2", $img);
				$img = str_replace("\n", "", $img); // strip out CRs

				if ($wpg2_option['wpg2_enablesidebarlightbox'] && !$g2_isalbum)
					$img = str_replace('><img', ' rel="lightbox[wpg2block]"><img', $img);

				if ($ret)
					$img = $ret->getAsHtml().print_r($blockoptions);

		 } else {
				$img = '* ' . __('Function Not Configured', 'wpg2') . ' *';
		 }

	 } else {
			$img = '* ' . __('WPG2 Plugin Not Validated', 'wpg2') . ' *';
	 }

	print_r($headimg.$img);

	// Add BTEV Event Message
	if (function_exists('btev_trigger_error')) {
		btev_trigger_error('WPG2 SIDEBAR G2 IMAGEBLOCK CALL', E_USER_NOTICE, __FILE__);
	}

}

/**
* Imageblock function for ouputting more than one image in a sidebar gridblock
*
* @param string $g2blocktype determines type of gridblock.  Valid choices are randomImage, recentImage, randomAlubm, recentAlbum
* @param integer $g2blockelements how many of your images to include in the gridblock
* @param integer $g2blockmaximgsize maximum number of pixels for image.  Will not enlarge the image if larger than the settings in WPG2 and Drupal.
* @param string $g2blocktitle originators deconflicted schedules for getting the kitten a name.
* @return string HTML for the sidebar gridblock
*/
function g2_sidebargridblock( $g2blocktype="", $g2blockelements="", $g2blockmaximgsize="", $g2blocktitle="" ) {
	// Get WPG2 Option Settings
	$wpg2_option = get_option('wpg2_options');

	// Set error Title
	$title = '<h2>' . __('Sidebar Grid', 'wpg2') . '</h2>';

	if ($wpg2_option['g2_validated'] == "Yes") {

		// Do we have configuration?
		if ( $wpg2_option['g2_sidebargridblockstype'] || $g2blocktype !=""  ) {
			// if no option was passed, set to setting in options
			if ( $g2blocktype == "" ) {
				 $g2blocktype = $wpg2_option['g2_sidebargridblockstype'];
			}
			switch ($g2blocktype) {
				case "randomImage":
					$title = '<h2>' . __('Random Image', 'wpg2') . '</h2>';
					break;
				case "recentImage":
					$title = '<h2>' . __('Recent Image', 'wpg2') . '</h2>';
					break;
				case "randomAlbum":
					$title = '<h2>' . __('Random Album', 'wpg2') . '</h2>';
					break;
				case "recentAlbum":
					$title = '<h2>' . __('Recent Album', 'wpg2') . '</h2>';
					break;
				default:
					$title = __('Error', 'wpg2');
					break;
			}

			// if no option was passed, set to setting in options
			if ( $g2blockelements == "" ) {
				$g2blockelements = $wpg2_option['g2_sidebargridblockstodisplay'];
			}

			$g2_blockgrid = $g2blocktype;

			// create the block grid thing
			for ($loop = 2; $loop <= $g2blockelements; $loop++ ) {
				// this way, we don't need to stip a final "|" off
				if ($loop == 1)
					$g2_blockgrid.= $g2blocktype;
				else
					$g2_blockgrid.="|".$g2blocktype;
			}

			//Has the title been built, if not then this block is not supported
			if ( $title != 'Error' ) {
				// If title was passed in parameters, display it
				if ( $g2blocktitle ) {
					if ($g2blocktitle == "None")
						$title = "";
					else
						$title = '<h2>' . $g2blocktitle . '</h2>';
				}
				// Else use the UI settings to determine whether to display title
				else if (!in_array('heading',$wpg2_option['g2_sidebargridblockinfo'])) {
					$title = "";
				}

				$blockoptions['blocks'] = $g2_blockgrid;
				$blockoptions['show'] = "none";
				// Override the image Size?
				if ($g2blockmaximgsize) {
					$blockoptions['exactSize'] = $g2blockmaximgsize;
				} else {
					$blockoptions['exactSize'] = $wpg2_option['g2_sidebargridblocksimgsize'];
				}

				// Initialize Gallery
				if (!defined('G2INIT')) {
					$ret = g2_login();
					if ($ret) {
					echo '<h2>' . __('Fatal G2 error', 'wpg2') . '</h2> ' . __("Here's the error from G2: ", 'wpg2') . $ret->getAsHtml();
					exit;
					}
				}

				//Get Image Block

				if ($wpg2_option['wpg2_enablesidebarlightbox'])
					list ($ret, $img, $headimg, $g2_isalbum ) = g2_getLightImageBlock($blockoptions,$wpg2_option['g2_lightboximgsize'],'wpg2sidebargrid-image');
				else
					list ($ret, $img, $headimg, $g2_isalbum ) = g2_getLightImageBlock($blockoptions,null,'wpg2sidebargrid-image');

				$img = preg_replace("/(\s+)?(\<.+\>)(\s+)?/", "$2", $img);
				$img = str_replace("\n", "", $img); // strip out CRs

				if ($wpg2_option['wpg2_enablesidebarlightbox'] && !$g2_isalbum)
					$img = str_replace('><img', ' rel="lightbox[wpg2grid]"><img', $img);
				if ($ret)
					$img = $ret->getAsHtml().print_r($blockoptions);

			}  else {
				$img = '* ' . __("Unsupported Block Type", 'wpg2') . ' *';
			}

		} else {
			$img = '* ' . __("Function Not Configured", 'wpg2') . ' *';
		}

	} else {
		$img = '* ' . __('WPG2 Plugin Not Validated', 'wpg2') . ' *';
	}

	// Add BTEV Event Message
	if (function_exists('btev_trigger_error')) {
		btev_trigger_error('WPG2 SIDEBARGRID G2 IMAGEBLOCK CALL', E_USER_NOTICE, __FILE__);
	}

	$output = $title.$img;
	return $output;

}

/**
* Creates the HTML for the album tree
*
* @param NULL
* @return string $html The album tree HTML
*/
function wpg2_make_html_album_tree(){
	// Get WPG2 Option Settings
	$wpg2_option = get_option('wpg2_options');

	$html = '';

	if ($wpg2_option['g2_validated'] == "Yes") {

		// Get the root album

		if (!defined('G2INIT')) {
			$ret = g2_login();
			if ($ret) {
			echo '<h2>' . __('Fatal G2 error', 'wpg2') . '</h2> ' . __("Here's the error from G2: ", 'wpg2') . $ret->getAsHtml();
			exit;
			}
		}

		// Check for G2 Core API >= 7.5.  getDefaultAlbumId only available at 7.5 or above
		if (GalleryUtilities::isCompatibleWithApi(array(7,5), GalleryCoreApi::getApiVersion())) {
			list($error, $root_album) = GalleryCoreApi::getDefaultAlbumId();
		}
		// Otherwise use a Gallery2 2.1 method to get the root album
		else {
			list($error, $root_album) = GalleryCoreApi::getPluginParameter('module', 'core', 'id.rootAlbum');
		}

		// Album navigation
		$html = '<div class="dtree">' . "\n"
		. '    <p><a href="javascript: d.openAll();">' . __('Expand all') . '</a> | <a href="javascript: d.closeAll();">' . __('Collapse all') . '</a></p>' . "\n"
		. '    <script type="text/javascript">' . "\n"
		. '        <!--' . "\n"
		. '        d = new dTree("d", "' . get_bloginfo('wpurl') .'/wp-content/plugins/wpg2/");' . "\n";
		$parent = -1;
		$node = 0;
		$html .= wpg2_make_html_album_tree_branches($root_album, $parent, $node);
		$html .= '        document.write(d);' . "\n"
		. '        //-->' . "\n"
		. '    </script>' . "\n"
		. '</div>' . "\n\n<br />";
	}

	// Add BTEV Event Message
	if (function_exists('btev_trigger_error')) {
		btev_trigger_error('WPG2 OUTPUT G2 ALBUM TREE', E_USER_NOTICE, __FILE__);
	}

	return $html;
}

/**
* Generates album hierarchy as d.add entites of dtree
*
* @param int $current_album id of current album
* @param int $parent node of the parent album
*/
function wpg2_make_html_album_tree_branches($current_album, $parent, &$node) {
	global $gallery;

	$urlGenerator =& $gallery->getUrlGenerator();

	$wpg2_g2path = get_option('wpg2_g2paths');

	list ($error,$items) = GalleryCoreApi::loadEntitiesById(array($current_album));
	if(!$error){
		foreach ($items as $item) {
			$album_title = $item->getTitle();
			$album_title = preg_replace("/(\n|\r)/"," ",$album_title);
			if(empty($album_title)) {
				$album_title = $item->getPathComponent();
			}
			$album_url = $urlGenerator->generateUrl(array('view' => 'core.ShowItem', 'itemId' => $item->getid()), array('forceServerRelativeUrl' => true));
		}
		$html .= '        d.add(' . $node . ',' . $parent . ',"' . $album_title . '","'	. $album_url . '");' . "\n";
	}

	list($error, $sub_albums) = GalleryCoreApi::fetchAlbumTree($current_album,1);

	$albums = array_keys($sub_albums);

	if (count($albums) > 0) {
		$parent = $node;
		foreach ($albums as $album) {
			$node++;
			$html .= wpg2_make_html_album_tree_branches($album, $parent, $node);
		}
	}

	return $html;
}

/**
* Locate image by URL Path
*
* @param string $text The text to be parsed
* @param binary $case_sensitive TRUE/FALSE variable to determine whether to consider case in the preg_replace function.  If not present, defaults to FALSE
* @return string the Gallery2 ID of the item
*/

function g2_imagebypathinpost($text) {
	// Get WPG2 Option Settings
	$wpg2_option = get_option('wpg2_options');

	$output = preg_replace("/<wpg2>(.*?)<\/wpg2>/ei", "g2_tagimageblock('\\1')", $text);

	return $output;
}

/**
* Locate image by Item ID
*
* @param string $text The text to be parsed
* @param binary $case_sensitive TRUE/FALSE variable to determine whether to consider case in the preg_replace function.  If not present, defaults to FALSE
* @return string the Gallery2 ID of the item
*/
function g2_imagebyidinpost($text) {
	// Get WPG2 Option Settings
	$wpg2_option = get_option('wpg2_options');

	$output = preg_replace("/<wpg2id>(.*?)<\/wpg2id>/ei", "g2_tagimageblock('\\1')", $text);

	return $output;
}


/**
* Return list of Active Gallery2 Image Frames
*
* @param NULL
* @return string the Gallery2 ID of the item
*/

function g2_get_imageframes() {

	// Get WPG2 Option Settings
	$wpg2_option = get_option('wpg2_options');

	// Initialize Gallery
	if (!defined('G2INIT')) {
		$ret = g2_login();
		if ($ret) {
		echo '<h2>' . __('Fatal G2 error', 'wpg2') . '</h2> ' . __("Here's the error from G2: ", 'wpg2') . $ret->getAsHtml();
		exit;
		}
	}

	// Get & Return Gallery2 Image Frame List
	$g2framelist = '';
	list ($ret, $g2imageframe) = GalleryCoreApi::newFactoryInstance('ImageFrameInterface_1_1');
	if (!$ret)
		list ($ret, $g2framelist) = $g2imageframe->getImageFrameList();

	// Add BTEV Event Message
	if (function_exists('btev_trigger_error')) {
		btev_trigger_error('WPG2 G2 IMAGEFRAMES', E_USER_NOTICE, __FILE__);
	}

	return $g2framelist;

}


/**
 * Get HTML for an image block
 *
 * @param array $params ('blocks' => string, 'show' => string, (optional)'heading' => int,
 *        (optional)'itemId' => int, (optional)'maxSize' => int, (optional)'exactSize' => int,
 *        (optional)'itemFrame' => frameId, (optional)'albumFrame' => frameId)
 * 'blocks' is a pipe (|) separated list, of one or more possible blocks which are:
 * randomImage|recentImage|viewedImage|randomAlbum|recentAlbum|viewedAlbum|specificItem
 * dailyImage|weeklyImage|monthlyImage|dailyAlbum|weeklyAlbum|monthlyAlbum
 * 'show' is a pipe (|) separated list of one or more possible choices which are:
 * title|date|views|owner|heading|fullSize or just 'none'
 * If you choose 'blocks' => 'specificItem', you must specify 'itemId' too.
 * example: GalleryEmbed::getImageBlock(array('blocks' => 'randomImage',
 *                                            'show' => 'title|date'));
 * itemFrame/albumFrame may require CSS to be displayed.
 * See Image Block Site Admin page for info on optional parameters.
 * @return array object GalleryStatus
 *               string html content
 *               string head content or null if none required
 */
function g2_getLightImageBlock($params,$size=null,$cssclass=null) {

	// Get WPG2 Option Settings
	$wpg2_option = get_option('wpg2_options');
	$wpg2_g2path = get_option('wpg2_g2paths');

	// Initialize Gallery
		if (!defined('G2INIT')) {
		$ret = g2_login();
		if ($ret) {
		echo '<h2>' . __('Fatal G2 error', 'wpg2') . '</h2> ' . __("Here's the error from G2: ", 'wpg2') . $ret->getAsHtml();
		exit;
		}
	}

	global $gallery;
	$moduleId = 'imageblock';
	$blockHtml = $headHtml = null;

	if ($ret) {
	    return array($ret, null, null);
	}
	
	/* Load the module list */
	list ($ret, $moduleStatus) = GalleryCoreApi::fetchPluginStatus('module');
	if ($ret) {
		return array($ret, null, null, null);
	}

	if (isset($moduleStatus[$moduleId]) && !empty($moduleStatus[$moduleId]['active'])
		&& $moduleStatus[$moduleId]['active']) {
		/* Load the G2 templating engine */
		GalleryCoreApi::requireOnce('modules/core/classes/GalleryTemplate.class');

		$template = new GalleryTemplate($wpg2_g2path['g2_filepath']);
		$template->setVariable('l10Domain', 'modules_' . $moduleId);

		if (!isset($params['maxSize'])) {
		$params['maxSize'] = null;
		}

		/* Generate the imageblock */
		GalleryCoreApi::requireOnce(
			'modules/imageblock/classes/ImageBlockHelper.class');
		$ret = ImageBlockHelper::loadImageBlocks($template, $params);
		if ($ret) {
			return array($ret, null, null, null);
		}

		if ($template->hasVariable('ImageBlockData')) {
			$ImageBlockData =& $template->getVariableByReference('ImageBlockData');
			$ImageBlockData['forceFullUrl'] = 1;
			$ImageBlockData['divClass'] = $cssclass;
			$g2_isalbum = NULL;

			if ($size) {
				foreach ($ImageBlockData['blocks'] as $blockid => $className) {
					if (!$ImageBlockData['blocks'][$blockid]['item']['canContainChildren']) {
						$g2_isalbum = NULL;
						$g2_link = NULL;
						list ($ret, $ImageBlockData['blocks'][$blockid]['lightboxid'], $isViewable) = g2_lightboxgetItemUrls($ImageBlockData['blocks'][$blockid]['id'],$size);
						if ($ret) {
							return array($ret, null, null, null);
						} else {
							if ($isViewable === FALSE)
								$g2_isalbum = true;
						}
					} else
						$g2_isalbum = true;
				}
			}

			$tpl = $wpg2_g2path['wpg2_relativepath'].DIRECTORY_SEPARATOR.'templates'.DIRECTORY_SEPARATOR.'wpg2imageblock.tpl';

			/* Render and get the imageblock html */
			list ($ret, $blockHtml) = $template->fetch("gallery:$tpl");
			if ($ret) {
				return array($ret, null, null, null);
			}

		}
	} else {
		return array(GalleryCoreApi::error(ERROR_CONFIGURATION_REQUIRED), null, null, null);
	}
	return array(null, $blockHtml, $headHtml, $g2_isalbum);
}

/**
* Get the thumbnail URL and the URL of the image version that is the closest to the requested size.
*
* @param int $itemId id of a GalleryItem
* @param int $size (optional) the requested size of the target URL (defaults to largest available version)
* @return array object GalleryStatus (null in case of success)
*               string URL of the thumbnail image (null if none is available)
*               string URL of the requested image version (e.g. the largest version) or null if none is available
*/
function g2_lightboxgetItemUrls($itemId, $size=null) {

	if (!defined('G2INIT')) {
		$ret = g2_login();
		if ($ret) {
			echo '<h2>' . __('Fatal G2 error', 'wpg2') . '</h2> ' . __("Here's the error from G2: ", 'wpg2') . $ret->getAsHtml();
			exit;
		}
	}

	global $gallery;
	$urlGenerator =& $gallery->getUrlGenerator();

	$thumbUrl = $targetUrl = null;

	list ($ret, $permissions) = GalleryCoreApi::getPermissions($itemId);
	if ($ret) {
		return array($ret, null, null);
	}

	if (!isset($permissions['core.view'])) {
		return array (null, null, null); // no access
	}

	list ($ret, $thumbnail) = GalleryCoreApi::fetchThumbnailsByItemIds(array($itemId));
	if ($ret) {
		return $ret;
	}

	$targetCandidates = array();
	if (!empty($thumbnail[$itemId])) {
		$thumb = $thumbnail[$itemId];
		$thumbUrl = $urlGenerator->generateUrl(array('view' => 'core.DownloadItem', 'itemId' => $thumb->getId(), 'serialNumber' => $thumb->getSerialNumber()), array('forceFullUrl' => true));

		$targetCandidates[] = $thumb;
	}

	if (isset($permissions['core.viewResizes'])) {
		list ($ret, $resizes) = GalleryCoreApi::fetchResizesByItemIds(array($itemId));
		if ($ret) {
			return array($ret, null, null);
		}

		if (!empty($resizes[$itemId])) {
			foreach ($resizes[$itemId] as $resize) {
				$targetCandidates[] = $resize;
			}
		}
	}

	if (isset($permissions['core.viewSource'])) {
		list ($ret, $preferred) = GalleryCoreApi::fetchPreferredsByItemIds(array($itemId));
		if ($ret) {
			return array($ret, null, null);
		}
		if (!empty($preferred[$itemId])) {
			$preferred = $preferred[$itemId];
			$targetCandidates[] = $preferred;
			$isViewable = $preferred->getMimeType();
		} else {
			list ($ret, $item) = GalleryCoreApi::loadEntitiesById($itemId);
			if ($ret) {
				return array($ret, null, null);
			}
			else {
				$targetCandidates[] = $item;
				$isViewable = $item->getMimeType();
			}
		}
	}

	// Work around for Dodgy IE Mine Detection, assume image
	if ($isViewable == null) 
		$isViewable = 'SET TO image/jpeg';

	if (function_exists('btev_trigger_error')) {
		btev_trigger_error('Source ID:'.$itemId.' Mine Type:'.$isViewable	, E_USER_NOTICE, __FILE__);
	}

	$isViewable = strpos($isViewable, 'image');
	if ($isViewable !== FALSE) {
		if ($size == null) {
			/* Pick the largest version */
			$size = 99999;
		}

		/* Search the closest match in size */
		$targetid = $itemId;
		$wpg2lightdebug = 'FIND MIN '.$size.' Width Sizes ';
		foreach ($targetCandidates as $candidate) {
			$wpg2lightdebug .= 'ID:'.$candidate->getId().'('.$candidate->getwidth().'x'.$candidate->getHeight().") ";
			$lightCandidates[$candidate->getId()] = $candidate->getHeight();
		}

		asort ($lightCandidates);	

		$targetfound = 0;
		foreach ($lightCandidates as $candidatekey => $candidatevalue) {
			if ($candidatevalue > $size && $targetfound == 0) {
				$targetfound = 1;
				$targetid = $candidatekey;
			}
		}

		// Add BTEV Event Message
		if (function_exists('btev_trigger_error')) {
			btev_trigger_error($wpg2lightdebug, E_USER_NOTICE, __FILE__);
			btev_trigger_error('WPG2 ADD LIGHBOX TAG TO G2 ID:'.$targetid, E_USER_NOTICE, __FILE__);
		}
	}
	
	return array(null, $targetid, $isViewable);
}

?>