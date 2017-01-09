<?php
//*************************************************************
//***CONTESTS LANDING PAGE***
//*************************************************************

function during_contests_page(){
 // Build Breadcrumbs
$breadcrumb = array();
$breadcrumb[] = l(t('Home'), '<front>');
$breadcrumb[] = t('Contests');
// Set Breadcrumbs
drupal_set_breadcrumb($breadcrumb);


$page = '';

global $user;

    $results = db_select('dur_contest', 'n');
		$results->leftJoin('dur_registration', 'r', 'n.contest_guid = r.contest_guid');
		$results->fields('n', array('contest_id','contest_guid','contest_title','contest_status','start_date','end_date'));
		//$results->condition('n.contest_status', 'Not Started','=');
		$results->distinct();
		//$results->isNull('r.contest_guid');
	$result = $results->execute();
    
    $r=0;
    
	//Setup table headers
	$header = array(
//					'link' => t('Link'),
//					'join' => t('Join'),
					'contest_title' => t('Title'),
					'contest_status' => t('Status'),
					'start_date' => t('Start Date'),
					'end_date' => t('End Date'),
					);
	
	//Get Row Data
    $rows = array();
        while($record = $result->fetchAssoc()) {
	$r++;
	$rows[$r] = array();
	$rows[$r]['contest_title'] = l(t($record['contest_title']),'during/contest/view/' . $record['contest_guid']);
	$rows[$r]['contest_status'] = $record['contest_status'];
	$rows[$r]['start_date'] = format_date(strtotime($record['start_date']),'custom','m/d/Y');
	$rows[$r]['end_date'] = format_date(strtotime($record['end_date']),'custom','m/d/Y');
    };
	
$page .= theme('table', array(
    'header' => $header, 
    'rows' => $rows,
    'attributes' => array('width' => '100%')
));

if(!$rows){
	$page .= t('No contests are available at this time');
}
	return $page;
}



//*************************************************************
//***CONTEST LANDING PAGE***
//*************************************************************
function dur_contest_header_block_define($item_guid){

global $user;

		$results = db_select('dur_contest', 'n')
		->fields('n', array('contest_id','contest_guid','contest_title','contest_status','start_date','end_date'))
		->condition('n.contest_guid',$item_guid,'=')
		->execute();
		$record = $results->fetchAssoc();

$block_content = '';
$block_content .= '<h1 class="title" id="page_title">' . $record['contest_title'] . '</h1>';
$block_content .= '<b>Contest Status: </b>' . $record['contest_status'] . '</br>';
$block_content .= '<b>Start Date: </b>' . format_date(strtotime($record['start_date']),'custom','m/d/Y') . '</br>';
$block_content .= '<b>End Date: </b>' . format_date(strtotime($record['end_date']),'custom','m/d/Y') . '</br>';
		
$block['subject'] = '';
$block['content'] = $block_content;
return $block;
}


function my_results_block_define($item_guid){

global $user;

		$results = db_select('dur_event', 'e')
		->fields('e', array('event_guid'))
		->condition('e.contest_guid',$item_guid,'=')
		->condition('e.event_status','closed','=')
		->orderBy('e.event_date', 'DESC')
		->range(0,1)
		->execute();
		$record = $results->fetchAssoc();
		
		
		$results = db_select('dur_event_score', 'r');
		$results->join('dur_result','s', 'r.result_guid = s.result_guid');
		$results->fields('r', array('start_weight','scored_weight','lbs_lost_td','pct_lost_td','pct_lost_td_raw_rank'));
		$results->fields('s', array('result_weight'));
		$results->condition('r.event_guid',$record['event_guid'],'=');
		$results->condition('r.contestant_id',$user->uid,'=');
		$records = $results->execute();
		$record = $records->fetchAssoc();

		$payouts = db_select('dur_payout', 'p');
				$payouts->fields('p',array('contestant_id'));
				$payouts->addExpression('sum(payout_amt)','payout');
				$payouts->condition('contest_guid',$item_guid,'=');
				$payouts->condition('contestant_id',$user->uid,'=');
				$payouts_val = $payouts->execute();
				$payout = $payouts_val->fetchAssoc();
				

$block_content = '';

$block_content .= '<b>Starting Weight: </b>' . $record['start_weight'] . '</br>';
$block_content .= '<b>Lowest Weight: </b>' . $record['scored_weight'] . '</br>';
$block_content .= '<b>Current Weight: </b>' . $record['result_weight'] . '</br>';
$block_content .= '<b>Pounds Lost: </b>' . $record['lbs_lost_td'] . '</br>';		
$block_content .= '<b>Pct. Lost: </b>' . round($record['pct_lost_td']*100,3) . '%</br>';
$block_content .= '<b>Rank: </b>' . $record['pct_lost_td_raw_rank'] . '</br>';
$block_content .= '<b>Earnings: $</b>' . $payout['payout'];
$block['subject'] = 'My Results';
$block['content'] = $block_content;
return $block;
}

