<?php

//VIEW RESULTS FOR BY EVENT
function during_scores_page($event_guid){
		$page = '';
		$page = 'hello world';
		return $page;
}





function during_score_event_page($event_guid){

$event = db_select('dur_event', 'n')
			->fields('n')
			->condition('n.event_guid', $event_guid, '=')
			->execute()
			->fetchAssoc();
			
$contest_guid = $event['contest_guid'];

if($event['event_type'] != 'initial' && $event['event_status'] == 'active'){

		$result = db_select('dur_result', 'n');
		$result->join('dur_event', 'e', 'n.event_guid = e.event_guid');
		$result->fields('n');
		$result->fields('e', array('event_date'));
		$result->condition('n.event_guid',$event_guid,'=');
		$result->distinct();
		$results = $result->execute();

		
$scores = array();
$sid = 0;
	 while($record = $results->fetchAssoc()) {

		$sid++;
		$scores[$sid] = $record;

// GET RESULTS FROM INITIAL EVENT
$start = db_select('dur_result', 'n');
		$start->join('dur_event','e', 'n.event_guid = e.event_guid');
		$start->fields('n', array('result_weight','contestant_id'));
		$start->addExpression('min(n.result_weight)','start_weight');
		$start->addExpression('min(n.measurement_total)','measure_start');
		$start->addExpression('min(n.body_fat_pct)','bf_pct_start');
		$start->condition('n.contest_guid',$record['contest_guid'],'=');
		$start->condition('e.event_type','initial','=');
		$start->condition('n.contestant_id',$record['contestant_id'],'=');
		$start->distinct();
		//$min->condition('n.result_weight','NOT NULL');
		$start_val = $start->execute();

		while($start_weight = $start_val->fetchAssoc()){
		$scores[$sid]['start_weight'] = $start_weight['start_weight'];
		$scores[$sid]['measure_start'] = $start_weight['measure_start'];
		$scores[$sid]['bf_pct_start'] = $start_weight['bf_pct_start'];
		};
		
		

// LOGIC TO SCORE LBS_LOST AND %_LBS_LOST		
		$min_lbs = db_select('dur_result', 'n');
		$min_lbs->join('dur_event','e', 'n.event_guid = e.event_guid');
		$min_lbs->fields('n', array('result_weight','contestant_id','eligible'));
		$min_lbs->addExpression('min(n.result_weight)','min_weight');
		$min_lbs->condition('n.contest_guid',$record['contest_guid'],'=');
		$min_lbs->condition('e.event_date',$record['event_date'],'<');
		$min_lbs->condition('n.contestant_id',$record['contestant_id'],'=');
		$min_lbs->distinct();
		$min_lbs_val = $min_lbs->execute();

		while($min_weight = $min_lbs_val->fetchAssoc()){

		$scores[$sid]['min_prev_weight'] = $min_weight['min_weight'];
		};

		
		
		if($scores[$sid]['min_prev_weight'] > 0 && $scores[$sid]['min_prev_weight'] > $scores[$sid]['result_weight']){
		$scores[$sid]['scored_weight'] = $scores[$sid]['result_weight'];
		$scores[$sid]['lbs_lost'] = $scores[$sid]['min_prev_weight'] - $scores[$sid]['result_weight'];
		$scores[$sid]['pct_lost'] = ($scores[$sid]['min_prev_weight'] - $scores[$sid]['result_weight'])/$scores[$sid]['min_prev_weight'];
		} else {
		$scores[$sid]['scored_weight'] = $scores[$sid]['min_prev_weight'];
		$scores[$sid]['lbs_lost'] = 0;
		$scores[$sid]['pct_lost'] = 0;
		}
		
		if($scores[$sid]['start_weight'] > 0 && $scores[$sid]['start_weight'] > $scores[$sid]['result_weight']){
		$scores[$sid]['lbs_lost_td'] = $scores[$sid]['start_weight'] - $scores[$sid]['result_weight'];
		$scores[$sid]['pct_lost_td'] = ($scores[$sid]['start_weight'] - $scores[$sid]['result_weight'])/$scores[$sid]['start_weight'];
		} else {
		$scores[$sid]['lbs_lost_td'] = 0;
		$scores[$sid]['pct_lost_td'] = 0;
		}
		if($scores[$sid]['min_prev_weight'] > 0 && $scores[$sid]['min_prev_weight'] > $scores[$sid]['result_weight'] && $scores[$sid]['eligible'] == 1){
		$scores[$sid]['lbs_lost_eligible'] = $scores[$sid]['min_prev_weight'] - $scores[$sid]['result_weight'];
		$scores[$sid]['pct_lost_eligible'] = ($scores[$sid]['min_prev_weight'] - $scores[$sid]['result_weight'])/$scores[$sid]['min_prev_weight'];
		} else {
		$scores[$sid]['lbs_lost_eligible'] = NULL;
		$scores[$sid]['pct_lost_eligible'] = NULL;
		}

		
// LOGIC TO SCORE TOTAL MEASUREMENTS LOST		
		$min_mes = db_select('dur_result', 'n');
		$min_mes->join('dur_event','e', 'n.event_guid = e.event_guid');
		$min_mes->fields('n', array('measurement_total','contestant_id','eligible'));
		$min_mes->addExpression('min(n.measurement_total)','measure_min_prev');
		$min_mes->condition('n.contest_guid',$record['contest_guid'],'=');
		$min_mes->condition('e.event_date',$record['event_date'],'<');
		$min_mes->condition('n.contestant_id',$record['contestant_id'],'=');
		$min_mes->distinct();
		$min_mes_val = $min_mes->execute();

		while($min_measure = $min_mes_val->fetchAssoc()){
				$scores[$sid]['measure_min_prev'] = $min_measure['measurement_total'];
		}
		
		//  $record = details from the current event results
		//  $min_measure = details from the lowest results
		if($scores[$sid]['measure_min_prev'] > 0 && $scores[$sid]['measure_min_prev'] > $scores[$sid]['measurement_total']){
		$scores[$sid]['measure_scored'] = $scores[$sid]['measurement_total'];
		$scores[$sid]['measure_lost'] = $scores[$sid]['measure_min_prev'] - $scores[$sid]['measurement_total'];
		} else {
		$scores[$sid]['measure_scored'] = $scores[$sid]['measure_min_prev'];
		$scores[$sid]['measure_lost'] = 0;
		}
		
		if($scores[$sid]['measure_start'] > 0 && $scores[$sid]['measure_start'] > $scores[$sid]['measurement_total']){
		$scores[$sid]['measure_lost_td'] = $scores[$sid]['measure_start'] - $scores[$sid]['measurement_total'];
		} else {
		$scores[$sid]['measure_lost_td'] = 0;
		}
		if($scores[$sid]['measure_min_prev'] > 0 && $scores[$sid]['measure_min_prev'] > $scores[$sid]['measurement_total'] && $scores[$sid]['eligible'] == 1){
		$scores[$sid]['measure_lost_eligible'] = $scores[$sid]['measure_min_prev'] - $scores[$sid]['measurement_total'];
		} else {
		$scores[$sid]['measure_lost_eligible'] = NULL;
		}
		
		
// LOGIC TO SCORE BODY FAT % LOST		
		$min_bf = db_select('dur_result', 'n');
		$min_bf->join('dur_event','e', 'n.event_guid = e.event_guid');
		$min_bf->fields('n', array('body_fat_pct','contestant_id','eligible'));
		$min_bf->addExpression('min(n.body_fat_pct)','bf_pct_min_prev');
		$min_bf->condition('n.contest_guid',$record['contest_guid'],'=');
		$min_bf->condition('e.event_date',$record['event_date'],'<');
		$min_bf->condition('n.contestant_id',$record['contestant_id'],'=');
		$min_bf->distinct();
		$min_bf_val = $min_bf->execute();

		while($min_bf_pct = $min_bf_val->fetchAssoc()){
		$scores[$sid]['bf_pct_min_prev'] = $min_bf_pct['body_fat_pct'];
		}
		//  $record = details from the current event results
		//  $min_bf_pct = details from the lowest results

		if($scores[$sid]['bf_pct_min_prev'] > 0 && $scores[$sid]['bf_pct_min_prev'] > $scores[$sid]['body_fat_pct']){
		$scores[$sid]['bf_pct_scored'] = $scores[$sid]['body_fat_pct'];
		$scores[$sid]['bf_pct_lost'] = $scores[$sid]['bf_pct_min_prev'] - $scores[$sid]['body_fat_pct'];
		} else {
		$scores[$sid]['bf_pct_scored'] = $scores[$sid]['bf_pct_min_prev'];
		$scores[$sid]['bf_pct_lost'] = 0;
		}
		
		if($scores[$sid]['bf_pct_start'] > 0 && $scores[$sid]['bf_pct_start'] > $scores[$sid]['body_fat_pct']){
		$scores[$sid]['bf_pct_lost_td'] = $scores[$sid]['bf_pct_start'] - $scores[$sid]['body_fat_pct'];
		} else {
		$scores[$sid]['bf_pct_lost_td'] = 0;
		}
		if($scores[$sid]['bf_pct_min_prev'] > 0 && $scores[$sid]['bf_pct_min_prev'] > $scores[$sid]['body_fat_pct'] && $scores[$sid]['eligible'] == 1){
		$scores[$sid]['bf_pct_lost_eligible'] = $scores[$sid]['bf_pct_min_prev'] - $scores[$sid]['body_fat_pct'];
		} else {
		$scores[$sid]['bf_pct_lost_eligible'] = NULL;
		}


//END LOGIC FOR THIS RESULT
}
// GENERATE RANKED RESULTS

		$scores = during_finish_scores($scores);
		
	foreach($scores as $key=>$val){
		$scored = db_insert('dur_event_score')
		->fields(array(
			'contest_guid' => $val['contest_guid'],
			'event_guid' => $val['event_guid'],
			'result_guid' => $val['result_guid'],
			'contestant_id' => $val['contestant_id'],
			'eligible' => $val['eligible'],
			'start_weight' => $val['start_weight'],
			'min_prev_weight' => $val['min_prev_weight'],
			'scored_weight' => $val['scored_weight'],
			'lbs_lost' => $val['lbs_lost'],
			'lbs_lost_raw_rank' => $val['lbs_lost_raw_rank'],
		    'lbs_lost_eligible_rank' => $val['lbs_lost_eligible_rank'],
			'lbs_lost_td' => $val['lbs_lost_td'],
			'lbs_lost_td_raw_rank' => $val['lbs_lost_td_raw_rank'],
			'pct_lost' => $val['pct_lost'],
		    'pct_lost_raw_rank' => $val['pct_lost_raw_rank'],
		    'pct_lost_eligible_rank' => $val['pct_lost_eligible_rank'],
			'pct_lost_td' => $val['pct_lost_td'],
			'pct_lost_td_raw_rank' => $val['pct_lost_td_raw_rank'],
			'measure_start' => $val['measure_start'],
			'measure_min_prev' => $val['measure_min_prev'],
			'measure_scored' => $val['measure_scored'],
		    'measure_lost' => $val['measure_lost'],
		    'measure_lost_raw_rank' => $val['measure_lost_raw_rank'],
		    'measure_lost_eligible_rank' => $val['measure_lost_eligible_rank'],
			'measure_lost_td' => $val['measure_lost_td'],
			'measure_lost_td_raw_rank' => $val['measure_lost_td_raw_rank'],
			'bf_pct_start' => $val['bf_pct_start'],
			'bf_pct_min_prev' => $val['bf_pct_min_prev'],
			'bf_pct_scored' => $val['bf_pct_scored'],
			'bf_pct_lost' => $val['bf_pct_lost'],
		    'bf_pct_lost_raw_rank' => $val['bf_pct_lost_raw_rank'],
		    'bf_pct_lost_eligible_rank' => $val['bf_pct_lost_eligible_rank'],
			'bf_pct_lost_td' => $val['bf_pct_lost_td'],
			'bf_pct_lost_td_raw_rank' => $val['bf_pct_lost_td_raw_rank'],			
		))
		->execute();
	}


$event_updated = db_update('dur_event') // Table name no longer needs {}
  ->fields(array(
    'event_status' => 'closed',
  ))
  ->condition('event_guid', $event_guid, '=')
  ->execute();
 
  		module_load_include('inc','during','z_scoring');
		during_event_prize($event_guid);

		

if($event['event_type'] != 'final'){	
	$next_event = db_select('dur_event', 'e')
		->fields('e', array('event_guid'))
		->condition('e.event_date', $event['event_date'],'>')
		->condition('e.contest_guid', $contest_guid, '=')
		->orderBy('e.event_date')
		->execute();
	$next_event_get = $next_event->fetchAssoc();
	$next_event_guid = $next_event_get['event_guid'];
	
$next_event_updated = db_update('dur_event') // Table name no longer needs {}
  ->fields(array(
    'event_status' => 'active',
  ))
  ->condition('event_guid', $next_event_guid, '=')
  ->execute();
}
	
		drupal_set_message('event ' . $event_guid . ' has been scored');
		drupal_goto('during/event/view/' . $event_guid);
}else{

$event_updated = db_update('dur_event') // Table name no longer needs {}
  ->fields(array(
    'event_status' => 'closed',
  ))
  ->condition('event_guid', $event_guid, '=')
  ->execute();
  
  
	if($event['event_type'] != 'final'){	
	$next_event = db_select('dur_event', 'e')
		->fields('e', array('event_guid'))
		//->addExpression('min(e.event_date)','next_event_date')
		->condition('e.event_date', $event['event_date'],'>')
		->condition('e.contest_guid', $contest_guid, '=')
		->orderBy('e.event_date')
		->execute();
	$next_event_get = $next_event->fetchAssoc();
	$next_event_guid = $next_event_get['event_guid'];
	
$next_event_updated = db_update('dur_event') // Table name no longer needs {}
  ->fields(array(
    'event_status' => 'active',
  ))
  ->condition('event_guid', $next_event_guid, '=')
  ->execute();
}


}

}


