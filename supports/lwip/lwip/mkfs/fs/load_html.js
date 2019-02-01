
function load_http_doc(url_to_load, target_div, div_content_while_loading, do_on_load){
	
	// get the element width styling (to restore it later)
	//problems with edge, keep size fixed
	//original_element_width_styling = document.getElementById(target_div).style.width;
	//original_element_height_styling = document.getElementById(target_div).style.height;
	
	// force a fixed width while loading
	//document.getElementById(target_div).style.width = document.getElementById(target_div).offsetWidth+"px";
	//document.getElementById(target_div).style.height = document.getElementById(target_div).offsetHeight+"px";

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
	xmlhttp.onreadystatechange=function()
	{
		if (xmlhttp.readyState==4 && xmlhttp.status==200){
			// restore element size styling
	//		document.getElementById(target_div).style.width = original_element_width_styling;
	//		document.getElementById(target_div).style.height = original_element_height_styling;
			
			document.getElementById(target_div).innerHTML=xmlhttp.responseText;
			eval(do_on_load);
		}
		// detect IE8 and above, and edge
		if (document.documentMode || /Edge/.test(navigator.userAgent)) {
			alert('This version supports Firefox or Chrome!');
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

function validade_device(form, header, deviceName)
{
	if (form == 'formFirmwareMCU')
	{
		if ((deviceName.includes("TX") && header == "767MCU-TX") || (deviceName.includes("RX") && header == "767MCU-RX")) 
			return true ;
	}
	else if ((deviceName.includes("TX") && header == "767FPGA-TX") || (deviceName.includes("RX") && header == "767FPGA-RX")) //formFirmwareFPGA form
	{ 
		return true;
	}
	
	return false;
}

function submit_firmware(form, url){
	if( form == 'formFirmwareFPGA' || form == 'formFirmwareMCU')	{

		var fileInput = document.querySelector('#'+form+' input[type="file"]');
		var file      = fileInput.files[0];
		var fileName  = file.name;

		reader = new FileReader();
		reader.onload = function()
		{
			var gTempConfigFile = reader.result;
	
			//header MCU example: 767MCU-TX
			//header FPGA example: 767FPGA-TX
			var maxSize = 10;
			if (form == 'formFirmwareMCU')
				maxSize = 9;

			var buf = gTempConfigFile.slice(0, maxSize);
			var header = String.fromCharCode.apply(null, new Uint8Array(buf));
			var deviceName = document.getElementById("id_mainProductName").innerHTML;

			if (validade_device(form, header, deviceName))
			{
				var content = gTempConfigFile.slice(maxSize);
				var newFile = new File([content], fileName, {type: "application/octet-stream"});

				var data = new FormData();
				data.append('file', newFile);
	
				//saveAs(newFile, fileName);
				document.getElementById('content').innerHTML = '<img src="loading.gif" />';
				fetch(url,{	mode: 'no-cors', 	method: 'POST',	body: data,	})
				.then(function(response){		 return response.text();})
				.then(function(text){	 document.getElementById('content').innerHTML = text;})
				.catch(function(err){	 document.getElementById('content').innerHTML = err;});
			} 
			else
			{
				alert("Bad firmware file.");
			}
		};
		reader.readAsArrayBuffer(file);
   } 
   else{
		var submitButton = document.querySelector('#'+form+' input[type="button"]');
		var resetButton = document.querySelector('#'+form+' input[type="reset"]');
		var hitLabel = document.querySelector('#'+form+' label[class="green"]');
		submitButton.disabled = true;
		resetButton.disabled = true;
		hitLabel.style.display = 'table-cell';
		hitLabel.innerHTML  = 'Submitting...';

		var data = new URLSearchParams();
	   	var fpgaAuto = document.getElementById("mediaSet");
	   	if (fpgaAuto) data.append(fpgaAuto.id, fpgaAuto.value);

		for (const pair of Array.from(new FormData(document.getElementById(form)))){
			data.append(pair[0], decodeURIComponent(pair[1]));
		}

	   	fetch(url,{	mode: 'no-cors', 	method: 'POST',	body: data,	})
	   	.then(function(response){
			hitLabel.innerHTML  = 'Submitted';
			setTimeout(function(){
				hitLabel.style.display = 'none'; 
			}, 2000);
			if (form == 'formNet'){alert('Reboot the unit for the new network settings be effective');}
			submitButton.disabled = false;
			resetButton.disabled = false;
	   })
	   .catch(function(err){submitButton.disabled = false;resetButton.disabled = false;hitLabel.style.display = 'none';alert('Error while saving new settings');});
   }
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

function dhcpEnable(){
	var dhcpEnable = document.getElementsByName("isDHCP")[0].value;
	if (dhcpEnable == 1) {
		document.getElementsByName("address")[0].disabled = true;
		document.getElementsByName("netmask")[0].disabled = true;
		document.getElementsByName("gateway")[0].disabled = true;
	} else {
		document.getElementsByName("address")[0].disabled = false;
		document.getElementsByName("netmask")[0].disabled = false;
		document.getElementsByName("gateway")[0].disabled = false;
	}
}

function tabColor(elementId){
	/* clear all tabs */
	var anchors = document.getElementsByClassName("anchor");
	var i;
	for (i = 0; i < anchors.length; i++) {
		anchors[i].classList.remove("black");
	} 
	/* turn the link black */
	document.getElementById(elementId).classList.add("black");
}
