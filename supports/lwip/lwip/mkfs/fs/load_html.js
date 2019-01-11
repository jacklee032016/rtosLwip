
function load_http_doc(url_to_load, target_div, div_content_while_loading, do_on_load){
	
	// get the element width styling (to restore it later)
	original_element_width_styling = document.getElementById(target_div).style.width;
	original_element_height_styling = document.getElementById(target_div).style.height;
	
	// force a fixed width while loading
	document.getElementById(target_div).style.width = document.getElementById(target_div).offsetWidth+"px";
	document.getElementById(target_div).style.height = document.getElementById(target_div).offsetHeight+"px";
		
	if(div_content_while_loading != null && div_content_while_loading != 'default'){
		document.getElementById(target_div).innerHTML = div_content_while_loading;
	}
	
	// accept a 'default' value so that we can easy change this across site
	if(div_content_while_loading == 'default' || div_content_while_loading == null){
		document.getElementById(target_div).innerHTML = '<img src="./logo.jpg" />';
	}
	
	var xmlhttp;
	if (window.XMLHttpRequest){
		// code for IE7+, Firefox, Chrome, Opera, Safari
		xmlhttp=new XMLHttpRequest();
	} else {
		// code for IE6, IE5
		xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");
	}
	xmlhttp.onreadystatechange=function(){
		if (xmlhttp.readyState==4 && xmlhttp.status==200){
			// restore element size styling
			document.getElementById(target_div).style.width = original_element_width_styling;
			document.getElementById(target_div).style.height = original_element_height_styling;
			
			document.getElementById(target_div).innerHTML=xmlhttp.responseText;
			eval(do_on_load);
		}
	}
	xmlhttp.open("GET",url_to_load,true);
	xmlhttp.send();
}

function showdiv(aval){
	if (aval == "SDP"){
		divSettingsSdp.style.display= 'inline-block';
		divSettingsData.style.display='none';}
	else if(aval == "Auto"){
		divVideoSettings.style.display = 'none';
	}
	else if(aval == "Manual"){
		divVideoSettings.style.display = 'inline-block';
	}
	else{
		divSettingsSdp.style.display='none';
		divSettingsData.style.display= 'inline-block';}
}
function submit_firmware(form, url){
	 if( 'formFirmwareFPGA' || form == 'formFirmwareMCU')	{
		var fileInput = document.querySelector('input[type="file"]');
		var data = new FormData();
		data.append('file', fileInput.files[0]);
	} 
	else{
		var data = new URLSearchParams();
		var fpgaAuto = document.getElementById("fpgaAuto");
		if (fpgaAuto) data.append(fpgaAuto.id, fpgaAuto.value);
 
		for (const pair of new FormData(document.getElementById(form))){
			// data.append(pair[0], pair[1]);
			data.append(pair[0], decodeURIComponent(pair[1]));
		}
	}

	document.getElementById('content').innerHTML = '<img src="loading.gif" />';
	fetch(url,{	mode: 'no-cors', 	method: 'POST',	body: data,	})
	.then(function(response){		 return response.text();})
	.then(function(text){	 document.getElementById('content').innerHTML = text;})
	.catch(function(err){	 document.getElementById('content').innerHTML = err;});
}

function reboot_device(){
	var response = confirm("Do you want to reboot?"); 
	if (response){ 
		load_http_doc('reboot', 'content',''); 
		setTimeout(function(){window.location.reload(1);}, 3000); 
	}
}

function reset_device(){
	var response = confirm("Do you want to restore the device to factory settings?"); 
	if (response){ 
		load_http_doc('reset', 'content',''); 
		setTimeout(function(){window.location.reload(1);}, 3000); 
	}
}
