<?php
/**
 * Fetches and displays the form.
 */
function ddcl_seated_angle_form_page()
{
	global $user;
	$data = '';
	if (!$user)
	{
		$data .= "Please login to give feedback";
	} //!$user
	/************* check if user has already submitted a proposal **************/
	$query = db_select('ddcl_seated_angle_form_step1_feedback');
	$query->fields('ddcl_seated_angle_form_step1_feedback');
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
				$ddcl_seated_angle_step1_form = drupal_get_form("ddcl_seated_angle_step1_form");
				$data .= drupal_render($ddcl_seated_angle_step1_form);
			}
		} //$feedback_data = $feedback_q->fetchObject()
		else
		{
			$ddcl_seated_angle_step1_form = drupal_get_form("ddcl_seated_angle_step1_form");
			$data .= drupal_render($ddcl_seated_angle_step1_form);
		}
	} //$feedback_q
	//$data .= drupal_get_form('ddcl_seated_angle_step1_form');
	return $data;
}
function ddcl_seated_angle_step1_form($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	global $base_url;
	$markup_text = "The design and detailing checks to be followed for seated angle connection are listed in the following
pages. Please give your feedback as comments on each item.";
	// All the real form fields.
	$form = array();
	$form['step1_header'] = array(
		'#markup' => $markup_text,
		'#required' => true
	);
	$query = db_select('ddcl_seated_angle_form_step1_feedback');
	$query->fields('ddcl_seated_angle_form_step1_feedback');
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
			'ddcl_seated_angle_step1_form_submit'
		)
	);
	return $form;
}
function ddcl_seated_angle_step1_form_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$query = db_select('ddcl_seated_angle_form_step1_feedback');
	$query->fields('ddcl_seated_angle_form_step1_feedback');
	$query->condition('uid', $user->uid);
	$form_step1_data = $query->execute();
	if ($form_step1_data)
	{
		if ($step1_data = $form_step1_data->fetchObject())
		{
			$query = "UPDATE ddcl_seated_angle_form_step1_feedback SET name=:name, email_id=:email_id, institute=:institute, current_position=:current_position WHERE uid=:uid ";
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
			$query = "INSERT INTO {ddcl_seated_angle_form_step1_feedback} (uid, name, email_id, institute, current_position, data_reletion, inserted_on) VALUES (:uid,:name,:email_id, :institute, :current_position, :data_reletion,:inserted_on)";
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
	$form_state['redirect'] = 'ddcl-seated-angle/form/step2/' . $user->uid;
}
/*************************************************************************/
/************************ Step 2 form starts *********************/
function ddcl_seated_angle_step2_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	$markup_text = '<strong> a. Seated Angle</strong><br> <p><strong>i. Bearing width</strong> of seated angle = width of supported beam [based on general practice]</p>
	<hr>';
	$form = array();
	drupal_set_title(t('a. Seated Angle > i. Bearing width design check'));
	$form['step2_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step2-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_seated_angle_form_step2_feedback');
	$query->fields('ddcl_seated_angle_form_step2_feedback');
	$query->condition('uid', $user->uid);
	$form_step2_data = $query->execute();
	if ($form_step2_data)
	{
		if ($step2_data = $form_step2_data->fetchObject())
		{
			$form['step2_field1_fieldset']['step2_field1_question1'] = array(
				'#markup' => $markup_text,
				'#required' => true
			);
			$form['step2_field1_fieldset']['step2_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok?</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				'#default_value' => $step2_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step2_field1_fieldset']['step2_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				'#default_value' => $step2_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step2_field1_fieldset\\[step2_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			$earlier_comments = _fetch_exsiting_ddcl_seated_angle_comments($step2_data->id, $step2_data->data_relation);
			$icom = 1;
			$form['step2_field1_fieldset']['exsiting_comments_title'] = array(
				'#markup' => '<hr><strong>Earlier comment</strong><hr><br>'
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
					'ddcl_seated_angle_step2_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_seated_angle_step2_form_next_submit'
				)
			);
		} //$step2_data = $form_step2_data->fetchObject()
		else
		{
			$form['step2_field1_fieldset']['step2_field1_question1'] = array(
				'#markup' => $markup_text
			);
			$form['step2_field1_fieldset']['step2_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok?</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				'#required' => true
			);
			$form['step2_field1_fieldset']['step2_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not?',
				'#type' => 'textarea',
				'#states' => array(
					'visible' => array(
						':input[name="step2_field1_fieldset\\[step2_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
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
					'ddcl_seated_angle_step2_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_seated_angle_step2_form_next_submit'
				)
			);
		} //$step2_data = $form_step2_data->fetchObject()
	} //$form_step2_data
	return $form;
}
function ddcl_seated_angle_step2_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step2_field1_fieldset']['step2_field1_question1'] = 'question 1';
	$query = db_select('ddcl_seated_angle_form_step2_feedback');
	$query->fields('ddcl_seated_angle_form_step2_feedback');
	$query->condition('uid', $user->uid);
	$form_step2_data = $query->execute()->fetchObject();
	if ($form_step2_data)
	{
		$query = "UPDATE ddcl_seated_angle_form_step2_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => 'question 1',
			":question_ok_not_ok" => $v['step2_field1_fieldset']['step2_field1_question_ok'],
			":question_not_ok_comment" => $v['step2_field1_fieldset']['step2_field1_question_not_ok_comment'],
			":data_relation" => "step2"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_seated_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step2_data->id
		));
		$form_step2_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step2_field1_fieldset']["step2_comment_count"]; $i++)
		{
			if ($v['step2_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_seated_angle_form_step2_feedback} (uid, question1,question_ok_not_ok,question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1,:question_ok_not_ok,:question_not_ok_comment, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step2_field1_fieldset']['step2_field1_question1'],
			":question_ok_not_ok" => $v['step2_field1_fieldset']['step2_field1_question_ok'],
			":question_not_ok_comment" => $v['step2_field1_fieldset']['step2_field1_question_not_ok_comment'],
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
	INSERT INTO ddcl_seated_angle_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-seated-angle/form/step1/' . $user->uid;
}
function ddcl_seated_angle_step2_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step2_field1_fieldset']['step2_field1_question1'] = 'question 1';
	$query = db_select('ddcl_seated_angle_form_step2_feedback');
	$query->fields('ddcl_seated_angle_form_step2_feedback');
	$query->condition('uid', $user->uid);
	$form_step2_data = $query->execute()->fetchObject();
	if ($form_step2_data)
	{
		$query = "UPDATE ddcl_seated_angle_form_step2_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => 'question 1',
			":question_ok_not_ok" => $v['step2_field1_fieldset']['step2_field1_question_ok'],
			":question_not_ok_comment" => $v['step2_field1_fieldset']['step2_field1_question_not_ok_comment'],
			":data_relation" => "step2"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_seated_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step2_data->id
		));
		$form_step2_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step2_field1_fieldset']["step2_comment_count"]; $i++)
		{
			if ($v['step2_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_seated_angle_form_step2_feedback} (uid, question1,question_ok_not_ok,question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1,:question_ok_not_ok,:question_not_ok_comment, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step2_field1_fieldset']['step2_field1_question1'],
			":question_ok_not_ok" => $v['step2_field1_fieldset']['step2_field1_question_ok'],
			":question_not_ok_comment" => $v['step2_field1_fieldset']['step2_field1_question_not_ok_comment'],
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
	INSERT INTO ddcl_seated_angle_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-seated-angle/form/step3/' . $user->uid;
}
/***************************************/
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
	} //$form_state['step3_num_comment'] > 1
	$form_state['rebuild'] = TRUE;
}
function step2_comment_add_more_callback($form, &$form_state)
{
	return $form['step2_field1_fieldset'];
}
/***************************************** step 3 form starts **************************/
function ddcl_seated_angle_step3_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('a. Seated Angle > ii. Length of outstanding leg > Web local crippling check'));
	$markup_text = '<p style="margin-bottom: 0cm; line-height: 100%;">
	<strong> a. Seated Angle</strong><br> <br><strong>ii. Bearing length</strong> (length of outstanding leg of seated angle) is governed by <strong>web local crippling </strong>of supported beam</p>
<p style="margin-bottom: 0cm; line-height: 100%;">&nbsp;</p>
<p style="margin-bottom: 0cm; line-height: 100%;">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; 1) The length of the outstanding leg of the seated angle is calculated on the basis of web crippling of the beam. The outstanding leg length is kept more than the calculated bearing length given by [Cl 8.7.4]</p>

<p style="margin-bottom: 0cm; line-height: 100%;">&nbsp;</p>
<p style="margin-bottom: 0cm; line-height: 100%;">&nbsp;</p>
<p style="margin-bottom: 0cm; line-height: 100%;">

<div style="font-size:20px;">
	<center>
	<math xmlns="http://www.w3.org/1998/Math/MathML">
  <mi>b</mi>
  <mo>=</mo>
  <mfrac>
    <mi>R</mi>
    <mrow>
      <msub>
        <mi>t</mi>
        <mi>w</mi>
      </msub>
      <mfrac>
        <msub>
          <mi>f</mi>
          <mrow class="MJX-TeXAtom-ORD">
            <mi>y</mi>
            <mi>w</mi>
          </mrow>
        </msub>
        <msub>
          <mi>&#x03B3;<!-- γ --></mi>
          <mrow class="MJX-TeXAtom-ORD">
            <mi>m</mi>
            <mn>0</mn>
          </mrow>
        </msub>
      </mfrac>
    </mrow>
  </mfrac>
</math>
</center>
	</div>

</p>
<p style="margin-bottom: 0cm; line-height: 100%;">&nbsp;</p>
<p style="margin-bottom: 0cm; line-height: 100%;">Where,</p>
<p style="margin-bottom: 0cm; line-height: 100%;">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<math xmlns="http://www.w3.org/1998/Math/MathML">
  <mi>R</mi>
</math> is the reaction from the beam,</p>

<p style="margin-bottom: 0cm; line-height: 100%;">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>t</mi>
    <mi>w</mi>
  </msub>
</math> is the thickness of the web of the beam,</p>

<p style="margin-bottom: 0cm; line-height: 100%;">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>f</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>y</mi>
      <mi>w</mi>
    </mrow>
  </msub>
</math> is the yield strength of the web of the beam</p>

<p style="margin-bottom: 0cm; line-height: 100%;">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>&#x03B3;<!-- γ --></mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>m</mi>
      <mn>0</mn>
    </mrow>
  </msub>
</math> is the partial safety factor for material = 1.10</p>

<p style="margin-bottom: 0cm; line-height: 100%;">&nbsp;</p>

<p style="margin-bottom: 0cm; line-height: 100%;">A dispersion of 45° is taken from the bearing on the cleat to the root line [Cl 8.7.1.3]
<br><br>
Note: The dispersion of 1:2.5 as given in Cl 8.7.4 is not applicable here. Instead a dispersion of 45
degrees is assumed as given in Cl 8.7.1.3.

</p>

';
	$markup_text1 = 'Bearing stiffeners should be provided for webs where forces applied through a flange
by loads or reactions exceeding the local capacity of the web at its connection to the
flange given by <br>

<center>
 
 <math xmlns="http://www.w3.org/1998/Math/MathML">
<msub>
<mi>F</mi>
<mi>w</mi>
</msub>
<mo>=</mo>
<mo stretchy="false">(</mo>
<msub>
<mi>b</mi>
<mn>1</mn>
</msub>
<mo>+</mo>
<msub>
<mi>n</mi>
<mn>2</mn>
</msub>
<mo stretchy="false">)</mo>
<msub>
<mi>t</mi>
<mi>w</mi>
</msub>
<msub>
<mi>f</mi>
<mrow class="MJX-TeXAtom-ORD">
<mi>y</mi>
<mi>w</mi>
</mrow>
</msub>
<mrow class="MJX-TeXAtom-ORD">
<mo>/</mo>
</mrow>
<msub>
<mi>&#x03B3;<!-- γ --></mi>
<mrow class="MJX-TeXAtom-ORD">
<mi>m</mi>
<mn>0</mn>
</mrow>
</msub>
</math>
</center>
<br>

<math xmlns="http://www.w3.org/1998/Math/MathML">

<msub>
    <mi>b</mi>
    <mn>1</mn>
  </msub>

 </math>
 = stiff bearing length

<br>
<math xmlns="http://www.w3.org/1998/Math/MathML">

<msub>
    <mi>n</mi>
    <mn>2</mn>
  </msub>

</math>

 = length obtained by dispersion through the flange to the web junction at a slope of 1:2.5 to the plane of the flange
<br>
<math xmlns="http://www.w3.org/1998/Math/MathML">

<msub>
    <mi>t</mi>
    <mn>w</mn>
  </msub>

</math>
 = thickness of the web, and

 <br>

<math xmlns="http://www.w3.org/1998/Math/MathML">

<msub>
    <mi>f</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>y</mi>
      <mi>w</mi>
    </mrow>
  </msub>

</math>

 = yield stress of the web
 



';
	$tooltip1 = '[Cl 8.7.4] Bearing Stiffeners';
	// All the real form fields.
	$form = array();
	$form['step3_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step3-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_seated_angle_form_step3_feedback');
	$query->fields('ddcl_seated_angle_form_step3_feedback');
	$query->condition('uid', $user->uid);
	$form_step3_data = $query->execute();
	if ($form_step3_data)
	{
		if ($step3_data = $form_step3_data->fetchObject())
		{
			$form['step3_field1_fieldset']['step3_field1_question1'] = array(
				'#markup' => $markup_text
			);
			$form['step3_field1_fieldset']['tooltip1'] = array(
				'#type' => 'fieldset',
				'#title' => $tooltip1,
				'#collapsible' => TRUE, // Added
				'#collapsed' => TRUE // Added
			);
			$form['step3_field1_fieldset']['tooltip1']['tooltip1_text'] = array(
				'#markup' => $markup_text1
			);
			$form['step3_field1_fieldset']['step3_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok?</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				'#default_value' => $step3_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step3_field1_fieldset']['step3_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not?',
				'#type' => 'textarea',
				'#default_value' => $step3_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step3_field1_fieldset\\[step3_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			$earlier_comments = _fetch_exsiting_ddcl_seated_angle_comments($step3_data->id, $step3_data->data_relation);
			$icom = 1;
			$form['step3_field1_fieldset']['exsiting_comments_title'] = array(
				'#markup' => '<hr><strong>Earlier comment</strong><hr><br>'
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
					'ddcl_seated_angle_step3_form_previous_submit'
				)
			);
			$form['next3'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_seated_angle_step3_form_next_submit'
				)
			);
		} //$step3_data = $form_step3_data->fetchObject()
		else
		{
			$form['step3_field1_fieldset']['step3_field1_question1'] = array(
				'#markup' => $markup_text
			);
			$form['step3_field1_fieldset']['step3_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok?</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				'#required' => true
			);
			$form['step3_field1_fieldset']['step3_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not?',
				'#type' => 'textarea',
				'#states' => array(
					'visible' => array(
						':input[name="step3_field1_fieldset\\[step3_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
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
					'ddcl_seated_angle_step3_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_seated_angle_step3_form_next_submit'
				)
			);
		} //$step3_data = $form_step3_data->fetchObject()
	} //$form_step3_data
	return $form;
}
function ddcl_seated_angle_step3_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step3_field1_fieldset']['step3_field1_question1'] = 'question 3';
	$query = db_select('ddcl_seated_angle_form_step3_feedback');
	$query->fields('ddcl_seated_angle_form_step3_feedback');
	$query->condition('uid', $user->uid);
	$form_step3_data = $query->execute()->fetchObject();
	//var_dump($form_step2_data);die;
	if ($form_step3_data)
	{
		$query = "UPDATE ddcl_seated_angle_form_step3_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step3_field1_fieldset']['step3_field1_question1'],
			":question_ok_not_ok" => $v['step3_field1_fieldset']['step3_field1_question_ok'],
			":question_not_ok_comment" => $v['step3_field1_fieldset']['step3_field1_question_not_ok_comment'],
			":data_relation" => "step3"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_seated_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step3_data->id
		));
		$form_step3_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step3_field1_fieldset']["step3_comment_count"]; $i++)
		{
			if ($v['step3_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_seated_angle_form_step3_feedback} (uid, question1,question_ok_not_ok,question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok,:question_not_ok_comment, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step3_field1_fieldset']['step3_field1_question1'],
			":question_ok_not_ok" => $v['step3_field1_fieldset']['step3_field1_question_ok'],
			":question_not_ok_comment" => $v['step3_field1_fieldset']['step3_field1_question_not_ok_comment'],
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
	INSERT INTO ddcl_seated_angle_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-seated-angle/form/step2/' . $user->uid;
}
function ddcl_seated_angle_step3_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step3_field1_fieldset']['step3_field1_question1'] = 'question 3';
	$query = db_select('ddcl_seated_angle_form_step3_feedback');
	$query->fields('ddcl_seated_angle_form_step3_feedback');
	$query->condition('uid', $user->uid);
	$form_step3_data = $query->execute()->fetchObject();
	//var_dump($form_step2_data);die;
	if ($form_step3_data)
	{
		$query = "UPDATE ddcl_seated_angle_form_step3_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step3_field1_fieldset']['step3_field1_question1'],
			":question_ok_not_ok" => $v['step3_field1_fieldset']['step3_field1_question_ok'],
			":question_not_ok_comment" => $v['step3_field1_fieldset']['step3_field1_question_not_ok_comment'],
			":data_relation" => "step3"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_seated_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step3_data->id
		));
		$form_step3_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step3_field1_fieldset']["step3_comment_count"]; $i++)
		{
			if ($v['step3_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_seated_angle_form_step3_feedback} (uid, question1,question_ok_not_ok,question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok,:question_not_ok_comment, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step3_field1_fieldset']['step3_field1_question1'],
			":question_ok_not_ok" => $v['step3_field1_fieldset']['step3_field1_question_ok'],
			":question_not_ok_comment" => $v['step3_field1_fieldset']['step3_field1_question_not_ok_comment'],
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
	INSERT INTO ddcl_seated_angle_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-seated-angle/form/step4/' . $user->uid;
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
function ddcl_seated_angle_step4_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('a. Seated Angle > iii. Angle thickness > 1) Moment capacity check'));
	$markup_text = '<p><strong>a. Seated Angle <br>iii. Angle thickness</strong> is governed by <strong>shear yielding and flexural yielding of angle</strong><br />
<strong>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; 1) Moment capacity check of the outstanding leg</strong><br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; a) [Cl 8.2.1.2] For low shear force<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; When the factored design shear force, 

