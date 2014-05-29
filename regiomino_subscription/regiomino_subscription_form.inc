<?php

/**
* Submit procedures of regiomino_subscription_form.
*/
function regiomino_subscription_form_submit($form, &$form_state) {

	//Determine amounts
	$amountarray = array();
	foreach($form_state['values'] as $key=>$value) {
		$temp = explode('amount', $key);
		if(isset($temp[1])) {
			$amountarray[$temp[1]] = $value;
		}
	}

	foreach($amountarray as $key=>$value) {
		if($value==0) {
			regiomino_subscription_remove_product($key, '', 'status', '', FALSE);
		} else {
			regiomino_subscription_update_subscription($key, array('qty' => $value));
		}
	}

	drupal_set_message(t("Your subscription was updated"));
}

/**
 * Constructs subscription form
 */
function regiomino_subscription_form($form, &$form_state) {
	//Helper variable for adding up subscription total values
	$valueaddup = 0;
	$vataddup = 0;
	global $user;
	//Load subscription
	$empty = regiomino_subscription_empty_subscription($user->uid);
	//Set message if subscription is empty
	if($empty) {
		drupal_set_message(t('Your subscription is empty'), 'warning');
	}
	//Construct form if subscription contains products
	else {
		//Initialise helper variables
		$abokiste = regiomino_subscription_load_subscription();

		//Step through subscription array
		foreach($abokiste as $key=>$value) {
			//Step through all products from this seller to create table rows and form elements for subscription display
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
						//Remove product from subscription
						regiomino_subscription_remove_product($productkey, $availability->message, "error", 'subscription');
						break;
					//Actions, if product has a reduced availability
					case 1:
						//Set message
						drupal_set_message(t("Unfortunately the amount you selected for <strong>@product</strong> is not available anymore. We have updated your selection to the maximum available quantity. Please check if that's alright for you, before you proceed.", array('@product' => $productvalue['title'])), "error");
						//Set selected amount to maximum available quantity
						$productvalue['menge'] = end($stock_array);
						break;
				}

				//Construct form element for changing quantity of selected products
				$form['regiomino_subscription']['subscription_contents']['amount' . $productkey] = array(
					'#type' => 'select',
					'#options' => $stock_array,
					'#default_value' => $productvalue['menge'],
				);
				
				if(isset($productvalue['pause']) && $productvalue['pause'] == 1) {
					$form['regiomino_subscription']['subscription_contents']['amount' . $productkey]['#disabled'] = TRUE;
				}
			}
		}
		//Submit button for refreshing values
		$form['regiomino_subscription']['submit'] = array(
			'#type' => 'submit',
			'#value' => t('Update subscription'),
		);

	}	
	return $form;
}

function regiomino_subscription_get_subscription() {

	$content = drupal_get_form('regiomino_subscription_form');

	return $content;
}