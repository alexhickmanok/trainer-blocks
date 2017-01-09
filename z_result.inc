<?php

function during_result_page($result_guid){
$page = '';

$result = db_select('dur_result', 'r')
            ->fields('r',array('event_guid','contest_guid'))
            ->condition('result_guid',$result_guid,'=')
            ->execute();
            $numRecs = $result->rowCount();
            
            if($numRecs == 1){    

$result_info = $result->fetchAssoc();
// Build Breadcrumbs
$breadcrumb = array();
$breadcrumb[] = l(t('Home'), '<front>');
$breadcrumb[] = l(t('Contest'), 'during/contest/view/' . $result_info['contest_guid']);
$breadcrumb[] = l(t('Event'), 'during/event/view/' . $result_info['event_guid']);
$breadcrumb[] = t('Results');

// Set Breadcrumbs
drupal_set_breadcrumb($breadcrumb);    
    
    $form = drupal_get_form('during_result_form',$result_guid);
    $page .= drupal_render($form);
            


}else{
      $page .= 'something went wrong';
}
    
    return $page;
    
}

//****************************************************************************
//    RESULT FORM CREATE/EDIT
//****************************************************************************

function during_result_form($form, &$form_state,$result_guid){
global $user;
	    $form = array();
    $results_get = db_select('dur_result', 'r')
	    ->fields('r', array())
        ->condition('result_guid',$result_guid,'=')
	    ->execute();
        
	    $results = $results_get->fetchAssoc();

        $rowCount = $results_get->rowCount();
    if($rowCount != 1){
        form_set_error('USER ERROR',t('something went wrong...oops'));
    }else{
    
    $contest_guid = $results['contest_guid'];
	$result_status = 'Open';
	$result_weight = NULL;
$form['result_id'] = array(
	'#title' => t('Result ID'),
	'#type' => 'hidden',
	'#access' => FALSE,
	'#required' => FALSE,
	'#default_value' => $results['result_id'],
	);
$form['result_guid'] = array(
	'#title' => t('Result GUID'),
	'#type' => 'hidden',
	'#access' => FALSE,
	'#required' => FALSE,
	'#default_value' => $results['result_guid'],
	);
$form['contestant_id'] = array(
	'#title' => t('Contestant ID'),
	'#type' => 'textfield',
	'#access' => FALSE,
	'#required' => TRUE,
	'#default_value' => $results['contestant_id'],
	);
$form['contest_guid'] = array(
	'#title' => t('Contest ID'),
	'#type' => 'textfield',
	'#access' => FALSE,
	'#size' => 15,
	'#maxlength' => 15,
	'#required' => TRUE,
	'#default_value' => $results['contest_guid'],
	);
$form['event_guid'] = array(
	'#title' => t('Event ID'),
	'#type' => 'textfield',
	'#access' => FALSE,
	'#size' => 15,
	'#maxlength' => 15,
	'#required' => TRUE,
	'#default_value' => $results['event_guid'],
	);
$form['result_status'] = array(
	'#title' => t('Result Status'),
	'#type' => 'select',
	'#options' => array(
		0 => t('Open'),
		1 => t('Closed'),
	),
	'#default_value' => $results['result_status'],
	'#access' => false,
	);
$form['result_weight'] = array(
	'#title' => t('Weight'),
	'#type' => 'textfield',
	'#access' => TRUE,
	'#size' => 15,
	'#maxlength' => 15,
	'#required' => TRUE,
	'#default_value' => $results['result_weight'] + 0.00,
	);
$form['measurement_1'] = array(
	'#title' => t('Measurement 1'),
	'#type' => 'textfield',
	'#access' => TRUE,
	'#size' => 10,
	'#maxlength' => 10,
	'#required' => TRUE,
	'#default_value' => $results['measurement_1'] + 0.00,
	);
$form['measurement_2'] = array(
	'#title' => t('Measurement 2'),
	'#type' => 'textfield',
	'#access' => TRUE,
	'#size' => 10,
	'#maxlength' => 10,
	'#required' => TRUE,
	'#default_value' => $results['measurement_2'] + 0.00,
	);
$form['measurement_3'] = array(
	'#title' => t('Measurement 3'),
	'#type' => 'textfield',
	'#access' => TRUE,
	'#size' => 10,
	'#maxlength' => 10,
	'#required' => TRUE,
	'#default_value' => $results['measurement_3'] + 0.00,
	);
$form['body_fat_pct'] = array(
	'#title' => t('Body Fat %'),
	'#type' => 'textfield',
	'#access' => TRUE,
	'#size' => 10,
	'#maxlength' => 10,
	'#required' => TRUE,
	'#default_value' => $results['body_fat_pct'] + 0.00,
	);
$form['custom_value'] = array(
	'#title' => t('Body Fat %'),
	'#type' => 'decimal',
	'#access' => TRUE,
	'#size' => 17,
	'#maxlength' => 17,
	'#required' => TRUE,
	'#default_value' => $results['body_fat_pct'],
	);
$form['eligible'] = array(
	'#title' => t('Eligible'),
	'#type' => 'checkbox',
	'#access' => TRUE,
	'#required' => FALSE,
	'#default_value' => $results['eligible'],
);
$form['submit'] = array(
	'#value' => t('Submit'),
	'#type' => 'submit',
	'#access' => TRUE,
	);
$form['#attributes'] = array('AutoComplete' => 'off');
    }
return $form;
}

