<?php
/**
 * Fetches and displays the form.
 */
function ddcl_cleat_angle_form_page()
{
	global $user;
	$data = '';
	if (!$user)
	{
		$data .= "Please login to give feedback";
	} //!$user
	/************* check if user has already submitted a proposal **************/
	$query = db_select('ddcl_cleat_angle_form_step1_feedback');
	$query->fields('ddcl_cleat_angle_form_step1_feedback');
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
				$data = drupal_set_message(t('You have already submitted the feedback.'), 'status');
				$data .= drupal_goto('');
				return $data;
			} //$feedback_data->submitted == 1
			else
			{
				$ddcl_cleat_angle_step1_form = drupal_get_form("ddcl_cleat_angle_step1_form");
				$data .= drupal_render($ddcl_cleat_angle_step1_form);
			}
		} //$feedback_data = $feedback_q->fetchObject()
		else
		{
			$ddcl_cleat_angle_step1_form = drupal_get_form("ddcl_cleat_angle_step1_form");
			$data .= drupal_render($ddcl_cleat_angle_step1_form);
		}
	} //$feedback_q
	//$data .= drupal_get_form('ddcl_cleat_angle_step1_form');
	return $data;
}
function ddcl_cleat_angle_step1_form($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	global $base_url;
	$markup_text = "Form Title";
	// All the real form fields.
	$form = array();
	$form['step1_header'] = array(
		'#markup' => $markup_text,
		'#required' => true
	);
	$query = db_select('ddcl_cleat_angle_form_step1_feedback');
	$query->fields('ddcl_cleat_angle_form_step1_feedback');
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
			'ddcl_cleat_angle_step1_form_submit'
		)
	);
	return $form;
}
function ddcl_cleat_angle_step1_form_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$query = db_select('ddcl_cleat_angle_form_step1_feedback');
	$query->fields('ddcl_cleat_angle_form_step1_feedback');
	$query->condition('uid', $user->uid);
	$form_step1_data = $query->execute();
	if ($form_step1_data)
	{
		if ($step1_data = $form_step1_data->fetchObject())
		{
			$query = "UPDATE ddcl_cleat_angle_form_step1_feedback SET name=:name, email_id=:email_id, institute=:institute, current_position=:current_position WHERE uid=:uid ";
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
			$query = "INSERT INTO {ddcl_cleat_angle_form_step1_feedback} (uid, name, email_id, institute, current_position, data_reletion, inserted_on) VALUES (:uid,:name,:email_id, :institute, :current_position, :data_reletion,:inserted_on)";
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
	$form_state['redirect'] = 'ddcl-cleat-angle/form/step2/' . $user->uid;
}
/************************ Step 1 form ends *********************/
/*************************************************************************/
/************************ Step 2 form starts *********************/
function ddcl_cleat_angle_step2_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('a. Material strength'));
	$markup_text = '
	<b>a. Material Strength </b><br>
		&emsp;&emsp;&emsp; <b> i. Yield Stress limits </b>[Reference: Table-2, IS 800] <br>					
					<center>
					<math xmlns="http://www.w3.org/1998/Math/MathML">
					<mn>165</mn>
					<mo>&lt;</mo>
					<msub>
						<mi>f</mi>
						<mi>y</mi>					  
					</msub>
					<mo>&lt;</mo>
					<mn>290</mn>
					</math>
					</center>		
		<br>
		&emsp;&emsp;&emsp; <b> ii. Ultimate Strength limits </b> [Reference: Table-2, IS 800] <br>
					<center>
					<math xmlns="http://www.w3.org/1998/Math/MathML">
					<mn>390</mn>
					<mo>&lt;</mo>
					<msub>
						<mi>f</mi>
						<mi>u</mi>
					</msub>
					<mo>&lt;</mo>
					<mn>690</mn>
					</math>
					</center>	
	<hr>';
	$form = array();	
	$form['step2_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step2-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_cleat_angle_form_step2_feedback');
	$query->fields('ddcl_cleat_angle_form_step2_feedback');
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
			$earlier_comments = _fetch_exsiting_ddcl_cleat_angle_comments($step2_data->id, $step2_data->data_relation);
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
					'ddcl_cleat_angle_step2_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_cleat_angle_step2_form_next_submit'
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
					'ddcl_cleat_angle_step2_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_cleat_angle_step2_form_next_submit'
				)
			);
		} //$step2_data = $form_step2_data->fetchObject()
	} //$form_step2_data
	return $form;
}
function ddcl_cleat_angle_step2_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step2_field1_fieldset']['step2_field1_question1'] = 'question 1';
	$query = db_select('ddcl_cleat_angle_form_step2_feedback');
	$query->fields('ddcl_cleat_angle_form_step2_feedback');
	$query->condition('uid', $user->uid);
	$form_step2_data = $query->execute()->fetchObject();
	if ($form_step2_data)
	{
		$query = "UPDATE ddcl_cleat_angle_form_step2_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_cleat_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step2_data->id
		));
		$form_step2_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step2_field1_fieldset']["step2_comment_count"]; $i++)
		{
			if ($v['step2_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_cleat_angle_form_step2_feedback} (uid, question1,question_ok_not_ok,question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1,:question_ok_not_ok,:question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-cleat-angle/form/step1/' . $user->uid;
}
function ddcl_cleat_angle_step2_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step2_field1_fieldset']['step2_field1_question1'] = 'question 1';
	$query = db_select('ddcl_cleat_angle_form_step2_feedback');
	$query->fields('ddcl_cleat_angle_form_step2_feedback');
	$query->condition('uid', $user->uid);
	$form_step2_data = $query->execute()->fetchObject();
	if ($form_step2_data)
	{
		$query = "UPDATE ddcl_cleat_angle_form_step2_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_cleat_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step2_data->id
		));
		$form_step2_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step2_field1_fieldset']["step2_comment_count"]; $i++)
		{
			if ($v['step2_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_cleat_angle_form_step2_feedback} (uid, question1,question_ok_not_ok,question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1,:question_ok_not_ok,:question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-cleat-angle/form/step3/' . $user->uid;
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

/************************ Step 2 form ends *********************/
/***************************************** step 3 form starts **************************/
function ddcl_cleat_angle_step3_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('b. Bolt > i. Shear capacity of bolt > 1) Black bolt'));	
	$markup_text = '
	<b> b. Bolt </b> <br>
	&emsp; <b> i. Shear capacity of bolt </b> <br>
	&emsp;&emsp; <b> 1) Black bolt </b> <br>
	&emsp;&emsp;&emsp;&emsp;This check will be the same, as implemented for the seated angle connection. <br>	
	';
	$markup_text1 = 'specimen text2';
	$tooltip1 = '';
	// All the real form fields.
	$form = array();
	$form['step3_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step3-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_cleat_angle_form_step3_feedback');
	$query->fields('ddcl_cleat_angle_form_step3_feedback');
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
			$earlier_comments = _fetch_exsiting_ddcl_cleat_angle_comments($step3_data->id, $step3_data->data_relation);
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
					'ddcl_cleat_angle_step3_form_previous_submit'
				)
			);
			$form['next3'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_cleat_angle_step3_form_next_submit'
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
					'ddcl_cleat_angle_step3_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_cleat_angle_step3_form_next_submit'
				)
			);
		} //$step3_data = $form_step3_data->fetchObject()
	} //$form_step3_data
	return $form;
}
function ddcl_cleat_angle_step3_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step3_field1_fieldset']['step3_field1_question1'] = 'question 3';
	$query = db_select('ddcl_cleat_angle_form_step3_feedback');
	$query->fields('ddcl_cleat_angle_form_step3_feedback');
	$query->condition('uid', $user->uid);
	$form_step3_data = $query->execute()->fetchObject();
	//var_dump($form_step2_data);die;
	if ($form_step3_data)
	{
		$query = "UPDATE ddcl_cleat_angle_form_step3_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_cleat_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step3_data->id
		));
		$form_step3_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step3_field1_fieldset']["step3_comment_count"]; $i++)
		{
			if ($v['step3_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_cleat_angle_form_step3_feedback} (uid, question1,question_ok_not_ok,question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok,:question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-cleat-angle/form/step2/' . $user->uid;
}
function ddcl_cleat_angle_step3_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step3_field1_fieldset']['step3_field1_question1'] = 'question 3';
	$query = db_select('ddcl_cleat_angle_form_step3_feedback');
	$query->fields('ddcl_cleat_angle_form_step3_feedback');
	$query->condition('uid', $user->uid);
	$form_step3_data = $query->execute()->fetchObject();
	//var_dump($form_step2_data);die;
	if ($form_step3_data)
	{
		$query = "UPDATE ddcl_cleat_angle_form_step3_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_cleat_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step3_data->id
		));
		$form_step3_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step3_field1_fieldset']["step3_comment_count"]; $i++)
		{
			if ($v['step3_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_cleat_angle_form_step3_feedback} (uid, question1,question_ok_not_ok,question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok,:question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-cleat-angle/form/step4/' . $user->uid;
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
function ddcl_cleat_angle_step4_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('b. Bolt > i.Shear capacity of bolt > 2) HSFG bolt'));
	$markup_text = '
	<b> b. Bolt </b> <br>
	&emsp; <b> i. Shear capacity of bolt </b> <br>
	&emsp;&emsp; <b> 2) HSFG bolt [Cl 10.4.3]</b> <br>
	&emsp;&emsp;&emsp; Design for friction type bolting in which slip is required to be limited, a bolt subjected only to a factored design shear force, 
			<math xmlns="http://www.w3.org/1998/Math/MathML">
			  <msub>
				<mi>V</mi>
				<mrow class="MJX-TeXAtom-ORD">
				  <mi>s</mi>
				  <mi>f</mi>
				</mrow>
			  </msub>
			</math>
			in the interface of connections at which lsip cannot be tolerated, shall satisfy the following: <br>
							<center>
							<math xmlns="http://www.w3.org/1998/Math/MathML">
							  <msub>
								<mi>V</mi>
								<mrow class="MJX-TeXAtom-ORD">
								  <mi>s</mi>
								  <mi>f</mi>
								</mrow>
							  </msub>
							  <mo>&#x2264;</mo>
							  <msub>
								<mi>V</mi>
								<mrow class="MJX-TeXAtom-ORD">
								  <mi>d</mi>
								  <mi>s</mi>
								  <mi>f</mi>
								</mrow>
							  </msub>
							</math>
							</center>
					
					&emsp;&emsp;&emsp;&emsp;
					Where, <br>
						&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
						<math xmlns="http://www.w3.org/1998/Math/MathML">
						  <msub>
							<mi>V</mi>
							<mrow class="MJX-TeXAtom-ORD">
							  <mi>d</mi>
							  <mi>s</mi>
							  <mi>f</mi>
							</mrow>
						  </msub>
						  <mo>=</mo>
						  <msub>
							<mi>V</mi>
							<mrow class="MJX-TeXAtom-ORD">
							  <mi>n</mi>
							  <mi>s</mi>
							  <mi>f</mi>
							</mrow>
						  </msub>
						  <mrow class="MJX-TeXAtom-ORD">
							<mo>/</mo>
						  </mrow>
						  <msub>
							<mi>&#x03B3;<!-- γ --></mi>
							<mrow class="MJX-TeXAtom-ORD">
							  <mi>m</mi>
							  <mi>f</mi>
							</mrow>
						  </msub>
						</math>							
						
						<br>
						&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
						<math xmlns="http://www.w3.org/1998/Math/MathML">
						  <msub>
							<mi>V</mi>
							<mrow class="MJX-TeXAtom-ORD">
							  <mi>n</mi>
							  <mi>s</mi>
							  <mi>f</mi>
							</mrow>
						  </msub>
						</math>
						= nominal shear capacity of a bolt as governed by slip for friction type connection, calculated as follows: <br>
						
						&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
						<math xmlns="http://www.w3.org/1998/Math/MathML">
						  <msub>
							<mi>V</mi>
							<mrow class="MJX-TeXAtom-ORD">
							  <mi>n</mi>
							  <mi>s</mi>
							  <mi>f</mi>
							</mrow>
						  </msub>
						  <mo>=</mo>
						  <msub>
							<mi>&#x03BC;<!-- μ --></mi>
							<mi>f</mi>
						  </msub>
						  <msub>
							<mi>n</mi>
							<mi>e</mi>
						  </msub>
						  <msub>
							<mi>K</mi>
							<mi>h</mi>
						  </msub>
						  <msub>
							<mi>F</mi>
							<mn>0</mn>
						  </msub>
						</math>
							
						<br>
						&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
						Where, <br>
						&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
						<math xmlns="http://www.w3.org/1998/Math/MathML">
						  <msub>
							<mi>&#x03BC;<!-- μ --></mi>
							<mi>f</mi>
						  </msub>
						</math>  = coefficient of friction (slip factor) as specified in Table 20 (default value=0.55), <br>
						&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
						<math xmlns="http://www.w3.org/1998/Math/MathML">
						  <msub>
							<mi>n</mi>
							<mi>e</mi>
						  </msub>
						</math>
						= number of effective interfaces offering frictional resitance to slip, <br>
						&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
						<math xmlns="http://www.w3.org/1998/Math/MathML">
						  <msub>
							<mi>K</mi>
							<mi>h</mi>
						  </msub>
						</math>
						= 1.0 for fasteners in clearance holes, <br>
						&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
						= 0.85 for fasteners in oversized and short slotted holes and for fasteners in long slotted <br>
						&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
						holes loaded perpendicular to the slot, <br>
						&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
						= 0.7 for fasteners in long slotted holes loaded parallel to the slot, <br>
						&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
						<math xmlns="http://www.w3.org/1998/Math/MathML">
						  <msub>
							<mi>&#x03B3;<!-- γ --></mi>
							<mrow class="MJX-TeXAtom-ORD">
							  <mi>m</mi>
							  <mi>f</mi>
							</mrow>
						  </msub>
						</math>
						=  1.10 (if slip resistance is designed at service load), <br>
						&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
						=  1.25 (if lsip resistance is designed at ultimate load), <br>
						&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
						<math xmlns="http://www.w3.org/1998/Math/MathML">
						  <msub>
							<mi>F</mi>
							<mi>0</mi>
						  </msub>
						</math>
						= minimum bolt tension (proof load) at installation and may be taken as 
						<math xmlns="http://www.w3.org/1998/Math/MathML">
						  <msub>
							<mi>A</mi>
							<mrow class="MJX-TeXAtom-ORD">
							  <mi>n</mi>
							  <mi>b</mi>
							</mrow>
						  </msub>
						</math>
						<math xmlns="http://www.w3.org/1998/Math/MathML">
						  <msub>
							<mi>f</mi>
							<mi>0</mi>
						  </msub>
						</math>
						, <br>
						&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
						<math xmlns="http://www.w3.org/1998/Math/MathML">
						  <msub>
							<mi>A</mi>
							<mrow class="MJX-TeXAtom-ORD">
							  <mi>n</mi>
							  <mi>b</mi>
							</mrow>
						  </msub>
						</math>
						= net area of the bolt at threads, and <br>
						&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
						<math xmlns="http://www.w3.org/1998/Math/MathML">
						  <msub>
							<mi>f</mi>
							<mi>0</mi>
						  </msub>
						</math>
						= proof stress (= 0.70
						<math xmlns="http://www.w3.org/1998/Math/MathML">
						  <msub>
							<mi>f</mi>
							<mrow class="MJX-TeXAtom-ORD">
							  <mi>u</mi>
							  <mi>b</mi>
							</mrow>
						  </msub>
						</math>
						) <br>
						
					<br>
					<p>Note: Designing the connection such that slip does not occur at ultimate loads <br>
					
					
					<br>
					<b>10.4.3.1 Long joints </b><br>
					&emsp;&emsp;&emsp;The provision for the long joints 10.3.3.1 shall apply to friction grip connections also.

</p>
	
	<hr>';
	// All the real form fields.
	$form = array();
	$form['step4_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step4-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_cleat_angle_form_step4_feedback');
	$query->fields('ddcl_cleat_angle_form_step4_feedback');
	$query->condition('uid', $user->uid);
	$form_step4_data = $query->execute();
	if ($form_step4_data)
	{
		if ($step4_data = $form_step4_data->fetchObject())
		{
			$form['step4_field1_fieldset']['step4_field1_question1'] = array(
				'#markup' => $markup_text
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
			$earlier_comments = _fetch_exsiting_ddcl_cleat_angle_comments($step4_data->id, $step4_data->data_relation);
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
					'ddcl_cleat_angle_step4_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_cleat_angle_step4_form_next_submit'
				)
			);
		} //$step4_data = $form_step4_data->fetchObject()
		else
		{
			$form['step4_field1_fieldset']['step4_field1_question1'] = array(
				'#markup' => $markup_text
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
					'ddcl_cleat_angle_step4_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_cleat_angle_step4_form_next_submit'
				)
			);
		} //$step4_data = $form_step4_data->fetchObject()
	} //$form_step4_data
	return $form;
}
function ddcl_cleat_angle_step4_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step4_field1_fieldset']['step4_field1_question1'] = 'question 1';
	$query = db_select('ddcl_cleat_angle_form_step4_feedback');
	$query->fields('ddcl_cleat_angle_form_step4_feedback');
	$query->condition('uid', $user->uid);
	$form_step4_data = $query->execute()->fetchObject();
	//var_dump($form_step4_data);die;
	if ($form_step4_data)
	{
		$query = "UPDATE ddcl_cleat_angle_form_step4_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_cleat_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step4_data->id
		));
		$form_step4_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step4_field1_fieldset']["step4_comment_count"]; $i++)
		{
			if ($v['step4_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_cleat_angle_form_step4_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-cleat-angle/form/step3/' . $user->uid;
}
function ddcl_cleat_angle_step4_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step4_field1_fieldset']['step4_field1_question1'] = 'question 1';
	$query = db_select('ddcl_cleat_angle_form_step4_feedback');
	$query->fields('ddcl_cleat_angle_form_step4_feedback');
	$query->condition('uid', $user->uid);
	$form_step4_data = $query->execute()->fetchObject();
	//var_dump($form_step4_data);die;
	if ($form_step4_data)
	{
		$query = "UPDATE ddcl_cleat_angle_form_step4_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_cleat_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step4_data->id
		));
		$form_step4_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step4_field1_fieldset']["step4_comment_count"]; $i++)
		{
			if ($v['step4_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_cleat_angle_form_step4_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-cleat-angle/form/step5/' . $user->uid;
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
function ddcl_cleat_angle_step5_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('b. Bolt > ii. Bearing capacity of bolt'));
	$markup_text = '
	<b> b. Bolt </b> <br>
	&emsp; <b> ii. Bearing capacity of bolt </b> <br>	
	&emsp;&emsp;&emsp; This check will be the same as implemented for the seated angle connection.
		
	
	<hr>';
	
	// All the real form fields.
	$form = array();
	$form['step5_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step5-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_cleat_angle_form_step5_feedback');
	$query->fields('ddcl_cleat_angle_form_step5_feedback');
	$query->condition('uid', $user->uid);
	$form_step5_data = $query->execute();
	if ($form_step5_data)
	{
		if ($step5_data = $form_step5_data->fetchObject())
		{
			$form['step5_field1_fieldset']['step5_field1_question1'] = array(
				'#markup' => $markup_text
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
			$earlier_comments = _fetch_exsiting_ddcl_cleat_angle_comments($step5_data->id, $step5_data->data_relation);
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
					'ddcl_cleat_angle_step5_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_cleat_angle_step5_form_next_submit'
				)
			);
		} //$step5_data = $form_step5_data->fetchObject()
		else
		{
			$form['step5_field1_fieldset']['step5_field1_question1'] = array(
				'#markup' => $markup_text
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
					'ddcl_cleat_angle_step5_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_cleat_angle_step5_form_next_submit'
				)
			);
		} //$step5_data = $form_step5_data->fetchObject()
	} //$form_step5_data
	return $form;
}
function ddcl_cleat_angle_step5_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step5_field1_fieldset']['step5_field1_question1'] = 'question 1';
	$query = db_select('ddcl_cleat_angle_form_step5_feedback');
	$query->fields('ddcl_cleat_angle_form_step5_feedback');
	$query->condition('uid', $user->uid);
	$form_step5_data = $query->execute()->fetchObject();
	//var_dump($form_step5_data);die;
	if ($form_step5_data)
	{
		$query = "UPDATE ddcl_cleat_angle_form_step5_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_cleat_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step5_data->id
		));
		$form_step5_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step5_field1_fieldset']["step5_comment_count"]; $i++)
		{
			if ($v['step5_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_cleat_angle_form_step5_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation, inserted_on) VALUES (:uid, :question1, :question_ok_not_ok,:question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-cleat-angle/form/step4/' . $user->uid;
}
function ddcl_cleat_angle_step5_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step5_field1_fieldset']['step5_field1_question1'] = 'question 1';
	$query = db_select('ddcl_cleat_angle_form_step5_feedback');
	$query->fields('ddcl_cleat_angle_form_step5_feedback');
	$query->condition('uid', $user->uid);
	$form_step5_data = $query->execute()->fetchObject();
	//var_dump($form_step5_data);die;
	if ($form_step5_data)
	{
		$query = "UPDATE ddcl_cleat_angle_form_step5_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_cleat_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step5_data->id
		));
		$form_step5_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step5_field1_fieldset']["step5_comment_count"]; $i++)
		{
			if ($v['step5_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_cleat_angle_form_step5_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation, inserted_on) VALUES (:uid, :question1, :question_ok_not_ok,:question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-cleat-angle/form/step6/' . $user->uid;
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
function ddcl_cleat_angle_step6_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('b. Bolt > iii. Bolt value'));
	$markup_text = '
	<b> b. Bolt </b> <br>
	&emsp; <b> iii. Bolt value </b> <br>
	&emsp;&emsp;&emsp; The bolt capacity is taken as the minimum of bolt shear capacity  and bolt bearing capacity. <br>
					<center>
					<math xmlns="http://www.w3.org/1998/Math/MathML">
					  <msub>
						<mi>V</mi>
						<mrow class="MJX-TeXAtom-ORD">
						  <mi>d</mi>
						  <mi>b</mi>
						</mrow>
					  </msub>
					  <mo>=</mo>
					  <mi>m</mi>
					  <mi>i</mi>
					  <mi>n</mi>
					  <mrow class="MJX-TeXAtom-ORD">
						<mo>&#x2061;</mo>
					  </mrow>
					  <mo stretchy="false">(</mo>
					  <msub>
						<mi>V</mi>
						<mrow class="MJX-TeXAtom-ORD">
						  <mi>d</mi>
						  <mi>s</mi>
						  <mi>b</mi>
						</mrow>
					  </msub>
					  <mo>,</mo>
					  <msub>
						<mi>V</mi>
						<mrow class="MJX-TeXAtom-ORD">
						  <mi>d</mi>
						  <mi>p</mi>
						  <mi>b</mi>
						</mrow>
					  </msub>
					  <mo stretchy="false">)</mo>
					</math>
					</center> 
					<br>
					
					&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					Where, <br>
					&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
						<math xmlns="http://www.w3.org/1998/Math/MathML">
						  <msub>
							<mi>V</mi>
							<mrow class="MJX-TeXAtom-ORD">
							  <mi>d</mi>
							  <mi>b</mi>
							</mrow>
						  </msub>
						</math> - design strength of bolt <br>
					&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
						<math xmlns="http://www.w3.org/1998/Math/MathML">
						  <msub>
							<mi>V</mi>
							<mrow class="MJX-TeXAtom-ORD">
							  <mi>d</mi>
							  <mi>s</mi>
							  <mi>b</mi>
							</mrow>
						  </msub>
						</math> - design shear capacity of bolt <br>
					&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
						<math xmlns="http://www.w3.org/1998/Math/MathML">
						  <msub>
							<mi>V</mi>
							<mrow class="MJX-TeXAtom-ORD">
							  <mi>d</mi>
							  <mi>p</mi>
							  <mi>b</mi>
							</mrow>
						  </msub>
						</math> - design bearing capacity of bolt 
						<br>
				<br>
	&emsp; <b> iv. Number of bolts (n)</b> <br>
	&emsp;&emsp;&emsp; n = roundup(Factored shear force/ bolt capacity) <br>
		
	<hr>';
	
	// All the real form fields.
	$form = array();
	$form['step6_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step6-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_cleat_angle_form_step6_feedback');
	$query->fields('ddcl_cleat_angle_form_step6_feedback');
	$query->condition('uid', $user->uid);
	$form_step6_data = $query->execute();
	if ($form_step6_data)
	{
		if ($step6_data = $form_step6_data->fetchObject())
		{
			$form['step6_field1_fieldset']['step6_field1_question1'] = array(
				'#markup' => $markup_text
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
			$earlier_comments = _fetch_exsiting_ddcl_cleat_angle_comments($step6_data->id, $step6_data->data_relation);
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
					'ddcl_cleat_angle_step6_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_cleat_angle_step6_form_next_submit'
				)
			);
		} //$step6_data = $form_step6_data->fetchObject()
		else
		{
			$form['step6_field1_fieldset']['step6_field1_question1'] = array(
				'#markup' => $markup_text
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
					'ddcl_cleat_angle_step6_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_cleat_angle_step6_form_next_submit'
				)
			);
		} //$step6_data = $form_step6_data->fetchObject()
	} //$form_step6_data
	return $form;
}
function ddcl_cleat_angle_step6_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step6_field1_fieldset']['step6_field1_question1'] = 'question 1';
	$query = db_select('ddcl_cleat_angle_form_step6_feedback');
	$query->fields('ddcl_cleat_angle_form_step6_feedback');
	$query->condition('uid', $user->uid);
	$form_step6_data = $query->execute()->fetchObject();
	//var_dump($form_step6_data);die;
	if ($form_step6_data)
	{
		$query = "UPDATE ddcl_cleat_angle_form_step6_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_cleat_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step6_data->id
		));
		$form_step6_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step6_field1_fieldset']["step6_comment_count"]; $i++)
		{
			if ($v['step6_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_cleat_angle_form_step6_feedback} (uid, question1, question_ok_not_ok,question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-cleat-angle/form/step5/' . $user->uid;
}
function ddcl_cleat_angle_step6_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step6_field1_fieldset']['step6_field1_question1'] = 'question 1';
	$query = db_select('ddcl_cleat_angle_form_step6_feedback');
	$query->fields('ddcl_cleat_angle_form_step6_feedback');
	$query->condition('uid', $user->uid);
	$form_step6_data = $query->execute()->fetchObject();
	//var_dump($form_step6_data);die;
	if ($form_step6_data)
	{
		$query = "UPDATE ddcl_cleat_angle_form_step6_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_cleat_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step6_data->id
		));
		$form_step6_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step6_field1_fieldset']["step6_comment_count"]; $i++)
		{
			if ($v['step6_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_cleat_angle_form_step6_feedback} (uid, question1, question_ok_not_ok,question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-cleat-angle/form/step7/' . $user->uid;
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
function ddcl_cleat_angle_step7_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('b. Bolt > v. Factored resultant shear force'));
	$markup_text = '
	<b> b. Bolt </b> <br>
	&emsp; <b> v. Factored resultant shear force</b> <br>
	&emsp;&emsp;&emsp; The bolts have to resist a resultant shear force arising due to the factored shear force (vertical, 
		<math xmlns="http://www.w3.org/1998/Math/MathML"><msub><mi>V</mi><mi>V</mi></msub></math>
	) and shear force arising due the bolt eccentricity (horizontal, 
		<math xmlns="http://www.w3.org/1998/Math/MathML"><msub><mi>V</mi><mi>H</mi></msub></math>
	) <br>
	<br>
	
	&emsp;&emsp;&emsp; The resultant shear force, 
			<math xmlns="http://www.w3.org/1998/Math/MathML"><msub><mi>V</mi><mi>R</mi></msub></math>
	on each bolt is given by:
			<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
			  <msub>
				<mi>V</mi>
				<mi>R</mi>
			  </msub>
			  <mo>=</mo>
			  <msqrt>
				<msubsup>
				  <mi>V</mi>
				  <mi>H</mi>
				  <mn>2</mn>
				</msubsup>
				<mo>+</mo>
				<msubsup>
				  <mi>V</mi>
				  <mi>V</mi>
				  <mn>2</mn>
				</msubsup>
			  </msqrt>
			</math>
		
			&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
			Where, <br>				
				<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
				  <msub>
					<mi>V</mi>
					<mi>H</mi>
				  </msub>
				  <mo>=</mo>
				  <mfrac>
					<mrow>
					  <msub>
						<mi>V</mi>
						<mi>x</mi>
					  </msub>
					  <msub>
						<mi>e</mi>
						<mi>x</mi>
					  </msub>
					  <msub>
						<mi>r</mi>
						<mi>i</mi>
					  </msub>
					</mrow>
					<mrow>
					  <mi mathvariant="normal">&#x03A3;<!-- Σ --></mi>
					  <msubsup>
						<mi>r</mi>
						<mi>i</mi>
						<mn>2</mn>
					  </msubsup>
					</mrow>
				  </mfrac>
				</math>				
								
				<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
				  <msub>
					<mi>V</mi>
					<mi>V</mi>
				  </msub>
				  <mo>=</mo>
				  <mfrac>
					<msub>
					  <mi>V</mi>
					  <mi>x</mi>
					</msub>
					<mi>n</mi>
				  </mfrac>
				</math>				
			
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
				Where, <br>
					&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
					  <msub>
						<mi>V</mi>
						<mi>x</mi>
					  </msub>
					</math> - applied shear force 
					<br>
					
					&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
					  <msub>
						<mi>e</mi>
						<mi>x</mi>
					  </msub>
					</math> - eccentricity of bolt group [Refer the front and side view images for eccentricity of beam bolts, 					
						<math xmlns="http://www.w3.org/1998/Math/MathML"><msub>
							<mi>e</mi>
							<mi>xb</mi>
						</msub></math>
					and eccentricity of column bolts, 
						<math xmlns="http://www.w3.org/1998/Math/MathML"><msub>
							<mi>e</mi>
							<mi>xc</mi>
						</msub></math>
					<br>
						
					&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						<msub>
							<mi>r</mi>
							<mi>i</mi>
						</msub>
					</math> - distance of each bolt from C.G. of the bolt group, and <br>
					
					&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						<mi>n</mi>
					</math> - number of bolts 
	
	
	
	<hr>';
	// All the real form fields.
	$form = array();
	$form['step7_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step7-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_cleat_angle_form_step7_feedback');
	$query->fields('ddcl_cleat_angle_form_step7_feedback');
	$query->condition('uid', $user->uid);
	$form_step7_data = $query->execute();
	if ($form_step7_data)
	{
		if ($step7_data = $form_step7_data->fetchObject())
		{
			$form['step7_field1_fieldset']['step7_field1_question1'] = array(
				'#markup' => $markup_text
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
			$earlier_comments = _fetch_exsiting_ddcl_cleat_angle_comments($step7_data->id, $step7_data->data_relation);
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
					'ddcl_cleat_angle_step7_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_cleat_angle_step7_form_next_submit'
				)
			);
		} //$step7_data = $form_step7_data->fetchObject()
		else
		{
			$form['step7_field1_fieldset']['step7_field1_question1'] = array(
				'#markup' => $markup_text
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
					'ddcl_cleat_angle_step7_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_cleat_angle_step7_form_next_submit'
				)
			);
		} //$step7_data = $form_step7_data->fetchObject()
	} //$form_step7_data
	return $form;
}
function ddcl_cleat_angle_step7_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step7_field1_fieldset']['step7_field1_question1'] = 'question 1';
	$query = db_select('ddcl_cleat_angle_form_step7_feedback');
	$query->fields('ddcl_cleat_angle_form_step7_feedback');
	$query->condition('uid', $user->uid);
	$form_step7_data = $query->execute()->fetchObject();
	//var_dump($form_step7_data);die;
	if ($form_step7_data)
	{
		$query = "UPDATE ddcl_cleat_angle_form_step7_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_cleat_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step7_data->id
		));
		$form_step7_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step7_field1_fieldset']["step7_comment_count"]; $i++)
		{
			if ($v['step7_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_cleat_angle_form_step7_feedback} (uid, question1,question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-cleat-angle/form/step6/' . $user->uid;
}
function ddcl_cleat_angle_step7_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step7_field1_fieldset']['step7_field1_question1'] = 'question 1';
	$query = db_select('ddcl_cleat_angle_form_step7_feedback');
	$query->fields('ddcl_cleat_angle_form_step7_feedback');
	$query->condition('uid', $user->uid);
	$form_step7_data = $query->execute()->fetchObject();
	//var_dump($form_step7_data);die;
	if ($form_step7_data)
	{
		$query = "UPDATE ddcl_cleat_angle_form_step7_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_cleat_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step7_data->id
		));
		$form_step7_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step7_field1_fieldset']["step7_comment_count"]; $i++)
		{
			if ($v['step7_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_cleat_angle_form_step7_feedback} (uid, question1,question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-cleat-angle/form/step8/' . $user->uid;
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
function ddcl_cleat_angle_step8_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('b. Bolt > vi. Bolt capacity check'));
	$markup_text = '
	<b> b. Bolt </b> <br>
	&emsp; <b> vi. Bolt capacity check</b><br>
	&emsp;&emsp;&emsp; The design strength of bolt, 
		<math xmlns="http://www.w3.org/1998/Math/MathML">
		  <msub>
			<mi>V</mi>
			<mi>db</mi>
		  </msub>
		</math>
	should be greater than the resultant shear force, 
		<math xmlns="http://www.w3.org/1998/Math/MathML">
		  <msub>
			<mi>V</mi>
			<mi>R</mi>
		  </msub>
		</math>
	on each bolt 

	<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
	  <msub>
		<mi>V</mi>
		<mrow class="MJX-TeXAtom-ORD">
		  <mi>d</mi>
		  <mi>b</mi>
		</mrow>
	  </msub>
	  <mo>&gt;</mo>
	  <msub>
		<mi>V</mi>
		<mi>R</mi>
	  </msub>
	</math>
	
	<hr>';
	// All the real form fields.
	$form = array();
	$form['step8_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step8-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_cleat_angle_form_step8_feedback');
	$query->fields('ddcl_cleat_angle_form_step8_feedback');
	$query->condition('uid', $user->uid);
	$form_step8_data = $query->execute();
	if ($form_step8_data)
	{
		if ($step8_data = $form_step8_data->fetchObject())
		{
			$form['step8_field1_fieldset']['step8_field1_question1'] = array(
				'#markup' => $markup_text
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
			$earlier_comments = _fetch_exsiting_ddcl_cleat_angle_comments($step8_data->id, $step8_data->data_relation);
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
					'ddcl_cleat_angle_step8_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_cleat_angle_step8_form_next_submit'
				)
			);
		} //$step8_data = $form_step8_data->fetchObject()
		else
		{
			$form['step8_field1_fieldset']['step8_field1_question1'] = array(
				'#markup' => $markup_text
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
					'ddcl_cleat_angle_step8_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_cleat_angle_step8_form_next_submit'
				)
			);
		} //$step8_data = $form_step8_data->fetchObject()
	} //$form_step8_data
	return $form;
}
function ddcl_cleat_angle_step8_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step8_field1_fieldset']['step8_field1_question1'] = 'question 1';
	$query = db_select('ddcl_cleat_angle_form_step8_feedback');
	$query->fields('ddcl_cleat_angle_form_step8_feedback');
	$query->condition('uid', $user->uid);
	$form_step8_data = $query->execute()->fetchObject();
	//var_dump($form_step8_data);die;
	if ($form_step8_data)
	{
		$query = "UPDATE ddcl_cleat_angle_form_step8_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_cleat_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step8_data->id
		));
		$form_step8_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step8_field1_fieldset']["step8_comment_count"]; $i++)
		{
			if ($v['step8_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_cleat_angle_form_step8_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-cleat-angle/form/step7/' . $user->uid;
}
function ddcl_cleat_angle_step8_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step8_field1_fieldset']['step8_field1_question1'] = 'question 1';
	$query = db_select('ddcl_cleat_angle_form_step8_feedback');
	$query->fields('ddcl_cleat_angle_form_step8_feedback');
	$query->condition('uid', $user->uid);
	$form_step8_data = $query->execute()->fetchObject();
	//var_dump($form_step8_data);die;
	if ($form_step8_data)
	{
		$query = "UPDATE ddcl_cleat_angle_form_step8_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_cleat_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step8_data->id
		));
		$form_step8_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step8_field1_fieldset']["step8_comment_count"]; $i++)
		{
			if ($v['step8_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_cleat_angle_form_step8_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-cleat-angle/form/step9/' . $user->uid;
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
function ddcl_cleat_angle_step9_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('c. Cleat angle > i. Height'));
	$markup_text = '
	<b> c. Cleat angle </b> <br>
	&emsp; <b> i. Minimum height</b> [INSDAG detailing manual p-733, Steel Designers Manual -6th Edition (2003)] <br>
	&emsp;&emsp;&emsp; Minimum height of the cleat angle shall be calculated as:
			<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
			  <msub>
				<mi>H</mi>
				<mrow class="MJX-TeXAtom-ORD">
				  <mi>m</mi>
				  <mi>i</mi>
				  <mi>n</mi>
				</mrow>
			  </msub>
			  <mo>=</mo>
			  <mn>0.6</mn>
			  <mi>d</mi>
			</math>
			
			&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
			Where, <br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
				<math xmlns="http://www.w3.org/1998/Math/MathML">
				  <msub>
					<mi>H</mi>
					<mi>min</mi>
				  </msub>
				</math> - Minimum height of cleat angle
				<br>
				
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
				<math xmlns="http://www.w3.org/1998/Math/MathML">
				  <mi>d</mi>
				</math> - beam depth
				<br>
				<br>
				
	&emsp; <b> ii. Maximum height</b> <font color=red>Jnote - Add reference</font><br>
	&emsp;&emsp;&emsp; Maximum height of the cleat angle shall be calculated as:
					
					<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
					  <msub>
						<mi>H</mi>
						<mrow class="MJX-TeXAtom-ORD">
						  <mi>m</mi>
						  <mi>a</mi>
						  <mi>x</mi>
						</mrow>
					  </msub>
					  <mo>=</mo>
					  <mi>d</mi>
					  <mo>&#x2212;</mo>
					  <mn>2</mn>
					  <msub>
						<mi>t</mi>
						<mrow class="MJX-TeXAtom-ORD">
						  <mi>f</mi>
						  <mi>b</mi>
						</mrow>
					  </msub>
					  <mo>&#x2212;</mo>
					  <mn>2</mn>
					  <msub>
						<mi>r</mi>
						<mrow class="MJX-TeXAtom-ORD">
						  <mn>1</mn>
						  <mi>b</mi>
						</mrow>
					  </msub>
					  <mo>&#x2212;</mo>
					  <mn>2</mn>
					  <mo>&#x2217;</mo>
					  <mn>5</mn>
					</math>
					
					&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					Where, <br>
						&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
						<math xmlns="http://www.w3.org/1998/Math/MathML">
						  <msub>
							<mi>H</mi>
							<mi>max</mi>
						  </msub>
						</math> - Maximum height of cleat angle
						<br>
						
						&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
						<math xmlns="http://www.w3.org/1998/Math/MathML">
						  <mi>d</mi>							
						</math> - beam depth						
						<br>
						
						&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
						<math xmlns="http://www.w3.org/1998/Math/MathML">
						  <msub>
							<mi>t</mi>
							<mi>fb</mi>
						  </msub>
						</math> - flange thickness of beam
						<br>
						
						&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
						<math xmlns="http://www.w3.org/1998/Math/MathML">
						  <msub>
							<mi>r</mi>
							<mi>1b</mi>
						  </msub>
						</math> - root radius of beam
						<br>
						<br>
					
					Note: Assumed clearance of 5 mm on each side.

	
	
	<hr>';
	// All the real form fields.
	$form = array();
	$form['step9_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step9-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_cleat_angle_form_step9_feedback');
	$query->fields('ddcl_cleat_angle_form_step9_feedback');
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
			$earlier_comments = _fetch_exsiting_ddcl_cleat_angle_comments($step9_data->id, $step9_data->data_relation);
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
					'ddcl_cleat_angle_step9_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_cleat_angle_step9_form_next_submit'
				)
			);
		} //$step9_data = $form_step9_data->fetchObject()
		else
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
					'ddcl_cleat_angle_step9_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_cleat_angle_step9_form_next_submit'
				)
			);
		} //$step9_data = $form_step9_data->fetchObject()
	} //$form_step9_data
	return $form;
}
function ddcl_cleat_angle_step9_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step9_field1_fieldset']['step9_field1_question1'] = 'question 1';
	$query = db_select('ddcl_cleat_angle_form_step9_feedback');
	$query->fields('ddcl_cleat_angle_form_step9_feedback');
	$query->condition('uid', $user->uid);
	$form_step9_data = $query->execute()->fetchObject();
	//var_dump($form_step9_data);die;
	if ($form_step9_data)
	{
		$query = "UPDATE ddcl_cleat_angle_form_step9_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_cleat_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step9_data->id
		));
		$form_step9_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step9_field1_fieldset']["step9_comment_count"]; $i++)
		{
			if ($v['step9_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_cleat_angle_form_step9_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-cleat-angle/form/step8/' . $user->uid;
}
function ddcl_cleat_angle_step9_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step9_field1_fieldset']['step9_field1_question1'] = 'question 1';
	$query = db_select('ddcl_cleat_angle_form_step9_feedback');
	$query->fields('ddcl_cleat_angle_form_step9_feedback');
	$query->condition('uid', $user->uid);
	$form_step9_data = $query->execute()->fetchObject();
	//var_dump($form_step9_data);die;
	if ($form_step9_data)
	{
		$query = "UPDATE ddcl_cleat_angle_form_step9_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_cleat_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step9_data->id
		));
		$form_step9_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step9_field1_fieldset']["step9_comment_count"]; $i++)
		{
			if ($v['step9_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_cleat_angle_form_step9_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-cleat-angle/form/step10/' . $user->uid;
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
function ddcl_cleat_angle_step10_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('c. Cleat angle > ii. Angle leg'));
	$markup_text = '
	<b> c. Cleat angle </b> <br>
	&emsp; <b> ii. Maximum cleat angle leg</b><br>
	&emsp;&emsp;&emsp; Maximum cleat angle leg size for beam - column web connectivity:
				<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
				  <mi>B</mi>
				  <mo>=</mo>
				  <mfrac>
					<mrow>
					  <msub>
						<mi>d</mi>
						<mrow class="MJX-TeXAtom-ORD">
						  <mi>c</mi>
						  <mi>o</mi>
						  <mi>l</mi>
						</mrow>
					  </msub>
					  <mo>&#x2212;</mo>
					  <mn>2</mn>
					  <msub>
						<mi>t</mi>
						<mrow class="MJX-TeXAtom-ORD">
						  <mi>f</mi>
						  <mi>c</mi>
						</mrow>
					  </msub>
					  <mo>&#x2212;</mo>
					  <msub>
						<mi>t</mi>
						<mrow class="MJX-TeXAtom-ORD">
						  <mi>w</mi>
						  <mi>b</mi>
						</mrow>
					  </msub>
					  <mo>&#x2212;</mo>
					  <mn>2*5</mn>
					</mrow>
					<mn>2</mn>
				  </mfrac>
				</math>
		
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
				Where, <br>
					&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						<mi>B</mi>						
					</math> - maximum cleat angle leg
					<br> 
					
					&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
					  <msub>
						<mi>d</mi>
						<mi>col</mi>
					  </msub>
					</math> - depth of column
					<br> 
					
					&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
					  <msub>
						<mi>t</mi>
						<mi>fc</mi>
					  </msub>
					</math> - thickness of flange of column
					<br> 
					
					&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
					  <msub>
						<mi>d</mi>
						<mi>col</mi>
					  </msub>
					</math> - depth of column
					<br> 
					
					&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
					  <msub>
						<mi>t</mi>
						<mi>wb</mi>
					  </msub>
					</math> - thickness of web of beam
					<br> 
					
					&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">					  
						<mi>2*5</mi>
					</math> mm - twice the minimum clearance from root radius

	
	<hr>';
	// All the real form fields.
	$form = array();
	$form['step10_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step10-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_cleat_angle_form_step10_feedback');
	$query->fields('ddcl_cleat_angle_form_step10_feedback');
	$query->condition('uid', $user->uid);
	$form_step10_data = $query->execute();
	if ($form_step10_data)
	{
		if ($step10_data = $form_step10_data->fetchObject())
		{
			$form['step10_field1_fieldset']['step10_field1_question1'] = array(
				'#markup' => $markup_text
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
			$earlier_comments = _fetch_exsiting_ddcl_cleat_angle_comments($step10_data->id, $step10_data->data_relation);
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
					'ddcl_cleat_angle_step10_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_cleat_angle_step10_form_next_submit'
				)
			);
		} //$step10_data = $form_step10_data->fetchObject()
		else
		{
			$form['step10_field1_fieldset']['step10_field1_question1'] = array(
				'#markup' => $markup_text
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
					'ddcl_cleat_angle_step10_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_cleat_angle_step10_form_next_submit'
				)
			);
		} //$step10_data = $form_step10_data->fetchObject()
	} //$form_step10_data
	return $form;
}
function ddcl_cleat_angle_step10_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step10_field1_fieldset']['step10_field1_question1'] = 'question 1';
	$query = db_select('ddcl_cleat_angle_form_step10_feedback');
	$query->fields('ddcl_cleat_angle_form_step10_feedback');
	$query->condition('uid', $user->uid);
	$form_step10_data = $query->execute()->fetchObject();
	//var_dump($form_step10_data);die;
	if ($form_step10_data)
	{
		$query = "UPDATE ddcl_cleat_angle_form_step10_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_cleat_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step10_data->id
		));
		$form_step10_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step10_field1_fieldset']["step10_comment_count"]; $i++)
		{
			if ($v['step10_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_cleat_angle_form_step10_feedback} (uid, question1,  question_ok_not_ok,question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-cleat-angle/form/step9/' . $user->uid;
}
function ddcl_cleat_angle_step10_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step10_field1_fieldset']['step10_field1_question1'] = 'question 1';
	$query = db_select('ddcl_cleat_angle_form_step10_feedback');
	$query->fields('ddcl_cleat_angle_form_step10_feedback');
	$query->condition('uid', $user->uid);
	$form_step10_data = $query->execute()->fetchObject();
	//var_dump($form_step10_data);die;
	if ($form_step10_data)
	{
		$query = "UPDATE ddcl_cleat_angle_form_step10_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_cleat_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step10_data->id
		));
		$form_step10_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step10_field1_fieldset']["step10_comment_count"]; $i++)
		{
			if ($v['step10_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_cleat_angle_form_step10_feedback} (uid, question1,  question_ok_not_ok,question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-cleat-angle/form/step11/' . $user->uid;
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
function ddcl_cleat_angle_step11_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('c. Cleat Angle > iii. Angle thickness'));
	$markup_text = '
	<b> c. Cleat angle </b> <br>
	&emsp; <b> iii. Angle thickness</b>	
			(<math xmlns="http://www.w3.org/1998/Math/MathML">
			  <msub>
				<mi>t</mi>
				<mi>c</mi>
			  </msub>
			</math>)
	<br>
	
	&emsp;&emsp;&emsp; The thickness of the cleat angle shall be determined based on the following rules
	<br>
	&emsp;&emsp;&emsp;
	[Reference: Steel Designers Manual - p733 - 6th Edition (2003)] 
	<br>
	<br>
	
	&emsp;&emsp;&emsp;&emsp;&emsp;&emsp; <b> a) For beam depth less than 450 mm</b>	<br>
	&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp; 
							<math xmlns="http://www.w3.org/1998/Math/MathML">
							  <msub>
								<mi>t</mi>
								<mi>c</mi>
							  </msub>
							  <mo>=</mo>
							  <mn>8</mn>
							</math> mm
							<br>
	
	&emsp;&emsp;&emsp;&emsp;&emsp;&emsp; <b> b) For beam depth greater than 450 mm</b>	<br>
	&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp; 
							<math xmlns="http://www.w3.org/1998/Math/MathML">
							  <msub>
								<mi>t</mi>
								<mi>c</mi>
							  </msub>
							  <mo>=</mo>
							  <mn>12</mn>
							</math> mm
							<br>
	
	
	
	<hr>';
	// All the real form fields.
	$form = array();
	$form['step11_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step11-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_cleat_angle_form_step11_feedback');
	$query->fields('ddcl_cleat_angle_form_step11_feedback');
	$query->condition('uid', $user->uid);
	$form_step11_data = $query->execute();
	if ($form_step11_data)
	{
		if ($step11_data = $form_step11_data->fetchObject())
		{
			$form['step11_field1_fieldset']['step11_field1_question1'] = array(
				'#markup' => $markup_text
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
			$earlier_comments = _fetch_exsiting_ddcl_cleat_angle_comments($step11_data->id, $step11_data->data_relation);
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
					'ddcl_cleat_angle_step11_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_cleat_angle_step11_form_next_submit'
				)
			);
		} //$step11_data = $form_step11_data->fetchObject()
		else
		{
			$form['step11_field1_fieldset']['step11_field1_question1'] = array(
				'#markup' => $markup_text
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
					'ddcl_cleat_angle_step11_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_cleat_angle_step11_form_next_submit'
				)
			);
		} //$step11_data = $form_step11_data->fetchObject()
	} //$form_step11_data
	return $form;
}
function ddcl_cleat_angle_step11_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step11_field1_fieldset']['step11_field1_question1'] = 'question 1';
	$query = db_select('ddcl_cleat_angle_form_step11_feedback');
	$query->fields('ddcl_cleat_angle_form_step11_feedback');
	$query->condition('uid', $user->uid);
	$form_step11_data = $query->execute()->fetchObject();
	//var_dump($form_step11_data);die;
	if ($form_step11_data)
	{
		$query = "UPDATE ddcl_cleat_angle_form_step11_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_cleat_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step11_data->id
		));
		$form_step11_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step11_field1_fieldset']["step11_comment_count"]; $i++)
		{
			if ($v['step11_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_cleat_angle_form_step11_feedback} (uid, question1,question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-cleat-angle/form/step10/' . $user->uid;
}
function ddcl_cleat_angle_step11_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step11_field1_fieldset']['step11_field1_question1'] = 'question 1';
	$query = db_select('ddcl_cleat_angle_form_step11_feedback');
	$query->fields('ddcl_cleat_angle_form_step11_feedback');
	$query->condition('uid', $user->uid);
	$form_step11_data = $query->execute()->fetchObject();
	//var_dump($form_step11_data);die;
	if ($form_step11_data)
	{
		$query = "UPDATE ddcl_cleat_angle_form_step11_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_cleat_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step11_data->id
		));
		$form_step11_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step11_field1_fieldset']["step11_comment_count"]; $i++)
		{
			if ($v['step11_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_cleat_angle_form_step11_feedback} (uid, question1,question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-cleat-angle/form/step12/' . $user->uid;
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
function ddcl_cleat_angle_step12_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('c. Cleat angle > iv. Moment capacity check'));
	$markup_text = '
	<b> c. Cleat angle </b> <br>
	&emsp; <b> iv. Moment capacity check</b><br>
	&emsp;&emsp;&emsp; 1) The factored design moment, 
		<math xmlns="http://www.w3.org/1998/Math/MathML"><mi>M</mi></math>
	shall satisfy
			<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
			  <mi>M</mi>
			  <mo>&lt;</mo>
			  <msub>
				<mi>M</mi>
				<mi>d</mi>
			  </msub>
			</math>
			&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
			Where, <br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
				<math xmlns="http://www.w3.org/1998/Math/MathML">
				  <msub>
					<mi>M</mi>
					<mi>d</mi>
				  </msub>
				</math> - design bending strength of the the cleat angle leg, calculated as given in [Cl 8.2.1.2]
				<br>
				<br>
	
	&emsp;&emsp;&emsp; <b>2) Moment due to shear force, </b>
		<math xmlns="http://www.w3.org/1998/Math/MathML"><mi>M</mi></math>
	shall satisfy <br>
			<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
			  <mi>M</mi>
			  <mo>=</mo>
			  <mn>0.5</mn>
			  <msub>
				<mi>V</mi>
				<mrow class="MJX-TeXAtom-ORD">
				  <mi>s</mi>
				  <mi>f</mi>
				</mrow>
			  </msub>
			  <msub>
				<mi>e</mi>
				<mrow class="MJX-TeXAtom-ORD">
				  <msub>
					<mi>x</mi>
					<mi>b</mi>
				  </msub>
				</mrow>
			  </msub>
			</math>
			
			&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
			Where, <br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
				<math xmlns="http://www.w3.org/1998/Math/MathML">
				  <msub>
					<mi>V</mi>
					<mi>sf</mi>
				  </msub>
				</math> - shear force
				<br>
				
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
				<math xmlns="http://www.w3.org/1998/Math/MathML">
				  <msub>
					<mi>e</mi>
					<mi>
						<msub>
							<mi>x</mi>
							<mi>b</mi>
						</msub>							
					</mi>
				  </msub>
					</mi>
				  </msub>
				</math> &emsp; - eccentricity
				<br>
				<br>
		
	&emsp;&emsp;&emsp;  <b> 3) Design Bending strength </b>[Cl 8.2.1.2]
		<math xmlns="http://www.w3.org/1998/Math/MathML"><msub><mi>M</mi><mi>d</mi></msub></math>
	<br>
	&emsp;&emsp;&emsp;&emsp;&emsp;&emsp; When the factored design shear force does not exceed 0
		<math xmlns="http://www.w3.org/1998/Math/MathML"><msub><mi>0.6V</mi><mi>d</mi></msub></math>
	, where
		<math xmlns="http://www.w3.org/1998/Math/MathML"><msub><mi>0.6V</mi><mi>d</mi></msub></math>
	is the design shear strength of the cross-section, the design bending strength, 
		<math xmlns="http://www.w3.org/1998/Math/MathML"><msub><mi>M</mi><mi>d</mi></msub></math>
	shall be taken as:
				<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
				  <msub>
					<mi>M</mi>
					<mi>d</mi>
				  </msub>
				  <mo>=</mo>
				  <mfrac>
					<mrow>
					  <mn>1.2</mn>
					  <msub>
						<mi>t</mi>
						<mi>a</mi>
					  </msub>
					  <msubsup>
						<mi>h</mi>
						<mi>a</mi>
						<mn>2</mn>
					  </msubsup>
					  <msub>
						<mi>f</mi>
						<mi>y</mi>
					  </msub>
					</mrow>
					<mrow>
					  <mn>6</mn>
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
			
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
				Where, <br>
					&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
					  <msub>
						<mi>t</mi>
						<mi>a</mi>
					  </msub>
					</math> - thickness of cleat angle
					<br>
					
					&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
					  <msub>
						<mi>h</mi>
						<mi>a</mi>
					  </msub>
					</math> - height of cleat angle
					<br>
					
					&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
					  <msub>
						<mi>f</mi>
						<mi>y</mi>
					  </msub>
					</math> - yield stress of the cleat angle material
					<br>
					
					&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
					  <msub>
						<mi>&#x03B3;<!-- γ --></mi>
						<mrow class="MJX-TeXAtom-ORD">
						  <mi>m</mi>
						  <mn>0</mn>
						</mrow>
					  </msub>
					</math> -  partial safety factor (= 1.1 )
					<br>
			
				
	
	<hr>';
	// All the real form fields.
	$form = array();
	$form['step12_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step12-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_cleat_angle_form_step12_feedback');
	$query->fields('ddcl_cleat_angle_form_step12_feedback');
	$query->condition('uid', $user->uid);
	$form_step12_data = $query->execute();
	if ($form_step12_data)
	{
		if ($step12_data = $form_step12_data->fetchObject())
		{
			$form['step12_field1_fieldset']['step12_field1_question1'] = array(
				'#markup' => $markup_text
			);
			$form['step12_field1_fieldset']['step12_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				'#default_value' => $step12_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step12_field1_fieldset']['step12_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				'#default_value' => $step12_data->question_not_ok_comment,
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
			$earlier_comments = _fetch_exsiting_ddcl_cleat_angle_comments($step12_data->id, $step12_data->data_relation);
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
					'ddcl_cleat_angle_step12_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_cleat_angle_step12_form_next_submit'
				)
			);
		} //$step12_data = $form_step12_data->fetchObject()
		else
		{
			$form['step12_field1_fieldset']['step12_field1_question1'] = array(
				'#markup' => $markup_text
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
					'ddcl_cleat_angle_step12_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_cleat_angle_step12_form_next_submit'
				)
			);
		} //$step12_data = $form_step12_data->fetchObject()
	} //$form_step12_data
	return $form;
}
function ddcl_cleat_angle_step12_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step12_field1_fieldset']['step12_field1_question1'] = 'question 1';
	$query = db_select('ddcl_cleat_angle_form_step12_feedback');
	$query->fields('ddcl_cleat_angle_form_step12_feedback');
	$query->condition('uid', $user->uid);
	$form_step12_data = $query->execute()->fetchObject();
	//var_dump($form_step12_data);die;
	if ($form_step12_data)
	{
		$query = "UPDATE ddcl_cleat_angle_form_step12_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_cleat_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step12_data->id
		));
		$form_step12_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step12_field1_fieldset']["step12_comment_count"]; $i++)
		{
			if ($v['step12_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_cleat_angle_form_step12_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-cleat-angle/form/step11/' . $user->uid;
}
function ddcl_cleat_angle_step12_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step12_field1_fieldset']['step12_field1_question1'] = 'question 1';
	$query = db_select('ddcl_cleat_angle_form_step12_feedback');
	$query->fields('ddcl_cleat_angle_form_step12_feedback');
	$query->condition('uid', $user->uid);
	$form_step12_data = $query->execute()->fetchObject();
	//var_dump($form_step12_data);die;
	if ($form_step12_data)
	{
		$query = "UPDATE ddcl_cleat_angle_form_step12_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_cleat_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step12_data->id
		));
		$form_step12_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step12_field1_fieldset']["step12_comment_count"]; $i++)
		{
			if ($v['step12_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_cleat_angle_form_step12_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-cleat-angle/form/step13/' . $user->uid;
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
function ddcl_cleat_angle_step13_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('specimen title'));
	$img_path = '';
	$markup_text = 'specimen text<hr>';
	// All the real form fields.
	$form = array();
	$form['step13_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step13-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_cleat_angle_form_step13_feedback');
	$query->fields('ddcl_cleat_angle_form_step13_feedback');
	$query->condition('uid', $user->uid);
	$form_step13_data = $query->execute();
	if ($form_step13_data)
	{
		if ($step13_data = $form_step13_data->fetchObject())
		{
			$form['step13_field1_fieldset']['step13_field1_question1'] = array(
				'#markup' => $markup_text
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
			$earlier_comments = _fetch_exsiting_ddcl_cleat_angle_comments($step13_data->id, $step13_data->data_relation);
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
					'ddcl_cleat_angle_step13_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_cleat_angle_step13_form_next_submit'
				)
			);
		} //$step13_data = $form_step13_data->fetchObject()
		else
		{
			$form['step13_field1_fieldset']['step13_field1_question1'] = array(
				'#markup' => $markup_text
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
					'ddcl_cleat_angle_step13_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_cleat_angle_step13_form_next_submit'
				)
			);
		} //$step13_data = $form_step13_data->fetchObject()
	} //$form_step13_data
	return $form;
}
function ddcl_cleat_angle_step13_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step13_field1_fieldset']['step13_field1_question1'] = 'question 1';
	$query = "SELECT * FROM ddcl_cleat_angle_form_step13_feedback WHERE uid=:uid";
	$args = array(
		":uid" => $user->uid
	);
	$form_step7_data = db_query($query, $args);
	$form_step7_data_count = $form_step7_data->rowCount();
	$form_step7_data = $form_step7_data->fetchObject();
	if ($form_step7_data_count != 0)
	{
		$query = "UPDATE ddcl_cleat_angle_form_step13_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$queryfinish = "UPDATE ddcl_cleat_angle_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_cleat_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step7_data->id
		));
		$form_step7_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step13_field1_fieldset']["step13_comment_count"]; $i++)
		{
			if ($v['step13_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_cleat_angle_form_step13_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1,  :question_ok_not_ok, :question_not_ok_comment,:data_relation,:inserted_on)";
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
		$queryfinish = "UPDATE ddcl_cleat_angle_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
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
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-cleat-angle/form/step12/' . $user->uid;
}
function ddcl_cleat_angle_step13_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step13_field1_fieldset']['step13_field1_question1'] = 'question 1';
	$query = "SELECT * FROM ddcl_cleat_angle_form_step13_feedback WHERE uid=:uid";
	$args = array(
		":uid" => $user->uid
	);
	$form_step7_data = db_query($query, $args);
	$form_step7_data_count = $form_step7_data->rowCount();
	$form_step7_data = $form_step7_data->fetchObject();
	if ($form_step7_data_count != 0)
	{
		$query = "UPDATE ddcl_cleat_angle_form_step13_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$queryfinish = "UPDATE ddcl_cleat_angle_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_cleat_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step7_data->id
		));
		$form_step7_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step13_field1_fieldset']["step13_comment_count"]; $i++)
		{
			if ($v['step13_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_cleat_angle_form_step13_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok,:question_not_ok_comment, :data_relation,:inserted_on)";
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
		$queryfinish = "UPDATE ddcl_cleat_angle_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
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
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
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
	//$form_state['redirect'] = 'ddcl-cleat-angle/success';
	$form_state['redirect'] = 'ddcl-cleat-angle/form/step14/' . $user->uid;
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
/***************************************** step 13 form ends ****************************/

/************************ Step 14 form starts *********************/
function ddcl_cleat_angle_step14_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('specimen title'));
	$img_path = '';
	$markup_text = 'specimen text<hr>';
	// All the real form fields.
	$form = array();
	$form['step14_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step14-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_cleat_angle_form_step14_feedback');
	$query->fields('ddcl_cleat_angle_form_step14_feedback');
	$query->condition('uid', $user->uid);
	$form_step14_data = $query->execute();
	if ($form_step14_data)
	{
		if ($step14_data = $form_step14_data->fetchObject())
		{
			$form['step14_field1_fieldset']['step14_field1_question1'] = array(
				'#markup' => $markup_text
			);
			$form['step14_field1_fieldset']['step14_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				'#default_value' => $step14_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step14_field1_fieldset']['step14_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				'#default_value' => $step14_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step14_field1_fieldset\\[step14_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			$earlier_comments = _fetch_exsiting_ddcl_cleat_angle_comments($step14_data->id, $step14_data->data_relation);
			$icom = 1;
			$form['step14_field1_fieldset']['exsiting_comments_title'] = array(
				'#markup' => 'Earlier comment<br>'
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step14_field1_fieldset']['exsiting_comments' . $icom] = array(
					'#markup' => $rowcomment
				);
				$icom++;
			} //$earlier_comments as $rowcomment
			if (empty($form_state['step14_num_comment']))
			{
				$form_state['step14_num_comment'] = 1;
			} //empty($form_state['step14_num_comment'])
			$step14_temp1 = 0;
			for ($step14_i = 0; $step14_i < $form_state['step14_num_comment']; $step14_i++)
			{
				$step14_temp1 = $step14_i;
				$form['step14_field1_fieldset'][$step14_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step14_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step14_field1_fieldset']["step14_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step14_temp1
			);
			$form['step14_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step14_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step14_comment_add_more_callback',
					'wrapper' => 'step14-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step14_num_comment'] > 1)
			{
				$form['step14_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step14_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step14_comment_add_more_callback',
						'wrapper' => 'step14-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step14_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step14_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step14_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step14_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_cleat_angle_step14_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_cleat_angle_step14_form_next_submit'
				)
			);
		} //$step14_data = $form_step14_data->fetchObject()
		else
		{
			$form['step14_field1_fieldset']['step14_field1_question1'] = array(
				'#markup' => $markup_text
			);
			$form['step14_field1_fieldset']['step14_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				//'#default_value' => $step14_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step14_field1_fieldset']['step14_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				//'#default_value' => $step14_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step14_field1_fieldset\\[step14_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			if (empty($form_state['step14_num_comment']))
			{
				$form_state['step14_num_comment'] = 1;
			} //empty($form_state['step14_num_comment'])
			$step14_temp1 = 0;
			for ($step14_i = 0; $step14_i < $form_state['step14_num_comment']; $step14_i++)
			{
				$step14_temp1 = $step14_i;
				$form['step14_field1_fieldset'][$step14_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step14_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step14_field1_fieldset']["step14_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step14_temp1
			);
			$form['step14_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step14_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step14_comment_add_more_callback',
					'wrapper' => 'step14-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step14_num_comment'] > 1)
			{
				$form['step14_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step14_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step14_comment_add_more_callback',
						'wrapper' => 'step14-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step14_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step14_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step14_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step14_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_cleat_angle_step14_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_cleat_angle_step14_form_next_submit'
				)
			);
		} //$step14_data = $form_step14_data->fetchObject()
	} //$form_step14_data
	return $form;
}
function ddcl_cleat_angle_step14_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step14_field1_fieldset']['step14_field1_question1'] = 'question 1';
	$query = "SELECT * FROM ddcl_cleat_angle_form_step14_feedback WHERE uid=:uid";
	$args = array(
		":uid" => $user->uid
	);
	$form_step7_data = db_query($query, $args);
	$form_step7_data_count = $form_step7_data->rowCount();
	$form_step7_data = $form_step7_data->fetchObject();
	if ($form_step7_data_count != 0)
	{
		$query = "UPDATE ddcl_cleat_angle_form_step14_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step14_field1_fieldset']['step14_field1_question1'],
			":question_ok_not_ok" => $v['step14_field1_fieldset']['step14_field1_question_ok'],
			":question_not_ok_comment" => $v['step14_field1_fieldset']['step14_field1_question_not_ok_comment'],
			":data_relation" => "step14"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_cleat_angle_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_cleat_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step7_data->id
		));
		$form_step7_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step14_field1_fieldset']["step14_comment_count"]; $i++)
		{
			if ($v['step14_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step7_data->id,
					":number" => $i + $form_step7_comment_data->max + 1,
					":comment" => $v['step14_field1_fieldset'][$i]["comment"],
					":data_relation" => "step14",
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
			} //$v['step14_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step14_field1_fieldset']["step14_comment_count"]; $i++
	} //$form_step7_data
	else
	{
		$query = "INSERT INTO {ddcl_cleat_angle_form_step14_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1,  :question_ok_not_ok, :question_not_ok_comment,:data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step14_field1_fieldset']['step14_field1_question1'],
			":question_ok_not_ok" => $v['step14_field1_fieldset']['step14_field1_question_ok'],
			":question_not_ok_comment" => $v['step14_field1_fieldset']['step14_field1_question_not_ok_comment'],
			":data_relation" => "step14",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_cleat_angle_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
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
		for ($i = 0; $i <= $v['step14_field1_fieldset']["step14_comment_count"]; $i++)
		{
			if ($v['step14_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step14_field1_fieldset'][$i]["comment"],
					":data_relation" => "step14",
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
			} //$v['step14_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step14_field1_fieldset']["step14_comment_count"]; $i++
	}
	$form_state['redirect'] = 'ddcl-cleat-angle/form/step13/' . $user->uid;
}
function ddcl_cleat_angle_step14_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step14_field1_fieldset']['step14_field1_question1'] = 'question 1';
	$query = "SELECT * FROM ddcl_cleat_angle_form_step14_feedback WHERE uid=:uid";
	$args = array(
		":uid" => $user->uid
	);
	$form_step7_data = db_query($query, $args);
	$form_step7_data_count = $form_step7_data->rowCount();
	$form_step7_data = $form_step7_data->fetchObject();
	if ($form_step7_data_count != 0)
	{
		$query = "UPDATE ddcl_cleat_angle_form_step14_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step14_field1_fieldset']['step14_field1_question1'],
			":question_ok_not_ok" => $v['step14_field1_fieldset']['step14_field1_question_ok'],
			":question_not_ok_comment" => $v['step14_field1_fieldset']['step14_field1_question_not_ok_comment'],
			":data_relation" => "step14"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_cleat_angle_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_cleat_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step7_data->id
		));
		$form_step7_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step14_field1_fieldset']["step14_comment_count"]; $i++)
		{
			if ($v['step14_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step7_data->id,
					":number" => $i + $form_step7_comment_data->max + 1,
					":comment" => $v['step14_field1_fieldset'][$i]["comment"],
					":data_relation" => "step14",
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
			} //$v['step14_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step14_field1_fieldset']["step14_comment_count"]; $i++
	} //$form_step7_data
	else
	{
		$query = "INSERT INTO {ddcl_cleat_angle_form_step14_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok,:question_not_ok_comment, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step14_field1_fieldset']['step14_field1_question1'],
			":question_ok_not_ok" => $v['step14_field1_fieldset']['step14_field1_question_ok'],
			":question_not_ok_comment" => $v['step14_field1_fieldset']['step14_field1_question_not_ok_comment'],
			":data_relation" => "step14",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_cleat_angle_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
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
		for ($i = 0; $i <= $v['step14_field1_fieldset']["step14_comment_count"]; $i++)
		{
			if ($v['step14_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step14_field1_fieldset'][$i]["comment"],
					":data_relation" => "step14",
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
			} //$v['step14_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step14_field1_fieldset']["step14_comment_count"]; $i++
	}
	//$form_state['redirect'] = 'ddcl-cleat-angle/success';
	$form_state['redirect'] = 'ddcl-cleat-angle/form/step15/' . $user->uid;
}
/***************************************/
function step14_comment_add_more_add_one($form, &$form_state)
{
	$form_state['step14_num_comment']++;
	$form_state['rebuild'] = TRUE;
	//$form_state['no_redirect'] = TRUE;
}
function step14_comment_add_more_remove_one($form, &$form_state)
{
	if ($form_state['step14_num_comment'] > 1)
	{
		$form_state['step14_num_comment']--;
	} //$form_state['step14_num_comment'] > 1
	$form_state['rebuild'] = TRUE;
}
function step14_comment_add_more_callback($form, &$form_state)
{
	return $form['step14_field1_fieldset'];
}
/***************************************/
/***************************************** step 14 form ends ****************************/
/************************ Step 15 form starts *********************/
function ddcl_cleat_angle_step15_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('specimen title'));
	$img_path = '';
	$markup_text = 'specimen text<hr>';
	// All the real form fields.
	$form = array();
	$form['step15_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step15-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_cleat_angle_form_step15_feedback');
	$query->fields('ddcl_cleat_angle_form_step15_feedback');
	$query->condition('uid', $user->uid);
	$form_step15_data = $query->execute();
	if ($form_step15_data)
	{
		if ($step15_data = $form_step15_data->fetchObject())
		{
			$form['step15_field1_fieldset']['step15_field1_question1'] = array(
				'#markup' => $markup_text
			);
			$form['step15_field1_fieldset']['step15_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				'#default_value' => $step15_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step15_field1_fieldset']['step15_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				'#default_value' => $step15_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step15_field1_fieldset\\[step15_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			$earlier_comments = _fetch_exsiting_ddcl_cleat_angle_comments($step15_data->id, $step15_data->data_relation);
			$icom = 1;
			$form['step15_field1_fieldset']['exsiting_comments_title'] = array(
				'#markup' => 'Earlier comment<br>'
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step15_field1_fieldset']['exsiting_comments' . $icom] = array(
					'#markup' => $rowcomment
				);
				$icom++;
			} //$earlier_comments as $rowcomment
			if (empty($form_state['step15_num_comment']))
			{
				$form_state['step15_num_comment'] = 1;
			} //empty($form_state['step15_num_comment'])
			$step15_temp1 = 0;
			for ($step15_i = 0; $step15_i < $form_state['step15_num_comment']; $step15_i++)
			{
				$step15_temp1 = $step15_i;
				$form['step15_field1_fieldset'][$step15_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step15_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step15_field1_fieldset']["step15_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step15_temp1
			);
			$form['step15_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step15_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step15_comment_add_more_callback',
					'wrapper' => 'step15-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step15_num_comment'] > 1)
			{
				$form['step15_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step15_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step15_comment_add_more_callback',
						'wrapper' => 'step15-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step15_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step15_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step15_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step15_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_cleat_angle_step15_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_cleat_angle_step15_form_next_submit'
				)
			);
		} //$step15_data = $form_step15_data->fetchObject()
		else
		{
			$form['step15_field1_fieldset']['step15_field1_question1'] = array(
				'#markup' => $markup_text
			);
			$form['step15_field1_fieldset']['step15_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				//'#default_value' => $step15_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step15_field1_fieldset']['step15_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				//'#default_value' => $step15_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step15_field1_fieldset\\[step15_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			if (empty($form_state['step15_num_comment']))
			{
				$form_state['step15_num_comment'] = 1;
			} //empty($form_state['step15_num_comment'])
			$step15_temp1 = 0;
			for ($step15_i = 0; $step15_i < $form_state['step15_num_comment']; $step15_i++)
			{
				$step15_temp1 = $step15_i;
				$form['step15_field1_fieldset'][$step15_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step15_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step15_field1_fieldset']["step15_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step15_temp1
			);
			$form['step15_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step15_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step15_comment_add_more_callback',
					'wrapper' => 'step15-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step15_num_comment'] > 1)
			{
				$form['step15_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step15_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step15_comment_add_more_callback',
						'wrapper' => 'step15-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step15_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step15_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step15_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step15_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_cleat_angle_step15_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_cleat_angle_step15_form_next_submit'
				)
			);
		} //$step15_data = $form_step15_data->fetchObject()
	} //$form_step15_data
	return $form;
}
function ddcl_cleat_angle_step15_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step15_field1_fieldset']['step15_field1_question1'] = 'question 1';
	$query = "SELECT * FROM ddcl_cleat_angle_form_step15_feedback WHERE uid=:uid";
	$args = array(
		":uid" => $user->uid
	);
	$form_step7_data = db_query($query, $args);
	$form_step7_data_count = $form_step7_data->rowCount();
	$form_step7_data = $form_step7_data->fetchObject();
	if ($form_step7_data_count != 0)
	{
		$query = "UPDATE ddcl_cleat_angle_form_step15_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step15_field1_fieldset']['step15_field1_question1'],
			":question_ok_not_ok" => $v['step15_field1_fieldset']['step15_field1_question_ok'],
			":question_not_ok_comment" => $v['step15_field1_fieldset']['step15_field1_question_not_ok_comment'],
			":data_relation" => "step15"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_cleat_angle_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_cleat_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step7_data->id
		));
		$form_step7_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step15_field1_fieldset']["step15_comment_count"]; $i++)
		{
			if ($v['step15_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step7_data->id,
					":number" => $i + $form_step7_comment_data->max + 1,
					":comment" => $v['step15_field1_fieldset'][$i]["comment"],
					":data_relation" => "step15",
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
			} //$v['step15_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step15_field1_fieldset']["step15_comment_count"]; $i++
	} //$form_step7_data
	else
	{
		$query = "INSERT INTO {ddcl_cleat_angle_form_step15_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1,  :question_ok_not_ok, :question_not_ok_comment,:data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step15_field1_fieldset']['step15_field1_question1'],
			":question_ok_not_ok" => $v['step15_field1_fieldset']['step15_field1_question_ok'],
			":question_not_ok_comment" => $v['step15_field1_fieldset']['step15_field1_question_not_ok_comment'],
			":data_relation" => "step15",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_cleat_angle_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
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
		for ($i = 0; $i <= $v['step15_field1_fieldset']["step15_comment_count"]; $i++)
		{
			if ($v['step15_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step15_field1_fieldset'][$i]["comment"],
					":data_relation" => "step15",
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
			} //$v['step15_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step15_field1_fieldset']["step15_comment_count"]; $i++
	}
	$form_state['redirect'] = 'ddcl-cleat-angle/form/step14/' . $user->uid;
}
function ddcl_cleat_angle_step15_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step15_field1_fieldset']['step15_field1_question1'] = 'question 1';
	$query = "SELECT * FROM ddcl_cleat_angle_form_step15_feedback WHERE uid=:uid";
	$args = array(
		":uid" => $user->uid
	);
	$form_step7_data = db_query($query, $args);
	$form_step7_data_count = $form_step7_data->rowCount();
	$form_step7_data = $form_step7_data->fetchObject();
	if ($form_step7_data_count != 0)
	{
		$query = "UPDATE ddcl_cleat_angle_form_step15_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step15_field1_fieldset']['step15_field1_question1'],
			":question_ok_not_ok" => $v['step15_field1_fieldset']['step15_field1_question_ok'],
			":question_not_ok_comment" => $v['step15_field1_fieldset']['step15_field1_question_not_ok_comment'],
			":data_relation" => "step15"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_cleat_angle_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_cleat_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step7_data->id
		));
		$form_step7_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step15_field1_fieldset']["step15_comment_count"]; $i++)
		{
			if ($v['step15_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step7_data->id,
					":number" => $i + $form_step7_comment_data->max + 1,
					":comment" => $v['step15_field1_fieldset'][$i]["comment"],
					":data_relation" => "step15",
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
			} //$v['step15_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step15_field1_fieldset']["step15_comment_count"]; $i++
	} //$form_step7_data
	else
	{
		$query = "INSERT INTO {ddcl_cleat_angle_form_step15_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok,:question_not_ok_comment, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step15_field1_fieldset']['step15_field1_question1'],
			":question_ok_not_ok" => $v['step15_field1_fieldset']['step15_field1_question_ok'],
			":question_not_ok_comment" => $v['step15_field1_fieldset']['step15_field1_question_not_ok_comment'],
			":data_relation" => "step15",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_cleat_angle_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
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
		for ($i = 0; $i <= $v['step15_field1_fieldset']["step15_comment_count"]; $i++)
		{
			if ($v['step15_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step15_field1_fieldset'][$i]["comment"],
					":data_relation" => "step15",
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
			} //$v['step15_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step15_field1_fieldset']["step15_comment_count"]; $i++
	}
	//$form_state['redirect'] = 'ddcl-cleat-angle/success';
	$form_state['redirect'] = 'ddcl-cleat-angle/form/step16/' . $user->uid;
}
/***************************************/
function step15_comment_add_more_add_one($form, &$form_state)
{
	$form_state['step15_num_comment']++;
	$form_state['rebuild'] = TRUE;
	//$form_state['no_redirect'] = TRUE;
}
function step15_comment_add_more_remove_one($form, &$form_state)
{
	if ($form_state['step15_num_comment'] > 1)
	{
		$form_state['step15_num_comment']--;
	} //$form_state['step15_num_comment'] > 1
	$form_state['rebuild'] = TRUE;
}
function step15_comment_add_more_callback($form, &$form_state)
{
	return $form['step15_field1_fieldset'];
}
/***************************************/
/***************************************** step 15 form ends ****************************/
/************************ Step 16 form starts *********************/
function ddcl_cleat_angle_step16_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('specimen title'));
	$img_path = '';
	$markup_text = 'specimen text<hr>';
	// All the real form fields.
	$form = array();
	$form['step16_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step16-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_cleat_angle_form_step16_feedback');
	$query->fields('ddcl_cleat_angle_form_step16_feedback');
	$query->condition('uid', $user->uid);
	$form_step16_data = $query->execute();
	if ($form_step16_data)
	{
		if ($step16_data = $form_step16_data->fetchObject())
		{
			$form['step16_field1_fieldset']['step16_field1_question1'] = array(
				'#markup' => $markup_text
			);
			$form['step16_field1_fieldset']['step16_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				'#default_value' => $step16_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step16_field1_fieldset']['step16_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				'#default_value' => $step16_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step16_field1_fieldset\\[step16_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			$earlier_comments = _fetch_exsiting_ddcl_cleat_angle_comments($step16_data->id, $step16_data->data_relation);
			$icom = 1;
			$form['step16_field1_fieldset']['exsiting_comments_title'] = array(
				'#markup' => 'Earlier comment<br>'
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step16_field1_fieldset']['exsiting_comments' . $icom] = array(
					'#markup' => $rowcomment
				);
				$icom++;
			} //$earlier_comments as $rowcomment
			if (empty($form_state['step16_num_comment']))
			{
				$form_state['step16_num_comment'] = 1;
			} //empty($form_state['step16_num_comment'])
			$step16_temp1 = 0;
			for ($step16_i = 0; $step16_i < $form_state['step16_num_comment']; $step16_i++)
			{
				$step16_temp1 = $step16_i;
				$form['step16_field1_fieldset'][$step16_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step16_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step16_field1_fieldset']["step16_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step16_temp1
			);
			$form['step16_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step16_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step16_comment_add_more_callback',
					'wrapper' => 'step16-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step16_num_comment'] > 1)
			{
				$form['step16_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step16_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step16_comment_add_more_callback',
						'wrapper' => 'step16-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step16_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step16_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step16_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step16_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_cleat_angle_step16_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_cleat_angle_step16_form_next_submit'
				)
			);
		} //$step16_data = $form_step16_data->fetchObject()
		else
		{
			$form['step16_field1_fieldset']['step16_field1_question1'] = array(
				'#markup' => $markup_text
			);
			$form['step16_field1_fieldset']['step16_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				//'#default_value' => $step16_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step16_field1_fieldset']['step16_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				//'#default_value' => $step16_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step16_field1_fieldset\\[step16_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			if (empty($form_state['step16_num_comment']))
			{
				$form_state['step16_num_comment'] = 1;
			} //empty($form_state['step16_num_comment'])
			$step16_temp1 = 0;
			for ($step16_i = 0; $step16_i < $form_state['step16_num_comment']; $step16_i++)
			{
				$step16_temp1 = $step16_i;
				$form['step16_field1_fieldset'][$step16_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step16_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step16_field1_fieldset']["step16_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step16_temp1
			);
			$form['step16_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step16_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step16_comment_add_more_callback',
					'wrapper' => 'step16-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step16_num_comment'] > 1)
			{
				$form['step16_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step16_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step16_comment_add_more_callback',
						'wrapper' => 'step16-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step16_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step16_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step16_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step16_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_cleat_angle_step16_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_cleat_angle_step16_form_next_submit'
				)
			);
		} //$step16_data = $form_step16_data->fetchObject()
	} //$form_step16_data
	return $form;
}
function ddcl_cleat_angle_step16_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step16_field1_fieldset']['step16_field1_question1'] = 'question 1';
	$query = "SELECT * FROM ddcl_cleat_angle_form_step16_feedback WHERE uid=:uid";
	$args = array(
		":uid" => $user->uid
	);
	$form_step7_data = db_query($query, $args);
	$form_step7_data_count = $form_step7_data->rowCount();
	$form_step7_data = $form_step7_data->fetchObject();
	if ($form_step7_data_count != 0)
	{
		$query = "UPDATE ddcl_cleat_angle_form_step16_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step16_field1_fieldset']['step16_field1_question1'],
			":question_ok_not_ok" => $v['step16_field1_fieldset']['step16_field1_question_ok'],
			":question_not_ok_comment" => $v['step16_field1_fieldset']['step16_field1_question_not_ok_comment'],
			":data_relation" => "step16"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_cleat_angle_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_cleat_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step7_data->id
		));
		$form_step7_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step16_field1_fieldset']["step16_comment_count"]; $i++)
		{
			if ($v['step16_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step7_data->id,
					":number" => $i + $form_step7_comment_data->max + 1,
					":comment" => $v['step16_field1_fieldset'][$i]["comment"],
					":data_relation" => "step16",
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
			} //$v['step16_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step16_field1_fieldset']["step16_comment_count"]; $i++
	} //$form_step7_data
	else
	{
		$query = "INSERT INTO {ddcl_cleat_angle_form_step16_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1,  :question_ok_not_ok, :question_not_ok_comment,:data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step16_field1_fieldset']['step16_field1_question1'],
			":question_ok_not_ok" => $v['step16_field1_fieldset']['step16_field1_question_ok'],
			":question_not_ok_comment" => $v['step16_field1_fieldset']['step16_field1_question_not_ok_comment'],
			":data_relation" => "step16",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_cleat_angle_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
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
		for ($i = 0; $i <= $v['step16_field1_fieldset']["step16_comment_count"]; $i++)
		{
			if ($v['step16_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step16_field1_fieldset'][$i]["comment"],
					":data_relation" => "step16",
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
			} //$v['step16_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step16_field1_fieldset']["step16_comment_count"]; $i++
	}
	$form_state['redirect'] = 'ddcl-cleat-angle/form/step15/' . $user->uid;
}
function ddcl_cleat_angle_step16_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step16_field1_fieldset']['step16_field1_question1'] = 'question 1';
	$query = "SELECT * FROM ddcl_cleat_angle_form_step16_feedback WHERE uid=:uid";
	$args = array(
		":uid" => $user->uid
	);
	$form_step7_data = db_query($query, $args);
	$form_step7_data_count = $form_step7_data->rowCount();
	$form_step7_data = $form_step7_data->fetchObject();
	if ($form_step7_data_count != 0)
	{
		$query = "UPDATE ddcl_cleat_angle_form_step16_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step16_field1_fieldset']['step16_field1_question1'],
			":question_ok_not_ok" => $v['step16_field1_fieldset']['step16_field1_question_ok'],
			":question_not_ok_comment" => $v['step16_field1_fieldset']['step16_field1_question_not_ok_comment'],
			":data_relation" => "step16"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_cleat_angle_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_cleat_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step7_data->id
		));
		$form_step7_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step16_field1_fieldset']["step16_comment_count"]; $i++)
		{
			if ($v['step16_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step7_data->id,
					":number" => $i + $form_step7_comment_data->max + 1,
					":comment" => $v['step16_field1_fieldset'][$i]["comment"],
					":data_relation" => "step16",
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
			} //$v['step16_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step16_field1_fieldset']["step16_comment_count"]; $i++
	} //$form_step7_data
	else
	{
		$query = "INSERT INTO {ddcl_cleat_angle_form_step16_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok,:question_not_ok_comment, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step16_field1_fieldset']['step16_field1_question1'],
			":question_ok_not_ok" => $v['step16_field1_fieldset']['step16_field1_question_ok'],
			":question_not_ok_comment" => $v['step16_field1_fieldset']['step16_field1_question_not_ok_comment'],
			":data_relation" => "step16",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_cleat_angle_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
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
		for ($i = 0; $i <= $v['step16_field1_fieldset']["step16_comment_count"]; $i++)
		{
			if ($v['step16_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step16_field1_fieldset'][$i]["comment"],
					":data_relation" => "step16",
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
			} //$v['step16_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step16_field1_fieldset']["step16_comment_count"]; $i++
	}
	//$form_state['redirect'] = 'ddcl-cleat-angle/success';
	$form_state['redirect'] = 'ddcl-cleat-angle/form/step17/' . $user->uid;
}
/***************************************/
function step16_comment_add_more_add_one($form, &$form_state)
{
	$form_state['step16_num_comment']++;
	$form_state['rebuild'] = TRUE;
	//$form_state['no_redirect'] = TRUE;
}
function step16_comment_add_more_remove_one($form, &$form_state)
{
	if ($form_state['step16_num_comment'] > 1)
	{
		$form_state['step16_num_comment']--;
	} //$form_state['step16_num_comment'] > 1
	$form_state['rebuild'] = TRUE;
}
function step16_comment_add_more_callback($form, &$form_state)
{
	return $form['step16_field1_fieldset'];
}
/***************************************/
/***************************************** step 16 form ends ****************************/


/************************ Step 17 form starts *********************/

function ddcl_cleat_angle_step17_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	// All the real form fields.
	$form = array();
	$form['step17_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#title' => '<strong>Any other recommendations/ Comments?</strong>',
		'#tree' => TRUE,
		'#prefix' => '<div id="step17-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_cleat_angle_form_step17_feedback');
	$query->fields('ddcl_cleat_angle_form_step17_feedback');
	$query->condition('uid', $user->uid);
	$form_step17_data = $query->execute();
	if ($form_step17_data)
	{
		if ($step17_data = $form_step17_data->fetchObject())
		{
			$form['step17_field1_fieldset']['step17_field1_question1'] = array(
				'#type' => 'hidden',
				'#default_value' => 'other recommendations/ Comments',
				'#required' => TRUE
			);
			$earlier_comments = _fetch_exsiting_ddcl_cleat_angle_comments($step17_data->id, $step17_data->data_relation);
			$icom = 1;
			$form['step17_field1_fieldset']['exsiting_comments_title'] = array(
				'#markup' => '<hr><strong>Earlier comment</strong><hr><br>'
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step17_field1_fieldset']['exsiting_comments' . $icom] = array(
					'#markup' => $rowcomment
				);
				$icom++;
			} //$earlier_comments as $rowcomment
			if (empty($form_state['step17_num_comment']))
			{
				$form_state['step17_num_comment'] = 1;
			} //empty($form_state['step17_num_comment'])
			$step17_temp1 = 0;
			for ($step17_i = 0; $step17_i < $form_state['step17_num_comment']; $step17_i++)
			{
				$step17_temp1 = $step17_i;
				$form['step17_field1_fieldset'][$step17_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step17_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step17_field1_fieldset']["step17_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step17_temp1
			);
			$form['step17_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step17_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step17_comment_add_more_callback',
					'wrapper' => 'step17-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step17_num_comment'] > 1)
			{
				$form['step17_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step17_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step17_comment_add_more_callback',
						'wrapper' => 'step17-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step17_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step17_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step17_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step17_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form['previous2'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_cleat_angle_step17_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Submit',
				'#attributes' => array(
					"onclick" => "return confirm('Are you sure?')"
				),
				'#submit' => array(
					'ddcl_cleat_angle_step17_form_next_submit'
				)
			);
			$form['save_submit'] = array(
				'#type' => 'submit',
				'#value' => 'Save as draft',
				'#attributes' => array(
					"onclick" => "return confirm('Are you sure?')"
				),
				'#submit' => array(
					'ddcl_cleat_angle_step17_form_save_submit'
				)
			);
		} //$step17_data = $form_step17_data->fetchObject()
		else
		{
			$form['step17_field1_fieldset']['step17_field1_question1'] = array(
				'#type' => 'hidden',
				'#default_value' => 'other recommendations/ Comments',
				'#required' => TRUE
			);
			$earlier_comments = _fetch_exsiting_ddcl_cleat_angle_comments($step17_data->id, $step17_data->data_relation);
			$icom = 1;
			$form['step17_field1_fieldset']['exsiting_comments_title'] = array(
				'#markup' => '<hr><strong>Earlier comment</strong><hr><br>'
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step17_field1_fieldset']['exsiting_comments' . $icom] = array(
					'#markup' => $rowcomment
				);
				$icom++;
			} //$earlier_comments as $rowcomment
			if (empty($form_state['step17_num_comment']))
			{
				$form_state['step17_num_comment'] = 1;
			} //empty($form_state['step17_num_comment'])
			$step17_temp1 = 0;
			for ($step17_i = 0; $step17_i < $form_state['step17_num_comment']; $step17_i++)
			{
				$step17_temp1 = $step17_i;
				$form['step17_field1_fieldset'][$step17_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step17_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step17_field1_fieldset']["step17_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step17_temp1
			);
			$form['step17_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step17_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step17_comment_add_more_callback',
					'wrapper' => 'step17-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step17_num_comment'] > 1)
			{
				$form['step17_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step17_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step17_comment_add_more_callback',
						'wrapper' => 'step17-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step17_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step17_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step17_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step17_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_cleat_angle_step17_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Submit',
				'#attributes' => array(
					"onclick" => "return confirm('Are you sure?')"
				),
				'#submit' => array(
					'ddcl_cleat_angle_step17_form_previous_submit'
				)
			);
			$form['save_submit'] = array(
				'#type' => 'submit',
				'#value' => 'Save as draft',
				'#attributes' => array(
					"onclick" => "return confirm('Are you sure?')"
				),
				'#submit' => array(
					'ddcl_cleat_angle_step17_form_save_submit'
				)
			);
		} //$step17_data = $form_step17_data->fetchObject()
	} //$form_step17_data
	return $form;
}
function ddcl_cleat_angle_step17_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	/*$query = db_select('ddcl_cleat_angle_form_step8_feedback');
	$query->fields('ddcl_cleat_angle_form_step8_feedback');
	$query->condition('uid', $user->uid);
	$form_step7_data = $query->execute()->fetchObject();
	*/
	$query = "SELECT * FROM ddcl_cleat_angle_form_step17_feedback WHERE uid=:uid";
	$args = array(
		":uid" => $user->uid
	);
	$form_step17_data = db_query($query, $args);
	$form_step17_data_count = $form_step17_data->rowCount();
	$form_step17_data = $form_step17_data->fetchObject();
	//var_dump($form_step7_data);die;
	if ($form_step17_data_count != 0)
	{
		$query = "UPDATE ddcl_cleat_angle_form_step17_feedback SET question1=:question1, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step17_field1_fieldset']['step17_field1_question1'],
			":data_relation" => "step17"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_cleat_angle_form_step1_feedback SET submitted=:submitted WHERE uid=:uid ";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_cleat_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step17_data->id
		));
		$form_step7_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step17_field1_fieldset']["step17_comment_count"]; $i++)
		{
			if ($v['step17_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step17_form_id" => $form_step17_data->id,
					":number" => $i + $form_step17_comment_data->max + 1,
					":comment" => $v['step17_field1_fieldset'][$i]["comment"],
					":data_relation" => "step17",
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
	} //$form_step7_data
	else
	{
		$query = "INSERT INTO {ddcl_cleat_angle_form_step17_feedback} (uid, question1, data_relation,inserted_on) VALUES (:uid,:question1, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step17_field1_fieldset']['step17_field1_question1'],
			":data_relation" => "step17",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_cleat_angle_form_step1_feedback SET submitted=:submitted WHERE uid=:uid ";
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
		for ($i = 0; $i <= $v['step17_field1_fieldset']["step8_comment_count"]; $i++)
		{
			if ($v['step17_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO 	ddcl_cleat_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step17_field1_fieldset'][$i]["comment"],
					":data_relation" => "step17",
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
	$form_state['redirect'] = 'ddcl-cleat-angle/form/step16/' . $user->uid;
}
function ddcl_cleat_angle_step17_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	/*$query = db_select('ddcl_cleat_angle_form_step8_feedback');
	$query->fields('ddcl_cleat_angle_form_step8_feedback');
	$query->condition('uid', $user->uid);
	$form_step7_data = $query->execute()->fetchObject();
	*/
	$query = "SELECT * FROM ddcl_cleat_angle_form_step17_feedback WHERE uid=:uid";
	$args = array(
		":uid" => $user->uid
	);
	$form_step17_data = db_query($query, $args);
	$form_step17_data_count = $form_step17_data->rowCount();
	$form_step17_data = $form_step17_data->fetchObject();
	//var_dump($form_step7_data);die;
	if ($form_step17_data_count != 0)
	{
		$query = "UPDATE ddcl_cleat_angle_form_step17_feedback SET question1=:question1, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step17_field1_fieldset']['step17_field1_question1'],
			":data_relation" => "step17"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_cleat_angle_form_step1_feedback SET submitted=:submitted WHERE uid=:uid ";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_cleat_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step17_data->id
		));
		$form_step17_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step17_field1_fieldset']["step17_comment_count"]; $i++)
		{
			if ($v['step17_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step17_data->id,
					":number" => $i + $form_step17_comment_data->max + 1,
					":comment" => $v['step17_field1_fieldset'][$i]["comment"],
					":data_relation" => "step17",
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
	} //$form_step7_data
	else
	{
		$query = "INSERT INTO {ddcl_cleat_angle_form_step17_feedback} (uid, question1, data_relation,inserted_on) VALUES (:uid,:question1, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step17_field1_fieldset']['step17_field1_question1'],
			":data_relation" => "step17",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_cleat_angle_form_step1_feedback SET submitted=:submitted WHERE uid=:uid ";
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
		for ($i = 0; $i <= $v['step17_field1_fieldset']["step8_comment_count"]; $i++)
		{
			if ($v['step17_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO 	ddcl_cleat_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step17_field1_fieldset'][$i]["comment"],
					":data_relation" => "step17",
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
	$form_state['redirect'] = 'ddcl-cleat-angle/success/' . $user->uid;
}


function ddcl_cleat_angle_step17_form_save_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step17_field1_fieldset']['step17_field1_question1'] = 'question 1';
	$query = "SELECT * FROM ddcl_cleat_angle_form_step17_feedback WHERE uid=:uid";
	$args = array(
		":uid" => $user->uid
	);
	$form_step17_data = db_query($query, $args);
	$form_step17_data_count = $form_step17_data->rowCount();
	$form_step17_data = $form_step17_data->fetchObject();
	if ($form_step17_data_count != 0)
	{
		$query = "UPDATE ddcl_cleat_angle_form_step17_feedback SET question1=:question1, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step17_field1_fieldset']['step17_field1_question1'],
			":data_relation" => "step17"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_cleat_angle_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 0
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_cleat_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step17_data->id
		));
		$form_step17_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step17_field1_fieldset']["step17_comment_count"]; $i++)
		{
			if ($v['step17_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step17_data->id,
					":number" => $i + $form_step17_comment_data->max + 1,
					":comment" => $v['step17_field1_fieldset'][$i]["comment"],
					":data_relation" => "step17",
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
			} //$v['step17_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step17_field1_fieldset']["step17_comment_count"]; $i++
	} //$form_step17_data
	else
	{
		$query = "INSERT INTO {ddcl_cleat_angle_form_step17_feedback} (uid, question1, data_relation,inserted_on) VALUES (:uid,:question1, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step17_field1_fieldset']['step17_field1_question1'],
			":data_relation" => "step17",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_cleat_angle_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
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
		for ($i = 0; $i <= $v['step17_field1_fieldset']["step17_comment_count"]; $i++)
		{
			if ($v['step17_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_cleat_angle_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step17_field1_fieldset'][$i]["comment"],
					":data_relation" => "step17",
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
			} //$v['step17_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step17_field1_fieldset']["step17_comment_count"]; $i++
	}
	drupal_set_message(t('You have successfully saved your feedback. You can anytime edit your feedback form.'), 'status');
	drupal_goto('home');
}

/***************************************/
function step17_comment_add_more_add_one($form, &$form_state)
{
	$form_state['step17_num_comment']++;
	$form_state['rebuild'] = TRUE;
	//$form_state['no_redirect'] = TRUE;
}
function step17_comment_add_more_remove_one($form, &$form_state)
{
	if ($form_state['step17_num_comment'] > 1)
	{
		$form_state['step17_num_comment']--;
	} //$form_state['step17_num_comment'] > 1
	$form_state['rebuild'] = TRUE;
}
function step17_comment_add_more_callback($form, &$form_state)
{
	return $form['step17_field1_fieldset'];
}
/***************************************/
/****************************************** step 17 form ends ****************************/



/**
 * Displays simple confirmation page after finishing the whole process.
 */
function ddcl_cleat_angle_success()
{
	$data .= drupal_set_message(t('You have successfully submitted the feedback.'), 'status');
	$data .= drupal_goto('home');
	return $data;
}
/**********************************************/
function ddcl_cleat_angle_result_page()
{
	/* get pending proposals to be approved */
	$output = "";
	$output .= l("Download All Result in CSV", "ddcl-cleat-angle/form/result/download");
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
step12.question_not_ok_comment as "step12_question_not_ok_comment",

step13.id as "step13_id",
step13.data_relation as "step13_data_relation",
step13.question1 as "step13_question1",
step13.question_ok_not_ok as "step13_question_ok_not_ok",
step13.question_not_ok_comment as "step13_question_not_ok_comment",

step14.id as "step14_id",
step14.data_relation as "step14_data_relation",
step14.question1 as "step14_question1",
step14.question_ok_not_ok as "step14_question_ok_not_ok",
step14.question_not_ok_comment as "step14_question_not_ok_comment",

step15.id as "step15_id",
step15.data_relation as "step15_data_relation",
step15.question1 as "step15_question1",
step15.question_ok_not_ok as "step15_question_ok_not_ok",
step15.question_not_ok_comment as "step15_question_not_ok_comment",

step16.id as "step16_id",
step16.data_relation as "step16_data_relation",
step16.question1 as "step16_question1",
step16.question_ok_not_ok as "step16_question_ok_not_ok",
step16.question_not_ok_comment as "step16_question_not_ok_comment",

step17.id as "step17_id",
step17.data_relation as "step17_data_relation",
step17.question1 as "step17_question1"


from ddcl_cleat_angle_form_step1_feedback step1

join ddcl_cleat_angle_form_step2_feedback step2 on step1.uid=step2.uid
join ddcl_cleat_angle_form_step3_feedback step3 on step2.uid=step3.uid
join ddcl_cleat_angle_form_step4_feedback step4 on step3.uid=step4.uid
join ddcl_cleat_angle_form_step5_feedback step5 on step4.uid=step5.uid
join ddcl_cleat_angle_form_step6_feedback step6 on step5.uid=step6.uid
join ddcl_cleat_angle_form_step7_feedback step7 on step6.uid=step7.uid
join ddcl_cleat_angle_form_step8_feedback step8 on step7.uid=step8.uid
join ddcl_cleat_angle_form_step9_feedback step9 on step8.uid=step9.uid
join ddcl_cleat_angle_form_step10_feedback step10 on step9.uid=step10.uid
join ddcl_cleat_angle_form_step11_feedback step11 on step10.uid=step11.uid
join ddcl_cleat_angle_form_step12_feedback step12 on step11.uid=step12.uid
join ddcl_cleat_angle_form_step13_feedback step13 on step12.uid=step13.uid
join ddcl_cleat_angle_form_step14_feedback step14 on step13.uid=step14.uid
join ddcl_cleat_angle_form_step15_feedback step15 on step14.uid=step15.uid
join ddcl_cleat_angle_form_step16_feedback step16 on step15.uid=step16.uid
join ddcl_cleat_angle_form_step17_feedback step17 on step16.uid=step17.uid
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
			_fetch_ddcl_cleat_angle_exsiting_comments_result($feedback_data->step2_id, $feedback_data->step2_data_relation),
			$feedback_data->step3_question_ok_not_ok,
			$feedback_data->step3_question_not_ok_comment,
			_fetch_ddcl_cleat_angle_exsiting_comments_result($feedback_data->step3_id, $feedback_data->step3_data_relation),
			$feedback_data->step4_question_ok_not_ok,
			$feedback_data->step4_question_not_ok_comment,
			_fetch_ddcl_cleat_angle_exsiting_comments_result($feedback_data->step4_id, $feedback_data->step4_data_relation),
			$feedback_data->step5_question_ok_not_ok,
			$feedback_data->step5_question_not_ok_comment,
			_fetch_ddcl_cleat_angle_exsiting_comments_result($feedback_data->step5_id, $feedback_data->step5_data_relation),
			$feedback_data->step6_question_ok_not_ok,
			$feedback_data->step6_question_not_ok_comment,
			_fetch_ddcl_cleat_angle_exsiting_comments_result($feedback_data->step6_id, $feedback_data->step6_data_relation),
			$feedback_data->step7_question_ok_not_ok,
			$feedback_data->step7_question_not_ok_comment,
			_fetch_ddcl_cleat_angle_exsiting_comments_result($feedback_data->step7_id, $feedback_data->step7_data_relation),
			$feedback_data->step8_question_ok_not_ok,
			$feedback_data->step8_question_not_ok_comment,
			_fetch_ddcl_cleat_angle_exsiting_comments_result($feedback_data->step8_id, $feedback_data->step8_data_relation),
			$feedback_data->step9_question_ok_not_ok,
			$feedback_data->step9_question_not_ok_comment,
			_fetch_ddcl_cleat_angle_exsiting_comments_result($feedback_data->step9_id, $feedback_data->step9_data_relation),
			$feedback_data->step10_question_ok_not_ok,
			$feedback_data->step10_question_not_ok_comment,
			_fetch_ddcl_cleat_angle_exsiting_comments_result($feedback_data->step10_id, $feedback_data->step10_data_relation),
			$feedback_data->step11_question_ok_not_ok,
			$feedback_data->step11_question_not_ok_comment,
			_fetch_ddcl_cleat_angle_exsiting_comments_result($feedback_data->step11_id, $feedback_data->step11_data_relation),
			$feedback_data->step12_question_ok_not_ok,
			$feedback_data->step12_question_not_ok_comment,
			_fetch_ddcl_cleat_angle_exsiting_comments_result($feedback_data->step12_id, $feedback_data->step12_data_relation),
			$feedback_data->step13_question_ok_not_ok,
			$feedback_data->step13_question_not_ok_comment,
			_fetch_ddcl_cleat_angle_exsiting_comments_result($feedback_data->step13_id, $feedback_data->step13_data_relation),
			$feedback_data->step14_question_ok_not_ok,
			$feedback_data->step14_question_not_ok_comment,
			_fetch_ddcl_cleat_angle_exsiting_comments_result($feedback_data->step14_id, $feedback_data->step14_data_relation),
			$feedback_data->step15_question_ok_not_ok,
			$feedback_data->step15_question_not_ok_comment,
			_fetch_ddcl_cleat_angle_exsiting_comments_result($feedback_data->step15_id, $feedback_data->step15_data_relation),
			$feedback_data->step16_question_ok_not_ok,
			$feedback_data->step16_question_not_ok_comment,
			_fetch_ddcl_cleat_angle_exsiting_comments_result($feedback_data->step16_id, $feedback_data->step16_data_relation),
			_fetch_ddcl_cleat_angle_exsiting_comments_result($feedback_data->step17_id, $feedback_data->step17_data_relation),
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
		'Page 13 Is this ok/not ok',
		'Page 13 Is this ok/not ok comment',
		'Page 13 comments',
		'Page 14 Is this ok/not ok',
		'Page 14 Is this ok/not ok comment',
		'Page 14 comments',
		'Page 15 Is this ok/not ok',
		'Page 15 Is this ok/not ok comment',
		'Page 15 comments',
		'Page 16 Is this ok/not ok',
		'Page 16 Is this ok/not ok comment',
		'Page 16 comments',
		'Page 17 Any other comments'
	);
	$output .= theme('table', array(
		'header' => $data_header,
		'rows' => $feedback_rows,
		'sticky' => FALSE
	));
	return $output;
}
//displayed in user page
function _fetch_exsiting_ddcl_cleat_angle_comments($step_id, $data_relation)
{
	global $base_url;
	$query_for_comment = db_query("SELECT * FROM ddcl_cleat_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id AND data_relation=:data_relation ORDER BY number ASC", array(
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
			<a href='{$base_url}/ddcl-cleat-angle/delete/{$row->data_relation}/{$row->id}'>Delete</a><br>" . $row->inserted_on . "</h6><br>";
			$i++;
		} //$row = $query_for_comment->fetchObject()
	} //$query_for_comment == NULL
	else
	{
		$comment[$i] = 'No earlier comments';
	}
	return $comment;
}
function ddcl_cleat_angle_delete_comment()
{
	global $base_url;
	$comment_id = arg(3);
	$comment_step = arg(2);
	if ($comment_id)
	{
		$query = "DELETE FROM {ddcl_cleat_angle_feedback_form_comment} WHERE id = :id";
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
	drupal_goto("{$base_url}/ddcl-cleat-angle/form/{$comment_step}/");
}
function _fetch_ddcl_cleat_angle_exsiting_comments($step_id, $data_relation)
{
	$query_for_comment = db_query("SELECT * FROM ddcl_cleat_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id AND data_relation=:data_relation ORDER BY number ASC", array(
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
function _fetch_ddcl_cleat_angle_exsiting_comments_result($step_id, $data_relation)
{
	$query_for_comment = db_query("SELECT * FROM ddcl_cleat_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id AND data_relation=:data_relation ORDER BY number ASC", array(
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
function _fetch_ddcl_cleat_angle_exsiting_comments_result_csv($step_id, $data_relation)
{
	$query_for_comment = db_query("SELECT * FROM ddcl_cleat_angle_feedback_form_comment WHERE step1_form_id = :step1_form_id AND data_relation=:data_relation ORDER BY number ASC", array(
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
function ddcl_cleat_angle_result_generate_csv()
{
	global $user;
	if ($user->uid != NULL)
	{
		$result_dir = $_SERVER['DOCUMENT_ROOT'] . base_path() . "osdag-downloads/ddcl-cleat-angle-form-result/{$user->uid}/";
		if (!file_exists($result_dir))
		{
			mkdir($result_dir, 0755, TRUE);
		} //!file_exists($result_dir)
		$result_file = $result_dir . "ddcl-cleat-angle-result-{$user->uid}" . ".csv";
		$csv_file = "ddcl-cleat-angle-result-{$user->uid}" . ".csv";
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
			'Page 13 Is this ok/not ok',
			'Page 13 Is this ok/not ok comment',
			'Page 13 comments',
			'Page 14 Is this ok/not ok',
			'Page 14 Is this ok/not ok comment',
			'Page 14 comments',
			'Page 15 Is this ok/not ok',
			'Page 15 Is this ok/not ok comment',
			'Page 15 comments',
			'Page 16 Is this ok/not ok',
			'Page 16 Is this ok/not ok comment',
			'Page 16 comments',
			'Page 17 Any other comments',
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
step12.question_not_ok_comment as "step12_question_not_ok_comment",

step13.id as "step13_id",
step13.data_relation as "step13_data_relation",
step13.question1 as "step13_question1",
step13.question_ok_not_ok as "step13_question_ok_not_ok",
step13.question_not_ok_comment as "step13_question_not_ok_comment",

step14.id as "step14_id",
step14.data_relation as "step14_data_relation",
step14.question1 as "step14_question1",
step14.question_ok_not_ok as "step14_question_ok_not_ok",
step14.question_not_ok_comment as "step14_question_not_ok_comment",

step15.id as "step15_id",
step15.data_relation as "step15_data_relation",
step15.question1 as "step15_question1",
step15.question_ok_not_ok as "step15_question_ok_not_ok",
step15.question_not_ok_comment as "step15_question_not_ok_comment",

step16.id as "step16_id",
step16.data_relation as "step16_data_relation",
step16.question1 as "step16_question1",
step16.question_ok_not_ok as "step16_question_ok_not_ok",
step16.question_not_ok_comment as "step16_question_not_ok_comment",

step17.id as "step17_id",
step17.data_relation as "step17_data_relation",
step17.question1 as "step17_question1"

from ddcl_cleat_angle_form_step1_feedback step1

join ddcl_cleat_angle_form_step2_feedback step2 on step1.uid=step2.uid
join ddcl_cleat_angle_form_step3_feedback step3 on step2.uid=step3.uid
join ddcl_cleat_angle_form_step4_feedback step4 on step3.uid=step4.uid
join ddcl_cleat_angle_form_step5_feedback step5 on step4.uid=step5.uid
join ddcl_cleat_angle_form_step6_feedback step6 on step5.uid=step6.uid
join ddcl_cleat_angle_form_step7_feedback step7 on step6.uid=step7.uid
join ddcl_cleat_angle_form_step8_feedback step8 on step7.uid=step8.uid
join ddcl_cleat_angle_form_step9_feedback step9 on step8.uid=step9.uid
join ddcl_cleat_angle_form_step10_feedback step10 on step9.uid=step10.uid
join ddcl_cleat_angle_form_step11_feedback step11 on step10.uid=step11.uid
join ddcl_cleat_angle_form_step12_feedback step12 on step11.uid=step12.uid
join ddcl_cleat_angle_form_step13_feedback step13 on step12.uid=step13.uid
join ddcl_cleat_angle_form_step14_feedback step14 on step13.uid=step14.uid
join ddcl_cleat_angle_form_step15_feedback step15 on step14.uid=step15.uid
join ddcl_cleat_angle_form_step16_feedback step16 on step15.uid=step16.uid
join ddcl_cleat_angle_form_step17_feedback step17 on step16.uid=step17.uid

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
				_fetch_ddcl_cleat_angle_exsiting_comments_result_csv($feedback_data->step2_id, $feedback_data->step2_data_relation),
				$feedback_data->step3_question_ok_not_ok,
				$feedback_data->step3_question_not_ok_comment,
				_fetch_ddcl_cleat_angle_exsiting_comments_result_csv($feedback_data->step3_id, $feedback_data->step3_data_relation),
				$feedback_data->step4_question_ok_not_ok,
				$feedback_data->step4_question_not_ok_comment,
				_fetch_ddcl_cleat_angle_exsiting_comments_result_csv($feedback_data->step4_id, $feedback_data->step4_data_relation),
				$feedback_data->step5_question_ok_not_ok,
				$feedback_data->step5_question_not_ok_comment,
				_fetch_ddcl_cleat_angle_exsiting_comments_result_csv($feedback_data->step5_id, $feedback_data->step5_data_relation),
				$feedback_data->step6_question_ok_not_ok,
				$feedback_data->step6_question_not_ok_comment,
				_fetch_ddcl_cleat_angle_exsiting_comments_result_csv($feedback_data->step6_id, $feedback_data->step6_data_relation),
				$feedback_data->step7_question_ok_not_ok,
				$feedback_data->step7_question_not_ok_comment,
				_fetch_ddcl_cleat_angle_exsiting_comments_result_csv($feedback_data->step7_id, $feedback_data->step7_data_relation),
				$feedback_data->step8_question_ok_not_ok,
				$feedback_data->step8_question_not_ok_comment,
				_fetch_ddcl_cleat_angle_exsiting_comments_result_csv($feedback_data->step8_id, $feedback_data->step8_data_relation),
				$feedback_data->step9_question_ok_not_ok,
				$feedback_data->step9_question_not_ok_comment,
				_fetch_ddcl_cleat_angle_exsiting_comments_result_csv($feedback_data->step9_id, $feedback_data->step9_data_relation),
				$feedback_data->step10_question_ok_not_ok,
				$feedback_data->step10_question_not_ok_comment,
				_fetch_ddcl_cleat_angle_exsiting_comments_result_csv($feedback_data->step10_id, $feedback_data->step10_data_relation),
				$feedback_data->step11_question_ok_not_ok,
				$feedback_data->step11_question_not_ok_comment,
				_fetch_ddcl_cleat_angle_exsiting_comments_result_csv($feedback_data->step11_id, $feedback_data->step11_data_relation),
				$feedback_data->step12_question_ok_not_ok,
				$feedback_data->step12_question_not_ok_comment,
				_fetch_ddcl_cleat_angle_exsiting_comments_result_csv($feedback_data->step12_id, $feedback_data->step12_data_relation),
				$feedback_data->step13_question_ok_not_ok,
				$feedback_data->step13_question_not_ok_comment,
				_fetch_ddcl_cleat_angle_exsiting_comments_result_csv($feedback_data->step13_id, $feedback_data->step13_data_relation),
				$feedback_data->step14_question_ok_not_ok,
				$feedback_data->step14_question_not_ok_comment,
				_fetch_ddcl_cleat_angle_exsiting_comments_result_csv($feedback_data->step14_id, $feedback_data->step14_data_relation),
				$feedback_data->step15_question_ok_not_ok,
				$feedback_data->step15_question_not_ok_comment,
				_fetch_ddcl_cleat_angle_exsiting_comments_result_csv($feedback_data->step15_id, $feedback_data->step15_data_relation),
				$feedback_data->step16_question_ok_not_ok,
				$feedback_data->step16_question_not_ok_comment,
				_fetch_ddcl_cleat_angle_exsiting_comments_result_csv($feedback_data->step16_id, $feedback_data->step16_data_relation),
				_fetch_ddcl_cleat_angle_exsiting_comments_result_csv($feedback_data->step17_id, $feedback_data->step17_data_relation),
			);
			fputcsv($fp, $item);
			$i++;
		} //$feedback_data = $result->fetchObject()
		
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
