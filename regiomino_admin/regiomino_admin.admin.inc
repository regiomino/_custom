<?php

function regiomino_admin_manage_group($groupid) {
	$groupobject = node_load($groupid);
	if($groupobject->type == 'group_profile') {
		$header = array(
			'uid' => array(
				'data' => 'Kundennummer',
				'type' => 'property',
				'specifier' => 'uid',
				'sort' => 'desc',
			),
			// 'mail' => array(
				// 'data' => 'E-Mail-Adresse',
				// 'type' => 'property',
				// 'specifier' => 'mail',
			// ),
			'commerce_profile' => 'Gewerbekundenprofile',
			'orders' => 'Bestellvorgänge',
		);
		
		$query = new EntityFieldQuery();
		$tmp = $query
			->entityCondition('entity_type', 'user')
			->fieldCondition('field_profilereference_group', 'target_id', $groupid)
			->fieldCondition('field_profilereference_commerce', 'target_id', 'NULL', '!=')
			->tableSort($header)
			->execute();
		$all_users = entity_load('user', array_keys($tmp['user']));
		$rows = array();
		foreach($all_users as $uid=>$value) {

			$commerceprofile = '';
			if(isset($value->field_profilereference_commerce[LANGUAGE_NONE][0]['target_id'])) {
				$profileobject = node_load($value->field_profilereference_commerce[LANGUAGE_NONE][0]['target_id']);
				$commerceprofile .= l($profileobject->title, 'node/' . $profileobject->nid) . '<br />';
			}
			
			$orderqry = new EntityFieldQuery();
			$tmp2 = $orderqry
				->entityCondition('entity_type', 'regiomino_order')
				->propertyCondition('uid', $uid)
				->execute();
				
			$orderobjects = entity_load('regiomino_order', array_keys($tmp2['regiomino_order']));
			$order_processes = array();
			foreach($orderobjects as $orderid => $orderobject) {
				$order_processes[$orderobject->order_process_id] = TRUE;
			}
			
			$rows[] = array(
				$uid,
				// $value->mail,
				$commerceprofile,
				count($order_processes),
			);
		}
			
		$customers = theme('table', array('header' => $header, 'rows' => $rows));
		
		
		$header = array(
			'uid' => array(
				'data' => 'Kundennummer',
				'type' => 'property',
				'specifier' => 'uid',
				'sort' => 'desc',
			),
			// 'mail' => array(
				// 'data' => 'E-Mail-Adresse',
				// 'type' => 'property',
				// 'specifier' => 'mail',
			// ),
			'seller_profile' => 'Verkäuferprofile',
		);
		
		$query = new EntityFieldQuery();
		$tmp = $query
			->entityCondition('entity_type', 'user')
			->fieldCondition('field_profilereference_group', 'target_id', $groupid)
			->fieldCondition('field_profilereference', 'target_id', 'NULL', '!=')
			->tableSort($header)
			->execute();
		$all_users = entity_load('user', array_keys($tmp['user']));
		$rows = array();
		foreach($all_users as $uid=>$value) {
			
			$sellerprofile = '';
			if(isset($value->field_profilereference[LANGUAGE_NONE][0]['target_id'])) {
				$profileobject = node_load($value->field_profilereference[LANGUAGE_NONE][0]['target_id']);
				$sellerprofile .= l($profileobject->title, 'node/' . $profileobject->nid) . '<br />';
			}
			
			$rows[] = array(
				$uid,
				// $value->mail,
				$sellerprofile,
			);
		}
			
		$sellers = theme('table', array('header' => $header, 'rows' => $rows));
		
		return '<h2>' . $groupobject->title . '</h2><h3>Gastronomen</h3>' .$customers . '<h3>Erzeuger</h3>' . $sellers;
	}
}

function regiomino_admin_groups() {
	global $user;

	$header = array(
		// 'nid' => array(
			// 'data' => 'Art.#',
			// 'type' => 'property',
			// 'specifier' => 'nid',
			// 'sort' => 'desc',
		// ),
		'title' => array(
			'data' => 'Titel',
			'type' => 'property',
			'specifier' => 'title',
		),
		'manage' => 'Verwalten',
	);

	$query = new EntityFieldQuery();
	$tmp = $query
		->entityCondition('entity_type', 'node')
		->entityCondition('bundle', 'group_profile')
		->propertyCondition('uid', $user->uid)
		->tableSort($header)
		->execute();
	$allgroups = entity_load('node', array_keys($tmp['node']));
	
	
	foreach($allgroups as $nid=>$value) {		
		$rows[] = array(
			l($value->title, 'node/' . $nid),
			l('Verwalten', 'admin/groups/manage/' . $nid),
		);
	}
		
	return theme('table', array('header' => $header, 'rows' => $rows));

	
}

function regiomino_admin_topsellers() {

	$queryparams = drupal_get_query_parameters();

	$form = drupal_get_form('regiomino_admin_topsellers_filterform');
	
	$limit = 10;
	$picker = NULL;
	$timerange = 60*60*24*7*12;
	$state = array(1);
	if(isset($queryparams['pickerfci']) && !empty($queryparams['pickerfci'])) $picker = $queryparams['pickerfci'];
	if(isset($queryparams['timerange']) && !empty($queryparams['timerange'])) $timerange = 60*60*24*7*$queryparams['timerange'];
	if(isset($queryparams['state']) && !empty($queryparams['state'])) $state = $queryparams['state'];
	if(isset($queryparams['count']) && !empty($queryparams['count'])) $limit = $queryparams['count'];
	$mostsaleseffective = regiomino_offer_most_sales_effective_products($timerange, $picker, 0, $limit, 0, 0, $state);

	$header = array(
		'Titel',
		'Umsatz',
		'Status',
	);
	$rows = array();
	
	$statusarray = array(
		0 => 'Aktiv',
		1 => 'abgelaufen',
	);
	foreach($mostsaleseffective as $nid=>$values) {
		$row = array(
			l($values['node']->title, 'node/' . $nid),
			number_format($values['cash'], 2, ",", ".") . ' €',
			$statusarray[$values['node']->soldout],
		);
		$rows[] = $row;
	}
	
	return '<h2>Top ' . $limit . ' Produkte mit dem höchsten Umsatz auf den gewählten Lieferstrecken</h2>' . render($form) . theme('table', array('header' => $header, 'rows' => $rows));
}

function _ajax_return_available_categorytwo($form, &$form_state) {
	return $form['regiomino_offer_create']['productinfo']['category2'];
}

function _ajax_prefill_origin_with_seller($form, &$form_state) {
	return $form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['productorigin'];
}

function _ajax_return_calculated_tu_fields($form, &$form_state) {
	$tmp = explode('-', $form_state['triggering_element']['#ajax']['wrapper']);
	$i = $tmp[1];
	return $form['regiomino_offer_create']['priceinfo']['tradingunits']['tradingunit_' . $i]['tucalculated_' . $i];
}

function _ajax_return_calculated_tradingprices($form, &$form_state) {
	return $form['regiomino_offer_create']['priceinfo']['tradingprices'];
}

function _ajax_update_tu_field_calculation($form, &$form_state) {
	return $form['regiomino_offer_create']['priceinfo']['tradingunits'];
}

function _ajax_update_tradingprice_field_calculation($form, &$form_state) {
	return $form['regiomino_offer_create']['priceinfo']['tradingprices'];
}

function _ajax_add_more_tradingunits_add_one($form, &$form_state) {
  $form_state['num_tu']++;
  $form_state['rebuild'] = TRUE;
}

function _ajax_add_more_tradingunits_remove_one($form, &$form_state) {
  if ($form_state['num_tu'] > 1) {
    $form_state['num_tu']--;
  }
  $form_state['rebuild'] = TRUE;
}

function _ajax_add_more_tradingunits_callback($form, &$form_state) {
  return $form['regiomino_offer_create']['priceinfo']['tradingunits'];
}

// function _ajax_add_more_images_add_one($form, &$form_state) {
  // $form_state['num_img']++;
  // $form_state['rebuild'] = TRUE;
// }

// function _ajax_add_more_images_remove_one($form, &$form_state) {
  // if ($form_state['num_img'] > 1) {
    // $form_state['num_img']--;
  // }
  // $form_state['rebuild'] = TRUE;
// }

function _ajax_add_more_images_callback($form, &$form_state) {
  return $form['regiomino_offer_create']['productinfo']['images'];
}

 
function regiomino_admin_manage_offers_create($form, &$form_state) {

	global $user;
	
	$form = array();
	
	$form = regiomino_admin_manage_offers_getformfields();
	
	if(isset($form_state['values']['category1'])) {
		$subcategorytree = taxonomy_get_tree(2, $form_state['values']['category1'], 1);
		$form['regiomino_offer_create']['productinfo']['category2']['#options'] = array();
		if(!isset($subcategorytree) || empty($subcategorytree)) $form['regiomino_offer_create']['productinfo']['category2']['#required'] = FALSE;
		foreach($subcategorytree as $tid=>$tobject) {
			$form['regiomino_offer_create']['productinfo']['category2']['#options'][$tobject->tid] = $tobject->name;
		}
	}
	
	if(!isset($form_state['values']['category1']) || empty($form_state['values']['category1'])) {
		$form['regiomino_offer_create']['productinfo']['category2']['#states'] = array(
			'visible'=>array(
				':input[name="category1"]' => array(
					'value'=>$form['regiomino_offer_create']['productinfo']['category1']['#options'],
				),
			),
		);
	}

	if(isset($form_state['values']['prefillwithselleraddress']) && !empty($form_state['values']['prefillwithselleraddress'])) {
		$sellerprofile = node_load(user_load($user->uid)->field_profilereference[LANGUAGE_NONE][0]['target_id']);
		$form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['productorigin']['locality_block']['locality']['#value'] = $sellerprofile->field_address[LANGUAGE_NONE][0]['locality'];
		$form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['productorigin']['locality_block']['postal_code']['#value'] = $sellerprofile->field_address[LANGUAGE_NONE][0]['postal_code'];
		$form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['productorigin']['country']['#value'] = 'DE';
		$form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['productorigin']['organisation_block']['organisation_name']['#value'] = $sellerprofile->title;
		$form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['productorigin']['locality_block']['postal_code']['#disabled'] = TRUE;
		$form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['productorigin']['locality_block']['locality']['#disabled'] = TRUE;
		$form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['productorigin']['country']['#disabled'] = TRUE;
		$form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['productorigin']['organisation_block']['organisation_name']['#disabled'] = TRUE;
	}
	else {
		unset($form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['productorigin']['locality_block']['locality']['#value']);
		unset($form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['productorigin']['locality_block']['postal_code']['#value']);
		unset($form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['productorigin']['country']['#value']);
		unset($form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['productorigin']['organisation_block']['organisation_name']['#value']);
		$form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['productorigin']['locality_block']['postal_code']['#disabled'] = FALSE;	
		$form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['productorigin']['locality_block']['locality']['#disabled'] = FALSE;
		$form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['productorigin']['country']['#disabled'] = FALSE;
		$form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['productorigin']['organisation_block']['organisation_name']['#disabled'] = FALSE;
	}
	
	if(isset($form_state['input']['tradingprice_1_netrevenue'])) {
		for($i=1; $i<=5; $i++) {
			$tpnet[$i] = floatval(str_replace(',', '.', $form_state['input']['tradingprice_' . $i . '_netrevenue']));
			$tpshippingfee[$i] = $tpnet[$i] * 0.1;
			if($form_state['input']['tradingprice_' . $i . '_customertype'] == 'commercial') {
				$tprmfee[$i] = $tpnet[$i] * 0.1;
			}
			else {
				$tprmfee[$i] = $tpnet[$i] * 0.135;
			}
			$tpvat[$i] = ($tpnet[$i] + $tpshippingfee[$i] + $tprmfee[$i]) * $form_state['input']['vat'] / 100;
			$tplisting[$i] = $tpnet[$i] + $tpshippingfee[$i] + $tprmfee[$i];
			$tpgross[$i] = $tpnet[$i] + $tpshippingfee[$i] + $tprmfee[$i] + $tpvat[$i];
			
			$form['regiomino_offer_create']['priceinfo']['tradingprices']['#options'][$i-1]['tradingprice_' . $i . '_shipping']['#default_value'] = $form['regiomino_offer_create']['priceinfo']['tradingprices']['#options'][$i-1]['tradingprice_' . $i . '_shipping']['#value'] = number_format($tpshippingfee[$i], 2, ",", ".");
			
			$form['regiomino_offer_create']['priceinfo']['tradingprices']['#options'][$i-1]['tradingprice_' . $i . '_rmfee']['#default_value'] = $form['regiomino_offer_create']['priceinfo']['tradingprices']['#options'][$i-1]['tradingprice_' . $i . '_rmfee']['#value'] = number_format($tprmfee[$i], 2, ",", ".");
			
			$form['regiomino_offer_create']['priceinfo']['tradingprices']['#options'][$i-1]['tradingprice_' . $i . '_vat']['#default_value'] = $form['regiomino_offer_create']['priceinfo']['tradingprices']['#options'][$i-1]['tradingprice_' . $i . '_vat']['#value'] = number_format($tpvat[$i], 2, ",", ".");
			
			$form['regiomino_offer_create']['priceinfo']['tradingprices']['#options'][$i-1]['tradingprice_' . $i . '_gross']['#default_value'] = $form['regiomino_offer_create']['priceinfo']['tradingprices']['#options'][$i-1]['tradingprice_' . $i . '_gross']['#value'] = number_format($tpgross[$i], 2, ",", ".");
			
			$form['regiomino_offer_create']['priceinfo']['tradingprices']['#options'][$i-1]['tradingprice_' . $i . '_listing']['#default_value'] = $form['regiomino_offer_create']['priceinfo']['tradingprices']['#options'][$i-1]['tradingprice_' . $i . '_listing']['#value'] = number_format($tplisting[$i], 2, ",", ".");
		}
	}
	
	
	
	$form['regiomino_offer_create']['saveastemplate'] = array(
		'#type' => 'checkbox',
		'#title' => 'Auch als Vorlage speichern',
	);
	
	//Submit button for refreshing values
	$form['regiomino_offer_create']['submit']['#value'] = 'Angebot erstellen';
	
	return $form;
}

