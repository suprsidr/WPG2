===WPG2===
Contributors: ozgreg, capt_kirk, suprsidr
Donate link: http://gallery.menalto.com/donate?donate_tag=website_footer
Tags: gallery2, wpg2, photos, photo albums, images, video, widget, lightbox
Requires at least: 2.5.0
Tested up to: 2.6
Stable tag: 3.0.7

WPG2 is a Wordpress Plug-in that embeds Gallery2 within Wordpress to share photos, videos and any other Gallery2 content seamlessly into Wordpress.

== Description ==

WPG2 is a Wordpress Plug-in that embeds Gallery2 within Wordpress to share photos, videos and any other Gallery2 content seamlessly into the Wordpress Sidebar and Blog entries.

###WPG2 Features###
* G2Image insertion tool in your Edit Toolbar to help you select one or more Gallery 2 Images, Videos, Albums (displays the Album Highlight Image) in your Blog Posts or Wordpress Pages
* Random, Recent, Daily, Weekly, Popular Images and/or Albums can be displayed as widgets in the Wordpress Sidebar
* Single Signon of Wordpress Users into Gallery2, with the ability to restrict Gallery2 Sign on through Wordpress User Roles and Capabilities
* Display your Gallery2 Page(s) from within the automatically created Wordpress Page.
* Add Lightbox Effects for your Gallery2 Images

