<?php

function regiomino_cart_addressinfo() {

/* 	$warenkorb = regiomino_cart_load_cart();
	//Set message if cart is empty
	if(empty($warenkorb)) {
		drupal_goto('cart');
	} */
	
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
	if(!$loggedin) {
		drupal_goto('user/register', array('query' => drupal_get_destination()));
	}
	else {
		//If billing address has not been set yet, fill it with values from shipping address
		$userobject = user_load($user->uid);
		$billingaddressitems = field_get_items('user', $userobject, 'field_billingaddress');
		if($billingaddressitems[0]['locality'] == '') {
			$addressitems = field_get_items('user', $userobject, 'field_shippingaddress');
			$userobject->field_billingaddress[LANGUAGE_NONE] = $addressitems;
		}
		user_save($userobject);
		drupal_goto('checkout/shipping');
	}
}