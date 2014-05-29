<?php
// $Id$

/**
 * @file
 * Administration page callbacks for the regiomino_offer module.
 */

/**
 * Form builder. Configure regiomino_offer
 * @ingroup forms
 * @see system_settings_form()
 */

function regiomino_offer_admin_settings() {
	
	$form['money_values'] = array(
    '#type' => 'fieldset',
    '#title' => t('Regiomino money values'),
		'#collapsible' => TRUE,
		'#collapsed' => TRUE,
		'#description' => t("These settings are used to avoid hard coded values in the custom Regiomino modules. They don't affect settings that are made in other contributed modules or views. If major changes are made, those settings and values have to checked as well."),
  );
	
	//Währungszeichen
	$form['money_values']['regiomino_currency'] = array(
    '#type' => 'select',
    '#title' => t('Currency sign'),
    '#description' => t('Please select a currency sign.'),
		'#options' => array('€' => '€', 'SFr.' => 'SFr.', '$' => '$', '£' => '£'),
    '#default_value' => variable_get('regiomino_currency', '€'),
  );
	
	//Währungszeichen Entität
	$form['money_values']['regiomino_currency_entity'] = array(
    '#type' => 'select',
    '#title' => t('Entity code of the currency sign'),
    '#description' => t('Please enter the html entity code for the selected currency sign.'),
		'#options' => array('&euro;' => '&euro;', 'SFr.' => 'SFr.', '$' => '$', '&pound;' => '&pound;'),
    '#default_value' => variable_get('regiomino_currency_entity', '&euro;'),
  );
	
	//Mwst
	$form['offer_values']['regiomino_mwst'] = array(
    '#type' => 'textfield',
    '#title' => t('Sales tax'),
    '#description' => t('Please enter the percentage of sales-tax.'),
		'#maxlength' => 2,
		'#size' => 1,
    '#default_value' => variable_get('regiomino_mwst', 19),
		'#field_suffix' => '%',
  );
	
	//Alle Felder
	
	$form['offer_values'] = array(
    '#type' => 'fieldset',
    '#title' => t('Regiomino Offer values'),
		'#collapsible' => TRUE,
		'#collapsed' => TRUE,
		'#description' => t("These settings are used to avoid hard coded values in the custom Regiomino modules. They don't affect settings that are made in other contributed modules or views. If major changes are made, those settings and values have to checked as well. NOTICE: If field names change, then views have to be altered as well. This also requires hard coded changes in modules that load views programmatically."),
  );
  
  $form['offer_values']['regiomino_offer_node_type'] = array(
    '#type' => 'select',
    '#title' => t('Name of the offer node type'),
    '#description' => t('Please enter the machine-readable name of the offer node type.'),
		'#options' => node_type_get_names(),
    '#default_value' => variable_get('regiomino_offer_node_type', 'offer'),
  );
	
	$form['offer_values']['regiomino_offer_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Field for title'),
		'#size' => 15,
    '#description' => t('Please enter the machine-readable name of the title field.'),
    '#default_value' => variable_get('regiomino_offer_title', 'title'),
  );
	
	$form['offer_values']['regiomino_offer_body'] = array(
    '#type' => 'textfield',
    '#title' => t('Field for body'),
		'#size' => 15,
    '#description' => t('Please enter the machine-readable name of the body field.'),
    '#default_value' => variable_get('regiomino_offer_body', 'body'),
  );
	
	$form['offer_values']['regiomino_offer_category'] = array(
    '#type' => 'textfield',
    '#title' => t('Field for category'),
		'#size' => 15,
    '#description' => t('Please enter the machine-readable name of the category field.'),
    '#default_value' => variable_get('regiomino_offer_category', 'field_category'),
  );
	
	$form['offer_values']['regiomino_offer_price'] = array(
    '#type' => 'textfield',
    '#title' => t('Field for price'),
		'#size' => 15,
    '#description' => t('Please enter the machine-readable name of the price field.'),
    '#default_value' => variable_get('regiomino_offer_price', 'field_price'),
  );
	
	$form['offer_values']['regiomino_offer_unit'] = array(
    '#type' => 'textfield',
    '#title' => t('Field for unit'),
		'#size' => 15,
    '#description' => t('Please enter the machine-readable name of the unit field.'),
    '#default_value' => variable_get('regiomino_offer_unit', 'field_unit'),
  );
	
	$form['offer_values']['regiomino_offer_stock'] = array(
    '#type' => 'textfield',
    '#title' => t('Field for stock'),
		'#size' => 15,
    '#description' => t('Please enter the machine-readable name of the stock field.'),
    '#default_value' => variable_get('regiomino_offer_stock', 'field_stock'),
  );
	
	$form['offer_values']['regiomino_offer_discount'] = array(
    '#type' => 'textfield',
    '#title' => t('Field for discount'),
		'#size' => 15,
    '#description' => t('Please enter the machine-readable name of the discount field.'),
    '#default_value' => variable_get('regiomino_offer_discount', 'field_discount'),
  );
	
	$form['offer_values']['regiomino_offer_duration'] = array(
    '#type' => 'textfield',
    '#title' => t('Field for duration'),
		'#size' => 15,
    '#description' => t('Please enter the machine-readable name of the duration field.'),
    '#default_value' => variable_get('regiomino_offer_duration', 'field_duration'),
  );
	
	$form['offer_values']['regiomino_offer_image'] = array(
    '#type' => 'textfield',
    '#title' => t('Field for image'),
		'#size' => 15,
    '#description' => t('Please enter the machine-readable name of the image field.'),
    '#default_value' => variable_get('regiomino_offer_image', 'field_image'),
  );
	
	$form['offer_values']['regiomino_offer_delay'] = array(
    '#type' => 'textfield',
    '#title' => t('Offer delay in seconds'),
		'#size' => 15,
    '#description' => t('Please enter the offer pickup delay time in seconds'),
    '#default_value' => variable_get('regiomino_offer_delay', 28800),
  );

	$form['offer_values']['regiomino_offer_deadline'] = array(
    '#type' => 'textfield',
    '#title' => t('Offer deadline in seconds'),
		'#size' => 15,
    '#description' => t('Please enter the offer pickup deadline time in seconds'),
    '#default_value' => variable_get('regiomino_offer_deadline', 345600),
  );
  
  return system_settings_form($form);
}