###Release Notes###
Please see the [WPG2 Release Notes](http://codex.gallery2.org/Integration:WPG2_Release_Notes) for a detailed list of changes.

###Known Issues###
Changes made to the way Wordpress hashes it's passwords in 2.5 are not compatible with Gallery2 2.2 (Will be fixed in Gallery2 2.3)  WPG2 can only make the Wordpress password compatible with Gallery2 after you have logged off the wordpress user and logged back in.
HTML Tags in Gallery Album Descriptions and Titles can cause Lightbox to break and or the ALT tag to incorrectly display.  Try to avoid HTML tags until WPG2 3.1   

###WPG2 Support###
Please post any requests for WPG2 core program support at the [WPG2 Plugin Support Forum](http://gallery.menalto.com/forum/83).
Please post any requests for WPG2 theme/CSS/layout support at the [WPG2 CSS/Layout Support Forum](http://gallery.menalto.com/forum/84).

###Manual###
[Codex / Documentation](http://codex.gallery2.org/Integration:WPG2)

== Installation ==

###File Installation###
1. Unzip the files.
2. FTP the contents of the "/wpg2/" directory to a new folder called /wpg2/ (in lowercase) in your WordPress plugins directory, /wordpress/wp-content/plugins/
3. Activate & Validate the WPG2 plugin (See below).

###Requirements###
1. To use the "WPG2" plugin, you must have Wordpress & Gallery2 installed and properly configured
2. Running at least Wordpress 2.5 and Gallery2 2.2
3. Wordpress & Gallery2 may be on different subdomains but NOT on different servers.
4. Wordpress & Gallery2 should be installed on the same database but can be different databases
5. Only one WordPress Blog tied to one Gallery2 Gallery
  * Unless you only have one identical user on multiple blogs, tying multiple blogs to one gallery will cause user errors because there is no way to currently distinguish one blog's users from another blog's users within Gallery2.
6. PHP memory limit of 16MB or better.
7. Browser that supports Javascript
8. Gallery2 Imageblock (version 1.0.9 or greater) and ImageFrame Plugins installed & Activated.

###Upgrading###
NOTE: If you are upgrading from any WPG2 version deactivate WPG2 on the WordPress Plugins Management Panel, prior to upgrading.

* If you are upgrading from WPG2 version 1.0, delete all of the files in the wp-content/plugins/wp-gallery2/ directory.
* If you are upgrading from WPG2 version 2.0, delete all of the files in the wp-includes/js/tinymce/plugins/g2image/ directory.
* If you are upgrading from WPG2 version 2.0, delete wp-gallery2.php from same directory that wp-config.php is in.

== Getting Started ==

###Activation##
Activate WPG2 in the WordPress Plugins Management Panel.

###Validatation###
* You will now have a "WPG2" options tab on the main WordPress Admin Menu Bar.
* Select it. The first tab is a WPG2 validation tab.  Validaton should occur automatically.
* If you have any errors, follow the directions in the error message to resolve any issues.
	* First try going to the G2 Paths tab, and enter your Gallery2 URL in the "Auto Configuration of Embedded Paths - Gallery2 URL" text field and press the "Auto Configure" button. This should result in a valid setup.
	* If you cannot get the auto configuration to work, go to the G2 Paths tab and enter the path information manually in the "Manually Configure/Adjust Embedded Paths" section. Guidance on each field is provided immediately below the field.
	This will happen in the rare case of using Gallery2 on a different subdomain on a clustered server arrangement where the computer hosting your site has a different IP address than the IP used in the HTTP request.

###Verify Gallery2 User Account Setup###
* Once WPG2 is validated, there will be a "Gallery2 Users" tab under the WordPress "Users" tab.
* Verify (and if missing, Grant) all of the Wordpress users who should have Gallery2 Admin privileges.
* Verify (and if missing, Grant) any other users who should be logged in as a Gallery2 user (but not admin).
* Any Wordpress users who do not have a corresponding Gallery2 account will appear in the list of WordPress users without Gallery2 user accounts.
	These lists will only appear if there is one or more user in them.

###Additional Language Support###
* Version 3.0 onwards has shipped with support for English (en), German (de), Spanish (es), French (fr), Hungarian (hu), Italian (it), Korean (ko), Norwegian Bokmal (nb), Dutch (nl), Polish (pl), and Chinese Traditional (zh_TW) for the G2Image popup windows. Check the WPG2 support forum and the G2Image homepage for more translations.
* WPG2 Admin Tabs: If you use poEdit to translate /wpg2/locale/wpg2.pot into your language's wpg2-xx.po and wpg2-xx.mo files, you can use them by placing them in the /locale/ folder. They must have the same encoding and name as the WordPress locale file as set by WPLANG in wp-config.php.
    Example: The WordPress Traditional Chinese localization is encoded in UTF-8 and are named zh_TW.po and zh_TW.mo. The corresponding WPG2 localizations are also in UTF-8 and are named wpg2-zh_TW.po and wpg2-zh_TW.mo.
* G2Image Pop-Up Windows: Please see the G2Image Internationization page for information on G2Image language support - http://g2image.steffensenfamily.com/index.php?title=Internationalization/Languages_Page
* Please help translate WPG2 core files and G2Image on Launchpad: https://translations.launchpad.net/wpg2/trunk/+pots/wpg2 and https://translations.launchpad.net/g2image/trunk/+pots/g2image
* Please share your translations by posting them on the WPG2 support forum.

== Frequently Asked Questions ==
####Q: Where Can I Get Help?####
[Codex / Documentation](http://codex.gallery2.org/Integration:WPG2)
[Support Forums](http://gallery.menalto.com/forum/83)
[Support FAQ](http://codex.gallery2.org/Integration:WPG2_FAQ)

####Q:How come the WPG2 Output page just returns blank?####
Most common cause of this problem is you do not have enough php memory allocated.  Gallery2 requires 16MB to run and you can increase the memory allocation by adding the following line to your .htaccess in your wordpress directory
`php_value memory_limit 16M`

####Q: How can I change the URL of the embedded WPG2 page?####
You can change the URL by changing the "Page Slug" under Manage -> Pages -> Edit for the embedded page. You can also change the "Page Title" while you're there, if you like.
If you have Gallery2 Template Caching turned on you will need to flush your Gallery2 Template Cache to force the G2 to regenerate its links, you can do this via the Gallery2 Site Admin -> Maintenance Menu -> Delete Template Cache.

####Q: How can I redirect my old wp-gallery2.php to my new location####
If you would like to redirect your old wp-gallery2.php links to your new embedded page, add the following at the top of your WordPress-generated .htaccess file:

 Redirect 301 /wp-gallery2.php http://www.yourdomain.com/wpg2

== Screenshots ==

[Embedded Album](http://wpg2.galleryembedded.com/index.php?title=WPG2:Screenshots_of_a_Gallery2_Embedded_Page_1)

[Gallery2 Main Page](http://wpg2.galleryembedded.com/index.php?title=WPG2:Screenshots_of_a_Gallery2_Embedded_Page_2)

[Wordpress Page](http://wpg2.galleryembedded.com/index.php?title=WPG2:Screenshots_of_a_Gallery2_Embedded_Page_3)

[Blog Entry](http://wpg2.galleryembedded.com/index.php?title=WPG2:Screenshots_of_a_Gallery2_Embedded_Page_4)