function regiomino_admin_manage_offers_edit($form, &$form_state) {
	global $user;
	
	$nid = arg(3);
	
	$nodeobject = node_load($nid);
	
	if($nodeobject->soldout == 1) {
		drupal_set_message('Das ausgewählte Angebot ist bereits beendet und kann daher nicht mehr bearbeitet werden. Bitte nutzen Sie stattdessen die Kopier-Funktion in der Angebotsverwaltung um einen neuen Datensatz aus diesem Angebot zu erstellen.', 'error');
		drupal_access_denied();
		exit;
	}
	
	$form = array();
	
	$form = regiomino_admin_manage_offers_getformfields();
	
	
	if(isset($form_state['values']['prefillwithselleraddress']) && !empty($form_state['values']['prefillwithselleraddress'])) {
		$sellerprofile = node_load(user_load($user->uid)->field_profilereference[LANGUAGE_NONE][0]['target_id']);
		$form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['productorigin']['locality_block']['locality']['#value'] = $sellerprofile->field_address[LANGUAGE_NONE][0]['locality'];
		$form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['productorigin']['locality_block']['postal_code']['#value'] = $sellerprofile->field_address[LANGUAGE_NONE][0]['postal_code'];
		$form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['productorigin']['country']['#value'] = 'DE';
		$form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['productorigin']['organisation_block']['organisation_name']['#value'] = $sellerprofile->title;
		$form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['productorigin']['locality_block']['postal_code']['#disabled'] = TRUE;
		$form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['productorigin']['locality_block']['locality']['#disabled'] = TRUE;
		$form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['productorigin']['country']['#disabled'] = TRUE;
		$form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['productorigin']['organisation_block']['organisation_name']['#disabled'] = TRUE;
	}
	else {
		unset($form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['productorigin']['locality_block']['locality']['#value']);
		unset($form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['productorigin']['locality_block']['postal_code']['#value']);
		unset($form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['productorigin']['country']['#value']);
		unset($form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['productorigin']['organisation_block']['organisation_name']['#value']);
		$form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['productorigin']['locality_block']['postal_code']['#disabled'] = FALSE;	
		$form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['productorigin']['locality_block']['locality']['#disabled'] = FALSE;
		$form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['productorigin']['country']['#disabled'] = FALSE;
		$form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['productorigin']['organisation_block']['organisation_name']['#disabled'] = FALSE;
	}
	
	
	
	
	
	if(isset($form_state['input']['tradingprice_1_netrevenue'])) {
		for($i=1; $i<=5; $i++) {
			$tpnet[$i] = floatval(str_replace(',', '.', $form_state['input']['tradingprice_' . $i . '_netrevenue']));
			$tpshippingfee[$i] = $tpnet[$i] * 0.1;
			if($form_state['input']['tradingprice_' . $i . '_customertype'] == 'commercial') {
				$tprmfee[$i] = $tpnet[$i] * 0.1;
			}
			else {
				$tprmfee[$i] = $tpnet[$i] * 0.135;
			}
			$tpvat[$i] = ($tpnet[$i] + $tpshippingfee[$i] + $tprmfee[$i]) * $form_state['input']['vat'] / 100;
			$tplisting[$i] = $tpnet[$i] + $tpshippingfee[$i] + $tprmfee[$i];
			$tpgross[$i] = $tpnet[$i] + $tpshippingfee[$i] + $tprmfee[$i] + $tpvat[$i];
			
			$form['regiomino_offer_create']['priceinfo']['tradingprices']['#options'][$i-1]['tradingprice_' . $i . '_shipping']['#default_value'] = $form['regiomino_offer_create']['priceinfo']['tradingprices']['#options'][$i-1]['tradingprice_' . $i . '_shipping']['#value'] = number_format($tpshippingfee[$i], 2, ",", ".");
			
			$form['regiomino_offer_create']['priceinfo']['tradingprices']['#options'][$i-1]['tradingprice_' . $i . '_rmfee']['#default_value'] = $form['regiomino_offer_create']['priceinfo']['tradingprices']['#options'][$i-1]['tradingprice_' . $i . '_rmfee']['#value'] = number_format($tprmfee[$i], 2, ",", ".");
			
			$form['regiomino_offer_create']['priceinfo']['tradingprices']['#options'][$i-1]['tradingprice_' . $i . '_vat']['#default_value'] = $form['regiomino_offer_create']['priceinfo']['tradingprices']['#options'][$i-1]['tradingprice_' . $i . '_vat']['#value'] = number_format($tpvat[$i], 2, ",", ".");
			
			$form['regiomino_offer_create']['priceinfo']['tradingprices']['#options'][$i-1]['tradingprice_' . $i . '_gross']['#default_value'] = $form['regiomino_offer_create']['priceinfo']['tradingprices']['#options'][$i-1]['tradingprice_' . $i . '_gross']['#value'] = number_format($tpgross[$i], 2, ",", ".");
			
			$form['regiomino_offer_create']['priceinfo']['tradingprices']['#options'][$i-1]['tradingprice_' . $i . '_listing']['#default_value'] = $form['regiomino_offer_create']['priceinfo']['tradingprices']['#options'][$i-1]['tradingprice_' . $i . '_listing']['#value'] = number_format($tplisting[$i], 2, ",", ".");
		}
	}

	
	//Set default title
	$form['regiomino_offer_create']['productinfo']['title']['#default_value'] = $nodeobject->title;
	
	
	
	
		
	if(!isset($form_state['values']['category1']) || empty($form_state['values']['category1'])) {
		$form['regiomino_offer_create']['productinfo']['category2']['#states'] = array(
			'visible'=>array(
				':input[name="category1"]' => array(
					'value'=>$form['regiomino_offer_create']['productinfo']['category1']['#options'],
				),
			),
		);
	}
	
	

	//Set default main category
	$form['regiomino_offer_create']['productinfo']['category1']['#default_value'] = $nodeobject->field_category[LANGUAGE_NONE][0]['tid'];

	
	$form['regiomino_offer_create']['productinfo']['category2']['#default_value'] = $nodeobject->field_category[LANGUAGE_NONE][1]['tid'];
	
	//Determine according subcategories
	$subcategorytree = taxonomy_get_tree(2, $nodeobject->field_category[LANGUAGE_NONE][0]['tid'], 1);
	
	if(isset($subcategorytree) && !empty($subcategorytree)) {
		$category_2_options = array();
		foreach($subcategorytree as $tid=>$tobject) {
			$category_2_options[$tobject->tid] = $tobject->name;
		}
		
		//Set subcategory options
		$form['regiomino_offer_create']['productinfo']['category2']['#options'] = $category_2_options;
	}
	else {
		$form['regiomino_offer_create']['productinfo']['category2']['#required'] = FALSE;
	}
	
	//Deactivate hiding of subcategoryfield
	unset($form['regiomino_offer_create']['productinfo']['category2']['#states']);
	
	
	
	if(isset($form_state['values']['category1'])) {
		$subcategorytree = taxonomy_get_tree(2, $form_state['values']['category1'], 1);
		$form['regiomino_offer_create']['productinfo']['category2']['#options'] = array();
		if(!isset($subcategorytree) || empty($subcategorytree)) {
			$form['regiomino_offer_create']['productinfo']['category2']['#required'] = FALSE;
		}
		else {
			$form['regiomino_offer_create']['productinfo']['category2']['#required'] = TRUE;
			$form['regiomino_offer_create']['productinfo']['category2']['#options'] = array();
			foreach($subcategorytree as $tid=>$tobject) {
				$form['regiomino_offer_create']['productinfo']['category2']['#options'][$tobject->tid] = $tobject->name;
			}
		}
	}
	
	
	
	
	//Set default images
	for($i=0; $i<5; $i++) {
		if(isset($nodeobject->field_image[LANGUAGE_NONE][$i]['fid'])) $form['regiomino_offer_create']['productinfo']['images']['image_' . $i]['#default_value'] = $nodeobject->field_image[LANGUAGE_NONE][$i]['fid'];
	}
	
	//Set default description
	$form['regiomino_offer_create']['productinfo']['description']['#default_value'] = $nodeobject->body[LANGUAGE_NONE][0]['value'];

	//Set default product origin
	$form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['productorigin']['#address']['locality'] = $nodeobject->field_origin[LANGUAGE_NONE][0]['locality'];
	$form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['productorigin']['#address']['postal_code'] = $nodeobject->field_origin[LANGUAGE_NONE][0]['postal_code'];
	$form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['productorigin']['#address']['country'] = $nodeobject->field_origin[LANGUAGE_NONE][0]['country'];
	$form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['productorigin']['#address']['organisation_name'] = $nodeobject->field_origin[LANGUAGE_NONE][0]['organisation_name'];

	//Set default usewithin value
	$form['regiomino_offer_create']['productinfo']['expiry_fieldset']['usewithin']['#default_value'] = $nodeobject->field_expiry[LANGUAGE_NONE][0]['first'];

	//Set default labels
	$tmp = array();
	foreach($nodeobject->field_labels[LANGUAGE_NONE] as $labels) {
		$form['regiomino_offer_create']['productinfo']['qualitylabels']['#default_value'][] = $labels['target_id'];
	}

	//Set default notices
	$form['regiomino_offer_create']['productinfo']['importantnotices']['#default_value'] = $nodeobject->field_importantnotices[LANGUAGE_NONE][0]['value'];
	
	//Set default packing unit
	$form['regiomino_offer_create']['priceinfo']['packingunit_first']['#default_value'] = $nodeobject->field_packingunit[LANGUAGE_NONE][0]['first'];
	$form['regiomino_offer_create']['priceinfo']['packingunit_second']['#default_value'] = array($nodeobject->field_packingunit[LANGUAGE_NONE][0]['second']);
	
	//Set default stock
	$form['regiomino_offer_create']['priceinfo']['stock']['#default_value'] = $nodeobject->field_stock[LANGUAGE_NONE][0]['value'];
	
	//Set default vat
	$form['regiomino_offer_create']['priceinfo']['vat']['#default_value'] = array($nodeobject->field_salestax[LANGUAGE_NONE][0]['value']);

	

	if(!isset($form_state['input']['tradingprice_1_netrevenue'])) {
	
		for($key=0; $key<5; $key++) {
			$index = $key + 1;
			unset($form['regiomino_offer_create']['priceinfo']['tradingprices']['#options'][$key]['tradingprice_' . $index . '_amount']['#value']);
			unset($form['regiomino_offer_create']['priceinfo']['tradingprices']['#options'][$key]['tradingprice_' . $index . '_shipping']['#value']);
			unset($form['regiomino_offer_create']['priceinfo']['tradingprices']['#options'][$key]['tradingprice_' . $index . '_rmfee']['#value']);
			unset($form['regiomino_offer_create']['priceinfo']['tradingprices']['#options'][$key]['tradingprice_' . $index . '_vat']['#value']);
			unset($form['regiomino_offer_create']['priceinfo']['tradingprices']['#options'][$key]['tradingprice_' . $index . '_listing']['#value']);
			unset($form['regiomino_offer_create']['priceinfo']['tradingprices']['#options'][$key]['tradingprice_' . $index . '_netrevenue']['#value']);
			unset($form['regiomino_offer_create']['priceinfo']['tradingprices']['#options'][$key]['tradingprice_' . $index . '_gross']['#value']);
			
			$form['regiomino_offer_create']['priceinfo']['tradingprices']['#options'][$key]['tradingprice_' . $index . '_amount']['#default_value'] = '';
		}
		//Load tradingunits
		$myfcis = array();
		foreach($nodeobject->field_tradingunits[LANGUAGE_NONE] as $myfcivalues) {
			$myfcis[] = $myfcivalues['value'];
		}
		$fcis = entity_load('field_collection_item', $myfcis);
		//Sort without key preservation to ensure that the smalles unit has key 0
		usort($fcis, "regiomino_offer_sort_tradingunits");
		
		foreach($fcis as $key=>$fcivalue) {
			$index = $key + 1;
			
			$form['regiomino_offer_create']['priceinfo']['tradingprices']['#options'][$key]['tradingprice_' . $index . '_amount']['#default_value'] = $fcivalue->field_tu_size[LANGUAGE_NONE][0]['value'];
			
			$form['regiomino_offer_create']['priceinfo']['tradingprices']['#options'][$key]['tradingprice_' . $index . '_netrevenue']['#default_value'] = number_format($fcivalue->field_tu_revenue[LANGUAGE_NONE][0]['value'], 2, ",", ".");

			$form['regiomino_offer_create']['priceinfo']['tradingprices']['#options'][$key]['tradingprice_' . $index . '_shipping'][	'#default_value'] = number_format($fcivalue->field_tu_shipping[LANGUAGE_NONE][0]['value'], 2, ",", ".");

			$form['regiomino_offer_create']['priceinfo']['tradingprices']['#options'][$key]['tradingprice_' . $index . '_rmfee']['#default_value'] = number_format($fcivalue->field_tu_commission[LANGUAGE_NONE][0]['value'], 2, ",", ".");

			$form['regiomino_offer_create']['priceinfo']['tradingprices']['#options'][$key]['tradingprice_' . $index . '_vat']['#default_value'] = number_format($fcivalue->field_tu_vat[LANGUAGE_NONE][0]['value'], 2, ",", ".");

			$form['regiomino_offer_create']['priceinfo']['tradingprices']['#options'][$key]['tradingprice_' . $index . '_listing']['#default_value'] = number_format($fcivalue->field_tu_price[LANGUAGE_NONE][0]['value'], 2, ",", ".");
			
			$form['regiomino_offer_create']['priceinfo']['tradingprices']['#options'][$key]['tradingprice_' . $index . '_gross']['#default_value'] = number_format($fcivalue->field_tu_gross[LANGUAGE_NONE][0]['value'], 2, ",", ".");
			
			$form['regiomino_offer_create']['priceinfo']['tradingprices']['#options'][$key]['tradingprice_' . $index . '_customertype']['#default_value'] = $fcivalue->field_tu_customertype[LANGUAGE_NONE][0]['value'];
		}
	}

	//Set default delay
	$form['regiomino_offer_create']['logisticsinfo']['delay']['#default_value'] = $nodeobject->field_pickupdelay[LANGUAGE_NONE][0]['value'];
	
	//Set default usewithin value
	if(isset($nodeobject->field_expiry[LANGUAGE_NONE][0]['second'])) $form['regiomino_offer_create']['logisticsinfo']['cooling']['#default_value'] = array($nodeobject->field_expiry[LANGUAGE_NONE][0]['second']);
	
	//Set default shipping weight
	$form['regiomino_offer_create']['logisticsinfo']['shippingweight']['#default_value'] = $nodeobject->field_shippingweight[LANGUAGE_NONE][0]['value'];
		
	//Submit button for refreshing values
	$form['regiomino_offer_create']['submit']['#value'] = 'Angebot speichern';
	
	return $form;
}

