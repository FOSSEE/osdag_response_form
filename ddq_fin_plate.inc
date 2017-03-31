<?php
/**
 * Fetches and displays the form.
 */
function ddq_fin_plate_form_page()
{
	global $user;
	$data = '';
	if (!$user)
	{
		$data .= "Please login to give feedback";
	} //!$user
	/************* check if user has already submitted a proposal **************/
	$query = db_select('ddq_fin_plate_form_step1_feedback');
	$query->fields('ddq_fin_plate_form_step1_feedback');
	$query->condition('uid', $user->uid);
	$query->orderBy('id', 'DESC');
	$query->range(0, 1);
	$feedback_q = $query->execute();
	if ($feedback_q)
	{
		if ($feedback_data = $feedback_q->fetchObject())
		{
			if ($feedback_data->submitted == 1)
			{
				$data .= drupal_set_message(t('You have already submitted the feedback.'), 'status');
				$data .= drupal_goto('');
				return;
			} //$feedback_data->submitted == 1
			else
			{
				//$data .= drupal_goto('ddq-fin-plate/form/step1/'.$user->uid);
				$ddq_fin_plate_step1_form = drupal_get_form("ddq_fin_plate_step1_form");
				$data .= drupal_render($ddq_fin_plate_step1_form);
			}
		} //$feedback_data = $feedback_q->fetchObject()
		else
		{
			$ddq_fin_plate_step1_form = drupal_get_form("ddq_fin_plate_step1_form");
			$data .= drupal_render($ddq_fin_plate_step1_form);
		}
	} //$feedback_q
	//$data .= drupal_get_form('ddq_fin_plate_step1_form');
	return $data;
}
/**
 * Displays simple confirmation page after finishing the whole process.
 */
function ddq_fin_plate_success()
{
	$data .= drupal_set_message(t('You have successfully submitted the feedback.'), 'status');
	$data .= drupal_goto('home');
	return $data;
}
/**
 * Generates the form.
 */