<math xmlns="http://www.w3.org/1998/Math/MathML">
  <mi>V</mi>
</math>, does not exceed 0.6 <math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>V</mi>
    <mi>d</mi>
  </msub>
</math><br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; Where,<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;&nbsp;
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>V</mi>
    <mi>d</mi>
  </msub>
</math>

 is the design shear strength of the cross-section, the design bending strength, <math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>M</mi>
    <mi>d</mi>
  </msub>
</math>
  shall be taken as:
 
<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>M</mi>
    <mi>d</mi>
  </msub>
  <mo>=</mo>
  <mfrac>
    <mrow>
      <msub>
        <mi>&#x03B2;<!-- β --></mi>
        <mi>b</mi>
      </msub>
      <msub>
        <mi>Z</mi>
        <mi>p</mi>
      </msub>
      <msub>
        <mi>f</mi>
        <mi>y</mi>
      </msub>
    </mrow>
    <msub>
      <mi>&#x03B3;<!-- γ --></mi>
      <mrow class="MJX-TeXAtom-ORD">
        <mi>m</mi>
        <mn>0</mn>
      </mrow>
    </msub>
  </mfrac>
</math>
</p>

<p>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; To avoid irreversible deformation under serviceability loads in cantilevers,

<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>M</mi>
    <mi>d</mi>
  </msub>
  <mo>&lt;</mo>
  <mfrac>
    <mrow>
      <mn>1.5</mn>
      <msub>
        <mi>Z</mi>
        <mi>e</mi>
      </msub>
      <msub>
        <mi>f</mi>
        <mi>y</mi>
      </msub>
    </mrow>
    <msub>
      <mi>&#x03B3;<!-- γ --></mi>
      <mrow class="MJX-TeXAtom-ORD">
        <mi>m</mi>
        <mn>0</mn>
      </mrow>
    </msub>
  </mfrac>
</math>
 <br />
<br>

 &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; 
Where,<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; &nbsp; &nbsp; &nbsp; &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; 

<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>&#x03B2;<!-- β --></mi>
    <mi>b</mi>
  </msub>
</math>
= 1.0 for plastic and compact sections;<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;&nbsp; 
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>&#x03B2;<!-- β --></mi>
    <mi>b</mi>
  </msub>
  <mo>=</mo>
  <msub>
    <mi>Z</mi>
    <mi>e</mi>
  </msub>
  <mrow class="MJX-TeXAtom-ORD">
    <mo>/</mo>
  </mrow>
  <msub>
    <mi>Z</mi>
    <mi>p</mi>
  </msub>
</math>
 for semi-compact sections;<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; 

<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>Z</mi>
    <mi>p</mi>
  </msub>
  <mo>,</mo>
  <msub>
    <mi>Z</mi>
    <mi>e</mi>
  </msub>
</math>
= plastic and elastic section moduli of the cross-section, respectively;<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; 
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>f</mi>
    <mi>y</mi>
  </msub>
</math>
= yield stress of the material; and<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; 

<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>&#x03B3;<!-- γ --></mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>m</mi>
      <mn>0</mn>
    </mrow>
  </msub>
</math>


 = partial safety factor for material</p>

<hr>
';
	// All the real form fields.
	$form = array();
	$form['step4_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step4-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_seated_angle_form_step4_feedback');
	$query->fields('ddcl_seated_angle_form_step4_feedback');
	$query->condition('uid', $user->uid);
	$form_step4_data = $query->execute();
	if ($form_step4_data)
	{
		if ($step4_data = $form_step4_data->fetchObject())
		{
			$form['step4_field1_fieldset']['step4_field1_question1'] = array(
				'#markup' => $markup_text,
			);
			$form['step4_field1_fieldset']['step4_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok?</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				'#default_value' => $step4_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step4_field1_fieldset']['step4_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				'#default_value' => $step4_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step4_field1_fieldset\\[step4_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			$earlier_comments = _fetch_exsiting_ddcl_seated_angle_comments($step4_data->id, $step4_data->data_relation);
			$icom = 1;
			$form['step4_field1_fieldset']['exsiting_comments_title'] = array(
				'#markup' => '<hr>Earlier comment<hr><br>'
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
					'ddcl_seated_angle_step4_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_seated_angle_step4_form_next_submit'
				)
			);
		} //$step4_data = $form_step4_data->fetchObject()
		else
		{
			$form['step4_field1_fieldset']['step4_field1_question1'] = array(
				'#markup' => $markup_text,
			);
			$form['step4_field1_fieldset']['step4_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok?</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				'#required' => true
			);
			$form['step4_field1_fieldset']['step4_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				'#states' => array(
					'visible' => array(
						':input[name="step4_field1_fieldset\\[step4_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
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
					'ddcl_seated_angle_step4_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_seated_angle_step4_form_next_submit'
				)
			);
		} //$step4_data = $form_step4_data->fetchObject()
	} //$form_step4_data
	return $form;
}
function ddcl_seated_angle_step4_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step4_field1_fieldset']['step4_field1_question1'] = 'question 1';
	$query = db_select('ddcl_seated_angle_form_step4_feedback');
	$query->fields('ddcl_seated_angle_form_step4_feedback');
	$query->condition('uid', $user->uid);
	$form_step4_data = $query->execute()->fetchObject();
	//var_dump($form_step4_data);die;
	if ($form_step4_data)
	{
		$query = "UPDATE ddcl_seated_angle_form_step4_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step4_field1_fieldset']['step4_field1_question1'],
			":question_ok_not_ok" => $v['step4_field1_fieldset']['step4_field1_question_ok'],
			":question_not_ok_comment" => $v['step4_field1_fieldset']['step4_field1_question_not_ok_comment'],
			":data_relation" => "step4"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_seated_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step4_data->id
		));
		$form_step4_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step4_field1_fieldset']["step4_comment_count"]; $i++)
		{
			if ($v['step4_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_seated_angle_form_step4_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step4_field1_fieldset']['step4_field1_question1'],
			":question_ok_not_ok" => $v['step4_field1_fieldset']['step4_field1_question_ok'],
			":question_not_ok_comment" => $v['step4_field1_fieldset']['step4_field1_question_not_ok_comment'],
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
	INSERT INTO ddcl_seated_angle_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-seated-angle/form/step3/' . $user->uid;
}
function ddcl_seated_angle_step4_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step4_field1_fieldset']['step4_field1_question1'] = 'question 1';
	$query = db_select('ddcl_seated_angle_form_step4_feedback');
	$query->fields('ddcl_seated_angle_form_step4_feedback');
	$query->condition('uid', $user->uid);
	$form_step4_data = $query->execute()->fetchObject();
	//var_dump($form_step4_data);die;
	if ($form_step4_data)
	{
		$query = "UPDATE ddcl_seated_angle_form_step4_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step4_field1_fieldset']['step4_field1_question1'],
			":question_ok_not_ok" => $v['step4_field1_fieldset']['step4_field1_question_ok'],
			":question_not_ok_comment" => $v['step4_field1_fieldset']['step4_field1_question_not_ok_comment'],
			":data_relation" => "step4"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_seated_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step4_data->id
		));
		$form_step4_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step4_field1_fieldset']["step4_comment_count"]; $i++)
		{
			if ($v['step4_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_seated_angle_form_step4_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step4_field1_fieldset']['step4_field1_question1'],
			":question_ok_not_ok" => $v['step4_field1_fieldset']['step4_field1_question_ok'],
			":question_not_ok_comment" => $v['step4_field1_fieldset']['step4_field1_question_not_ok_comment'],
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
	INSERT INTO ddcl_seated_angle_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-seated-angle/form/step5/' . $user->uid;
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
function ddcl_seated_angle_step5_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('a. Seated Angle > iii. Angle thickness > 1) Moment capacity check'));
	$markup_text = '<p><strong>a. Seated Angle <br>iii. Angle thickness</strong> is governed by <strong>shear yielding and flexural yielding of angle</strong><br />
<strong>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; 1) Moment capacity check of the outstanding leg</strong><br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
b) [CL 8.2.1.3] For high shear force<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; When the design shear force (factored), 
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <mi>V</mi>
</math>
 , exceeds 0.6 
 <math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>V</mi>
    <mi>d</mi>
  </msub>
</math>
 
 <br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; Where,<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; 
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>V</mi>
    <mi>d</mi>
  </msub>
</math>
 is the design shear strength of the cross-section the design bending strength, 
 <math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>M</mi>
    <mi>d</mi>
  </msub>
</math>
  shall be taken as 
  <math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>M</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>d</mi>
      <mi>v</mi>
    </mrow>
  </msub>
</math>
   (= design bending strength under high shear as defined in Cl 9.2)</p>

<p>[CL 9.2.2]<br />
When the factored value of the applied shear force is high (exceeds the limit specified in Cl 9.2.1), the factored moment of the section should be less than the moment capacity of the section under higher shear force, 
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>M</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>d</mi>
      <mi>v</mi>
    </mrow>
  </msub>
</math>
 , for plastic or compact section is calculated as given below:
 <math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>M</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>d</mi>
      <mi>v</mi>
    </mrow>
  </msub>
  <mo>=</mo>
  <mi>m</mi>
  <mi>i</mi>
  <mi>n</mi>
  <mrow class="MJX-TeXAtom-ORD">
    <mo>&#x2061;</mo>
  </mrow>
  <mrow>
    <mo>{</mo>
    <mrow>
      <msub>
        <mi>M</mi>
        <mi>d</mi>
      </msub>
      <mo>&#x2212;</mo>
      <mi>&#x03B2;<!-- β --></mi>
      <mo stretchy="false">(</mo>
      <msub>
        <mi>M</mi>
        <mi>d</mi>
      </msub>
      <mo>&#x2212;</mo>
      <msub>
        <mi>M</mi>
        <mrow class="MJX-TeXAtom-ORD">
          <mi>f</mi>
          <mi>d</mi>
        </mrow>
      </msub>
      <mo stretchy="false">)</mo>
      <mo>,</mo>
      <mfrac>
        <mrow>
          <mn>1.2</mn>
          <msub>
            <mi>Z</mi>
            <mi>e</mi>
          </msub>
          <msub>
            <mi>f</mi>
            <mi>y</mi>
          </msub>
        </mrow>
        <msub>
          <mi>&#x03B3;<!-- γ --></mi>
          <mrow class="MJX-TeXAtom-ORD">
            <mi>m</mi>
            <mn>0</mn>
          </mrow>
        </msub>
      </mfrac>
    </mrow>
    <mo>}</mo>
  </mrow>
</math>

<p>[Note: Assuming, 
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>M</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>f</mi>
      <mi>d</mi>
    </mrow>
  </msub>
</math>
= 0, as the shear resisting area and moment resisting area are the same for the cross section of the outstanding leg] =</p>

<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>M</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>d</mi>
      <mi>v</mi>
    </mrow>
  </msub>
  <mo>=</mo>
  <mi>m</mi>
  <mi>i</mi>
  <mi>n</mi>
  <mrow class="MJX-TeXAtom-ORD">
    <mo>&#x2061;</mo>
  </mrow>
  <mrow>
    <mo>{</mo>
    <mrow>
      <mo stretchy="false">(</mo>
      <mn>1</mn>
      <mo>&#x2212;</mo>
      <mi>&#x03B2;<!-- β --></mi>
      <mo stretchy="false">)</mo>
      <msub>
        <mi>M</mi>
        <mi>d</mi>
      </msub>
      <mo>,</mo>
      <mfrac>
        <mrow>
          <mn>1.2</mn>
          <msub>
            <mi>Z</mi>
            <mi>e</mi>
          </msub>
          <msub>
            <mi>f</mi>
            <mi>y</mi>
          </msub>
        </mrow>
        <msub>
          <mi>&#x03B3;<!-- γ --></mi>
          <mrow class="MJX-TeXAtom-ORD">
            <mi>m</mi>
            <mn>0</mn>
          </mrow>
        </msub>
      </mfrac>
    </mrow>
    <mo>}</mo>
  </mrow>
</math>

<p>Where,<br />
<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <mi>&#x03B2;<!-- β --></mi>
  <mo>=</mo>
  <msup>
    <mrow>
      <mo>{</mo>
      <mrow>
        <mfrac>
          <mrow>
            <mn>2</mn>
            <mi>V</mi>
          </mrow>
          <msub>
            <mi>V</mi>
            <mi>d</mi>
          </msub>
        </mfrac>
        <mo>&#x2212;<!-- − --></mo>
        <mn>1</mn>
      </mrow>
      <mo>}</mo>
    </mrow>
    <mn>2</mn>
  </msup>
</math><br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>M</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>d</mi>
      <mi>v</mi>
    </mrow>
  </msub>
</math>
 = design bending strength under high shear<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>M</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>d</mi>
    </mrow>
  </msub>
</math>
 = plastic design moment of the whole section disregarding high shear force effect (see 8.2.1.2) considering web buckling effects (see 8.2.1.1)<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; 
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <mi>V</mi>
</math>
= factored applied shear force as governed by web yielding or web buckling<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>V</mi>
    <mi>d</mi>
  </msub>
</math>
 = design shear strength as governed by web yielding or web buckling (see 8.4.1 or 8.4.2)<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; 
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>M</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>f</mi>
      <mi>d</mi>
    </mrow>
  </msub>
</math>
= plastic design strength of the area of the cross-section excluding the shear area, considering partial safety factor 
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>&#x03B3;<!-- γ --></mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>m</mi>
      <mn>0</mn>
    </mrow>
  </msub>
</math>
, and<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; 
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>Z</mi>
    <mi>e</mi>
  </msub>
</math>
= elastic section modulus of the whole section</p>

<hr>
';
	// All the real form fields.
	$form = array();
	$form['step5_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step5-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_seated_angle_form_step5_feedback');
	$query->fields('ddcl_seated_angle_form_step5_feedback');
	$query->condition('uid', $user->uid);
	$form_step5_data = $query->execute();
	if ($form_step5_data)
	{
		if ($step5_data = $form_step5_data->fetchObject())
		{
			$form['step5_field1_fieldset']['step5_field1_question1'] = array(
				'#markup' => $markup_text,
			);
			$form['step5_field1_fieldset']['step5_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok?</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				'#default_value' => $step5_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step5_field1_fieldset']['step5_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				'#default_value' => $step5_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step5_field1_fieldset\\[step5_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			$earlier_comments = _fetch_exsiting_ddcl_seated_angle_comments($step5_data->id, $step5_data->data_relation);
			$icom = 1;
			$form['step5_field1_fieldset']['exsiting_comments_title'] = array(
				'#markup' => '<hr><strong>Earlier comment</strong><hr><br>'
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
					'ddcl_seated_angle_step5_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_seated_angle_step5_form_next_submit'
				)
			);
		} //$step5_data = $form_step5_data->fetchObject()
		else
		{
			$form['step5_field1_fieldset']['step5_field1_question1'] = array(
				'#markup' => $markup_text,
			);
			$form['step5_field1_fieldset']['step5_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok?</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				//'#default_value' => $step2_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step5_field1_fieldset']['step5_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				//'#default_value' => $step5_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step5_field1_fieldset\\[step5_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
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
					'ddcl_seated_angle_step5_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_seated_angle_step5_form_next_submit'
				)
			);
		} //$step5_data = $form_step5_data->fetchObject()
	} //$form_step5_data
	return $form;
}
function ddcl_seated_angle_step5_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step5_field1_fieldset']['step5_field1_question1'] = 'question 1';
	$query = db_select('ddcl_seated_angle_form_step5_feedback');
	$query->fields('ddcl_seated_angle_form_step5_feedback');
	$query->condition('uid', $user->uid);
	$form_step5_data = $query->execute()->fetchObject();
	//var_dump($form_step5_data);die;
	if ($form_step5_data)
	{
		$query = "UPDATE ddcl_seated_angle_form_step5_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step5_field1_fieldset']['step5_field1_question1'],
			":question_ok_not_ok" => $v['step5_field1_fieldset']['step5_field1_question_ok'],
			":question_not_ok_comment" => $v['step5_field1_fieldset']['step5_field1_question_not_ok_comment'],
			":data_relation" => "step5"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_seated_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step5_data->id
		));
		$form_step5_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step5_field1_fieldset']["step5_comment_count"]; $i++)
		{
			if ($v['step5_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_seated_angle_form_step5_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation, inserted_on) VALUES (:uid, :question1, :question_ok_not_ok,:question_not_ok_comment, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step5_field1_fieldset']['step5_field1_question1'],
			":question_ok_not_ok" => $v['step5_field1_fieldset']['step5_field1_question_ok'],
			":question_not_ok_comment" => $v['step5_field1_fieldset']['step5_field1_question_not_ok_comment'],
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
	INSERT INTO ddcl_seated_angle_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-seated-angle/form/step4/' . $user->uid;
}
function ddcl_seated_angle_step5_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step5_field1_fieldset']['step5_field1_question1'] = 'question 1';
	$query = db_select('ddcl_seated_angle_form_step5_feedback');
	$query->fields('ddcl_seated_angle_form_step5_feedback');
	$query->condition('uid', $user->uid);
	$form_step5_data = $query->execute()->fetchObject();
	//var_dump($form_step5_data);die;
	if ($form_step5_data)
	{
		$query = "UPDATE ddcl_seated_angle_form_step5_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step5_field1_fieldset']['step5_field1_question1'],
			":question_ok_not_ok" => $v['step5_field1_fieldset']['step5_field1_question_ok'],
			":question_not_ok_comment" => $v['step5_field1_fieldset']['step5_field1_question_not_ok_comment'],
			":data_relation" => "step5"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_seated_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step5_data->id
		));
		$form_step5_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step5_field1_fieldset']["step5_comment_count"]; $i++)
		{
			if ($v['step5_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_seated_angle_form_step5_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation, inserted_on) VALUES (:uid, :question1, :question_ok_not_ok,:question_not_ok_comment, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step5_field1_fieldset']['step5_field1_question1'],
			":question_ok_not_ok" => $v['step5_field1_fieldset']['step5_field1_question_ok'],
			":question_not_ok_comment" => $v['step5_field1_fieldset']['step5_field1_question_not_ok_comment'],
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
	INSERT INTO ddcl_seated_angle_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-seated-angle/form/step6/' . $user->uid;
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
/************************ Step 6 form starts *********************/
function ddcl_seated_angle_step6_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('a. Seated Angle > iii. Angle thickness > 2) Shear capacity check'));
	$markup_text = '<p><strong>a. Seated Angle <br>iii. Angle thickness</strong> is governed by <strong>shear yielding and flexural yielding of angle</strong><br />
<strong>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
2) Shear capacity of the outstanding leg of cleat</strong><br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; 
a) [Cl 8.4.1] <strong>Plastic shear resistance under pure shear</strong><br />
<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>V</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>d</mi>
      <mi>p</mi>
    </mrow>
  </msub>
  <mo>=</mo>
  <mfrac>
    <mrow>
      <msub>
        <mi>A</mi>
        <mi>v</mi>
      </msub>
      <msub>
        <mi>f</mi>
        <mrow class="MJX-TeXAtom-ORD">
          <mi>y</mi>
          <mi>w</mi>
        </mrow>
      </msub>
    </mrow>
    <mrow>
      <msqrt>
        <mn>3</mn>
      </msqrt>
      <mo>&#x2217;<!-- ∗ --></mo>
      <msub>
        <mi>&#x03B3;<!-- γ --></mi>
        <mrow class="MJX-TeXAtom-ORD">
          <mi>m</mi>
          <mn>0</mn>
        </mrow>
      </msub>
    </mrow>
  </mfrac>
