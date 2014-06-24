<?php

function regiomino_cart_shippingaddressinfo() {

	//Make sure checkout is a process of steps, no manual jumps
	global $base_url;
	$force_referer[] = '/checkout/shippingaddress';
	$force_referer[] = '/checkout/shipping';
	$force_referer[] = '/checkout/payment';
	$parsedvalue = parse_url($_SERVER['HTTP_REFERER']);
	$referer = $parsedvalue['path'];
	if(!in_array($referer, $force_referer)) {
		drupal_set_message(t('Please follow the steps of the checkout process one after another'), 'warning');
		drupal_goto('checkout/shipping');
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
	$content = drupal_get_form('regiomino_cart_shippingaddressinfo_form');
	return	theme('regiomino_cart_theme_shippingaddress', array(
						'vars' => array(
							'form' => $content,
						),
					));
}


function regiomino_cart_shippingaddressinfo_form($form, &$form_state) {

// db_query('UPDATE {regiomino_order}, {regiomino_order_shipping} SET order_status = :processing WHERE shipping_id = :shipping_id AND shipping_id = :shipping', array(':processing' => 'processing', ':shipping_id' => 302, ':shipping' => 'shipping'));

//db_query('UPDATE {regiomino_order}, {regiomino_order_shipping} SET order_status = :status WHERE shipping_id = :shipping_id AND shipping_id = shipping', array(':status' => 'completed', ':shipping_id' => 302,));

	//Form submit
	global $user;
		
	$shippingkey = regiomino_cart_get_shippingkey($user->uid);
	
	$userobject = user_load($user->uid);
	$address = $userobject->field_shippingaddress['und'][0];
	$billingaddress = $userobject->field_billingaddress['und'][0];
	$different = 0;
	$diff = array_diff($billingaddress, $address);
	if(!empty($diff)) $different = 1;

	//Display notice
	//$form['notice']['#markup'] = '<div class="messages status">' . t('Please provide and confirm your delivery address. If you wish to enter a separate address for billing you can do so at the bottom of the form. If everything is alright, please click @continue', array('@continue' => t('Continue'))) . '</div>';
	drupal_set_title('');
	if($shippingkey != 'centralpickup') {
		$form['notice']['#markup'] = '<div class="shippingaddressinfo"><p>' . t('Please provide and confirm your delivery address. If you wish to enter a separate address for billing you can do so at the bottom of the form. If everything is alright, please click @continue', array('@continue' => t('Continue'))) . '</p></div>';
		
		$form['deliveryaddress'] = array(
			'#type' => 'fieldset', 
			//'#title' => t('Delivery address'),
			//'#description' => t('Please provide and confirm your delivery address. If you wish to enter a separate address for billing you can do so at the bottom of the form. If everything is alright, please click @continue', array('@continue' => t('Continue'))),
		);

		$form['deliveryaddress']['deliveryfirstname'] = array(
			'#type' => 'textfield', 
			'#title' => t('First name'), 
			'#default_value' => $address['first_name'], 
			'#size' => 20, 
			'#maxlength' => 64, 
			'#required' => TRUE,
		);
		$form['deliveryaddress']['deliverylastname'] = array(
			'#type' => 'textfield', 
			'#title' => t('Last name'), 
			'#default_value' => $address['last_name'], 
			'#size' => 20, 
			'#maxlength' => 64, 
			'#required' => TRUE,
		);
		$form['deliveryaddress']['deliverystreet1'] = array(
			'#type' => 'textfield', 
			'#title' => t('Address 1'), 
			'#default_value' => $address['thoroughfare'],
			'#size' => 46, 
			'#maxlength' => 128, 
			'#required' => TRUE,
		);
		$form['deliveryaddress']['deliverystreet2'] = array(
			'#type' => 'textfield', 
			'#title' => t('Address 2'), 
			'#default_value' => $address['premise'], 
			'#size' => 46, 
			'#maxlength' => 128, 
			'#required' => FALSE,
		);
		$form['deliveryaddress']['deliveryzip'] = array(
			'#type' => 'textfield', 
			'#title' => t('Postal code'), 
			'#default_value' => $address['postal_code'], 
			'#size' => 20, 
			'#maxlength' => 128, 
			'#required' => TRUE,
		);
		$form['deliveryaddress']['deliverylocality'] = array(
			'#type' => 'textfield', 
			'#title' => t('Locality'), 
			'#default_value' => $address['locality'], 
			'#size' => 20, 
			'#maxlength' => 128, 
			'#required' => TRUE,
		);
		$form['deliveryaddress']['phonenumber'] = array(
			'#type' => 'textfield', 
			'#title' => t('Phone'), 
			'#default_value' => $userobject->field_privatephone['und'][0]['number'], 
			'#size' => 41, 
			'#maxlength' => 15,
			'#field_prefix' => '+49',
			'#required' => TRUE,
			'#description' => t('Please be available under this number during the delivery time frame.'),
		);
		
		$form['deliveryaddress']['differentbilling'] = array(
			'#type' => 'checkbox',
			'#title' => t('I would like my bill to be issued to a different name/address'),
			'#default_value' => $different,
		);
	
	}
	
	if($shippingkey == 'centralpickup') {
		$form['notice']['#markup'] = '<div class="shippingaddressinfo"><p>' . t('Please provide and confirm your billing address. If everything is alright, please click @continue', array('@continue' => t('Continue'))) . '</p></div>';
	}
	$form['billingaddress'] = array(
		'#type' => 'fieldset', 
		//'#description' => t('Please provide a billing address for your invoice.'),
	);
	if($shippingkey != 'centralpickup') {
		$form['billingaddress']['#title'] = t('Billing address');
		$form['billingaddress']['#states']['visible'][':input[name="differentbilling"]']['checked'] = TRUE;
	}
		
		
	$form['billingaddress']['billingfirstname'] = array(
		'#type' => 'textfield', 
		'#title' => t('First name'), 
		'#default_value' => $billingaddress['first_name'], 
		'#size' => 20, 
		'#maxlength' => 64, 
	);
	$form['billingaddress']['billinglastname'] = array(
		'#type' => 'textfield', 
		'#title' => t('Last name'), 
		'#default_value' => $billingaddress['last_name'], 
		'#size' => 20, 
		'#maxlength' => 64, 
	);
	$form['billingaddress']['billingstreet1'] = array(
		'#type' => 'textfield', 
		'#title' => t('Address 1'), 
		'#default_value' => $billingaddress['thoroughfare'], 
		'#size' => 46, 
		'#maxlength' => 128, 
	);
	$form['billingaddress']['billingstreet2'] = array(
		'#type' => 'textfield', 
		'#title' => t('Address 2'), 
		'#default_value' => $billingaddress['premise'], 
		'#size' => 46, 
		'#maxlength' => 128, 
	);
	$form['billingaddress']['billingzip'] = array(
		'#type' => 'textfield', 
		'#title' => t('Postal code'), 
		'#default_value' => $billingaddress['postal_code'], 
		'#size' => 20, 
		'#maxlength' => 128, 
	);
	$form['billingaddress']['billinglocality'] = array(
		'#type' => 'textfield', 
		'#title' => t('Locality'), 
		'#default_value' => $billingaddress['locality'], 
		'#size' => 20, 
		'#maxlength' => 128, 
	);

	if($shippingkey != 'centralpickup') {
		$form['endofform']['alternative'] = array(
			'#type' => 'select',
			'#title' => t('Delivery alternative'),
			'#options' => array(
				'garage' => t('Carport or Garage'),
				'terrace' => t('Terrace'),
				'frontdoor' => t('Leave at the front door'),
				'backdoor' => t('Leave at the backdoor'),
			),
			'#required' => FALSE,
			'#empty_option' => t('- Select -'),
			'#default_value' => $userobject->field_alternativelocation['und'][0]['value'],
			'#description' => t("If your shipper does not reach you, he or she will try to leave your order with your neighbours. If you don't want this, you can select an alternative delivery location. Since this is about your groceries, please make sure the location is in your interest, e.g. safe from external influences."),
		);
	}
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Continue'),
	);
	$form['submit']['#prefix'] = '<a href="/checkout/shipping">' . t('Back') . '</a> ';
	return $form;
}