function regiomino_admin_manage_offers_getformfields() {
	$form = array();
	
	$form['regiomino_offer_create']['productinfo'] = array(
		'#type' => 'fieldset',
		'#collapsible' => TRUE,
		'#collapsed' => FALSE,
		'#title' => '1 ' . 'Produktinformationen eingeben',
	);

	$form['regiomino_offer_create']['productinfo']['title'] = array(
		'#type' => 'textfield',
		'#required' => TRUE,
		'#attributes' => array('placeholder' => 'Titel, z.B. "Bamberger Hörnla, 3kg" (ca. 30 Zeichen)'),
		'#title' => 'Titel',
		'#title_display' => 'invisible',
	);
	
	$categorytree = taxonomy_get_tree(2, 0, 1);
	$category_1_options = array();
	foreach($categorytree as $tid=>$tobject) {
		$category_1_options[$tobject->tid] = $tobject->name;
	}

	$form['regiomino_offer_create']['productinfo']['category1'] = array(
		'#type' => 'select',
		'#multiple' => FALSE,
		'#size' => 10,
		'#required' => TRUE,
		'#options' => $category_1_options,
		'#ajax' => array(
			'callback' => '_ajax_return_available_categorytwo',
			'wrapper' => 'category2-field-wrapper'
		),
		'#title' => 'Kategorie',
		'#title_display' => 'invisible',
	);
	
	//Determine all subcategories
	$category_2_options = array();
	foreach($category_1_options as $tid=>$tobject) {
		$subcategorytree = taxonomy_get_tree(2, $tid, 1);
		if(isset($subcategorytree) && !empty($subcategorytree)) {
			foreach($subcategorytree as $stid=>$stobject) {
				$category_2_options[$stobject->tid] = $stobject->name;
			}
		}
	}
	
	$form['regiomino_offer_create']['productinfo']['category2'] = array(
		'#type' => 'select',
		'#multiple' => FALSE,
		'#size' => 10,
		'#required' => TRUE,
		'#options' => $category_2_options,
		'#prefix' => '<div id="category2-field-wrapper">',
		'#suffix' => '</div>',
		'#title' => 'Unterkategorie',
		'#title_display' => 'invisible',
	);
	
	$form['regiomino_offer_create']['productinfo']['images'] = array(
		'#type' => 'fieldset',
		'#collapsible' => TRUE,
		'#collapsed' => FALSE,
	  '#prefix' => '<div id="images-fieldset-wrapper">',
    '#suffix' => '</div>',
	);
	
	$form['regiomino_offer_create']['productinfo']['images']['image_0'] = array(
		'#type' => 'managed_file',
		'#multiple' => TRUE,
		'#theme' => 'offer_thumb_upload',
		'#markup' => '&nbsp;',
		'#title' => 'Bild 1',
		'#title_display' => 'invisible',
	);
		
	for ($i = 1; $i < 5; $i++) {
		$form['regiomino_offer_create']['productinfo']['images']['image_' . $i] = array(
			'#type' => 'managed_file',
			'#multiple' => TRUE,
			'#theme' => 'offer_thumb_upload',
			'#markup' => '&nbsp;',
			'#title' => 'Bild ' . $i + 1,
			'#title_display' => 'invisible',
		);
	}
	
	$form['regiomino_offer_create']['productinfo']['description'] = array(
		'#type' => 'text_format',
		'#required' => TRUE,
		'#title' => 'Beschreibung',
		'#title_display' => 'invisible',
	);
	
	$form['regiomino_offer_create']['productinfo']['productorigin_fieldset'] = array(
		'#type' => 'fieldset',
	);
	global $user;
	if(in_array('Verkäufer', $user->roles)) {
		$form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['prefillwithselleraddress'] = array(
			'#type' => 'checkbox',
			'#title' => 'Mit Verkäuferadresse befüllen',
			'#ajax' => array(
				'callback' => '_ajax_prefill_origin_with_seller',
				'wrapper' => 'productorigin-field-wrapper',
				'event' => 'change',
			),
		);
	}
	
  $form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['productorigin'] = addressfield_generate(addressfield_default_values('DE'), array('address' => 'address', 'organisation' => 'organisation'), array('mode' => 'form'));
	unset($form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['productorigin']['street_block']);
	
	$form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['productorigin']['#prefix'] = '<div id="productorigin-field-wrapper">';
	$form['regiomino_offer_create']['productinfo']['productorigin_fieldset']['productorigin']['#suffix'] = '</div>';
	
	$form['regiomino_offer_create']['productinfo']['expiry_fieldset'] = array(
		'#type' => 'fieldset',
	);
	
	$form['regiomino_offer_create']['productinfo']['expiry_fieldset']['expiryselection'] = array(
		'#type' => 'radios',
		'#title' => 'Art der Haltbarkeitsangabe',
		'#description' => 'Bitte wählen Sie die Art der Haltbarkeitsangabe, die Sie zu Ihrem Produkt machen möchten.',
		'#options' => array(
			'date' => 'Genaues Mindesthaltbarkeitsdatum (derzeit nicht verfügbar, wir arbeiten mit Hochdruck an dieser Option)',
			'usage' => 'Anzahl an Tagen in denen das Produkt verbraucht werden muss',
		),
		'#disabled_options' => array('date'),
		'#default_value' => 'usage',
	);
	
	$form['regiomino_offer_create']['productinfo']['expiry_fieldset']['expirydate'] = array(
		'#type' => 'date',
		'#title' => 'Mindesthaltbarkeitsdatum',
		'#description' => 'Wenn Sie ein Mindesthaltbarkeitsdatum angeben, stellen Sie bitte sicher, dass Sie dieses Datum prüfen und ggf. korrigieren, sobald Sie Exemplare aus einem anderen Bestand verkaufen.',
		'#states'=>array(
			'visible'=>array(
				':input[name="expiryselection"]' => array(
					'value'=>'date',
				),
			),
		),
	);
	
	$form['regiomino_offer_create']['productinfo']['expiry_fieldset']['usewithin'] = array(
		'#type' => 'textfield',
		'#title' => 'Produkt verbrauchen innerhalb von',
		'#description' => 'Bitte geben Sie einen Zeitraum in Tagen an, in dem das Produkt verbraucht werden muss.',
		'#states'=>array(
			'visible'=>array(
				':input[name="expiryselection"]' => array(
					'value'=>'usage',
				),
			),
		),
		'#size' => 3,
		'#field_suffix' => 'Tagen',
	);
	
	$query = new EntityFieldQuery;
	$labelqry = $query
		->entityCondition('entity_type', 'node')
		->entityCondition('bundle', array('siegel'))
		->propertyCondition('status', 1)
		->execute();
	$labels = entity_load('node', array_keys($labelqry['node']));
	$labeloptions = array();
	foreach($labels as $lid=>$lobject) {
		$labeleoptions[$lid] = '<img src="' . image_style_url('label_select', $lobject->field_image[LANGUAGE_NONE][0]['uri']) . '" />';
	}
	
	$form['regiomino_offer_create']['productinfo']['qualitylabels'] = array(
		'#type' => 'checkboxes',
		'#options' => $labeleoptions,
	);
	
	$form['regiomino_offer_create']['productinfo']['importantnotices'] = array(
		'#type' => 'text_format',
	);
	
	$form['regiomino_offer_create']['priceinfo'] = array(
		'#type' => 'fieldset',
		'#collapsible' => TRUE,
		'#collapsed' => FALSE,
		'#title' => '2 ' . 'Preisinformationen',
	);
	
		
	$form['regiomino_offer_create']['priceinfo']['packingunit_first'] = array(
		'#type' => 'textfield',
		'#size' => 5,
		'#default_value' => 0,
		'#title' => 'Verkaufseinheit (Menge)',
		'#title_display' => 'invisible',
		'#required' => TRUE,
	);

	$form['regiomino_offer_create']['priceinfo']['packingunit_second'] = array(
		'#type' => 'select',
		'#size' => 1,
		'#default_value' => 'ml',
		'#options' => array(
				'ml' => 'ml',
				'l' => 'l',
				'pc' => 'St.',
				'g' => 'g',
				'kg' => 'kg',
				'bd' => 'Bund'
		),
		'#title' => 'Verkaufseinheit (Einheit)',
		'#title_display' => 'invisible',
	);
	
	$form['regiomino_offer_create']['priceinfo']['stock'] = array(
		'#type' => 'textfield',
		'#size' => 5,
		'#field_suffix' => 'Einheiten',
		'#default_value' => 0,
		'#title' => 'Bestand',
		'#title_display' => 'invisible',
		'#required' => TRUE,
	);
	
	$form['regiomino_offer_create']['priceinfo']['vat'] = array(
		'#type' => 'select',
		'#size' => 4,
		'#default_value' => 7,
		'#options' => array(
			'0' => '0 %',
			'7' => '7 %',
			'10.7' => '10,7 %',
			'19' => '19 %',
		),
		// '#ajax' => array(
			// 'callback' => '_ajax_update_tradingprice_field_calculation',
			// 'wrapper' => 'tradingprices-fieldset-wrapper',
			// 'event' => 'change',
		// ),
		'#title' => 'MwSt.',
		'#title_display' => 'invisible',
	);
	
	$form['regiomino_offer_create']['priceinfo']['tradingprices'] = array(
    '#type' => 'tableform',
	  '#prefix' => '<div id="tradingprices-fieldset-wrapper">',
    '#suffix' => '</div>',
    '#header' => array(
      'Anzahl',
      'Ihr Netto-Erlös',
      'Aufpreis Lieferung',
      'Aufpreis RM-Gebühr',
      'Preis (netto)',
      'zzgl. MwSt.',
      'Preis (brutto)',
			'Kundentyp',
    ),
    '#options'  => array(		
      array(
        'tradingprice_1_amount' => array(
          '#type'           => 'textfield',
          '#size'           => 1,
          '#default_value'  => 1,
					'#field_suffix'		=> 'Einheiten',
        ),
        'tradingprice_1_netrevenue' => array(
          '#type'           => 'textfield',
          '#size'           => 3,  
          '#required'       => TRUE,
					'#default_value'	=> number_format(0, 2, ",", "."),
					'#field_suffix'		=> '€',
/* 					'#ajax' => array(
						'callback' => '_ajax_return_calculated_tradingprices',
						'wrapper' => 'tradingprices-fieldset-wrapper',
						'event' => 'blur',
					), */
        ),
        'tradingprice_1_shipping' => array(
          '#type'           => 'textfield',
          '#size'           => 3,
					'#default_value'	=> number_format(0, 2, ",", "."),
					'#value'					=> number_format(0, 2, ",", "."),
					'#field_suffix'		=> '€',
					'#disabled'				=> TRUE,
        ),
        'tradingprice_1_rmfee' => array(
          '#type'           => 'textfield',
          '#size'           => 3,
					'#default_value'	=> number_format(0, 2, ",", "."),
					'#value'					=> number_format(0, 2, ",", "."),
					'#field_suffix'		=> '€',
					'#disabled'				=> TRUE,
        ),
        'tradingprice_1_listing' => array(
          '#type'           => 'textfield',
          '#size'           => 3,
					'#default_value'	=> number_format(0, 2, ",", "."),
					'#value'					=> number_format(0, 2, ",", "."),
          '#default_value'  => '0,00',
					'#field_suffix'		=> '€',
					'#disabled'				=> TRUE,
        ),
        'tradingprice_1_vat' => array(
          '#type'           => 'textfield',
          '#size'           => 3,
					'#default_value'	=> number_format(0, 2, ",", "."),
					'#value'					=> number_format(0, 2, ",", "."),
					'#field_suffix'		=> '€',
					'#disabled'				=> TRUE,
        ),
        'tradingprice_1_gross' => array(
          '#type'           => 'textfield',
          '#size'           => 3,
					'#default_value'	=> number_format(0, 2, ",", "."),
					'#value'					=> number_format(0, 2, ",", "."),
					'#field_suffix'		=> '€',
					'#disabled'				=> TRUE,
        ),
        'tradingprice_1_customertype' => array(
          '#type'           => 'select',
          '#size'           => 1,
					'#default_value' => 'private',
					'#options'				=> array(
						'private'	=> 'Privat',
						'commercial'	=> 'Gewerblich',
					),
					// '#ajax' => array(
						// 'callback' => '_ajax_update_tradingprice_field_calculation',
						// 'wrapper' => 'tradingprices-fieldset-wrapper',
						// 'event' => 'change',
					// ),
        ),
      ),			
      array(
        'tradingprice_2_amount' => array(
          '#type'           => 'textfield',
          '#size'           => 1,
					'#default_value'	=> 10,
					'#field_suffix'		=> 'Einheiten',
        ),
        'tradingprice_2_netrevenue' => array(
          '#type'           => 'textfield',
          '#size'           => 3,  
          '#required'       => TRUE,
					'#default_value'	=> number_format(0, 2, ",", "."),
					'#field_suffix'		=> '€',
/* 					'#ajax' => array(
						'callback' => '_ajax_return_calculated_tradingprices',
						'wrapper' => 'tradingprices-fieldset-wrapper',
						'event' => 'blur',
					), */
        ),
        'tradingprice_2_shipping' => array(
          '#type'           => 'textfield',
          '#size'           => 3,
					'#default_value'	=> number_format(0, 2, ",", "."),
					'#value'					=> number_format(0, 2, ",", "."),
					'#field_suffix'		=> '€',
					'#disabled'				=> TRUE,
        ),
        'tradingprice_2_rmfee' => array(
          '#type'           => 'textfield',
          '#size'           => 3,
					'#default_value'	=> number_format(0, 2, ",", "."),
					'#value'					=> number_format(0, 2, ",", "."),
					'#field_suffix'		=> '€',
					'#disabled'				=> TRUE,
        ),
        'tradingprice_2_listing' => array(
          '#type'           => 'textfield',
          '#size'           => 3,
					'#default_value'	=> number_format(0, 2, ",", "."),
					'#value'					=> number_format(0, 2, ",", "."),
					'#field_suffix'		=> '€',
					'#disabled'				=> TRUE,
        ),
        'tradingprice_2_vat' => array(
          '#type'           => 'textfield',
          '#size'           => 3,
					'#default_value'	=> number_format(0, 2, ",", "."),
					'#value'					=> number_format(0, 2, ",", "."),
					'#field_suffix'		=> '€',
					'#disabled'				=> TRUE,
        ),
        'tradingprice_2_gross' => array(
          '#type'           => 'textfield',
          '#size'           => 3,
					'#default_value'	=> number_format(0, 2, ",", "."),
					'#value'					=> number_format(0, 2, ",", "."),
					'#field_suffix'		=> '€',
					'#disabled'				=> TRUE,
        ),
        'tradingprice_2_customertype' => array(
          '#type'           => 'select',
          '#size'           => 1,
					'#default_value' => 'private',
					'#options'				=> array(
						'private'	=> 'Privat',
						'commercial'	=> 'Gewerblich',
					),
					// '#ajax' => array(
						// 'callback' => '_ajax_update_tradingprice_field_calculation',
						// 'wrapper' => 'tradingprices-fieldset-wrapper',
						// 'event' => 'change',
					// ),
        ),
      ),	
      array(
        'tradingprice_3_amount' => array(
          '#type'           => 'textfield',
          '#size'           => 1,
					'#default_value'	=> 10,
					'#field_suffix'		=> 'Einheiten',
        ),
        'tradingprice_3_netrevenue' => array(
          '#type'           => 'textfield',
          '#size'           => 3,  
          '#required'       => TRUE,
					'#default_value'	=> number_format(0, 2, ",", "."),
					'#field_suffix'		=> '€',
/* 					'#ajax' => array(
						'callback' => '_ajax_return_calculated_tradingprices',
						'wrapper' => 'tradingprices-fieldset-wrapper',
						'event' => 'blur',
					), */
        ),
        'tradingprice_3_shipping' => array(
          '#type'           => 'textfield',
          '#size'           => 3,
					'#default_value'	=> number_format(0, 2, ",", "."),
					'#value'					=> number_format(0, 2, ",", "."),
					'#field_suffix'		=> '€',
					'#disabled'				=> TRUE,
        ),
        'tradingprice_3_rmfee' => array(
          '#type'           => 'textfield',
          '#size'           => 3,
					'#default_value'	=> number_format(0, 2, ",", "."),
					'#value'					=> number_format(0, 2, ",", "."),
					'#field_suffix'		=> '€',
					'#disabled'				=> TRUE,
        ),
        'tradingprice_3_listing' => array(
          '#type'           => 'textfield',
          '#size'           => 3,
					'#default_value'	=> number_format(0, 2, ",", "."),
					'#value'					=> number_format(0, 2, ",", "."),
					'#field_suffix'		=> '€',
					'#disabled'				=> TRUE,
        ),
        'tradingprice_3_vat' => array(
          '#type'           => 'textfield',
          '#size'           => 3,
					'#default_value'	=> number_format(0, 2, ",", "."),
					'#value'					=> number_format(0, 2, ",", "."),
					'#field_suffix'		=> '€',
					'#disabled'				=> TRUE,
        ),
        'tradingprice_3_gross' => array(
          '#type'           => 'textfield',
          '#size'           => 3,
					'#default_value'	=> number_format(0, 2, ",", "."),
					'#value'					=> number_format(0, 2, ",", "."),
					'#field_suffix'		=> '€',
					'#disabled'				=> TRUE,
        ),
        'tradingprice_3_customertype' => array(
          '#type'           => 'select',
          '#size'           => 1,
					'#default_value' => 'commercial',
					'#options'				=> array(
						'private'	=> 'Privat',
						'commercial'	=> 'Gewerblich',
					),
					// '#ajax' => array(
						// 'callback' => '_ajax_update_tradingprice_field_calculation',
						// 'wrapper' => 'tradingprices-fieldset-wrapper',
						// 'event' => 'change',
					// ),
        ),
      ),
      array(
        'tradingprice_4_amount' => array(
          '#type'           => 'textfield',
          '#size'           => 1,
					'#default_value'	=> 100,
					'#field_suffix'		=> 'Einheiten',
        ),
        'tradingprice_4_netrevenue' => array(
          '#type'           => 'textfield',
          '#size'           => 3,  
          '#required'       => TRUE,
					'#default_value'	=> number_format(0, 2, ",", "."),
					'#field_suffix'		=> '€',
/* 					'#ajax' => array(
						'callback' => '_ajax_return_calculated_tradingprices',
						'wrapper' => 'tradingprices-fieldset-wrapper',
						'event' => 'blur',
					), */
        ),
        'tradingprice_4_shipping' => array(
          '#type'           => 'textfield',
          '#size'           => 3,
					'#default_value'	=> number_format(0, 2, ",", "."),
					'#value'					=> number_format(0, 2, ",", "."),
					'#field_suffix'		=> '€',
					'#disabled'				=> TRUE,
        ),
        'tradingprice_4_rmfee' => array(
          '#type'           => 'textfield',
          '#size'           => 3,
					'#default_value'	=> number_format(0, 2, ",", "."),
					'#value'					=> number_format(0, 2, ",", "."),
					'#field_suffix'		=> '€',
					'#disabled'				=> TRUE,
        ),
        'tradingprice_4_listing' => array(
          '#type'           => 'textfield',
          '#size'           => 3,
					'#default_value'	=> number_format(0, 2, ",", "."),
					'#value'					=> number_format(0, 2, ",", "."),
					'#field_suffix'		=> '€',
					'#disabled'				=> TRUE,
        ),
        'tradingprice_4_vat' => array(
          '#type'           => 'textfield',
          '#size'           => 3,
					'#default_value'	=> number_format(0, 2, ",", "."),
					'#value'					=> number_format(0, 2, ",", "."),
					'#field_suffix'		=> '€',
					'#disabled'				=> TRUE,
        ),
        'tradingprice_4_gross' => array(
          '#type'           => 'textfield',
          '#size'           => 3,
					'#default_value'	=> number_format(0, 2, ",", "."),
					'#value'					=> number_format(0, 2, ",", "."),
					'#field_suffix'		=> '€',
					'#disabled'				=> TRUE,
        ),
        'tradingprice_4_customertype' => array(
          '#type'           => 'select',
          '#size'           => 1,
					'#default_value' => 'commercial',
					'#options'				=> array(
						'private'	=> 'Privat',
						'commercial'	=> 'Gewerblich',
					),
					// '#ajax' => array(
						// 'callback' => '_ajax_update_tradingprice_field_calculation',
						// 'wrapper' => 'tradingprices-fieldset-wrapper',
						// 'event' => 'change',
					// ),
        ),
      ),
      array(
        'tradingprice_5_amount' => array(
          '#type'           => 'textfield',
          '#size'           => 1,
					'#field_suffix'		=> 'Einheiten',
        ),
        'tradingprice_5_netrevenue' => array(
          '#type'           => 'textfield',
          '#size'           => 3,  
          '#required'       => TRUE,
					'#default_value'	=> number_format(0, 2, ",", "."),
					'#field_suffix'		=> '€',
/* 					'#ajax' => array(
						'callback' => '_ajax_return_calculated_tradingprices',
						'wrapper' => 'tradingprices-fieldset-wrapper',
						'event' => 'blur',
					), */
        ),
        'tradingprice_5_shipping' => array(
          '#type'           => 'textfield',
          '#size'           => 3,
					'#default_value'	=> number_format(0, 2, ",", "."),
					'#value'					=> number_format(0, 2, ",", "."),
					'#field_suffix'		=> '€',
					'#disabled'				=> TRUE,
        ),
        'tradingprice_5_rmfee' => array(
          '#type'           => 'textfield',
          '#size'           => 3,
					'#default_value'	=> number_format(0, 2, ",", "."),
					'#value'					=> number_format(0, 2, ",", "."),
					'#field_suffix'		=> '€',
					'#disabled'				=> TRUE,
        ),
        'tradingprice_5_listing' => array(
          '#type'           => 'textfield',
          '#size'           => 3,
					'#default_value'	=> number_format(0, 2, ",", "."),
					'#value'					=> number_format(0, 2, ",", "."),
					'#field_suffix'		=> '€',
					'#disabled'				=> TRUE,
        ),
        'tradingprice_5_vat' => array(
          '#type'           => 'textfield',
          '#size'           => 3,
					'#default_value'	=> number_format(0, 2, ",", "."),
					'#value'					=> number_format(0, 2, ",", "."),
					'#field_suffix'		=> '€',
					'#disabled'				=> TRUE,
        ),
        'tradingprice_5_gross' => array(
          '#type'           => 'textfield',
          '#size'           => 3,
					'#default_value'	=> number_format(0, 2, ",", "."),
					'#value'					=> number_format(0, 2, ",", "."),
					'#field_suffix'		=> '€',
					'#disabled'				=> TRUE,
        ),
        'tradingprice_5_customertype' => array(
          '#type'           => 'select',
          '#size'           => 1,
					'#default_value' => 'commercial',
					'#options'				=> array(
						'private'	=> 'Privat',
						'commercial'	=> 'Gewerblich',
					),
					// '#ajax' => array(
						// 'callback' => '_ajax_update_tradingprice_field_calculation',
						// 'wrapper' => 'tradingprices-fieldset-wrapper',
						// 'event' => 'change',
					// ),
        ),
      ),
    ),  
  );
	
	
	$form['regiomino_offer_create']['logisticsinfo'] = array(
		'#type' => 'fieldset',
		'#collapsible' => TRUE,
		'#collapsed' => FALSE,
		'#title' => '3 ' . 'Logistik Informationen eingeben',
	);
	
	$form['regiomino_offer_create']['logisticsinfo']['delay'] = array(
		'#type' => 'textfield',
		'#size' => 5,
		'#default_value' => 0,
		'#field_suffix' => 'Stunden',
		'#title' => 'Aufschub zwischen Bestellung und Abholung',
		'#title_display' => 'invisible',
	);
	
	$form['regiomino_offer_create']['logisticsinfo']['cooling'] = array(
		'#type' => 'select',
		'#field_suffix' => '°C',
		'#default_value' => 7,
		'#options' => array(
			-18 => -18,
			-14 => -14,
			2 => 2,
			4 => 4,
			7 => 7,
			10 => 10,
			20 => 20,
		),
	);

	$form['regiomino_offer_create']['logisticsinfo']['shippingweight'] = array(
		'#type' => 'textfield',
		'#size' => 20,
		'#required' => TRUE,
		'#field_suffix' => 'g',
		'#attributes' => array('placeholder' => 'Gewicht (in Gramm)'),
		'#title' => 'Versandgewicht',
		'#title_display' => 'invisible',
	);
	
	$form['regiomino_offer_create']['submit'] = array(
		'#type' => 'submit',
	);
	
	return $form;
}


function regiomino_admin_manage_offers_active($form, &$form_state) {

	global $user;
	
	$header = array(
		'nid' => array(
			'data' => 'Art.#',
			'type' => 'property',
			'specifier' => 'nid',
			'sort' => 'desc',
		),
		'title' => array(
			'data' => 'Titel',
			'type' => 'property',
			'specifier' => 'title',
		),
		'field_packingunit' => array(
			'data' => 'Produkteinheit',
			'type' => 'field',
			'specifier' => array(
				'field' => 'field_packingunit',
				'column' => 'first',
			),
		),
		'field_stock' => array(
			'data' => 'Produktbestand',
			'type' => 'field',
			'specifier' => array(
				'field' => 'field_stock',
				'column' => 'value',
			),
		),
		'field_tu_revenue_private' => 'Netto-Erlös (privat)',
		'field_tu_revenue_commercial' => 'Netto-Erlös (gewerblich)',
		'edit' => 'Bearbeiten',
	);
	
	$query = new EntityFieldQuery;
	$tmp = $query
		->entityCondition('entity_type', 'node')
		->entityCondition('bundle', array('offer'))
		->propertyCondition('uid', $user->uid)
		->propertyCondition('status', 1)
		->propertyCondition('soldout', 0)
		->tableSort($header)
		->pager(50)
		->execute();
	
	$allactiveoffers = entity_load('node', array_keys($tmp['node']));
	
	// $form = array();
			
	$options = array();
	foreach($allactiveoffers as $nid => $activeoffer) {
	
	
	
	
		$fcloadarray = array();
		if(isset($activeoffer->field_tradingunits[LANGUAGE_NONE])) {
			foreach($activeoffer->field_tradingunits[LANGUAGE_NONE] as $key => $value) {
				$fcloadarray[] = $value['value'];
			}
		}
		//Load all tradingunits stored in the node
		$fcis_commercial = $fcis_private = entity_load('field_collection_item', $fcloadarray);
		
		//Filter out prices that do not belong to the customertype (private or commercial)
		if(isset($fcis_commercial) && !empty($fcis_commercial)) {
			foreach($fcis_commercial as $key=>$value) {
				if(isset($value->field_tu_customertype[LANGUAGE_NONE][0]['value']) && $value->field_tu_customertype[LANGUAGE_NONE][0]['value'] != 'commercial') unset($fcis_commercial[$key]);
			}
		}
		//If prices are left, continue here.
		if(isset($fcis_commercial) && !empty($fcis_commercial)) {
			//Sort DESC by tradingunit size without key preservation
			usort($fcis_commercial, "regiomino_offer_sort_tradingunits");
		}
		
		//Filter out prices that do not belong to the customertype (private or commercial)
		if(isset($fcis_private) && !empty($fcis_private)) {
			foreach($fcis_private as $key=>$value) {
				if(isset($value->field_tu_customertype[LANGUAGE_NONE][0]['value']) && $value->field_tu_customertype[LANGUAGE_NONE][0]['value'] != 'private') unset($fcis_private[$key]);
			}
		}
		//If prices are left, continue here. Otherwise NULL is returned
		if(isset($fcis_private) && !empty($fcis_private)) {
			//Sort DESC by tradingunit size without key preservation
			usort($fcis_private, "regiomino_offer_sort_tradingunits");
		}
		
		if(isset($fcis_commercial[0]->field_tu_size[LANGUAGE_NONE][0]['value']) && !empty($fcis_commercial[0]->field_tu_size[LANGUAGE_NONE][0]['value'])) {
			$field_tu_size_commercial_default = $fcis_commercial[0]->field_tu_size[LANGUAGE_NONE][0]['value'];
			$field_tu_revenue_commercial_default = number_format($fcis_commercial[0]->field_tu_revenue[LANGUAGE_NONE][0]['value'], 2, ",", "");
		}
		else {
			$field_tu_size_commercial_default = 1;
			$field_tu_revenue_commercial_default = '-';
		}
		
		if(isset($fcis_private[0]->field_tu_size[LANGUAGE_NONE][0]['value']) && !empty($fcis_private[0]->field_tu_size[LANGUAGE_NONE][0]['value'])) {
			$field_tu_size_private_default = $fcis_private[0]->field_tu_size[LANGUAGE_NONE][0]['value'];
			$field_tu_revenue_private_default = number_format($fcis_private[0]->field_tu_revenue[LANGUAGE_NONE][0]['value'], 2, ",", "");
		}
		else {
			$field_tu_size_private_default = 1;
			$field_tu_revenue_private_default = '-';
		}
	
		$options[$nid] = array(
				'nid' => array('data' => $activeoffer->nid, 'specifier' => 'nid', 'class' => 'nid' . $activeoffer->nid),
				'title' => array('data' => l($activeoffer->title, 'node/' . $activeoffer->nid), 'specifier' => 'title'),
				'field_packingunit' => l($activeoffer->field_packingunit[LANGUAGE_NONE][0]['first'], 'javascript:', array('external' => TRUE, 'attributes' => array('class' => array('editablefield', 'nid' . $activeoffer->nid, 'field_packingunit_first'), 'data-value' => $activeoffer->field_packingunit[LANGUAGE_NONE][0]['first']))) . ' ' . t($activeoffer->field_packingunit[LANGUAGE_NONE][0]['second']),
				'field_stock' => l($activeoffer->field_stock[LANGUAGE_NONE][0]['value'], 'javascript:', array('external' => TRUE, 'attributes' => array('class' => array('editablefield', 'nid' . $activeoffer->nid, 'field_stock'), 'data-value' => $activeoffer->field_stock[LANGUAGE_NONE][0]['value']))),
				'field_tu_revenue_private' => l($field_tu_revenue_private_default, 'javascript:', array('external' => TRUE, 'attributes' => array('class' => array('editablefield', 'nid' . $activeoffer->nid, 'field_tu_revenue_private'), 'data-value' => number_format($fcis_private[0]->field_tu_revenue[LANGUAGE_NONE][0]['value'], 2, ",", "")))) . " € für " . number_format($field_tu_size_private_default) . " Produkteinheit(en)",
				'field_tu_revenue_commercial' => l($field_tu_revenue_commercial_default, 'javascript:', array('external' => TRUE, 'attributes' => array('class' => array('editablefield', 'nid' . $activeoffer->nid, 'field_tu_revenue_commercial'), 'data-value' => number_format($fcis_commercial[0]->field_tu_revenue[LANGUAGE_NONE][0]['value'], 2, ",", "")))) . " € für " . number_format($field_tu_size_commercial_default) . " Produkteinheit(en)",
				'edit' => l('Datensatz bearbeiten', 'admin/offers/edit/' . $nid, array('query' => drupal_get_destination(), 'attributes' => array('class' => array('nid' . $activeoffer->nid)))),
		);
	}
  $form['submit_clone'] = array(
		'#type' => 'submit',
		'#value' => 'Neue Datensätze aus gewählten Angeboten erstellen',
		'#submit' => array('regiomino_admin_manage_offers_selected_clone'),
	);
  $form['submit_model'] = array(
		'#type' => 'submit',
		'#value' => 'Vorlagen aus gewählten Angeboten erstellen',
		'#submit' => array('regiomino_admin_manage_offers_selected_model'),
	);
  $form['submit_end'] = array(
		'#type' => 'submit',
		'#value' => 'Gewählten Angebote beenden',
		'#submit' => array('regiomino_admin_manage_offers_selected_end'),
	);
	$form['offers'] = array(
		'#type' => 'tableselect',
		'#header' => $header,
		'#options' => $options,
	);
	$form['#header'] = $header;
	$form['pager'] = array('#markup' => theme('pager'));

	return $form;
}

