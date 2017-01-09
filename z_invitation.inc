<?php

//**********************************************************************
//***INVITATION LANDING PAGE***
//**********************************************************************
function during_invitation_page($contest_guid){
// Build Breadcrumbs
$breadcrumb = array();
$breadcrumb[] = l('Home', '<front>');
$breadcrumb[] = l('Contest', 'during/contest/view/' . $contest_guid);

// Set Breadcrumbs
drupal_set_breadcrumb($breadcrumb);

$page = '';

global $user;

    $results = db_select('dur_invitation', 'n');
		$results->fields('n', array('user_email','invitation_status'));
		$results->condition('n.contest_guid', $contest_guid,'=');
        $result = $results->execute();
    
    $r=0;
    
	//Setup table headers
	$header = array(
					'user_email' => t('Email'),
					'invitation_status' => t('Invitation Status'),
					);
	
    $rows = array();
        while($record = $result->fetchAssoc()) {
	$r++;
	$rows[$r] = array();
	$rows[$r]['user_email'] = $record['user_email'];
	$rows[$r]['invitation_status'] = $record['invitation_status'];
    };
	
$page .= theme('table', array(
    'header' => $header, 
    'rows' => $rows,
    'attributes' => array('width' => '100%')
));

if(!$rows){
	$page .= t('No users have been invited to the contest yet');
}
	return $page;
}

//**********************************************************************
//***HOOK FORM***
//**********************************************************************
function during_invitation_form($form, &$form_state,$contest_guid,$contest_title){
    $form['invitation_id'] = array(
	'#title' => t('Invitation ID'),
	'#type' => 'hidden',
	'#access' => FALSE,
	'#required' => FALSE,
	);

    $form['contest_guid'] = array(
	'#title' => t('Contestant ID'),
	'#type' => 'textfield',
	'#access' => FALSE,
	'#required' => FALSE,
    '#default_value' => $contest_guid,
	);
        
    $form['user_email'] = array(
	'#title' => t('Email Address'),
	'#type' => 'textfield',
    '#size' => '25',
    '#max_length' => '255',
	'#access' => TRUE,
	'#required' => FALSE,
	);

    $form['invitation_status'] = array(
	'#title' => t('Invitation Status'),
	'#type' => 'select',
	'#options' => array(
		0 => t('No response'),
		1 => t('Accepted'),
		2 => t('Ignored'),
	),
	'#access' => FALSE,
    '#default_value' => 0,
	);

    $form['contest_title'] = array(
	'#title' => t('Contest Title'),
	'#type' => 'hidden',
	'#access' => FALSE,
    '#default_value' => $contest_title,
	);
	
    $form['submit'] = array(
	'#value' => t('Send Invitation'),
	'#type' => 'submit',
	'#access' => TRUE,
	);

    $form['#attributes'] = array('AutoComplete' => 'off');
  
  return $form;

}


//**********************************************************************
//***HOOK FORM ALTER***
//**********************************************************************
function during_form_during_invitation_form_alter(&$form, &$form_state, $formid){

}


//**********************************************************************
//***HOOK FORM VALIDATE***
//**********************************************************************
function during_invitation_form_validate($form,&$form_state){
	
	
}

//**********************************************************************
//***HOOK FORM SUBMIT***
//**********************************************************************
function during_invitation_form_submit($form,&$form_state){
        
        $invitation_id = $form_state['values']['invitation_id'];
        $contest_guid = $form_state['values']['contest_guid'];
        $user_email = $form_state['values']['user_email'];
        $invitation_status = $form_state['values']['invitation_status'];
		$contest_title = $form_state['values']['contest_title'];

		//module_load_include('module','during','during');
		$invitation_guid = during_random_guid('invitation',16);
		$invitation_id = during_invitation_create($invitation_id,$invitation_guid,$contest_guid,$user_email,$invitation_status);
        during_add_admin('',$contest_guid,'','invite',$user_email,$invitation_guid);
		    
      // example_mail() will be called based on the first drupal_mail() parameter.
	  global $language;
	  $params=array();
	  $params['invitation_guid'] = $invitation_guid;
	  $params['contest_title'] = $contest_title;
	  $params['message'] = 'hello world';
	  $params['language'] = $language;
      drupal_mail('during', 'contest_invitation', $user_email, $language,$params,NULL,TRUE);
}


function during_invitation_create($invitation_id='',$invitation_guid,$contest_guid,$user_email,$invitation_status){
        if($invitation_id == ''){
            $invitation_id = NULL;
        }
        $invite = db_merge('dur_invitation')
                ->key(array('invitation_id' => $invitation_id))
                ->fields(array(
				  'invitation_guid' => $invitation_guid,
                  'contest_guid' => $contest_guid,
                  'user_email' => $user_email,
                  'invitation_status' => $invitation_status,
                   ))
                ->execute();
        $invite_id = Database::getConnection()->lastInsertId();
        return $invite_id;
}

function invitation_block_define($contest_guid){
		$result = db_select('dur_contest', 'c')
				->fields('c',array('contest_title'))
				->condition('contest_guid',$contest_guid,'=')
				->execute();
				$contest= $result->fetchAssoc();
				
				$contest_title = $contest['contest_title'];
	
	
			$block = array();
			$block['subject'] = t('Invite Contestants');
			
            $block_content = drupal_get_form('during_invitation_form',$contest_guid,$contest_title);
            $block['content'] = $block_content;
			return $block;
}


function during_mail($key, &$message, $params) {
  global $user;

  // Each message is associated with a language, which may or may not be the
  // current user's selected language, depending on the type of e-mail being
  // sent. This $options array is used later in the t() calls for subject
  // and body to ensure the proper translation takes effect.
  $options = array(
    'langcode' => $message['language']->language,
  );

  switch ($key) {
    // Send a simple message from the contact form.
    case 'contest_invitation':
      $message['subject'] = t('E-mail sent from @site-name', array('@site-name' => variable_get('site_name', 'Drupal')), $options);
      // Note that the message body is an array, not a string.
      $message['body'][] = t('@name sent you the following message:', array('@name' => $user->name), $options);
	  $message['body'][] = t('You have been invited to join @contest_title.  To join this contest follow the link below.', array('@contest_title' => $params['contest_title']),$options);
	  $message['body'][] = 'http://dev-losingit.pantheon.io/during/invitation/accept/' . $params['invitation_guid'];
      // Because this is just user-entered text, we do not need to translate it.
      // Since user-entered text may have unintentional HTML entities in it like
      // '<' or '>', we need to make sure these entities are properly escaped,
      // as the body will later be transformed from HTML to text, meaning
      // that a normal use of '<' will result in truncation of the message.
      $message['body'][] = check_plain($params['message']);
	  $message['send'] = TRUE;
      break;
  }
}

