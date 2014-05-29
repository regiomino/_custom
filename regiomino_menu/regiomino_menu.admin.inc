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
function regiomino_menu_admin_settings() {
	//Fieldset for grouping all values related to the cart
	$form['menu_values'] = array(
    '#type' => 'fieldset',
    '#title' => t('Regiomino menu and category values'),
		'#collapsible' => TRUE,
		'#collapsed' => TRUE,
		'#description' => t("These settings are used to avoid hard coded values in the custom Regiomino modules. They don't affect settings that are made in other contributed modules or views. If major changes are made, those settings and values have to checked as well."),
  );
	//Cart preservation. Maximum number of days, inactive cart items should be preserved
	$form['menu_values']['regiomino_menu_cache_expire'] = array(
		'#type' => 'textfield',
		'#title' => t('Cache expire'),
		'#description' => t('Please enter the amount in seconds, that menu and category taxonomy data shall be cached.'),
		'#maxlength' => 20,
		'#size' => 5,
		'#default_value' => variable_get('regiomino_menu_cache_expire', 1),
		'#field_suffix' => t('seconds'),
	);
	//Process form with system_settings_form
  return system_settings_form($form);
}