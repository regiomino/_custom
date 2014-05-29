<?php

/**
* Submit procedures of regiomino_cart_form.
*/
function regiomino_cart_form_submit($form, &$form_state) {

	//Determine amounts
	$amountarray = array();
	foreach($form_state['values'] as $key=>$value) {
		$temp_amount = explode('amount', $key);
		if(isset($temp_amount[1])) {
			$amountarray[$temp_amount[1]]['qty'] = $value;
		}
		$temp_frequency = explode('frequency', $key);
		if(isset($temp_frequency[1])) {
			$amountarray[$temp_frequency[1]]['frequency'] = $value;
		}
		$temp_comments = explode('comments', $key);
		if(isset($temp_comments[1])) {
			$amountarray[$temp_comments[1]]['comments'] = $value;
		}
	}

	foreach($amountarray as $key=>$value) {
		if($value['qty']==0) {
			regiomino_cart_remove_product($key, '', 'status', '', FALSE);
		} else {
			regiomino_cart_update_cart($key, $value);
		}
	}

	
	if($form_state['values']['op'] == $form_state['values']['continue']) {
		drupal_goto('checkout/address');
	}
	drupal_set_message(t("Your cart was updated"));
}

/**
 * Constructs cart form
 */
function regiomino_cart_form($form, &$form_state) {
	//Helper variable for adding up cart total values
	$valueaddup = 0;
	$vataddup = 0;
	global $user;
	
/*
	if ($user->uid == 1) {
		regiomino_order_dothis(350, 19.4);
	}
	*/

	if ($user->uid) {
		$cart_id = $user->uid;
	}
	else {
		$cart_id = session_id();
	}
	//Load cart
	$empty = regiomino_cart_empty_cart($cart_id);
	//Set message if cart is empty
	if($empty) {
		drupal_set_message(t('Your cart is empty'), 'warning');
	}
	//Construct form if cart contains products
	else {
		//Initialise helper variables
		$warenkorb = regiomino_cart_load_cart();

		//Step through cart array
		foreach($warenkorb as $key=>$value) {
			
			//Step through all products from this seller to create table rows and form elements for cart display
			foreach($value['product'] as $productkey => $productvalue) {
			
				//Check availability of product
				$nodeobject = $productvalue['nodeobject'];
				$availability = regiomino_offer_availability($nodeobject, $productvalue['menge']);
				//Store stock return value from availability check
				$stock_array = $availability->stock;
				//Perform different actions based on availability of selected products
				switch($availability->available) {
					//Actions, if product is not available anymore
					case 0:
						//Remove product from cart
						regiomino_cart_remove_product($productkey, $availability->message, "error", 'cart');
						break;
					//Actions, if product has a reduced availability
					case 1:
						//Set message
						drupal_set_message(t("Unfortunately the amount you selected for <strong>@product</strong> is not available anymore. We have updated your selection to the maximum available quantity. Please check if that's alright for you, before you proceed.", array('@product' => $productvalue['title'])), "error");
						//Set selected amount to maximum available quantity
						$productvalue['menge'] = end($stock_array);
						break;
				}
				
				$field = field_info_field('field_frequency');
				$allowed_values = list_allowed_values($field);
				$freqdisabled = TRUE;
				if($productvalue['nodeobject']->field_renewal[LANGUAGE_NONE][0]['value']) {
					$freqdisabled = FALSE;
				}
				
				//Construct form element for changing quantity of selected products
				$form['regiomino_cart']['cart_contents']['frequency' . $productkey] = array(
					'#type' => 'select',
					'#options' => $allowed_values,
					'#default_value' => $productvalue['frequency'],
					'#disabled' => $freqdisabled,
				);
				//Use form element prefix for adding markup and product information
				
				//Construct form element for changing quantity of selected products
				$form['regiomino_cart']['cart_contents']['amount' . $productkey] = array(
					'#type' => 'select',
					'#options' => $stock_array,
					'#default_value' => $productvalue['menge'],
				);
				
				//Construct form element for changing quantity of selected products
				$form['regiomino_cart']['cart_contents']['comments' . $productkey] = array(
					'#type' => 'textarea',
					'#default_value' => $productvalue['comments'],
				);
			}
		}
		//Use cart suffix for displaying summary infos
		
		//Submit button for refreshing values
		$form['regiomino_cart']['submit'] = array(
			'#type' => 'submit',
			'#value' => t('Update cart'),
		);
		//Additional button for submitting and continuing checkout process
 		$form['regiomino_cart']['continue'] = array(
			'#type' => 'submit',
			'#executes_submit_callback' => TRUE, 
			'#value' => t('Proceed to checkout'),
		);

	}	
		//drupal_set_message('<h2>Der Kaufprozess ist derzeit geschlossen</h2><p>Mit dem Verkaufsstart in Bamberg am 19. September kann wieder wie gewohnt bestellt werden, mit breiterem Angebot und neuen Funktionen.</p>', 'error');
	return $form;
}