function regiomino_cart_shippingaddressinfo_form_validate($form, &$form_state) {

	global $user;
		
	$shippingkey = regiomino_cart_get_shippingkey($user->uid);
	
	
	
	$metro_key = $_SESSION['geolocation_data']['metro_key'];
	
	if($shippingkey != 'centralpickup') {

		$err = TRUE;
		if(isset($metro_key)) {
			$query = new EntityFieldQuery;
			$tmp = $query
				->entityCondition('entity_type', 'field_collection_item')
				->fieldCondition('field_metro_key', 'value', $metro_key)
				->execute();
			$alltmpvalues = array_values($tmp['field_collection_item']);
			$fci = field_collection_item_load($alltmpvalues[0]->item_id);
			$myzipkey = '';
			foreach($fci->field_delivery_areas[LANGUAGE_NONE] as $index => $zipcode) {
				if(strpos($form_state['values']['deliveryzip'], $zipcode['value']) !== FALSE) {
					$err = FALSE;
					break;
				}
			}
		}
		else {
			form_set_error('deliveryaddress', t('Please select a region before you check out.'));
		}
	
		if($err) {
			form_set_error('deliveryaddress', t('You have entered a delivery address that differs from your selected zip code region. Please enter a matching delivery address or <a href="@url">change the region</a> (<strong>All products will be removed from you cart</strong>)', array('@url' => url('node', array('query' => array('resetloc' => '1'))))));
		}
	
		//Validate that billing address fields are actually filled in, when different address is requested
		if($form_state['values']['differentbilling']) {
			if(empty($form_state['values']['billingfirstname'])) {
				form_set_error('billingfirstname', t('The field %field is required.', array('%field' => $form['billingaddress']['billingfirstname']['#title'])));
			}
			if(empty($form_state['values']['billinglastname'])) {
				form_set_error('billinglastname', t('The field %field is required.', array('%field' => $form['billingaddress']['billinglastname']['#title'])));
			}
			if(empty($form_state['values']['billingstreet1'])) {
				form_set_error('billingstreet1', t('The field %field is required.', array('%field' => $form['billingaddress']['billingstreet1']['#title'])));
			}
			if(empty($form_state['values']['billingzip'])) {
				form_set_error('billingzip', t('The field %field is required.', array('%field' => $form['billingaddress']['billingzip']['#title'])));
			}
			if(empty($form_state['values']['billinglocality'])) {
				form_set_error('billinglocality', t('The field %field is required.', array('%field' => $form['billingaddress']['billinglocality']['#title'])));
			}
		}
	}
	else {
		if(empty($form_state['values']['billingfirstname'])) {
			form_set_error('billingfirstname', t('The field %field is required.', array('%field' => $form['billingaddress']['billingfirstname']['#title'])));
		}
		if(empty($form_state['values']['billinglastname'])) {
			form_set_error('billinglastname', t('The field %field is required.', array('%field' => $form['billingaddress']['billinglastname']['#title'])));
		}
		if(empty($form_state['values']['billingstreet1'])) {
			form_set_error('billingstreet1', t('The field %field is required.', array('%field' => $form['billingaddress']['billingstreet1']['#title'])));
		}
		if(empty($form_state['values']['billingzip'])) {
			form_set_error('billingzip', t('The field %field is required.', array('%field' => $form['billingaddress']['billingzip']['#title'])));
		}
		if(empty($form_state['values']['billinglocality'])) {
			form_set_error('billinglocality', t('The field %field is required.', array('%field' => $form['billingaddress']['billinglocality']['#title'])));
		}
	}
}
function regiomino_cart_shippingaddressinfo_form_submit($form, &$form_state) {
	//Save values to user object
	global $user;
		
	$shippingkey = regiomino_cart_get_shippingkey($user->uid);
	
	
	if($shippingkey == 'centralpickup') {
		$form_state['values']['deliveryfirstname'] = $form_state['values']['billingfirstname'];
		$form_state['values']['deliverylastname'] = $form_state['values']['billinglastname'];
		$form_state['values']['deliverystreet1'] = $form_state['values']['billingstreet1'];
		$form_state['values']['deliverystreet2'] = $form_state['values']['billingstreet2'];
		$form_state['values']['deliveryzip'] = $form_state['values']['billingzip'];
		$form_state['values']['deliverylocality'] = $form_state['values']['billinglocality'];
		$form_state['values']['differentbilling'] = 1;
	}
	$userobject = user_load($user->uid);
	$userobject->field_shippingaddress['und'][0]['first_name'] = $form_state['values']['deliveryfirstname'];
	$userobject->field_shippingaddress['und'][0]['last_name'] = $form_state['values']['deliverylastname'];
	$userobject->field_shippingaddress['und'][0]['thoroughfare'] = $form_state['values']['deliverystreet1'];
	$userobject->field_shippingaddress['und'][0]['premise'] = $form_state['values']['deliverystreet2'];
	$userobject->field_shippingaddress['und'][0]['postal_code'] = $form_state['values']['deliveryzip'];
	$userobject->field_shippingaddress['und'][0]['locality'] = $form_state['values']['deliverylocality'];
	if($form_state['values']['differentbilling']) {
		$userobject->field_billingaddress['und'][0]['first_name'] = $form_state['values']['billingfirstname'];
		$userobject->field_billingaddress['und'][0]['last_name'] = $form_state['values']['billinglastname'];
		$userobject->field_billingaddress['und'][0]['thoroughfare'] = $form_state['values']['billingstreet1'];
		$userobject->field_billingaddress['und'][0]['premise'] = $form_state['values']['billingstreet2'];
		$userobject->field_billingaddress['und'][0]['postal_code'] = $form_state['values']['billingzip'];
		$userobject->field_billingaddress['und'][0]['locality'] = $form_state['values']['billinglocality'];
	}
	else {
		$userobject->field_billingaddress['und'][0]['first_name'] = '';
		$userobject->field_billingaddress['und'][0]['last_name'] = '';
		$userobject->field_billingaddress['und'][0]['thoroughfare'] = '';
		$userobject->field_billingaddress['und'][0]['premise'] = '';
		$userobject->field_billingaddress['und'][0]['postal_code'] = '';
		$userobject->field_billingaddress['und'][0]['locality'] = '';	
	}
	if($shippingkey != 'centralpickup') {
		$userobject->field_privatephone['und'][0]['number'] = $form_state['values']['phonenumber'];
		$userobject->field_alternativelocation['und'][0]['value'] = $form_state['values']['alternative'];
	}
	user_save($userobject);
	
	$includessubscription = FALSE;
	$warenkorb = regiomino_cart_load_cart();
	foreach($warenkorb as $verkaeuferkey=>$verkaeufervalue) {
		foreach($verkaeufervalue['product'] as $fkey => $fvalue) {
			if($fvalue['frequency'] > 0) $includessubscription = TRUE;
		}
	}
	if($includessubscription) drupal_set_message(t('Due to a subscription item in your cart we can only accept debit payment.'), 'warning');
	
	//Direct to next step
	drupal_goto('checkout/payment');
}