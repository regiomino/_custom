jQuery(document).ready(function($) {

	var customIcons = {
		/* storage_profile: {
			icon: Drupal.settings.basePath + 'sites/all/themes/rmshoploop/images/blatt_karte.png'
		},
		shipper_profile: {
			icon: Drupal.settings.basePath + 'sites/all/themes/rmshoploop/images/auto_karte.png'
		},*/
		commercial_profile: {
			icon: "https://maps.google.com/mapfiles/ms/icons/green.png"
		},
		seller_profile: {
			icon: "https://maps.google.com/mapfiles/ms/icons/red.png"
		}
	};
	
	var map = new google.maps.Map(document.getElementById("map"), {
		center: new google.maps.LatLng(49.800855, 11.017640),
		zoom: 9,
		mapTypeId: 'roadmap'
	});
	
	var infoWindow = new google.maps.InfoWindow;
	
	downloadUrl(Drupal.settings.basePath + 'regiomino-geolocation-participantxml', function(data) {
		var xml = data.responseXML;
		var markers = xml.documentElement.getElementsByTagName("marker");
		for (var i = 0; i < markers.length; i++) {
			var name = markers[i].getAttribute("name");
			var address = markers[i].getAttribute("address");
			var type = markers[i].getAttribute("type");
			var point = new google.maps.LatLng(
				parseFloat(markers[i].getAttribute("lat")),
				parseFloat(markers[i].getAttribute("lng")));
			var html = "<b>" + name + "</b> <br/>" + address;
			var icon = customIcons[type] || {};
			var marker = new google.maps.Marker({
				map: map,
				position: point,
				icon: icon.icon
			});
			bindInfoWindow(marker, map, infoWindow, html);
		}
	});
	
	function downloadUrl(url,callback) {
		var request = window.ActiveXObject ?
		new ActiveXObject('Microsoft.XMLHTTP') :
		new XMLHttpRequest;

		request.onreadystatechange = function() {
			if (request.readyState == 4) {
				request.onreadystatechange = doNothing;
				callback(request, request.status);
			}
		};

		request.open('GET', url, true);
		request.send(null);
	}
	
	function bindInfoWindow(marker, map, infoWindow, html) {
		google.maps.event.addListener(marker, 'click', function() {
			infoWindow.setContent(html);
			infoWindow.open(map, marker);
		});
	}
	
	function doNothing() {}
}); 