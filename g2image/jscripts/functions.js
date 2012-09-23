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

function activateInsertButton() {
	var obj = document.forms[0];
	var checked = 0;

	if (obj.images.length) {
		loop = obj.images.length;
		for (var i=0;i<loop;i++) {
			if (obj.images[i].checked) {
				checked++;
			}
		}
	}
	else {
		if (obj.images.checked) {
			checked = 1;
		}
	}
	if (checked) {
		document.forms[0].insert_button.disabled = false;
	}
	else {
		document.forms[0].insert_button.disabled = true;
	}
}

function checkAllImages() {
	var obj = document.forms[0];

	if (obj.images.length) {
		loop = obj.images.length;
		for (var i=0;i<loop;i++) {
			obj.images[i].checked = true;
		}
	}
	else {
		obj.images.checked = true;
	}
	document.forms[0].insert_button.disabled = false;
}

function uncheckAllImages() {
	var obj = document.forms[0];

	if (obj.images.length) {
		loop = obj.images.length;
		for (var i=0;i<loop;i++) {
			obj.images[i].checked = false;
		}
	}
	else {
		obj.images.checked = false;
	}
	document.forms[0].insert_button.disabled = true;
}

function toggleTextboxes() {
	var obj = document.forms[0];

	if (obj.imginsert.value == 'thumbnail_custom_url')
		document.getElementsByName('custom_url_textbox')[0].className = 'displayed_textbox';
	else
		document.getElementsByName('custom_url_textbox')[0].className = 'hidden_textbox';

	if ((obj.imginsert.value == 'link_image') || (obj.imginsert.value == 'link_album'))
		document.getElementsByName('link_text_textbox')[0].className = 'displayed_textbox';
	else
		document.getElementsByName('link_text_textbox')[0].className = 'hidden_textbox';

	if (obj.imginsert.value == 'thumbnail_lightbox')
		document.getElementsByName('lightbox_group_textbox')[0].className = 'displayed_textbox';
	else
		document.getElementsByName('lightbox_group_textbox')[0].className = 'hidden_textbox';

	if (obj.g2ic_wpg2_valid.value == true) {
		if (obj.imginsert.value == 'wpg2_image')
			document.getElementsByName('wpg2_tag_size_textbox')[0].className = 'displayed_textbox';
		else
			document.getElementsByName('wpg2_tag_size_textbox')[0].className = 'hidden_textbox';
	}

	if (obj.drupal_g2_filter.value == true) {
		if (obj.imginsert.value == 'drupal_g2_filter')
			document.getElementsByName('drupal_exactsize_textbox')[0].className = 'displayed_textbox';
		else
			document.getElementsByName('drupal_exactsize_textbox')[0].className = 'hidden_textbox';
	}
}

function insertAtCursor(myField, myValue) {
	//IE support
	if (document.selection && !window.opera) {
		myField.focus();
		sel = window.opener.document.selection.createRange();
		sel.text = myValue;
	}
	//MOZILLA/NETSCAPE/OPERA support
	else if (myField.selectionStart || myField.selectionStart == '0') {
		var startPos = myField.selectionStart;
		var endPos = myField.selectionEnd;
		myField.value = myField.value.substring(0, startPos)
		+ myValue
		+ myField.value.substring(endPos, myField.value.length);
	}
	else {
		myField.value += myValue;
	}
}

function showFileNames(){
	divs = document.getElementsByTagName('div');
	for (var i = 0; i < divs.length; i++) {
		if (divs[i].className == 'hidden_title')
			divs[i].className = 'displayed_title';
		else if (divs[i].className == 'thumbnail_imageblock')
			divs[i].className = 'title_imageblock';
		else if (divs[i].className == 'inactive_placeholder')
			divs[i].className = 'active_placeholder';
	}
}

function showThumbnails(){
	divs = document.getElementsByTagName('div');
	for (var i = 0; i < divs.length; i++) {
		if (divs[i].className == 'displayed_title')
			divs[i].className = 'hidden_title';
		else if (divs[i].className == 'title_imageblock')
			divs[i].className = 'thumbnail_imageblock';
		else if (divs[i].className == 'active_placeholder')
			divs[i].className = 'inactive_placeholder';
	}
}

function insertHtml(html,form) {
	g2ic_form=form.g2ic_form.value;
	g2ic_field=form.g2ic_field.value;

	if(window.tinyMCE)
		window.opener.tinyMCE.execCommand("mceInsertContent",true,html);
	else if (window.opener.FCK)
		window.opener.FCK.InsertHtml(html);
	else
		insertAtCursor(window.opener.document.forms[g2ic_form].elements[g2ic_field],html);
	window.close();
}