function regiomino_admin_manage_offers_templates ($form, &$form_state) {
	global $user;
	
	$header = array(
		'nid' => array(
			'data' => 'Art.#',
			'type' => 'property',
			'specifier' => 'nid',
			'sort' => 'desc',
		),
		'title' => array(
			'data' => 'Titel',
			'type' => 'property',
			'specifier' => 'title',
		),
		'field_packingunit' => array(
			'data' => 'Verkaufseinheit',
			'type' => 'field',
			'specifier' => array(
				'field' => 'field_packingunit',
				'column' => 'first',
			),
		),
		'edit' => 'Bearbeiten',
	);
	
	$query = new EntityFieldQuery;
	$tmp = $query
		->entityCondition('entity_type', 'node')
		->entityCondition('bundle', array('offer'))
		->propertyCondition('uid', $user->uid)
		->propertyCondition('status', 0)
		->propertyCondition('model', 1)
		->tableSort($header)
		->pager(50)
		->execute();
	
	$allarchivedoffers = entity_load('node', array_keys($tmp['node']));

	$form = array();

	$options = array();
	foreach($allarchivedoffers as $nid => $archivedoffer) {
		$options[$nid] = array(
			'nid' => array('data' => $archivedoffer->nid, 'specifier' => 'nid'),
			'title' => array('data' => l($archivedoffer->title, 'node/' . $archivedoffer->nid), 'specifier' => 'title'),
			'field_packingunit' => $archivedoffer->field_packingunit[LANGUAGE_NONE][0]['first'] . ' ' . t($archivedoffer->field_packingunit[LANGUAGE_NONE][0]['second']),
			'edit' => l('Datensatz bearbeiten', 'admin/offers/edit/' . $nid, array('query' => drupal_get_destination())),
		);
	}
	
  $form['submit_clone'] = array(
		'#type' => 'submit',
		'#value' => 'Neue Datensätze aus gewählten Vorlagen erstellen',
		'#submit' => array('regiomino_admin_manage_offers_selected_clone'),
	);
  $form['submit_delete'] = array(
		'#type' => 'submit',
		'#value' => 'Gewählte Vorlagen löschen',
		'#submit' => array('regiomino_admin_manage_offers_selected_delete'),
	);
	$form['offers'] = array(
		'#type' => 'tableselect',
		'#header' => $header,
		'#options' => $options,
	);
	$form['pager'] = array('#markup' => theme('pager'));

	return $form;
}

function regiomino_admin_manage_offers_unpublished($form, &$form_state) {
	global $user;
	
	$header = array(
		'nid' => array(
			'data' => 'Art.#',
			'type' => 'property',
			'specifier' => 'nid',
			'sort' => 'desc',
		),
		'title' => array(
			'data' => 'Titel',
			'type' => 'property',
			'specifier' => 'title',
		),
		'field_packingunit' => array(
			'data' => 'Verkaufseinheit',
			'type' => 'field',
			'specifier' => array(
				'field' => 'field_packingunit',
				'column' => 'first',
			),
		),
		'edit' => 'Bearbeiten',
	);
	
	$query = new EntityFieldQuery;
	$tmp = $query
		->entityCondition('entity_type', 'node')
		->entityCondition('bundle', array('offer'))
		->propertyCondition('uid', $user->uid)
		->propertyCondition('status', 0)
		->propertyCondition('soldout', 0)		
		->propertyCondition('model', 0)
		->tableSort($header)
		->pager(50)
		->execute();
	
	$allarchivedoffers = entity_load('node', array_keys($tmp['node']));

	$form = array();

	$options = array();
	foreach($allarchivedoffers as $nid => $archivedoffer) {
		$options[$nid] = array(
			'nid' => array('data' => $archivedoffer->nid, 'specifier' => 'nid'),
			'title' => array('data' => l($archivedoffer->title, 'node/' . $archivedoffer->nid), 'specifier' => 'title'),
			'field_packingunit' => $archivedoffer->field_packingunit[LANGUAGE_NONE][0]['first'] . ' ' . t($archivedoffer->field_packingunit[LANGUAGE_NONE][0]['second']),
			'edit' => l('Datensatz bearbeiten', 'admin/offers/edit/' . $nid, array('query' => drupal_get_destination())),
		);
	}
  $form['submit_clone'] = array(
		'#type' => 'submit',
		'#value' => 'Gewählte Angebote kopieren',
		'#submit' => array('regiomino_admin_manage_offers_selected_clone'),
	);
  $form['submit_delete'] = array(
		'#type' => 'submit',
		'#value' => 'Gewählte Angebote löschen',
		'#submit' => array('regiomino_admin_manage_offers_selected_delete'),
	);
  $form['submit_publish'] = array(
		'#type' => 'submit',
		'#value' => 'Gewählte Angebote veröffentlichen',
		'#submit' => array('regiomino_admin_manage_offers_selected_publish'),
	);
  $form['submit_model'] = array(
		'#type' => 'submit',
		'#value' => 'Vorlagen aus gewählten Angeboten erstellen',
		'#submit' => array('regiomino_admin_manage_offers_selected_model'),
	);
	$form['offers'] = array(
		'#type' => 'tableselect',
		'#header' => $header,
		'#options' => $options,
	);
	$form['pager'] = array('#markup' => theme('pager'));

	return $form;
}

function regiomino_admin_manage_offers_archive($form, &$form_state) {
	global $user;
	
	$header = array(
		'nid' => array(
			'data' => 'Art.#',
			'type' => 'property',
			'specifier' => 'nid',
			'sort' => 'desc',
		),
		'title' => array(
			'data' => 'Titel',
			'type' => 'property',
			'specifier' => 'title',
		),
		'field_packingunit' => array(
			'data' => 'Verkaufseinheit',
			'type' => 'field',
			'specifier' => array(
				'field' => 'field_packingunit',
				'column' => 'first',
			),
		),
	);
	
	$query = new EntityFieldQuery;
	$tmp = $query
		->entityCondition('entity_type', 'node')
		->entityCondition('bundle', array('offer'))
		->propertyCondition('uid', $user->uid)
		->propertyCondition('status', 1)
		->propertyCondition('soldout', 1)
		->propertyCondition('model', 0)
		->tableSort($header)
		//->fieldOrderBy('field_duration', 'value', 'DESC')
		->pager(50)
		->execute();
	
	$allarchivedoffers = entity_load('node', array_keys($tmp['node']));

	$form = array();

	$options = array();
	foreach($allarchivedoffers as $nid => $archivedoffer) {
		$options[$nid] = array(			
			'nid' => array('data' => $archivedoffer->nid, 'specifier' => 'nid'),
			'title' => array('data' => l($archivedoffer->title, 'node/' . $archivedoffer->nid), 'specifier' => 'title'),
			'field_packingunit' => $archivedoffer->field_packingunit[LANGUAGE_NONE][0]['first'] . ' ' . t($archivedoffer->field_packingunit[LANGUAGE_NONE][0]['second']),
		);
	}
  $form['submit_clone'] = array(
		'#type' => 'submit',
		'#value' => 'Neue Datensätze aus gewählten Angeboten erstellen',
		'#submit' => array('regiomino_admin_manage_offers_selected_clone'),
	);
  $form['submit_model'] = array(
		'#type' => 'submit',
		'#value' => 'Vorlagen aus gewählten Angeboten erstellen',
		'#submit' => array('regiomino_admin_manage_offers_selected_model'),
	);
	$form['offers'] = array(
		'#type' => 'tableselect',
		'#header' => $header,
		'#options' => $options,
	);
	$form['pager'] = array('#markup' => theme('pager'));

	return $form;
}

function regiomino_admin_manage_offers_selected_delete($form, &$form_state) {
	$tbd = array();
	foreach($form_state['values']['offers'] as $nid) {
		if($nid > 0) $tbd[] = $nid;
	}
	node_delete_multiple($tbd);
	global $user;
	watchdog('offer_management', 'Nodes @nodes have been deleted by user @user', array('@nodes' => print_r($tbd, TRUE), '@user' => $user->uid));
	drupal_set_message('Die gewählten Angebote wurden erfolgreich gelöscht.', 'status');
}

function regiomino_admin_manage_offers_selected_publish($form, &$form_state) {
	$tbd = array();
	foreach($form_state['values']['offers'] as $nid) {
		if($nid > 0) {
			$tbd[] = $nid;
			$node = node_load($nid);
			$node->status = 1;
			$node->soldout = 0;
			//Store field_duration
			$dateformat = explode('+', date('c', mktime(0, 0, 0, date("m"), date("d")+7, date("Y"))));
			$node->field_duration[LANGUAGE_NONE][0] = array(
				'value' => $dateformat[0],
				'timezone' => 'Europe/Berlin',
				'timezone_db' => 'Europe/Berlin',
				'date_type' => 'date',
			);
			node_save($node);
		}
	}
	global $user;
	watchdog('offer_management', 'Nodes @nodes have been published by user @user', array('@nodes' => print_r($tbd, TRUE), '@user' => $user->uid));
	drupal_set_message('Die gewählten Angebote wurden erfolgreich veröffentlicht.', 'status');
}

function regiomino_admin_manage_offers_selected_end($form, &$form_state) {
	$tbd = array();
	foreach($form_state['values']['offers'] as $nid) {
		if($nid > 0) {
			$tbd[] = $nid;
			$node = node_load($nid);
			$node->soldout = 1;
			$dateformat = explode('+', date('c', mktime(0, 0, 0, date("m"), date("d"), date("Y"))));
			$node->field_duration[LANGUAGE_NONE][0] = array(
				'value' => $dateformat[0],
				'timezone' => 'Europe/Berlin',
				'timezone_db' => 'Europe/Berlin',
				'date_type' => 'date',
			);
			node_save($node);
		}
	}
	global $user;
	watchdog('offer_management', 'Nodes @nodes have been ended by user @user', array('@nodes' => print_r($tbd, TRUE), '@user' => $user->uid));
	drupal_set_message('Die gewählten Angebote wurden erfolgreich beendet.', 'status');
}

function regiomino_admin_manage_offers_selected_clone($form, &$form_state) {
	$tbd = array();
	foreach($form_state['values']['offers'] as $nid) {
		if($nid > 0) {
			$tbd[] = $nid;
			$node = node_load($nid);
			$cloned_node = clone $node;
			$cloned_node->status = 0;
			$cloned_node->soldout = 0;
			$cloned_node->model = 0;
			$cloned_node->is_new = TRUE;
			$cloned_node->nid = NULL;
			$cloned_node->vid = NULL;
			$cloned_node->tnid = NULL;
			$cloned_node->title = str_replace('(VORLAGE) ', '', $cloned_node->title);
			$tradingunits = entity_load('field_collection_item', $cloned_node->field_tradingunits[LANGUAGE_NONE]);
			unset($cloned_node->field_tradingunits);
			node_save($cloned_node);
			foreach($tradingunits as $fcikey=>$fcivalue) {
				regiomino_offer_add_tradingunit($cloned_node, $fcivalue->field_tu_size[LANGUAGE_NONE][0]['value'], $fcivalue->field_tu_revenue[LANGUAGE_NONE][0]['value'], $fcivalue->field_tu_customertype[LANGUAGE_NONE][0]['value']);
			}
		}
	}
	global $user;
	watchdog('offer_management', 'Nodes @nodes have been cloned by user @user', array('@nodes' => print_r($tbd, TRUE), '@user' => $user->uid));
	drupal_set_message('Die gewählten Angebote wurden erfolgreich kopiert und sind jetzt unter "Unveröffentlichte Angebote" verfügbar.', 'status');
}

function regiomino_admin_manage_offers_selected_model($form, &$form_state) {
	$tbd = array();
	foreach($form_state['values']['offers'] as $nid) {
		if($nid > 0) {
			$tbd[] = $nid;
			$node = node_load($nid);
			$cloned_node = clone $node;
			$cloned_node->status = 0;
			$cloned_node->soldout = 0;
			$cloned_node->model = 1;
			$cloned_node->is_new = TRUE;
			$cloned_node->nid = NULL;
			$cloned_node->vid = NULL;
			$cloned_node->tnid = NULL;
			$cloned_node->title = $cloned_node->title;
			$tradingunits = entity_load('field_collection_item', $cloned_node->field_tradingunits[LANGUAGE_NONE]);
			unset($cloned_node->field_tradingunits);
			node_save($cloned_node);
			foreach($tradingunits as $fcikey=>$fcivalue) {
				regiomino_offer_add_tradingunit($cloned_node, $fcivalue->field_tu_size[LANGUAGE_NONE][0]['value'], $fcivalue->field_tu_revenue[LANGUAGE_NONE][0]['value'], $fcivalue->field_tu_customertype[LANGUAGE_NONE][0]['value']);
			}
		}
	}
	global $user;
	watchdog('offer_management', 'Nodes @nodes have been cloned by user @user', array('@nodes' => print_r($tbd, TRUE), '@user' => $user->uid));
	drupal_set_message('Die gewählten Angebote wurden erfolgreich kopiert und sind jetzt unter "Vorlagen" verfügbar.', 'status');
}

function regiomino_admin_incomingorders($displaytype = 'grouped', $timerange = 'upcoming') {
	global $user;

	if($displaytype == 'old') {
		$start = 0;
		$end = strtotime('today')-1;
	}
	else {
		$start = strtotime('today');
		$end = strtotime('tomorrow +100 week');
	}

	$query = new EntityFieldQuery;
	$shpqry = $query
		->entityCondition('entity_type', 'regiomino_order_shipping')
		->propertyCondition('pickup_range_to', $start, '>')
		->propertyCondition('pickup_range_to', $end, '<')
		->execute();
	$shpkeys = array_keys($shpqry['regiomino_order_shipping']);
	
	$query = new EntityFieldQuery;
	$orderqry = $query
		->entityCondition('entity_type', 'regiomino_order')
		->propertyCondition('shipping', $shpkeys)
		->propertyCondition('uid_s', $user->uid)
		->propertyCondition('order_status', array('delivered', 'processing', 'billed', 'completed'))
		->propertyOrderBy('order_id', 'DESC')
		->pager(200)
		->execute();
	$orderobjects = entity_load('regiomino_order', array_keys($orderqry['regiomino_order']));
	$orders = array();
	foreach($orderobjects as $order_id=>$orderobject) {
		$orderobjects[$order_id]->shipping_item = entity_load('regiomino_order_shipping', array($orderobject->shipping));
		$orders[$orderobjects[$order_id]->shipping_item[$orderobject->shipping]->pickup_range_from][$orderobjects[$order_id]->shipping_item[$orderobject->shipping]->fci_p][json_encode($orderobject->delivery_first_name . ' ' . $orderobject->delivery_last_name . ' (Kundennr.: ' . $orderobject->uid . ')')][] = $orderobject;
	}
	
	switch($displaytype) {
		case "grouped":
			$returnorders = array();
			$allkeys = array_keys($orders);
			if($timerange == 'upcoming') {
				$returnorders[$allkeys[0]] = $orders[$allkeys[0]];
			}
			else {
				unset($orders[$allkeys[0]]);
				$returnorders = $orders;
			}
			return	theme('regiomino_admin_theme_incomingorders_grouped', array(
								'vars' => array(
									'orders' => $returnorders,
									'pager' => theme('pager'),
								),
							));
		case "solo":
			return	theme('regiomino_admin_theme_incomingorders_solo', array(
								'vars' => array(
									'orders' => $orderobjects,
									'pager' => theme('pager'),
								),
							));
		case "old":
			return	theme('regiomino_admin_theme_incomingorders_old', array(
								'vars' => array(
									'orders' => $orderobjects,
									'pager' => theme('pager'),
								),
							));
	}
}


