<?php
//**********************************************************************
//***HOOK FORM***
//**********************************************************************
function during_admin_form($form, &$form_state,$contest_guid){
 // Build Breadcrumbs
$breadcrumb = array();
$breadcrumb[] = l('Home', '<front>');
$breadcrumb[] = l('Contest', 'during/contest/view/' . $contest_guid);

// Set Breadcrumbs
drupal_set_breadcrumb($breadcrumb);

  $header = array(
    'user_name' => t('User'),
    'user_email' => t('Email'),
    'user_access' => t('Access type'),
    'registration_status' => t('Registration Status'),
  );

  $rows = array();
        $result = db_select('dur_registration', 'r');
        $result->join('users','u','r.contestant_id = u.uid');
        $result->fields('r', array('registration_status'));
        $result->fields('u', array('uid','name','mail'));
        $result->condition('r.contest_guid',$contest_guid,'=');
        $results = $result->execute();
        
        $i = 1;
    while($record = $results->fetchAssoc()){
        
        $access = db_select('dur_access','a')
         ->fields('a', array('access_type'))
         ->condition('a.contest_guid',$contest_guid,'=')
         ->condition('a.user_id',$record['uid'],'=')
         ->execute();
         
         $access_type = '';
         $r = 0;
         while($acc_rec = $access->fetchAssoc()){
            if($r>0){
                $access_type .= ', ';
            }
            
            $access_type .= $acc_rec['access_type'];
            $r++;
         }
        
        $rows[$i] = array();
        $rows[$i]['user_name'] = $record['name'];
        $rows[$i]['user_email'] = $record['mail'];
        $rows[$i]['user_access'] = $access_type;
        if($record['registration_status'] == 0){
            $reg_status = 'Invited';
        } else {
            $reg_status = 'Registered';
        }
        $rows[$i]['registration_status'] = $reg_status;
    
    
    $i++;
    }
  

    $form['admins_create'] = array(
      '#type' => 'tableselect',
      '#header' => $header,
      '#options' => $rows,
      '#empty' => t('No users have been invited to this contest.'),
    );
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Submit'),
    );
  return $form;

}


//**********************************************************************
//***HOOK FORM ALTER***
//**********************************************************************
function during_form_during_admin_form_alter(&$form, &$form_state, $formid){
	
	return $form;
}


//**********************************************************************
//***HOOK FORM VALIDATE***
//**********************************************************************
function during_admin_form_validate($form,&$form_state){
	
	
}

//**********************************************************************
//***HOOK FORM SUBMIT***
//**********************************************************************
function during_admin_form_submit($form,&$form_state){
    $contest_guid = $form_state['build_info']['args'][0];
    
    $values = $form_state['complete form']['admins_create']['#options'];
    foreach($values as $key=>$row){
        
        if($form_state['complete form']['admins_create'][$key]['#checked'] == 1){
            $user_name = $row['user_name'];
            $user_get = db_select('users', 'u');
                $user_get->leftJoin('dur_access','a','a.user_id = u.uid');
                $user_get->fields('u',array('uid'));
                $user_get->fields('a', array('access_guid'));
                $user_get->condition('u.name',$user_name,'=');
                $user_get->condition('a.contest_guid',$contest_guid,'=');
                $user_get->condition('a.access_type',$row['user_access'],'=');
                $user_info = $user_get->execute();
            $uid = $user_info->fetchAssoc();
            $user_id = $uid['uid'];
            $access_guid = $uid['access_guid'];
            if($reg_type == 'Invited'){
                drupal_set_message('User ' . $user_name . ' must be registered before assigning admin access','error');
            }else{
            during_add_admin($access_guid,$contest_guid,$user_id,'admin','','');
            }
        }
        
    }
}


function contestant_block_define($contest_guid){
	
			$block = array();
			$block['subject'] = t('Registered Contestants');
			
			
			
			$results = db_select('dur_contest', 'n');
			$results->join('dur_registration', 'r', 'n.contest_guid = r.contest_guid');
			$results->join('users','u','u.uid = r.contestant_id');
			$results->fields('n', array('contest_id','contest_guid','contest_title','contest_status','start_date','end_date'));
			$results->fields('r', array('registration_guid'));
			$results->fields('u',array('name'));
			$results->condition('n.contest_guid', $contest_guid,'=');
			$result = $results->execute();
			
			$block_content = '';
			$i=1;
			while($record = $result->fetchAssoc()) {
				if($i>1){
				$block_content .= '</br>';
				};
				$block_content .= $i . ')  ' . $record['name'];
				$i++;
			};
            $block['content'] = $block_content;
			return $block;
}
