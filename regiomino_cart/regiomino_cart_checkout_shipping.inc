<?php


function regiomino_cart_shippinginfo() {

	//Make sure checkout is a process of steps, no manual jumps
	global $base_url;
	$force_referer[] = '/checkout/address';
	$force_referer[] = '/checkout/shippingaddress';
	$force_referer[] = '/checkout/shipping';
	$force_referer[] = '/checkout/payment';
	$force_referer[] = '/user/register';
	$force_referer[] = '/user/login';
	$force_referer[] = '/cart';
	$parsedvalue = parse_url($_SERVER['HTTP_REFERER']);
	$referer = $parsedvalue['path'];
	if(!in_array($referer, $force_referer)) {
		drupal_set_message(t('Please follow the steps of the checkout process one after another'), 'warning');
		drupal_goto('cart');
	}
	
	global $user;
	if ($user->uid) {
		$loggedin = TRUE;
		$cart_id = $user->uid;
	}
	else {
		$loggedin = FALSE;
		$cart_id = session_id();
	}

	//Load cart
	$empty = regiomino_cart_empty_cart($cart_id);
	//Set message if cart is empty
	if($empty) {
		drupal_goto('cart');
	}
	
	//Make sure user logs in or registers before continuing to checkout
	if(!$loggedin) drupal_goto('user', array('query' => drupal_get_destination()));
	$content = drupal_get_form('regiomino_cart_shippinginfo_form');
	
	return	theme('regiomino_cart_theme_shipping', array(
						'vars' => array(
							'form' => $content,
						),
					));
}

