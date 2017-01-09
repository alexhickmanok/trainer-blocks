<?php

function during_invitation_accept_page($invitation_guid){
// Build Breadcrumbs
$breadcrumb = array();
$breadcrumb[] = l('Home', '<front>');

	
	$page = '';

	$results = db_select('dur_invitation','i')
		->fields('i')
		->condition('invitation_guid',$invitation_guid,'=')
		->condition('invitation_status',0,'=')
		->execute();
		$numCount = $results->rowCount();
		if($numCount == 1){
		$invitation = $results->fetchAssoc();
		$contest_guid = $invitation['contest_guid'];

		module_load_include('inc', 'during', 'z_contest');
		$page .= during_contest_page('view',$contest_guid);
		$breadcrumb[] = l('Contest', 'during/contest/view/' . $contest_guid);		
		}else{
			form_set_error('USER ERROR',t('This is not a valid invitation'));
		}

// Set Breadcrumbs
drupal_set_breadcrumb($breadcrumb);
	return $page;
}




//****************************************************************************
//***HOOK FORM SETUP***
//****************************************************************************

function during_registration_form($form, &$form_state,$invitation_guid){

	$results = db_select('dur_invitation','i')
		->fields('i')
		->condition('invitation_guid',$invitation_guid,'=')
		->execute();
		$numCount = $results->rowCount();

		if($numCount = 1){
		$invitation = $results->fetchAssoc();
		global $user;
		$user_id = $user->uid;
		}else{
			return $form;
		}


    $form['registration_id'] = array(
	'#title' => t('Registration ID'),
	'#type' => 'hidden',
	'#access' => FALSE,
	'#required' => FALSE,
	);

    $form['registration_guid'] = array(
	'#title' => t('Registration GUID'),
	'#type' => 'hidden',
	'#access' => FALSE,
	'#required' => FALSE,
	);
        
    $form['contestant_id'] = array(
	'#title' => t('Contestant ID'),
	'#type' => 'textfield',
	'#access' => FALSE,
	'#required' => TRUE,
	'#default_value' => $user_id,
	);

    $form['contest_guid'] = array(
	'#title' => t('Contest GUID'),
	'#type' => 'textfield',
	'#access' => FALSE,
	'#size' => 15,
	'#maxlength' => 15,
	'#required' => TRUE,
	'#default_value' => $invitation['contest_guid'],
	);

    $form['payment_status'] = array(
	'#title' => t('Payment Status'),
	'#type' => 'select',
	'#options' => array(
		0 => t('Not Paid'),
		1 => t('Partial Payment'),
		2 => t('Paid'),
	),
	'#access' => FALSE,
    '#default_value' => 0,
	);

    $form['registration_status'] = array(
	'#title' => t('Registration Status'),
	'#type' => 'select',
	'#options' => array(
		0 => t('Started'),
		1 => t('Complete'),
	),
	'#access' => FALSE,
    '#default_value' => 0,
	);
	
	$form['invitation_guid'] = array(
	'#title' => t('Invitation GUID'),
	'#type' => 'hidden',
	'#required' => TRUE,
	'#default_value' => $invitation['invitation_guid'],
	);

    $form['submit'] = array(
	'#value' => t('Join'),
	'#type' => 'submit',
	'#access' => TRUE,
	);

    $form['#attributes'] = array('AutoComplete' => 'off');

return $form;

}

function during_form_during_registration_form_alter(&$form, &$form_state, $formid){
	
	$registration_guid = during_random_guid('registration',16);
	$form['registration_guid']['#default_value'] = $registration_guid;
	$form['payment_status']['#default_value'] = 0;
	$form['registration_status']['#default_value'] = 0;
	return $form;
}


function during_registration_form_validate($form,&$form_state){

$invitation_guid = $form_state['values']['invitation_guid'];
$contest_guid = $form_state['values']['contest_guid'];
$contestant_id = $form_state['values']['contestant_id'];

	$inv_guid = db_select('dur_invitation', 'i')
				->fields('i',array('invitation_guid'))
				->condition('invitation_guid',$invitation_guid,'=')
				->condition('contest_guid',$contest_guid,'=')
				->condition('invitation_status',0,'=')
				->execute();
				$numCount = $inv_guid->rowCount();
				
				if($numCount == 1){
					$invitation_pass = TRUE;
				} else{
					$invitation_pass = FALSE;
					form_set_error('USER ERROR','This is not a valid invitation or has already been used.');
				}
				

    $edit_type = $form_state['build_info']['args'][0];

	
	$results = db_select('dur_registration', 'r')
				->fields('r', array('contestant_id'))
				->condition('contest_guid',$contest_guid,'=')
				->condition('contestant_id',$contestant_id,'=')
				->execute();
		$numResults = $results->rowCount();
		if($numResults !=0){
						form_set_error('USER ERROR','You are already registered for this contest.');
						drupal_goto('during/contest/view/' . $contest_guid);
					}

	
	switch($edit_type){
		case 'new':
			//dsm('new form');
			break;
		case 'edit':
			//dsm('edit form');
			break;
		default:
			//form_set_error('Error: the form cannot be processed');
			break;
};

}

function during_registration_form_submit($form,&$form_state){
	$registration_id = $form_state['values']['registration_id'];
	$registration_guid = $form_state['values']['registration_guid'];
	$invitation_guid = $form_state['values']['invitation_guid'];
	$contest_guid = $form_state['values']['contest_guid'];
	$contestant_id = $form_state['values']['contestant_id'];
	$payment_status = $form_state['values']['payment_status'];
	$registration_status = $form_state['values']['registration_status'];
	during_create_registration($registration_id,$registration_guid,$invitation_guid,$contest_guid,$contestant_id,$payment_status,$registration_status);
	$invite = db_merge('dur_invitation')
                ->key(array('invitation_guid' => $invitation_guid))
                ->fields(array(
                  'invitation_status' => 1,
                   ))
                ->execute();
				
	$form_state['redirect'] = 'during/contest/view/' . $contest_guid;
}



function during_create_registration($registration_id='',$registration_guid,$invitation_guid,$contest_guid='',$contestant_id='',$payment_status='',$registration_status=''){
	if($registration_id == ''){
		$registration_id = NULL;
	};
	$cid = db_merge('dur_registration')
	->key(array('registration_id' => $registration_id))
	->fields(array(
		'registration_guid' => $registration_guid,
		'invitation_guid' => $invitation_guid,
		'contest_guid' => $contest_guid,
		'contestant_id' => $contestant_id,
		'payment_status' => $payment_status,
		'registration_status' => $registration_status,
	))
	->execute();

	//Update the registration table to make registration status = 1
	//0 = invite; 1 = registered
	
}

function registration_block_define($invitation_guid){

			$block = array();
			global $user;
			if(user_access('join contest')==1){
			$user_id = $user->uid;
			$inv = db_select('dur_invitation', 'i')
						->fields('i', array('invitation_guid'))
						->condition('invitation_guid',$invitation_guid,'=')
						->condition('invitation_status',0,'=')
						->execute();
						$numReg = $inv->rowCount();

						if($numReg){
			$block['subject'] = t('Join Contest');
			module_load_include('inc', 'during', 'z_registration');
			$block['content'] = drupal_get_form('during_registration_form',$invitation_guid);
			}
			}
			return $block;
}