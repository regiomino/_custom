/**
 * @file
 * A JavaScript file for the module.
 */ 
(function ($, Drupal, window, document, undefined) {

	// $.fn.locationChosen = function(loc) {
		// var zipcode = loc.split(' ');
		// $.cookie('Drupal.visitor.locationsubmitted', 1, { path: '/' });
		// $.cookie('Drupal.visitor.postalcode', zipcode[0], { path: '/' });
		// //$.cookie('Drupal.visitor.city', loc.replace(zipcode[0] + " ",""), { path: '/' });
		// window.location = window.location.pathname;
	// }
	
	// $.fn.checkValidityOfLocation = function(loc) {
		
	// }

	/*
	function geolocationSuccessFunction(position) {
		regiomino_geolocation = $.fn.codeLatLng(position.coords.latitude, position.coords.longitude, '');
	}
	function geolocationErrorFunction() {
		//$('body').append('<div id="browserwontshare">' + Drupal.t('Your location could not be detected. Please allow your browser to share your location or enter a zipcode instead.') + '</div>');
		//setTimeout(function() {
			//$('#browserwontshare').fadeOut(500);
		//}, 5000);
		$.fn.removeThrobber();
	}
	
	//function geolocationDetected(geocodeResult, parsedGeocodeResult) {
	function geolocationDetected() {
					
		// var lat = geocodeResult['geometry']['location']['mb'];
		// var lon = geocodeResult['geometry']['location']['nb'];
		var error = 1;
		$('.ui-autocomplete .ui-menu-item a').each(function() {
			if($(this).text() == $('#edit-zipcode--2').val()) {
				error = 0;
			}
		});
		
		if(error == 1) {
			//entry differs from suggestions, geocode it separately

			var geocoder = new google.maps.Geocoder();
			geocoder.geocode({'address': $('#edit-zipcode--2').val(), 'region': 'de'}, function (result, status) {
				if (status == google.maps.GeocoderStatus.OK) {
					var lat = result[0].geometry.location.lat();
					var lng = result[0].geometry.location.lng();
					//var state = "N/A";
					var city = "N/A";
					var zipcode = "N/A";
					for (var component in result[0]['address_components']) {
						for (var i in result[0]['address_components'][component]['types']) {
							if (result[0]['address_components'][component]['types'][i] == "locality") {
								//state = result[0]['address_components'][component]['short_name'];
								city = result[0]['address_components'][component]['long_name'];
							}
						}
					}
					
					lat = parseFloat(lat);
					lon = parseFloat(lng);
					
					//Save variables in cookies
					$.cookie('Drupal.visitor.latitude', lat, { path: '/' });
					$.cookie('Drupal.visitor.longitude', lng, { path: '/' });
					$.cookie('Drupal.visitor.city', city, { path: '/' });
					
					var latlng = new google.maps.LatLng(lat, lon);
					var geocoder_int = new google.maps.Geocoder();
					geocoder_int.geocode({'latLng': latlng}, function(results, status) {
						if (status == google.maps.GeocoderStatus.OK) {
							if (results) {
								for (var index in results) {
									for (var component in results[index]['address_components']) {
										for (var i in results[index]['address_components'][component]['types']) {
											if (results[index]['address_components'][component]['types'][i] == "postal_code") {
												$.cookie('Drupal.visitor.postalcode', results[index]['address_components'][component]['long_name'], { path: '/' });
											}
										}
									}
								}
								$.cookie('Drupal.visitor.locationentered', 1, { path: '/' });
								$('#geolocationdetector').html('<p><img src="/sites/all/themes/regiomino_seven/images/ajax-loader.gif"><p>' + Drupal.t('Please wait. We are detecting your location...') + '</p>');
								$('#refreshcounter').val('1');
								location.reload();
							} else {
								alert(Drupal.t('We could not detect a valid address. Please be more specific.'));
							}
						} else {
							alert(Drupal.t('We could not detect a valid address. Please be more specific.'));
						}
					});
					
					 
					//$.fancybox.close();
					//$.fn.getLocationsortedOffers(lat, lng, city);
				} else {
				}
			});
			
		}
		else {
			var lat = $('input[name="lat"]').val();
			var lon = $('input[name="lon"]').val();
			var city = $('input[name="loc"]').val();
			var postcode = $('input[name="zip"]').val();
			
			$.cookie('Drupal.visitor.latitude', lat, { path: '/' });
			$.cookie('Drupal.visitor.longitude', lon, { path: '/' });
			// for (var component in geocodeResult['address_components']) {
				// for (var i in geocodeResult['address_components'][component]['types']) {
					// if (geocodeResult['address_components'][component]['types'][i] == "locality") {
						// var city = geocodeResult['address_components'][component]['long_name'];
					// }
				// }
			// }
			$.cookie('Drupal.visitor.city', city, { path: '/' });
			// for (var component in geocodeResult['address_components']) {
				// for (var i in geocodeResult['address_components'][component]['types']) {
					// if (geocodeResult['address_components'][component]['types'][i] == "postal_code") {
						// var postcode = geocodeResult['address_components'][component]['long_name'];
					// }
				// }
			// }
			if(postcode != 'false') {
				$.cookie('Drupal.visitor.postalcode', postcode, { path: '/' });
				$.cookie('Drupal.visitor.locationentered', 1, { path: '/' }); 
				//$.fancybox.close();
				//$.fn.getLocationsortedOffers(lat, lon, city);
				$('#geolocationdetector').html('<p><img src="/sites/all/themes/regiomino_seven/images/ajax-loader.gif"><p>' + Drupal.t('Please wait. We are detecting your location...') + '</p>');
				$('#refreshcounter').val('1');
				location.reload();
			}
			else {
				lat = parseFloat(lat);
				lon = parseFloat(lon);
				var latlng = new google.maps.LatLng(lat, lon);
				var geocoder = new google.maps.Geocoder();
				geocoder.geocode({'latLng': latlng}, function(results, status) {
					if (status == google.maps.GeocoderStatus.OK) {
						if (results[1]) {
							// map.setZoom(11);
							// marker = new google.maps.Marker({
									// position: latlng,
									// map: map
							// });
							// infowindow.setContent(results[1].formatted_address);
							// infowindow.open(map, marker);
							for (var component in results[1]['address_components']) {
								for (var i in results[1]['address_components'][component]['types']) {
									if (results[1]['address_components'][component]['types'][i] == "postal_code") {
										$.cookie('Drupal.visitor.postalcode', results[1]['address_components'][component]['long_name'], { path: '/' });
									}
								}
							}
							$.cookie('Drupal.visitor.locationentered', 1, { path: '/' });
							//$.fancybox.close();
							//$.fn.getLocationsortedOffers(lat, lon, city);
							$('#geolocationdetector').html('<p><img src="/sites/all/themes/regiomino_seven/images/ajax-loader.gif"><p>' + Drupal.t('Please wait. We are detecting your location...') + '</p>');
							$('#refreshcounter').val('1');
							location.reload();
						} else {
							alert(Drupal.t('We could not detect a valid address. Please be more specific.'));
						}
					} else {
						alert(Drupal.t('We could not detect a valid address. Please be more specific.'));
					}
				});
			}
		}

	}

// 	$(document).ready(function() {

		// if($('#refreshcounter').val() == '1') {
			// $('#refreshcounter').val('0');
			// location.reload();
		// }
		// if(!$.cookie('Drupal.visitor.locationentered')) {
			// $.fn.detectGeolocation();
  		// $.fancybox({
				// 'autoScale': true,
				// 'transitionIn': 'fade',
				// 'transitionOut': 'fade',
				// 'href': '#geolocationdetector',
				// 'helpers' : { 
					// 'overlay' : {
						// 'closeClick': false
					// }
				// }
			// });
			// $.cookie('Drupal.visitor.locationentered', 1, { path: '/' });
		// }
		// $('#changeregionbtn').fancybox({
			// 'autoScale': true,
			// 'transitionIn': 'fade',
			// 'transitionOut': 'fade',
			// 'href': '#geolocationdetector'
		// });
		// $('input[name="zipcode"]').addresspicker({
			// regionBias: 'de',
			// componentsFilter: 'country:DE',
			// //updateCallback: geolocationDetected,
			// elements: {
		    // lat:      'input[name="lat"]',
		    // lng:      'input[name="lon"]',
		    // locality: 'input[name="loc"]',
		    // postal_code: 'input[name="zip"]'
		  // }
		// });
		// $('#edit-zipcode--2-button').click(function() {
			// geolocationDetected();
		// });
		
	// });
	$.fn.detectGeolocation = function(zipcode) {
		var user_js_uid = Drupal.settings.user_js_uid;
		
		if(user_js_uid > 0 && !zipcode) {
				var user_js_lat = Drupal.settings.DRUPAL_VISITOR_LATITUDE;
				var user_js_lon = Drupal.settings.DRUPAL_VISITOR_LONGITUDE;	
				regiomino_geolocation = $.fn.codeLatLng(user_js_lat, user_js_lon, '');				
		}
		else {
		//$.fn.createThrobber();
			var regiomino_geolocation = new Object;
			if(zipcode) {
				regiomino_geolocation = $.fn.codeLatLng('', '', zipcode);
			}
			else if (navigator.geolocation) {
				navigator.geolocation.getCurrentPosition(geolocationSuccessFunction, geolocationErrorFunction);
			}
		}
	}
	$.fn.codeLatLng = function(lat, lng, address) {
		var geocoder = new google.maps.Geocoder();
		regiomino_geolocation = new Object;
		if(address) {
			var latlong = "";
			geocoder.geocode({'address': address, 'region': 'de'}, function (result, status) {
				if (status == google.maps.GeocoderStatus.OK) {
					var lat = result[0].geometry.location.lat();
					var lng = result[0].geometry.location.lng();
					//var state = "N/A";
					var city = "N/A";
					var zipcode = "N/A";
					
					for (var component in result[0]['address_components']) {
						for (var i in result[0]['address_components'][component]['types']) {
							if (result[0]['address_components'][component]['types'][i] == "administrative_area_level_1") {
								//state = result[0]['address_components'][component]['short_name'];
								city = result[0]['address_components'][1]['long_name'];
							}
						}
					}
					//Save variables in cookies

					$.fn.getLocationsortedOffers(lat, lng, city);
					$.fn.zipcodeInfo(address);
					$.cookie('Drupal.visitor.latitude', lat, { path: '/' });
					$.cookie('Drupal.visitor.longitude', lng, { path: '/' });
					$.cookie('Drupal.visitor.city', city, { path: '/' });
					$.cookie('Drupal.visitor.postalcode', address, { path: '/' });
					$.cookie('Drupal.visitor.locationentered', 1, { path: '/' });
				} else {
				}
			});
		}	else {
			var latlng = new google.maps.LatLng(lat, lng);
			geocoder.geocode({'latLng': latlng}, function(results, status) {
				if (status == google.maps.GeocoderStatus.OK) {
					if (results[1]) {
						for (var i=0; i<results[0].address_components.length; i++) {
							for (var b=0;b<results[0].address_components[i].types.length;b++) {
								var breakone = false;
								var breaktwo = false;
								if (results[0].address_components[i].types[b] == "locality") {
									city= results[0].address_components[i];
									breakone = true;
								}
								if (results[0].address_components[i].types[b] == "postal_code") {
									postal_code= results[0].address_components[i];
									breaktwo = true;
								}
								if(breakone && breaktwo) break;
							}
						}
						//Save variables in cookies
						$('input[name="zipcode"]').val(postal_code.long_name + ' ' + city.long_name);
						$('input[name="lat"]').val(lat);
						$('input[name="lon"]').val(lng);
						$('input[name="loc"]').val(city.long_name);
						$('input[name="zip"]').val(postal_code.long_name);
						
						//$.fn.latlongInfo(lat, lng, city.long_name);
 						//$.cookie('Drupal.visitor.latitude', lat);
						//$.cookie('Drupal.visitor.longitude', lng);
						//$.cookie('Drupal.visitor.city', city.long_name);
						//$.cookie('Drupal.visitor.postalcode', postal_code.long_name);
					} else {
					}
				} else {
				}
			});
		}
	}
	$.fn.latlongInfo = function(lat, lng, loc) {
		var data = new Object;
		data['lat'] = lat;
		data['lng'] = lng;
		data['loc'] = loc;
		var callback_url = Drupal.settings.basePath + 'regiomino-geolocation-latlonginfo';
		$.ajax({
			url: callback_url,
			type: 'POST',
			data: data,
		});				
	}
	$.fn.zipcodeInfo = function(zipcode) {
		var data = new Object;
		data['lat'] = $.cookie('Drupal.visitor.latitude');
		data['lng'] = $.cookie('Drupal.visitor.longitude');
		data['loc'] = $.cookie('Drupal.visitor.city');
		
		
		data['zipcode'] = zipcode;
		var callback_url = Drupal.settings.basePath + 'regiomino-geolocation-zipcodeinfo';
		$.ajax({
			url: callback_url,
			type: 'POST',
			data: data,
		});				
	}
	*/
	
})(jQuery, Drupal, this, this.document);