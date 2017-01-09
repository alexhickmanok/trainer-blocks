<?php

function dur_comments_block_define($item_guid){
	$block['subject'] = t('Comments');
	$block['content'] = 'TODO:  REPLACE WITH TABLE OF COMMENTS';
	
	$comments = db_select('dur_comments', 'c')
	->fields('c', array('comment'))
	->condition('reference_guid',$item_guid,'=')
	->execute();
	$comments_get = $comments->fetchAll();
	$comments_return = '<ul>';
	foreach($comments_get as $comment){
		$comments_return .= '<li>' . $comment->comment . '</li>';
	}
	$comments_return .='</ul>';
	$block['content'] = $comments_return;
	return $block;
}
function dur_comment_form_block_define($item_guid){
	$block['subject'] = '';
	$block['content'] = drupal_get_form('during_comments_form',$item_guid);
	return $block;
}

function dur_recent_comments_block_define($item_guid){
	$block['subject'] = t('Recent Comments');
	
	$comments = db_select('dur_comments', 'c')
				->fields('c',array('comment'))
				->condition('contest_guid',$item_guid,'=')
				->orderBy('timestamp','ASC')
				->range(0,3)
				->execute();
	$comments_get = $comments->fetchAll();
	$comments_return = '<ul>';
	foreach($comments_get as $comment){
		$comments_return .= '<li>' . $comment->comment . '</li>';
	}
	$comments_return .='</ul>';
	$block['content'] = $comments_return;
	return $block;
}

function during_comments_form($form, &$form_state, $item_guid){    
   	global $user;
	
	$form['comment_id'] = array(
	'#title' => t('Comment ID'),
	'#type' => 'hidden',
	'#access' => FALSE,
	'#required' => TRUE,
	);
    
    $form['comment_guid'] = array(
	'#title' => t('Comment GUID'),
	'#type' => 'hidden',
	'#access' => FALSE,
	'#required' => TRUE,
	);
		
	$form['reference_type'] = array(
	'#title' => t('Reference Type'),
	'#type' => 'textfield',
	'#access' => FALSE,
	'#size' => 45,
	'#maxlength' => 65,
	'#required' => TRUE,
	'#default_value' => arg(1),
	);

    $form['reference_guid'] = array(
	'#title' => t('Reference GUID'),
	'#type' => 'hidden',
	'#access' => FALSE,
	'#required' => TRUE,
	'#default_value' => $item_guid,
	);
    
    $form['user_id'] = array(
	'#title' => t('User ID'),
	'#type' => 'hidden',
	'#access' => FALSE,
	'#required' => TRUE,
	'#default_value' => $user->uid,
	);

    $form['comment'] = array(
	'#type' => 'textarea',
    '#cols' => 10,
	'#empty' => t('Comment'),
    '#width' => 15,
    '#access' => TRUE,
	'#required' => FALSE,
	);
    
    $form['parent_comment_guid'] = array(
	'#title' => t('Reference GUID'),
	'#type' => 'hidden',
	'#access' => FALSE,
	'#required' => TRUE,
	);
    
    $form['tags'] = array(
	'#title' => t('Tags'),
	'#type' => 'textfield',
    '#size' => 25,
	'#access' => TRUE,
	'#required' => FALSE,
	);

    $form['submit'] = array(
	'#value' => t('Submit'),
	'#type' => 'submit',
	'#access' => TRUE,
	);
    
    $form['#attributes'] = array('AutoComplete' => 'off');

return $form;
}

function during_comments_form_validate($form,&$form_state){
    
}

function during_comments_form_submit($form,&$form_state){
	$values = $form_state['values'];
	
	
	module_load_include('module','during','during');
	$comment_guid = during_random_guid('comment',16);
	$reference_type = $values['reference_type'];
	$reference_guid = $values['reference_guid'];
		if($reference_type == 'event'){
		$contest = db_select('dur_event', 'e')
					->fields('e', array('contest_guid'))
					->condition('event_guid',$reference_guid,'=')
					->execute();
		$contest_get = $contest->fetchAssoc();
		$contest_guid = $contest_get['contest_guid'];
		}else{
			$contest_guid = $reference_guid;
		}
	$user_id = $values['user_id'];
	$comment = $values['comment'];
	$parent_comment_guid = $values['parent_comment_guid'];
	$tags = $values['tags'];
	
	during_comments_create($comment_guid,$reference_type,$reference_guid,$user_id,$comment,$parent_comment_guid,$tags,$contest_guid);
}

function during_comments_create($comment_guid,$reference_type,$reference_guid,$user_id, $comment, $parent_comment_guid, $tags,$contest_guid){
	$datetime=date_create();
	
	$comment = db_merge('dur_comments')
				->key(array('comment_guid' => $comment_guid))
				->fields(array(
				'reference_type' => $reference_type,
				'reference_guid' => $reference_guid,
				'user_id' => $user_id,
				'comment' => $comment,
				'parent_comment_guid' => $parent_comment_guid,
				'tags' => $tags,
				//'timestamp' => to_string($datetime),
				'contest_guid' => $contest_guid,
				))
				->execute();
}