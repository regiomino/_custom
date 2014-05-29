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
function regiomino_geolocation_admin_settings() {
	$test = regiomino_shipping_get_regional_deliveryoptions('91052', 1);

	//Submit button
	$form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Flush and refill database'),
	);
	//Process form with system_settings_form
  return $form;
}

function regiomino_geolocation_admin_settings_submit(&$form, &$form_state) {

	
	
	//Perform flush and refill here
	regiomino_geolocation_refill_db();
}

function regiomino_geolocation_refill_db() {

	//Retrieve current list of shipper zip codes
/* 	$query = new EntityFieldQuery;
	$tmp = $query
		->entityCondition('entity_type', 'field_collection_item')
		->entityCondition('bundle', 'field_delivery_options')
		->execute();
	
	$field_collections = entity_load('field_collection_item', array_keys($tmp['field_collection_item'])); */
	
	
	$query = new EntityFieldQuery;
	$tmp = $query
		->entityCondition('entity_type', 'field_collection_item')
		->entityCondition('bundle', 'field_delivery_options')
		->execute();
	$shipperprofileid = array();
	foreach($tmp['field_collection_item'] as $fckey => $fcinfo) {
		$query = new EntityFieldQuery;
		$tmp2 = $query
			->entityCondition('entity_type', 'node')
			->entityCondition('bundle', 'shipper_profile')
			->propertyCondition('status', 1)
			->fieldCondition('field_delivery_options', 'value', $fckey)
			->execute();
		foreach($tmp2['node'] as $nkey=>$nval) {
			$shipperprofileid[$fckey] = $nkey;
		}
	}
	
	$field_collections = entity_load('field_collection_item', array_keys($shipperprofileid));

	$deliveryzips = array();
	foreach($field_collections as $keys=>$values) {
		foreach($values->field_delivery_areas[LANGUAGE_NONE] as $key=>$value) {
			if(!empty($value['value'])) {
				if(strlen($value['value']) == 5) {
					$deliveryzips[$value['value']] = array();
				}
				elseif(strlen($value['value']) == 4) {
					for($i = 0; $i<=9; $i++) {
						$deliveryzips[$value['value'] . $i] = array();
					}
				}
				elseif(strlen($value['value']) == 3) {
					for($i = 0; $i<=9; $i++) {
						for($j = 0; $j<=9; $j++) {
							$deliveryzips[$value['value'] . $i . $j] = array();
						}
					}
				}
				elseif(strlen($value['value']) == 2) {
					for($i = 0; $i<=9; $i++) {
						for($j = 0; $j<=9; $j++) {
							for($k = 0; $k<=9; $k++) {
								$deliveryzips[$value['value'] . $i . $j . $k] = array();
							}
						}
					}
				}
				elseif(strlen($value['value']) == 1) {
					for($i = 0; $i<=9; $i++) {
						for($j = 0; $j<=9; $j++) {
							for($k = 0; $k<=9; $k++) {
								for($l = 0; $l<=9; $l++) {
									$deliveryzips[$value['value'] . $i . $j . $k . $l] = array();
								}
							}
						}
					}
				}
			}
		}
	}

	foreach($deliveryzips as $zipcode=>$other) {
		$locinfo = regiomino_geolocation_queryinfobyzip($zipcode);
		
		if(empty($locinfo)) {
			unset($deliveryzips[$zipcode]);
		}
		else {
			$mainissub = FALSE;
			if(isset($locinfo['sub_regions']) && !empty($locinfo['sub_regions'])) {
				foreach($locinfo['sub_regions'] as $keyno=>$infotype) {
					$subregionname = explode(',',$infotype['name']);
					$mainregionname = explode(',',$locinfo['main_region']['name']);
					if($infotype['name'] != $locinfo['main_region']['name'] && $subregionname[0] != $mainregionname[0]) {
						$infotype['name'] = $mainregionname[0] . ' (' . $subregionname[0] . ')';
					}
					$deliveryzips[$zipcode][] = $infotype;
				}
			}
			else {
				$deliveryzips[$zipcode][] = $locinfo['main_region'];
			}
		}
	}
	ksort($deliveryzips);
	foreach($deliveryzips as $zipcode=>$value) {
		usort($deliveryzips[$zipcode], "regiomino_geolocation_sortdeliveryzips");
	}


	variable_set('regiomino_geodb_avlbregions', $deliveryzips);
	drupal_set_message(t('Variable with available zipcodes has been updated: @variable', array('@variable' => 'regiomino_geodb_avlbregions')), 'status');
	
	///Retrieve current list of storage profiles
	$query = new EntityFieldQuery;
	$tmp = $query
		->entityCondition('entity_type', 'field_collection_item')
		->entityCondition('bundle', 'field_storage_data')
		->execute();
	$shipperprofileid = array();
	foreach($tmp['field_collection_item'] as $fckey => $fcinfo) {
		$query = new EntityFieldQuery;
		$tmp2 = $query
			->entityCondition('entity_type', 'node')
			->entityCondition('bundle', 'storage_profile')
			->propertyCondition('status', 1)
			->fieldCondition('field_storage_data', 'value', $fckey)
			->execute();
		foreach($tmp2['node'] as $nkey=>$nval) {
			$shipperprofileid[$fckey] = $nkey;
		}
	}

	//$shipperprofileid now contains all shippers that have matching regions and offer a possibly valid time
	//now retrieve that time and offer it to the customer as a selection
	
	$field_collections = entity_load('field_collection_item', array_keys($shipperprofileid));
	
	$pickuppoints = array();
	foreach($field_collections as $fci=>$fcinfo) {
		$pickuppoints[$fci] = $fcinfo->field_address[LANGUAGE_NONE][0]['postal_code'] . ' ' . $fcinfo->field_address[LANGUAGE_NONE][0]['locality'] . ' ' . $fcinfo->field_address[LANGUAGE_NONE][0]['name_line'];
	}
	
	variable_set('regiomino_geodb_avlbpoints', $pickuppoints);
	drupal_set_message(t('Variable with available Regiomino points has been updated: @variable', array('@variable' => 'regiomino_geodb_avlbpoints')), 'status');

	/* $counter = 1;
	foreach($deliveryzips as $zipcode=>$value) {
		foreach($value as $index=>$values) {
			echo $counter . ': ' . $zipcode . ' ' . $values['name'] . ' --> ' . $values['lat'] . ',' . $values['lon'] . '<br />';
			$counter++;
		}
	}
	
	die(); */
	
	/*
	//Retrieve and unzip file to private file path
	$geodb = url('http://download.geonames.org/export/zip/DE.zip');
	$contents = drupal_http_request($geodb);
	$filename = 'geodb_tmp.zip';
	$filepath = "private://" . $filename;
	file_put_contents($filepath, $contents->data);
	$drpl_geodb_path = variable_get('file_private_path');
	$drpl_geodb_path_unzip = $drpl_geodb_path . '/GEODB.txt';
	$command = 'unzip -p ' . $drpl_geodb_path . '/' . $filename . ' DE.txt > ' . $drpl_geodb_path_unzip;
	system($command);
	
	//Retrieve csv data
	if (($handle = fopen($drpl_geodb_path_unzip, "r")) !== FALSE) {
    while (($data = fgetcsv($handle, 0, "\t")) !== FALSE) {
			$finalarray[] = $data;
    }
	}
	drupal_set_message(t('File retrieved, unzipped and csv-parsed from @geodb', array('@geodb' => $geodb)), 'status');
	
	$result = db_truncate('regiomino_geodb')->execute();
	drupal_set_message(t('Database table regiomino_geodb truncated'), 'status');
	$avlbregions = array();
 	foreach($finalarray as $key=>$value) {
		db_query("INSERT INTO {regiomino_geodb} (country_code, postal_code, place_name, admin_name1, admin_code1, admin_name2, admin_code2, admin_name3, admin_code3, latitude, longitude, accuracy) VALUES (:country_code, :postal_code, :place_name, :admin_name1, :admin_code1, :admin_name2, :admin_code2, :admin_name3, :admin_code3, :latitude, :longitude, :accuracy)", array(':country_code' => $value[0], ':postal_code' => $value[1], ':place_name' => $value[2], ':admin_name1' => $value[3], ':admin_code1' => $value[4], ':admin_name2' => $value[5], ':admin_code2' => $value[6], ':admin_name3' => $value[7], ':admin_code3' => $value[8], ':latitude' => $value[9], ':longitude' => $value[10], ':accuracy' => $value[11]));
		if(array_key_exists($value[1], $deliveryzips)) {
			$avlbregions[] = $value;
		}
	}
	drupal_set_message(t('Data written to database table regiomino_geodb'), 'status');
	
	variable_set('regiomino_geodb_avlbregions', $avlbregions);
	drupal_set_message(t('Variable with available zipcodes has been updated: @variable', array('@variable' => 'regiomino_geodb_avlbregions')), 'status');
	*/
	
}