function during_contest_page($edit_type,$contest_guid){
 // Build Breadcrumbs
$breadcrumb = array();
$breadcrumb[] = l(t('Home'), '<front>');
$breadcrumb[] = t('Contest');
// Set Breadcrumbs
drupal_set_breadcrumb($breadcrumb);
$page = '';

global $user;

		$results = db_select('dur_contest', 'n')
		->fields('n', array('contest_status'))
		->condition('n.contest_guid',$contest_guid,'=')
		->execute();
		$record = $results->fetchAssoc();
		
$contest_status = $record['contest_status'];

global $user;

		$results = db_select('dur_contest', 'n');
		$results->leftJoin('dur_event', 'r', 'n.contest_guid = r.contest_guid');
		$results->fields('r');
		$results->condition('n.contest_guid',$contest_guid,'=');
	$result = $results->execute();

    $r=0;
    
	//Setup table headers
	$header2 = array(
					'event_title' => t('Title'),
					'event_date' => t('Event Date'),
					'event_status' => t('Status'),
					);
	
	//Get Row Data
    $rows2 = array();
        while($record = $result->fetchAssoc()) {
	$r++;
	$rows2[$r] = array();
		if($contest_status == 'Closed'){
			$rows2[$r]['event_title'] = l(t($record['event_title']),'during/event/view/' . $record['event_guid']);
		}else{
			$rows2[$r]['event_title'] = t($record['event_title']);
		};
	$rows2[$r]['event_date'] = format_date(strtotime($record['event_date']),'custom','m/d/Y');
	$rows2[$r]['contest_status'] = $record['event_status'];
    };
	
$page .= theme('table', array(
    'header' => $header2, 
    'rows' => $rows2,
    'attributes' => array('width' => '100%')
));

if(!$rows2){
	$page .= t('No events are available at this time');
}
	return $page;
}


function during_contest_prizes_page($edit_type,$contest_guid){
 // Build Breadcrumbs
$breadcrumb = array();
$breadcrumb[] = l(t('Home'), '<front>');
$breadcrumb[] = t('Contest');
// Set Breadcrumbs
drupal_set_breadcrumb($breadcrumb);
$page = '<h1>PRIZES</h1>';

	return $page;
}


function during_contest_rules_page($edit_type,$contest_guid){
 // Build Breadcrumbs
$breadcrumb = array();
$breadcrumb[] = l(t('Home'), '<front>');
$breadcrumb[] = t('Contest');
// Set Breadcrumbs
drupal_set_breadcrumb($breadcrumb);
$page = '<h1>RULES</h1>';
	return $page;
}


function during_contest_settings_page($edit_type,$contest_guid){
 // Build Breadcrumbs
$breadcrumb = array();
$breadcrumb[] = l(t('Home'), '<front>');
$breadcrumb[] = t('Contest');
// Set Breadcrumbs
drupal_set_breadcrumb($breadcrumb);
$page = '<h1>SETTINGS</h1>';
	return $page;
}



//**********************************************************************
//***CONTEST CLOSING PAGE***
//**********************************************************************
function during_contest_close_page($contest_guid){

	$updated = db_update('dur_contest') // Table name no longer needs {}
		->fields(array(
		'contest_status' => 'Closed',
		))
		->condition('contest_guid', $contest_guid, '=')
		->execute();
    
	during_create_prizes($contest_guid);
		
//Look for Events to Activate
    $result = db_select('dur_event', 'n')
	->fields('n', array('event_guid'))
	->condition('contest_guid',$contest_guid,'=')
	->execute();

//Activate all events
	module_load_include('inc','during','z_event');
    while($record = $result->fetchAssoc()){
	
	during_activate_event_page($record['event_guid']);
	
    }
	
	$min_event = db_select('dur_event', 'e');
			$min_event->fields('e', array('event_guid'));
			$min_event->addExpression('min(e.event_date)','min_event');
			$min_event->condition('contest_guid',$contest_guid,'=');
			$min_exec = $min_event->execute();
			
			$min_event_guid = $min_exec->fetchAssoc();
			
	$event_update = db_update('dur_event')
		->fields(array(
			'event_status' => 'active',
		))
		->condition('event_guid',$min_event_guid['event_guid'])
		->execute();
			
	drupal_goto('during/contest/view/' . $contest_guid);
	
    return 'ALL DONE';
}