function ddq_fin_plate_step1_form($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	global $base_url;
	// All the real form fields.
	$query = db_select('ddq_fin_plate_form_step1_feedback');
	$query->fields('ddq_fin_plate_form_step1_feedback');
	$query->condition('uid', $user->uid);
	$form_step1_data = $query->execute();
	if ($form_step1_data)
	{
		if ($step1_data = $form_step1_data->fetchObject())
		{
			/* everything ok */
		} //$step1_data = $form_step1_data->fetchObject()
		/*else
		{
		drupal_set_message(t('Invalid form selected. Please try again.'), 'error');
		//drupal_goto('lab_migration/manage_proposal');
		return;
		}*/
	} //$form_step1_data
	$form = array();
	$form['step1_field1_name'] = array(
		'#title' => 'Name',
		'#type' => 'textfield',
		'#default_value' => $step1_data->name,
		'#required' => TRUE
	);
	$form['step1_field2_email_id'] = array(
		'#type' => 'textfield',
		'#title' => t('Email'),
		'#size' => 30,
		'#value' => $user->mail,
		'#disabled' => TRUE,
		'#required' => TRUE
		//'#default_value' => $form_state['storage']['values'][1]['step1_field2_email_id']
	);
	$form['step1_field3_institute'] = array(
		'#title' => 'Institute / Company',
		'#type' => 'textfield',
		'#default_value' => $step1_data->institute,
		'#required' => TRUE
	);
	$form['step1_field4_current_position'] = array(
		'#title' => 'Current position',
		'#type' => 'textfield',
		'#default_value' => $step1_data->current_position,
		'#required' => TRUE
	);
	$form["step1_page_id"] = array(
		"#type" => "hidden",
		"#value" => "step1"
	);
	$form['next1'] = array(
		'#type' => 'submit',
		'#value' => 'Save & Next',
		'#submit' => array(
			'ddq_fin_plate_step1_form_submit'
		)
	);
	return $form;
}
function ddq_fin_plate_step1_form_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$query = db_select('ddq_fin_plate_form_step1_feedback');
	$query->fields('ddq_fin_plate_form_step1_feedback');
	$query->condition('uid', $user->uid);
	$form_step1_data = $query->execute();
	if ($form_step1_data)
	{
		if ($step1_data = $form_step1_data->fetchObject())
		{
			$query = "UPDATE ddq_fin_plate_form_step1_feedback SET name=:name, email_id=:email_id, institute=:institute, current_position=:current_position WHERE uid=:uid ";
			$args = array(
				":uid" => $user->uid,
				":name" => _field_sentence_case($v['step1_field1_name']),
				":email_id" => $v['step1_field2_email_id'],
				":institute" => _field_sentence_case($v['step1_field3_institute']),
				":current_position" => _field_sentence_case($v['step1_field4_current_position'])
			);
			db_query($query, $args);
		} //$step1_data = $form_step1_data->fetchObject()
		else
		{
			$query = "INSERT INTO {ddq_fin_plate_form_step1_feedback} (uid, name, email_id, institute, current_position, data_reletion, inserted_on) VALUES (:uid,:name,:email_id, :institute, :current_position, :data_reletion,:inserted_on)";
			$args = array(
				":uid" => $user->uid,
				":name" => $v['step1_field1_name'],
				":email_id" => $v['step1_field2_email_id'],
				":institute" => $v['step1_field3_institute'],
				":current_position" => $v['step1_field4_current_position'],
				":data_reletion" => "step1",
				":inserted_on" => date('Y-m-d')
			);
			db_query($query, $args);
		}
	} //$form_step1_data
	$form_state['redirect'] = 'ddq-fin-plate/form/step2/' . $user->uid;
}
/************************ Step 1 form ends *********************/
/************************ Step 2 form starts *********************/
function ddq_fin_plate_step2_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	// All the real form fields.
	$img_path = $base_url . '/' . drupal_get_path("module", "osdag_response_form") . '/images';
	$step2_options = array(
		'Yes' => 'Yes',
		'No' => 'No'
	);
	$form = array();
	$form['step2_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step2-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddq_fin_plate_form_step2_feedback');
	$query->fields('ddq_fin_plate_form_step2_feedback');
	$query->condition('uid', $user->uid);
	$form_step2_data = $query->execute();
	if ($form_step2_data)
	{
		if ($step2_data = $form_step2_data->fetchObject())
		{
			$form['step2_field1_fieldset']['step2_field1_question1'] = array(
				'#title' => 'a. The minimum number of bolts for the fin plate connection are assumed to be 3. 
				
				<br><br>Is this assumption ok?',
				'#type' => 'radios',
				'#options' => $step2_options,
				'#default_value' => $step2_data->question1,
				'#required' => true
			);
			$form['step2_field1_fieldset']['step2_field2_reasoning_including_reference_from_literature'] = array(
				'#title' => 'Reasoning (including reference from literature):',
				'#type' => 'textarea',
				'#default_value' => $step2_data->any_other_recommendation,
				'#states' => array(
					'visible' => array(
						':input[name="step2_field1_fieldset\\[step2_field1_question1\\]"]' => array(
							array(
								'value' => t('Any other recommendation')
							)
						)
					)
				)
			);
			$earlier_comments = _fetch_exsiting_ddq_fin_plate_comments($step2_data->id, $step2_data->data_relation);
			$icom = 1;
			$form['step2_field1_fieldset']['exsiting_comments_title'] = array(
				'#markup' => '<hr><strong>Earlier comments:</strong><hr><br>'
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step2_field1_fieldset']['exsiting_comments' . $icom] = array(
					'#markup' => $rowcomment
				);
				$icom++;
			} //$earlier_comments as $rowcomment
			if (empty($form_state['step2_num_comment']))
			{
				$form_state['step2_num_comment'] = 1;
			} //empty($form_state['step2_num_comment'])
			$step2_temp1 = 0;
			for ($step2_i = 0; $step2_i < $form_state['step2_num_comment']; $step2_i++)
			{
				$step2_temp1 = $step2_i;
				$form['step2_field1_fieldset'][$step2_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step2_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step2_field1_fieldset']["step2_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step2_temp1
			);
			$form['step2_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step2_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step2_comment_add_more_callback',
					'wrapper' => 'step2-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step2_num_comment'] > 1)
			{
				$form['step2_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step2_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step2_comment_add_more_callback',
						'wrapper' => 'step2-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step2_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step2_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step2_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step2_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddq_fin_plate_step2_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddq_fin_plate_step2_form_next_submit'
				)
			);
		} //$step2_data = $form_step2_data->fetchObject()
		else
		{
			$form['step2_field1_fieldset']['step2_field1_question1'] = array(
				'#title' => 'The minimum number of bolts for the fin plate connection are assumed to be 3. 
				
				<br><br>Is this assumption ok?',
				'#type' => 'radios',
				'#options' => $step2_options,
				//'#default_value' => $step2_data->question1,
				'#required' => true
			);
			$form['step2_field1_fieldset']['step2_field2_reasoning_including_reference_from_literature'] = array(
				'#title' => 'Reasoning (including reference from literature):',
				'#type' => 'textarea',
				//'#default_value' => $step2_data->any_other_recommendation,
				'#states' => array(
					'visible' => array(
						':input[name="step2_field1_fieldset\\[step2_field1_question1\\]"]' => array(
							array(
								'value' => t('Any other recommendation')
							)
						)
					)
				)
			);
			if (empty($form_state['step2_num_comment']))
			{
				$form_state['step2_num_comment'] = 1;
			} //empty($form_state['step2_num_comment'])
			$step2_temp1 = 0;
			for ($step2_i = 0; $step2_i < $form_state['step2_num_comment']; $step2_i++)
			{
				$step2_temp1 = $step2_i;
				$form['step2_field1_fieldset'][$step2_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step2_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step2_field1_fieldset']["step2_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step2_temp1
			);
			$form['step2_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step2_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step2_comment_add_more_callback',
					'wrapper' => 'step2-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step2_num_comment'] > 1)
			{
				$form['step2_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step2_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step2_comment_add_more_callback',
						'wrapper' => 'step2-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step2_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step2_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step2_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step2_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddq_fin_plate_step2_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddq_fin_plate_step2_form_next_submit'
				)
			);
		} //$step2_data = $form_step2_data->fetchObject()
	} //$form_step2_data
	return $form;
}
function ddq_fin_plate_step2_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$query = db_select('ddq_fin_plate_form_step2_feedback');
	$query->fields('ddq_fin_plate_form_step2_feedback');
	$query->condition('uid', $user->uid);
	$form_step2_data = $query->execute()->fetchObject();
	if ($form_step2_data)
	{
		$query = "UPDATE ddq_fin_plate_form_step2_feedback SET question1=:question1, any_other_recommendation=:any_other_recommendation, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step2_field1_fieldset']['step2_field1_question1'],
			":any_other_recommendation" => $v['step2_field1_fieldset']['step2_field2_reasoning_including_reference_from_literature'],
			":data_relation" => "step2"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddq_fin_plate_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step2_data->id
		));
		$form_step2_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step2_field1_fieldset']["step2_comment_count"]; $i++)
		{
			if ($v['step2_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddq_fin_plate_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step2_data->id,
					":number" => $i + $form_step2_comment_data->max + 1,
					":comment" => $v['step2_field1_fieldset'][$i]["comment"],
					":data_relation" => "step2",
					":inserted_on" => date('Y-m-d')
				);
				/* storing the row id in $result */
				$commentresult = db_query($commentquery, $commentargs, array(
					'return' => Database::RETURN_INSERT_ID
				));
				if ($commentresult != 0)
				{
					$comments++;
				} //$commentresult != 0
			} //$v['step2_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step2_field1_fieldset']["step2_comment_count"]; $i++
	} //$form_step2_data
	else
	{
		$query = "INSERT INTO {ddq_fin_plate_form_step2_feedback} (uid, question1, any_other_recommendation, data_relation,inserted_on) VALUES (:uid,:question1,:any_other_recommendation, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step2_field1_fieldset']['step2_field1_question1'],
			":any_other_recommendation" => $v['step2_field1_fieldset']['step2_field2_reasoning_including_reference_from_literature'],
			":data_relation" => "step2",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$comments = 0;
		for ($i = 0; $i <= $v['step2_field1_fieldset']["step2_comment_count"]; $i++)
		{
			if ($v['step2_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddq_fin_plate_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step2_field1_fieldset'][$i]["comment"],
					":data_relation" => "step2",
					":inserted_on" => date('Y-m-d')
				);
				/* storing the row id in $result */
				$commentresult = db_query($commentquery, $commentargs, array(
					'return' => Database::RETURN_INSERT_ID
				));
				if ($commentresult != 0)
				{
					$comments++;
				} //$commentresult != 0
			} //$v['step2_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step2_field1_fieldset']["step2_comment_count"]; $i++
	}
	$form_state['redirect'] = 'ddq-fin-plate/form/step1/' . $user->uid;
}
function ddq_fin_plate_step2_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$query = db_select('ddq_fin_plate_form_step2_feedback');
	$query->fields('ddq_fin_plate_form_step2_feedback');
	$query->condition('uid', $user->uid);
	$form_step2_data = $query->execute()->fetchObject();
	if ($form_step2_data)
	{
		$query = "UPDATE ddq_fin_plate_form_step2_feedback SET question1=:question1, any_other_recommendation=:any_other_recommendation, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step2_field1_fieldset']['step2_field1_question1'],
			":any_other_recommendation" => $v['step2_field1_fieldset']['step2_field2_reasoning_including_reference_from_literature'],
			":data_relation" => "step2"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddq_fin_plate_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step2_data->id
		));
		$form_step2_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step2_field1_fieldset']["step2_comment_count"]; $i++)
		{
			if ($v['step2_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddq_fin_plate_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step2_data->id,
					":number" => $i + $form_step2_comment_data->max + 1,
					":comment" => $v['step2_field1_fieldset'][$i]["comment"],
					":data_relation" => "step2",
					":inserted_on" => date('Y-m-d')
				);
				/* storing the row id in $result */
				$commentresult = db_query($commentquery, $commentargs, array(
					'return' => Database::RETURN_INSERT_ID
				));
				if ($commentresult != 0)
				{
					$comments++;
				} //$commentresult != 0
			} //$v['step2_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step2_field1_fieldset']["step2_comment_count"]; $i++
	} //$form_step2_data
	else
	{
		$query = "INSERT INTO {ddq_fin_plate_form_step2_feedback} (uid, question1, any_other_recommendation, data_relation,inserted_on) VALUES (:uid,:question1,:any_other_recommendation, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step2_field1_fieldset']['step2_field1_question1'],
			":any_other_recommendation" => $v['step2_field1_fieldset']['step2_field2_reasoning_including_reference_from_literature'],
			":data_relation" => "step2",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$comments = 0;
		for ($i = 0; $i <= $v['step2_field1_fieldset']["step2_comment_count"]; $i++)
		{
			if ($v['step2_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddq_fin_plate_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step2_field1_fieldset'][$i]["comment"],
					":data_relation" => "step2",
					":inserted_on" => date('Y-m-d')
				);
				/* storing the row id in $result */
				$commentresult = db_query($commentquery, $commentargs, array(
					'return' => Database::RETURN_INSERT_ID
				));
				if ($commentresult != 0)
				{
					$comments++;
				} //$commentresult != 0
			} //$v['step2_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step2_field1_fieldset']["step2_comment_count"]; $i++
	}
	$form_state['redirect'] = 'ddq-fin-plate/form/step3/' . $user->uid;
}
/***************************************/
function step2_comment_add_more_add_one($form, &$form_state)
{
	$form_state['step2_num_comment']++;
	$form_state['rebuild'] = TRUE;
	//$form_state['no_redirect'] = TRUE;
}
function step2_comment_add_more_remove_one($form, &$form_state)
{
	if ($form_state['step2_num_comment'] > 1)
	{
		$form_state['step2_num_comment']--;
	} //$form_state['step2_num_comment'] > 1
	$form_state['rebuild'] = TRUE;
}
function step2_comment_add_more_callback($form, &$form_state)
{
	return $form['step2_field1_fieldset'];
}
/***************************************/
/***************************************** step 2 from ends ****************************/
/***************************************** step 3 form starts **************************/
function ddq_fin_plate_step3_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	// All the real form fields.
	$form = array();
	$step3_options = array(
		'Yes' => 'Yes',
		'No' => 'No'
	);
	$form['step3_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step3-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddq_fin_plate_form_step3_feedback');
	$query->fields('ddq_fin_plate_form_step3_feedback');
	$query->condition('uid', $user->uid);
	$form_step3_data = $query->execute();
	if ($form_step3_data)
	{
		if ($step3_data = $form_step3_data->fetchObject())
		{
			$form['step3_field1_fieldset']['step3_field1_question1'] = array(
				'#title' => 'b. A maximum of two (vertical) lines of bolts are allowed. 
				<br><br> Is this assumption ok? ',
				'#type' => 'radios',
				'#default_value' => $step3_data->question1,
				'#options' => $step3_options,
				'#required' => true
				//'#description' => "Note: You may give 'None' or 'N/A', if you feel that this is not applicable"
			);
			$form['step3_field1_fieldset']['step3_field1_question2'] = array(
				'#title' => 'specimen text3-3',
				'#type' => 'textarea',
				'#default_value' => $step3_data->question2,
				'#states' => array(
					'visible' => array(
						':input[name="step3_field1_fieldset\\[step3_field1_question1\\]"]' => array(
							array(
								'filled' => false
							)
						)
					)
				)
			);
			$earlier_comments = _fetch_exsiting_ddq_fin_plate_comments($step3_data->id, $step3_data->data_relation);
			$icom = 1;
			$form['step3_field1_fieldset']['exsiting_comments_title'] = array(
				'#markup' => '<hr><strong>Earlier comments: </strong><hr><br>'
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step3_field1_fieldset']['exsiting_comments' . $icom] = array(
					'#markup' => $rowcomment
				);
				$icom++;
			} //$earlier_comments as $rowcomment
			if (empty($form_state['step3_num_comment']))
			{
				$form_state['step3_num_comment'] = 1;
			} //empty($form_state['step3_num_comment'])
			$step3_temp1 = 0;
			for ($step3_i = 0; $step3_i < $form_state['step3_num_comment']; $step3_i++)
			{
				$step3_temp1 = $step3_i;
				$form['step3_field1_fieldset'][$step3_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step3_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step3_field1_fieldset']["step3_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step3_temp1
			);
			$form['step3_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step3_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step3_comment_add_more_callback',
					'wrapper' => 'step3-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step3_num_comment'] > 1)
			{
				$form['step3_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step3_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step3_comment_add_more_callback',
						'wrapper' => 'step3-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step3_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step3_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step3_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step3_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form['previous2'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddq_fin_plate_step3_form_previous_submit'
				)
			);
			$form['next3'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddq_fin_plate_step3_form_next_submit'
				)
			);
		} //$step3_data = $form_step3_data->fetchObject()
		else
		{
			$form['step3_field1_fieldset']['step3_field1_question1'] = array(
				'#title' => 'b. A maximum of two (vertical) lines of bolts are allowed. 
				<br><br> Is this assumption ok? ',
				'#type' => 'radios',
				'#options' => $step3_options,
				'#required' => true
			);
			$form['step3_field1_fieldset']['step3_field1_question2'] = array(
				'#title' => 'specimen text3-3',
				'#type' => 'textarea',
				//'#default_value' => $step3_data->question2,
				'#states' => array(
					'visible' => array(
						':input[name="step3_field1_fieldset\\[step3_field1_question1\\]"]' => array(
							array(
								'filled' => false
							)
						)
					)
				)
			);
			if (empty($form_state['step3_num_comment']))
			{
				$form_state['step3_num_comment'] = 1;
			} //empty($form_state['step3_num_comment'])
			$step3_temp1 = 0;
			for ($step3_i = 0; $step3_i < $form_state['step3_num_comment']; $step3_i++)
			{
				$step3_temp1 = $step3_i;
				$form['step3_field1_fieldset'][$step3_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step3_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step3_field1_fieldset']["step3_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step3_temp1
			);
			$form['step3_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step3_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step3_comment_add_more_callback',
					'wrapper' => 'step3-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step3_num_comment'] > 1)
			{
				$form['step3_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step3_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step3_comment_add_more_callback',
						'wrapper' => 'step3-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step3_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step3_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step3_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step3_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddq_fin_plate_step3_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddq_fin_plate_step3_form_next_submit'
				)
			);
		} //$step3_data = $form_step3_data->fetchObject()
	} //$form_step3_data
	return $form;
}
function ddq_fin_plate_step3_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$query = db_select('ddq_fin_plate_form_step3_feedback');
	$query->fields('ddq_fin_plate_form_step3_feedback');
	$query->condition('uid', $user->uid);
	$form_step3_data = $query->execute()->fetchObject();
	//var_dump($form_step3_data);die;
	if ($form_step3_data)
	{
		$query = "UPDATE ddq_fin_plate_form_step3_feedback SET question1=:question1,question2=:question2, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step3_field1_fieldset']['step3_field1_question1'],
			":question2" => $v['step3_field1_fieldset']['step3_field1_question2'],
			":data_relation" => "step3"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddq_fin_plate_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step2_data->id
		));
		$form_step3_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step3_field1_fieldset']["step3_comment_count"]; $i++)
		{
			if ($v['step3_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddq_fin_plate_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step3_data->id,
					":number" => $i + $form_step3_comment_data->max + 1,
					":comment" => $v['step3_field1_fieldset'][$i]["comment"],
					":data_relation" => "step3",
					":inserted_on" => date('Y-m-d')
				);
				/* storing the row id in $result */
				$commentresult = db_query($commentquery, $commentargs, array(
					'return' => Database::RETURN_INSERT_ID
				));
				if ($commentresult != 0)
				{
					$comments++;
				} //$commentresult != 0
			} //$v['step3_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step3_field1_fieldset']["step3_comment_count"]; $i++
	} //$form_step2_data
	else
	{
		$query = "INSERT INTO {ddq_fin_plate_form_step3_feedback} (uid, question1, question2, data_relation,inserted_on) VALUES (:uid,:question1,:question2, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step3_field1_fieldset']['step3_field1_question1'],
			":question2" => $v['step3_field1_fieldset']['step3_field1_question2'],
			":data_relation" => "step3",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$comments = 0;
		for ($i = 0; $i <= $v['step3_field1_fieldset']["step3_comment_count"]; $i++)
		{
			if ($v['step3_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddq_fin_plate_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step3_field1_fieldset'][$i]["comment"],
					":data_relation" => "step3",
					":inserted_on" => date('Y-m-d')
				);
				/* storing the row id in $result */
				$commentresult = db_query($commentquery, $commentargs, array(
					'return' => Database::RETURN_INSERT_ID
				));
				if ($commentresult != 0)
				{
					$comments++;
				} //$commentresult != 0
			} //$v['step3_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step3_field1_fieldset']["step3_comment_count"]; $i++
	}
	$form_state['redirect'] = 'ddq-fin-plate/form/step2/' . $user->uid;
}
function ddq_fin_plate_step3_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$query = db_select('ddq_fin_plate_form_step3_feedback');
	$query->fields('ddq_fin_plate_form_step3_feedback');
	$query->condition('uid', $user->uid);
	$form_step3_data = $query->execute()->fetchObject();
	//var_dump($form_step3_data);die;
	if ($form_step3_data)
	{
		$query = "UPDATE ddq_fin_plate_form_step3_feedback SET question1=:question1,question2=:question2, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step3_field1_fieldset']['step3_field1_question1'],
			":question2" => $v['step3_field1_fieldset']['step3_field1_question2'],
			":data_relation" => "step3"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddq_fin_plate_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step3_data->id
		));
		$form_step3_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step3_field1_fieldset']["step3_comment_count"]; $i++)
		{
			if ($v['step3_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddq_fin_plate_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step3_data->id,
					":number" => $i + $form_step3_comment_data->max + 1,
					":comment" => $v['step3_field1_fieldset'][$i]["comment"],
					":data_relation" => "step3",
					":inserted_on" => date('Y-m-d')
				);
				/* storing the row id in $result */
				$commentresult = db_query($commentquery, $commentargs, array(
					'return' => Database::RETURN_INSERT_ID
				));
				if ($commentresult != 0)
				{
					$comments++;
				} //$commentresult != 0
			} //$v['step3_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step3_field1_fieldset']["step3_comment_count"]; $i++
	} //$form_step2_data
	else
	{
		$query = "INSERT INTO {ddq_fin_plate_form_step3_feedback} (uid, question1, question2, data_relation,inserted_on) VALUES (:uid,:question1,:question2, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step3_field1_fieldset']['step3_field1_question1'],
			":question2" => $v['step3_field1_fieldset']['step3_field1_question2'],
			":data_relation" => "step3",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$comments = 0;
		for ($i = 0; $i <= $v['step3_field1_fieldset']["step3_comment_count"]; $i++)
		{
			if ($v['step3_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddq_fin_plate_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step3_field1_fieldset'][$i]["comment"],
					":data_relation" => "step3",
					":inserted_on" => date('Y-m-d')
				);
				/* storing the row id in $result */
				$commentresult = db_query($commentquery, $commentargs, array(
					'return' => Database::RETURN_INSERT_ID
				));
				if ($commentresult != 0)
				{
					$comments++;
				} //$commentresult != 0
			} //$v['step3_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step3_field1_fieldset']["step3_comment_count"]; $i++
	}
	$form_state['redirect'] = 'ddq-fin-plate/form/step4/' . $user->uid;
}
/***************************************/
function step3_comment_add_more_add_one($form, &$form_state)
{
	$form_state['step3_num_comment']++;
	$form_state['rebuild'] = TRUE;
	//$form_state['no_redirect'] = TRUE;
}
function step3_comment_add_more_remove_one($form, &$form_state)
{
	if ($form_state['step3_num_comment'] > 1)
	{
		$form_state['step3_num_comment']--;
	} //$form_state['step3_num_comment'] > 1
	$form_state['rebuild'] = TRUE;
}
function step3_comment_add_more_callback($form, &$form_state)
{
	return $form['step3_field1_fieldset'];
}
/***************************************/
/***************************************** step 3 form ends **************************/
/************************ Step 4 form starts *********************/
function ddq_fin_plate_step4_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	// All the real form fields.
	$img_path = $base_url . '/' . drupal_get_path("module", "osdag_response_form") . '/images';
	$step4_options = array(
		'Yes' => 'Yes',
		'No' => 'No'
	);
	$form = array();
	$form['step4_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step4-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddq_fin_plate_form_step4_feedback');
	$query->fields('ddq_fin_plate_form_step4_feedback');
	$query->condition('uid', $user->uid);
	$form_step4_data = $query->execute();
	if ($form_step4_data)
	{
		if ($step4_data = $form_step4_data->fetchObject())
		{
			$form['step4_field1_fieldset']['step4_field1_question1'] = array(
				'#title' => 'c. The gap between the edge of the supported beam and the edge (or face) of the support (column or beam, depending on the connectivity) is assumed to be 20 mm. 
 
				<br><br>Is this assumption ok?',
				'#type' => 'radios',
				'#options' => $step4_options,
				'#default_value' => $step4_data->question1,
				'#required' => true
			);
			$earlier_comments = _fetch_exsiting_ddq_fin_plate_comments($step4_data->id, $step4_data->data_relation);
			$icom = 1;
			$form['step4_field1_fieldset']['exsiting_comments_title'] = array(
				'#markup' => '<hr><strong>Earlier comments: </strong><hr><br>'
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step4_field1_fieldset']['exsiting_comments' . $icom] = array(
					'#markup' => $rowcomment
				);
				$icom++;
			} //$earlier_comments as $rowcomment
			if (empty($form_state['step4_num_comment']))
			{
				$form_state['step4_num_comment'] = 1;
			} //empty($form_state['step4_num_comment'])
			$step4_temp1 = 0;
			for ($step4_i = 0; $step4_i < $form_state['step4_num_comment']; $step4_i++)
			{
				$step4_temp1 = $step4_i;
				$form['step4_field1_fieldset'][$step4_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step4_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step4_field1_fieldset']["step4_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step4_temp1
			);
			$form['step4_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step4_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step4_comment_add_more_callback',
					'wrapper' => 'step4-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step4_num_comment'] > 1)
			{
				$form['step4_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step4_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step4_comment_add_more_callback',
						'wrapper' => 'step4-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step4_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step4_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step4_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step4_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddq_fin_plate_step4_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddq_fin_plate_step4_form_next_submit'
				)
			);
		} //$step4_data = $form_step4_data->fetchObject()
		else
		{
			$form['step4_field1_fieldset']['step4_field1_question1'] = array(
				'#title' => 'c. The gap between the edge of the supported beam and the edge (or face) of the support (column or beam, depending on the connectivity) is assumed to be 20 mm. 
				<br><br>Is this assumption ok?',
				'#type' => 'radios',
				'#options' => $step4_options,
				//'#default_value' => $step4_data->question1,
				'#required' => true
			);
			if (empty($form_state['step4_num_comment']))
			{
				$form_state['step4_num_comment'] = 1;
			} //empty($form_state['step4_num_comment'])
			$step4_temp1 = 0;
			for ($step4_i = 0; $step4_i < $form_state['step4_num_comment']; $step4_i++)
			{
				$step4_temp1 = $step4_i;
				$form['step4_field1_fieldset'][$step4_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step4_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step4_field1_fieldset']["step4_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step4_temp1
			);
			$form['step4_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step4_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step4_comment_add_more_callback',
					'wrapper' => 'step4-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step4_num_comment'] > 1)
			{
				$form['step4_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step4_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step4_comment_add_more_callback',
						'wrapper' => 'step4-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step4_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step4_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step4_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step4_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddq_fin_plate_step4_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddq_fin_plate_step4_form_next_submit'
				)
			);
		} //$step4_data = $form_step4_data->fetchObject()
	} //$form_step4_data
	return $form;
}
function ddq_fin_plate_step4_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$query = db_select('ddq_fin_plate_form_step4_feedback');
	$query->fields('ddq_fin_plate_form_step4_feedback');
	$query->condition('uid', $user->uid);
	$form_step4_data = $query->execute()->fetchObject();
	//var_dump($form_step4_data);die;
	if ($form_step4_data)
	{
		$query = "UPDATE ddq_fin_plate_form_step4_feedback SET question1=:question1, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step4_field1_fieldset']['step4_field1_question1'],
			":data_relation" => "step4"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddq_fin_plate_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step4_data->id
		));
		$form_step4_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step4_field1_fieldset']["step4_comment_count"]; $i++)
		{
			if ($v['step4_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddq_fin_plate_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step4_data->id,
					":number" => $i + $form_step4_comment_data->max + 1,
					":comment" => $v['step4_field1_fieldset'][$i]["comment"],
					":data_relation" => "step4",
					":inserted_on" => date('Y-m-d')
				);
				/* storing the row id in $result */
				$commentresult = db_query($commentquery, $commentargs, array(
					'return' => Database::RETURN_INSERT_ID
				));
				if ($commentresult != 0)
				{
					$comments++;
				} //$commentresult != 0
			} //$v['step4_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step4_field1_fieldset']["step4_comment_count"]; $i++
	} //$form_step4_data
	else
	{
		$query = "INSERT INTO {ddq_fin_plate_form_step4_feedback} (uid, question1, data_relation,inserted_on) VALUES (:uid,:question1, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step4_field1_fieldset']['step4_field1_question1'],
			":data_relation" => "step4",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$comments = 0;
		for ($i = 0; $i <= $v['step4_field1_fieldset']["step4_comment_count"]; $i++)
		{
			if ($v['step4_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddq_fin_plate_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step4_field1_fieldset'][$i]["comment"],
					":data_relation" => "step4",
					":inserted_on" => date('Y-m-d')
				);
				/* storing the row id in $result */
				$commentresult = db_query($commentquery, $commentargs, array(
					'return' => Database::RETURN_INSERT_ID
				));
				if ($commentresult != 0)
				{
					$comments++;
				} //$commentresult != 0
			} //$v['step4_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step4_field1_fieldset']["step4_comment_count"]; $i++
	}
	$form_state['redirect'] = 'ddq-fin-plate/form/step3/' . $user->uid;
}
function ddq_fin_plate_step4_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$query = db_select('ddq_fin_plate_form_step4_feedback');
	$query->fields('ddq_fin_plate_form_step4_feedback');
	$query->condition('uid', $user->uid);
	$form_step4_data = $query->execute()->fetchObject();
	//var_dump($form_step4_data);die;
	if ($form_step4_data)
	{
		$query = "UPDATE ddq_fin_plate_form_step4_feedback SET question1=:question1, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step4_field1_fieldset']['step4_field1_question1'],
			":data_relation" => "step4"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddq_fin_plate_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step4_data->id
		));
		$form_step4_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step4_field1_fieldset']["step4_comment_count"]; $i++)
		{
			if ($v['step4_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddq_fin_plate_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step4_data->id,
					":number" => $i + $form_step4_comment_data->max + 1,
					":comment" => $v['step4_field1_fieldset'][$i]["comment"],
					":data_relation" => "step4",
					":inserted_on" => date('Y-m-d')
				);
				/* storing the row id in $result */
				$commentresult = db_query($commentquery, $commentargs, array(
					'return' => Database::RETURN_INSERT_ID
				));
				if ($commentresult != 0)
				{
					$comments++;
				} //$commentresult != 0
			} //$v['step4_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step4_field1_fieldset']["step4_comment_count"]; $i++
	} //$form_step4_data
	else
	{
		$query = "INSERT INTO {ddq_fin_plate_form_step4_feedback} (uid, question1, data_relation,inserted_on) VALUES (:uid,:question1, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step4_field1_fieldset']['step4_field1_question1'],
			":data_relation" => "step4",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$comments = 0;
		for ($i = 0; $i <= $v['step4_field1_fieldset']["step4_comment_count"]; $i++)
		{
			if ($v['step4_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddq_fin_plate_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step4_field1_fieldset'][$i]["comment"],
					":data_relation" => "step4",
					":inserted_on" => date('Y-m-d')
				);
				/* storing the row id in $result */
				$commentresult = db_query($commentquery, $commentargs, array(
					'return' => Database::RETURN_INSERT_ID
				));
				if ($commentresult != 0)
				{
					$comments++;
				} //$commentresult != 0
			} //$v['step4_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step4_field1_fieldset']["step4_comment_count"]; $i++
	}
	$form_state['redirect'] = 'ddq-fin-plate/form/step5/' . $user->uid;
}
/***************************************/
function step4_comment_add_more_add_one($form, &$form_state)
{
	$form_state['step4_num_comment']++;
	$form_state['rebuild'] = TRUE;
	//$form_state['no_redirect'] = TRUE;
}
function step4_comment_add_more_remove_one($form, &$form_state)
{
	if ($form_state['step4_num_comment'] > 1)
	{
		$form_state['step4_num_comment']--;
	} //$form_state['step4_num_comment'] > 1
	$form_state['rebuild'] = TRUE;
}
function step4_comment_add_more_callback($form, &$form_state)
{
	return $form['step4_field1_fieldset'];
}
/***************************************/
/***************************************** step 4 from ends ****************************/
/************************ Step 5 form starts *********************/
function ddq_fin_plate_step5_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	// All the real form fields.
	$img_path = $base_url . '/' . drupal_get_path("module", "osdag_response_form") . '/images';
	$step5_options = array(
		'Yes' => 'Yes',
		'No' => 'No'
	);
	$form = array();
	$form['step5_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step5-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddq_fin_plate_form_step5_feedback');
	$query->fields('ddq_fin_plate_form_step5_feedback');
	$query->condition('uid', $user->uid);
	$form_step5_data = $query->execute();
	if ($form_step5_data)
	{
		if ($step5_data = $form_step5_data->fetchObject())
		{
			$form['step5_field1_fieldset']['step5_field1_question1'] = array(
				'#title' => 'd. The minimum plate thickness should be greater than the supported beam web thickness. 
				<br><br>Is this assumption ok?',
				'#type' => 'radios',
				'#options' => $step5_options,
				'#default_value' => $step5_data->question1,
				'#required' => true
			);
			
			$earlier_comments = _fetch_exsiting_ddq_fin_plate_comments($step5_data->id, $step5_data->data_relation);
			$icom = 1;
			$form['step5_field1_fieldset']['exsiting_comments_title'] = array(
				'#markup' => '<hr><strong>Earlier comments:</strong><hr><br>'
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step5_field1_fieldset']['exsiting_comments' . $icom] = array(
					'#markup' => $rowcomment
				);
				$icom++;
			} //$earlier_comments as $rowcomment
			if (empty($form_state['step5_num_comment']))
			{
				$form_state['step5_num_comment'] = 1;
			} //empty($form_state['step5_num_comment'])
			$step5_temp1 = 0;
			for ($step5_i = 0; $step5_i < $form_state['step5_num_comment']; $step5_i++)
			{
				$step5_temp1 = $step5_i;
				$form['step5_field1_fieldset'][$step5_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step5_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step5_field1_fieldset']["step5_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step5_temp1
			);
			$form['step5_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step5_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step5_comment_add_more_callback',
					'wrapper' => 'step5-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step5_num_comment'] > 1)
			{
				$form['step5_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step5_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step5_comment_add_more_callback',
						'wrapper' => 'step5-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step5_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step5_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step5_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step5_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddq_fin_plate_step5_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddq_fin_plate_step5_form_next_submit'
				)
			);
		} //$step5_data = $form_step5_data->fetchObject()
		else
		{
			$form['step5_field1_fieldset']['step5_field1_question1'] = array(
				'#title' => 'd. The minimum plate thickness should be greater than the supported beam web thickness. 
				<br><br>Is this assumption ok?',
				'#type' => 'radios',
				'#options' => $step5_options,
				//'#default_value' => $step5_data->question1,
				'#required' => true
			);
			
			if (empty($form_state['step5_num_comment']))
			{
				$form_state['step5_num_comment'] = 1;
			} //empty($form_state['step5_num_comment'])
			$step5_temp1 = 0;
			for ($step5_i = 0; $step5_i < $form_state['step5_num_comment']; $step5_i++)
			{
				$step5_temp1 = $step5_i;
				$form['step5_field1_fieldset'][$step5_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step5_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step5_field1_fieldset']["step5_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step5_temp1
			);
			$form['step5_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step5_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step5_comment_add_more_callback',
					'wrapper' => 'step5-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step5_num_comment'] > 1)
			{
				$form['step5_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step5_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step5_comment_add_more_callback',
						'wrapper' => 'step5-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step5_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step5_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step5_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step5_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddq_fin_plate_step5_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddq_fin_plate_step5_form_next_submit'
				)
			);
		} //$step5_data = $form_step5_data->fetchObject()
	} //$form_step5_data
	return $form;
}
function ddq_fin_plate_step5_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$query = db_select('ddq_fin_plate_form_step5_feedback');
	$query->fields('ddq_fin_plate_form_step5_feedback');
	$query->condition('uid', $user->uid);
	$form_step5_data = $query->execute()->fetchObject();
	//var_dump($form_step5_data);die;
	if ($form_step5_data)
	{
		$query = "UPDATE ddq_fin_plate_form_step5_feedback SET question1=:question1, what_IS_code_clauses_and_references_should_be_followed=:if_yes, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step5_field1_fieldset']['step5_field1_question1'],
			":if_yes" => $v['step5_field1_fieldset']['step5_field2_what_IS_code_clauses_and_references_should_be_followed'],
			":data_relation" => "step5"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddq_fin_plate_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step5_data->id
		));
		$form_step5_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step5_field1_fieldset']["step5_comment_count"]; $i++)
		{
			if ($v['step5_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddq_fin_plate_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step5_data->id,
					":number" => $i + $form_step5_comment_data->max + 1,
					":comment" => $v['step5_field1_fieldset'][$i]["comment"],
					":data_relation" => "step5",
					":inserted_on" => date('Y-m-d')
				);
				/* storing the row id in $result */
				$commentresult = db_query($commentquery, $commentargs, array(
					'return' => Database::RETURN_INSERT_ID
				));
				if ($commentresult != 0)
				{
					$comments++;
				} //$commentresult != 0
			} //$v['step5_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step5_field1_fieldset']["step5_comment_count"]; $i++
	} //$form_step5_data
	else
	{
		$query = "INSERT INTO {ddq_fin_plate_form_step5_feedback} (uid, question1,what_IS_code_clauses_and_references_should_be_followed, data_relation,inserted_on) VALUES (:uid,:question1,:if_yes, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step5_field1_fieldset']['step5_field1_question1'],
			":if_yes" => $v['step5_field1_fieldset']['step5_field2_what_IS_code_clauses_and_references_should_be_followed'],
			":data_relation" => "step5",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$comments = 0;
		for ($i = 0; $i <= $v['step5_field1_fieldset']["step5_comment_count"]; $i++)
		{
			if ($v['step5_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddq_fin_plate_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step5_field1_fieldset'][$i]["comment"],
					":data_relation" => "step5",
					":inserted_on" => date('Y-m-d')
				);
				/* storing the row id in $result */
				$commentresult = db_query($commentquery, $commentargs, array(
					'return' => Database::RETURN_INSERT_ID
				));
				if ($commentresult != 0)
				{
					$comments++;
				} //$commentresult != 0
			} //$v['step5_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step5_field1_fieldset']["step5_comment_count"]; $i++
	}
	$form_state['redirect'] = 'ddq-fin-plate/form/step4/' . $user->uid;
}
function ddq_fin_plate_step5_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$query = db_select('ddq_fin_plate_form_step5_feedback');
	$query->fields('ddq_fin_plate_form_step5_feedback');
	$query->condition('uid', $user->uid);
	$form_step5_data = $query->execute()->fetchObject();
	//var_dump($form_step5_data);die;
	if ($form_step5_data)
	{
		$query = "UPDATE ddq_fin_plate_form_step5_feedback SET question1=:question1, what_IS_code_clauses_and_references_should_be_followed=:if_yes, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step5_field1_fieldset']['step5_field1_question1'],
			":if_yes" => $v['step5_field1_fieldset']['step5_field2_what_IS_code_clauses_and_references_should_be_followed'],
			":data_relation" => "step5"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddq_fin_plate_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step5_data->id
		));
		$form_step5_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step5_field1_fieldset']["step5_comment_count"]; $i++)
		{
			if ($v['step5_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddq_fin_plate_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step5_data->id,
					":number" => $i + $form_step5_comment_data->max + 1,
					":comment" => $v['step5_field1_fieldset'][$i]["comment"],
					":data_relation" => "step5",
					":inserted_on" => date('Y-m-d')
				);
				/* storing the row id in $result */
				$commentresult = db_query($commentquery, $commentargs, array(
					'return' => Database::RETURN_INSERT_ID
				));
				if ($commentresult != 0)
				{
					$comments++;
				} //$commentresult != 0
			} //$v['step5_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step5_field1_fieldset']["step5_comment_count"]; $i++
	} //$form_step5_data
	else
	{
		$query = "INSERT INTO {ddq_fin_plate_form_step5_feedback} (uid, question1,what_IS_code_clauses_and_references_should_be_followed, data_relation,inserted_on) VALUES (:uid,:question1,:if_yes, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step5_field1_fieldset']['step5_field1_question1'],
			":if_yes" => $v['step5_field1_fieldset']['step5_field2_what_IS_code_clauses_and_references_should_be_followed'],
			":data_relation" => "step5",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$comments = 0;
		for ($i = 0; $i <= $v['step5_field1_fieldset']["step5_comment_count"]; $i++)
		{
			if ($v['step5_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddq_fin_plate_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step5_field1_fieldset'][$i]["comment"],
					":data_relation" => "step5",
					":inserted_on" => date('Y-m-d')
				);
				/* storing the row id in $result */
				$commentresult = db_query($commentquery, $commentargs, array(
					'return' => Database::RETURN_INSERT_ID
				));
				if ($commentresult != 0)
				{
					$comments++;
				} //$commentresult != 0
			} //$v['step5_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step5_field1_fieldset']["step5_comment_count"]; $i++
	}
	$form_state['redirect'] = 'ddq-fin-plate/form/step6/' . $user->uid;
}
/***************************************/
function step5_comment_add_more_add_one($form, &$form_state)
{
	$form_state['step5_num_comment']++;
	$form_state['rebuild'] = TRUE;
	//$form_state['no_redirect'] = TRUE;
}
function step5_comment_add_more_remove_one($form, &$form_state)
{
	if ($form_state['step5_num_comment'] > 1)
	{
		$form_state['step5_num_comment']--;
	} //$form_state['step5_num_comment'] > 1
	$form_state['rebuild'] = TRUE;
}
function step5_comment_add_more_callback($form, &$form_state)
{
	return $form['step5_field1_fieldset'];
}
/***************************************/
/***************************************** step 5 from ends ****************************/
/***************************************** step 6 form starts **************************/
function ddq_fin_plate_step6_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	// All the real form fields.
	$form = array();
	$form['step6_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#title' => '<strong>Any other recommendations/ Comments?</strong>',
		'#tree' => TRUE,
		'#prefix' => '<div id="step6-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddq_fin_plate_form_step6_feedback');
	$query->fields('ddq_fin_plate_form_step6_feedback');
	$query->condition('uid', $user->uid);
	$form_step6_data = $query->execute();
	if ($form_step6_data)
	{
		if ($step6_data = $form_step6_data->fetchObject())
		{
			$form['step6_field1_fieldset']['step6_field1_question1'] = array(
				'#type' => 'hidden',
				'#default_value' => 'Other recommendations/ Comments',
				'#required' => TRUE
			);
			$earlier_comments = _fetch_exsiting_ddq_fin_plate_comments($step6_data->id, $step6_data->data_relation);
			$form['step6_field1_fieldset']['exsiting_comments_title'] = array(
				'#markup' => '<hr><strong>Earlier comments:</strong><hr><br>'
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step6_field1_fieldset']['exsiting_comments' . $icom] = array(
					'#markup' => $rowcomment
				);
				$icom++;
			} //$earlier_comments as $rowcomment
			if (empty($form_state['step6_num_comment']))
			{
				$form_state['step6_num_comment'] = 1;
			} //empty($form_state['step6_num_comment'])
			$step6_temp1 = 0;
			for ($step6_i = 0; $step6_i < $form_state['step6_num_comment']; $step6_i++)
			{
				$step6_temp1 = $step6_i;
				$form['step6_field1_fieldset'][$step6_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step6_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step6_field1_fieldset']["step6_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step6_temp1
			);
			$form['step6_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step6_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step6_comment_add_more_callback',
					'wrapper' => 'step6-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step6_num_comment'] > 1)
			{
				$form['step6_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step6_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step6_comment_add_more_callback',
						'wrapper' => 'step6-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step6_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step6_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step6_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step6_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form['previous2'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddq_fin_plate_step6_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Submit',
				'#attributes' => array(
					"onclick" => "return confirm('Are you sure?')"
				),
				'#submit' => array(
					'ddq_fin_plate_step6_form_next_submit'
				)
			);
			$form['save_submit'] = array(
				'#type' => 'submit',
				'#value' => 'Save as draft',
				'#attributes' => array(
					"onclick" => "return confirm('Are you sure?')"
				),
				'#submit' => array(
					'ddq_fin_plate_step6_form_save_submit'
				)
			);
		} //$step6_data = $form_step6_data->fetchObject()
		else
		{
			$form['step6_field1_fieldset']['step6_field1_question1'] = array(
				'#type' => 'hidden',
				'#default_value' => 'other recommendations/ Comments',
				'#required' => TRUE
			);
			if (empty($form_state['step6_num_comment']))
			{
				$form_state['step6_num_comment'] = 1;
			} //empty($form_state['step6_num_comment'])
			$step6_temp1 = 0;
			for ($step6_i = 0; $step6_i < $form_state['step6_num_comment']; $step6_i++)
			{
				$step6_temp1 = $step6_i;
				$form['step6_field1_fieldset'][$step6_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step6_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step6_field1_fieldset']["step6_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step6_temp1
			);
			$form['step6_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step6_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step6_comment_add_more_callback',
					'wrapper' => 'step6-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step6_num_comment'] > 1)
			{
				$form['step6_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step6_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step6_comment_add_more_callback',
						'wrapper' => 'step6-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step6_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step6_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step6_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step6_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddq_fin_plate_step6_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Submit',
				'#attributes' => array(
					"onclick" => "return confirm('Are you sure?')"
				),
				'#submit' => array(
					'ddq_fin_plate_step6_form_next_submit'
				)
			);
			$form['save_submit'] = array(
				'#type' => 'submit',
				'#value' => 'Save as draft',
				'#attributes' => array(
					"onclick" => "return confirm('Are you sure?')"
				),
				'#submit' => array(
					'ddq_fin_plate_step6_form_save_submit'
				)
			);
		} //$step6_data = $form_step6_data->fetchObject()
	} //$form_step6_data
	return $form;
}
function ddq_fin_plate_step6_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step6_field1_fieldset']['step6_field1_question1'] = 'other recommendations/ Comments';
	/*$query = db_select('ddq_fin_plate_form_step6_feedback');
	$query->fields('ddq_fin_plate_form_step6_feedback');
	$query->condition('uid', $user->uid);
	$form_step6_data = $query->execute()->fetchObject();
	*/
	$query = "SELECT * FROM ddq_fin_plate_form_step6_feedback WHERE uid=:uid";
	$args = array(
		":uid" => $user->uid
	);
	$form_step6_data = db_query($query, $args);
	$form_step6_data_count = $form_step6_data->rowCount();
	$form_step6_data = $form_step6_data->fetchObject();
	//var_dump($form_step6_data);die;
	if ($form_step6_data_count != 0)
	{
		$query = "UPDATE ddq_fin_plate_form_step6_feedback SET question1=:question1, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step6_field1_fieldset']['step6_field1_question1'],
			":data_relation" => "step6"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddq_fin_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid ";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddq_fin_plate_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step6_data->id
		));
		$form_step6_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step6_field1_fieldset']["step6_comment_count"]; $i++)
		{
			if ($v['step6_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddq_fin_plate_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step6_data->id,
					":number" => $i + $form_step6_comment_data->max + 1,
					":comment" => $v['step6_field1_fieldset'][$i]["comment"],
					":data_relation" => "step6",
					":inserted_on" => date('Y-m-d')
				);
				/* storing the row id in $result */
				$commentresult = db_query($commentquery, $commentargs, array(
					'return' => Database::RETURN_INSERT_ID
				));
				if ($commentresult != 0)
				{
					$comments++;
				} //$commentresult != 0
			} //$v['step6_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step6_field1_fieldset']["step6_comment_count"]; $i++
	} //$form_step6_data
	else
	{
		$query = "INSERT INTO {ddq_fin_plate_form_step6_feedback} (uid, question1, data_relation,inserted_on) VALUES (:uid,:question1, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step6_field1_fieldset']['step6_field1_question1'],
			":data_relation" => "step6",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddq_fin_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid ";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$comments = 0;
		for ($i = 0; $i <= $v['step6_field1_fieldset']["step6_comment_count"]; $i++)
		{
			if ($v['step6_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddq_fin_plate_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step6_field1_fieldset'][$i]["comment"],
					":data_relation" => "step6",
					":inserted_on" => date('Y-m-d')
				);
				/* storing the row id in $result */
				$commentresult = db_query($commentquery, $commentargs, array(
					'return' => Database::RETURN_INSERT_ID
				));
				if ($commentresult != 0)
				{
					$comments++;
				} //$commentresult != 0
			} //$v['step6_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step6_field1_fieldset']["step6_comment_count"]; $i++
	}
	$form_state['redirect'] = 'ddq-fin-plate/form/step5/' . $user->uid;
}
function ddq_fin_plate_step6_form_save_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step6_field1_fieldset']['step6_field1_question1'] = 'other recommendations/ Comments';
	/*$query = db_select('ddq_fin_plate_form_step6_feedback');
	$query->fields('ddq_fin_plate_form_step6_feedback');
	$query->condition('uid', $user->uid);
	$form_step6_data = $query->execute()->fetchObject();
	*/
	$query = "SELECT * FROM ddq_fin_plate_form_step6_feedback WHERE uid=:uid";
	$args = array(
		":uid" => $user->uid
	);
	$form_step6_data = db_query($query, $args);
	$form_step6_data_count = $form_step6_data->rowCount();
	$form_step6_data = $form_step6_data->fetchObject();
	//var_dump($form_step6_data);die;
	if ($form_step6_data_count != 0)
	{
		$query = "UPDATE ddq_fin_plate_form_step6_feedback SET question1=:question1, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step6_field1_fieldset']['step6_field1_question1'],
			":data_relation" => "step6"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddq_fin_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid ";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 0
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddq_fin_plate_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step6_data->id
		));
		$form_step6_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step6_field1_fieldset']["step6_comment_count"]; $i++)
		{
			if ($v['step6_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddq_fin_plate_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step6_data->id,
					":number" => $i + $form_step6_comment_data->max + 1,
					":comment" => $v['step6_field1_fieldset'][$i]["comment"],
					":data_relation" => "step6",
					":inserted_on" => date('Y-m-d')
				);
				/* storing the row id in $result */
				$commentresult = db_query($commentquery, $commentargs, array(
					'return' => Database::RETURN_INSERT_ID
				));
				if ($commentresult != 0)
				{
					$comments++;
				} //$commentresult != 0
			} //$v['step6_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step6_field1_fieldset']["step6_comment_count"]; $i++
	} //$form_step6_data
	else
	{
		$query = "INSERT INTO {ddq_fin_plate_form_step6_feedback} (uid, question1, data_relation,inserted_on) VALUES (:uid,:question1, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step6_field1_fieldset']['step6_field1_question1'],
			":data_relation" => "step6",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddq_fin_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid ";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 0
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$comments = 0;
		for ($i = 0; $i <= $v['step6_field1_fieldset']["step6_comment_count"]; $i++)
		{
			if ($v['step6_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddq_fin_plate_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step6_field1_fieldset'][$i]["comment"],
					":data_relation" => "step6",
					":inserted_on" => date('Y-m-d')
				);
				/* storing the row id in $result */
				$commentresult = db_query($commentquery, $commentargs, array(
					'return' => Database::RETURN_INSERT_ID
				));
				if ($commentresult != 0)
				{
					$comments++;
				} //$commentresult != 0
			} //$v['step6_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step6_field1_fieldset']["step6_comment_count"]; $i++
	}
	drupal_set_message(t('You have successfully saved your feedback. You can anytime edit your feedback form.'), 'status');
	drupal_goto('home');
}
function ddq_fin_plate_step6_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step6_field1_fieldset']['step6_field1_question1'] = 'other recommendations/ Comments';
	/*$query = db_select('ddq_fin_plate_form_step6_feedback');
	$query->fields('ddq_fin_plate_form_step6_feedback');
	$query->condition('uid', $user->uid);
	$form_step6_data = $query->execute()->fetchObject();
	*/
	$query = "SELECT * FROM ddq_fin_plate_form_step6_feedback WHERE uid=:uid";
	$args = array(
		":uid" => $user->uid
	);
	$form_step6_data = db_query($query, $args);
	$form_step6_data_count = $form_step6_data->rowCount();
	$form_step6_data = $form_step6_data->fetchObject();
	//var_dump($form_step6_data);die;
	if ($form_step6_data_count != 0)
	{
		$query = "UPDATE ddq_fin_plate_form_step6_feedback SET question1=:question1, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step6_field1_fieldset']['step6_field1_question1'],
			":data_relation" => "step6"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddq_fin_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid ";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddq_fin_plate_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step6_data->id
		));
		$form_step6_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step6_field1_fieldset']["step6_comment_count"]; $i++)
		{
			if ($v['step6_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddq_fin_plate_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step6_data->id,
					":number" => $i + $form_step6_comment_data->max + 1,
					":comment" => $v['step6_field1_fieldset'][$i]["comment"],
					":data_relation" => "step6",
					":inserted_on" => date('Y-m-d')
				);
				/* storing the row id in $result */
				$commentresult = db_query($commentquery, $commentargs, array(
					'return' => Database::RETURN_INSERT_ID
				));
				if ($commentresult != 0)
				{
					$comments++;
				} //$commentresult != 0
			} //$v['step6_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step6_field1_fieldset']["step6_comment_count"]; $i++
	} //$form_step7_data
	else
	{
		$query = "INSERT INTO {ddq_fin_plate_form_step6_feedback} (uid, question1, data_relation,inserted_on) VALUES (:uid,:question1, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step6_field1_fieldset']['step6_field1_question1'],
			":data_relation" => "step6",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddq_fin_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid ";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$comments = 0;
		for ($i = 0; $i <= $v['step6_field1_fieldset']["step6_comment_count"]; $i++)
		{
			if ($v['step6_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddq_fin_plate_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step6_field1_fieldset'][$i]["comment"],
					":data_relation" => "step6",
					":inserted_on" => date('Y-m-d')
				);
				/* storing the row id in $result */
				$commentresult = db_query($commentquery, $commentargs, array(
					'return' => Database::RETURN_INSERT_ID
				));
				if ($commentresult != 0)
				{
					$comments++;
				} //$commentresult != 0
			} //$v['step6_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step6_field1_fieldset']["step6_comment_count"]; $i++
	}
	$form_state['redirect'] = 'ddq-fin-plate/success';
}
/***************************************/
function step6_comment_add_more_add_one($form, &$form_state)
{
	$form_state['step6_num_comment']++;
	$form_state['rebuild'] = TRUE;
	//$form_state['no_redirect'] = TRUE;
}
function step6_comment_add_more_remove_one($form, &$form_state)
{
	if ($form_state['step6_num_comment'] > 1)
	{
		$form_state['step6_num_comment']--;
	} //$form_state['step6_num_comment'] > 1
	$form_state['rebuild'] = TRUE;
}
function step6_comment_add_more_callback($form, &$form_state)
{
	return $form['step6_field1_fieldset'];
}
/***************************************/
/***************************************** step 6 form ends **************************/
function ddq_fin_plate_result_page()
{
	/* get pending proposals to be approved */
	$output = "";
	$output .= l("Download All Result in CSV", "ddq-fin-plate/form/result/download");
	$feedback_rows = array();
	$feedback_q = db_query('

select

step1.uid as "step1_uid",
step1.name as "step1_name",
step1.inserted_on as "step1_date",

step2.id as "step2_id",
step2.data_relation as "step2_data_relation",
step2.question1 as "step2_question1", 

step3.id as "step3_id",
step3.data_relation as "step3_data_relation",
step3.question1 as "step3_question1", 

step4.id as "step4_id",
step4.data_relation as "step4_data_relation",
step4.question1 as "step4_question1",

step5.id as "step5_id",
step5.data_relation as "step5_data_relation",
step5.question1 as "step5_question1", 

step6.id as "step6_id",
step6.data_relation as "step6_data_relation",
step6.question1 as "step6_question1", 

from ddq_fin_plate_form_step1_feedback step1

join ddq_fin_plate_form_step2_feedback step2 on step1.uid=step2.uid
join ddq_fin_plate_form_step3_feedback step3 on step2.uid=step3.uid
join ddq_fin_plate_form_step4_feedback step4 on step3.uid=step4.uid
join ddq_fin_plate_form_step5_feedback step5 on step4.uid=step5.uid
join ddq_fin_plate_form_step6_feedback step6 on step5.uid=step6.uid


	');
	$i = 1;
	while ($feedback_data = $feedback_q->fetchObject())
	{
		$feedback_rows[$feedback_data->step1_uid] = array(
			$i,
			$feedback_data->step1_date,
			$feedback_data->step1_name,
			$feedback_data->step2_question1,
			_fetch_exsiting_ddq_fin_plate_comments_result($feedback_data->step2_id, $feedback_data->step2_data_relation),
			$feedback_data->step3_question1,
			_fetch_exsiting_ddq_fin_plate_comments_result($feedback_data->step3_id, $feedback_data->step3_data_relation),
			$feedback_data->step4_question1,
			_fetch_exsiting_ddq_fin_plate_comments_result($feedback_data->step4_id, $feedback_data->step4_data_relation),
			$feedback_data->step5_question1,
			_fetch_exsiting_ddq_fin_plate_comments_result($feedback_data->step5_id, $feedback_data->step5_data_relation),
			$feedback_data->step6_question1,
			_fetch_exsiting_ddq_fin_plate_comments_result($feedback_data->step6_id, $feedback_data->step6_data_relation)
		);
		$i++;
	} //$pending_data = $pending_q->fetchObject()
	/* check if there are any pending proposals */
	/*if (!$feedback_rows)
	{
	drupal_set_message(t('There are no results.'), 'status');
	return '';
	} //!$pending_rows*/
	$data_header = array(
		'No.',
		'Date of Submission',
		'User',
		'The minimum number of bolts for the fin plate connection are assumed to be 3. Is this assumption ok?',
		'comments',
		'A maximum of two (vertical) lines of bolts are allowed. Is this ok? ',
		'comments',
		'The gap between the edge of the supported beam and the edge (or face) of the support (column or beam, depending on the connectivity) is assumed to be 20 mm.  Is this ok?',
		'comments',
		'The minimum plate thickness should be greater than the supported beam web thickness. Is this ok?',
		'comments',
		'Any other recommendation/ Comments?',
		'comments'
	);
	$output .= theme('table', array(
		'header' => $data_header,
		'rows' => $feedback_rows,
		'sticky' => FALSE
	));
	return $output;
}
function _fetch_exsiting_ddq_fin_plate_comments($step_id, $data_relation)
{
	global $base_url;
	$query_for_comment = db_query("SELECT * FROM ddq_fin_plate_form_comment WHERE step1_form_id = :step1_form_id AND data_relation=:data_relation ORDER BY number ASC", array(
		':step1_form_id' => $step_id,
		':data_relation' => $data_relation
	));
	$comment = array();
	$i = 0;
	if ($query_for_comment != NULL)
	{
		while ($row = $query_for_comment->fetchObject())
		{
			//$comment[$i] = $i + 1 . ") " . $row->comment . "<br><h6 style='text-align: right;'>" . $row->inserted_on . "</h6><br>";
			$comment[$i] = $i + 1 . ") " . $row->comment . "<br><h6 style='text-align: right;'>
			<a href='{$base_url}/ddq-fin-plate/delete/{$row->data_relation}/{$row->id}' >Delete</a><br>" . $row->inserted_on . "</h6><br>";
			$i++;
		} //$row = $query_for_comment->fetchObject()
	} //$query_for_comment == NULL
	else
	{
		$comment[$i] = 'No earlier comments';
	}
	return $comment;
}
function ddq_fin_plate_delete_comment()
{
	global $base_url;
	$comment_id = arg(3);
	$comment_step = arg(2);
	//var_dump($comment_id .'----');die;
	if ($comment_id)
	{
		//$query = "DELETE FROM ddq_fin_plate_form_comment WHERE id = :id";
		$query_result = db_query("DELETE FROM ddq_fin_plate_form_comment WHERE id = :id", array(
			':id' => $comment_id
		));
		if ($query_result->rowCount() > 0)
		{
			drupal_set_message("Deleted");
		} //$query_result->rowCount() > 0
		else
		{
			drupal_set_message("MySQL Error: row deletion failed.");
		}
	} //$comment_id && user_access("osdag_feedback_form")
	drupal_goto("{$base_url}/ddq-fin-plate/form/{$comment_step}/");
}
function _fetch_exsiting_ddq_fin_plate_comments_result($step_id, $data_relation)
{
	$query_for_comment = db_query("SELECT * FROM ddq_fin_plate_form_comment WHERE step1_form_id = :step1_form_id AND data_relation=:data_relation ORDER BY number ASC", array(
		':step1_form_id' => $step_id,
		':data_relation' => $data_relation
	));
	$text = "";
	$i = 0;
	while ($row = $query_for_comment->fetchObject())
	{
		if (strlen($row->comment) != 0)
		{
			$text .= $i + 1 . ") " . $row->comment . "<br>";
		} //strlen($row->comment) != 0
		else
		{
			$text .= 'No earlier comments';
		}
		$i++;
	} //$row = $query_for_comment->fetchObject()
	//$query_for_comment == NULL
	return $text;
}
function _fetch_exsiting_ddq_fin_plate_comments_result_csv($step_id, $data_relation)
{
	$query_for_comment = db_query("SELECT * FROM ddq_fin_plate_form_comment WHERE step1_form_id = :step1_form_id AND data_relation=:data_relation ORDER BY number ASC", array(
		':step1_form_id' => $step_id,
		':data_relation' => $data_relation
	));
	$text = "";
	$i = 0;
	while ($row = $query_for_comment->fetchObject())
	{
		if (strlen($row->comment) != 0)
		{
			$text .= $i + 1 . ") " . $row->comment . "\n";
		} //strlen($row->comment) != 0
		else
		{
			$text .= 'No earlier comments';
		}
		$i++;
	} //$row = $query_for_comment->fetchObject()
	//$query_for_comment == NULL
	return $text;
}
function ddq_fin_plate_result_generate_csv()
{
	global $user;
	if ($user->uid != NULL)
	{
		$result_dir = $_SERVER['DOCUMENT_ROOT'] . base_path() . "osdag-downloads/osdag-response-form-result/{$user->uid}/";
		if (!file_exists($result_dir))
		{
			mkdir($result_dir, 0755, TRUE);
		} //!file_exists($result_dir)
		$result_file = $result_dir . "ddcl-result-{$user->uid}" . ".csv";
		$csv_file = "ddcl-result-{$user->uid}" . ".csv";
		$fp = fopen($result_file, "w");
		/* making the first row */
		$item = array(
			'No.',
			'Date of Submission',
			'User',
			'Company / Institute',
			'Current Position',
			'The minimum number of bolts for the fin plate connection are assumed to be 3. Is this assumption ok?',
			'comments',
			'A maximum of two (vertical) lines of bolts are allowed. Is this ok? ',
			'comments',
			'The gap between the column edge and beam edge, beam-beam edge is assumed to be 20 mm. Is this ok?',
			'comments',
			'The minimum plate thickness should be greater than the supported beam web thickness. Is this ok?',
			'comments',
			'Any other recomendation',
			'comments'
		);
		fputcsv($fp, $item);
		$query = '

select

step1.uid as "step1_uid",
step1.name as "step1_name",
step1.institute as "step1_institute",
step1.current_position as "step1_current_position",
step1.inserted_on as "step1_date",

step2.id as "step2_id",
step2.data_relation as "step2_data_relation",
step2.question1 as "step2_question1", 
/* step2.any_other_recommendation as "step2_any_other", */

step3.id as "step3_id",
step3.data_relation as "step3_data_relation",
step3.question1 as "step3_question1", 
/* step3.question2 as "step3_question2", */

step4.id as "step4_id",
step4.data_relation as "step4_data_relation",
step4.question1 as "step4_question1",

step5.id as "step5_id",
step5.data_relation as "step5_data_relation",
step5.question1 as "step5_question1", 
/* step5.what_IS_code_clauses_and_references_should_be_followed as "step5_question2", */

step6.id as "step6_id",
step6.data_relation as "step6_data_relation",
step6.question1 as "step6_question1"

from ddq_fin_plate_form_step1_feedback step1

join ddq_fin_plate_form_step2_feedback step2 on step1.uid=step2.uid
join ddq_fin_plate_form_step3_feedback step3 on step2.uid=step3.uid
join ddq_fin_plate_form_step4_feedback step4 on step3.uid=step4.uid
join ddq_fin_plate_form_step5_feedback step5 on step4.uid=step5.uid
join ddq_fin_plate_form_step6_feedback step6 on step5.uid=step6.uid
			';
		// $args = array(');
		$result = db_query($query);
		$i = 1;
		while ($feedback_data = $result->fetchObject())
		{
			$item = array(
				$i,
				$feedback_data->step1_date,
				$feedback_data->step1_name,
				$feedback_data->step1_institute,
				$feedback_data->step1_current_position,
				$feedback_data->step2_question1,
				_fetch_exsiting_ddq_fin_plate_comments_result_csv($feedback_data->step2_id, $feedback_data->step2_data_relation),
				$feedback_data->step3_question1,
				_fetch_exsiting_ddq_fin_plate_comments_result_csv($feedback_data->step3_id, $feedback_data->step3_data_relation),
				$feedback_data->step4_question1,
				_fetch_exsiting_ddq_fin_plate_comments_result_csv($feedback_data->step4_id, $feedback_data->step4_data_relation),
				$feedback_data->step5_question1,
				_fetch_exsiting_ddq_fin_plate_comments_result_csv($feedback_data->step5_id, $feedback_data->step5_data_relation),
				$feedback_data->step6_question1,
				_fetch_exsiting_ddq_fin_plate_comments_result_csv($feedback_data->step6_id, $feedback_data->step6_data_relation)
			);
			fputcsv($fp, $item);
		} //$feedback_data = $result->fetchObject()
		$i++;
		fclose($fp);
		$file = $result_file;
		if (file_exists($file))
		{
			header('Content-Description: File Transfer');
			header('Content-Type: application/octet-stream');
			header('Content-Disposition: attachment; filename=' . basename($file));
			header('Expires: 0');
			header('Cache-Control: must-revalidate');
			header('Pragma: public');
			header('Content-Length: ' . filesize($file));
			ob_clean();
			flush();
			readfile($file);
			unlink($file);
			exit;
		} //file_exists($file)
	} //!$user->uid
	else
	{
		drupal_set_message(t('You do not have permission to access this page.'), 'error');
		drupal_goto('');
	}
}