function regiomino_admin_allorders() {

	$queryparams = drupal_get_query_parameters();

	$form = drupal_get_form('regiomino_admin_allorders_filterform');

	$query = new EntityFieldQuery;
	
	$query->entityCondition('entity_type', 'regiomino_order');
	
	if(isset($queryparams['order_id']) && !empty($queryparams['order_id'])) $query->propertyCondition('order_id', $queryparams['order_id']);
	
	if(isset($queryparams['shipping']) && !empty($queryparams['shipping'])) $query->propertyCondition('shipping', $queryparams['shipping']);
	
	if(isset($queryparams['order_process_id']) && !empty($queryparams['order_process_id'])) $query->propertyCondition('order_process_id', $queryparams['order_process_id']);
	
	if(isset($queryparams['uid']) && !empty($queryparams['uid'])) $query->propertyCondition('uid', $queryparams['uid']);
	
	if(isset($queryparams['seller_postal_code']) && !empty($queryparams['seller_postal_code'])) $query->propertyCondition('seller_postal_code', $queryparams['seller_postal_code'], 'CONTAINS');
	
	if(isset($queryparams['delivery_postal_code']) && !empty($queryparams['delivery_postal_code'])) $query->propertyCondition('delivery_postal_code', $queryparams['delivery_postal_code'], 'CONTAINS');
	
	if(isset($queryparams['delivery_first_name']) && !empty($queryparams['delivery_first_name'])) $query->propertyCondition('delivery_first_name', $queryparams['delivery_first_name'], 'CONTAINS');
	
	if(isset($queryparams['delivery_last_name']) && !empty($queryparams['delivery_last_name'])) $query->propertyCondition('delivery_last_name', $queryparams['delivery_last_name'], 'CONTAINS');
	
	if(isset($queryparams['frequency'])) {
		$query->propertyCondition('data', $queryparams['frequency'], 'CONTAINS');
	}
	
	if(isset($queryparams['nid']) && !empty($queryparams['nid'])) $query->propertyCondition('nid', $queryparams['nid']);
	
	if(isset($queryparams['product_name']) && !empty($queryparams['product_name'])) {
		$productqry = new EntityFieldQuery;
		$products = $productqry
			->entityCondition('entity_type', 'node')
			->entityCondition('bundle', array('offer'))
			->propertyCondition('status', 1)
			->propertyCondition('title', $queryparams['product_name'], 'CONTAINS')
			->execute();
		$productkeys = array_keys($products['node']);
		$query->propertyCondition('nid', $productkeys);
	}
	
	if(isset($queryparams['seller_company']) && !empty($queryparams['seller_company'])) $query->propertyCondition('seller_company', $queryparams['seller_company'], 'CONTAINS');
	
	if(isset($queryparams['shipping_type']) && !empty($queryparams['shipping_type'])) {
		$shippingsqry = new EntityFieldQuery;
		$shippings = $shippingsqry
			->entityCondition('entity_type', 'regiomino_order_shipping')
			->propertyCondition('shipping_type', $queryparams['shipping_type'])
			->execute();
		$shippingkeys = array_keys($shippings['regiomino_order_shipping']);
		$query->propertyCondition('shipping', $shippingkeys);
	}
	
	if(isset($queryparams['order_status']) && !empty($queryparams['order_status'])) $query->propertyCondition('order_status', $queryparams['order_status']);
	
	if(isset($queryparams['pickup_name']) && !empty($queryparams['pickup_name'])) {
		$pickersqry = new EntityFieldQuery;
		$pickers = $pickersqry
			->entityCondition('entity_type', 'node')
			->entityCondition('bundle', array('shipper_profile'))
			->propertyCondition('status', 1)
			->propertyCondition('title', $queryparams['pickup_name'], 'CONTAINS')
			->execute();
		$pickerskeys = array_keys($pickers['node']);
		
		$pickersuserqry = new EntityFieldQuery;
		$pickersuser = $pickersuserqry
			->entityCondition('entity_type', 'user')
			->fieldCondition('field_profilereference_shipper', 'target_id', $pickerskeys)
			->execute();
		$pickersuserkeys = array_keys($pickersuser['user']);		
		
		$shippingsqry = new EntityFieldQuery;
		$shippings = $shippingsqry
			->entityCondition('entity_type', 'regiomino_order_shipping')
			->propertyCondition('uid_p', $pickersuserkeys)
			->execute();
		$shippingkeys = array_keys($shippings['regiomino_order_shipping']);
		
		$query->propertyCondition('shipping', $shippingkeys);
	}
	
	if(isset($queryparams['shipper_name']) && !empty($queryparams['shipper_name'])) {
	
		$pickersqry = new EntityFieldQuery;
		$pickers = $pickersqry
			->entityCondition('entity_type', 'node')
			->entityCondition('bundle', array('shipper_profile'))
			->propertyCondition('status', 1)
			->propertyCondition('title', $queryparams['shipper_name'], 'CONTAINS')
			->execute();
		$pickerskeys = array_keys($pickers['node']);
		
		$pickersuserqry = new EntityFieldQuery;
		$pickersuser = $pickersuserqry
			->entityCondition('entity_type', 'user')
			->fieldCondition('field_profilereference_shipper', 'target_id', $pickerskeys)
			->execute();
		$pickersuserkeys = array_keys($pickersuser['user']);		
		
		$shippingsqry = new EntityFieldQuery;
		$shippings = $shippingsqry
			->entityCondition('entity_type', 'regiomino_order_shipping')
			->propertyCondition('uid', $pickersuserkeys)
			->execute();
		$shippingkeys_1 = array_keys($shippings['regiomino_order_shipping']);
		
		$queryparams['shipper_name'] = str_replace('Point ', '', $queryparams['shipper_name']);
		$queryparams['shipper_name'] = str_replace('point ', '', $queryparams['shipper_name']);
		
		$pickersqry = new EntityFieldQuery;
		$pickers = $pickersqry
			->entityCondition('entity_type', 'field_collection_item')
			->entityCondition('bundle', 'field_storage_data')
			->fieldCondition('field_address', 'name_line', $queryparams['shipper_name'], 'CONTAINS')
			->execute();
		$pickerskeys = array_keys($pickers['field_collection_item']);
		
		$shippingsqry = new EntityFieldQuery;
		$shippings = $shippingsqry
			->entityCondition('entity_type', 'regiomino_order_shipping')
			->propertyCondition('fci', $pickerskeys)
			->execute();
		$shippingkeys_2 = array_keys($shippings['regiomino_order_shipping']);

		if(!empty($shippingkeys_1)) {
			array_push($shippingkeys_1, $shippingkeys_2);
		}
		else {
			$shippingkeys_1 = $shippingkeys_2;
		}
		
		$query->propertyCondition('shipping', $shippingkeys_1);
	}
	
	$query->propertyOrderBy('order_id', 'DESC');
	$query->pager(100);
	$orderqry = $query->execute();
	$orderobjects = entity_load('regiomino_order', array_keys($orderqry['regiomino_order']));
	
	$header = array(
		'Pos.',
		'Best.',
		'Vrg.',
		'Name des Kunden',
		'Bezeichnung',
		'Mg.',
		'Einzelpreis',
		'Verkäufer',
		'Status',
		'Bestellt am',
		'Abholung durch',
		'Abholung in',
		'Auslieferung durch',
		'Auslieferung in',
		'Auslieferung am',
		'Abo',
		'Kommentare',
	);
	
	$rows = array();
	foreach($orderobjects as $orderid=>$orderobject) {
		$productnode = node_load($orderobject->nid);
		$shippingobject = entity_load('regiomino_order_shipping', array($orderobject->shipping));
		$skeys = array_keys($shippingobject);
		$fcipicker = node_load(user_load($shippingobject[$skeys[0]]->uid_p)->field_profilereference_shipper[LANGUAGE_NONE][0]['target_id'])->title;
		if($shippingobject[$skeys[0]]->shipping_type == 'bringlivery') {
			$fcishipper = node_load(user_load($shippingobject[$skeys[0]]->uid)->field_profilereference_shipper[LANGUAGE_NONE][0]['target_id'])->title;
		}
		else {
			$fcishipper = 'Point ' . field_collection_item_load($shippingobject[$skeys[0]]->fci)->field_address[LANGUAGE_NONE][0]['name_line'];
		}
		
		$orderstatus = 'NN';
		switch($orderobject->order_status) {
			case "pending": $orderstatus = t('Payment pending'); break;
			case "canceled": $orderstatus = t('Canceled'); break;
			case "completed": $orderstatus = t('Payment completed'); break;
			case "delivered": $orderstatus = t('Delivered'); break;
			case "billed": $orderstatus = t('Billed'); break;
			case "processing": $orderstatus = t('Processing'); break;
		}
		$frequencyfield = field_info_field('field_frequency');
		$allowed_values_frequency = list_allowed_values($frequencyfield);
		$givenfrequency = $allowed_values_frequency[$orderobject->data['frequency']];
		if(empty($givenfrequency)) $givenfrequency = $allowed_values_frequency[0];
		$row = array(
			$orderobject->order_id,
			$orderobject->shipping,
			$orderobject->order_process_id,
			$orderobject->delivery_first_name . ' ' . $orderobject->delivery_last_name . ' (' . l($orderobject->uid, 'user/' . $orderobject->uid) . ')',
			$productnode->title . ' (' . l($productnode->nid, 'node/' . $productnode->nid) . ')',
			$orderobject->product_count,
			number_format($orderobject->product_price, 2, ",", ".") . ' € / ' . $productnode->field_packingunit[LANGUAGE_NONE][0]['first'] . ' ' . t($productnode->field_packingunit[LANGUAGE_NONE][0]['second']),
			$orderobject->seller_company,
			$orderstatus,
			date('d.m.Y H:i', $orderobject->created),
			$fcipicker,
			$orderobject->seller_postal_code,
			$fcishipper,
			$orderobject->delivery_postal_code,
			date('d.m.Y', $shippingobject[$skeys[0]]->shipping_range_from) . '<br />' . date('H:i', $shippingobject[$skeys[0]]->shipping_range_from) . '-' . date('H:i', $shippingobject[$skeys[0]]->shipping_range_to),
			$givenfrequency,
			$orderobject->data['comments'],
		);
		$rows[] = $row;
	}
	
	$output = theme('table', array('header' => $header, 'rows' => $rows));
	
	return render($form) . $output . theme('pager');
}

function regiomino_admin_shippingoptions() {

	$header = array(
		'field_metro_name' => array(
			'data' => 'Großraum',
			'type' => 'field',
			'specifier' => array(
				'field' => 'field_metro_name',
				'column' => 'value',
			),
		),
		'field_metro_key' => array(
			'data' => 'Subdomain',
			'type' => 'field',
			'specifier' => array(
				'field' => 'field_metro_key',
				'column' => 'value',
			),
		),
		'field_pickup_areas' => array(
			'data' => 'Abhol-PLZ',
			'type' => 'field',
			'specifier' => array(
				'field' => 'field_pickup_areas',
				'column' => 'value',
			),
		),
		'field_delivery_areas' => array(
			'data' => 'Liefer-PLZ',
			'type' => 'field',
			'specifier' => array(
				'field' => 'field_delivery_areas',
				'column' => 'value',
			),
		),
		'edit' => 'Aktionen',
	);
	
	$query = new EntityFieldQuery;
	$tmp = $query
		->entityCondition('entity_type', 'node')
		->entityCondition('bundle', 'shipper_profile')
		->propertyCondition('status', 1)
		->execute();
	$active_shipper_profiles = entity_load('node', array_keys($tmp['node']));
	
	$active_shipper_profile_ids = array();
	foreach($active_shipper_profiles as $nodeobject) {
		foreach($nodeobject->field_delivery_options[LANGUAGE_NONE] as $deliveryoption) {
			$active_shipper_profile_ids[] = $deliveryoption['value'];
		}
	}

	$query = new EntityFieldQuery;
	$tmp = $query
		->entityCondition('entity_type', 'field_collection_item')
		->entityCondition('bundle', 'field_delivery_options')
		->entityCondition('entity_id', $active_shipper_profile_ids)
		->tableSort($header)
		->pager(50)
		->execute();
	
	$fieldcollections = entity_load('field_collection_item', array_keys($tmp['field_collection_item']));
	
	$queryparams = drupal_get_query_parameters();
	
	$findthis = '';
	if(isset($queryparams['fci']) && !empty($queryparams['fci'])) {
		$findthis = $queryparams['fci'];
	}
	
	$options = array();
	foreach($fieldcollections as $fci => $field_collection_item) {
		
		if($findthis == $fci) {
			$displayfci = $field_collection_item;
		}
	
		$field_pickup_areas = $field_collection_item->field_pickup_areas[LANGUAGE_NONE][0]['value'];
		for($i=1; $i<count($field_collection_item->field_pickup_areas[LANGUAGE_NONE]); $i++) {
			$field_pickup_areas .= ', ' . $field_collection_item->field_pickup_areas[LANGUAGE_NONE][$i]['value'];
		}
		
		$field_delivery_areas = $field_collection_item->field_delivery_areas[LANGUAGE_NONE][0]['value'];
		for($i=1; $i<count($field_collection_item->field_delivery_areas[LANGUAGE_NONE]); $i++) {
			$field_delivery_areas .= ', ' . $field_collection_item->field_delivery_areas[LANGUAGE_NONE][$i]['value'];
		}

		$options[$fci] = array(
				'field_metro_name' => $field_collection_item->field_metro_name[LANGUAGE_NONE][0]['value'],
				'field_metro_key' => $field_collection_item->field_metro_key[LANGUAGE_NONE][0]['value'] . '.regiomino.de',
				'field_pickup_areas' => $field_pickup_areas,
				'field_delivery_areas' => $field_delivery_areas,
				'edit' => l('Karte anzeigen', 'admin/support/shippingoptions', array('query' => array('fci' => $fci))),
		);
	}
	
	$output = theme('table', array('header' => $header, 'rows' => $options));
	
	drupal_add_js(array(
		'SHIPPING_OPTION_METRO_NAME' => $displayfci->field_metro_name[LANGUAGE_NONE][0]['value'],
		'SHIPPING_OPTION_METRO_KEY' => $displayfci->field_metro_key[LANGUAGE_NONE][0]['value'],
	), 'setting');

	drupal_add_js('https://maps.googleapis.com/maps/api/js?sensor=false', 'external');
	drupal_add_js(drupal_get_path('module', 'regiomino_admin') . '/js/shippingmap.js');
	
	return	theme('regiomino_admin_theme_shippingoptions', array(
					'vars' => array(
						'output' => $output,
						'pager' => theme('pager'),
						'fci' => $displayfci,
					),
				));
}

/* function regiomino_admin_offersmissingcommercial() {

	$header = array(
		'nid' => array(
			'data' => 'Art.#',
			'type' => 'property',
			'specifier' => 'nid',
			'sort' => 'desc',
		),
		'title' => array(
			'data' => 'Titel',
			'type' => 'property',
			'specifier' => 'title',
		),
		'uid' => array(
			'data' => 'Verkäufer (Teilnehmernr.)',
			'type' => 'property',
			'specifier' => 'uid',
		),
		'field_packingunit' => array(
			'data' => 'Einheit',
			'type' => 'field',
			'specifier' => array(
				'field' => 'field_packingunit',
				'column' => 'first',
			),
		),
		'field_tu_revenue_private' => 'Netto-Erlös (privat)',
		'field_tu_revenue_commercial' => 'Netto-Erlös (gewerblich)',
		'edit' => 'Aktionen',
	);
	
	$query = new EntityFieldQuery;
	$tmp = $query
		->entityCondition('entity_type', 'node')
		->entityCondition('bundle', 'offer')
		->propertyCondition('status', 1)
		->propertyCondition('soldout', 0)
		->execute();
	$allactiveoffers = entity_load('node', array_keys($tmp['node']));
	$nocommprice = array();
	foreach($allactiveoffers as $nid => $activeoffer) {
		$fcloadarray = array();
		if(isset($activeoffer->field_tradingunits[LANGUAGE_NONE])) {
			foreach($activeoffer->field_tradingunits[LANGUAGE_NONE] as $key => $value) {
				$fcloadarray[] = $value['value'];
			}
		}
		//Load all tradingunits stored in the node
		$fcis_commercial = entity_load('field_collection_item', $fcloadarray);
		
		//Filter out prices that do not belong to the customertype (private or commercial)
		if(isset($fcis_commercial) && !empty($fcis_commercial)) {
			foreach($fcis_commercial as $key=>$value) {
				if(isset($value->field_tu_customertype[LANGUAGE_NONE][0]['value']) && $value->field_tu_customertype[LANGUAGE_NONE][0]['value'] != 'commercial') unset($fcis_commercial[$key]);
			}
		}
		//If prices are left, continue here.
		if(!isset($fcis_commercial) || empty($fcis_commercial)) {
			//Sort DESC by tradingunit size without key preservation
			$nocommprice[] = $nid;
		}
	}

	$query = new EntityFieldQuery;
	$query
		->entityCondition('entity_type', 'node')
		->entityCondition('bundle', array('offer'))
		->propertyCondition('nid', $nocommprice);
	$tmp = $query
		->tableSort($header)
		->pager(50)
		->execute();
	
	$allactiveoffers = entity_load('node', array_keys($tmp['node']));
	
	$options = array();
	foreach($allactiveoffers as $nid => $activeoffer) {

		$fcloadarray = array();
		if(isset($activeoffer->field_tradingunits[LANGUAGE_NONE])) {
			foreach($activeoffer->field_tradingunits[LANGUAGE_NONE] as $key => $value) {
				$fcloadarray[] = $value['value'];
			}
		}
		//Load all tradingunits stored in the node
		$fcis_commercial = $fcis_private = entity_load('field_collection_item', $fcloadarray);
		
		//Filter out prices that do not belong to the customertype (private or commercial)
		if(isset($fcis_commercial) && !empty($fcis_commercial)) {
			foreach($fcis_commercial as $key=>$value) {
				if(isset($value->field_tu_customertype[LANGUAGE_NONE][0]['value']) && $value->field_tu_customertype[LANGUAGE_NONE][0]['value'] != 'commercial') unset($fcis_commercial[$key]);
			}
		}
		//If prices are left, continue here.
		if(isset($fcis_commercial) && !empty($fcis_commercial)) {
			//Sort DESC by tradingunit size without key preservation
			usort($fcis_commercial, "regiomino_offer_sort_tradingunits");
		}
		
		//Filter out prices that do not belong to the customertype (private or commercial)
		if(isset($fcis_private) && !empty($fcis_private)) {
			foreach($fcis_private as $key=>$value) {
				if(isset($value->field_tu_customertype[LANGUAGE_NONE][0]['value']) && $value->field_tu_customertype[LANGUAGE_NONE][0]['value'] != 'private') unset($fcis_private[$key]);
			}
		}
		//If prices are left, continue here. Otherwise NULL is returned
		if(isset($fcis_private) && !empty($fcis_private)) {
			//Sort DESC by tradingunit size without key preservation
			usort($fcis_private, "regiomino_offer_sort_tradingunits");
		}
		
		if(isset($fcis_commercial[0]->field_tu_size[LANGUAGE_NONE][0]['value']) && !empty($fcis_commercial[0]->field_tu_size[LANGUAGE_NONE][0]['value'])) {
			$field_tu_size_commercial_default = $fcis_commercial[0]->field_tu_size[LANGUAGE_NONE][0]['value'];
			$field_tu_revenue_commercial_default = number_format($fcis_commercial[0]->field_tu_revenue[LANGUAGE_NONE][0]['value'], 2, ",", "");
		}
		else {
			$field_tu_size_commercial_default = 1;
			$field_tu_revenue_commercial_default = '-';
		}
		
		if(isset($fcis_private[0]->field_tu_size[LANGUAGE_NONE][0]['value']) && !empty($fcis_private[0]->field_tu_size[LANGUAGE_NONE][0]['value'])) {
			$field_tu_size_private_default = $fcis_private[0]->field_tu_size[LANGUAGE_NONE][0]['value'];
			$field_tu_revenue_private_default = number_format($fcis_private[0]->field_tu_revenue[LANGUAGE_NONE][0]['value'], 2, ",", "");
		}
		else {
			$field_tu_size_private_default = 1;
			$field_tu_revenue_private_default = '-';
		}
		
		$sellerprofile = node_load(user_load($activeoffer->uid)->field_profilereference[LANGUAGE_NONE][0]['target_id']);
	
		$options[$nid] = array(
				'nid' => array('data' => $activeoffer->nid, 'specifier' => 'nid', 'class' => 'nid' . $activeoffer->nid),
				'title' => array('data' => l($activeoffer->title, 'node/' . $activeoffer->nid), 'specifier' => 'title'),
				'uid' => array('data' => l($sellerprofile->title . ' (' . $activeoffer->uid . ')', 'node/' . $sellerprofile->nid), 'specifier' => 'uid'),
				'field_packingunit' => $activeoffer->field_packingunit[LANGUAGE_NONE][0]['first'] . ' ' . t($activeoffer->field_packingunit[LANGUAGE_NONE][0]['second']),
				'field_tu_revenue_private' => $field_tu_revenue_private_default . " € / " . number_format($field_tu_size_private_default) . " Einheit(en)",
				'field_tu_revenue_commercial' => $field_tu_revenue_commercial_default . " € / " . number_format($field_tu_size_commercial_default) . " Einheit(en)",
				'edit' => l('Schattenlogin beim Verkäufer', 'user/' . $user->uid . '/shadow/' . $activeoffer->uid, array('query' => array('destination' => 'admin/offers'))),
		);
	}
	
	$output = theme('table', array('header' => $header, 'rows' => $options));
	
	return $output . theme('pager');
} */