function insertItems(){
	var obj = document.forms[0];
	var htmlCode = '';
	var imgtitle = '';
	var imgalt = '';
	var loop = '';
	var item_summary = new Array();
	var item_title = new Array();
	var item_description = new Array();
	var image_url = new Array();
	var thumbnail_src = new Array();
	var fullsize_img = new Array();
	var thumbw = new Array();
	var thumbh = new Array();
	var image_id = new Array();

	//hack required for when there is only one image

	if (obj.images.length) {
		loop = obj.images.length;
		for (var i=0;i<loop;i++) {
			image_id[i] = obj.image_id[i].value;
			item_title[i] = obj.item_title[i].value;
			item_summary[i] = obj.item_summary[i].value;
			item_description[i] = obj.item_description[i].value
			image_url[i] = obj.image_url[i].value;
			fullsize_img[i] = obj.fullsize_img[i].value;
			thumbnail_src[i] = obj.thumbnail_src[i].value;
			thumbw[i] = obj.thumbw[i].value;
			thumbh[i] = obj.thumbh[i].value;
		}
	}
	else {
		loop = 1;
		image_id[0] = obj.image_id.value;
		item_title[0] = obj.item_title.value;
		item_summary[0] = obj.item_summary.value;
		item_description[0] = obj.item_description.value
		image_url[0] = obj.image_url.value;
		thumbnail_src[0] = obj.thumbnail_src.value;
		fullsize_img[0] = obj.fullsize_img.value;
		thumbw[0] = obj.thumbw.value;
		thumbh[0] = obj.thumbh.value;
	}

	//let's generate HTML code according to selected insert option

	for (var i=0;i<loop;i++) {
		if ((loop == 1) || obj.images[i].checked) {

			imgtitle = ' title="' + item_summary[i] + '"';
			imgalt = ' alt="' + item_title[i] + '"';
			thumbw[i] = 'width="' + thumbw[i] + '" ';
			thumbh[i] = 'height="' + thumbh[i] + '" ';

			switch(obj.imginsert.value){
				case 'thumbnail_image':
					if ((obj.alignment.value != 'none') && (obj.class_mode.value == 'div')){
						htmlCode += '<div class="' + obj.alignment.value + '">';
					}
					htmlCode += '<a href="' + image_url[i]
					+ '"><img src="'+ thumbnail_src[i] + '" ' + thumbw[i]
					+ ' ' + thumbh[i] + imgalt + imgtitle;
					if ((obj.alignment.value != 'none') && (obj.class_mode.value == 'img')){
						htmlCode += ' class="' + obj.alignment.value + '"';
					}
					htmlCode += ' /></a>';
					if ((obj.alignment.value != 'none') && (obj.class_mode.value == 'div')){
						htmlCode += '</div>';
					}
				break;
				case 'thumbnail_album':
					if ((obj.alignment.value != 'none') && (obj.class_mode.value == 'div')){
						htmlCode += '<div class="' + obj.alignment.value + '">';
					}
					htmlCode += '<a href="' + obj.album_url.value
					+ '"><img src="'+thumbnail_src[i] + '" ' + thumbw[i]
					+ ' ' + thumbh[i] + imgalt + imgtitle;
					if ((obj.alignment.value != 'none') && (obj.class_mode.value == 'img')){
						htmlCode += ' class="' + obj.alignment.value + '"';
					}
					htmlCode += ' /></a>';
					if ((obj.alignment.value != 'none') && (obj.class_mode.value == 'div')){
						htmlCode += '</div>';
					}
				break;
				case 'thumbnail_lightbox':
					if ((obj.alignment.value != 'none') && (obj.class_mode.value == 'div')){
						htmlCode += '<div class="' + obj.alignment.value + '">';
					}
					htmlCode += '<a href="' + fullsize_img[i] + '" rel="lightbox';
					if (obj.lightbox_group.value)
						htmlCode += '[' + obj.lightbox_group.value + ']';
					htmlCode += '" title="'
					+ item_description[i] + '" ><img src="'
					+ thumbnail_src[i] + '" ' + thumbw[i]
					+ ' ' + thumbh[i] + imgalt + imgtitle;
					if ((obj.alignment.value != 'none') && (obj.class_mode.value == 'img')){
						htmlCode += ' class="' + obj.alignment.value + '"';
					}
					htmlCode += ' /></a>';
					if ((obj.alignment.value != 'none') && (obj.class_mode.value == 'div')){
						htmlCode += '</div>';
					}
				break;
				case 'fullsize_image':
					if ((obj.alignment.value != 'none') && (obj.class_mode.value == 'div')){
						htmlCode += '<div class="' + obj.alignment.value + '">';
					}
					htmlCode += '<a href="' + image_url[i]
					+ '"><img src="'+fullsize_img[i] + '" ' + imgalt + imgtitle;
					if ((obj.alignment.value != 'none') && (obj.class_mode.value == 'img')){
						htmlCode += ' class="' + obj.alignment.value + '"';
					}
					htmlCode += ' /></a>';
					if ((obj.alignment.value != 'none') && (obj.class_mode.value == 'div')){
						htmlCode += '</div>';
					}
				break;
				case 'thumbnail_custom_url':
					if ((obj.alignment.value != 'none') && (obj.class_mode.value == 'div')){
						htmlCode += '<div class="' + obj.alignment.value + '">';
					}
					htmlCode += '<a href="' + obj.custom_url.value
					+ '"><img src="'+thumbnail_src[i] + '" ' + thumbw[i]
					+ ' ' + thumbh[i] + imgalt + imgtitle;
					if ((obj.alignment.value != 'none') && (obj.class_mode.value == 'img')){
						htmlCode += ' class="' + obj.alignment.value + '"';
					}
					htmlCode += ' /></a>';
					if ((obj.alignment.value != 'none') && (obj.class_mode.value == 'div')){
						htmlCode += '</div>';
					}
				break;
				case 'thumbnail_only':
					if ((obj.alignment.value != 'none') && (obj.class_mode.value == 'div')){
						htmlCode += '<div class="' + obj.alignment.value + '">';
					}
					htmlCode += '<img src="'+thumbnail_src[i] + '" ' + thumbw[i]
					+ ' ' + thumbh[i] + imgalt + imgtitle;
					if ((obj.alignment.value != 'none') && (obj.class_mode.value == 'img')){
						htmlCode += ' class="' + obj.alignment.value + '"';
					}
					htmlCode += ' />';
					if ((obj.alignment.value != 'none') && (obj.class_mode.value == 'div')){
						htmlCode += '</div>';
					}
				break;
				case 'fullsize_only':
					if ((obj.alignment.value != 'none') && (obj.class_mode.value == 'div')){
						htmlCode += '<div class="' + obj.alignment.value + '">';
					}
					htmlCode += '<img src="'+fullsize_img[i] + '" ' + imgalt + imgtitle;
					if ((obj.alignment.value != 'none') && (obj.class_mode.value == 'img')){
						htmlCode += ' class="' + obj.alignment.value + '"';
					}
					htmlCode += ' />';
					if ((obj.alignment.value != 'none') && (obj.class_mode.value == 'div')){
						htmlCode += '</div>';
					}
				break;
				case 'wpg2_image':
					if (obj.alignment.value != 'none'){
						htmlCode += '<div class="' + obj.alignment.value + '">';
					}
					if(window.tinyMCE) {
						htmlCode += '<img src="' + thumbnail_src[i]
						+ '" alt="' + image_id[i];
						if (obj.wpg2_tag_size.value)
							htmlCode += '|' + obj.wpg2_tag_size.value;
						htmlCode += '" title="' + image_id[i];
						if (obj.wpg2_tag_size.value)
							htmlCode += '|' + obj.wpg2_tag_size.value;
						htmlCode += '" ' + thumbw[i] + thumbh[i]
						+ 'id="mce_plugin_g2image_wpg2" />';
					}
					else {
						htmlCode += '<wpg2>' + image_id[i];
						if (obj.wpg2_tag_size.value)
							htmlCode += '|' + obj.wpg2_tag_size.value;
						htmlCode += '</wpg2>';
					}
					if (obj.alignment.value != 'none'){
						htmlCode += '</div>';
					}
				break;
				case 'drupal_g2_filter':
					htmlCode += '[' + obj.drupal_filter_prefix.value + ':' + obj.image_id[i].value;
					if (obj.alignment.value != 'none'){
						htmlCode += ' class=' + obj.alignment.value;
					}
					if (obj.drupal_exactsize.value)
						htmlCode += ' exactsize=' + obj.drupal_exactsize.value;
					htmlCode += ']';
				break;
				case 'link_image':
					htmlCode += '<a href="' + image_url[i] + '">' + obj.link_text.value + '</a>';
				break;
				case 'link_album':
					htmlCode += '<a href="' + obj.album_url.value + '">' + obj.link_text.value + '</a>';
				break;
				default:
					htmlCode += 'Error';
				break;
			}
		}
	}
	insertHtml(htmlCode,obj);
}

function insertWpg2Tag(){

	var obj = document.forms[0];
	var htmlCode = '';

	if (obj.alignment.value != 'none'){
		htmlCode += '<div class="' + obj.alignment.value + '">';
	}
	if(window.tinyMCE) {
		htmlCode += '<img src="' + obj.wpg2_thumbnail.value
		+ '" alt="' + obj.wpg2_id.value
		+ '" title="' + obj.wpg2_id.value
		+ '" width="' + obj.wpg2_thumbw.value + '" height="' + obj.wpg2_thumbh.value
		+ '" id="mce_plugin_g2image_wpg2" />';
	}
	else {
		htmlCode += '<wpg2>' + obj.wpg2_id.value + '</wpg2>';
	}
	if (obj.alignment.value != 'none'){
		htmlCode += '</div>';
	}
	insertHtml(htmlCode,obj);
}

function insertDrupalFilter(){

	var obj = document.forms[0];
	var htmlCode = '';

	htmlCode += '[' + obj.drupal_filter_prefix.value + ':' + obj.drupal_image_id.value;
	if (obj.alignment.value != 'none'){
		htmlCode += ' class=' + obj.alignment.value;
	}
	htmlCode += ']';

	insertHtml(htmlCode,obj);
}
