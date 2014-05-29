<?php

/**
 * @file
 * Administration page callbacks for the regiomino_order module.
 */

/**
 * Form builder. Configure regiomino_order
 * @ingroup forms
 * @see system_settings_form()
 */
/* function regiomino_order_admin_settings() {
	//Fieldset for grouping all values related to ordering
	$form['order_values'] = array(
    '#type' => 'fieldset',
    '#title' => t('Regiomino order values'),
		'#collapsible' => TRUE,
		'#collapsed' => TRUE,
		'#description' => t("These settings steer and affect the regiomino order process"),
  );
	//Make sure in the following form element options are displayed in correct language
	$optionspayment = array();
	$temp = regiomino_order_get_payment_types();
	foreach($temp as $key=>$value) {
		$optionspayment[$key] = t($value);
	}
	//Controls the payment types that are available to the customer
	$form['order_values']['regiomino_payment_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Payment types'),
    '#description' => t('Please select all payment types that should be available to the customer'),
		'#options' => $optionspayment,
    '#default_value' => array_flip(variable_get('regiomino_payment_types', array('prepaid' => 'Prepaid'))),
	);
	//Make sure in the following form element options are displayed in correct language
	$optionsshipping = array();
	$temp = regiomino_order_get_shipping_types();
	foreach($temp as $key=>$value) {
		$optionsshipping[$key] = t($value);
	}
	//Controls the shipping types that are available to the customer
	$form['order_values']['regiomino_shipping_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Shipping types'),
    '#description' => t('Please select all shipping types that should be available to the customer'),
		'#options' => $optionsshipping,
    '#default_value' => array_flip(variable_get('regiomino_shipping_types', array('pickup' => 'Pickup'))),
	);
	//Submit button
	$form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
	);
	//Process form with system_settings_form
  return $form;
} */

/**
 * Submit procedure for regiomino_order_admin_settings form.
 *
 * Necessary to store key/value pairs in variable, not only keys,
 * as would be the case with system_settings_form
 */
/* function regiomino_order_admin_settings_submit(&$form, &$form_state) {
	/**
	 * Take care of payment values
	 */
	//Get array of payment types
	$paymenttypes = regiomino_order_get_payment_types();
	//Initialise array for storing in variable
	$regiomino_payment_types = array();
	//Step through form submit values and pair up with predefined payment types
	foreach($form_state['values']['regiomino_payment_types'] as $key=>$value) {
		if($value) $regiomino_payment_types[$key] = $paymenttypes[$key];
	}
	//Store selected payment types in variable
	variable_set('regiomino_payment_types', $regiomino_payment_types);

	/**
	 * Take care of shipping values
	 */
	//Get array of shipping types
	$shippingtypes = regiomino_order_get_shipping_types();
	//Initialise array for storing in variable
	$regiomino_shipping_types = array();
	//Step through form submit values and pair up with predefined payment types
	foreach($form_state['values']['regiomino_shipping_types'] as $key=>$value) {
		if($value) $regiomino_shipping_types[$key] = $shippingtypes[$key];
	}
	//Store selected payment types in variable
	variable_set('regiomino_shipping_types', $regiomino_shipping_types);
	
	/**
	 * Set message
	 */
	drupal_set_message(t('The configuration options have been saved.'));
}
 */

/**
 * Function for retrieving all available payment types from db and storing them in array
 */
/* function regiomino_order_get_payment_types() {
	$result = db_select('regiomino_payment_types', 'op')
	->fields('op', array('payment_id', 'title'))
	->execute();
	$types = array();
	while($record = $result->fetchAssoc()) {
		$types[$record['payment_id']] = $record['title'];
	}
	return $types;
} */