function regiomino_admin_alloffers() {

	global $user;

	$queryparams = drupal_get_query_parameters();

	$form = drupal_get_form('regiomino_admin_alloffers_filterform');

	$header = array(
		'nid' => array(
			'data' => 'Art.#',
			'type' => 'property',
			'specifier' => 'nid',
			'sort' => 'desc',
		),
		'title' => array(
			'data' => 'Titel',
			'type' => 'property',
			'specifier' => 'title',
		),
		'uid' => array(
			'data' => 'Verkäufer (Teilnehmernr.)',
			'type' => 'property',
			'specifier' => 'uid',
		),
		'field_packingunit' => array(
			'data' => 'Einheit',
			'type' => 'field',
			'specifier' => array(
				'field' => 'field_packingunit',
				'column' => 'first',
			),
		),
		'field_tu_revenue_private' => 'Netto-Erlös (privat)',
		'field_tu_revenue_commercial' => 'Netto-Erlös (gewerblich)',
		'edit' => 'Aktionen',
	);
	
	$query = new EntityFieldQuery;
	$query
		->entityCondition('entity_type', 'node')
		->entityCondition('bundle', array('offer'));
		
	if(isset($queryparams['publish_status']) && !empty($queryparams['publish_status'])) {
		$query->propertyCondition('status', $queryparams['publish_status']);
	}
	else {
		$query->propertyCondition('status', 1);
	}
	
	if(isset($queryparams['sale_status']) && !empty($queryparams['sale_status'])) {
		$query->propertyCondition('soldout', $queryparams['sale_status']);
	}
	else {
		$query->propertyCondition('soldout', 0);
	}
	
	$tmp = $query
		->tableSort($header)
		->pager(50)
		->execute();
	
	$allactiveoffers = entity_load('node', array_keys($tmp['node']));
	
	$options = array();
	foreach($allactiveoffers as $nid => $activeoffer) {

		$fcloadarray = array();
		if(isset($activeoffer->field_tradingunits[LANGUAGE_NONE])) {
			foreach($activeoffer->field_tradingunits[LANGUAGE_NONE] as $key => $value) {
				$fcloadarray[] = $value['value'];
			}
		}
		//Load all tradingunits stored in the node
		$fcis_commercial = $fcis_private = entity_load('field_collection_item', $fcloadarray);
		
		//Filter out prices that do not belong to the customertype (private or commercial)
		if(isset($fcis_commercial) && !empty($fcis_commercial)) {
			foreach($fcis_commercial as $key=>$value) {
				if(isset($value->field_tu_customertype[LANGUAGE_NONE][0]['value']) && $value->field_tu_customertype[LANGUAGE_NONE][0]['value'] != 'commercial') unset($fcis_commercial[$key]);
			}
		}
		//If prices are left, continue here.
		if(isset($fcis_commercial) && !empty($fcis_commercial)) {
			//Sort DESC by tradingunit size without key preservation
			usort($fcis_commercial, "regiomino_offer_sort_tradingunits");
		}
		
		//Filter out prices that do not belong to the customertype (private or commercial)
		if(isset($fcis_private) && !empty($fcis_private)) {
			foreach($fcis_private as $key=>$value) {
				if(isset($value->field_tu_customertype[LANGUAGE_NONE][0]['value']) && $value->field_tu_customertype[LANGUAGE_NONE][0]['value'] != 'private') unset($fcis_private[$key]);
			}
		}
		//If prices are left, continue here. Otherwise NULL is returned
		if(isset($fcis_private) && !empty($fcis_private)) {
			//Sort DESC by tradingunit size without key preservation
			usort($fcis_private, "regiomino_offer_sort_tradingunits");
		}
		
		if(isset($fcis_commercial[0]->field_tu_size[LANGUAGE_NONE][0]['value']) && !empty($fcis_commercial[0]->field_tu_size[LANGUAGE_NONE][0]['value'])) {
			$field_tu_size_commercial_default = $fcis_commercial[0]->field_tu_size[LANGUAGE_NONE][0]['value'];
			$field_tu_revenue_commercial_default = number_format($fcis_commercial[0]->field_tu_revenue[LANGUAGE_NONE][0]['value'], 2, ",", "");
		}
		else {
			$field_tu_size_commercial_default = 1;
			$field_tu_revenue_commercial_default = '-';
		}
		
		if(isset($fcis_private[0]->field_tu_size[LANGUAGE_NONE][0]['value']) && !empty($fcis_private[0]->field_tu_size[LANGUAGE_NONE][0]['value'])) {
			$field_tu_size_private_default = $fcis_private[0]->field_tu_size[LANGUAGE_NONE][0]['value'];
			$field_tu_revenue_private_default = number_format($fcis_private[0]->field_tu_revenue[LANGUAGE_NONE][0]['value'], 2, ",", "");
		}
		else {
			$field_tu_size_private_default = 1;
			$field_tu_revenue_private_default = '-';
		}
		
		$sellerprofile = node_load(user_load($activeoffer->uid)->field_profilereference[LANGUAGE_NONE][0]['target_id']);
	
		$options[$nid] = array(
				'nid' => array('data' => $activeoffer->nid, 'specifier' => 'nid', 'class' => 'nid' . $activeoffer->nid),
				'title' => array('data' => l($activeoffer->title, 'node/' . $activeoffer->nid), 'specifier' => 'title'),
				'uid' => array('data' => l($sellerprofile->title . ' (' . $activeoffer->uid . ')', 'node/' . $sellerprofile->nid), 'specifier' => 'uid'),
				'field_packingunit' => $activeoffer->field_packingunit[LANGUAGE_NONE][0]['first'] . ' ' . t($activeoffer->field_packingunit[LANGUAGE_NONE][0]['second']),
				'field_tu_revenue_private' => $field_tu_revenue_private_default . " € / " . number_format($field_tu_size_private_default) . " Einheit(en)",
				'field_tu_revenue_commercial' => $field_tu_revenue_commercial_default . " € / " . number_format($field_tu_size_commercial_default) . " Einheit(en)",
				'edit' => l('Schattenlogin beim Verkäufer', 'user/' . $user->uid . '/shadow/' . $activeoffer->uid, array('query' => array('destination' => 'admin/offers'))),
		);
	}
	
	$output = theme('table', array('header' => $header, 'rows' => $options));
	
	return render($form) . $output . theme('pager');
}

function regiomino_admin_paymenttasks() {
	//Beginning and end of last month
	$lastmonth = mktime(0,0,0,date('n')-1,1,date('Y'));
	//Get all invoice summaries for sellers from the last month with status not equal to canceled, processing, pending or completed (leaves delivered and billed)	
	$query = new EntityFieldQuery;
	$nodeqry = $query
		->entityCondition('entity_type', 'node')
		->entityCondition('bundle', array('invoice_summary_shipper', 'invoice_summary_seller'))
		->propertyCondition('status', 1)
		->fieldCondition('field_month', 'value', $lastmonth)
		//->fieldCondition('field_total', 'value', 0, '>')
		->fieldOrderBy('field_total', 'value', 'DESC')
		->execute();
	$nodeobjects = entity_load('node', array_keys($nodeqry['node']));
	
	$header = array(
		'Aktueller Kontostand',
		'Erlös im letzten Monat',
		'Profil',
		'Bankverbindung',
		'Buchungslink',
	);
	
	foreach($nodeobjects as $nid=>$value) {

		$userobject = user_load($value->uid);
		if($value->type == 'invoice_summary_shipper') {
			$profileid = $userobject->field_profilereference_shipper[LANGUAGE_NONE][0]['target_id'];
		}
		else {
			$profileid = $userobject->field_profilereference[LANGUAGE_NONE][0]['target_id'];
		}
		$profile = node_load($profileid);
		
		$rows[] = array(
			number_format(userpoints_get_current_points($value->uid, 447), 2, ",", ".") . ' €',
			number_format($value->field_total[LANGUAGE_NONE][0]['value'], 2, ",", ".") . ' € (' . l('PDF', 'node/' . $nid) . ')',
			l($profile->title, 'node/' . $profileid),
			$profile->field_bankaccountholder[LANGUAGE_NONE][0]['value'] . ', IBAN: ' . $profile->field_iban[LANGUAGE_NONE][0]['value'] . ', BIC: ' . $profile->field_bic[LANGUAGE_NONE][0]['value'] . ', Institut: ' . $profile->field_institute[LANGUAGE_NONE][0]['value'] . ', Konto: ' . $profile->field_bankaccount[LANGUAGE_NONE][0]['value'] . ', BLZ: ' . $profile->field_bankcode[LANGUAGE_NONE][0]['value'],
			l('Erlös auszahlen', 'admin/config/people/userpoints/add/' . $value->uid . ';447;seller_profile;-' . number_format($value->field_total[LANGUAGE_NONE][0]['value'], 2, ".", "")),
		);
	}
		
	return theme('table', array('header' => $header, 'rows' => $rows));
}

function regiomino_admin_sales() {

	global $user;
	if($user->uid == 1) {
		$current_path = current_path();
		$options = array(
			'query' => array(
				'refresh' => 1,
			),
		);
		$cacheclearurl = url($current_path, $options);
		$queryparams = drupal_get_query_parameters();
		if($queryparams['refresh'] == '1') {
			cache_clear_all('regiominopayment_monthlysales', 'cache', TRUE);
		}
	}

	$time = time();
	$cacheexpire = $time - 86400;
	//Get children of parent item (cache this)
	$cached = cache_get('regiominopayment_monthlysales');
	if($cached && $cached->created > $cacheexpire) {
		$monthlysales = $cached->data;
	}
	else {
		cache_clear_all('regiominopayment_monthlysales', 'cache', TRUE);

		//Get all orders with status not equal to canceled, processing, pending or completed (leaves delivered and billed)	
		$query = new EntityFieldQuery;
		$orderids = $query
			->entityCondition('entity_type', 'regiomino_order')
			->propertyCondition('order_status', 'canceled', '!=')
			->propertyCondition('order_status', 'pending', '!=')
			// ->propertyCondition('order_status', 'processing', '!=')
			// ->propertyCondition('order_status', 'completed', '!=')
			// ->propertyCondition('order_status', 'delivered', '!=')
			->execute();

		$orders = entity_load('regiomino_order', array_keys($orderids['regiomino_order']));

		//Add complete nodeobject to each order instead of just nid
		$allnids = array();
		foreach($orders as $key=>$value) {
			$allnids[] = $value->nid;
		}
		$nodeobjects = entity_load('node', $allnids);
		foreach($orders as $key=>$value) {
			$orders[$key]->nodeobject = $nodeobjects[$value->nid];
		}	

		$monthlysales = array();	
		foreach($orders as $key=>$value) {
			$timekey = strtotime(date('Y-m', $value->created));
			if($timekey >= 1375308000) {
				if($value->created < 1384038000) {
					$discountedprice = regiomino_offer_get_discountedprice($value->nodeobject, FALSE, TRUE, TRUE);
				}
				else {
					$discountedprice = regiomino_offer_get_discountedprice($value->nodeobject);
				}
				$finalprice = $discountedprice * $value->product_count;
				if(isset($monthlysales[$timekey])) {
					$monthlysales[$timekey] += $finalprice;
				}
				else {
					$monthlysales[$timekey] = $finalprice;
				}
			}
		}
		cache_set('regiominopayment_monthlysales', $monthlysales);
	}

	$data1 = array();
	foreach($monthlysales as $key=>$value) {
		$data1[] = array($key*1000, $value);
	}

	drupal_set_title('Umsatz');
	if(isset($cached->created) && !empty($cached->created)) {
		$cachecreated = $cached->created;
	}
	else {
		$cachecreated = time();
	}
	$returnvalue = '
	<p>Berücksichtigt alle bezahlten Bestellungen mit ihrem Endverkaufspreis. <strong>D.h. die Werte können sich in den aktuelleren Monaten noch durch Stornos und zusätzliche Rabattierungen verändern. Je länger die Angebote dauern, desto rückwirkender können Änderungen entstehen.</strong>.</p>
	<p><em>Aufgrund der hohen Datenbankbelastung werden die Berechnungen für mind. 24 Std. zwischengespeichert. Letzte Zwischenspeicherung erfolgte am ' . date('d.m.Y H:i', $cachecreated) . '. Besuchen Sie diese Seite frühestens am ' . date('d.m.Y H:i', $cachecreated + 86400) . ' um neue Werte zu sehen.</em></p>';
	
	if($user->uid == '1') $returnvalue .= '<p><a href="' . $cacheclearurl . '">ADMIN: Diesen Cache manuell löschen</a></p>';

  $d1 = new flotData($data1);
	$style = new flotStyle();
	$style->series->lines->show = TRUE;
	$style->series->lines->fill = TRUE;
	$style->series->lines->lineWidth = 5;
	$style->xaxis->mode = 'time';
	$style->xaxis->timeformat = '%y/%0m';
	$style->xaxis->tickSize = array(1, "month");
	$style->grid->labelMargin= 20;
  $style->grid->axisMargin= 10;
	$style->legend->show = FALSE;
  $variables = array(
    'element' => array(
      'id' => 'flot-example-normal',
      'class' => 'flot-example',
      'style' => "width:500px;height:400px;",
    ),
    'data' => array($d1),
		'options' => $style,
  );

	$returnvalue .= '<div id="statsdataplot">' . theme('flot_graph', $variables) . '</div>';

	$returnvalue .= '
	<div id="statsdatatable">
	<table id="monthlysales">
	<tr>
		<th>Monat</th>
		<th>Umsatz</th>
	</tr>';

	foreach($monthlysales as $key=>$value) {
		$returnvalue .= '
			<tr>
				<td>' . t(date('F', $key)) . ' ' . date('Y', $key) . '</td>
				<td>' . number_format($value, 2, ",", ".") . ' &euro;</td>
			</tr>
		';
	}

	$returnvalue .= '</table></div>';
  return $returnvalue;
}

function regiomino_admin_performance() {
	global $user;
	if($user->uid == 1) {
		$current_path = current_path();
		$options = array(
			'query' => array(
				'refresh' => 1,
			),
		);
		$cacheclearurl = url($current_path, $options);
		$queryparams = drupal_get_query_parameters();
		if($queryparams['refresh'] == '1') cache_clear_all('regiominopayment_performance', 'cache', TRUE);
	}

	$time = time();
	$cacheexpire = $time - 86400;
	//Get children of parent item (cache this)
	$cached = cache_get('regiominopayment_performance');
	if($cached && $cached->created > $cacheexpire) {
		$performance = $cached->data;
	}
	else {
		cache_clear_all('regiominopayment_performance', 'cache', TRUE);
		$performance_all = array();
		if (($handle = fopen("/var/siege.log", "r")) !== FALSE) {
			while (($data = fgetcsv($handle, 1000, ",")) !== FALSE) {
				$timekey = strtotime($data[0]);
				if($timekey >= $time - 604800 && $data[8] == 1) {
					//$performance_all[$timekey]['elaptime'] = $data[2];
					$performance_all[$timekey]['resptime'] = $data[4];
				}
			}
			fclose($handle);
		}
		$performance = array();
		//$medianelap= array();
		$medianresp = array();
		$counter = 0;
		foreach($performance_all as $key=>$value) {
			$counter++;
			if($counter <= 29) {
				//$medianelap[] = $value['elaptime'];
				$medianresp[] = $value['resptime'];
			}
			else {
				//$performance[$key]['elaptime'] = regiomino_admin_median($medianelap);
				$performance[$key]['resptime'] = regiomino_admin_median($medianresp);
				$counter = 0;
				//$medianelap= array();
				$medianresp = array();
			}
		}
		ksort($performance);
		cache_set('regiominopayment_performance', $performance);
	}

	//$data1 = array();
	$data2 = array();
	foreach($performance as $key=>$value) {
		//$data1[] = array($key*1000, $value['elaptime']);
		$data2[] = array($key*1000, $value['resptime']);
	}
	
	drupal_set_title('Performance');
	if(isset($cached->created) && !empty($cached->created)) {
		$cachecreated = $cached->created;
	}
	else {
		$cachecreated = time();
	}
	$returnvalue = '
	<p><em>Aufgrund der hohen Datenbankbelastung werden die Berechnungen für mind. 24 Std. zwischengespeichert. Letzte Zwischenspeicherung erfolgte am ' . date('d.m.Y H:i', $cachecreated) . '. Besuchen Sie diese Seite frühestens am ' . date('d.m.Y H:i', $cachecreated + 86400) . ' um neue Werte zu sehen.</em></p>';
	
	if($user->uid == '1') $returnvalue .= '<p><a href="' . $cacheclearurl . '">ADMIN: Diesen Cache manuell löschen</a></p>';
	
  //$d1 = new flotData($data1);
	$d2 = new flotData($data2);
	$style = new flotStyle();
	$style->series->lines->show = TRUE;
	$style->series->lines->fill = TRUE;
	$style->series->lines->lineWidth = 5;
	$style->xaxis->mode = 'time';
	$style->xaxis->timeformat = '%y/%0m';
	$style->xaxis->tickSize = array(1, "month");
	$style->grid->labelMargin= 20;
  $style->grid->axisMargin= 10;
	$style->legend->show = FALSE;
  $variables = array(
    'element' => array(
      'id' => 'flot-example-normal',
      'class' => 'flot-example',
      'style' => "width:500px;height:400px;",
    ),
    'data' => array($d2),
		'options' => $style,
  );
	
	$returnvalue .= '<div id="statsdataplot">' . theme('flot_graph', $variables) . '</div>';
	
	$returnvalue .= '
	<div id="statsdatatable">
	<table id="performance">
	<tr>
		<th>Uhrzeit</th>
		<th>Response-Time</th>
	</tr>';
	
	foreach($performance as $key=>$value) {
		$returnvalue .= '
			<tr>
				<td>' . date('d.m.Y H:i', $key-(30*60)) . ' - ' . date('H:i', $key) . '</td>
				<td>' . number_format($value['resptime']*1000, 0, ",", "") . 'ms</td>
			</tr>
		';
	}
	
	$returnvalue .= '</table></div>';
  return $returnvalue;
}

