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
function regiomino_productranking_admin_settings() {	
	
	$form['search_values'] = array(
    '#type' => 'fieldset',
    '#title' => t('Regiomino search values'),
		'#collapsible' => TRUE,
		'#collapsed' => TRUE,
  );
	
	$form['search_values']['search_autocomplete_url'] = array(
		'#type' => 'textfield',
		'#title' => t('Autocomplete URL'),
		'#default_value' => variable_get('search_autocomplete_url', 'http://188.40.37.206'),
	);
	$form['search_values']['search_autocomplete_port'] = array(
		'#type' => 'textfield',
		'#title' => t('Autocomplete Port'),
		'#default_value' => variable_get('search_autocomplete_port', '9692'),
	);
	$form['search_values']['search_autocomplete_path'] = array(
		'#type' => 'textfield',
		'#title' => t('Autocomplete Path'),
		'#default_value' => variable_get('search_autocomplete_path', '/json/autocomplete.ov49fxokylmbbrbxmcozlmapga3pct'),
	);
	
	$form['search_values']['search_index_url'] = array(
		'#type' => 'textfield',
		'#title' => t('Index URL'),
		'#default_value' => variable_get('search_index_url', 'http://188.40.37.206'),
	);
	$form['search_values']['search_index_port'] = array(
		'#type' => 'textfield',
		'#title' => t('Index Port'),
		'#default_value' => variable_get('search_index_port', '9690'),
	);
	$form['search_values']['search_index_path'] = array(
		'#type' => 'textfield',
		'#title' => t('Index Path'),
		'#default_value' => variable_get('search_index_path', '/json/index.ov49fxokylmbbrbxmcozlmapga3pct'),
	);
	
	$form['search_values']['search_search_url'] = array(
		'#type' => 'textfield',
		'#title' => t('Search URL'),
		'#default_value' => variable_get('search_search_url', 'http://188.40.37.206'),
	);
	$form['search_values']['search_search_port'] = array(
		'#type' => 'textfield',
		'#title' => t('Search Port'),
		'#default_value' => variable_get('search_search_port', '9690'),
	);
	$form['search_values']['search_search_path'] = array(
		'#type' => 'textfield',
		'#title' => t('Search Path'),
		'#default_value' => variable_get('search_search_path', '/json/search.ov49fxokylmbbrbxmcozlmapga3pct'),
	);
	
	//Process form with system_settings_form
  return system_settings_form($form);
}

function regiomino_productranking_index($form, &$form_state) {
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Refresh index'),
	);
	return $form;
}