function during_form_during_result_form_alter(&$form, &$form_state, $formid){

	  return $form;
}


function during_result_form_validate($form,&$form_state){

	$result_id = $form_state['values']['result_id'];
    $result_guid = $form_state['values']['result_guid'];
	$contestant_id = $form_state['values']['contestant_id'];
	$contest_guid = $form_state['values']['contest_guid'];
    $event_guid = $form_state['values']['event_guid'];
	$result_status = $form_state['values']['result_status'];
	$result_weight = $form_state['values']['result_weight'];
	$eligible = $form_state['values']['eligible'];
}

function during_result_form_submit($form,&$form_state){
	
	$result_id = $form_state['values']['result_id'];
    $result_guid = $form_state['values']['result_guid'];
	$contestant_id = $form_state['values']['contestant_id'];
	$contest_guid = $form_state['values']['contest_guid'];
	$event_guid = $form_state['values']['event_guid'];
	$result_status = $form_state['values']['result_status'];
	$result_weight = $form_state['values']['result_weight'];
	$measurement_1 = $form_state['values']['measurement_1'];
	$measurement_2 = $form_state['values']['measurement_2'];
	$measurement_3 = $form_state['values']['measurement_3'];
	$measurement_total = $measurement_1 + $measurement_2 + $measurement_3;
	$body_fat_pct = $form_state['values']['body_fat_pct'];
	$eligible = $form_state['values']['eligible'];

	during_create_result($result_id,$result_guid,$contestant_id,$contest_guid,$event_guid,$result_status,$result_weight,$measurement_1,$measurement_2,$measurement_3,$measurement_total,$body_fat_pct,$eligible);
	$form_state['redirect'] = 'during/event/view/' . $event_guid;
}

function during_create_result($result_id,$result_guid,$contestant_id,$contest_guid,$event_guid,$result_status,$result_weight,$measurement_1,$measurement_2,$measurement_3,$measurement_total,$body_fat_pct,$eligible){
	$result_new = db_merge('dur_result') // Table name no longer needs {}
        ->key(array('result_id'=>$result_id))
		->fields(array(
			'result_guid' => $result_guid,
			'contestant_id' => $contestant_id,
			'contest_guid' => $contest_guid,
			'event_guid' => $event_guid,
			'result_status' => $result_status,
            'result_weight' => $result_weight,
			'measurement_1' => $measurement_1,
			'measurement_2' => $measurement_2,
			'measurement_3' => $measurement_3,
			'measurement_total' => $measurement_total,
			'body_fat_pct' => $body_fat_pct,
			'eligible' => $eligible,
		))	
		->execute();
}