function during_create_prizes($contest_guid){
	//Create Prize Settings
	$challenges = db_select('dur_challenge','c')
					->fields('c')
					->condition('contest_guid',$contest_guid,'=')
					->execute();
	
	$events = db_select('dur_event','e')
					->fields('e',array('event_guid'))
					->condition('contest_guid',$contest_guid,'=')
					->execute();
					$eventCount = $events->rowCount();
	
	$event = array();
	while($event_arr = $events->fetchAssoc()){
		$event[] = $event_arr['event_guid'];
	}
					
	while($challenge = $challenges->fetchAssoc()){
		for($i=$challenge['winners_from'];$i<=$challenge['winners_to'];$i++){
		module_load_include('module','during','during');
		$prize_guid = during_random_guid('prize',16);
		$challenge_guid = $challenge['challenge_guid'];
		$winner_rank = $i;
		$prize_amt = $challenge['winnings_amt'] / ($challenge['winners_to'] - $challenge['winners_from'] + 1);
		if($challenge['challenge_type'] == 1){
			$prize_amt = $prize_amt / $eventCount;
			
			for($e=1;$e<$eventCount;$e++){
				$event_guid = $event[$e];
				module_load_include('module','during','during');
				$prize_guid = during_random_guid('prize',16);
				during_create_prize($prize_guid,$challenge_guid,$contest_guid,$event_guid,$winner_rank,$prize_amt);
			}
		}else{
		$event_guid = '';
		module_load_include('module','during','during');
		$prize_guid = during_random_guid('prize',16);
		during_create_prize($prize_guid,$challenge_guid,$contest_guid,$event_guid,$winner_rank,$prize_amt);
		}
		}
	}
	$page = '';
	return $page;
}

function during_create_prize($prize_guid,$challenge_guid,$contest_guid,$event_guid='',$winner_rank,$prize_amt){

	$pid = db_merge('dur_prize')
	->key(array('prize_guid'=>$prize_guid))
	->fields(array(
				   'challenge_guid' => $challenge_guid,
				   'contest_guid' => $contest_guid,
				   'event_guid' => $event_guid,
				   'winner_rank' => $winner_rank,
				   'prize_amt' => $prize_amt,
				   ))
	->execute();

}

//**********************************************************************
//***HOOK FORM***
//**********************************************************************
function during_contest_form($form, &$form_state){
	$form['contest_id'] = array(
	'#title' => t('Contest ID'),
	'#type' => 'hidden',
	'#access' => FALSE,
	'#required' => TRUE,
	);
    
    $form['contest_guid'] = array(
	'#title' => t('Contest GUID'),
	'#type' => 'hidden',
	'#access' => FALSE,
	'#required' => TRUE,
	);
		
	$form['contest_title'] = array(
	'#title' => t('Contest Name'),
	'#type' => 'textfield',
	'#access' => TRUE,
	'#size' => 45,
	'#maxlength' => 65,
	'#required' => TRUE,
	'#default_value' => '',
	);
    
    $form['contest_status'] = array(
	'#title' => t('Contest Status'),
	'#type' => 'select',
	'#options' => array(
		0 => t('Not Started'),
		1 => t('Closed'),
		2 => t('Complete'),
	),
	'#default_value' => 'Not Started',
	'#access' => FALSE,
	);
    
    $form['start_date'] = array(
	'#title' => t('Start Date'),
	'#description' => '',
	'#date_label_position' => '',
	'#type' => 'date_popup',
	'#date_format' => 'Y-m-d',
	'#date_year_range' => '-1:+2',
	'#required' => TRUE,
	'#access' => TRUE,
	);
	
	$form['recurring'] = array(
    '#type' => 'checkbox', 
    '#title' => t('Recurring Events?'),
	'#default_value' => 1,
	);

	$form['frequency'] = array(
	'#title' => t('Frequency'),
	'#type' => 'textfield',
	'#required' => TRUE,
	'#access' => TRUE,
	'#size' => 10,
	'#maxlength' => 10,
	'#default_value' => 1,
	);
	
	$form['units'] = array(
	'#title' => t('Units'),
	'#type' => 'select',
	'#options' => array(
		0 => t('Day(s)'),
		1 => t('Week(s)'),
		2 => t('Month(s)'),
	),
	'#required' => TRUE,
	'#access' => TRUE,
	'#default_value' => 1,
	);
	
	$form['quantity'] = array(
	'#title' => t('Number of Events'),
	'#type' => 'textfield',
	'#required' => TRUE,
	'#access' => TRUE,
	'#size' => 10,
	'#maxlength' => 10,
	'#default_value' => 12,
	);
    
    $form['end_date'] = array(
	'#title' => t('End Date'),
	'#description' => '',
	'#date_label_position' => '',
	'#type' => 'date_popup',
	'#date_format' => 'Y-m-d',
	'#date_year_range' => '-1:+2',
	'#required' => FALSE,
	'#access' => FALSE,
	);
    
    $form['submit'] = array(
	'#value' => t('Submit'),
	'#type' => 'submit',
	'#access' => TRUE,
	);
    
    $form['#attributes'] = array('AutoComplete' => 'off');

return $form;
}


