jQuery(document).ready(function($) {
	
	var address = Drupal.settings.SHIPPING_OPTION_METRO_NAME;
	
	geocoder = new google.maps.Geocoder();
	geocoder.geocode({
			'address': address
	}, function(results, status) {
			if (status == google.maps.GeocoderStatus.OK) {
					var myOptions = {
							zoom: 9,
							center: results[0].geometry.location,
							//center: new google.maps.LatLng(24.886436490787712, -70.2685546875),
							mapTypeId: google.maps.MapTypeId.ROADMAP
					}
					map = new google.maps.Map(document.getElementById("map"), myOptions);

					var marker = new google.maps.Marker({
							map: map,
							position: results[0].geometry.location
					});
					
					data = new Object,
					callback_url = Drupal.settings.basePath + 'regiomino-geolocation-coordinatestring';
					data['metro_key'] = Drupal.settings.SHIPPING_OPTION_METRO_KEY;
					data['field'] = 'field_pickup_areas';
					
					$.ajax({
							url: callback_url,
							type: 'POST',
							data: data,
							success: function (data, textStatus, jqXHR) {
								var polygons = data.split('|');
								for(var i=1; i<polygons.length; i++) {
									var polygon = polygons[i].split(' ');
									var polygonCoords = new Array();
									for(var j=0; j<polygon.length; j++) {
										var polygonpoint = polygon[j].split(',');
										polygonCoords.push(new google.maps.LatLng(polygonpoint[1], polygonpoint[0]));
										console.log(polygonpoint[1], polygonpoint[0]);
									}
									var zipcodePolygon = new google.maps.Polygon({
										paths: polygonCoords,
										strokeColor: '#FF0000',
										strokeOpacity: 0.8,
										strokeWeight: 2,
										fillColor: '#FF0000',
										fillOpacity: 0.35
									});
									zipcodePolygon.setMap(map);
								}
							},
							error: function (http) {
							},
							complete: function() {	
							}
					});
					
					data = new Object,
					callback_url = Drupal.settings.basePath + 'regiomino-geolocation-coordinatestring';
					data['metro_key'] = Drupal.settings.SHIPPING_OPTION_METRO_KEY;
					data['field'] = 'field_delivery_areas';
					
					$.ajax({
							url: callback_url,
							type: 'POST',
							data: data,
							success: function (data, textStatus, jqXHR) {
								var polygons = data.split('|');
								for(var i=1; i<polygons.length; i++) {
									var polygon = polygons[i].split(' ');
									var polygonCoords = new Array();
									for(var j=0; j<polygon.length; j++) {
										var polygonpoint = polygon[j].split(',');
										polygonCoords.push(new google.maps.LatLng(polygonpoint[1], polygonpoint[0]));
										console.log(polygonpoint[1], polygonpoint[0]);
									}
									var zipcodePolygon = new google.maps.Polygon({
										paths: polygonCoords,
										strokeColor: '#00FF00',
										strokeOpacity: 0.8,
										strokeWeight: 2,
										fillColor: '#00FF00',
										fillOpacity: 0.35
									});
									zipcodePolygon.setMap(map);
								}
							},
							error: function (http) {
							},
							complete: function() {	
							}
					});				
					
			}
	});
	
}); 