function regiomino_admin_transactions() {
	global $user;
	if($user->uid == 1) {
		$current_path = current_path();
		$options = array(
			'query' => array(
				'refresh' => 1,
			),
		);
		$cacheclearurl = url($current_path, $options);
		$queryparams = drupal_get_query_parameters();
		if($queryparams['refresh'] == '1') cache_clear_all('regiominopayment_transactions', 'cache', TRUE);
	}

	$time = time();
	$cacheexpire = $time - 86400;
	//Get children of parent item (cache this)
	$cached = cache_get('regiominopayment_transactions');
	if($cached && $cached->created > $cacheexpire) {
		$transactions = $cached->data;
	}
	else {
		cache_clear_all('regiominopayment_transactions', 'cache', TRUE);
		
		$query = db_query("SELECT * FROM {userpoints_txn} WHERE operation = 'customer' AND tid = 446 AND (description LIKE 'Nachträglich erzielter Rabatt%' OR reference LIKE 'subseq_discount_%') AND points <> 0");
		
		$transactions = array();	
		foreach($query as $record) {
			$timekey = strtotime(date('Y-m', $record->time_stamp));
			if($timekey >= 1375308000) {
				if(isset($transactions[$timekey])) {
					$transactions[$timekey]['amount'] += 1;
					$transactions[$timekey]['sum'] += $record->points;
				}
				else {
					$transactions[$timekey]['amount'] = 1;
					$transactions[$timekey]['sum'] = $record->points;
				}
			}
		}
		
		ksort($transactions);
		cache_set('regiominopayment_transactions', $transactions);
	}

	$data1 = array();
	$data2 = array();
	foreach($transactions as $key=>$value) {
		$data1[] = array($key*1000, $value['amount']);
		$data2[] = array($key*1000, $value['sum']);
	}
	
	drupal_set_title('Nachträglich erzielte Rabatte');
	if(isset($cached->created) && !empty($cached->created)) {
		$cachecreated = $cached->created;
	}
	else {
		$cachecreated = time();
	}
	$returnvalue = '
	<p>Nachträglich erzielte Rabatte auf Regiomino.</p>
	<p><em>Aufgrund der hohen Datenbankbelastung werden die Berechnungen für mind. 24 Std. zwischengespeichert. Letzte Zwischenspeicherung erfolgte am ' . date('d.m.Y H:i', $cachecreated) . '. Besuchen Sie diese Seite frühestens am ' . date('d.m.Y H:i', $cachecreated + 86400) . ' um neue Werte zu sehen.</em></p>';
	
	if($user->uid == '1') $returnvalue .= '<p><a href="' . $cacheclearurl . '">ADMIN: Diesen Cache manuell löschen</a></p>';
	
  $d1 = new flotData($data1);
	$d2 = new flotData($data2);
	$style = new flotStyle();
	$style->series->lines->show = TRUE;
	$style->series->lines->fill = TRUE;
	$style->series->lines->lineWidth = 5;
	$style->xaxis->mode = 'time';
	$style->xaxis->timeformat = '%y/%0m';
	$style->xaxis->tickSize = array(1, "month");
	$style->grid->labelMargin= 20;
  $style->grid->axisMargin= 10;
	$style->legend->show = FALSE;
  $variables = array(
    'element' => array(
      'id' => 'flot-example-normal',
      'class' => 'flot-example',
      'style' => "width:500px;height:400px;",
    ),
    'data' => array($d1, $d2),
		'options' => $style,
  );
	
	$returnvalue .= '<div id="statsdataplot">' . theme('flot_graph', $variables) . '</div>';
	
	$returnvalue .= '
	<div id="statsdatatable">
	<table id="transactions">
	<tr>
		<th>Monat</th>
		<th>Anzahl</th>
		<th>Summe</th>
	</tr>';
	
	foreach($transactions as $key=>$value) {
		$returnvalue .= '
			<tr>
				<td>' . t(date('F', $key)) . ' ' . date('Y', $key) . '</td>
				<td>' . $value['amount'] . '</td>
				<td>' . $value['sum'] . '</td>
			</tr>
		';
	}
	
	$returnvalue .= '</table></div>';
  return $returnvalue;
}

function regiomino_admin_webformsubmissions() {
	global $user;
	if($user->uid == 1) {
		$current_path = current_path();
		$options = array(
			'query' => array(
				'refresh' => 1,
			),
		);
		$cacheclearurl = url($current_path, $options);
		$queryparams = drupal_get_query_parameters();
		if($queryparams['refresh'] == '1') cache_clear_all('regiominopayment_webformsubmissions', 'cache', TRUE);
	}

	$time = time();
	$cacheexpire = $time - 86400;
	//Get children of parent item (cache this)
	$cached = cache_get('regiominopayment_webformsubmissions');
	if($cached && $cached->created > $cacheexpire) {
		$webformsubmissions = $cached->data;
	}
	else {
		cache_clear_all('regiominopayment_webformsubmissions', 'cache', TRUE);
		module_load_include('inc','webform','includes/webform.submissions');
		$submissions = webform_get_submissions(array('nid'=>1188));
		
		$webformsubmissions = array();	
		foreach($submissions as $key=>$value) {
			$timekey = strtotime(date('Y-m', $value->submitted));
			if($timekey >= 1375308000) {
				if(isset($webformsubmissions[$timekey])) {
					$webformsubmissions[$timekey]['support'] += 1;
					if(!isset($webformsubmissions[$timekey]['seller'])) $webformsubmissions[$timekey]['seller'] = 0;
				}
				else {
					$webformsubmissions[$timekey]['support'] = 1;
					if(!isset($webformsubmissions[$timekey]['seller'])) $webformsubmissions[$timekey]['seller'] = 0;
				}
			}
		}
		
		
		$submissions = webform_get_submissions(array('nid'=>6540));
	
		foreach($submissions as $key=>$value) {
			$timekey = strtotime(date('Y-m', $value->submitted));
			if($timekey >= 1375308000) {
				if(isset($webformsubmissions[$timekey])) {
					$webformsubmissions[$timekey]['seller'] += 1;
					if(!isset($webformsubmissions[$timekey]['support'])) $webformsubmissions[$timekey]['support'] = 0;
				}
				else {
					$webformsubmissions[$timekey]['seller'] = 1;
					if(!isset($webformsubmissions[$timekey]['support'])) $webformsubmissions[$timekey]['support'] = 0;
				}
			}
		}
		
		ksort($webformsubmissions);
		cache_set('regiominopayment_webformsubmissions', $webformsubmissions);
	}

	$data1 = array();
	$data2 = array();
	foreach($webformsubmissions as $key=>$value) {
		$data1[] = array($key*1000, $value['support']);
		$data2[] = array($key*1000, $value['seller']);
	}
	
	drupal_set_title('Kontaktanfragen');
	if(isset($cached->created) && !empty($cached->created)) {
		$cachecreated = $cached->created;
	}
	else {
		$cachecreated = time();
	}
	$returnvalue = '
	<p>Kontaktanfragen auf Regiomino.</p>
	<p><em>Aufgrund der hohen Datenbankbelastung werden die Berechnungen für mind. 24 Std. zwischengespeichert. Letzte Zwischenspeicherung erfolgte am ' . date('d.m.Y H:i', $cachecreated) . '. Besuchen Sie diese Seite frühestens am ' . date('d.m.Y H:i', $cachecreated + 86400) . ' um neue Werte zu sehen.</em></p>';
	
	if($user->uid == '1') $returnvalue .= '<p><a href="' . $cacheclearurl . '">ADMIN: Diesen Cache manuell löschen</a></p>';
	
  $d1 = new flotData($data1);
	$d2 = new flotData($data2);
	$style = new flotStyle();
	$style->series->lines->show = TRUE;
	$style->series->lines->fill = TRUE;
	$style->series->lines->lineWidth = 5;
	$style->xaxis->mode = 'time';
	$style->xaxis->timeformat = '%y/%0m';
	$style->xaxis->tickSize = array(1, "month");
	$style->grid->labelMargin= 20;
  $style->grid->axisMargin= 10;
	$style->legend->show = FALSE;
  $variables = array(
    'element' => array(
      'id' => 'flot-example-normal',
      'class' => 'flot-example',
      'style' => "width:500px;height:400px;",
    ),
    'data' => array($d1, $d2),
		'options' => $style,
  );
	
	$returnvalue .= '<div id="statsdataplot">' . theme('flot_graph', $variables) . '</div>';
	
	$returnvalue .= '
	<div id="statsdatatable">
	<table id="webformsubmissions">
	<tr>
		<th>Monat</th>
		<th>Kontaktformular</th>
		<th>Anfrage an Verkäufer</th>
	</tr>';
	
	foreach($webformsubmissions as $key=>$value) {
		$returnvalue .= '
			<tr>
				<td>' . t(date('F', $key)) . ' ' . date('Y', $key) . '</td>
				<td>' . $value['support'] . '</td>
				<td>' . $value['seller'] . '</td>
			</tr>
		';
	}
	
	$returnvalue .= '</table></div>';
  return $returnvalue;
}

function regiomino_admin_registered() {

	global $user;
	if($user->uid == 1) {
		$current_path = current_path();
		$options = array(
			'query' => array(
				'refresh' => 1,
			),
		);
		$cacheclearurl = url($current_path, $options);
		$queryparams = drupal_get_query_parameters();
		if($queryparams['refresh'] == '1') {
			cache_clear_all('regiominopayment_monthlyusers', 'cache', TRUE);
			cache_clear_all('regiominopayment_monthlyapplications', 'cache', TRUE);
		}
	}

	$time = time();
	$cacheexpire = $time - 86400;
	//Get children of parent item (cache this)
	$cached = cache_get('regiominopayment_monthlyusers');
	if($cached && $cached->created > $cacheexpire) {
		$monthlyusers = $cached->data;
	}
	else {
		cache_clear_all('regiominopayment_monthlyusers', 'cache', TRUE);
		
		//Get all orders with status not equal to canceled, processing, pending or completed (leaves delivered and billed)	
		$query = new EntityFieldQuery;
		$userids = $query
			->entityCondition('entity_type', 'user')
			->execute();

		$users = entity_load('user', array_keys($userids['user']));
		unset($users[0]); //Remove anonymous user
		unset($users[1]); //Remove superuser
		
		$monthlyusers = array();	
		foreach($users as $key=>$value) {
			$timekey = strtotime(date('Y-m', $value->created));
			if($timekey >= 1375308000) {
				if(isset($monthlyusers[$timekey])) {
					$monthlyusers[$timekey] += 1;
				}
				else {
					$monthlyusers[$timekey] = 1;
				}
			}
		}
		ksort($monthlyusers);
		cache_set('regiominopayment_monthlyusers', $monthlyusers);
	}

	$data1 = array();
	foreach($monthlyusers as $key=>$value) {
		$data1[] = array($key*1000, $value);
	}
	
	$cached = cache_get('regiominopayment_monthlyapplications');
	if($cached && $cached->created > $cacheexpire) {
		$monthlyprofileapplications = $cached->data;
	}
	else {
		cache_clear_all('regiominopayment_monthlyapplications', 'cache', TRUE);
		
		//Get all orders with status not equal to canceled, processing, pending or completed (leaves delivered and billed)	
		$query = new EntityFieldQuery;
		$profileids = $query
			->entityCondition('entity_type', 'node')
			->entityCondition('bundle', array('seller_profile', 'storage_profile', 'coop_profile', 'shipper_profile'))
			->propertyCondition('status', 1)
			->execute();

		$profiles = entity_load('node', array_keys($profileids['node']));

		$monthlyprofileapplications = array();
		foreach($profiles as $key=>$value) {
			$timekey = strtotime(date('Y-m', $value->created));
			if($timekey >= 1375308000) {
				switch($value->type) {
					case 'seller_profile':
						if(isset($monthlyprofileapplications[$timekey]['seller'])) {
							$monthlyprofileapplications[$timekey]['seller'] += 1;
						}
						else {
							$monthlyprofileapplications[$timekey]['seller'] = 1;
						}
					break;
					case 'shipper_profile':
						if(isset($monthlyprofileapplications[$timekey]['shipper'])) {
							$monthlyprofileapplications[$timekey]['shipper'] += 1;
						}
						else {
							$monthlyprofileapplications[$timekey]['shipper'] = 1;
						}
					break;
					case 'coop_profile':
						if(isset($monthlyprofileapplications[$timekey]['coop'])) {
							$monthlyprofileapplications[$timekey]['coop'] += 1;
						}
						else {
							$monthlyprofileapplications[$timekey]['coop'] = 1;
						}
					break;
					case 'storage_profile':
						if(isset($monthlyprofileapplications[$timekey]['point'])) {
							$monthlyprofileapplications[$timekey]['point'] += 1;
						}
						else {
							$monthlyprofileapplications[$timekey]['point'] = 1;
						}
					break;
				}
			}
		}
		ksort($monthlyprofileapplications);
		cache_set('regiominopayment_monthlyapplications', $monthlyprofileapplications);
	}

	$data2 = array();
	$data3 = array();
	$data4 = array();
	$data5 = array();
	foreach($monthlyprofileapplications as $key=>$value) {
		$data2[] = array($key*1000, $value['seller']);
		$data3[] = array($key*1000, $value['shipper']);
		$data4[] = array($key*1000, $value['coop']);
		$data5[] = array($key*1000, $value['point']);
	}
	
	drupal_set_title('Registrierungen');
	if(isset($cached->created) && !empty($cached->created)) {
		$cachecreated = $cached->created;
	}
	else {
		$cachecreated = time();
	}
	$returnvalue = '
	<p>Registrierungen auf Regiomino.</p>
	<p><em>Aufgrund der hohen Datenbankbelastung werden die Berechnungen für mind. 24 Std. zwischengespeichert. Letzte Zwischenspeicherung erfolgte am ' . date('d.m.Y H:i', $cachecreated) . '. Besuchen Sie diese Seite frühestens am ' . date('d.m.Y H:i', $cachecreated + 86400) . ' um neue Werte zu sehen.</em></p>';
	
	if($user->uid == '1') $returnvalue .= '<p><a href="' . $cacheclearurl . '">ADMIN: Diesen Cache manuell löschen</a></p>';
	
  $d1 = new flotData($data1);
  $d2 = new flotData($data2);
  $d3 = new flotData($data3);
  $d4 = new flotData($data4);
  $d5 = new flotData($data5);
	$style = new flotStyle();
	$style->series->lines->show = TRUE;
	$style->series->lines->fill = TRUE;
	$style->series->lines->lineWidth = 5;
	$style->xaxis->mode = 'time';
	$style->xaxis->timeformat = '%y/%0m';
	$style->xaxis->tickSize = array(1, "month");
	$style->grid->labelMargin= 20;
  $style->grid->axisMargin= 10;
	$style->legend->show = FALSE;
  $variables = array(
    'element' => array(
      'id' => 'flot-example-normal',
      'class' => 'flot-example',
      'style' => "width:500px;height:400px;",
    ),
    'data' => array($d1, $d2, $d3, $d4, $d5),
		'options' => $style,
  );
	
	$returnvalue .= '<div id="statsdataplot">' . theme('flot_graph', $variables) . '</div>';
	
	$returnvalue .= '
	<div id="statsdatatable">
	<table id="monthlyusers">
	<tr>
		<th>Monat</th>
		<th>Registrierungen</th>
		<th>Verkäufer</th>
		<th>Lieferanten</th>
		<th>Betreuer</th>
		<th>RM Points</th>
	</tr>';
	
	foreach($monthlyusers as $key=>$value) {
		$returnvalue .= '
			<tr>
				<td>' . t(date('F', $key)) . ' ' . date('Y', $key) . '</td>
				<td>' . $value . '</td>
				<td>' . $monthlyprofileapplications[$key]['seller'] . '</td>
				<td>' . $monthlyprofileapplications[$key]['shipper'] . '</td>
				<td>' . $monthlyprofileapplications[$key]['coop'] . '</td>
				<td>' . $monthlyprofileapplications[$key]['point'] . '</td>
			</tr>
		';
	}
	
	$returnvalue .= '</table></div>';
  return $returnvalue;
}

function regiomino_admin_invoices() {

	global $user;
	if($user->uid == 1) {
		$current_path = current_path();
		$options = array(
			'query' => array(
				'refresh' => 1,
			),
		);
		$cacheclearurl = url($current_path, $options);
		$queryparams = drupal_get_query_parameters();
		if($queryparams['refresh'] == '1') cache_clear_all('regiominopayment_monthlyinvoices', 'cache', TRUE);
	}

	$time = time();
	$cacheexpire = $time - 86400;
	//Get children of parent item (cache this)
	$cached = cache_get('regiominopayment_monthlyinvoices');
	if($cached && $cached->created > $cacheexpire) {
		$monthlyinvoices = $cached->data;
	}
	else {
		cache_clear_all('regiominopayment_monthlyinvoices', 'cache', TRUE);

		//Get all orders with status not equal to canceled, processing, pending or completed (leaves delivered and billed)	
		$query = new EntityFieldQuery;
		$invoiceqry = $query
			->entityCondition('entity_type', 'node')
			->entityCondition('bundle', array('invoice_sale', 'invoice_commission', 'invoice_txnfee', 'invoice_shipping', 'invoice_shipping_sale', 'invoice_summary_seller', 'invoice_summary_shipper', 'invoice_summary_regiomino'))
			->propertyCondition('created', 1375308000, '>=')
			->execute();

		$invoicedocs = entity_load('node', array_keys($invoiceqry['node']));

		$monthlyinvoices = array();	
		foreach($invoicedocs as $key=>$value) {
			$timekey = strtotime(date('Y-m', $value->created));
			if($value->type == 'invoice_summary_seller' || $value->type == 'invoice_summary_shipper' || $value->type == 'invoice_summary_regiomino') {
				$timekey = strtotime(date('Y-m', $value->field_month[LANGUAGE_NONE][0]['value']));
			}
			if($timekey >= 1375308000) {
				if(isset($monthlyinvoices[$value->type][$timekey])) {
					$monthlyinvoices[$value->type][$timekey] += $value->field_total[LANGUAGE_NONE][0]['value'];
				}
				else {
					$monthlyinvoices[$value->type][$timekey] = $value->field_total[LANGUAGE_NONE][0]['value'];
				}
			}
		}

		cache_set('regiominopayment_monthlyinvoices', $monthlyinvoices);
	}

	$data1 = array();
	foreach($monthlyinvoices['invoice_sale'] as $key=>$value) {
		$data1[] = array($key*1000, $value);
	}
	$data2 = array();
	foreach($monthlyinvoices['invoice_shipping'] as $key=>$value) {
		$data2[] = array($key*1000, $value);
	}
	$data3 = array();
	foreach($monthlyinvoices['invoice_commission'] as $key=>$value) {
		$data3[] = array($key*1000, $value);
	}
	$data4 = array();
	foreach($monthlyinvoices['invoice_txnfee'] as $key=>$value) {
		$data4[] = array($key*1000, $value);
	}
	$data5 = array();
	foreach($monthlyinvoices['invoice_shipping_sale'] as $key=>$value) {
		$data5[] = array($key*1000, $value);
	}
	$data6 = array();
	foreach($monthlyinvoices['invoice_summary_seller'] as $key=>$value) {
		$data6[] = array($key*1000, $value);
	}
	$data7 = array();
	foreach($monthlyinvoices['invoice_summary_shipper'] as $key=>$value) {
		$data7[] = array($key*1000, $value);
	}
	$data8 = array();
	foreach($monthlyinvoices['invoice_summary_regiomino'] as $key=>$value) {
		$data8[] = array($key*1000, $value);
	}

	drupal_set_title('Rechnungen');
	if(isset($cached->created) && !empty($cached->created)) {
		$cachecreated = $cached->created;
	}
	else {
		$cachecreated = time();
	}
	$returnvalue = '
	<p>Berücksichtigt alle gestellten Rechnungen mit ihrer Gesamtsumme. <strong>D.h. die Werte können sich in den aktuelleren Monaten noch durch Stornos und zusätzliche Rabattierungen verändern. Je länger die Angebote dauern, desto rückwirkender können Änderungen entstehen.</strong>.</p>
	<p><em>Aufgrund der hohen Datenbankbelastung werden die Berechnungen für mind. 24 Std. zwischengespeichert. Letzte Zwischenspeicherung erfolgte am ' . date('d.m.Y H:i', $cachecreated) . '. Besuchen Sie diese Seite frühestens am ' . date('d.m.Y H:i', $cachecreated + 86400) . ' um neue Werte zu sehen.</em></p>';
	
	if($user->uid == '1') $returnvalue .= '<p><a href="' . $cacheclearurl . '">ADMIN: Diesen Cache manuell löschen</a></p>';

  $d1 = new flotData($data1);
  $d2 = new flotData($data2);
  $d3 = new flotData($data3);
  $d4 = new flotData($data4);
  $d5 = new flotData($data5);
  $d6 = new flotData($data6);
  $d7 = new flotData($data7);
  $d8 = new flotData($data8);
	$style = new flotStyle();
	$style->series->lines->show = TRUE;
	$style->series->lines->fill = TRUE;
	$style->series->lines->lineWidth = 5;
	$style->xaxis->mode = 'time';
	$style->xaxis->timeformat = '%y/%0m';
	$style->xaxis->tickSize = array(1, "month");
	$style->grid->labelMargin= 20;
  $style->grid->axisMargin= 10;
	$style->legend->show = FALSE;
  $variables = array(
    'element' => array(
      'id' => 'flot-example-normal',
      'class' => 'flot-example',
      'style' => "width:500px;height:400px;",
    ),
    'data' => array($d1, $d2, $d3),
		'options' => $style,
  );

	$returnvalue .= '<div id="statsdataplot">' . theme('flot_graph', $variables) . '</div>';

	$returnvalue .= '
	<div id="statsdatatable">
	<table id="monthlyinvoices">
	<tr>
		<th>Monat</th>
		<th>Rechnungen für Verkäufe</th>
		<th>Rechnungen für Versand</th>
		<th>Rechnungen für Versand an Kunde</th>
		<th>Rechnungen für Transaktionsgebühr</th>
		<th>Rechnungen für Regiomino Provision</th>
		<th>Rechnungsübersicht Verkäufer</th>
		<th>Rechnungsübersicht Lieferant</th>
		<th>Rechnungsübersicht Regiomino</th>
	</tr>';

	foreach($monthlyinvoices['invoice_sale'] as $key=>$value) {
		$returnvalue .= '
			<tr>
				<td>' . t(date('F', $key)) . ' ' . date('Y', $key) . '</td>
				<td>' . number_format($value, 2, ",", ".") . ' &euro;</td>
				<td>' . number_format($monthlyinvoices['invoice_shipping'][$key], 2, ",", ".") . ' &euro;</td>
				<td>' . number_format($monthlyinvoices['invoice_shipping_sale'][$key], 2, ",", ".") . ' &euro;</td>
				<td>' . number_format($monthlyinvoices['invoice_txnfee'][$key], 2, ",", ".") . ' &euro;</td>
				<td>' . number_format($monthlyinvoices['invoice_commission'][$key], 2, ",", ".") . ' &euro;</td>
				<td>' . number_format($monthlyinvoices['invoice_summary_seller'][$key], 2, ",", ".") . ' &euro;</td>
				<td>' . number_format($monthlyinvoices['invoice_summary_shipper'][$key], 2, ",", ".") . ' &euro;</td>
				<td>' . number_format($monthlyinvoices['invoice_summary_regiomino'][$key], 2, ",", ".") . ' &euro;</td>
			</tr>
		';
	}

	$returnvalue .= '</table></div>';
  return $returnvalue;
}

