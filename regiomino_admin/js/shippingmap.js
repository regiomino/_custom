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
							//center: new google.maps.LatLng(41.875696,-87.624207),
							mapTypeId: google.maps.MapTypeId.ROADMAP
					}
					map = new google.maps.Map(document.getElementById("map"), myOptions);

					var marker = new google.maps.Marker({
							map: map,
							position: results[0].geometry.location
					});
					var zipCodeLayer = new google.maps.KmlLayer({
						url: Drupal.settings.basePath + 'sites/all/modules/regiomino_admin/kml/9.kml'
						//url: 'http://gmaps-samples.googlecode.com/svn/trunk/ggeoxml/cta.kml'
					});
					zipCodeLayer.setMap(map);
			}
	});
	
}); 