</math>
<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; Where,<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; &nbsp;&nbsp; 
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>V</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>d</mi>
      <mi>p</mi>
    </mrow>
  </msub>
</math>
 = design plastic shear resistance under pure shear<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>A</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>v</mi>
    </mrow>
  </msub>
  <mo>=</mo>
  <mi>A</mi>
  <mo>=</mo>
  <msub>
    <mi>d</mi>
    <mi>a</mi>
  </msub>
  <msub>
    <mi>t</mi>
    <mi>a</mi>
  </msub>
</math>

(Note:This formula is for a plate, assuming clear horizontal leg of the seated angle as a plate) [Cl 8.4.1.1]<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; 
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>d</mi>
    <mi>a</mi>
  </msub>
</math>
= bearing width of seated angle<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; 
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>t</mi>
    <mi>a</mi>
  </msub>
</math>
= thickness of seated angle<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; 
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>f</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>y</mi>
      <mi>w</mi>
    </mrow>
  </msub>
</math> = yield strength of the web<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; 
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>&#x03B3;<!-- γ --></mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>m</mi>
      <mn>0</mn>
    </mrow>
  </msub>
</math>
= partial safety factor<br />
<br>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
b) [Cl 8.4.2] [Resistance to shear buckling]<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
[Note: Assuming that this clause check does not need to be implemented as the outstanding leg of the seated angle will not buckle due to the low slenderness ratio]</p>



