<?php


function regiomino_cart_paymentinfo() {

	//Make sure checkout is a process of steps, no manual jumps
	global $base_url;
/* 	$force_referer[] = 'checkout/shipping';
	$force_referer[] = 'checkout/payment';
	$force_referer[] = 'checkout/verify';
	$referer = str_replace($base_url . '/', "", $_SERVER['HTTP_REFERER']);

	if($referer != $force_referer[0] && $referer != $force_referer[1] && $referer != $force_referer[2]) {
		drupal_set_message(t('Please follow the steps of the checkout process one after another'), 'warning');
		drupal_goto('checkout/address');
	} */
	
	
	  $coopprofile = node_load(3524);
		//var_dump($coopprofile->field_region);
	
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
	
	$content = drupal_get_form('regiomino_cart_paymentinfo_form');	
	return	theme('regiomino_cart_theme_payment', array(
						'vars' => array(
							'form' => $content,
						),
					));
}

function regiomino_cart_paymentinfo_form($form, &$form_state) {
	
	drupal_set_title('');
	
	$warenkorb = regiomino_cart_load_cart();
	$valueaddup = 0;
	$orderaddup = 0;
	$vataddup = 0;
	$shipping = 0;
	$counter = 1;
	$includessubscription = FALSE;
	foreach($warenkorb as $verkaeuferkey=>$verkaeufervalue) {
	
		//Retrieve selected shipping type for use in #default_value key of form element
		foreach($verkaeufervalue['product'] as $fkey => $fvalue) {
			//Since shipping is the same for all items of this current seller, it doesn't matter which one we take.
			//Just loop through all cart item id's and take the last.
			$my_cart_item_id = $fkey;
		}
		//Retrieve shipping type for cart item id from db and write in variable			
		$shippingvalue = $verkaeufervalue['product'][$my_cart_item_id]['shipping_type'];
		//Retrieve shipping type for cart item id from db and write in variable	
		$shippingoption = $verkaeufervalue['product'][$my_cart_item_id]['shipping_option'];
		//Select chosen delivery options
		$shippingoptionarray = explode("-", $shippingoption);
		$shipping += $shippingoptionarray[5];
		
		foreach($verkaeufervalue['product'] as $productkey=>$productvalue) {
			if($productvalue['frequency'] > 0) $includessubscription = TRUE;
			$valueaddup += $productvalue['gesamt'];
			$vataddup += $productvalue['gesamt_mwst'];
			$counter++;
		}	
	}
	//Sum up costs for shipping
	$shippingtax = 19;
	$shippingtaxamount = $shipping * $shippingtax / ($shippingtax + 100);
	$valueaddup += $shipping;
	$vataddup += $shippingtaxamount;
	$orderaddup += $valueaddup;

	global $user;
	$userobject = user_load($user->uid);
	$balance = (float)userpoints_get_current_points($user->uid, 446);
	if($balance < 0) $balance = 0;
	if($balance > $orderaddup) {
		$usedbalance = $orderaddup;
	}
	else {
		$usedbalance = $balance;
	}
	$leftoverbalance = $balance - $usedbalance;
	$topay = $orderaddup - $balance;
	if($topay < 0) $topay = 0;

	$query = new EntityFieldQuery;
	$paymenttype_ids = $query
		->entityCondition('entity_type', 'node')
		->propertyCondition('type', 'paymenttype')
		->fieldOrderBy('field_weight', 'value')
		->execute();

	$payment_types = entity_load('node', array_keys($paymenttype_ids['node']));
	$payment = array();
	$deactivate = array();
	foreach($payment_types as $key=>$value) {
		$imgstyle = 'paymenttype_logo';
		$notavailable = '';
		if($value->status == 0) {
			$deactivate[] = $value->field_paymentkey[LANGUAGE_NONE][0]['value'];
			$notavailable = ' (' . t('Available shortly') . ')';
			$imgstyle = 'paymenttype_logo_disabled';
		}
		if(!$includessubscription || $value->field_paymentkey[LANGUAGE_NONE][0]['value'] == 'debit') {
			$payment[$value->field_paymentkey[LANGUAGE_NONE][0]['value']] = '<img src="' . image_style_url($imgstyle, $value->field_logo[LANGUAGE_NONE][0]['uri']) . '" alt="'. $value->title . '" />&nbsp;&nbsp;&nbsp;<strong>'. $value->title . '</strong>' . $notavailable . '<p class="paymenttypedescription">' . $value->body[LANGUAGE_NONE][0]['value'] . '</p>';
		}
	}

	//Display notice
	//$form['notice']['#markup'] = '<div class="paymentinfo"></div>';
	
/* 	$form['notice']['#markup'] = '
		<div style="float: right;"><table id="checkoutinvoice">
		<tr>
			<td class="desc">Gesamtbetrag der Bestellung</td><td class="numbers">' . number_format($orderaddup, 2, ",", ".") . ' &euro;</td>
		</tr>
		<tr>
			<td class="desc">' . t('Used balance') . '</td><td class="numbers">' . number_format(-$usedbalance, 2, ",", ".") . ' &euro;</td>
		</tr>
		<tr class="total">
			<td class="desc">' . t('Total to be paid') . '</td><td class="numbers">' . number_format($topay, 2, ",", ".") . ' &euro;</td>
		</tr>
		</table></div>
		<!--<p>Nach dieser Bestellung verbleibt Ihnen ein Guthaben von ' . number_format($leftoverbalance, 2, ",", ".") . ' &euro;</p>-->
	'; */
			
	if($topay) {
		$form['notice']['#markup'] .= '<div class="paymentinfo"><p>' . t('Please decide how you would like to pay for your order. Afterwards, please click "@continue". On the next page you can still check everything before placing your order.', array('@continue' => t('Continue'))) . '</p></div>';
		
		$form['bof_checkout'] = array(
			'#type' => 'fieldset', 
			'#title' => '',
		);
		
		$form['bof_checkout']['payment'] = array(
			'#type' => 'radios',
			'#options' => $payment,
			'#disabled_options' => $deactivate,
			'#required' => TRUE,
		);
		
		if($includessubscription) {
			$form['bof_checkout']['payment']['#default_value'] = 'debit';
		}
		
		$form['bof_checkout']['directdebit']['bankaccountholder'] = array(
			'#type' => 'textfield',
			'#title' => t('Inhaber des Kontos'),
			'#default_value' => $userobject->field_bankaccountholder[LANGUAGE_NONE][0]['value'],
			'#states' => array(
				'visible' => array(
					':input[name="payment"]' => array('value' => 'debit'),
				),
				'required' => array(
					':input[name="payment"]' => array('value' => 'debit'),
				),
			),
		);
		$form['bof_checkout']['directdebit']['bankaccount'] = array(
			'#type' => 'textfield',
			'#title' => t('Kontonummer'),
			'#default_value' => $userobject->field_bankaccount[LANGUAGE_NONE][0]['value'],
			'#states' => array(
				'visible' => array(
					':input[name="payment"]' => array('value' => 'debit'),
				),
				'required' => array(
					':input[name="payment"]' => array('value' => 'debit'),
				),
			)
		);
		$form['bof_checkout']['directdebit']['bankcode'] = array(
			'#type' => 'textfield',
			'#title' => t('BLZ'),
			'#default_value' => $userobject->field_bankcode[LANGUAGE_NONE][0]['value'],
			'#states' => array(
				'visible' => array(
					':input[name="payment"]' => array('value' => 'debit'),
				),
				'required' => array(
					':input[name="payment"]' => array('value' => 'debit'),
				),
			)
		);
	}
	else {
		$form['notice']['#markup'] .= '<div class="paymentinfo"><p>' . t('Your balance is sufficient to cover the total amount of your order. Please continue by clicking "@continue". On the next page you can still check everything before placing your order.', array('@continue' => t('Continue'))) . '</p></div>';
	}
	
	
	
	//Form submit
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Continue'),
	);
	$form['submit']['#prefix'] = '<a href="/checkout/shipping">' . t('Back') . '</a> ';
	return $form;
}