function regiomino_geolocation_sortdeliveryzips($a, $b) {
	return strcasecmp($a["name"], $b["name"]);
}

function regiomino_geolocation_queryinfobyzip($zip) {
	db_set_active('opengeodb');
	$query = db_query("SELECT local.text_val as region_name, plz.text_val as plz, hierarchy.level as level, coords.lat, coords.lon FROM geodb_textdata plz, geodb_textdata local, geodb_coordinates coords, geodb_hierarchies hierarchy WHERE plz.text_val = :plz AND plz.text_type = 500300000 AND plz.loc_id = local.loc_id AND local.text_type = 500100000 AND local.loc_id = hierarchy.loc_id AND local.loc_id = coords.loc_id", array(':plz' => $zip));
	
	$results = array();
	foreach($query as $value) {
		$results[] = $value;
	}
	db_set_active();
	$locinfo = array();
	$row = 0;
	foreach($results as $key=>$value) {

		if($value->level == 6) {
			$locinfo['main_region']['name'] = $value->region_name;
			$locinfo['main_region']['lat'] = $value->lat;
			$locinfo['main_region']['lon'] = $value->lon;
		}
 		elseif($value->level == 8) {
			$locinfo['sub_regions'][$row]['name'] = $value->region_name;
			$locinfo['sub_regions'][$row]['lat'] = $value->lat;
			$locinfo['sub_regions'][$row]['lon'] = $value->lon;
			$row++;
		}
	}

	return $locinfo;
}