function regiomino_productranking_index_submit($form, &$form_state) {
	$full_index_url = variable_get('search_index_url', 'http://188.40.37.206');
	$full_index_url .= ':';
	$full_index_url .= variable_get('search_index_port', '9690');
	$full_index_url .= variable_get('search_index_path', '/json/index.ov49fxokylmbbrbxmcozlmapga3pct');
	
	$query = new EntityFieldQuery();
	$tmp = $query
		->entityCondition('entity_type', 'node')
		->entityCondition('bundle', array('offer'))
		->propertyCondition('status', 1)
		->propertyCondition('soldout', 0)
		->execute();
	$nodeobjects = entity_load('node', array_keys($tmp['node']));
	$size = count($nodeobjects);
	$fields = array();
	if(isset($nodeobjects)) {
		foreach($nodeobjects as $nid => $values) {
			$allcategories = array();
			if(isset($values->field_category[LANGUAGE_NONE])) {
				foreach($values->field_category[LANGUAGE_NONE] as $catval) {
					$allcategories[] = $catval['tid'];
				}
			}
			$sellerprofile = node_load(user_load($values->uid)->field_profilereference[LANGUAGE_NONE][0]['target_id']);
			$sellerprofile_nid = $sellerprofile->nid;
			$sellerprofile_title = $sellerprofile->title;
			$sellerprofile_locality = $sellerprofile->field_address[LANGUAGE_NONE][0]['locality'];
			$labelarray = array();
			if(isset($values->field_labels[LANGUAGE_NONE])) {
				foreach($values->field_labels[LANGUAGE_NONE] as $value) {
					$labelnode = node_load($value['target_id']);
					$labelarray[] = array(
						'target_id' => $value['target_id'],
						'image_uri' => $labelnode->field_image[LANGUAGE_NONE][0]['uri'],
						'title' => $labelnode->title,
					);
				}
			}
			$fields[] = array(
				'id' => $nid,
				'fields' => array(
					'title' => array($values->title),
					'body' => array($values->body[LANGUAGE_NONE][0]['value']),
					'created' => array($values->created),
					'soldout' => array($values->soldout),
					'status' => array($values->status),
					'image' => array($values->field_image[LANGUAGE_NONE][0]['uri']),
					'pickupdelay' => array($values->field_pickupdelay[LANGUAGE_NONE][0]['value']),
					'duration' => array($values->field_duration[LANGUAGE_NONE][0]['value']),
					'origin_locality' => array($values->field_origin[LANGUAGE_NONE][0]['locality']),
					'origin_country' => array($values->field_origin[LANGUAGE_NONE][0]['country']),
					'origin_lat' => array($values->field_origin_geoloc[LANGUAGE_NONE][0]['lat']),
					'origin_lon' => array($values->field_origin_geoloc[LANGUAGE_NONE][0]['lon']),
					'packingunit_first' => array($values->field_packingunit[LANGUAGE_NONE][0]['first']),
					'packingunit_second' => array($values->field_packingunit[LANGUAGE_NONE][0]['second']),
					'sellerprofile_nid' => array($sellerprofile_nid),
					'sellerprofile_title' => array($sellerprofile_title),
					'sellerprofile_locality' => array($sellerprofile_locality),
					'labels' => array(json_encode($labelarray)),
					'_kategorie' => $allcategories,
					'_author' => array($values->uid),
					'_type' => array($values->type),
				),
			);
		}
	}
	$data = array(
			'method' => 'index',
			'param0' => json_encode($fields),
	);
	$full_url = url($full_index_url, array('query' => $data));
	$result = drupal_http_request($full_url);
	//Ergebnis-Daten finden sich jetzt in $result->data
	watchdog('searchindex', '@amount nodes have been indexed: @result', array('@amount' => $size, '@result' => print_r($result, TRUE)));
	if($result->code == 200) {
		drupal_set_message(t('@amount nodes have been indexed', array('@amount' => $size)), 'status');
	}
	else {
		drupal_set_message(t('An error occurred. Please check the log for type searchindex'), 'error');
	}
}

function regiomino_productranking_delete($form, &$form_state) {
	$form['nid'] = array(
		'#type' => 'textfield',
		'#title' => 'Node ID',
		'#size' => 3,
		'#description' => t('If left blank the whole index will be deleted!!!'),
	);
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Delete index'),
	);
	return $form;
}

function regiomino_productranking_delete_validate($form, &$form_state) {
	if(!empty($form_state['values']['nid']) && !is_numeric($form_state['values']['nid'])) {
		form_set_error('nid', t('Please enter a valid number.'));
	}
}

function regiomino_productranking_delete_submit($form, &$form_state) {

	$nid = $form_state['values']['nid'];
	$full_index_url = variable_get('search_index_url', 'http://188.40.37.206');
	$full_index_url .= ':';
	$full_index_url .= variable_get('search_index_port', '9690');
	$full_index_url .= variable_get('search_index_path', '/json/index.ov49fxokylmbbrbxmcozlmapga3pct');
	if(!empty($nid)) {
		$data = array(
			'method' => 'delete',
			'param0' => json_encode($nid),
		);
		$result = drupal_http_request(url($full_index_url, array('query' => $data)));
		watchdog('searchindex', 'Node @nid was deleted from the index: @result', array('@nid' => $nid, '@result' => print_r($result, TRUE)));
		if($result->code = 200) {
			drupal_set_message(t('Node @nid was deleted from index', array('@nid' => $nid)), 'status');
		}
		else {
			drupal_set_message(t('An error occurred. Please check the log for type searchindex'), 'error');
		}
	}
	else {
		$query = new EntityFieldQuery();
		$tmp = $query
			->entityCondition('entity_type', 'node')
			->entityCondition('bundle', array('offer'))
			->execute();
		$allnids = array_keys($tmp['node']);
		$data = array(
			'method' => 'delete',
			'param0' => json_encode($allnids),
		);
		$result = drupal_http_request(url($full_index_url, array('query' => $data)));
		watchdog('searchindex', 'All nodes were deleted from the index');
		drupal_set_message(t('All nodes were deleted from index'), 'status');
	}	
}