<hr>
';
	// All the real form fields.
	$form = array();
	$form['step6_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step6-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_seated_angle_form_step6_feedback');
	$query->fields('ddcl_seated_angle_form_step6_feedback');
	$query->condition('uid', $user->uid);
	$form_step6_data = $query->execute();
	if ($form_step6_data)
	{
		if ($step6_data = $form_step6_data->fetchObject())
		{
			$form['step6_field1_fieldset']['step6_field1_question1'] = array(
				'#markup' => $markup_text,
			);
			$form['step6_field1_fieldset']['step6_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok?</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				'#default_value' => $step6_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step6_field1_fieldset']['step6_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				'#default_value' => $step6_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step6_field1_fieldset\\[step6_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			$earlier_comments = _fetch_exsiting_ddcl_seated_angle_comments($step6_data->id, $step6_data->data_relation);
			$icom = 1;
			$form['step6_field1_fieldset']['exsiting_comments_title'] = array(
				'#markup' => '<hr>Earlier comment<hr><br>'
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
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_seated_angle_step6_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_seated_angle_step6_form_next_submit'
				)
			);
		} //$step6_data = $form_step6_data->fetchObject()
		else
		{
			$form['step6_field1_fieldset']['step6_field1_question1'] = array(
				'#markup' => $markup_text,
			);
			$form['step6_field1_fieldset']['step6_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok?</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				//'#default_value' => $step6_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step6_field1_fieldset']['step6_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				//'#default_value' => $step6_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step6_field1_fieldset\\[step6_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
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
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_seated_angle_step6_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_seated_angle_step6_form_next_submit'
				)
			);
		} //$step6_data = $form_step6_data->fetchObject()
	} //$form_step6_data
	return $form;
}
function ddcl_seated_angle_step6_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step6_field1_fieldset']['step6_field1_question1'] = 'question 1';
	$query = db_select('ddcl_seated_angle_form_step6_feedback');
	$query->fields('ddcl_seated_angle_form_step6_feedback');
	$query->condition('uid', $user->uid);
	$form_step6_data = $query->execute()->fetchObject();
	//var_dump($form_step6_data);die;
	if ($form_step6_data)
	{
		$query = "UPDATE ddcl_seated_angle_form_step6_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step6_field1_fieldset']['step6_field1_question1'],
			":question_ok_not_ok" => $v['step6_field1_fieldset']['step6_field1_question_ok'],
			":question_not_ok_comment" => $v['step6_field1_fieldset']['step6_field1_question_not_ok_comment'],
			":data_relation" => "step6"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_seated_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step6_data->id
		));
		$form_step6_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step6_field1_fieldset']["step6_comment_count"]; $i++)
		{
			if ($v['step6_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_seated_angle_form_step6_feedback} (uid, question1, question_ok_not_ok,question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step6_field1_fieldset']['step6_field1_question1'],
			":question_ok_not_ok" => $v['step6_field1_fieldset']['step6_field1_question_ok'],
			":question_not_ok_comment" => $v['step6_field1_fieldset']['step6_field1_question_not_ok_comment'],
			":data_relation" => "step6",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$comments = 0;
		for ($i = 0; $i <= $v['step6_field1_fieldset']["step6_comment_count"]; $i++)
		{
			if ($v['step6_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-seated-angle/form/step5/' . $user->uid;
}
function ddcl_seated_angle_step6_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step6_field1_fieldset']['step6_field1_question1'] = 'question 1';
	$query = db_select('ddcl_seated_angle_form_step6_feedback');
	$query->fields('ddcl_seated_angle_form_step6_feedback');
	$query->condition('uid', $user->uid);
	$form_step6_data = $query->execute()->fetchObject();
	//var_dump($form_step6_data);die;
	if ($form_step6_data)
	{
		$query = "UPDATE ddcl_seated_angle_form_step6_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step6_field1_fieldset']['step6_field1_question1'],
			":question_ok_not_ok" => $v['step6_field1_fieldset']['step6_field1_question_ok'],
			":question_not_ok_comment" => $v['step6_field1_fieldset']['step6_field1_question_not_ok_comment'],
			":data_relation" => "step6"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_seated_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step6_data->id
		));
		$form_step6_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step6_field1_fieldset']["step6_comment_count"]; $i++)
		{
			if ($v['step6_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_seated_angle_form_step6_feedback} (uid, question1, question_ok_not_ok,question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step6_field1_fieldset']['step6_field1_question1'],
			":question_ok_not_ok" => $v['step6_field1_fieldset']['step6_field1_question_ok'],
			":question_not_ok_comment" => $v['step6_field1_fieldset']['step6_field1_question_not_ok_comment'],
			":data_relation" => "step6",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$comments = 0;
		for ($i = 0; $i <= $v['step6_field1_fieldset']["step6_comment_count"]; $i++)
		{
			if ($v['step6_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-seated-angle/form/step7/' . $user->uid;
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
/***************************************** step 6 from ends ****************************/
/************************ Step 7 form starts *********************/
function ddcl_seated_angle_step7_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('b. Bolt > i. Shear capacity'));
	$markup_text = '<p><strong>b. Bolt<br /></strong>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; <strong>i.Shear capacity of bolt</strong> [Cl. 10.3.3] <br />


<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>V</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>d</mi>
      <mi>s</mi>
      <mi>b</mi>
    </mrow>
  </msub>
  <mo>=</mo>
  <mfrac>
    <mrow>
      <msub>
        <mi>f</mi>
        <mi>u</mi>
      </msub>
      <mo stretchy="false">(</mo>
      <msub>
        <mi>n</mi>
        <mi>s</mi>
      </msub>
      <msub>
        <mi>A</mi>
        <mrow class="MJX-TeXAtom-ORD">
          <mi>s</mi>
          <mi>b</mi>
        </mrow>
      </msub>
      <mo>+</mo>
      <msub>
        <mi>n</mi>
        <mi>n</mi>
      </msub>
      <msub>
        <mi>A</mi>
        <mrow class="MJX-TeXAtom-ORD">
          <mi>n</mi>
          <mi>b</mi>
        </mrow>
      </msub>
      <mo stretchy="false">)</mo>
    </mrow>
    <mrow>
      <msqrt>
        <mn>3</mn>
      </msqrt>
      <msub>
        <mi>&#x03B3;<!-- γ --></mi>
        <mrow class="MJX-TeXAtom-ORD">
          <mi>m</mi>
          <mi>b</mi>
        </mrow>
      </msub>
    </mrow>
  </mfrac>
</math>
<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; Note: Currently, conservatively assuming &nbsp;  
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>n</mi>
    <mi>n</mi>
  </msub>
  <msub>
    <mi>A</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>n</mi>
      <mi>b</mi>
    </mrow>
  </msub>
  <mo>=</mo>
  <mn>0</mn>
</math>
 &nbsp;that is, all shear planes pass through the threads.<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; The option to specify this as an user input shall be implemented in future.<br />
Thus,<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; 
<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>V</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>d</mi>
      <mi>s</mi>
      <mi>b</mi>
    </mrow>
  </msub>
  <mo>=</mo>
  <mfrac>
    <mrow>
      <msub>
        <mi>f</mi>
        <mi>u</mi>
      </msub>
      <msub>
        <mi>n</mi>
        <mi>n</mi>
      </msub>
      <msub>
        <mi>A</mi>
        <mrow class="MJX-TeXAtom-ORD">
          <mi>n</mi>
          <mi>b</mi>
        </mrow>
      </msub>
    </mrow>
    <mrow>
      <msqrt>
        <mn>3</mn>
      </msqrt>
      <msub>
        <mi>&#x03B3;<!-- γ --></mi>
        <mrow class="MJX-TeXAtom-ORD">
          <mi>m</mi>
          <mi>b</mi>
        </mrow>
      </msub>
    </mrow>
  </mfrac>
</math>
<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; Where,<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>V</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>d</mi>
      <mi>s</mi>
      <mi>b</mi>
    </mrow>
  </msub>
</math>
 = design strength of bolt, as governed by shear strength<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; 
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>f</mi>
    <mi>u</mi>
  </msub>
</math>
 = ultimate tensile strength of a bolt<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; 
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>n</mi>
    <mi>s</mi>
  </msub>
</math>
 = number of shear planes without threads intercepting the shear plane<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; 
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>A</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>s</mi>
      <mi>b</mi>
    </mrow>
  </msub>
</math>
 = nominal plain shank area of the bolt<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; 
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>n</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>n</mi>
    </mrow>
  </msub>
</math> 

= number of shear planes with threads intercepting the shear plane<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; 
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>A</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>n</mi>
      <mi>b</mi>
    </mrow>
  </msub>
</math>

= net shear area of the bolt at threads, may be taken as the area corresponding to root diameter at the thread<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; 
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>&#x03B3;<!-- γ --></mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>m</mi>
      <mi>b</mi>
    </mrow>
  </msub>
</math>
= partial safety factor for bolt</p>

';
	$markup_text1 = '

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; When the length of joint, 
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>l</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>j</mi>
    </mrow>
  </msub>
</math>
, of a splice or end connection in a compression or tension element containing more than two bolts (that is the distance between the first and last rows of bolts in the joint, measure in the direction of the load transfer) exceeds 15d in the direction of load, the nominal shear capacity (see 10.3.2)&nbsp;
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>V</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>d</mi>
      <mi>b</mi>
    </mrow>
  </msub>
</math>
&nbsp;shall be reduced by the factor&nbsp; 
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>&#x03B2;<!-- β --></mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>l</mi>
      <mi>j</mi>
    </mrow>
  </msub>
</math>
, given by:<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; 

<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mrow class="MJX-TeXAtom-ORD">
      <mo>&#x3B2;</mo>
    </mrow>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>l</mi>
      <mi>j</mi>
    </mrow>
  </msub>
  <mo>=</mo>
  <mn>1.075</mn>
  <mo>&#x2212;</mo>
  <msub>
    <mi>l</mi>
    <mi>j</mi>
  </msub>
  <mrow class="MJX-TeXAtom-ORD">
    <mo>/</mo>
  </mrow>
  <mn>200</mn>
  <mi>d</mi>
</math>
&nbsp;
but
&nbsp;
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <mn>0.75</mn>
  <mo>&#x2264;<!-- ≤ --></mo>
  <msub>
    <mrow class="MJX-TeXAtom-ORD">
      <mo>&#x3B2;</mo>
    </mrow>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>l</mi>
      <mi>j</mi>
    </mrow>
  </msub>
  <mo>&#x2264;<!-- ≤ --></mo>
  <mn>1.0</mn>
</math>
<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <mo>=</mo>
  <mn>1.075</mn>
  <mo>&#x2212;</mo>
  <mn>0.005</mn>
  <mfrac>
    <msub>
      <mi>l</mi>
      <mi>j</mi>
    </msub>
    <mi>d</mi>
  </mfrac>
</math>
<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; Where,<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; <math xmlns="http://www.w3.org/1998/Math/MathML">
  <mi>d</mi>
</math> = nominal diameter of the fastener<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; Note: This provision does not apply when the distribution of shear over the length of joint is uniform, as in the connection of web of a section to the flanges</p>

';
	$markup_text2 = '

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; When the grip length, 


<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>l</mi>
    <mi>g</mi>
  </msub>
</math>

, (equal to the total thickness of the connected plates), exceeds 5 times the diameter, d of the bolts, the design shear capacity shall be reduced by a factor given by:<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; 


<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>&#x03B2;<!-- β --></mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>l</mi>
      <mi>g</mi>
    </mrow>
  </msub>
  <mo>=</mo>
  <mfrac>
    <mrow>
      <mn>8</mn>
      <mi>d</mi>
    </mrow>
    <mrow>
      <mn>3</mn>
      <mi>d</mi>
      <mo>+</mo>
      <msub>
        <mi>l</mi>
        <mi>g</mi>
      </msub>
    </mrow>
  </mfrac>
</math>

<br />
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>&#x03B2;<!-- β --></mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>l</mi>
      <mi>g</mi>
    </mrow>
  </msub>  
</math>
shall not be more than 
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>&#x03B2;<!-- β --></mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>l</mi>
      <mi>j</mi>
    </mrow>
  </msub>  
</math>
,given in [Cl 10.3.3.1]. The grip length shall in no case be greater than 8d</p>

';
	$markup_text3 = '
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; The design shear capacity of bolts carrying shear through a packing plate in excess of 6 mm shall be decreased by a factor, 
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>&#x03B2;<!-- β --></mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>p</mi>
      <mi>k</mi>
    </mrow>
  </msub>
</math>
, given by:

<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>&#x03B2;<!-- β --></mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>p</mi>
      <mi>k</mi>
    </mrow>
  </msub>
  <mo>=</mo>
  <mn>1</mn>
  <mo>&#x2212;</mo>
  <mn>0.0125</mn>
  <msub>
    <mi>t</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>p</mi>
      <mi>k</mi>
    </mrow>
  </msub>
</math>

<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; Where,<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; 
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>t</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>p</mi>
      <mi>k</mi>
    </mrow>
  </msub>
</math>
 = thickness of the thicker packing, in mm</p>


';
	$tooltip1 = '1) [Cl 10.3.3.1] Reduction factor for long joints [Note: assuming that this does not apply]';
	$tooltip2 = '2) [Cl 10.3.3.2] Reduction factor for large grip lengths';
	$tooltip3 = '3) [Cl 10.3.3.3] Reduction factor for packing plates [Note: assuming that this does not apply]';
	// All the real form fields.
	$form = array();
	$form['step7_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step7-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_seated_angle_form_step7_feedback');
	$query->fields('ddcl_seated_angle_form_step7_feedback');
	$query->condition('uid', $user->uid);
	$form_step7_data = $query->execute();
	if ($form_step7_data)
	{
		if ($step7_data = $form_step7_data->fetchObject())
		{
			$form['step7_field1_fieldset']['step7_field1_question1'] = array(
				'#markup' => $markup_text
			);
			$form['step7_field1_fieldset']['tooltip1'] = array(
				'#type' => 'fieldset',
				'#title' => $tooltip1,
				'#collapsible' => TRUE, // Added
				'#collapsed' => TRUE // Added
			);
			$form['step7_field1_fieldset']['tooltip1']['tooltip1_text'] = array(
				'#markup' => $markup_text1
			);
			$form['step7_field1_fieldset']['tooltip2'] = array(
				'#type' => 'fieldset',
				'#title' => $tooltip2,
				'#collapsible' => TRUE, // Added
				'#collapsed' => TRUE // Added
			);
			$form['step7_field1_fieldset']['tooltip2']['tooltip2_text'] = array(
				'#markup' => $markup_text2
			);
			$form['step7_field1_fieldset']['tooltip3'] = array(
				'#type' => 'fieldset',
				'#title' => $tooltip3,
				'#collapsible' => TRUE, // Added
				'#collapsed' => TRUE // Added
			);
			$form['step7_field1_fieldset']['tooltip3']['tooltip3_text'] = array(
				'#markup' => $markup_text3
			);
			$form['step7_field1_fieldset']['step7_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				'#default_value' => $step7_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step7_field1_fieldset']['step7_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				'#default_value' => $step7_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step7_field1_fieldset\\[step7_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			$earlier_comments = _fetch_exsiting_ddcl_seated_angle_comments($step7_data->id, $step7_data->data_relation);
			$icom = 1;
			$form['step7_field1_fieldset']['exsiting_comments_title'] = array(
				'#markup' => '<hr>Earlier comment<hr><br>'
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step7_field1_fieldset']['exsiting_comments' . $icom] = array(
					'#markup' => $rowcomment
				);
				$icom++;
			} //$earlier_comments as $rowcomment
			if (empty($form_state['step7_num_comment']))
			{
				$form_state['step7_num_comment'] = 1;
			} //empty($form_state['step7_num_comment'])
			$step7_temp1 = 0;
			for ($step7_i = 0; $step7_i < $form_state['step7_num_comment']; $step7_i++)
			{
				$step7_temp1 = $step7_i;
				$form['step7_field1_fieldset'][$step7_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step7_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step7_field1_fieldset']["step7_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step7_temp1
			);
			$form['step7_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step7_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step7_comment_add_more_callback',
					'wrapper' => 'step7-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step7_num_comment'] > 1)
			{
				$form['step7_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step7_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step7_comment_add_more_callback',
						'wrapper' => 'step7-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step7_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step7_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step7_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step7_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_seated_angle_step7_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_seated_angle_step7_form_next_submit'
				)
			);
		} //$step7_data = $form_step7_data->fetchObject()
		else
		{
			$form['step7_field1_fieldset']['step7_field1_question1'] = array(
				'#markup' => $markup_text,
			);
			$form['step7_field1_fieldset']['tooltip1'] = array(
				'#type' => 'fieldset',
				'#title' => $tooltip1,
				'#collapsible' => TRUE, // Added
				'#collapsed' => TRUE // Added
			);
			$form['step7_field1_fieldset']['tooltip1']['tooltip1_text'] = array(
				'#markup' => $markup_text1
			);
			$form['step7_field1_fieldset']['tooltip2'] = array(
				'#type' => 'fieldset',
				'#title' => $tooltip2,
				'#collapsible' => TRUE, // Added
				'#collapsed' => TRUE // Added
			);
			$form['step7_field1_fieldset']['tooltip2']['tooltip2_text'] = array(
				'#markup' => $markup_text2
			);
			$form['step7_field1_fieldset']['tooltip3'] = array(
				'#type' => 'fieldset',
				'#title' => $tooltip2,
				'#collapsible' => TRUE, // Added
				'#collapsed' => TRUE // Added
			);
			$form['step7_field1_fieldset']['tooltip3']['tooltip3_text'] = array(
				'#markup' => $markup_text3
			);
			$form['step7_field1_fieldset']['step7_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				//'#default_value' => $step7_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step7_field1_fieldset']['step7_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				//'#default_value' => $step7_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step7_field1_fieldset\\[step7_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			if (empty($form_state['step7_num_comment']))
			{
				$form_state['step7_num_comment'] = 1;
			} //empty($form_state['step7_num_comment'])
			$step7_temp1 = 0;
			for ($step7_i = 0; $step7_i < $form_state['step7_num_comment']; $step7_i++)
			{
				$step7_temp1 = $step7_i;
				$form['step7_field1_fieldset'][$step7_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step7_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step7_field1_fieldset']["step7_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step7_temp1
			);
			$form['step7_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step7_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step7_comment_add_more_callback',
					'wrapper' => 'step7-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step7_num_comment'] > 1)
			{
				$form['step7_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step7_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step7_comment_add_more_callback',
						'wrapper' => 'step7-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step7_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step7_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step7_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step7_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_seated_angle_step7_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_seated_angle_step7_form_next_submit'
				)
			);
		} //$step7_data = $form_step7_data->fetchObject()
	} //$form_step7_data
	return $form;
}
function ddcl_seated_angle_step7_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step7_field1_fieldset']['step7_field1_question1'] = 'question 1';
	$query = db_select('ddcl_seated_angle_form_step7_feedback');
	$query->fields('ddcl_seated_angle_form_step7_feedback');
	$query->condition('uid', $user->uid);
	$form_step7_data = $query->execute()->fetchObject();
	//var_dump($form_step7_data);die;
	if ($form_step7_data)
	{
		$query = "UPDATE ddcl_seated_angle_form_step7_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step7_field1_fieldset']['step7_field1_question1'],
			":question_ok_not_ok" => $v['step7_field1_fieldset']['step7_field1_question_ok'],
			":question_not_ok_comment" => $v['step7_field1_fieldset']['step7_field1_question_not_ok_comment'],
			":data_relation" => "step7"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_seated_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step7_data->id
		));
		$form_step7_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step7_field1_fieldset']["step7_comment_count"]; $i++)
		{
			if ($v['step7_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step7_data->id,
					":number" => $i + $form_step7_comment_data->max + 1,
					":comment" => $v['step7_field1_fieldset'][$i]["comment"],
					":data_relation" => "step7",
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
			} //$v['step7_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step7_field1_fieldset']["step7_comment_count"]; $i++
	} //$form_step7_data
	else
	{
		$query = "INSERT INTO {ddcl_seated_angle_form_step7_feedback} (uid, question1,question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step7_field1_fieldset']['step7_field1_question1'],
			":question_ok_not_ok" => $v['step7_field1_fieldset']['step7_field1_question_ok'],
			":question_not_ok_comment" => $v['step7_field1_fieldset']['step7_field1_question_not_ok_comment'],
			":data_relation" => "step7",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$comments = 0;
		for ($i = 0; $i <= $v['step7_field1_fieldset']["step7_comment_count"]; $i++)
		{
			if ($v['step7_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step7_field1_fieldset'][$i]["comment"],
					":data_relation" => "step7",
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
			} //$v['step7_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step7_field1_fieldset']["step7_comment_count"]; $i++
	}
	$form_state['redirect'] = 'ddcl-seated-angle/form/step6/' . $user->uid;
}
function ddcl_seated_angle_step7_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step7_field1_fieldset']['step7_field1_question1'] = 'question 1';
	$query = db_select('ddcl_seated_angle_form_step7_feedback');
	$query->fields('ddcl_seated_angle_form_step7_feedback');
	$query->condition('uid', $user->uid);
	$form_step7_data = $query->execute()->fetchObject();
	//var_dump($form_step7_data);die;
	if ($form_step7_data)
	{
		$query = "UPDATE ddcl_seated_angle_form_step7_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step7_field1_fieldset']['step7_field1_question1'],
			":question_ok_not_ok" => $v['step7_field1_fieldset']['step7_field1_question_ok'],
			":question_not_ok_comment" => $v['step7_field1_fieldset']['step7_field1_question_not_ok_comment'],
			":data_relation" => "step7"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_seated_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step7_data->id
		));
		$form_step7_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step7_field1_fieldset']["step7_comment_count"]; $i++)
		{
			if ($v['step7_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step7_data->id,
					":number" => $i + $form_step7_comment_data->max + 1,
					":comment" => $v['step7_field1_fieldset'][$i]["comment"],
					":data_relation" => "step7",
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
			} //$v['step7_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step7_field1_fieldset']["step7_comment_count"]; $i++
	} //$form_step7_data
	else
	{
		$query = "INSERT INTO {ddcl_seated_angle_form_step7_feedback} (uid, question1,question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step7_field1_fieldset']['step7_field1_question1'],
			":question_ok_not_ok" => $v['step7_field1_fieldset']['step7_field1_question_ok'],
			":question_not_ok_comment" => $v['step7_field1_fieldset']['step7_field1_question_not_ok_comment'],
			":data_relation" => "step7",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$comments = 0;
		for ($i = 0; $i <= $v['step7_field1_fieldset']["step7_comment_count"]; $i++)
		{
			if ($v['step7_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step7_field1_fieldset'][$i]["comment"],
					":data_relation" => "step7",
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
			} //$v['step7_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step7_field1_fieldset']["step7_comment_count"]; $i++
	}
	$form_state['redirect'] = 'ddcl-seated-angle/form/step8/' . $user->uid;
}
/***************************************/
function step7_comment_add_more_add_one($form, &$form_state)
{
	$form_state['step7_num_comment']++;
	$form_state['rebuild'] = TRUE;
	//$form_state['no_redirect'] = TRUE;
}
function step7_comment_add_more_remove_one($form, &$form_state)
{
	if ($form_state['step7_num_comment'] > 1)
	{
		$form_state['step7_num_comment']--;
	} //$form_state['step7_num_comment'] > 1
	$form_state['rebuild'] = TRUE;
}
function step7_comment_add_more_callback($form, &$form_state)
{
	return $form['step7_field1_fieldset'];
}
/***************************************/
/***************************************** step 7 from ends ****************************/
/************************ Step 8 form starts *********************/
function ddcl_seated_angle_step8_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('b. Bolt > ii. Bearing capacity'));
	$markup_text = '<p><strong>b. Bolt<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; ii. Bearing capacity of bolt</strong> [Cl. 10.3.4]<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; 1) Bolt bearing on the seat angle (and column)<br />
The design bearing strength of a bolt on any plate, 
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>V</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>d</mi>
      <mi>p</mi>
      <mi>b</mi>
    </mrow>
  </msub>
</math>
, as governed by bearing is given by:

<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
<msub>
<mi>V</mi>
<mrow class="MJX-TeXAtom-ORD">
<mi>d</mi>
<mi>p</mi>
<mi>b</mi>
</mrow>
</msub>
<mo>=</mo>
<msub>
<mi>V</mi>
<mrow class="MJX-TeXAtom-ORD">
<mi>n</mi>
<mi>p</mi>
<mi>b</mi>
</mrow>
</msub>
<mrow class="MJX-TeXAtom-ORD">
<mo>/</mo>
</mrow>
<msub>
<mi>&#x03B3;<!-- γ --></mi>
<mrow class="MJX-TeXAtom-ORD">
<mi>m</mi>
<mi>b</mi>
</mrow>
</msub>
</math>

<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
<msub>
<mi>V</mi>
<mrow class="MJX-TeXAtom-ORD">
<mi>n</mi>
<mi>p</mi>
<mi>b</mi>
</mrow>
</msub>
<mo>=</mo>
<mn>2.5</mn>
<msub>
<mi>k</mi>
<mi>b</mi>
</msub>
<mi>d</mi>
<mi>t</mi>
<msub>
<mi>f</mi>
<mrow class="MJX-TeXAtom-ORD">
<mi>u</mi>
<mi>b</mi>
</mrow>
</msub>
</math>



<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; Where,<br />
&emsp;&emsp;&emsp;&emsp;&emsp;
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>k</mi>
    <mi>b</mi>
  </msub>
</math>

is smaller of
<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <mo>{</mo>
  <mfrac>
    <mi>e</mi>
    <mrow>
      <mn>3</mn>
      <msub>
        <mi>d</mi>
        <mn>0</mn>
      </msub>
    </mrow>
  </mfrac>
  <mo>,</mo>
  <mfrac>
    <mi>p</mi>
    <mrow>
      <mn>3</mn>
      <msub>
        <mi>d</mi>
        <mn>0</mn>
      </msub>
    </mrow>
  </mfrac>
  <mo>&#x2212;</mo>
  <mn>0.25</mn>
  <mo>,</mo>
  <mfrac>
    <msub>
      <mi>f</mi>
      <mrow class="MJX-TeXAtom-ORD">
        <mi>u</mi>
        <mi>b</mi>
      </mrow>
    </msub>
    <msub>
      <mi>f</mi>
      <mi>u</mi>
    </msub>
  </mfrac>
  <mo>,</mo>
  <mn>1.0</mn>
  <mo>}</mo>
</math>
<br />
&emsp;&emsp;&emsp;&emsp;&emsp;
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <mi>e</mi>
  <mo>,</mo>
  <mi>p</mi>
</math>
 = end and pitch distances of the fastener along bearing direction;<br />
&emsp;&emsp;&emsp;&emsp;&emsp;
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>d</mi>
    <mn>0</mn>
  </msub>
</math>
 = diameter of the hole<br />
&emsp;&emsp;&emsp;&emsp;&emsp;
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <msub>
    <mi>f</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>u</mi>
      <mi>b</mi>
    </mrow>
  </msub>
  <mo>,</mo>
  <msub>
    <mi>f</mi>
    <mi>u</mi>
  </msub>
</math>
= ultimate tensile stress of the bolt and the ultimate tensile stress of the plate, respectively;<br />
&emsp;&emsp;&emsp;&emsp;&emsp;
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <mi>d</mi>
</math>
= nominal diameter of the bolt,<br />
&emsp;&emsp;&emsp;&emsp;&emsp;
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <mi>t</mi>
</math>
= summation of the thicknesses of the connected plates experiencing bearing stress in the same direction, or if the bolts are countersunk, the thickness of the plate&nbsp; minus one half of the depth of countersinking.<br />
&emsp;&emsp;&emsp;&emsp;&emsp;
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <mi>t</mi>
</math>
shall be minimum of {thickness of seat angle, thickness of support}</p>

<p>Note: A reduction factor of 0.7 should be applied to the bearing resistance, in case of oversize holes.</p>


<hr>
';
	// All the real form fields.
	$form = array();
	$form['step8_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step8-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_seated_angle_form_step8_feedback');
	$query->fields('ddcl_seated_angle_form_step8_feedback');
	$query->condition('uid', $user->uid);
	$form_step8_data = $query->execute();
	if ($form_step8_data)
	{
		if ($step8_data = $form_step8_data->fetchObject())
		{
			$form['step8_field1_fieldset']['step8_field1_question1'] = array(
				'#markup' => $markup_text,
			);
			$form['step8_field1_fieldset']['step8_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				'#default_value' => $step8_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step8_field1_fieldset']['step8_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				'#default_value' => $step8_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step8_field1_fieldset\\[step8_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			$earlier_comments = _fetch_exsiting_ddcl_seated_angle_comments($step8_data->id, $step8_data->data_relation);
			$icom = 1;
			$form['step8_field1_fieldset']['exsiting_comments_title'] = array(
				'#markup' => '<hr>Earlier comment<hr><br>'
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step8_field1_fieldset']['exsiting_comments' . $icom] = array(
					'#markup' => $rowcomment
				);
				$icom++;
			} //$earlier_comments as $rowcomment
			if (empty($form_state['step8_num_comment']))
			{
				$form_state['step8_num_comment'] = 1;
			} //empty($form_state['step8_num_comment'])
			$step8_temp1 = 0;
			for ($step8_i = 0; $step8_i < $form_state['step8_num_comment']; $step8_i++)
			{
				$step8_temp1 = $step8_i;
				$form['step8_field1_fieldset'][$step8_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step8_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step8_field1_fieldset']["step8_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step8_temp1
			);
			$form['step8_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step8_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step8_comment_add_more_callback',
					'wrapper' => 'step8-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step8_num_comment'] > 1)
			{
				$form['step8_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step8_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step8_comment_add_more_callback',
						'wrapper' => 'step8-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step8_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step8_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step8_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step8_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_seated_angle_step8_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_seated_angle_step8_form_next_submit'
				)
			);
		} //$step8_data = $form_step8_data->fetchObject()
		else
		{
			$form['step8_field1_fieldset']['step8_field1_question1'] = array(
				'#markup' => $markup_text,
			);
			$form['step8_field1_fieldset']['step8_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				//'#default_value' => $step8_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step8_field1_fieldset']['step8_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				//'#default_value' => $step8_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step8_field1_fieldset\\[step8_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			if (empty($form_state['step8_num_comment']))
			{
				$form_state['step8_num_comment'] = 1;
			} //empty($form_state['step8_num_comment'])
			$step8_temp1 = 0;
			for ($step8_i = 0; $step8_i < $form_state['step8_num_comment']; $step8_i++)
			{
				$step8_temp1 = $step8_i;
				$form['step8_field1_fieldset'][$step8_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step8_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step8_field1_fieldset']["step8_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step8_temp1
			);
			$form['step8_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step8_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step8_comment_add_more_callback',
					'wrapper' => 'step8-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step8_num_comment'] > 1)
			{
				$form['step8_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step8_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step8_comment_add_more_callback',
						'wrapper' => 'step8-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step8_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step8_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step8_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step8_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_seated_angle_step8_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_seated_angle_step8_form_next_submit'
				)
			);
		} //$step8_data = $form_step8_data->fetchObject()
	} //$form_step8_data
	return $form;
}
function ddcl_seated_angle_step8_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step8_field1_fieldset']['step8_field1_question1'] = 'question 1';
	$query = db_select('ddcl_seated_angle_form_step8_feedback');
	$query->fields('ddcl_seated_angle_form_step8_feedback');
	$query->condition('uid', $user->uid);
	$form_step8_data = $query->execute()->fetchObject();
	//var_dump($form_step8_data);die;
	if ($form_step8_data)
	{
		$query = "UPDATE ddcl_seated_angle_form_step8_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step8_field1_fieldset']['step8_field1_question1'],
			":question_ok_not_ok" => $v['step8_field1_fieldset']['step8_field1_question_ok'],
			":question_not_ok_comment" => $v['step8_field1_fieldset']['step8_field1_question_not_ok_comment'],
			":data_relation" => "step8"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_seated_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step8_data->id
		));
		$form_step8_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step8_field1_fieldset']["step8_comment_count"]; $i++)
		{
			if ($v['step8_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step8_data->id,
					":number" => $i + $form_step8_comment_data->max + 1,
					":comment" => $v['step8_field1_fieldset'][$i]["comment"],
					":data_relation" => "step8",
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
			} //$v['step8_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step8_field1_fieldset']["step8_comment_count"]; $i++
	} //$form_step8_data
	else
	{
		$query = "INSERT INTO {ddcl_seated_angle_form_step8_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step8_field1_fieldset']['step8_field1_question1'],
			":question_ok_not_ok" => $v['step8_field1_fieldset']['step8_field1_question_ok'],
			":question_not_ok_comment" => $v['step8_field1_fieldset']['step8_field1_question_not_ok_comment'],
			":data_relation" => "step8",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$comments = 0;
		for ($i = 0; $i <= $v['step8_field1_fieldset']["step8_comment_count"]; $i++)
		{
			if ($v['step8_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step8_field1_fieldset'][$i]["comment"],
					":data_relation" => "step8",
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
			} //$v['step8_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step8_field1_fieldset']["step8_comment_count"]; $i++
	}
	$form_state['redirect'] = 'ddcl-seated-angle/form/step7/' . $user->uid;
}
function ddcl_seated_angle_step8_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step8_field1_fieldset']['step8_field1_question1'] = 'question 1';
	$query = db_select('ddcl_seated_angle_form_step8_feedback');
	$query->fields('ddcl_seated_angle_form_step8_feedback');
	$query->condition('uid', $user->uid);
	$form_step8_data = $query->execute()->fetchObject();
	//var_dump($form_step8_data);die;
	if ($form_step8_data)
	{
		$query = "UPDATE ddcl_seated_angle_form_step8_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step8_field1_fieldset']['step8_field1_question1'],
			":question_ok_not_ok" => $v['step8_field1_fieldset']['step8_field1_question_ok'],
			":question_not_ok_comment" => $v['step8_field1_fieldset']['step8_field1_question_not_ok_comment'],
			":data_relation" => "step8"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_seated_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step8_data->id
		));
		$form_step8_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step8_field1_fieldset']["step8_comment_count"]; $i++)
		{
			if ($v['step8_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step8_data->id,
					":number" => $i + $form_step8_comment_data->max + 1,
					":comment" => $v['step8_field1_fieldset'][$i]["comment"],
					":data_relation" => "step8",
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
			} //$v['step8_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step8_field1_fieldset']["step8_comment_count"]; $i++
	} //$form_step8_data
	else
	{
		$query = "INSERT INTO {ddcl_seated_angle_form_step8_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step8_field1_fieldset']['step8_field1_question1'],
			":question_ok_not_ok" => $v['step8_field1_fieldset']['step8_field1_question_ok'],
			":question_not_ok_comment" => $v['step8_field1_fieldset']['step8_field1_question_not_ok_comment'],
			":data_relation" => "step8",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$comments = 0;
		for ($i = 0; $i <= $v['step8_field1_fieldset']["step8_comment_count"]; $i++)
		{
			if ($v['step8_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step8_field1_fieldset'][$i]["comment"],
					":data_relation" => "step8",
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
			} //$v['step8_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step8_field1_fieldset']["step8_comment_count"]; $i++
	}
	$form_state['redirect'] = 'ddcl-seated-angle/form/step9/' . $user->uid;
}
/***************************************/
function step8_comment_add_more_add_one($form, &$form_state)
{
	$form_state['step8_num_comment']++;
	$form_state['rebuild'] = TRUE;
	//$form_state['no_redirect'] = TRUE;
}
function step8_comment_add_more_remove_one($form, &$form_state)
{
	if ($form_state['step8_num_comment'] > 1)
	{
		$form_state['step8_num_comment']--;
	} //$form_state['step8_num_comment'] > 1
	$form_state['rebuild'] = TRUE;
}
function step8_comment_add_more_callback($form, &$form_state)
{
	return $form['step8_field1_fieldset'];
}
/***************************************/
/***************************************** step 8 from ends ****************************/
/************************ Step 9 form starts *********************/
function ddcl_seated_angle_step9_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('b. Bolt> iii. Number of bolts'));
	$markup_text = '<p><strong>b. Bolt<br /></strong>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; iii.</strong> Number of bolts (N1) = Factored Load/capacity per bolt</p>


<hr>
';
	// All the real form fields.
	$form = array();
	$form['step9_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step9-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_seated_angle_form_step9_feedback');
	$query->fields('ddcl_seated_angle_form_step9_feedback');
	$query->condition('uid', $user->uid);
	$form_step9_data = $query->execute();
	if ($form_step9_data)
	{
		if ($step9_data = $form_step9_data->fetchObject())
		{
			$form['step9_field1_fieldset']['step9_field1_question1'] = array(
				'#markup' => $markup_text
			);
			$form['step9_field1_fieldset']['step9_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				'#default_value' => $step9_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step9_field1_fieldset']['step9_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				'#default_value' => $step9_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step9_field1_fieldset\\[step9_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			$earlier_comments = _fetch_exsiting_ddcl_seated_angle_comments($step9_data->id, $step9_data->data_relation);
			$icom = 1;
			$form['step9_field1_fieldset']['exsiting_comments_title'] = array(
				'#markup' => '<hr><strong>Earlier comment</strong><hr><br>'
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step9_field1_fieldset']['exsiting_comments' . $icom] = array(
					'#markup' => $rowcomment
				);
				$icom++;
			} //$earlier_comments as $rowcomment
			if (empty($form_state['step9_num_comment']))
			{
				$form_state['step9_num_comment'] = 1;
			} //empty($form_state['step9_num_comment'])
			$step9_temp1 = 0;
			for ($step9_i = 0; $step9_i < $form_state['step9_num_comment']; $step9_i++)
			{
				$step9_temp1 = $step9_i;
				$form['step9_field1_fieldset'][$step9_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step9_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step9_field1_fieldset']["step9_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step9_temp1
			);
			$form['step9_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step9_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step9_comment_add_more_callback',
					'wrapper' => 'step9-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step9_num_comment'] > 1)
			{
				$form['step9_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step9_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step9_comment_add_more_callback',
						'wrapper' => 'step9-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step9_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step9_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step9_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step9_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_seated_angle_step9_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_seated_angle_step9_form_next_submit'
				)
			);
		} //$step9_data = $form_step9_data->fetchObject()
		else
		{
			$form['step9_field1_fieldset']['step9_field1_question1'] = array(
				'#markup' => $markup_text,
			);
			$form['step9_field1_fieldset']['step9_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				//'#default_value' => $step9_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step9_field1_fieldset']['step9_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				//'#default_value' => $step9_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step9_field1_fieldset\\[step9_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			if (empty($form_state['step9_num_comment']))
			{
				$form_state['step9_num_comment'] = 1;
			} //empty($form_state['step9_num_comment'])
			$step9_temp1 = 0;
			for ($step9_i = 0; $step9_i < $form_state['step9_num_comment']; $step9_i++)
			{
				$step9_temp1 = $step9_i;
				$form['step9_field1_fieldset'][$step9_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step9_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step9_field1_fieldset']["step9_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step9_temp1
			);
			$form['step9_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step9_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step9_comment_add_more_callback',
					'wrapper' => 'step9-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step9_num_comment'] > 1)
			{
				$form['step9_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step9_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step9_comment_add_more_callback',
						'wrapper' => 'step9-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step9_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step9_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step9_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step9_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_seated_angle_step9_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_seated_angle_step9_form_next_submit'
				)
			);
		} //$step9_data = $form_step9_data->fetchObject()
	} //$form_step9_data
	return $form;
}
function ddcl_seated_angle_step9_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step9_field1_fieldset']['step9_field1_question1'] = 'question 1';
	$query = db_select('ddcl_seated_angle_form_step9_feedback');
	$query->fields('ddcl_seated_angle_form_step9_feedback');
	$query->condition('uid', $user->uid);
	$form_step9_data = $query->execute()->fetchObject();
	//var_dump($form_step9_data);die;
	if ($form_step9_data)
	{
		$query = "UPDATE ddcl_seated_angle_form_step9_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step9_field1_fieldset']['step9_field1_question1'],
			":question_ok_not_ok" => $v['step9_field1_fieldset']['step9_field1_question_ok'],
			":question_not_ok_comment" => $v['step9_field1_fieldset']['step9_field1_question_not_ok_comment'],
			":data_relation" => "step9"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_seated_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step9_data->id
		));
		$form_step9_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step9_field1_fieldset']["step9_comment_count"]; $i++)
		{
			if ($v['step9_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step9_data->id,
					":number" => $i + $form_step9_comment_data->max + 1,
					":comment" => $v['step9_field1_fieldset'][$i]["comment"],
					":data_relation" => "step9",
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
			} //$v['step9_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step9_field1_fieldset']["step9_comment_count"]; $i++
	} //$form_step9_data
	else
	{
		$query = "INSERT INTO {ddcl_seated_angle_form_step9_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step9_field1_fieldset']['step9_field1_question1'],
			":question_ok_not_ok" => $v['step9_field1_fieldset']['step9_field1_question_ok'],
			":question_not_ok_comment" => $v['step9_field1_fieldset']['step9_field1_question_not_ok_comment'],
			":data_relation" => "step9",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$comments = 0;
		for ($i = 0; $i <= $v['step9_field1_fieldset']["step9_comment_count"]; $i++)
		{
			if ($v['step9_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step9_field1_fieldset'][$i]["comment"],
					":data_relation" => "step9",
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
			} //$v['step9_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step9_field1_fieldset']["step9_comment_count"]; $i++
	}
	$form_state['redirect'] = 'ddcl-seated-angle/form/step8/' . $user->uid;
}
function ddcl_seated_angle_step9_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step9_field1_fieldset']['step9_field1_question1'] = 'question 1';
	$query = db_select('ddcl_seated_angle_form_step9_feedback');
	$query->fields('ddcl_seated_angle_form_step9_feedback');
	$query->condition('uid', $user->uid);
	$form_step9_data = $query->execute()->fetchObject();
	//var_dump($form_step9_data);die;
	if ($form_step9_data)
	{
		$query = "UPDATE ddcl_seated_angle_form_step9_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step9_field1_fieldset']['step9_field1_question1'],
			":question_ok_not_ok" => $v['step9_field1_fieldset']['step9_field1_question_ok'],
			":question_not_ok_comment" => $v['step9_field1_fieldset']['step9_field1_question_not_ok_comment'],
			":data_relation" => "step9"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_seated_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step9_data->id
		));
		$form_step9_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step9_field1_fieldset']["step9_comment_count"]; $i++)
		{
			if ($v['step9_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step9_data->id,
					":number" => $i + $form_step9_comment_data->max + 1,
					":comment" => $v['step9_field1_fieldset'][$i]["comment"],
					":data_relation" => "step9",
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
			} //$v['step9_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step9_field1_fieldset']["step9_comment_count"]; $i++
	} //$form_step9_data
	else
	{
		$query = "INSERT INTO {ddcl_seated_angle_form_step9_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step9_field1_fieldset']['step9_field1_question1'],
			":question_ok_not_ok" => $v['step9_field1_fieldset']['step9_field1_question_ok'],
			":question_not_ok_comment" => $v['step9_field1_fieldset']['step9_field1_question_not_ok_comment'],
			":data_relation" => "step9",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$comments = 0;
		for ($i = 0; $i <= $v['step9_field1_fieldset']["step9_comment_count"]; $i++)
		{
			if ($v['step9_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step9_field1_fieldset'][$i]["comment"],
					":data_relation" => "step9",
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
			} //$v['step9_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step9_field1_fieldset']["step9_comment_count"]; $i++
	}
	$form_state['redirect'] = 'ddcl-seated-angle/form/step10/' . $user->uid;
}
/***************************************/
function step9_comment_add_more_add_one($form, &$form_state)
{
	$form_state['step9_num_comment']++;
	$form_state['rebuild'] = TRUE;
	//$form_state['no_redirect'] = TRUE;
}
function step9_comment_add_more_remove_one($form, &$form_state)
{
	if ($form_state['step9_num_comment'] > 1)
	{
		$form_state['step9_num_comment']--;
	} //$form_state['step9_num_comment'] > 1
	$form_state['rebuild'] = TRUE;
}
function step9_comment_add_more_callback($form, &$form_state)
{
	return $form['step9_field1_fieldset'];
}
/***************************************/
/***************************************** step 9 from ends ****************************/
/************************ Step 10 form starts *********************/
function ddcl_seated_angle_step10_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('a. Detailing > i. Spacing requirements'));
	$markup_text = '<p><strong>c. Detailing</strong><br />
&emsp;&emsp;&emsp;
i. [Cl 10.2.2] <strong>Minimum spacing</strong><br />
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
The distance between centre of fasteners shall not be less than 2.5 times the nominal diameter of the fastener.

<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
<mi>p</mi>
<mo>&#x2265;<!-- ≥ --></mo>
<mn>2.5</mn>
<mi>d</mi>
</math>

<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
<mi>g</mi>
<mo>&#x2265;<!-- ≥ --></mo>
<mn>2.5</mn>
<mi>d</mi>
</math>

&emsp;&emsp;&emsp;
ii. [Cl 10.2.3.1] <strong>Maximum spacing</strong><br />
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
The distance between the centres of any two adjacent fasteners shall not exceed 32t or 300mm, whichever is less.

<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
<mi>p</mi>
<mo>&#x2264;<!-- ≤ --></mo>
<mi>m</mi>
<mi>i</mi>
<mi>n</mi>
<mo stretchy="false">(</mo>
<mn>32</mn>
<mo>&#x2217;<!-- ∗ --></mo>
<mi>t</mi>
<mo>,</mo>
<mn>300</mn>
<mo stretchy="false">)</mo>
</math>

<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
<mi>g</mi>
<mo>&#x2264;<!-- ≤ --></mo>
<mi>m</mi>
<mi>i</mi>
<mi>n</mi>
<mo stretchy="false">(</mo>
<mn>32</mn>
<mo>&#x2217;<!-- ∗ --></mo>
<mi>t</mi>
<mo>,</mo>
<mn>300</mn>
<mo stretchy="false">)</mo>
</math>

</p>

&emsp;&emsp;&emsp;
Where, <br>
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <mi>p</mi>
</math>
- pitch 

<br>
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <mi>g</mi>
</math>
- gauge

<br>
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <mi>d</mi>
</math>
- nominal bolt diameter 

<br>
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <mi>t</mi>
</math>
- thickness of the thinner plate.

<hr>
';
	// All the real form fields.
	$form = array();
	$form['step10_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step10-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_seated_angle_form_step10_feedback');
	$query->fields('ddcl_seated_angle_form_step10_feedback');
	$query->condition('uid', $user->uid);
	$form_step10_data = $query->execute();
	if ($form_step10_data)
	{
		if ($step10_data = $form_step10_data->fetchObject())
		{
			$form['step10_field1_fieldset']['step10_field1_question1'] = array(
				'#markup' => $markup_text,
			);
			$form['step10_field1_fieldset']['step10_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				'#default_value' => $step10_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step10_field1_fieldset']['step10_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				'#default_value' => $step10_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step10_field1_fieldset\\[step10_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			$earlier_comments = _fetch_exsiting_ddcl_seated_angle_comments($step10_data->id, $step10_data->data_relation);
			$icom = 1;
			$form['step10_field1_fieldset']['exsiting_comments_title'] = array(
				'#markup' => '<hr><strong>Earlier comment</strong><hr><br>'
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step10_field1_fieldset']['exsiting_comments' . $icom] = array(
					'#markup' => $rowcomment
				);
				$icom++;
			} //$earlier_comments as $rowcomment
			if (empty($form_state['step10_num_comment']))
			{
				$form_state['step10_num_comment'] = 1;
			} //empty($form_state['step10_num_comment'])
			$step10_temp1 = 0;
			for ($step10_i = 0; $step10_i < $form_state['step10_num_comment']; $step10_i++)
			{
				$step10_temp1 = $step10_i;
				$form['step10_field1_fieldset'][$step10_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step10_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step10_field1_fieldset']["step10_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step10_temp1
			);
			$form['step10_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step10_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step10_comment_add_more_callback',
					'wrapper' => 'step10-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step10_num_comment'] > 1)
			{
				$form['step10_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step10_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step10_comment_add_more_callback',
						'wrapper' => 'step10-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step10_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step10_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step10_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step10_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_seated_angle_step10_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_seated_angle_step10_form_next_submit'
				)
			);
		} //$step10_data = $form_step10_data->fetchObject()
		else
		{
			$form['step10_field1_fieldset']['step10_field1_question1'] = array(
				'#markup' => $markup_text,
			);
			$form['step10_field1_fieldset']['step10_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				//'#default_value' => $step2_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step10_field1_fieldset']['step10_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				//'#default_value' => $step2_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step10_field1_fieldset\\[step10_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			if (empty($form_state['step10_num_comment']))
			{
				$form_state['step10_num_comment'] = 1;
			} //empty($form_state['step10_num_comment'])
			$step10_temp1 = 0;
			for ($step10_i = 0; $step10_i < $form_state['step10_num_comment']; $step10_i++)
			{
				$step10_temp1 = $step10_i;
				$form['step10_field1_fieldset'][$step10_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step10_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step10_field1_fieldset']["step10_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step10_temp1
			);
			$form['step10_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step10_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step10_comment_add_more_callback',
					'wrapper' => 'step10-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step10_num_comment'] > 1)
			{
				$form['step10_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step10_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step10_comment_add_more_callback',
						'wrapper' => 'step10-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step10_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step10_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step10_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step10_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_seated_angle_step10_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_seated_angle_step10_form_next_submit'
				)
			);
		} //$step10_data = $form_step10_data->fetchObject()
	} //$form_step10_data
	return $form;
}
function ddcl_seated_angle_step10_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step10_field1_fieldset']['step10_field1_question1'] = 'question 1';
	$query = db_select('ddcl_seated_angle_form_step10_feedback');
	$query->fields('ddcl_seated_angle_form_step10_feedback');
	$query->condition('uid', $user->uid);
	$form_step10_data = $query->execute()->fetchObject();
	//var_dump($form_step10_data);die;
	if ($form_step10_data)
	{
		$query = "UPDATE ddcl_seated_angle_form_step10_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step10_field1_fieldset']['step10_field1_question1'],
			":question_ok_not_ok" => $v['step10_field1_fieldset']['step10_field1_question_ok'],
			":question_not_ok_comment" => $v['step10_field1_fieldset']['step10_field1_question_not_ok_comment'],
			":data_relation" => "step10"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_seated_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step10_data->id
		));
		$form_step10_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step10_field1_fieldset']["step10_comment_count"]; $i++)
		{
			if ($v['step10_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step10_data->id,
					":number" => $i + $form_step10_comment_data->max + 1,
					":comment" => $v['step10_field1_fieldset'][$i]["comment"],
					":data_relation" => "step10",
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
			} //$v['step10_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step10_field1_fieldset']["step10_comment_count"]; $i++
	} //$form_step10_data
	else
	{
		$query = "INSERT INTO {ddcl_seated_angle_form_step10_feedback} (uid, question1,  question_ok_not_ok,question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step10_field1_fieldset']['step10_field1_question1'],
			":question_ok_not_ok" => $v['step10_field1_fieldset']['step10_field1_question_ok'],
			":question_not_ok_comment" => $v['step10_field1_fieldset']['step10_field1_question_not_ok_comment'],
			":data_relation" => "step10",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$comments = 0;
		for ($i = 0; $i <= $v['step10_field1_fieldset']["step10_comment_count"]; $i++)
		{
			if ($v['step10_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step10_field1_fieldset'][$i]["comment"],
					":data_relation" => "step10",
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
			} //$v['step10_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step10_field1_fieldset']["step10_comment_count"]; $i++
	}
	$form_state['redirect'] = 'ddcl-seated-angle/form/step9/' . $user->uid;
}
function ddcl_seated_angle_step10_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step10_field1_fieldset']['step10_field1_question1'] = 'question 1';
	$query = db_select('ddcl_seated_angle_form_step10_feedback');
	$query->fields('ddcl_seated_angle_form_step10_feedback');
	$query->condition('uid', $user->uid);
	$form_step10_data = $query->execute()->fetchObject();
	//var_dump($form_step10_data);die;
	if ($form_step10_data)
	{
		$query = "UPDATE ddcl_seated_angle_form_step10_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step10_field1_fieldset']['step10_field1_question1'],
			":question_ok_not_ok" => $v['step10_field1_fieldset']['step10_field1_question_ok'],
			":question_not_ok_comment" => $v['step10_field1_fieldset']['step10_field1_question_not_ok_comment'],
			":data_relation" => "step10"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_seated_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step10_data->id
		));
		$form_step10_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step10_field1_fieldset']["step10_comment_count"]; $i++)
		{
			if ($v['step10_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step10_data->id,
					":number" => $i + $form_step10_comment_data->max + 1,
					":comment" => $v['step10_field1_fieldset'][$i]["comment"],
					":data_relation" => "step10",
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
			} //$v['step10_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step10_field1_fieldset']["step10_comment_count"]; $i++
	} //$form_step10_data
	else
	{
		$query = "INSERT INTO {ddcl_seated_angle_form_step10_feedback} (uid, question1,  question_ok_not_ok,question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step10_field1_fieldset']['step10_field1_question1'],
			":question_ok_not_ok" => $v['step10_field1_fieldset']['step10_field1_question_ok'],
			":question_not_ok_comment" => $v['step10_field1_fieldset']['step10_field1_question_not_ok_comment'],
			":data_relation" => "step10",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$comments = 0;
		for ($i = 0; $i <= $v['step10_field1_fieldset']["step10_comment_count"]; $i++)
		{
			if ($v['step10_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step10_field1_fieldset'][$i]["comment"],
					":data_relation" => "step10",
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
			} //$v['step10_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step10_field1_fieldset']["step10_comment_count"]; $i++
	}
	$form_state['redirect'] = 'ddcl-seated-angle/form/step11/' . $user->uid;
}
/***************************************/
function step10_comment_add_more_add_one($form, &$form_state)
{
	$form_state['step10_num_comment']++;
	$form_state['rebuild'] = TRUE;
	//$form_state['no_redirect'] = TRUE;
}
function step10_comment_add_more_remove_one($form, &$form_state)
{
	if ($form_state['step10_num_comment'] > 1)
	{
		$form_state['step10_num_comment']--;
	} //$form_state['step10_num_comment'] > 1
	$form_state['rebuild'] = TRUE;
}
function step10_comment_add_more_callback($form, &$form_state)
{
	return $form['step10_field1_fieldset'];
}
/***************************************/
/***************************************** step 10 from ends ****************************/
/************************ Step 11 form starts *********************/
function ddcl_seated_angle_step11_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('General Assumptions'));
	$markup_text = '
	
	<p class="rtejustify">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<strong>iii. [Cl 10.2.4] Edge and end distances</strong><br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; [Cl 10.2.4.2]<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;The minimum edge and end distances from the centre of any hole to the nearest edge of a plate shall not be less than 1.7 times the hole diameter in case of sheared or hand-flame cut edges; and 1.5 times the hole diameter in case of rolled, machine-flame cut, sawn and planed edges.</p>



<p>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; [Cl 10.2.4.3]<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; The maximum edge distance to the nearest line of fasteners from an edge of any un-stiffened part should not exceed 
<math xmlns="http://www.w3.org/1998/Math/MathML">
  <mn>12</mn>
  <mi>t</mi>
  <mi>&#x03F5;<!-- ϵ --></mi>
  <mo>,</mo>
  <mi>w</mi>
  <mi>h</mi>
  <mi>e</mi>
  <mi>r</mi>
  <mi>e</mi>
  <mtext>&#xA0;</mtext>
  <mi>&#x03F5;<!-- ϵ --></mi>
  <mo>=</mo>
  <msqrt>
    <mfrac>
      <mn>250</mn>
      <msub>
        <mi>f</mi>
        <mi>y</mi>
      </msub>
    </mfrac>
  </msqrt>
</math>
 is the thickness of the thinner outer plate.</p>

<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">

<mi>E</mi>
<mi>n</mi>
<mi>d</mi>
<mi>D</mi>
<mi>i</mi>
<mi>s</mi>
<mi>t</mi>
<mi>a</mi>
<mi>n</mi>
<mi>c</mi>
<mi>e</mi>
<mo stretchy="false">(</mo>
<mi>m</mi>
<mi>m</mi>
<mo stretchy="false">)</mo>
<mo>&#x2265;<!-- ≥ --></mo>
<mn>1.7</mn>
<mo>&#x2217;<!-- ∗ --></mo>
<mo stretchy="false">(</mo>
<mi>h</mi>
<mi>o</mi>
<mi>l</mi>
<mi>e</mi>
<mi>d</mi>
<mi>i</mi>
<mi>a</mi>
<mi>m</mi>
<mi>e</mi>
<mi>t</mi>
<mi>e</mi>
<mi>r</mi>
<mo stretchy="false">)</mo>
<mo>,</mo>
<mi>E</mi>
<mi>n</mi>
<mi>d</mi>
<mi>D</mi>
<mi>i</mi>
<mi>s</mi>
<mi>t</mi>
<mi>a</mi>
<mi>n</mi>
<mi>c</mi>
<mi>e</mi>
<mo>&#x2264;<!-- ≤ --></mo>
<mn>12</mn>
<mo>&#x2217;<!-- ∗ --></mo>
<mi>t</mi>
<mi>&#x03F5;<!-- ε --></mi>
</math>

<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
<mi>E</mi>
<mi>d</mi>
<mi>g</mi>
<mi>e</mi>
<mi>D</mi>
<mi>i</mi>
<mi>s</mi>
<mi>t</mi>
<mi>a</mi>
<mi>n</mi>
  <mi>c</mi>
<mi>e</mi>
<mo stretchy="false">(</mo>
<mi>m</mi>
<mi>m</mi>
<mo stretchy="false">)</mo>
<mo>&#x2265;<!-- ≥ --></mo>
<mn>1.7</mn>
<mo>&#x2217;<!-- ∗ --></mo>
<mo stretchy="false">(</mo>
<mi>h</mi>
<mi>o</mi>
<mi>l</mi>
<mi>e</mi>
<mi>d</mi>
<mi>i</mi>
<mi>a</mi>
<mi>m</mi>
<mi>e</mi>
<mi>t</mi>
<mi>e</mi>
<mi>r</mi>
<mo stretchy="false">)</mo>
<mo>,</mo>
<mi>E</mi>
<mi>d</mi>
<mi>g</mi>
<mi>e</mi>
<mi>D</mi>
<mi>i</mi>
<mi>s</mi>
<mi>t</mi>
<mi>a</mi>
<mi>n</mi>
<mi>c</mi>
<mi>e</mi>
<mo>&#x2264;<!-- ≤ --></mo>
<mn>12</mn>
<mo>&#x2217;<!-- ∗ --></mo>
<mi>t</mi>
<mi>&#x03F5;<!-- ε --></mi>
</math>

<hr>
';
	// All the real form fields.
	$form = array();
	$form['step11_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step11-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_seated_angle_form_step11_feedback');
	$query->fields('ddcl_seated_angle_form_step11_feedback');
	$query->condition('uid', $user->uid);
	$form_step11_data = $query->execute();
	if ($form_step11_data)
	{
		if ($step11_data = $form_step11_data->fetchObject())
		{
			$form['step11_field1_fieldset']['step11_field1_question1'] = array(
				'#markup' => $markup_text,
			);
			$form['step11_field1_fieldset']['step11_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				'#default_value' => $step11_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step11_field1_fieldset']['step11_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				'#default_value' => $step11_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step11_field1_fieldset\\[step11_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			$earlier_comments = _fetch_exsiting_ddcl_seated_angle_comments($step11_data->id, $step11_data->data_relation);
			$icom = 1;
			$form['step11_field1_fieldset']['exsiting_comments_title'] = array(
				'#markup' => 'Earlier comment<br>'
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step11_field1_fieldset']['exsiting_comments' . $icom] = array(
					'#markup' => $rowcomment
				);
				$icom++;
			} //$earlier_comments as $rowcomment
			if (empty($form_state['step11_num_comment']))
			{
				$form_state['step11_num_comment'] = 1;
			} //empty($form_state['step11_num_comment'])
			$step11_temp1 = 0;
			for ($step11_i = 0; $step11_i < $form_state['step11_num_comment']; $step11_i++)
			{
				$step11_temp1 = $step11_i;
				$form['step11_field1_fieldset'][$step11_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step11_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step11_field1_fieldset']["step11_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step11_temp1
			);
			$form['step11_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step11_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step11_comment_add_more_callback',
					'wrapper' => 'step11-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step11_num_comment'] > 1)
			{
				$form['step11_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step11_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step11_comment_add_more_callback',
						'wrapper' => 'step11-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step11_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step11_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step11_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step11_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_seated_angle_step11_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_seated_angle_step11_form_next_submit'
				)
			);
		} //$step11_data = $form_step11_data->fetchObject()
		else
		{
			$form['step11_field1_fieldset']['step11_field1_question1'] = array(
				'#markup' => $markup_text,
			);
			$form['step11_field1_fieldset']['step11_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				//'#default_value' => $step11_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step11_field1_fieldset']['step11_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				//'#default_value' => $step2_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step11_field1_fieldset\\[step11_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			if (empty($form_state['step11_num_comment']))
			{
				$form_state['step11_num_comment'] = 1;
			} //empty($form_state['step11_num_comment'])
			$step11_temp1 = 0;
			for ($step11_i = 0; $step11_i < $form_state['step11_num_comment']; $step11_i++)
			{
				$step11_temp1 = $step11_i;
				$form['step11_field1_fieldset'][$step11_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step11_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step11_field1_fieldset']["step11_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step11_temp1
			);
			$form['step11_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step11_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step11_comment_add_more_callback',
					'wrapper' => 'step11-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step11_num_comment'] > 1)
			{
				$form['step11_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step11_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step11_comment_add_more_callback',
						'wrapper' => 'step11-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step11_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step11_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step11_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step11_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_seated_angle_step11_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_seated_angle_step11_form_next_submit'
				)
			);
		} //$step11_data = $form_step11_data->fetchObject()
	} //$form_step11_data
	return $form;
}
function ddcl_seated_angle_step11_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step11_field1_fieldset']['step11_field1_question1'] = 'question 1';
	$query = db_select('ddcl_seated_angle_form_step11_feedback');
	$query->fields('ddcl_seated_angle_form_step11_feedback');
	$query->condition('uid', $user->uid);
	$form_step11_data = $query->execute()->fetchObject();
	//var_dump($form_step11_data);die;
	if ($form_step11_data)
	{
		$query = "UPDATE ddcl_seated_angle_form_step11_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step11_field1_fieldset']['step11_field1_question1'],
			":question_ok_not_ok" => $v['step11_field1_fieldset']['step11_field1_question_ok'],
			":question_not_ok_comment" => $v['step11_field1_fieldset']['step11_field1_question_not_ok_comment'],
			":data_relation" => "step11"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_seated_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step11_data->id
		));
		$form_step11_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step11_field1_fieldset']["step11_comment_count"]; $i++)
		{
			if ($v['step11_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step11_data->id,
					":number" => $i + $form_step11_comment_data->max + 1,
					":comment" => $v['step11_field1_fieldset'][$i]["comment"],
					":data_relation" => "step11",
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
			} //$v['step11_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step11_field1_fieldset']["step11_comment_count"]; $i++
	} //$form_step11_data
	else
	{
		$query = "INSERT INTO {ddcl_seated_angle_form_step11_feedback} (uid, question1,question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step11_field1_fieldset']['step11_field1_question1'],
			":question_ok_not_ok" => $v['step11_field1_fieldset']['step11_field1_question_ok'],
			":question_not_ok_comment" => $v['step11_field1_fieldset']['step11_field1_question_not_ok_comment'],
			":data_relation" => "step11",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$comments = 0;
		for ($i = 0; $i <= $v['step11_field1_fieldset']["step11_comment_count"]; $i++)
		{
			if ($v['step11_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step11_field1_fieldset'][$i]["comment"],
					":data_relation" => "step11",
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
			} //$v['step11_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step11_field1_fieldset']["step11_comment_count"]; $i++
	}
	$form_state['redirect'] = 'ddcl-seated-angle/form/step10/' . $user->uid;
}
function ddcl_seated_angle_step11_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step11_field1_fieldset']['step11_field1_question1'] = 'question 1';
	$query = db_select('ddcl_seated_angle_form_step11_feedback');
	$query->fields('ddcl_seated_angle_form_step11_feedback');
	$query->condition('uid', $user->uid);
	$form_step11_data = $query->execute()->fetchObject();
	//var_dump($form_step11_data);die;
	if ($form_step11_data)
	{
		$query = "UPDATE ddcl_seated_angle_form_step11_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step11_field1_fieldset']['step11_field1_question1'],
			":question_ok_not_ok" => $v['step11_field1_fieldset']['step11_field1_question_ok'],
			":question_not_ok_comment" => $v['step11_field1_fieldset']['step11_field1_question_not_ok_comment'],
			":data_relation" => "step11"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_seated_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step11_data->id
		));
		$form_step11_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step11_field1_fieldset']["step11_comment_count"]; $i++)
		{
			if ($v['step11_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step11_data->id,
					":number" => $i + $form_step11_comment_data->max + 1,
					":comment" => $v['step11_field1_fieldset'][$i]["comment"],
					":data_relation" => "step11",
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
			} //$v['step11_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step11_field1_fieldset']["step11_comment_count"]; $i++
	} //$form_step11_data
	else
	{
		$query = "INSERT INTO {ddcl_seated_angle_form_step11_feedback} (uid, question1,question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step11_field1_fieldset']['step11_field1_question1'],
			":question_ok_not_ok" => $v['step11_field1_fieldset']['step11_field1_question_ok'],
			":question_not_ok_comment" => $v['step11_field1_fieldset']['step11_field1_question_not_ok_comment'],
			":data_relation" => "step11",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$comments = 0;
		for ($i = 0; $i <= $v['step11_field1_fieldset']["step11_comment_count"]; $i++)
		{
			if ($v['step11_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step11_field1_fieldset'][$i]["comment"],
					":data_relation" => "step11",
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
			} //$v['step11_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step11_field1_fieldset']["step11_comment_count"]; $i++
	}
	$form_state['redirect'] = 'ddcl-seated-angle/form/step12/' . $user->uid;
}
/***************************************/
function step11_comment_add_more_add_one($form, &$form_state)
{
	$form_state['step11_num_comment']++;
	$form_state['rebuild'] = TRUE;
	//$form_state['no_redirect'] = TRUE;
}
function step11_comment_add_more_remove_one($form, &$form_state)
{
	if ($form_state['step11_num_comment'] > 1)
	{
		$form_state['step11_num_comment']--;
	} //$form_state['step11_num_comment'] > 1
	$form_state['rebuild'] = TRUE;
}
function step11_comment_add_more_callback($form, &$form_state)
{
	return $form['step11_field1_fieldset'];
}
/***************************************/
/***************************************** step 11 from ends ****************************/
/************************ Step 12 form starts *********************/
function ddcl_seated_angle_step12_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('General Assumptions'));
	$markup_text = '<p>It is assumed that block shear failure is not a governing mode of failure for the seated angle
connection</p>

<hr>
';
	// All the real form fields.
	$form = array();
	$form['step12_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step12-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_seated_angle_form_step12_feedback');
	$query->fields('ddcl_seated_angle_form_step12_feedback');
	$query->condition('uid', $user->uid);
	$form_step12_data = $query->execute();
	if ($form_step12_data)
	{
		if ($step12_data = $form_step12_data->fetchObject())
		{
			$form['step12_field1_fieldset']['step12_field1_question1'] = array(
				'#markup' => $markup_text,
			);
			$form['step12_field1_fieldset']['step12_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				'#default_value' => $step2_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step12_field1_fieldset']['step12_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				'#default_value' => $step2_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step12_field1_fieldset\\[step12_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			$earlier_comments = _fetch_exsiting_ddcl_seated_angle_comments($step12_data->id, $step12_data->data_relation);
			$icom = 1;
			$form['step12_field1_fieldset']['exsiting_comments_title'] = array(
				'#markup' => '<hr><strong>Earlier comment<strong><hr><br>'
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step12_field1_fieldset']['exsiting_comments' . $icom] = array(
					'#markup' => $rowcomment
				);
				$icom++;
			} //$earlier_comments as $rowcomment
			if (empty($form_state['step12_num_comment']))
			{
				$form_state['step12_num_comment'] = 1;
			} //empty($form_state['step12_num_comment'])
			$step12_temp1 = 0;
			for ($step12_i = 0; $step12_i < $form_state['step12_num_comment']; $step12_i++)
			{
				$step12_temp1 = $step12_i;
				$form['step12_field1_fieldset'][$step12_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step12_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step12_field1_fieldset']["step12_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step12_temp1
			);
			$form['step12_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step12_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step12_comment_add_more_callback',
					'wrapper' => 'step12-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step12_num_comment'] > 1)
			{
				$form['step12_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step12_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step12_comment_add_more_callback',
						'wrapper' => 'step12-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step12_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step12_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step12_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step12_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_seated_angle_step12_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_seated_angle_step12_form_next_submit'
				)
			);
		} //$step12_data = $form_step12_data->fetchObject()
		else
		{
			$form['step12_field1_fieldset']['step12_field1_question1'] = array(
				'#markup' => $markup_text,
			);
			$form['step12_field1_fieldset']['step12_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				//'#default_value' => $step2_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step12_field1_fieldset']['step12_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				//'#default_value' => $step2_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step12_field1_fieldset\\[step12_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			if (empty($form_state['step12_num_comment']))
			{
				$form_state['step12_num_comment'] = 1;
			} //empty($form_state['step12_num_comment'])
			$step12_temp1 = 0;
			for ($step12_i = 0; $step12_i < $form_state['step12_num_comment']; $step12_i++)
			{
				$step12_temp1 = $step12_i;
				$form['step12_field1_fieldset'][$step12_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step12_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step12_field1_fieldset']["step12_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step12_temp1
			);
			$form['step12_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step12_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step12_comment_add_more_callback',
					'wrapper' => 'step12-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step12_num_comment'] > 1)
			{
				$form['step12_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step12_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step12_comment_add_more_callback',
						'wrapper' => 'step12-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step12_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step12_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step12_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step12_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_seated_angle_step12_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_seated_angle_step12_form_next_submit'
				)
			);
		} //$step12_data = $form_step12_data->fetchObject()
	} //$form_step12_data
	return $form;
}
function ddcl_seated_angle_step12_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step12_field1_fieldset']['step12_field1_question1'] = 'question 1';
	$query = db_select('ddcl_seated_angle_form_step12_feedback');
	$query->fields('ddcl_seated_angle_form_step12_feedback');
	$query->condition('uid', $user->uid);
	$form_step12_data = $query->execute()->fetchObject();
	//var_dump($form_step12_data);die;
	if ($form_step12_data)
	{
		$query = "UPDATE ddcl_seated_angle_form_step12_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step12_field1_fieldset']['step12_field1_question1'],
			":question_ok_not_ok" => $v['step12_field1_fieldset']['step12_field1_question_ok'],
			":question_not_ok_comment" => $v['step12_field1_fieldset']['step12_field1_question_not_ok_comment'],
			":data_relation" => "step12"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_seated_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step12_data->id
		));
		$form_step12_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step12_field1_fieldset']["step12_comment_count"]; $i++)
		{
			if ($v['step12_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step12_data->id,
					":number" => $i + $form_step12_comment_data->max + 1,
					":comment" => $v['step12_field1_fieldset'][$i]["comment"],
					":data_relation" => "step12",
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
			} //$v['step12_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step12_field1_fieldset']["step12_comment_count"]; $i++
	} //$form_step12_data
	else
	{
		$query = "INSERT INTO {ddcl_seated_angle_form_step12_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step12_field1_fieldset']['step12_field1_question1'],
			":question_ok_not_ok" => $v['step12_field1_fieldset']['step12_field1_question_ok'],
			":question_not_ok_comment" => $v['step12_field1_fieldset']['step12_field1_question_not_ok_comment'],
			":data_relation" => "step12",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$comments = 0;
		for ($i = 0; $i <= $v['step12_field1_fieldset']["step12_comment_count"]; $i++)
		{
			if ($v['step12_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step12_field1_fieldset'][$i]["comment"],
					":data_relation" => "step12",
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
			} //$v['step12_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step12_field1_fieldset']["step12_comment_count"]; $i++
	}
	$form_state['redirect'] = 'ddcl-seated-angle/form/step11/' . $user->uid;
}
function ddcl_seated_angle_step12_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step12_field1_fieldset']['step12_field1_question1'] = 'question 1';
	$query = db_select('ddcl_seated_angle_form_step12_feedback');
	$query->fields('ddcl_seated_angle_form_step12_feedback');
	$query->condition('uid', $user->uid);
	$form_step12_data = $query->execute()->fetchObject();
	//var_dump($form_step12_data);die;
	if ($form_step12_data)
	{
		$query = "UPDATE ddcl_seated_angle_form_step12_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step12_field1_fieldset']['step12_field1_question1'],
			":question_ok_not_ok" => $v['step12_field1_fieldset']['step12_field1_question_ok'],
			":question_not_ok_comment" => $v['step12_field1_fieldset']['step12_field1_question_not_ok_comment'],
			":data_relation" => "step12"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_seated_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step12_data->id
		));
		$form_step12_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step12_field1_fieldset']["step12_comment_count"]; $i++)
		{
			if ($v['step12_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step12_data->id,
					":number" => $i + $form_step12_comment_data->max + 1,
					":comment" => $v['step12_field1_fieldset'][$i]["comment"],
					":data_relation" => "step12",
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
			} //$v['step12_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step12_field1_fieldset']["step12_comment_count"]; $i++
	} //$form_step12_data
	else
	{
		$query = "INSERT INTO {ddcl_seated_angle_form_step12_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step12_field1_fieldset']['step12_field1_question1'],
			":question_ok_not_ok" => $v['step12_field1_fieldset']['step12_field1_question_ok'],
			":question_not_ok_comment" => $v['step12_field1_fieldset']['step12_field1_question_not_ok_comment'],
			":data_relation" => "step12",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$comments = 0;
		for ($i = 0; $i <= $v['step12_field1_fieldset']["step12_comment_count"]; $i++)
		{
			if ($v['step12_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step12_field1_fieldset'][$i]["comment"],
					":data_relation" => "step12",
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
			} //$v['step12_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step12_field1_fieldset']["step12_comment_count"]; $i++
	}
	$form_state['redirect'] = 'ddcl-seated-angle/form/step13/' . $user->uid;
}
/***************************************/
function step12_comment_add_more_add_one($form, &$form_state)
{
	$form_state['step12_num_comment']++;
	$form_state['rebuild'] = TRUE;
	//$form_state['no_redirect'] = TRUE;
}
function step12_comment_add_more_remove_one($form, &$form_state)
{
	if ($form_state['step12_num_comment'] > 1)
	{
		$form_state['step12_num_comment']--;
	} //$form_state['step12_num_comment'] > 1
	$form_state['rebuild'] = TRUE;
}
function step12_comment_add_more_callback($form, &$form_state)
{
	return $form['step12_field1_fieldset'];
}
/***************************************/
/***************************************** step 12 from ends ****************************/
/************************ Step 13 form starts *********************/
function ddcl_seated_angle_step13_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('d. Top Angle'));
	$img_path = $base_url . '/' . drupal_get_path("module", "osdag_response_form") . '/images';
	$markup_text = '<p><strong>d. Top Angle</strong><br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; Literature suggests use of "nominal size" for the top angle (from stability consideration).<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; However the term nominal size is not well defined.</p>

<p>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; The AISC Steel Construction Manual, 14th Ed, Page 10-84, suggests<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; A 1/4-in.-thick angle with a 4-in., vertical leg dimension will generally be adequate</p>
<img src="' . $img_path . '/seated-top-angle.png" style="width: 600px;" />
<p>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; This translates to 6.35 mm thick angle with a 101.6 mm vertical leg<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; Can ISA 100 65x6 or ISA 100 65x8 be used?<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; Can we directly use the above sizes in the Indian context?</p>



<hr>
';
	// All the real form fields.
	$form = array();
	$form['step13_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step13-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_seated_angle_form_step13_feedback');
	$query->fields('ddcl_seated_angle_form_step13_feedback');
	$query->condition('uid', $user->uid);
	$form_step13_data = $query->execute();
	if ($form_step13_data)
	{
		if ($step13_data = $form_step13_data->fetchObject())
		{
			$form['step13_field1_fieldset']['step13_field1_question1'] = array(
				'#markup' => $markup_text,
			);
			$form['step13_field1_fieldset']['step13_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				'#default_value' => $step13_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step13_field1_fieldset']['step13_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				'#default_value' => $step13_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step13_field1_fieldset\\[step13_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			$earlier_comments = _fetch_exsiting_ddcl_seated_angle_comments($step13_data->id, $step13_data->data_relation);
			$icom = 1;
			$form['step13_field1_fieldset']['exsiting_comments_title'] = array(
				'#markup' => 'Earlier comment<br>'
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step13_field1_fieldset']['exsiting_comments' . $icom] = array(
					'#markup' => $rowcomment
				);
				$icom++;
			} //$earlier_comments as $rowcomment
			if (empty($form_state['step13_num_comment']))
			{
				$form_state['step13_num_comment'] = 1;
			} //empty($form_state['step13_num_comment'])
			$step13_temp1 = 0;
			for ($step13_i = 0; $step13_i < $form_state['step13_num_comment']; $step13_i++)
			{
				$step13_temp1 = $step13_i;
				$form['step13_field1_fieldset'][$step13_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step13_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step13_field1_fieldset']["step13_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step13_temp1
			);
			$form['step13_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step13_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step13_comment_add_more_callback',
					'wrapper' => 'step13-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step13_num_comment'] > 1)
			{
				$form['step13_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step13_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step13_comment_add_more_callback',
						'wrapper' => 'step13-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step13_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step13_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step13_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step13_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_seated_angle_step13_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Submit',
				'#attributes' => array(
					"onclick" => "return confirm('Are you sure?')"
				),
				'#submit' => array(
					'ddcl_seated_angle_step13_form_next_submit'
				)
			);
			$form['save_submit'] = array(
				'#type' => 'submit',
				'#value' => 'Save as draft',
				'#attributes' => array(
					"onclick" => "return confirm('Are you sure?')"
				),
				'#submit' => array(
					'ddcl_seated_angle_step13_form_save_submit'
				)
			);
		} //$step13_data = $form_step13_data->fetchObject()
		else
		{
			$form['step13_field1_fieldset']['step13_field1_question1'] = array(
				'#markup' => $markup_text,
			);
			$form['step13_field1_fieldset']['step13_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				//'#default_value' => $step13_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step13_field1_fieldset']['step13_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				//'#default_value' => $step13_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step13_field1_fieldset\\[step13_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			if (empty($form_state['step13_num_comment']))
			{
				$form_state['step13_num_comment'] = 1;
			} //empty($form_state['step13_num_comment'])
			$step13_temp1 = 0;
			for ($step13_i = 0; $step13_i < $form_state['step13_num_comment']; $step13_i++)
			{
				$step13_temp1 = $step13_i;
				$form['step13_field1_fieldset'][$step13_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step13_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step13_field1_fieldset']["step13_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step13_temp1
			);
			$form['step13_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step13_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step13_comment_add_more_callback',
					'wrapper' => 'step13-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step13_num_comment'] > 1)
			{
				$form['step13_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step13_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step13_comment_add_more_callback',
						'wrapper' => 'step13-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step13_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step13_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step13_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step13_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_seated_angle_step13_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Submit',
				'#attributes' => array(
					"onclick" => "return confirm('Are you sure?')"
				),
				'#submit' => array(
					'ddcl_seated_angle_step13_form_next_submit'
				)
			);
			$form['save_submit'] = array(
				'#type' => 'submit',
				'#value' => 'Save as draft',
				'#attributes' => array(
					"onclick" => "return confirm('Are you sure?')"
				),
				'#submit' => array(
					'ddcl_seated_angle_step13_form_save_submit'
				)
			);
		} //$step13_data = $form_step13_data->fetchObject()
	} //$form_step13_data
	return $form;
}
function ddcl_seated_angle_step13_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step13_field1_fieldset']['step13_field1_question1'] = 'question 1';
	$query = "SELECT * FROM ddcl_seated_angle_form_step13_feedback WHERE uid=:uid";
	$args = array(
		":uid" => $user->uid
	);
	$form_step7_data = db_query($query, $args);
	$form_step7_data_count = $form_step7_data->rowCount();
	$form_step7_data = $form_step7_data->fetchObject();
	if ($form_step7_data_count != 0)
	{
		$query = "UPDATE ddcl_seated_angle_form_step13_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step13_field1_fieldset']['step13_field1_question1'],
			":question_ok_not_ok" => $v['step13_field1_fieldset']['step13_field1_question_ok'],
			":question_not_ok_comment" => $v['step13_field1_fieldset']['step13_field1_question_not_ok_comment'],
			":data_relation" => "step13"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_seated_angle_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_seated_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step7_data->id
		));
		$form_step7_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step13_field1_fieldset']["step13_comment_count"]; $i++)
		{
			if ($v['step13_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step7_data->id,
					":number" => $i + $form_step7_comment_data->max + 1,
					":comment" => $v['step13_field1_fieldset'][$i]["comment"],
					":data_relation" => "step13",
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
			} //$v['step13_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step13_field1_fieldset']["step13_comment_count"]; $i++
	} //$form_step7_data
	else
	{
		$query = "INSERT INTO {ddcl_seated_angle_form_step13_feedback} (uid, question1, :question_ok_not_ok, :question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step13_field1_fieldset']['step13_field1_question1'],
			":question_ok_not_ok" => $v['step13_field1_fieldset']['step13_field1_question_ok'],
			":question_not_ok_comment" => $v['step13_field1_fieldset']['step13_field1_question_not_ok_comment'],
			":data_relation" => "step13",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_seated_angle_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
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
		for ($i = 0; $i <= $v['step13_field1_fieldset']["step13_comment_count"]; $i++)
		{
			if ($v['step13_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step13_field1_fieldset'][$i]["comment"],
					":data_relation" => "step13",
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
			} //$v['step13_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step13_field1_fieldset']["step13_comment_count"]; $i++
	}
	$form_state['redirect'] = 'ddcl-seated-angle/form/step11/' . $user->uid;
}
function ddcl_seated_angle_step13_form_save_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step13_field1_fieldset']['step13_field1_question1'] = 'question 1';
	$query = "SELECT * FROM ddcl_seated_angle_form_step13_feedback WHERE uid=:uid";
	$args = array(
		":uid" => $user->uid
	);
	$form_step7_data = db_query($query, $args);
	$form_step7_data_count = $form_step7_data->rowCount();
	$form_step7_data = $form_step7_data->fetchObject();
	if ($form_step7_data_count != 0)
	{
		$query = "UPDATE ddcl_seated_angle_form_step13_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step13_field1_fieldset']['step13_field1_question1'],
			":question_ok_not_ok" => $v['step13_field1_fieldset']['step13_field1_question_ok'],
			":question_not_ok_comment" => $v['step13_field1_fieldset']['step13_field1_question_not_ok_comment'],
			":data_relation" => "step13"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_seated_angle_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 0
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_seated_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step7_data->id
		));
		$form_step7_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step13_field1_fieldset']["step13_comment_count"]; $i++)
		{
			if ($v['step13_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step7_data->id,
					":number" => $i + $form_step7_comment_data->max + 1,
					":comment" => $v['step13_field1_fieldset'][$i]["comment"],
					":data_relation" => "step13",
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
			} //$v['step13_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step13_field1_fieldset']["step13_comment_count"]; $i++
	} //$form_step7_data
	else
	{
		$query = "INSERT INTO {ddcl_seated_angle_form_step13_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1,:question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step13_field1_fieldset']['step13_field1_question1'],
			":question_ok_not_ok" => $v['step13_field1_fieldset']['step13_field1_question_ok'],
			":question_not_ok_comment" => $v['step13_field1_fieldset']['step13_field1_question_not_ok_comment'],
			":data_relation" => "step13",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_seated_angle_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
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
		for ($i = 0; $i <= $v['step13_field1_fieldset']["step13_comment_count"]; $i++)
		{
			if ($v['step13_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step13_field1_fieldset'][$i]["comment"],
					":data_relation" => "step13",
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
			} //$v['step13_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step13_field1_fieldset']["step13_comment_count"]; $i++
	}
	drupal_set_message(t('You have successfully saved your feedback. You can anytime edit your feedback form.'), 'status');
	drupal_goto('home');
}
function ddcl_seated_angle_step13_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step13_field1_fieldset']['step13_field1_question1'] = 'question 1';
	$query = "SELECT * FROM ddcl_seated_angle_form_step13_feedback WHERE uid=:uid";
	$args = array(
		":uid" => $user->uid
	);
	$form_step7_data = db_query($query, $args);
	$form_step7_data_count = $form_step7_data->rowCount();
	$form_step7_data = $form_step7_data->fetchObject();
	if ($form_step7_data_count != 0)
	{
		$query = "UPDATE ddcl_seated_angle_form_step13_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step13_field1_fieldset']['step13_field1_question1'],
			":question_ok_not_ok" => $v['step13_field1_fieldset']['step13_field1_question_ok'],
			":question_not_ok_comment" => $v['step13_field1_fieldset']['step13_field1_question_not_ok_comment'],
			":data_relation" => "step13"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_seated_angle_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_seated_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step7_data->id
		));
		$form_step7_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step13_field1_fieldset']["step13_comment_count"]; $i++)
		{
			if ($v['step13_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step7_data->id,
					":number" => $i + $form_step7_comment_data->max + 1,
					":comment" => $v['step13_field1_fieldset'][$i]["comment"],
					":data_relation" => "step13",
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
			} //$v['step13_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step13_field1_fieldset']["step13_comment_count"]; $i++
	} //$form_step7_data
	else
	{
		$query = "INSERT INTO {ddcl_seated_angle_form_step13_feedback} (uid, question1, :question_ok_not_ok, :question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step13_field1_fieldset']['step13_field1_question1'],
			":question_ok_not_ok" => $v['step13_field1_fieldset']['step13_field1_question_ok'],
			":question_not_ok_comment" => $v['step13_field1_fieldset']['step13_field1_question_not_ok_comment'],
			":data_relation" => "step13",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_seated_angle_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
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
		for ($i = 0; $i <= $v['step13_field1_fieldset']["step13_comment_count"]; $i++)
		{
			if ($v['step13_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_seated_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step13_field1_fieldset'][$i]["comment"],
					":data_relation" => "step13",
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
			} //$v['step13_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step13_field1_fieldset']["step13_comment_count"]; $i++
	}
	$form_state['redirect'] = 'ddcl-seated-angle/success';
}
/***************************************/
function step13_comment_add_more_add_one($form, &$form_state)
{
	$form_state['step13_num_comment']++;
	$form_state['rebuild'] = TRUE;
	//$form_state['no_redirect'] = TRUE;
}
function step13_comment_add_more_remove_one($form, &$form_state)
{
	if ($form_state['step13_num_comment'] > 1)
	{
		$form_state['step13_num_comment']--;
	} //$form_state['step13_num_comment'] > 1
	$form_state['rebuild'] = TRUE;
}
function step13_comment_add_more_callback($form, &$form_state)
{
	return $form['step13_field1_fieldset'];
}
/***************************************/
/***************************************** step 13 from ends ****************************/
/**
 * Displays simple confirmation page after finishing the whole process.
 */
function ddcl_seated_angle_success()
{
	$data .= drupal_set_message(t('You have successfully submitted the feedback.'), 'status');
	$data .= drupal_goto('home');
	return $data;
}
/**********************************************/
function ddcl_seated_angle_response_result_page()
{
	/* get pending proposals to be approved */
	$output = "";
	$output .= l("Download All Result in CSV", "ddcl-seated-angle/form/result/download");
	$feedback_rows = array();
	$feedback_q = db_query('

select

step1.uid as "step1_uid",
step1.name as "step1_name",
step1.inserted_on as "step1_date",
step1.institute as "step1_institute",
step1.current_position as "step1_current_position",

step2.id as "step2_id",
step2.data_relation as "step2_data_relation",
step2.question1 as "step2_question1",
step2.question_ok_not_ok as "step2_question_ok_not_ok",
step2.question_not_ok_comment as "step2_question_not_ok_comment",


step3.id as "step3_id",
step3.data_relation as "step3_data_relation",
step3.question1 as "step3_question1",
step3.question_ok_not_ok as "step3_question_ok_not_ok",
step3.question_not_ok_comment as "step3_question_not_ok_comment",



step4.id as "step4_id",
step4.data_relation as "step4_data_relation",
step4.question1 as "step4_question1",
step4.question_ok_not_ok as "step4_question_ok_not_ok",
step4.question_not_ok_comment as "step4_question_not_ok_comment",


step5.id as "step5_id",
step5.data_relation as "step5_data_relation",
step5.question1 as "step5_question1",
step5.question_ok_not_ok as "step5_question_ok_not_ok",
step5.question_not_ok_comment as "step5_question_not_ok_comment",



step6.id as "step6_id",
step6.data_relation as "step6_data_relation",
step6.question1 as "step6_question1",
step6.question_ok_not_ok as "step6_question_ok_not_ok",
step6.question_not_ok_comment as "step6_question_not_ok_comment",


step7.id as "step7_id",
step7.data_relation as "step7_data_relation",
step7.question1 as "step7_question1",
step7.question_ok_not_ok as "step7_question_ok_not_ok",
step7.question_not_ok_comment as "step7_question_not_ok_comment",


step8.id as "step8_id",
step8.data_relation as "step8_data_relation",
step8.question1 as "step8_question1",
step8.question_ok_not_ok as "step8_question_ok_not_ok",
step8.question_not_ok_comment as "step8_question_not_ok_comment",


step9.id as "step9_id",
step9.data_relation as "step9_data_relation",
step9.question1 as "step9_question1",
step9.question_ok_not_ok as "step9_question_ok_not_ok",
step9.question_not_ok_comment as "step9_question_not_ok_comment",


step10.id as "step10_id",
step10.data_relation as "step10_data_relation",
step10.question1 as "step10_question1",
step10.question_ok_not_ok as "step10_question_ok_not_ok",
step10.question_not_ok_comment as "step10_question_not_ok_comment",


step11.id as "step11_id",
step11.data_relation as "step11_data_relation",
step11.question1 as "step11question1",
step11.question_ok_not_ok as "step11_question_ok_not_ok",
step11.question_not_ok_comment as "step11_question_not_ok_comment",

step12.id as "step12_id",
step12.data_relation as "step12_data_relation",
step12.question1 as "step12_question1",
step12.question_ok_not_ok as "step12_question_ok_not_ok",
step12.question_not_ok_comment as "step12_question_not_ok_comment"


from ddcl_seated_angle_form_step1_feedback step1

join ddcl_seated_angle_form_step2_feedback step2 on step1.uid=step2.uid
join ddcl_seated_angle_form_step3_feedback step3 on step2.uid=step3.uid
join ddcl_seated_angle_form_step4_feedback step4 on step3.uid=step4.uid
join ddcl_seated_angle_form_step5_feedback step5 on step4.uid=step5.uid
join ddcl_seated_angle_form_step6_feedback step6 on step5.uid=step6.uid
join ddcl_seated_angle_form_step7_feedback step7 on step6.uid=step7.uid
join ddcl_seated_angle_form_step8_feedback step8 on step7.uid=step8.uid
join ddcl_seated_angle_form_step9_feedback step9 on step8.uid=step9.uid
join ddcl_seated_angle_form_step10_feedback step10 on step9.uid=step10.uid
join ddcl_seated_angle_form_step11_feedback step11 on step10.uid=step11.uid
join ddcl_seated_angle_form_step12_feedback step12 on step11.uid=step12.uid
	');
	$i = 1;
	while ($feedback_data = $feedback_q->fetchObject())
	{
		$feedback_rows[$feedback_data->step1_uid] = array(
			$i,
			$feedback_data->step1_date,
			$feedback_data->step1_name,
			$feedback_data->step1_institute,
			$feedback_data->step1_current_position,
			$feedback_data->step2_question_ok_not_ok,
			$feedback_data->step2_question_not_ok_comment,
			_fetch_ddcl_seated_angle_exsiting_comments_result($feedback_data->step2_id, $feedback_data->step2_data_relation),
			$feedback_data->step3_question_ok_not_ok,
			$feedback_data->step3_question_not_ok_comment,
			_fetch_ddcl_seated_angle_exsiting_comments_result($feedback_data->step3_id, $feedback_data->step3_data_relation),
			$feedback_data->step4_question_ok_not_ok,
			$feedback_data->step4_question_not_ok_comment,
			_fetch_ddcl_seated_angle_exsiting_comments_result($feedback_data->step4_id, $feedback_data->step4_data_relation),
			$feedback_data->step5_question_ok_not_ok,
			$feedback_data->step5_question_not_ok_comment,
			_fetch_ddcl_seated_angle_exsiting_comments_result($feedback_data->step5_id, $feedback_data->step5_data_relation),
			$feedback_data->step6_question_ok_not_ok,
			$feedback_data->step6_question_not_ok_comment,
			_fetch_ddcl_seated_angle_exsiting_comments_result($feedback_data->step6_id, $feedback_data->step6_data_relation),
			$feedback_data->step7_question_ok_not_ok,
			$feedback_data->step7_question_not_ok_comment,
			_fetch_ddcl_seated_angle_exsiting_comments_result($feedback_data->step7_id, $feedback_data->step7_data_relation),
			$feedback_data->step8_question_ok_not_ok,
			$feedback_data->step8_question_not_ok_comment,
			_fetch_ddcl_seated_angle_exsiting_comments_result($feedback_data->step8_id, $feedback_data->step8_data_relation),
			$feedback_data->step9_question_ok_not_ok,
			$feedback_data->step9_question_not_ok_comment,
			_fetch_ddcl_seated_angle_exsiting_comments_result($feedback_data->step9_id, $feedback_data->step9_data_relation),
			$feedback_data->step10_question_ok_not_ok,
			$feedback_data->step10_question_not_ok_comment,
			_fetch_ddcl_seated_angle_exsiting_comments_result($feedback_data->step10_id, $feedback_data->step10_data_relation),
			$feedback_data->step11_question_ok_not_ok,
			$feedback_data->step11_question_not_ok_comment,
			_fetch_ddcl_seated_angle_exsiting_comments_result($feedback_data->step11_id, $feedback_data->step11_data_relation),
			$feedback_data->step12_question_ok_not_ok,
			$feedback_data->step12_question_not_ok_comment,
			_fetch_ddcl_seated_angle_exsiting_comments_result($feedback_data->step12_id, $feedback_data->step12_data_relation)
		);
		$i++;
	} //$pending_data = $pending_q->fetchObject()
	/* check if there are any pending proposals */
	if (!$feedback_rows)
	{
		drupal_set_message(t('There are no results.'), 'status');
		return '';
	} //!$pending_rows
	$data_header = array(
		'No.',
		'Date of Submission',
		'Name',
		'Institute / Company',
		'Current Position',
		'Page 2 Is this ok/not ok',
		'Page 2 Is this ok/not ok comment',
		'Page 2 comments',
		'Page 3 Is this ok/not ok',
		'Page 3 Is this ok/not ok comment',
		'Page 3 comments',
		'Page 4 Is this ok/not ok',
		'Page 4 Is this ok/not ok comment',
		'Page 4 comments',
		'Page 5 Is this ok/not ok',
		'Page 5 Is this ok/not ok comment',
		'Page 5 comments',
		'Page 6 Is this ok/not ok',
		'Page 6 Is this ok/not ok comment',
		'Page 6 comments',
		'Page 7 Is this ok/not ok',
		'Page 7 Is this ok/not ok comment',
		'Page 7 comments',
		'Page 8 Is this ok/not ok',
		'Page 8 Is this ok/not ok comment',
		'Page 8 comments',
		'Page 9 Is this ok/not ok',
		'Page 9 Is this ok/not ok comment',
		'Page 9 comments',
		'Page 10 Is this ok/not ok',
		'Page 10 Is this ok/not ok comment',
		'Page 10 comments',
		'Page 11 Is this ok/not ok',
		'Page 11 Is this ok/not ok comment',
		'Page 11 comments',
		'Page 12 Is this ok/not ok',
		'Page 12 Is this ok/not ok comment',
		'Page 12 comments',
	);
	$output .= theme('table', array(
		'header' => $data_header,
		'rows' => $feedback_rows,
		'sticky' => FALSE
	));
	return $output;
}
//displayed in user page
function _fetch_exsiting_ddcl_seated_angle_comments($step_id, $data_relation)
{
	global $base_url;
	$query_for_comment = db_query("SELECT * FROM ddcl_seated_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id AND data_relation=:data_relation ORDER BY number ASC", array(
		':step1_form_id' => $step_id,
		':data_relation' => $data_relation
	));
	$comment = array();
	$i = 0;
	if ($query_for_comment != NULL)
	{
		while ($row = $query_for_comment->fetchObject())
		{
			$comment[$i] = $i + 1 . ") " . $row->comment . "<br><h6 style='text-align: right;'>
			<a href='{$base_url}/ddcl-seated-angle/delete/{$row->data_relation}/{$row->id}'>Delete</a><br>" . $row->inserted_on . "</h6><br>";
			$i++;
		} //$row = $query_for_comment->fetchObject()
	} //$query_for_comment == NULL
	else
	{
		$comment[$i] = 'No earlier comments';
	}
	return $comment;
}
function ddcl_seated_angle_delete_comment()
{
	global $base_url;
	$comment_id = arg(3);
	$comment_step = arg(2);
	if ($comment_id && user_access("osdag_feedback_form"))
	{
		$query = "DELETE FROM {ddcl_seated_angle_feedback_form_comment} WHERE id = :id";
		$query_result = db_query($query, array(
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
	drupal_goto("{$base_url}/ddcl-seated-angle/form/{$comment_step}/");
}
function _fetch_ddcl_seated_angle_exsiting_comments($step_id, $data_relation)
{
	$query_for_comment = db_query("SELECT * FROM ddcl_seated_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id AND data_relation=:data_relation ORDER BY number ASC", array(
		':step1_form_id' => $step_id,
		':data_relation' => $data_relation
	));
	$comment = array();
	$i = 0;
	if ($query_for_comment != NULL)
	{
		while ($row = $query_for_comment->fetchObject())
		{
			$comment[$i] = $i + 1 . ") " . $row->comment . "<br><h6 style='text-align: right;'>" . $row->inserted_on . "</h6><br>";
			$i++;
		} //$row = $query_for_comment->fetchObject()
	} //$query_for_comment == NULL
	else
	{
		$comment[$i] = 'No earlier comments';
	}
	return $comment;
}
function _fetch_ddcl_seated_angle_exsiting_comments_result($step_id, $data_relation)
{
	$query_for_comment = db_query("SELECT * FROM ddcl_seated_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id AND data_relation=:data_relation ORDER BY number ASC", array(
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
function _fetch_ddcl_seated_angle_exsiting_comments_result_csv($step_id, $data_relation)
{
	$query_for_comment = db_query("SELECT * FROM ddcl_seated_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id AND data_relation=:data_relation ORDER BY number ASC", array(
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
	return trim($text);
}
function ddcl_seated_angle_result_generate_csv()
{
	global $user;
	if ($user->uid != NULL)
	{
		$result_dir = $_SERVER['DOCUMENT_ROOT'] . base_path() . "ddcl-seated-angle-form-result/{$user->uid}/";
		if (!file_exists($result_dir))
		{
			mkdir($result_dir, 0755, TRUE);
		} //!file_exists($result_dir)
		$result_file = $result_dir . "ddcl-seated-angle-result-{$user->uid}" . ".csv";
		$csv_file = "ddcl-seated-angle-result-{$user->uid}" . ".csv";
		$fp = fopen($result_file, "w");
		/* making the first row */
		$item = array(
			'No.',
		'Date of Submission',
		'Name',
		'Institute / Company',
		'Current Position',
		'Page 2 Is this ok/not ok',
		'Page 2 Is this ok/not ok comment',
		'Page 2 comments',
		'Page 3 Is this ok/not ok',
		'Page 3 Is this ok/not ok comment',
		'Page 3 comments',
		'Page 4 Is this ok/not ok',
		'Page 4 Is this ok/not ok comment',
		'Page 4 comments',
		'Page 5 Is this ok/not ok',
		'Page 5 Is this ok/not ok comment',
		'Page 5 comments',
		'Page 6 Is this ok/not ok',
		'Page 6 Is this ok/not ok comment',
		'Page 6 comments',
		'Page 7 Is this ok/not ok',
		'Page 7 Is this ok/not ok comment',
		'Page 7 comments',
		'Page 8 Is this ok/not ok',
		'Page 8 Is this ok/not ok comment',
		'Page 8 comments',
		'Page 9 Is this ok/not ok',
		'Page 9 Is this ok/not ok comment',
		'Page 9 comments',
		'Page 10 Is this ok/not ok',
		'Page 10 Is this ok/not ok comment',
		'Page 10 comments',
		'Page 11 Is this ok/not ok',
		'Page 11 Is this ok/not ok comment',
		'Page 11 comments',
		'Page 12 Is this ok/not ok',
		'Page 12 Is this ok/not ok comment',
		'Page 12 comments',
		);
		fputcsv($fp, $item);
		$query = '

select

step1.uid as "step1_uid",
step1.name as "step1_name",
step1.inserted_on as "step1_date",
step1.institute as "step1_institute",
step1.current_position as "step1_current_position",

step2.id as "step2_id",
step2.data_relation as "step2_data_relation",
step2.question1 as "step2_question1",
step2.question_ok_not_ok as "step2_question_ok_not_ok",
step2.question_not_ok_comment as "step2_question_not_ok_comment",


step3.id as "step3_id",
step3.data_relation as "step3_data_relation",
step3.question1 as "step3_question1",
step3.question_ok_not_ok as "step3_question_ok_not_ok",
step3.question_not_ok_comment as "step3_question_not_ok_comment",



step4.id as "step4_id",
step4.data_relation as "step4_data_relation",
step4.question1 as "step4_question1",
step4.question_ok_not_ok as "step4_question_ok_not_ok",
step4.question_not_ok_comment as "step4_question_not_ok_comment",


step5.id as "step5_id",
step5.data_relation as "step5_data_relation",
step5.question1 as "step5_question1",
step5.question_ok_not_ok as "step5_question_ok_not_ok",
step5.question_not_ok_comment as "step5_question_not_ok_comment",



step6.id as "step6_id",
step6.data_relation as "step6_data_relation",
step6.question1 as "step6_question1",
step6.question_ok_not_ok as "step6_question_ok_not_ok",
step6.question_not_ok_comment as "step6_question_not_ok_comment",


step7.id as "step7_id",
step7.data_relation as "step7_data_relation",
step7.question1 as "step7_question1",
step7.question_ok_not_ok as "step7_question_ok_not_ok",
step7.question_not_ok_comment as "step7_question_not_ok_comment",


step8.id as "step8_id",
step8.data_relation as "step8_data_relation",
step8.question1 as "step8_question1",
step8.question_ok_not_ok as "step8_question_ok_not_ok",
step8.question_not_ok_comment as "step8_question_not_ok_comment",


step9.id as "step9_id",
step9.data_relation as "step9_data_relation",
step9.question1 as "step9_question1",
step9.question_ok_not_ok as "step9_question_ok_not_ok",
step9.question_not_ok_comment as "step9_question_not_ok_comment",


step10.id as "step10_id",
step10.data_relation as "step10_data_relation",
step10.question1 as "step10_question1",
step10.question_ok_not_ok as "step10_question_ok_not_ok",
step10.question_not_ok_comment as "step10_question_not_ok_comment",


step11.id as "step11_id",
step11.data_relation as "step11_data_relation",
step11.question1 as "step11question1",
step11.question_ok_not_ok as "step11_question_ok_not_ok",
step11.question_not_ok_comment as "step11_question_not_ok_comment",

step12.id as "step12_id",
step12.data_relation as "step12_data_relation",
step12.question1 as "step12_question1",
step12.question_ok_not_ok as "step12_question_ok_not_ok",
step12.question_not_ok_comment as "step12_question_not_ok_comment"


from ddcl_seated_angle_form_step1_feedback step1

join ddcl_seated_angle_form_step2_feedback step2 on step1.uid=step2.uid
join ddcl_seated_angle_form_step3_feedback step3 on step2.uid=step3.uid
join ddcl_seated_angle_form_step4_feedback step4 on step3.uid=step4.uid
join ddcl_seated_angle_form_step5_feedback step5 on step4.uid=step5.uid
join ddcl_seated_angle_form_step6_feedback step6 on step5.uid=step6.uid
join ddcl_seated_angle_form_step7_feedback step7 on step6.uid=step7.uid
join ddcl_seated_angle_form_step8_feedback step8 on step7.uid=step8.uid
join ddcl_seated_angle_form_step9_feedback step9 on step8.uid=step9.uid
join ddcl_seated_angle_form_step10_feedback step10 on step9.uid=step10.uid
join ddcl_seated_angle_form_step11_feedback step11 on step10.uid=step11.uid
join ddcl_seated_angle_form_step12_feedback step12 on step11.uid=step12.uid
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
			$feedback_data->step2_question_ok_not_ok,
			$feedback_data->step2_question_not_ok_comment,
			_fetch_ddcl_seated_angle_exsiting_comments_result_csv($feedback_data->step2_id, $feedback_data->step2_data_relation),
			$feedback_data->step3_question_ok_not_ok,
			$feedback_data->step3_question_not_ok_comment,
			_fetch_ddcl_seated_angle_exsiting_comments_result_csv($feedback_data->step3_id, $feedback_data->step3_data_relation),
			$feedback_data->step4_question_ok_not_ok,
			$feedback_data->step4_question_not_ok_comment,
			_fetch_ddcl_seated_angle_exsiting_comments_result_csv($feedback_data->step4_id, $feedback_data->step4_data_relation),
			$feedback_data->step5_question_ok_not_ok,
			$feedback_data->step5_question_not_ok_comment,
			_fetch_ddcl_seated_angle_exsiting_comments_result_csv($feedback_data->step5_id, $feedback_data->step5_data_relation),
			$feedback_data->step6_question_ok_not_ok,
			$feedback_data->step6_question_not_ok_comment,
			_fetch_ddcl_seated_angle_exsiting_comments_result_csv($feedback_data->step6_id, $feedback_data->step6_data_relation),
			$feedback_data->step7_question_ok_not_ok,
			$feedback_data->step7_question_not_ok_comment,
			_fetch_ddcl_seated_angle_exsiting_comments_result_csv($feedback_data->step7_id, $feedback_data->step7_data_relation),
			$feedback_data->step8_question_ok_not_ok,
			$feedback_data->step8_question_not_ok_comment,
			_fetch_ddcl_seated_angle_exsiting_comments_result_csv($feedback_data->step8_id, $feedback_data->step8_data_relation),
			$feedback_data->step9_question_ok_not_ok,
			$feedback_data->step9_question_not_ok_comment,
			_fetch_ddcl_seated_angle_exsiting_comments_result_csv($feedback_data->step9_id, $feedback_data->step9_data_relation),
			$feedback_data->step10_question_ok_not_ok,
			$feedback_data->step10_question_not_ok_comment,
			_fetch_ddcl_seated_angle_exsiting_comments_result_csv($feedback_data->step10_id, $feedback_data->step10_data_relation),
			$feedback_data->step11_question_ok_not_ok,
			$feedback_data->step11_question_not_ok_comment,
			_fetch_ddcl_seated_angle_exsiting_comments_result_csv($feedback_data->step11_id, $feedback_data->step11_data_relation),
			$feedback_data->step12_question_ok_not_ok,
			$feedback_data->step12_question_not_ok_comment,
			_fetch_ddcl_seated_angle_exsiting_comments_result_csv($feedback_data->step12_id, $feedback_data->step12_data_relation)
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
