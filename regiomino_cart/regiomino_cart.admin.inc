<?php

/**
 * @file
 * Administration page callbacks for the regiomino_cart module.
 */

/**
 * Form builder. Configure regiomino_cart
 * @ingroup forms
 * @see system_settings_form()
 */
function regiomino_cart_admin_settings() {

	
	/* $query = new EntityFieldQuery;
	$userids = $query
	->entityCondition('entity_type', 'user')
	->execute();
	
	unset($userids['user'][0]);
	
	$users = entity_load('user', array_keys($userids['user']));
	
	foreach($users as $uid => $uobj) {
		$uobj->field_shippingaddress = $uobj->field_address;
		user_save($uobj);
	} */
	
	
	
	//Fieldset for grouping all values related to the cart
	$form['cart_values'] = array(
    '#type' => 'fieldset',
    '#title' => t('Regiomino cart values'),
		'#collapsible' => TRUE,
		'#collapsed' => TRUE,
		'#description' => t("These settings are used to avoid hard coded values in the custom Regiomino modules. They don't affect settings that are made in other contributed modules or views. If major changes are made, those settings and values have to checked as well."),
  );
	//Cart preservation. Maximum number of days, inactive cart items should be preserved
	$form['cart_values']['regiomino_max_cart_days'] = array(
		'#type' => 'textfield',
		'#title' => t('Cart preservation'),
		'#description' => t('Please enter the maximum number of days, inactive cart items should be preserved. If a cart item becomes older than this, it will be deleted with the next cronjob.'),
		'#maxlength' => 3,
		'#size' => 1,
		'#default_value' => variable_get('regiomino_max_cart_days', 7),
		'#field_suffix' => t('days'),
	);
	/* //Fieldset for grouping all prepaid payment related settings
	$form['prepaid_values'] = array(
    '#type' => 'fieldset',
    '#title' => t('Regiomino prepaid values'),
		'#collapsible' => TRUE,
		'#collapsed' => TRUE,
		'#description' => t("These settings are used to centrally manage prepaid payment settings"),
  );
	//Name of bank
	$form['prepaid_values']['regiomino_payment_prepaid_bank'] = array(
		'#type' => 'textfield',
		'#title' => t('Name of Bank'),
		'#description' => t('Please enter the name of the bank'),
		'#default_value' => variable_get('regiomino_payment_prepaid_bank', ''),
	);
	//Account number
	$form['prepaid_values']['regiomino_payment_prepaid_accountholder'] = array(
		'#type' => 'textfield',
		'#title' => t('Account holder'),
		'#description' => t('Please enter the name of the account holder'),
		'#default_value' => variable_get('regiomino_payment_prepaid_accountholder', ''),
	);
	//Account number
	$form['prepaid_values']['regiomino_payment_prepaid_account'] = array(
		'#type' => 'textfield',
		'#title' => t('Account number'),
		'#description' => t('Please enter your bank account number'),
		'#default_value' => variable_get('regiomino_payment_prepaid_account', ''),
	);
	//Bank code
	$form['prepaid_values']['regiomino_payment_prepaid_bankcode'] = array(
		'#type' => 'textfield',
		'#title' => t('Bank Code'),
		'#description' => t('Please enter your bank code'),
		'#default_value' => variable_get('regiomino_payment_prepaid_bankcode', ''),
	);
	//IBAN
	$form['prepaid_values']['regiomino_payment_prepaid_iban'] = array(
		'#type' => 'textfield',
		'#title' => t('IBAN'),
		'#description' => t('Please enter your IBAN'),
		'#default_value' => variable_get('regiomino_payment_prepaid_iban', ''),
	);
	//BIC
	$form['prepaid_values']['regiomino_payment_prepaid_bic'] = array(
		'#type' => 'textfield',
		'#title' => t('BIC'),
		'#description' => t('Please enter your BIC'),
		'#default_value' => variable_get('regiomino_payment_prepaid_bic', ''),
	); */
	//Fieldset for grouping all paypal payment related settings
	$form['paypal_values'] = array(
    '#type' => 'fieldset',
    '#title' => t('Regiomino PayPal values'),
		'#collapsible' => TRUE,
		'#collapsed' => TRUE,
		'#description' => t("These settings are used to centrally manage PayPal payment settings"),
  );
	//Paypal account
	$form['paypal_values']['regiomino_payment_paypal_account'] = array(
		'#type' => 'textfield',
		'#title' => t('PayPal account'),
		'#description' => t('Please enter the e-mail address you registered with paypal.'),
		'#default_value' => variable_get('regiomino_payment_paypal_account', ''),
	);
	//Fieldset for grouping all novalnet payment related settings
	$form['novalnet_values'] = array(
    '#type' => 'fieldset',
    '#title' => t('Regiomino Novalnet values'),
		'#collapsible' => TRUE,
		'#collapsed' => TRUE,
		'#description' => t("These settings are used to centrally manage Novalnet payment settings"),
  );
	


	$form['novalnet_values']['regiomino_payment_novalnet_vendor'] = array(
		'#type' => 'textfield',
		'#title' => t('Novalnet Vendor ID'),
		'#description' => t('Please enter your vendor id.'),
		'#default_value' => variable_get('regiomino_payment_novalnet_vendor', 1852),
	);
	$form['novalnet_values']['regiomino_payment_novalnet_authcode'] = array(
		'#type' => 'textfield',
		'#title' => t('Novalnet Auth Code'),
		'#description' => t('Please enter your auth code.'),
		'#default_value' => variable_get('regiomino_payment_novalnet_authcode', 'fXtYibPcVkVy7iJvKQ0WPZ2yK6Mwan'),
	);
	$form['novalnet_values']['regiomino_payment_novalnet_product'] = array(
		'#type' => 'textfield',
		'#title' => t('Novalnet Product'),
		'#description' => t('Please enter the product number of your project.'),
		'#default_value' => variable_get('regiomino_payment_novalnet_product', 2489),
	);
	$form['novalnet_values']['regiomino_payment_novalnet_tariff'] = array(
		'#type' => 'textfield',
		'#title' => t('Novalnet Tariff'),
		'#description' => t('Please enter the tariff number of your project.'),
		'#default_value' => variable_get('regiomino_payment_novalnet_tariff', 4115),
	);
	$form['novalnet_values']['regiomino_payment_novalnet_testmode'] = array(
		'#type' => 'textfield',
		'#title' => t('Novalnet Testmode'),
		'#description' => t('Do you wish to work in test mode? Enter 1 for Yes or 0 for No. <strong>All other values will produce errors.</strong>'),
		'#default_value' => variable_get('regiomino_payment_novalnet_testmode', 0),
	);
	$form['novalnet_values']['regiomino_payment_novalnet_paygate'] = array(
		'#type' => 'textfield',
		'#title' => t('Novalnet Paygate'),
		'#description' => t('Please enter the paygate url you chose in the novalnet admin interface.'),
		'#default_value' => variable_get('regiomino_payment_novalnet_paygate', 'https://payport.novalnet.de/paygate.jsp'),
	);
	$form['novalnet_values']['regiomino_payment_novalnet_password'] = array(
		'#type' => 'textfield',
		'#title' => t('Novalnet Password'),
		'#description' => t('For some external payment providers such as Instant Bank Transfer you need a password.'),
		'#default_value' => variable_get('regiomino_payment_novalnet_password', 'eb1e78328c46506b46a4ac4a1e378b91'),
	);
	$form['novalnet_values']['regiomino_payment_novalnet_onlinepayport'] = array(
		'#type' => 'textfield',
		'#title' => t('Novalnet Online Transfer Payport'),
		'#description' => t('For some external payment providers such as Instant Bank Transfer you need a payport for redirection.'),
		'#default_value' => variable_get('regiomino_payment_novalnet_onlinepayport', 'https://payport.novalnet.de/online_transfer_payport'),
	);
	
	
	
	//Process form with system_settings_form
  return system_settings_form($form);
}