function regiomino_cart_paymentinfo_form_validate($form, &$form_state) {
	//Validate that billing address fields are actually filled in, when different address is requested
	if($form_state['values']['payment'] == 'debit') {
		if(empty($form_state['values']['bankaccountholder'])) {
			form_set_error('bankaccountholder', t('The field %field is required.', array('%field' => $form['bof_checkout']['directdebit']['bankaccountholder']['#title'])));
		}
		if(empty($form_state['values']['bankaccount'])) {
			form_set_error('bankaccount', t('The field %field is required.', array('%field' => $form['bof_checkout']['directdebit']['bankaccount']['#title'])));
		}
		if(empty($form_state['values']['bankcode'])) {
			form_set_error('bankcode', t('The field %field is required.', array('%field' => $form['bof_checkout']['directdebit']['bankcode']['#title'])));
		}
	}
}

function regiomino_cart_paymentinfo_form_submit($form, &$form_state) {
	//Retrieve payment options and make them selectable through form item
	global $user;
	$userobject = user_load($user->uid);
	$userobject->field_use_balance_for_purchase['und'][0]['value'] = $form_state['values']['paywithbalance'];
	if($form_state['values']['paywithbalance'] == '1') {
		$userobject->field_payment['und'][0]['value'] = $form_state['values']['paymentbalance'];
	}
	else {
		$userobject->field_payment['und'][0]['value'] = $form_state['values']['payment'];
	}
	$userobject->field_bankaccountholder[LANGUAGE_NONE][0]['value'] = $form_state['values']['bankaccountholder'];
	$userobject->field_bankaccount[LANGUAGE_NONE][0]['value'] = $form_state['values']['bankaccount'];
	$userobject->field_bankcode[LANGUAGE_NONE][0]['value'] = $form_state['values']['bankcode'];
	
	user_save($userobject);

	drupal_goto('checkout/verify');
}