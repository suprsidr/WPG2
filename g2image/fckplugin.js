//fckplugin.js
/*
 *
 * G2Image
 * @author: Steve Lineberry    aka    ShockSLL
 */
var G2ImageCommand=function(){
        //create our own command, we dont want to use the FCKDialogCommand because it uses the default fck layout and not our own
};
G2ImageCommand.prototype.Execute=function(){

}
G2ImageCommand.GetState=function() {
        return FCK_TRISTATE_OFF; //we dont want the button to be toggled
}

G2ImageCommand.Execute=function() {
        //open a popup window when the button is clicked
        window.open(FCKPlugins.Items['g2image'].Path + 'g2image.php?g2ic_tinymce=0', 'G2Image', 'width=800,height=600,scrollbars=yes,scrolling=yes,location=no,toolbar=no');
}

FCKCommands.RegisterCommand('G2Image', G2ImageCommand ); //otherwise our command will not be found

var oG2Image = new FCKToolbarButton('G2Image', 'Insert G2 Image');
oG2Image.IconPath = FCKPlugins.Items['g2image'].Path + 'images/g2image.gif'; //specifies the image used in the toolbar

FCKToolbarItems.RegisterItem( 'G2Image', oG2Image );

