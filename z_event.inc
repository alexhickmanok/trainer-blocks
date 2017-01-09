<?php
function during_event_page($event_guid){

	$page = '';
	
    $results = db_select('dur_result', 'n');
	$results->leftJoin('dur_event_score', 's', 'n.result_guid = s.result_guid');
	$results->leftJoin('dur_event', 'e', 'n.event_guid = e.event_guid');
	$results->leftJoin('users','u','n.contestant_id = u.uid');
	$results->fields('n', array());
	$results->fields('s', array('min_prev_weight','lbs_lost','pct_lost_eligible_rank'));
	$results->fields('e', array('event_status','contest_guid','event_type'));
	$results->fields('u', array('name'));
	$results->condition('n.event_guid', arg(3),'=');
    $results->orderBy('n.contestant_id', 'ASC');
	$result = $results->execute();

    $r=0;

    $rows = array();
        while($record = $result->fetchAssoc()) {
	
	
	if($r==0){
		$event_type = $record['event_type'];
		if($event_type == 'initial'){
			$header = array(
			'user_name' => t('User Name'),
			'eligible' => t('Eligible'),
			'result_weight' => t('Result Weight'),
			);			
		}else{
			$header = array(
			'user_name' => t('User Name'),
			'eligible' => t('Eligible'),
			'result_weight' => t('Result Weight'),
			'min_prev_weight' => t('Min. Prev. Weight'),
			'lbs_lost' => t('Lbs Lost'),
			'eligible_rank' => t('Event Rank'),
			);
		}
	}
	$r++;
	$rows[$r] = array();
		$event_status = $record['event_status'];
		$contest_guid = $record['contest_guid'];
		
	if($event_status == 'active'){
		
		$rows[$r]['user_name'] = l(t($record['name']),'during/result/edit/' . $record['result_guid']);
	}else{
		$rows[$r]['user_name'] = t($record['name']);
	}
	
	$rows[$r]['eligible'] = $record['eligible'];
	$rows[$r]['result_weight'] = $record['result_weight'];
    
	if($event_type != 'initial'){
	$rows[$r]['min_prev_weight'] = $record['min_prev_weight'];
	$rows[$r]['lbs_lost'] = t($record['lbs_lost']);
	$rows[$r]['pct_lost_eligible_rank'] = t($record['pct_lost_eligible_rank']);
	};
	
	};
		
	if($event_status == 'active'){
		$form_button = "<form><button type='submit' formaction='/during/event/close/" . $event_guid . "'>Close Event</button></form>";
		$page .= $form_button;
		$page .= "</br>";
	};
$page .= theme('table', array(
    'header' => $header, 
    'rows' => $rows,
    'attributes' => array('width' => '100%')
));
//dsm('user random name: ' . generateRandomString());
// Build Breadcrumbs
$breadcrumb = array();
$breadcrumb[] = l(t('Home'), '<front>');
$breadcrumb[] = l(t('Contest'), 'during/contest/view/' . $contest_guid);
$breadcrumb[] = t('Event');
// Set Breadcrumbs
drupal_set_breadcrumb($breadcrumb);
	return $page;
}


function during_create_event($event_id='',$event_guid,$contest_guid,$title='',$date='',$status='',$event_type=''){
	if($event_id == ''){
		$event_id = NULL;
	};
	$cid = db_merge('dur_event')
	->key(array('event_id' => $event_id))
	->fields(array(
		'event_guid' => $event_guid,
		'contest_guid' => $contest_guid,
		'event_title' => $title,
		'event_date' => $date,
		'event_status' => $status,
		'event_type' => $event_type,
	))
	->execute();
}

function during_activate_event_page($event_guid=''){
	
	if($event_guid == ''){
		$event_guid = arg(2);
	}
		
$result = db_select('dur_registration', 'n');
		$result->join('dur_contest', 'c', 'n.contest_guid = c.contest_guid');
		$result->join('dur_event', 'e', 'c.contest_guid = e.contest_guid');
		$result->fields('n', array('contestant_id'));
		$result->fields('c', array('contest_guid'));
		$result->fields('e', array('event_guid'));
		$result->condition('e.event_guid',$event_guid,'=');
		$result->distinct();
		$results = $result->execute();

	 while($record = $results->fetchAssoc()) {
	
	module_load_include('module','during','during');
	$result_guid = during_random_guid('result',16);
	module_load_include('inc','during','z_result');
	during_create_result(NULL,$result_guid,$record['contestant_id'],$record['contest_guid'],$record['event_guid'],0,NULL,NULL,NULL,NULL,NULL,NULL,1);
}


return 'finished';
}