function regiomino_cart_shippinginfo_form($form, &$form_state) {

	//Set title and checkout steps
	drupal_set_title('');
	
	//Load cart
	$warenkorb = regiomino_cart_load_cart();
	$reloadcart = FALSE;
	$availableforall = array();
	$availableforall_cmp1 = array();
	$firstrun = TRUE;
	$centavailableforall = array();
	$centavailableforall_cmp1 = array();
	$centfirstrun = TRUE;
	//Perform some actions on the cart items
	foreach($warenkorb as $key=>$value) {
	
		//Check if the products are still available in the selected quantity
		foreach($value['product'] as $cart_item_id => $productvalue) {
			$nodeobject = node_load($productvalue['nid']);
			$availability = regiomino_offer_availability($nodeobject, $productvalue['menge']);
			//Store stock return value from availability check
			$stock_array = $availability->stock;
			//Perform different actions based on availability of selected products
			switch($availability->available) {
				//Actions, if product is not available anymore
				case 0:
					$reloadcart = TRUE;
					//Remove product from cart
					regiomino_cart_remove_product($cart_item_id, $availability->message, $msgtype = 'error', drupal_get_destination(), FALSE);
					break;
				//Actions, if product has a reduced availability
				case 1:
					$reloadcart = TRUE;
					//Set selected amount to maximum available quantity
					regiomino_cart_remove_product($cart_item_id, t("Unfortunately the amount you selected for <strong>@product</strong> is not available anymore. We have removed the product from your cart.", array('@product' => $productvalue['title'])), $msgtype = 'error', drupal_get_destination(), FALSE);
					break;
			}
		}
		
		//Check if the products are still deliverable
		$avlbpickupdates = array();
		$avlbpickupdates = regiomino_shipping_get_available_pickupdates($value['product'], 'bringlivery', TRUE, FALSE, 1);

		$avlbcentpickupdates = array();
		$avlbcentpickupdates = regiomino_shipping_get_available_pickupdates($value['product'], 'centralpickup', TRUE, FALSE, 1);

		if(empty($avlbpickupdates) && empty($avlbcentpickupdates)) {
			$reloadcart = TRUE;
			foreach($value['product'] as $cart_item_id => $else) {
				regiomino_cart_remove_product($cart_item_id, t('The product <strong>@title</strong> was removed from your cart, because it is not deliverable to your address', array('@title' => $else['title'])), $msgtype = 'error', drupal_get_destination(), FALSE);
			}
		}
		//Check if the same shipping options are available for all sellers
		else {
			if($firstrun) {
				foreach($avlbpickupdates as $avlbkey => $avlbval) {
					$availableforall[$avlbkey] = $avlbval;
					$availableforall_cmp1[] = $avlbkey;
				}
				$firstrun = FALSE;
			}
			else {
				$availableforall_cmp2 = array();
				foreach($avlbpickupdates as $avlbkey => $avlbval) {
					$availableforall_cmp2[] = $avlbkey;
				}
				$returned = array_diff($availableforall_cmp1, $availableforall_cmp2);
				if(!empty($returned)) {
					foreach($returned as $retval) {
						unset($availableforall[$retval]);
					}
				}
			}
			
			if($centfirstrun) {
				foreach($avlbcentpickupdates as $avlbkey => $avlbval) {
					$centavailableforall[$avlbkey] = $avlbval;
					$centavailableforall_cmp1[] = $avlbkey;
				}
				$centfirstrun = FALSE;
			}
			else {
				$centavailableforall_cmp2 = array();
				foreach($avlbcentpickupdates as $avlbkey => $avlbval) {
					$centavailableforall_cmp2[] = $avlbkey;
				}
				$centreturned = array_diff($centavailableforall_cmp1, $centavailableforall_cmp2);
				if(!empty($centreturned)) {
					foreach($centreturned as $retval) {
						unset($centavailableforall[$retval]);
					}
				}
			}
			
		}
	}
	
	//Reload cart if changes on products have been performed during the above checks
	if($reloadcart) {
		$warenkorb = regiomino_cart_load_cart();
		if(empty($warenkorb)) {
			drupal_goto();
		}
	}

	
	
	if(!empty($availableforall) || !empty($centavailableforall)) {	
	
		$newbringliveryarray = $availableforall;
		$newcentralpickuparray = $centavailableforall;

		
		$form['notice']['#markup'] = '<div class="shippinginfo"><p>' . t('Please decide for a type of shipping for your order. Afterwards, please click "@continue".', array('@continue' => t('Continue'))) . '</p></div>';
		
		$form['regiomino_cart']['cart_contents']['all'] = array(
			'#type' => 'fieldset', 
			'#title' => '',
		);
	
		
		$query = new EntityFieldQuery;
		$shippingtype_ids = $query
			->entityCondition('entity_type', 'node')
			->propertyCondition('type', 'deliverytype')
			->fieldOrderBy('field_weight', 'value')
			->execute();

		$shipping_types = entity_load('node', array_keys($shippingtype_ids['node']));
		$shipping = array();
		$deactivate = array();
		foreach($shipping_types as $key=>$value) {
			if($value->status == 0) $deactivate[] = $value->field_deliverykey[LANGUAGE_NONE][0]['value'];
			$imgstyle = 'paymenttype_logo';
			$notavailable = '';
			if(in_array($value->field_deliverykey[LANGUAGE_NONE][0]['value'], $deactivate)) {
				$notavailable = ' (' . t('Available shortly') . ')';
				$imgstyle = 'paymenttype_logo_disabled';
			}
			$shipping[$value->field_deliverykey[LANGUAGE_NONE][0]['value']] = '<img src="' . image_style_url($imgstyle, $value->field_logo[LANGUAGE_NONE][0]['uri']) . '" alt="'. $value->title . '" />&nbsp;&nbsp;&nbsp;<strong>'. $value->title . '</strong>' . $notavailable . '<p class="paymenttypedescription">' . $value->body[LANGUAGE_NONE][0]['value'] . '</p>';
		}

		//Delete bringlivery as an option if no bringlivery men are available
		if(empty($newbringliveryarray)) {
			$deactivate[] = 'bringlivery';
		}
		if(empty($newcentralpickuparray)) {
			$deactivate[] = 'centralpickup';
		}
		
		// $shippingtypes = array();
		// if(!empty($temp)) {
			// foreach($temp as $shippingtypeskey=>$shippingtypesvalue) {
				// $shippingtypes[$shippingtypeskey] = t($shippingtypesvalue);
			// }
		// }
		// $disabledarray = array('centralpickup');
		// $shippingtypes['centralpickup'] .= ' (' . t('Available shortly') . ')';
		
		foreach($warenkorb as $key=>$value) {
			$my_seller = $key;
			foreach($value['product'] as $productkey => $productvalue) {
				$my_cart_item_id = $productkey;
			}		
		}
		
		//Retrieve shipping type for cart item id from db and write in variable that will be used as #default_value			
		$shippingvalue = $warenkorb[$my_seller]['product'][$my_cart_item_id]['shipping_type'];
		//Retrieve shipping type for cart item id from db and write in variable that will be used as #default_value		
		$shippingoption = $warenkorb[$my_seller]['product'][$my_cart_item_id]['shipping_option'];
 		reset($newbringliveryarray);
		reset($newcentralpickuparray);
		if($shippingoption == 'default') {
			$shippingoption = key($newbringliveryarray);
			$pickupoption = key($newcentralpickuparray);
		}
		if($_SESSION['geolocation_data']['type'] == 'centralpickup') {
			$pickupoption = $_SESSION['geolocation_data']['deliveryoption'];
		}
		else {
			$shippingoption = $_SESSION['geolocation_data']['deliveryoption'];
		}
		if($shippingvalue == 'default') $shippingvalue = $_SESSION['geolocation_data']['type'];
		if(empty($newbringliveryarray)) $shippingvalue = 'centralpickup';
		
		//Construct form element
		$form['regiomino_cart']['cart_contents']['all']['shipping'] = array(
			'#type' => 'radios',
			'#default_value' => $shippingvalue,
			'#options' => $shipping,
			'#required' => TRUE,
			'#disabled_options' => $deactivate,
		);


		//Use form element prefix and suffix for adding markup
		$form['regiomino_cart']['cart_contents']['all']['bringlivery'] = array(
			'#type' => 'select',
			'#title' => t('When would you like to receive the products?'),
			'#options' => $newbringliveryarray,
			'#states' => array(
				'visible' => array(
					':input[name="shipping"]' => array('value' => 'bringlivery'),
				),
				'required' => array(
					':input[name="shipping"]' => array('value' => 'bringlivery'),
				),
			),
		);
		if(!empty($newbringliveryarray)) {
			$form['regiomino_cart']['cart_contents']['all']['bringlivery']['#default_value'] = $shippingoption;
		}
		
		//Use form element prefix and suffix for adding markup
		$form['regiomino_cart']['cart_contents']['all']['centralpickup'] = array(
			'#type' => 'radios',
			'#title' => t('When would you like to pickup your products?'),
			'#options' => $newcentralpickuparray,
			'#states' => array(
				'visible' => array(
					':input[name="shipping"]' => array('value' => 'centralpickup'),
				),
				'required' => array(
					':input[name="shipping"]' => array('value' => 'centralpickup'),
				),
			),
		);
		if(!empty($newcentralpickuparray)) {
			$form['regiomino_cart']['cart_contents']['all']['centralpickup']['#default_value'] = $pickupoption;
		}
	}
	else {
		//Display notice
		$form['notice']['#markup'] = '<div class="shippinginfo"><p>' . t('Unfortunately the products you selected cannot be delivered at the same time. Please decide for a type of shipping for each of the sellers from which you have products in your cart. Afterwards, please click "@continue".', array('@continue' => t('Continue'))) . '</p></div>';

		//Initialise helper variables
		$first = TRUE;
		$topborder = "";
		//Step through cart array
		foreach($warenkorb as $key=>$value) {			
			//Set markup for css
			if(!$first) $topborder = " topborder";
			$first = FALSE;

			$form['regiomino_cart']['cart_contents'][$key] = array(
				'#type' => 'fieldset',
			);
			
			
			$form['regiomino_cart']['cart_contents'][$key]['productliststart' . $productkey] = array(
				'#markup' => '<p>' . t('Shipping for articles sold by <strong>@seller</strong> in @location', array('@seller' => $value['profile']['users_profile_title'], '@location' => $value['profile']['ort'])) . '</p><ul>',
			);
			//Step through all products from this seller to create table rows and form elements for cart display
			foreach($value['product'] as $productkey => $productvalue) {
				//Check availability of product
				$nodeobject = node_load($productvalue['nid']);
				$availability = regiomino_offer_availability($nodeobject, $productvalue['menge']);
				//Store stock return value from availability check
				$stock_array = $availability->stock;
				//Perform different actions based on availability of selected products
				switch($availability->available) {
					//Actions, if product is not available anymore
					case 0:
						//Set message
						drupal_set_message($availability->message, "error");
						//Set selected amount to 0
						$productvalue['menge'] = end($stock_array);
						break;
					//Actions, if product has a reduced availability
					case 1:
						//Set message
						drupal_set_message(t("Unfortunately the amount you selected for <strong>@product</strong> is not available anymore. We have updated your selection to the maximum available quantity. Please check if that's alright for you, before you proceed.", array('@product' => $productvalue['title'])), "error");
						//Set selected amount to maximum available quantity
						$productvalue['menge'] = end($stock_array);
						break;
				}
				//Get unformatted discounted product price
				$preis = regiomino_offer_get_discountedprice($nodeobject);
				//Get formatted discounted product price
				$productvalue['preis'] = regiomino_offer_get_discountedprice($nodeobject, TRUE);
				//Get product total by multiplying selected amount with unformatted discounted product price
				$productvalue['gesamt'] = $productvalue['menge'] * $preis;
				//Get included VAT
				$productvalue['includedvat'] = $productvalue['mwst_wert'] * $preis / ($productvalue['mwst_wert'] + 100);
				//Construct form element for changing quantity of selected products
				$form['regiomino_cart']['cart_contents'][$key]['amount' . $productkey] = array(
					'#markup' => $productvalue['menge'] . ' x ' . $productvalue['einheit'] . ' ',
				);
				//Use form element prefix for adding markup and product information
				$form['regiomino_cart']['cart_contents'][$key]['amount' . $productkey]['#prefix'] = '
					<li><strong>' . $productvalue['title'] . '</strong> (';
				//Use form element suffix for adding markup and product information
				$form['regiomino_cart']['cart_contents'][$key]['amount'. $productkey]['#suffix'] = 
					t('for @price / @unit incl. @perc sales tax', array('@price' => $productvalue['preis'], '@unit' => $productvalue['einheit'], '@perc' => $productvalue['mwst'])) . ')</li>
				';
			}
			$form['regiomino_cart']['cart_contents'][$key]['productlistend' . $productkey] = array(
				'#markup' => '</ul>',
			);

			
			/* //Get all available bringlivery options
			$bringliveryview = views_get_view('available_bringlivery_men');
			//Filter out all bringlivery options that are passed one day after the timeout of the earliest article
			$durationarray = array();
			foreach($value['product'] as $dkey => $dvalue) {
				$durationarray[] = strtotime('+2 days', strtotime($dvalue['dauer']));
			}
			$bringliveryview->display['default']->display_options['filters']['field_deliveryoptions_value']['value']['value'] = date('Y-m-d', $durationarray[0]);
			
			//Ausführen des Views
			$bringliveryview->execute();

			//Abfangen der Ergebnisse
			$bringliveryresult = $bringliveryview->result;

			//Weiterverarbeitung der Ergebnisse
			global $user;
			$userobject = user_load($user->uid);
			$country = $userobject->field_address[LANGUAGE_NONE][0]['country'];
			$zip = $userobject->field_address[LANGUAGE_NONE][0]['postal_code'];
			$sellerprofile = node_load($value['profile']['profil_nid']);
			$sellerzip = $sellerprofile->field_address[LANGUAGE_NONE][0]['postal_code'];
			$newbringliveryarray = array();
			foreach($bringliveryresult as $bringliverykey => $bringliveryvalue) {
				$sellerzipok = FALSE;
				$userzipok = FALSE;
				//Alle Ergebnisse herausfiltern, die nicht in die PLZ Bereiche des Users fallen
				if(isset($bringliveryvalue->field_field_deliveryareas) && !empty($bringliveryvalue->field_field_deliveryareas)) {
					foreach($bringliveryvalue->field_field_deliveryareas as $areakey => $areavalue) {
						if(substr($zip, 0, strlen($areavalue['raw']['postal_code'])) === $areavalue['raw']['postal_code']) {
							$userzipok = TRUE;
						}
						if(substr($sellerzip, 0, strlen($areavalue['raw']['postal_code'])) === $areavalue['raw']['postal_code']) {
							$sellerzipok = TRUE;
						}
					}
				}
				if($sellerzipok && $userzipok) {
					//Menge der aktuellen Beauftragungen für Lieferant auslesen und gegen das Feld Kapazität prüfen
					$capacity = $bringliveryvalue->field_field_deliverycapacity[0]['raw']['value'];
					$deliverytimestart = $bringliveryvalue->field_field_deliveryoptions[0]['raw']['value'];
					$deliverytimeend = $bringliveryvalue->field_field_deliveryoptions[0]['raw']['value2'];
					$capaquery = db_select('regiomino_order_shipping', 'r')
						->fields('r', array('shipping_id'))
						->condition('shipping_range_from', $deliverytimestart)
						->execute()
						->rowCount();
					if($capaquery < $capacity - $capacounter) {
						if($bringliveryvalue->field_field_price_per_shipment[0]['raw']['value'] == '0.00') {
							$bringliveryprice = t('free');
						}
						else {
							$bringliveryprice = $bringliveryvalue->field_field_price_per_shipment[0]['rendered']['#markup'];
						}
						$newbringliveryarray[$bringliveryvalue->users_node_uid . '-' . $deliverytimestart . '-' . $deliverytimeend . '-' . $bringliveryvalue->field_field_price_per_shipment[0]['raw']['value']] = t('!timewindow by <a href="@profileurl">@shipper</a> (<em>!price</em>)', array(
							'!timewindow' => $bringliveryvalue->field_field_deliveryoptions[0]['rendered']['#markup'],
							'@profileurl' => '/node/' . $bringliveryvalue->nid,
							'@shipper' => $bringliveryvalue->node_title,
							'!price' => $bringliveryprice,
						));
					}
				}
			}
			$capacounter++; */
			
			

			
			
			
			
			$newbringliveryarray = array();
			$newbringliveryarray = regiomino_shipping_get_available_pickupdates($value['product'], 'bringlivery', TRUE);
			$newcentralpickuparray = array();
			$newcentralpickuparray = regiomino_shipping_get_available_pickupdates($value['product'], 'centralpickup', TRUE);

			$query = new EntityFieldQuery;
			$shippingtype_ids = $query
				->entityCondition('entity_type', 'node')
				->propertyCondition('type', 'deliverytype')
				->fieldOrderBy('field_weight', 'value')
				->execute();

			$shipping_types = entity_load('node', array_keys($shippingtype_ids['node']));
			$shipping = array();
			$deactivate = array();
			foreach($shipping_types as $key2=>$value2) {
				if($value2->status == 0) $deactivate[] = $value2->field_deliverykey[LANGUAGE_NONE][0]['value'];
				$imgstyle = 'paymenttype_logo';
				$notavailable = '';
				if(in_array($value2->field_deliverykey[LANGUAGE_NONE][0]['value'], $deactivate)) {
					$notavailable = ' (' . t('Available shortly') . ')';
					$imgstyle = 'paymenttype_logo_disabled';
				}
				$shipping[$value2->field_deliverykey[LANGUAGE_NONE][0]['value']] = '<img src="' . image_style_url($imgstyle, $value2->field_logo[LANGUAGE_NONE][0]['uri']) . '" alt="'. $value2->title . '" />&nbsp;&nbsp;&nbsp;<strong>'. $value2->title . '</strong>' . $notavailable . '<p class="paymenttypedescription">' . $value2->body[LANGUAGE_NONE][0]['value'] . '</p>';
			}

			//Delete bringlivery as an option if no bringlivery men are available
			if(empty($newbringliveryarray)) {
				$deactivate[] = 'bringlivery';
			}
			if(empty($newcentralpickuparray)) {
				$deactivate[] = 'centralpickup';
			}
			
			//Retrieve selected shipping type for use in #default_value key of form element
			foreach($value['product'] as $fkey => $fvalue) {
				//Since shipping is the same for all items of this current seller, it doesn't matter which one we take.
				//Just loop through all cart item id's and take the last.
				$my_cart_item_id = $fkey;
			}
			//Retrieve shipping type for cart item id from db and write in variable that will be used as #default_value			
			$shippingvalue = $value['product'][$my_cart_item_id]['shipping_type'];
			//Retrieve shipping type for cart item id from db and write in variable that will be used as #default_value		
			$shippingoption = $value['product'][$my_cart_item_id]['shipping_option'];
			reset($newbringliveryarray);
			if($shippingoption == 'default') {
				$shippingoption = key($newbringliveryarray);
				$pickupoption = key($newcentralpickuparray);
			}
			if($shippingvalue == 'default') $shippingvalue = 'bringlivery';
			if(empty($newbringliveryarray)) $shippingvalue = 'centralpickup';
			
			
			//Construct form element
			$form['regiomino_cart']['cart_contents'][$key]['shipping' . $key] = array(
				'#type' => 'radios',
				'#options' => $shipping,
				'#default_value' => $shippingvalue,
				'#disabled_options' => $deactivate,
				'#required' => TRUE,
			);
		
			//Use form element prefix and suffix for adding markup
			$form['regiomino_cart']['cart_contents'][$key]['bringlivery' . $key] = array(
				'#type' => 'select',
				'#title' => t('When would you like to receive the products?'),
				'#options' => $newbringliveryarray,
				'#states' => array(
					'visible' => array(
						':input[name="shipping' . $key . '"]' => array('value' => 'bringlivery'),
					),
					'required' => array(
						':input[name="shipping' . $key . '"]' => array('value' => 'bringlivery'),
					),
				),
			);
			if(!empty($newbringliveryarray)) $form['regiomino_cart']['cart_contents'][$key]['bringlivery' . $key]['#default_value'] = $shippingoption;
			
			//Use form element prefix and suffix for adding markup
			$form['regiomino_cart']['cart_contents'][$key]['centralpickup' . $key] = array(
				'#type' => 'radios',
				'#title' => t('When would you like to pickup your products?'),
				'#options' => $newcentralpickuparray,
				'#states' => array(
					'visible' => array(
						':input[name="shipping' . $key . '"]' => array('value' => 'centralpickup'),
					),
					'required' => array(
						':input[name="shipping' . $key . '"]' => array('value' => 'centralpickup'),
					),
				),
			);
			if(!empty($newcentralpickuparray)) $form['regiomino_cart']['cart_contents'][$key]['centralpickup' . $key]['#default_value'] = $pickupoption;
	/* 		$form['regiomino_cart']['cart_contents'][$key]['pickup' . $key] = array(
				'#type' => 'select',
				'#title' => t(''),
				'#options' => $avlbpickupdates,
				'#states' => array(
					'visible' => array(
						':input[name="shipping' . $key . '"]' => array('value' => 'pickup'),
					),
					'required' => array(
						':input[name="shipping' . $key . '"]' => array('value' => 'pickup'),
					),
				),
			);
			if(!empty($avlbpickupdates)) $form['regiomino_cart']['cart_contents'][$key]['pickup' . $key]['#default_value'] = $shippingoption; */
			
			
			
		}
	}
	//Form submit
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Continue'),
	);
	$form['submit']['#prefix'] = '<a href="/cart">' . t('Back') . '</a> ';
	return $form;
}

