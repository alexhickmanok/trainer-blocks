<?php

function dur_contest_scoring_setup_page($edit_type,$contest_guid){
    $page = '';
	
	
	
	$header = array();
	$rows = array();
	$rows2 = array();
	
	//$header['challenge_guid'] = t('Challenge GUID');
	//$header['contest_guid'] = t('Contest GUID');
	
	$header['team_setting'] = t('Team Setting');
	$header['challenge_type'] = t('Challenge Type');
	$header['scoring_setting'] = t('Scoring Setting');
	$header['winnings_pct'] = t('Winnings %');
	$header['winnings_amt'] = t('Winnings $');
	$header['prize_setting'] = t('Prize Setting');
	$header['winnings_amt_per'] = t('Winnings $ per Event');
	$header['winners_from'] = t('Winners From');
	$header['winners_to'] = t('Winners To');
	
	$a=0;
	$b=0;
	$result = db_select('dur_challenge', 'c')
				->fields('c')
				->condition('contest_guid',$contest_guid,'=')
				->execute();
			while($record = $result->fetchAssoc()){
				if($record['team_setting'] == 0){
				
					$a++;

				//$rows[$a]['challenge_guid'] = $record['challenge_guid'];
				//$rows[$a]['contest_guid'] = $record['contest_guid'];
				
				$rows[$a]['team_setting'] = $record['team_setting'];
				$rows[$a]['challenge_type'] = $record['challenge_type'];
				$rows[$a]['scoring_setting'] = $record['scoring_setting'];
				$rows[$a]['winnings_pct'] = $record['winnings_pct'];
				$rows[$a]['winnings_amt'] = $record['winnings_amt'];
				$rows[$a]['prize_setting'] = $record['prize_setting'];
				$rows[$a]['winnings_amt_per'] = $record['winnings_amt_per'];
				$rows[$a]['winners_from'] = $record['winners_from'];
				$rows[$a]['winners_to'] = $record['winners_to'];
				}else{
				
					$b++;
				
				//$rows2[$b]['challenge_guid'] = $record['challenge_guid'];
				//$rows2[$b]['contest_guid'] = $record['contest_guid'];
				
				$rows2[$b]['team_setting'] = $record['team_setting'];
				$rows2[$b]['challenge_type'] = $record['challenge_type'];
				$rows2[$b]['scoring_setting'] = $record['scoring_setting'];
				$rows2[$b]['winnings_pct'] = $record['winnings_pct'];
				$rows2[$b]['winnings_amt'] = $record['winnings_amt'];
				$rows2[$b]['prize_setting'] = $record['prize_setting'];
				$rows2[$b]['winnings_amt_per'] = $record['winnings_amt_per'];
				$rows2[$b]['winners_from'] = $record['winners_from'];
				$rows2[$b]['winners_to'] = $record['winners_to'];
				};
			};
		
		$page .= '</br><h1>Individual Challenges</h1>';	
		$page .= theme('table', array(
		'header' => $header, 
		'rows' => $rows,
		'attributes' => array('width' => '100%')
));
		$page .= '<h1>Team Challenges</h1>';	
		$page .= theme('table', array(
		'header' => $header, 
		'rows' => $rows2,
		'attributes' => array('width' => '100%')
));
    return $page;
}

function dur_contest_scoring_block_define($contest_guid){
	$block['subject'] =  t('Add Scoring Rule');
	$block['content'] = drupal_get_form('dur_contest_challenge_form');
	return $block;
}