function regiomino_admin_soldproducts() {

	$time = time();
	$cacheexpire = $time - 86400;
	
	global $user;
	if($user->uid == 1) {
		$current_path = current_path();
		$options = array(
			'query' => array(
				'refresh' => 1,
			),
		);
		$cacheclearurl = url($current_path, $options);
		$queryparams = drupal_get_query_parameters();
		if($queryparams['refresh'] == '1') {
			$cacheexpire = $time;
		}
	}
	
	
	$cached = cache_get('regiominopayment_monthlysales');
	if($cached && $cached->created > $cacheexpire) {
		$monthlysales = $cached->data;
	}
	else {
		cache_clear_all('regiominopayment_monthlysales', 'cache', TRUE);

		//Get all orders with status not equal to canceled, processing, pending or completed (leaves delivered and billed)	
		$query = new EntityFieldQuery;
		$orderids = $query
			->entityCondition('entity_type', 'regiomino_order')
			->propertyCondition('order_status', 'canceled', '!=')
			->propertyCondition('order_status', 'pending', '!=')
			// ->propertyCondition('order_status', 'processing', '!=')
			// ->propertyCondition('order_status', 'completed', '!=')
			// ->propertyCondition('order_status', 'delivered', '!=')
			->execute();

		$orders = entity_load('regiomino_order', array_keys($orderids['regiomino_order']));

		//Add complete nodeobject to each order instead of just nid
		$allnids = array();
		foreach($orders as $key=>$value) {
			$allnids[] = $value->nid;
		}
		$nodeobjects = entity_load('node', $allnids);
		foreach($orders as $key=>$value) {
			$orders[$key]->nodeobject = $nodeobjects[$value->nid];
		}	

		$monthlysales = array();	
		foreach($orders as $key=>$value) {
			$timekey = strtotime(date('Y-m', $value->created));
			if($timekey >= 1375308000) {
				if($value->created < 1384038000) {
					$discountedprice = regiomino_offer_get_discountedprice($value->nodeobject, FALSE, TRUE, TRUE);
				}
				else {
					$discountedprice = regiomino_offer_get_discountedprice($value->nodeobject);
				}
				$finalprice = $discountedprice * $value->product_count;
				if(isset($monthlysales[$timekey])) {
					$monthlysales[$timekey] += $finalprice;
				}
				else {
					$monthlysales[$timekey] = $finalprice;
				}
			}
		}

		cache_set('regiominopayment_monthlysales', $monthlysales);
	}

	$data0 = array();
	foreach($monthlysales as $key=>$value) {
		$data0[] = array($key*1000, $value);
	}
	

	//Get children of parent item (cache this)
	$cached = cache_get('regiominopayment_monthlysoldproducts');
	if($cached && $cached->created > $cacheexpire) {
		$monthlysoldproducts = $cached->data;
	}
	else {
		cache_clear_all('regiominopayment_monthlysoldproducts', 'cache', TRUE);
		
		//Get all orders with status not equal to canceled, processing, pending or completed (leaves delivered and billed)	
		$query = new EntityFieldQuery;
		$orderids = $query
			->entityCondition('entity_type', 'regiomino_order')
			->propertyCondition('order_status', 'canceled', '!=')
			->propertyCondition('order_status', 'pending', '!=')
			// ->propertyCondition('order_status', 'processing', '!=')
			// ->propertyCondition('order_status', 'completed', '!=')
			// ->propertyCondition('order_status', 'delivered', '!=')
			->execute();

		$orders = entity_load('regiomino_order', array_keys($orderids['regiomino_order']));

		$monthlysoldproducts = array();	
		foreach($orders as $key=>$value) {
			$timekey = strtotime(date('Y-m', $value->created));
			if($timekey >= 1375308000) {
				if(isset($monthlysoldproducts[$timekey])) {
					$monthlysoldproducts[$timekey] += 1;
				}
				else {
					$monthlysoldproducts[$timekey] = 1;
				}
			}
		}

		cache_set('regiominopayment_monthlysoldproducts', $monthlysoldproducts);
	}	
	
	$data1 = array();
	foreach($monthlysoldproducts as $key=>$value) {
		$data1[] = array($key*1000, $value);
	}
	
	$cached = cache_get('regiominopayment_monthlysoldorders');
	if($cached && $cached->created > $cacheexpire) {
		$monthlysoldorders = $cached->data;
	}
	else {
		cache_clear_all('regiominopayment_monthlysoldorders', 'cache', TRUE);
		
		//Get all orders with status not equal to canceled, processing, pending or completed (leaves delivered and billed)	
		$query = new EntityFieldQuery;
		$orderids = $query
			->entityCondition('entity_type', 'regiomino_order')
			->propertyCondition('order_status', 'canceled', '!=')
			->propertyCondition('order_status', 'pending', '!=')
			// ->propertyCondition('order_status', 'processing', '!=')
			// ->propertyCondition('order_status', 'completed', '!=')
			// ->propertyCondition('order_status', 'delivered', '!=')
			->execute();

		$orders = entity_load('regiomino_order', array_keys($orderids['regiomino_order']));

		$monthlysoldorders_tmp = array();	
		foreach($orders as $key=>$value) {
			$timekey = strtotime(date('Y-m', $value->created));
			if($timekey >= 1375308000) {
				if(isset($monthlysoldorders_tmp[$timekey][$value->shipping])) {
					$monthlysoldorders_tmp[$timekey][$value->shipping] += 1;
				}
				else {
					$monthlysoldorders_tmp[$timekey][$value->shipping] = 1;
				}
			}
		}
		
		$monthlysoldorders = array();
		foreach($monthlysoldorders_tmp as $key=>$value) {
			foreach($value as $k => $v) {
				if(isset($monthlysoldorders[$key])) {
					$monthlysoldorders[$key] += 1;
				}
				else {
					$monthlysoldorders[$key] = 1;
				}
			}
		}

		cache_set('regiominopayment_monthlysoldorders', $monthlysoldorders);
	}	
	
	$data2 = array();
	foreach($monthlysoldorders as $key=>$value) {
		$data2[] = array($key*1000, $value);
	}
	
	$cached = cache_get('regiominopayment_monthlysoldorderprocesses');
	if($cached && $cached->created > $cacheexpire) {
		$monthlysoldorders2 = $cached->data;
	}
	else {
		cache_clear_all('regiominopayment_monthlysoldorderprocesses', 'cache', TRUE);
		
		//Get all orders with status not equal to canceled, processing, pending or completed (leaves delivered and billed)	
		$query = new EntityFieldQuery;
		$orderids = $query
			->entityCondition('entity_type', 'regiomino_order')
			->propertyCondition('order_status', 'canceled', '!=')
			->propertyCondition('order_status', 'pending', '!=')
			// ->propertyCondition('order_status', 'processing', '!=')
			// ->propertyCondition('order_status', 'completed', '!=')
			// ->propertyCondition('order_status', 'delivered', '!=')
			->execute();

		$orders = entity_load('regiomino_order', array_keys($orderids['regiomino_order']));

		$monthlysoldorders_tmp = array();	
		foreach($orders as $key=>$value) {
			$timekey = strtotime(date('Y-m', $value->created));
			if($timekey >= 1375308000) {
				if(isset($monthlysoldorders_tmp[$timekey][$value->order_process_id])) {
					$monthlysoldorders_tmp[$timekey][$value->order_process_id] += 1;
				}
				else {
					$monthlysoldorders_tmp[$timekey][$value->order_process_id] = 1;
				}
			}
		}
		
		$monthlysoldorders2 = array();
		foreach($monthlysoldorders_tmp as $key=>$value) {
			foreach($value as $k => $v) {
				if(isset($monthlysoldorders2[$key])) {
					$monthlysoldorders2[$key] += 1;
				}
				else {
					$monthlysoldorders2[$key] = 1;
				}
			}
		}

		cache_set('regiominopayment_monthlysoldorderprocesses', $monthlysoldorders2);
	}	
	
	$data3 = array();
	foreach($monthlysoldorders2 as $key=>$value) {
		$data3[] = array($key*1000, $value);
	}
	
	$cached = cache_get('regiominopayment_monthlyorderers');
	if($cached && $cached->created > $cacheexpire) {
		$monthlyorderers = $cached->data;
	}
	else {
		cache_clear_all('regiominopayment_monthlyorderers', 'cache', TRUE);
		
		//Get all orders with status not equal to canceled, processing, pending or completed (leaves delivered and billed)	
		$query = new EntityFieldQuery;
		$orderids = $query
			->entityCondition('entity_type', 'regiomino_order')
			->propertyCondition('order_status', 'canceled', '!=')
			->propertyCondition('order_status', 'pending', '!=')
			// ->propertyCondition('order_status', 'processing', '!=')
			// ->propertyCondition('order_status', 'completed', '!=')
			// ->propertyCondition('order_status', 'delivered', '!=')
			->execute();

		$orders = entity_load('regiomino_order', array_keys($orderids['regiomino_order']));

		$monthlyorderers_tmp = array();	
		foreach($orders as $key=>$value) {
			$timekey = strtotime(date('Y-m', $value->created));
			if($timekey >= 1375308000) {
				if(isset($monthlyorderers_tmp[$timekey][$value->uid])) {
					$monthlyorderers_tmp[$timekey][$value->uid] += 1;
				}
				else {
					$monthlyorderers_tmp[$timekey][$value->uid] = 1;
				}
			}
		}
		
		$monthlyorderers = array();
		foreach($monthlyorderers_tmp as $key=>$value) {
			foreach($value as $k => $v) {
				if(isset($monthlyorderers[$key])) {
					$monthlyorderers[$key] += 1;
				}
				else {
					$monthlyorderers[$key] = 1;
				}
			}
		}

		cache_set('regiominopayment_monthlyorderers', $monthlyorderers);
	}	
	
	$data4 = array();
	foreach($monthlyorderers as $key=>$value) {
		$data4[] = array($key*1000, $value);
	}
	
	drupal_set_title('Verkaufszahlen');
	if(isset($cached->created) && !empty($cached->created)) {
		$cachecreated = $cached->created;
	}
	else {
		$cachecreated = time();
	}
	$returnvalue = '
	<p><em>Aufgrund der hohen Datenbankbelastung werden die Berechnungen für mind. 24 Std. zwischengespeichert. Letzte Zwischenspeicherung erfolgte am ' . date('d.m.Y H:i', $cachecreated) . '. Besuchen Sie diese Seite frühestens am ' . date('d.m.Y H:i', $cachecreated + 86400) . ' um neue Werte zu sehen.</em></p>';
	
	if($user->uid == '1') $returnvalue .= '<p><a href="' . $cacheclearurl . '">ADMIN: Diesen Cache manuell löschen</a></p>';
	
  $d1 = new flotData($data1);
	$d2 = new flotData($data2);
	$d3 = new flotData($data3);
	$d4 = new flotData($data4);
	$style = new flotStyle();
	$style->series->lines->show = TRUE;
	$style->series->lines->fill = TRUE;
	$style->series->lines->lineWidth = 5;
	$style->xaxis->mode = 'time';
	$style->xaxis->timeformat = '%y/%0m';
	$style->xaxis->tickSize = array(1, "month");
	$style->grid->labelMargin= 20;
  $style->grid->axisMargin= 10;
	$style->legend->show = FALSE;
  $variables = array(
    'element' => array(
      'id' => 'flot-example-normal',
      'class' => 'flot-example',
      'style' => "width:500px;height:400px;",
    ),
    'data' => array($d1, $d2, $d3, $d4),
		'options' => $style,
  );
	
	$returnvalue .= '<div id="statsdataplot">' . theme('flot_graph', $variables) . '</div>';
	
	$returnvalue .= '
	<div id="statsdatatable">
	<table id="monthlysoldproducts">
	<tr>
		<th>Monat</th>
		<th>Prodvkf</th>
		<th>Bstlg</th>
		<th>Bstlvrg</th>
		<th>Besteller</th>
		<th>Prodvkf/Bstlg</th>
		<th>Prodvkf/Bstlvrg</th>
		<th>Bstlg/Bstlvrg</th>
		<th>Bstlvrg/Besteller</th>
		<th>Bstlvrgwert</th>
		<th>Ausgabe/Person</th>
	</tr>';
	
	foreach($monthlysoldproducts as $key=>$value) {
		$returnvalue .= '
			<tr>
				<td>' . date('m/Y', $key) . '</td>
				<td>' . $monthlysoldproducts[$key] . '</td>
				<td>' . $monthlysoldorders[$key] . '</td>
				<td>' . $monthlysoldorders2[$key] . '</td>
				<td>' . $monthlyorderers[$key] . '</td>
				<td>' . round($monthlysoldproducts[$key]/$monthlysoldorders[$key], 1) . '</td>
				<td>' . round($monthlysoldproducts[$key]/$monthlysoldorders2[$key], 1) . '</td>
				<td>' . round($monthlysoldorders[$key]/$monthlysoldorders2[$key], 1) . '</td>
				<td>' . round($monthlysoldorders2[$key]/$monthlyorderers[$key], 1) . '</td>
				<td>' . number_format($monthlysales[$key]/$monthlysoldorders2[$key], 2, ",", ".") . ' &euro;</td>
				<td>' . number_format($monthlysales[$key]/$monthlyorderers[$key], 2, ",", ".") . ' &euro;</td>
			</tr>
		';
	}
	
	$returnvalue .= '</table></div>	<p style="clear: left; padding-top: 50px;">
	<strong>Produktverkauf (Prodvkf)</strong><br />Ein Produktverkauf ist der Verkauf eines einzelnen Produktes, unabhängig davon, wie groß der Warenkorb war oder wie viele Verkäufer der Warenkorb umfasst hat. Wird ein Produkt mit Anzahl 2 oder mehr verkauft, so zählt dies trotzdem nur als 1 Produktverkauf.<br /><br />
	<strong>Bestellung (Bstlg)</strong><br />Eine Bestellung umfasst die Produktverkäufe bei <em>einem</em> bestimmten Verkäufer.<br /><br />
	<strong>Bestellvorgang (Bstlvrg)</strong><br />Ein Bestellvorgang ist identisch mit dem Kauf eines Warenkorbs. Der Bestellvorgang umfasst daher in der Regel mehrere Produktverkäufe und mehrere Bestellungen. Er kann aber auch identisch zu einer Bestellung oder zu einem Produktverkauf sein, nämlich dann, wenn der Warenkorb nur Produkte von einem Verkäufer umfasst hat, oder wenn der Warenkorb nur ein einziges Produkt enthalten hat.<br /><br />
	<strong>Besteller</strong><br />Zeigt, wie viele unterschiedliche Personen eingekauft haben.<br /><br />
	<strong>Produktverkäufe pro Bestellung (Prodvkf/Bstlg)</strong><br />Zeigt, wie viele Produkte eine Bestellung (also Einkauf bei einem einzelnen Verkäufer) im Durchschnitt enthalten hat.<br /><br />
	<strong>Produktverkäufe pro Bestellvorgang (Prodvkf/Bstlvrg)</strong><br />Zeigt, wie viele Produkte ein Bestellvorgang (also ein Warenkorb) im Durchschnitt enthalten hat.<br /><br />
	<strong>Bestellungen pro Bestellvorgang (Bstlg/Bstlvrg)</strong><br />Zeigt, wie viele Verkäufer ein Bestellvorgang (also ein Warenkorb) im Durchschnitt umfasst hat.<br /><br />
	<strong>Bestellvorgänge pro Besteller (Bstlvrg/Besteller)</strong><br />Zeigt, wie oft ein Besteller im Schnitt bei Regiomino eingekauft hat.<br /><br />
	<strong>Wert eines Bestellvorgangs (Bstlvrgwert)</strong><br />Durchschnittlicher Wert eines Warenkorbs<br /><br />
	<strong>Ausgaben pro Person (Ausgabe/Person)</strong><br />Zeigt, wieviel eine Person durchschnittlich bei Regiomino ausgegeben hat
	</p>';
	
  return $returnvalue;
}