function regiomino_cart_shippinginfo_form_validate($form, &$form_state) {
	$toparray = array();
	foreach($form_state['values'] as $key => $value) {
		$temp = explode('shipping', $key);
		if(isset($temp[1])) $toparray[] = $temp[1];
	}
	$array = array();
	foreach($toparray as $key => $value) {
		$array[$key][$value] = $form_state['values']['shipping' . $value];
	}
	foreach($array as $key => $value) {
		foreach($value as $subkey => $subvalue) {
			$createdstring = $subvalue . $subkey;
			if($form_state['values'][$createdstring] == 0) form_set_error($createdstring, t('Please make sure you check all necessary suboptions.'));
		}
	}
}

function regiomino_cart_shippinginfo_form_submit($form, &$form_state) {
	//Update shipping methods
	$shippingarray = array();
	foreach($form_state['values'] as $key=>$value) {

		$temp = explode('shipping', $key);

		if(isset($temp[1])) {
			switch($value) {
				case 'centralpickup':
					$option = $form_state['values']['centralpickup' . $temp[1]];
					break;
				case 'bringlivery':
					$isbringlivery = TRUE;
					$option = $form_state['values']['bringlivery' . $temp[1]];
					break;
			}
			$shippingarray[$temp[1]][$value] = $option;
		}
	}

	$nidshipmentarray = array();
	$warenkorb = regiomino_cart_load_cart();
	foreach($shippingarray as $skey=>$svalue) {
		foreach($warenkorb as $wkey=>$wvalue) {
			if($skey == $wkey || $skey == '') {
				foreach($wvalue['product'] as $fkey=>$fvalue) {
					foreach($svalue as $onekey=>$onevalue) {
						$nidshipmentarray[$fkey][$onekey] = $onevalue;
					}
				}
			}
		}
	}
	foreach($nidshipmentarray as $keycartid=>$valueshipping) {
		foreach($valueshipping as $keyshippingtype=>$valueshippingoption) {
			regiomino_cart_update_cart($keycartid, array('shipping_type' => $keyshippingtype, 'shipping_option' => $valueshippingoption));
			// db_update('regiomino_cart')
			// ->fields(
				// array(
					// 'shipping_type' => $keyshippingtype,
					// 'shipping_option' => $valueshippingoption,
					// 'changed' => time(),
				// )
			// )
			// ->condition('cart_item_id', $keycartid)
			// ->execute();
		}
	}

	//Find out if user is logged in
	global $user;
	if ($user->uid) {
		$loggedin = TRUE;
	}
	else {
		$loggedin = FALSE;
	}
	//Clear cart cache for this user
	//cache_clear_all('cart' . $cart_id, 'cache');
	drupal_goto('checkout/shippingaddress');

}