function dur_contest_challenge_form($form, &$form_state){
    $form['challenge_id'] = array(
	'#title' => t('Challenge ID'),
	'#type' => 'hidden',
	'#access' => FALSE,
	'#required' => TRUE,
	);
    
    $form['challenge_guid'] = array(
        '#title' => t('Challenge GUID'),
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
    $form['team_setting'] = array(
        '#title' => t('Team Setting'),
        '#type' => 'select',
        '#options' => array(
		0 => t('Individual'),
		1 => t('Team')),
        '#access' => TRUE,
        '#required' => FALSE,
    );
    $form['scoring_setting'] = array(
        '#title' => t('Measurement Setting'),
        '#type' => 'select',
        '#options' => array(
		0 => t('% Weight Lost'),
		1 => t('Lbs Lost'),
		2 => t('Body Fat % Lost'),
		3 => t('Inches Lost')),
        '#access' => TRUE,
        '#required' => FALSE,
    ); 
    $form['challenge_type'] = array(
        '#title' => t('Challenge Type'),
        '#type' => 'select',
        '#options' => array(
		0 => t('Contest'),
		1 => t('Event'),
        2 => t('Head-to-Head'),
		3 => t('Tournament')),
        '#access' => TRUE,
        '#required' => FALSE,
		'#ajax' => array(
			'callback' => 'dur_contest_challenge_form_callback',
			'wrapper' => 'eventsshow',
		)
	);
	
	    $form['prize_setting'] = array(
        '#title' => t('Payout Settings'),
        '#type' => 'select',
        '#options' => array(
		0 => t('Winner Take All'),
		1 => t('Range of Winners'),
        2 => t('Top % of Contestants')),
        '#access' => TRUE,
        '#required' => FALSE,
	);
		$myVal = FALSE;
		if(!empty($form_state['values']['prize_setting']) && $form_state['values']['prize_setting'] != 0){
			$myVal = TRUE;
		};
		$form['winner_count_from'] = array(
		'#type' => 'textfield',
		'#access' => TRUE,
		'#title' => t('Winners From:'),
		);
		$form['winner_count_to'] = array(
		'#type' => 'textfield',
		'#access' => TRUE,
		'#title' => t('To:'),
	);
		$form['winnings_type'] = array(
        '#title' => t('Payout Amount'),
        '#type' => 'select',
        '#options' => array(
		0 => t('Percent of Winnings'),
		1 => t('$ Amount')),
		'#access' => TRUE,
        '#required' => FALSE,
	);
		$form['winnings_amt'] = array(
        '#type' => 'textfield',
		'#access' => TRUE,
        '#required' => FALSE,
	);
	
	$form['submit'] = array(
	'#value' => t('Submit'),
	'#type' => 'submit',
	'#access' => TRUE,
	);

    return $form;

}

function dur_contest_challenge_form_validate($form, &$form_state){

}

function dur_contest_challenge_form_submit($form, &$form_state){
	module_load_include('module','during','during');
	$challenge_guid = during_random_guid('challenge',16);
	$contest_guid = arg(3);
	$team_setting = $form_state['values']['team_setting'];
	$challenge_type = $form_state['values']['challenge_type'];
	$scoring_setting = $form_state['values']['scoring_setting'];
	$winnings_type = $form_state['values']['winnings_type'];
	$winnings_value = $form_state['values']['winnings_amt'];
	if($winnings_type == 0){
		$winnings_pct = (float) $winnings_value;
		$winnings_amt = (float) $winnings_value * 1;  //replace with Contest Total Payout Amount
	} else {
		$winnings_pct = (float) $winnings_value / 1;  //replace with Contest Total Payout Amount
		$winnings_amt = (float) $winnings_value;
	}
	$prize_setting = $form_state['values']['prize_setting'];
	if($prize_setting == 0){
		$winners_from = 1;
		$winners_to = 1;
	}else{
	$winners_from = $form_state['values']['winner_count_from'];
	$winners_to = $form_state['values']['winner_count_to'];
	}
	$winnings_amt_per = 0;//$form_state['values']['winnings_amt_per'];

	dur_challenge_create($challenge_guid,$contest_guid,$team_setting,$challenge_type,$scoring_setting,$winnings_pct,$winnings_amt,$prize_setting,$winnings_amt_per,$winners_from,$winners_to);
}


function dur_challenge_create($challenge_guid,$contest_guid,$team_setting,$challenge_type,$scoring_setting,$winnings_pct=0,$winnings_amt=0,$prize_setting,$winnings_amt_per,$winners_from,$winners_to){
	$new_challenge = db_merge('dur_challenge')
					->key(array('challenge_guid' => $challenge_guid))
					->fields(array(
						'contest_guid' => $contest_guid,
						'team_setting' => $team_setting,
						'challenge_type' => $challenge_type,
						'scoring_setting' => $scoring_setting,
						'winnings_pct' => $winnings_pct,
						'winnings_amt' => $winnings_amt,
						'prize_setting' => $prize_setting,
						'winnings_amt_per' => $winnings_amt_per,
						'winners_from' => $winners_from,
						'winners_to' => $winners_to,
					))
					->execute();	
}

function dur_contest_challenge_form_callback($form, $form_state){
	$new_state = array();
	$new_state['build_info'] = $form_state['build_info'];
	$new_state['rebuild'] = TRUE;
	$new_state['values'] = array();
	$new_state += form_state_defaults();

	$new_form_array = drupal_rebuild_form('dur_contest_challenge_form', $new_state);
	$new_form = drupal_render($new_form_array);

	$commands = array();
	$commands[] = ajax_command_html('eventsshow', $new_form);
  return array('#type' => 'ajax', '#commands' => $commands);
}

function during_event_prize($event_guid){
	$event_guid_hold = $event_guid;
	$event_prizes = db_select('dur_prize', 'p');
	$event_prizes->join('dur_challenge','c','p.challenge_guid = c.challenge_guid');
	$event_prizes->join('dur_event','e','p.event_guid = e.event_guid');
	$event_prizes->fields('p');
	$event_prizes->fields('c');
	$event_prizes->condition('e.event_guid',$event_guid,'=');
	$event_prizes->condition('c.challenge_type',array(1,2,3),'IN');
	$event_prizes->orderBy('p.winner_rank','ASC');
	$event_prize = $event_prizes->execute();
	//create payout amounts
	while($payout = $event_prize->fetchAssoc()){
		$event_guid = $event_guid_hold;

	switch ($payout['scoring_setting']){
		case 0:
			$score = 'pct_lost_eligible_rank';
			break;
		case 1:
			$score = 'lbs_lost_eligible_rank';
			break;
		case 2:	
			$score = 'bf_pct_lost_eligible_rank';
			break;
		case 3:
			$score = 'measure_lost_eligible_rank';
			break;
	}

	$payout_qry = db_select('dur_event_score','s')
					->fields('s')
					//->condition($score,$payout['winner_rank'],'=')
					->condition('event_guid',$event_guid,'=')
					->orderBy($score,'ASC')
					->range($payout['winner_rank'],1)
					->execute();
					$winners = $payout_qry->rowCount();

		while($pay = $payout_qry->fetchAssoc()){
		
		module_load_include('module','during','during');
		$payout_guid = during_random_guid('payout',16);
		$prize_guid = $payout['prize_guid'];
		$challenge_guid = $payout['challenge_guid'];
		$contest_guid = $payout['contest_guid'];
		$event_guid = $event_guid;
		$contestant_id = $pay['contestant_id'];
		$winner_rank = $payout['winner_rank'];
		$payout_amt = $payout['prize_amt'] / $winners;
		during_create_payout($payout_guid,$prize_guid,$challenge_guid,$contest_guid,$event_guid,$contestant_id,$winner_rank,$payout_amt);
		};
	};
}

function during_create_payout($payout_guid,$prize_guid,$challenge_guid,$contest_guid,$event_guid='',$contestant_id,$winner_rank,$payout_amt){
		$new_payout = db_merge('dur_payout')
					->key(array('payout_guid'=>$payout_guid))
					->fields(array(
						'prize_guid' => $prize_guid,
						'challenge_guid' => $challenge_guid,
						'contest_guid' => $contest_guid,
						'event_guid' => $event_guid,
						'contestant_id' => $contestant_id,
						'winner_rank' => $winner_rank,
						'payout_amt' => $payout_amt,
					))
					->execute();
}