//**********************************************************************
//***HOOK FORM ALTER***
//**********************************************************************
function during_form_during_contest_form_alter(&$form, &$form_state, $formid){
	if (!array_key_exists(1,$form_state['build_info']['args']))
  {
	$form_state['build_info']['args'][0] = 'new';
  }	
	
	$edit_type = $form_state['build_info']['args'][0];
	
	switch($edit_type){
		case 'new':
			
			break;
		
		
		case 'edit':
			//Get values from the form
			$contest_guid = $form_state['build_info']['args'][1];
			
			$results = db_select('dur_contest','c')
						->fields('c')
						->condition('contest_guid',$contest_guid,'=')
						->execute();
			$numRowCount = $results->rowCount();

			if($numRowCount != 1){
				$form_state['redirect'] = 'during/contests';
				form_set_error('Error: the form cannot be processed');
				drupal_set_message('The Contest was not found');
				drupal_goto($form_state['redirect']);
			}
			$contest = $results->fetchAssoc();
			
			$form['contest_id']['#access'] = FALSE;
			$form['contest_guid']['#access'] = FALSE;
			$form['contest_title']['#access'] = TRUE;
			$form['contest_status']['#access'] = FALSE;
			$form['start_date']['#access'] = FALSE;
			$form['recurring']['#access'] = FALSE;
			$form['frequency']['#access'] = FALSE;
			$form['units']['#access'] = FALSE;
			$form['quantity']['#access'] = FALSE;
			$form['end_date']['#access'] = FALSE;
			$form['submit']['#access'] = TRUE;
			
			//LOAD EXISTING VALUES
			$form['contest_id']['#default_value'] = $contest['contest_id'];
			$form['contest_guid']['#default_value'] = $contest['contest_guid'];
			$form['contest_title']['#default_value'] = $contest['contest_title'];

			break;
	}
	return $form;
}


//**********************************************************************
//***HOOK FORM VALIDATE***
//**********************************************************************
function during_contest_form_validate($form,&$form_state){
	$edit_type = $form_state['build_info']['args'][0];
	
	switch($edit_type){
		case 'new':
			//Errors regardless of edit_type
			if($form_state['values']['start_date'] >= $form_state['values']['end_date']){
			form_set_error('end_date',t('Error: end date must be after start date'));
		};
			//dsm('new form');
			break;
		case 'edit':
			//dsm('edit form');
			break;
		default:
			form_set_error('Error: the form cannot be processed');
	};
	
}




