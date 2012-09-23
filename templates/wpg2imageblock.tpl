{*
 * Author: WPG2 Team, Gallery2 Development Team
 * Author URI: http://wpg2.galleryembedded.com/
 * Updated: 23/06/2007

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 *}

{foreach from=$ImageBlockData.blocks item=block}

  {if !empty($ImageBlockData.divClass)}
 	<div class="{$ImageBlockData.divClass}">
  {else}
	<div class="one-image">
  {/if}
  {if !empty($block.title)}
    <h3> {g->text text=$block.title} </h3>
  {/if}

  {capture assign="linkHref"}{strip}
    {if empty($block.link)}
	{if empty($block.lightboxid)}
		{g->url arg1="view=core.ShowItem" arg2="itemId=`$block.id`" forceFullUrl=true}
	{else}
		{g->url arg1="view=core.DownloadItem" arg2="itemId=`$block.lightboxid`" forceFullUrl=true}
	{/if}
    {elseif $block.link != 'none'}
      {$block.link}
    {/if}
  {/strip}{/capture}
  {capture assign="link"}{if !empty($linkHref)}
    <a href="{$linkHref}"{if
	!empty($ImageBlockData.linkTarget)} target="{$ImageBlockData.linkTarget}" {/if} title="{$block.item.title|markup}">
  {/if}{/capture}
  {if $block.item.canContainChildren}
    {assign var=frameType value="albumFrame"}
  {else}
    {assign var=frameType value="itemFrame"}
  {/if}
  {if array_key_exists('maxSize', $ImageBlockData)}
    {assign var=maxSize value=$ImageBlockData.maxSize}
  {/if}
  {assign var=imageItem value=$block.item}
  {if isset($block.forceItem)}{assign var=imageItem value=$block.thumb}{/if}
  {if isset($ImageBlockData.$frameType)}
    {g->container type="imageframe.ImageFrame" frame=$ImageBlockData.$frameType
		  width=$block.thumb.width height=$block.thumb.height maxSize=$maxSize}
      {$link}
	{g->image item=$imageItem image=$block.thumb id="%ID%" class="%CLASS%" maxSize=$maxSize forceFullUrl=true}
      {if !empty($linkHref)} </a> {/if}
    {/g->container}
  {else}
    {$link}
      {g->image item=$imageItem image=$block.thumb class="giThumbnail" maxSize=$maxSize forceFullUrl=true}
    {if !empty($linkHref)} </a> {/if}
  {/if}

  {if isset($ImageBlockData.show.title) && isset($block.item.title)}
    <h4 class="giDescription">
      {$block.item.title|markup}
    </h4>
  {/if}

  {if isset($ImageBlockData.show.date) ||
      isset($ImageBlockData.show.views) ||
      isset($ImageBlockData.show.owner)}
    <p class="giInfo">
      {if isset($ImageBlockData.show.date)}
      <span class="summary">
	{g->text text="Date:"} {g->date timestamp=$block.item.originationTimestamp}
      </span>
      {/if}

      {if isset($ImageBlockData.show.views)}
      <span class="summary">
	{g->text text="Views: %d" arg1=$block.viewCount}
      </span>
      {/if}

      {if isset($ImageBlockData.show.owner)}
      <span class="summary">
	{g->text text="Owner: %s" arg1=$block.owner.fullName|default:$block.owner.userName}
      </span>
      {/if}
    </p>
  {/if}
 </div>
{/foreach}