function during_finish_scores($scores){

		$scores = during_rank_results($scores,'pct_lost','pct_lost_raw_rank');
		$scores = during_rank_results($scores,'lbs_lost','lbs_lost_raw_rank');
		$scores = during_rank_results($scores,'pct_lost_td','pct_lost_td_raw_rank');
		$scores = during_rank_results($scores,'lbs_lost_td','lbs_lost_td_raw_rank');
		$scores = during_rank_results($scores,'pct_lost_eligible','pct_lost_eligible_rank');
		$scores = during_rank_results($scores,'lbs_lost_eligible','lbs_lost_eligible_rank');
		$scores = during_rank_results($scores,'measure_lost','measure_lost_raw_rank');
		$scores = during_rank_results($scores,'measure_lost_td','measure_lost_td_raw_rank');
		$scores = during_rank_results($scores,'measure_lost_eligible','measure_lost_eligible_rank');
		$scores = during_rank_results($scores,'bf_pct_lost','bf_pct_lost_raw_rank');
		$scores = during_rank_results($scores,'bf_pct_lost_td','bf_pct_lost_td_raw_rank');
		$scores = during_rank_results($scores,'bf_pct_lost_eligible','bf_pct_lost_eligible_rank');
		return $scores;
		
}
function during_rank_results($arr,$res_val,$res_label){
// GET RANKINGS FOR EACH SCORE TYPE
		$score_out = array();
		$score_out = array_column($arr,$res_val);
		arsort($score_out);

		$s=0;
		$prior_key = '';
		foreach($score_out as $key=>$val){
			$key_Val = $key+1;
			$s++;
			if(empty($prior_key)){
				$rank = $s;
			} else if($val != $score_out[$prior_key-1]){
				$rank = $s;
			} else {
				$rank = $arr[$prior_key][$res_label];
			}
			$arr[$key_Val][$res_label] = $rank;
		
			$prior_key = $key_Val;
		}
return $arr;		
}	