//**********************************************************************
//***HOOK FORM SUBMIT***
//**********************************************************************
function during_contest_form_submit($form,&$form_state){
	
	$edit_type = $form_state['build_info']['args'][0];
	
	switch($edit_type){
		case 'new':
			
			$contest_id = $form_state['values']['contest_id'];
			$contest_guid = $form_state['values']['contest_guid'];
			$title = $form_state['values']['contest_title'];
			$status = $form_state['values']['contest_status'];
			$start_date = $form_state['values']['start_date'];
			$recurring = $form_state['values']['recurring'];

			if($recurring == 1){
				$frequency = $form_state['values']['frequency'];
				$units = $form_state['values']['units'];
				$quantity = $form_state['values']['quantity'];
	
				switch($units){
					case 0:
						$dateAddType = ' days';
						$dateAddMultiplier = 1;
					break;

					case 1:
						$dateAddType = ' days';
						$dateAddMultiplier = 7;
					break;

					case 2:
						$dateAddType = ' months';
						$dateAddMultiplier = 1;
					break;
				};
	
				$end_date = format_date(strtotime($start_date . ' + ' . $frequency * $quantity * $dateAddMultiplier . $dateAddType),'custom','Y-m-d');
	
				};
	
			if(!$contest_guid){
				$contest_guid = during_random_guid('contest',16);
			};
	
			$contest = during_create_contest($contest_id,$contest_guid,$title,$status,$start_date,$end_date,$frequency,$units,$quantity);
			$new_id = Database::getConnection()->lastInsertId();
	
			module_load_include('inc','during','z_event');
			$event_initial_guid = during_random_guid('event',16);
			during_create_event(NULL,$event_initial_guid,$contest_guid,'Initial Weigh-in',$start_date,'pending','initial');

	
			if($recurring == 1){
				for($i=1;$i<$quantity;$i++){
					$event_guid = during_random_guid('event',16);
					$event_date = format_date(strtotime($start_date . ' + ' . $frequency * $i * $dateAddMultiplier . $dateAddType),'custom','Y-m-d');
					$event_title_no = $i;
					during_create_event(NULL,$event_guid,$contest_guid,'Event #' . $event_title_no,$event_date,'pending','standard');
				};
			};
			
			$event_final_guid = during_random_guid('event',16);
			during_create_event(NULL,$event_final_guid,$contest_guid,'Final Weigh-in',$end_date,'pending','final');
	
	
			$form_state['redirect'] = 'during/contest/view/' . $contest_guid;
	
			//Make Author admin of the contest
			global $user;
			$admin = $user->uid;
			during_add_admin('',$contest_guid,$admin,'admin','','');			
			break;
		case 'edit':
			dsm('save changed title');
			break;
	};
}


//CREATE DB FUNCTIONS TO CREATE OBJECTS
function during_create_contest($contest_id='',$contest_guid='',$title='',$status='',$start_date='',$end_date='',$frequency='',$units='',$quantity=''){
	if($contest_id == ''){
		$contest_id = NULL;
	};	
	$cid = db_merge('dur_contest')
	->key(array('contest_id' => $contest_id))
	->fields(array(
		'contest_guid' => $contest_guid,
		'contest_title' => $title,
		'contest_status' => $status,
		'start_date' => $start_date,
		'end_date' => $end_date,
		'frequency' => $frequency,
		'units' => $units,
		'quantity' => $quantity,
	))
	->execute();
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
			
			$block_content = t('No registered contestants');
			$i=1;
			while($record = $result->fetchAssoc()) {
				if($i>1){
				$block_content .= '</br>';
				}else{
				$block_content = '';
				};
				$block_content .= $i . ')  ' . $record['name'];
				$i++;
			};
            $block['content'] = $block_content;
			return $block;
}

function admin_contest_block_define($contest_guid){
			$block = array();

			global $user;
			$user_id = $user->uid;
			
			$admin = db_select('dur_access', 'r');
			$admin->join('dur_contest','c','c.contest_guid = r.contest_guid');
			$admin->fields('r', array('user_id'));
			$admin->fields('c', array('contest_status'));
			$admin->condition('c.contest_guid',$contest_guid,'=');
			$admin->condition('r.user_id',$user_id,'=');
			$admin->condition('r.access_type','admin','=');
			$admin_ex = $admin->execute();
			
			$numReg = $admin_ex->rowCount();
			$contest = $admin_ex->fetchAssoc();
			
			if($numReg){
			
			$block['subject'] = t('Admin Tasks');
			
			$content = l('Add Admins','during/admin/add/' . $contest_guid) . '</br>';
			if($contest['contest_status']=='Not Started'){
			$content .= l('Invite Contestants','during/invitation/add/'  . $contest_guid) . '</br>';
			$content .= l('Close Registration','during/contest/close/' . $contest_guid) . '</br>';
			$content .= l('Scoring Rules','during/scoring/edit/' . $contest_guid) . '</br>';
			};
			
			$block['content'] = $content;
			}
			return $block;
		
		
}