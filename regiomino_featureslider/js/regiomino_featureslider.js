/**
 * @file
 * A JavaScript file for the module.
 */

// JavaScript should be made compatible with libraries other than jQuery by
// wrapping it with an "anonymous closure". See:
// - http://drupal.org/node/1446420
// - http://www.adequatelygood.com/2010/3/JavaScript-Module-Pattern-In-Depth

(function ($, Drupal, window, document, undefined) {

  /* The resize event */
  $(window).resize(function() {
    var carousel_element = 194; /* Height of each carousel element + padding */

    var padding = 80; /* Height of the header section */
    var viewport_width = $('#region-user-first').width(); /* Viewport height */

    /* Calculate how many images will fit in the given height */  
    var cpp = Math.floor((viewport_width - padding)/carousel_element);

    /* And resize the carousel */
    $('.jcarousel-container, .jcarousel-clip').width(carousel_element * cpp);
  });

  /* Make sure this gets called when the page first loads */
  Drupal.behaviors.mytheme = {                   
    attach: function(context, settings) {
      $(window).resize();
    }
	}

	$(document).ready(function() {
	
		$.fn.getLocationsortedOffers(Drupal.settings.DRUPAL_VISITOR_LATITUDE, Drupal.settings.DRUPAL_VISITOR_LONGITUDE, Drupal.settings.DRUPAL_VISITOR_CITY);
	
		$('#cancelthrobber').click(function() {
			$.fn.removeThrobber();
		});

		/* //Check if position is already available
		if($.cookie('Drupal.visitor.locationentered')) {
			$.fn.getLocationsortedOffers($.cookie('Drupal.visitor.latitude'), $.cookie('Drupal.visitor.longitude'), $.cookie('Drupal.visitor.city'));
		}
		else {
			//$('#block-regiomino-geolocation-regiomino-geolocation-request').fadeIn(400);
		} */

/* 		$('#changeregionbtn').click(function() {
				$('.price').each(function() {
					$(this).fadeOut(400);
				});
				$('#block-views-feature-slider-block').fadeOut(400);
				//$('#block-regiomino-geolocation-regiomino-geolocation-request').fadeIn(400);
		}); */
		
	});

	$.fn.createThrobber = function() {
		$('.region-user-first').append('<div class="geolocationthrobber"><div class="throbber"><a id="cancelthrobber" href="#"><img src="/sites/all/themes/regiomino_seven/images/entfernen.png" /></a></div></div>');
	}
	$.fn.removeThrobber = function() {
		$('.geolocationthrobber').remove();
	}
	
	$.fn.getLocationsortedOffers = function(lat, lng, loc) {
		$.fn.createThrobber();
		//$('.featuresliderlocality').html(loc);
		
		var data = new Object;
		data['lat'] = lat;
		data['lng'] = lng;
		data['loc'] = loc;

		var callback_url = Drupal.settings.basePath + 'regiomino-featureslider-get-featured-callback';

		$.ajax({
			url: callback_url,
			type: 'POST',
			data: data,
			success: function (data, textStatus, jqXHR) {
				$('#block-views-feature-slider-block').fadeIn(400);
				$('#block-views-feature-slider-block').html(data);
				if ($('.price').length) {
					$('.price').each(function() {
						var classes = $(this).attr('class');
						var nid = classes.split(' ');
						var currentprice = $(this).text();
						var data = new Object;
						data['nid'] = nid;
						data['currentprice'] = currentprice;
						//console.log(data);
						var callback_url = Drupal.settings.basePath + 'regiomino-featureslider-get-feature-prices';
						$.ajax({
							url: callback_url,
							type: 'POST',
							data: data,
							success: function (data, textStatus, jqXHR) {
								$.fn.updateFeaturePrices(nid[1], data);
							},
							error: function (http) {
								//console.log('ERROR');
								//console.log(http);
							},
							complete: function() {
							}
						});
					});
				}
				else {
					$('#block-block-5').fadeIn(400);
				}		
		
				$('.jcarousel-skin-regiomino').jcarousel({scroll: 1});
				var carousel_element = 194; /* Height of each carousel element + padding */
				var padding = 80; 
				var viewport_width = $('#region-user-first').width();
				var cpp = Math.floor((viewport_width - padding)/carousel_element);
				$('.jcarousel-container, .jcarousel-clip').width(carousel_element * cpp);
			},
			error: function (http) {
				$.fn.removeThrobber();
			},
			complete: function() {
				//$('#block-regiomino-geolocation-regiomino-geolocation-request').fadeOut(400);
				$.fn.removeThrobber();
			}
		});				
	}

	$.fn.updateFeaturePrices = function(nid, data) {
		$('.price').each(function() {
			var classes = $(this).attr('class');
			var nodeid = classes.split(' ');
			if(nodeid[1] == nid) $(this).html(data);
			$(this).show();
		});
	}
	
})(jQuery, Drupal, this, this.document);