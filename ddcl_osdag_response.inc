<?php


/**
 * Fetches and displays the form.
 */
function ddcl_osdag_response_form_page()
{
	global $user;
	$data = '';
	if (!$user)
	{
		$data .= "Please login to give feedback";
	} //!$user
	/************* check if user has already submitted a proposal **************/
	$query = db_select('ddcl_osdag_response_form_step1_feedback');
	$query->fields('ddcl_osdag_response_form_step1_feedback');
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
				//$data .= drupal_goto('ddcl-osdag-response/form/step1/'.$user->uid);
				$ddcl_osdag_response_step1_form = drupal_get_form("ddcl_osdag_response_step1_form");
				$data .= drupal_render($ddcl_osdag_response_step1_form);
			}
		} //$feedback_data = $feedback_q->fetchObject()
		else
		{
			$ddcl_osdag_response_step1_form = drupal_get_form("ddcl_osdag_response_step1_form");
			$data .= drupal_render($ddcl_osdag_response_step1_form);
		}
	} //$feedback_q
	//$data .= drupal_get_form('ddcl_osdag_response_step1_form');
	return $data;
}
/**
 * Displays simple confirmation page after finishing the whole process.
 */
function ddcl_osdag_response_success()
{
	$data .= drupal_set_message(t('You have successfully submitted the feedback.'), 'status');
	$data .= drupal_goto('home');
	return $data;
}
/**
 * Generates the form.
 */
function ddcl_osdag_response_step1_form($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	global $base_url;
	// All the real form fields.
	$query = db_select('ddcl_osdag_response_form_step1_feedback');
	$query->fields('ddcl_osdag_response_form_step1_feedback');
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
			'ddcl_osdag_response_step1_form_submit'
		)
	);
	return $form;
}
function ddcl_osdag_response_step1_form_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$query = db_select('ddcl_osdag_response_form_step1_feedback');
	$query->fields('ddcl_osdag_response_form_step1_feedback');
	$query->condition('uid', $user->uid);
	$form_step1_data = $query->execute();
	if ($form_step1_data)
	{
		if ($step1_data = $form_step1_data->fetchObject())
		{
			$query = "UPDATE ddcl_osdag_response_form_step1_feedback SET name=:name, email_id=:email_id, institute=:institute, current_position=:current_position WHERE uid=:uid ";
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
			$query = "INSERT INTO {ddcl_osdag_response_form_step1_feedback} (uid, name, email_id, institute, current_position, data_reletion, inserted_on) VALUES (:uid,:name,:email_id, :institute, :current_position, :data_reletion,:inserted_on)";
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
	$form_state['redirect'] = 'ddcl-osdag-response/form/step2/' . $user->uid;
}
/************************ Step 2 form starts *********************/
function ddcl_osdag_response_step2_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	// All the real form fields.
	$img_path = $base_url . '/' . drupal_get_path("module", "osdag_response_form") . '/images';
	$step2_options = array(
		'100mm' => '100mm' . '
					<p>Reference: The connected leg is so chosen that at least two horizontal rows
of bolts can be accommodated. Hence, the assumed angle should have a leg
length of 100 mm or more. (Ref I - page 369 - Point 4 of 5.7.4)
</p>
					<img src="' . $img_path . '/100mm.png" />',
		'150mm' => '150mm' . '<p>Reference: The connected length is adopted in such a way that at least
three horizontal rows of rivets can be accommodated. In general, it is 150
mm or more (Ref II - Time 9:00)</p>
<img src="' . $img_path . '/150mm.png" width="70%" />',
		'Any other recommendation' => 'Any other recommendation'
	);
	$form = array();
	$form['step2_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step2-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_osdag_response_form_step2_feedback');
	$query->fields('ddcl_osdag_response_form_step2_feedback');
	$query->condition('uid', $user->uid);
	$form_step2_data = $query->execute();
	if ($form_step2_data)
	{
		if ($step2_data = $form_step2_data->fetchObject())
		{
			$form['step2_field1_fieldset']['step2_field1_question1'] = array(
				'#title' => 'What should be the minimum length of the connected leg (to the support) of the seated angle?',
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
			$earlier_comments = _fetch_exsiting_comments($step2_data->id, $step2_data->data_relation);
			$icom = 1;
			$form['step2_field1_fieldset']['exsiting_comments_title'] = array(
				'#markup' => 'Earlier comment<br>'
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
					'ddcl_osdag_response_step2_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_osdag_response_step2_form_next_submit'
				)
			);
		} //$step2_data = $form_step2_data->fetchObject()
		else
		{
			$form['step2_field1_fieldset']['step2_field1_question1'] = array(
				'#title' => 'What should be the minimum length of the connected leg (to the support) of the seated angle?',
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
					'ddcl_osdag_response_step2_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_osdag_response_step2_form_next_submit'
				)
			);
		} //$step2_data = $form_step2_data->fetchObject()
	} //$form_step2_data
	return $form;
}
function ddcl_osdag_response_step2_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$form_state['redirect'] = 'ddcl-osdag-response/form/step1/' . $user->uid;
}
function ddcl_osdag_response_step2_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$query = db_select('ddcl_osdag_response_form_step2_feedback');
	$query->fields('ddcl_osdag_response_form_step2_feedback');
	$query->condition('uid', $user->uid);
	$form_step2_data = $query->execute()->fetchObject();
	if ($form_step2_data)
	{
		$query = "UPDATE ddcl_osdag_response_form_step2_feedback SET question1=:question1, any_other_recommendation=:any_other_recommendation, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step2_data->id
		));
		$form_step2_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step2_field1_fieldset']["step2_comment_count"]; $i++)
		{
			if ($v['step2_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_osdag_response_form_step2_feedback} (uid, question1, any_other_recommendation, data_relation,inserted_on) VALUES (:uid,:question1,:any_other_recommendation, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-osdag-response/form/step3/' . $user->uid;
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
function ddcl_osdag_response_step3_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	// All the real form fields.
	$form = array();
	$form['step3_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step3-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_osdag_response_form_step3_feedback');
	$query->fields('ddcl_osdag_response_form_step3_feedback');
	$query->condition('uid', $user->uid);
	$form_step3_data = $query->execute();
	if ($form_step3_data)
	{
		if ($step3_data = $form_step3_data->fetchObject())
		{
			$form['step3_field1_fieldset']['step3_field1_question1'] = array(
				'#title' => '<strong>b. What should be the minimum thickness of the seated angle?</strong>',
				'#type' => 'textfield',
				'#default_value' => $step3_data->question1,
				'#required' => TRUE
			);
			$form['step3_field1_fieldset']['step3_field1_question2'] = array(
				'#title' => 'What code provision(/s)/ reference text should be referred for this?',
				'#type' => 'textarea',
				'#default_value' => $step3_data->question2,
				'#states' => array(
					'visible' => array(
						':input[name="step3_field1_fieldset\\[step3_field1_question1\\]"]' => array(
							array(
								'filled' => true
							)
						)
					)
				)
			);
			$earlier_comments = _fetch_exsiting_comments($step3_data->id, $step3_data->data_relation);
			$icom = 1;
			$form['step3_field1_fieldset']['exsiting_comments_title'] = array(
				'#markup' => 'Earlier comment<br>'
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
					'ddcl_osdag_response_step3_form_previous_submit'
				)
			);
			$form['next3'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_osdag_response_step3_form_next_submit'
				)
			);
		} //$step3_data = $form_step3_data->fetchObject()
		else
		{
			$form['step3_field1_fieldset']['step3_field1_question1'] = array(
				'#title' => '<strong>b. What should be the minimum thickness of the seated angle?</strong>',
				'#type' => 'textfield',
				'#required' => TRUE
				//'#default_value' => $step3_data->question1
			);
			$form['step3_field1_fieldset']['step3_field1_question2'] = array(
				'#title' => 'What code provision(/s)/ reference text should be referred for this?',
				'#type' => 'textarea',
				//'#default_value' => $step3_data->question2,
				'#states' => array(
					'visible' => array(
						':input[name="step3_field1_fieldset\\[step3_field1_question1\\]"]' => array(
							array(
								'filled' => true
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
					'ddcl_osdag_response_step3_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_osdag_response_step3_form_next_submit'
				)
			);
		} //$step3_data = $form_step3_data->fetchObject()
	} //$form_step3_data
	return $form;
}
function ddcl_osdag_response_step3_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$form_state['redirect'] = 'ddcl-osdag-response/form/step2/' . $user->uid;
}
function ddcl_osdag_response_step3_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$query = db_select('ddcl_osdag_response_form_step3_feedback');
	$query->fields('ddcl_osdag_response_form_step3_feedback');
	$query->condition('uid', $user->uid);
	$form_step2_data = $query->execute()->fetchObject();
	//var_dump($form_step2_data);die;
	if ($form_step2_data)
	{
		$query = "UPDATE ddcl_osdag_response_form_step3_feedback SET question1=:question1,question2=:question2, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step3_field1_fieldset']['step3_field1_question1'],
			":question2" => $v['step3_field1_fieldset']['step3_field1_question2'],
			":data_relation" => "step2"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step2_data->id
		));
		$form_step2_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step3_field1_fieldset']["step3_comment_count"]; $i++)
		{
			if ($v['step3_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step2_data->id,
					":number" => $i + $form_step2_comment_data->max + 1,
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
		$query = "INSERT INTO {ddcl_osdag_response_form_step3_feedback} (uid, question1, question2, data_relation,inserted_on) VALUES (:uid,:question1,:question2, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-osdag-response/form/step4/' . $user->uid;
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
function ddcl_osdag_response_step4_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	// All the real form fields.
	$img_path = $base_url . '/' . drupal_get_path("module", "osdag_response_form") . '/images';
	$step4_options = array(
		'Plastic Moment Capacity (with Plastic Section Modulus)' => 'Plastic Moment Capacity (with Plastic Section Modulus)<p>Argument: The full available plastic moment capacity of the outstanding leg should be utilized to resist the moment. In the event that the connection fails in other limit states, the failure would be brittle. It would not make sense to add significantly more conservatism in the moment resistance than the other limit states.
				<img src="' . $img_path . '/Elastic_Moment_Capacity.png" />',
		'Elastic Moment Capacity (with Elastic Section Modulus)' => 'Elastic Moment Capacity (with Elastic Section Modulus)<p>Reference: (Ref I - Example 5.14 - page 407 )</p>
				<img src="' . $img_path . '/Reference-Ref-II-10-52min.png" />'
	);
	$form = array();
	$form['step4_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step4-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_osdag_response_form_step4_feedback');
	$query->fields('ddcl_osdag_response_form_step4_feedback');
	$query->condition('uid', $user->uid);
	$form_step4_data = $query->execute();
	if ($form_step4_data)
	{
		if ($step4_data = $form_step4_data->fetchObject())
		{
			$form['step4_field1_fieldset']['step4_field1_question1'] = array(
				'#title' => '<strong>c. The moment capacity of the outstanding leg should be based on</strong>',
				'#type' => 'radios',
				'#options' => $step4_options,
				'#default_value' => $step4_data->question1,
				'#required' => true
			);
			$earlier_comments = _fetch_exsiting_comments($step4_data->id, $step4_data->data_relation);
			$icom = 1;
			$form['step4_field1_fieldset']['exsiting_comments_title'] = array(
				'#markup' => 'Earlier comment<br>'
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
					'ddcl_osdag_response_step4_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_osdag_response_step4_form_next_submit'
				)
			);
		} //$step4_data = $form_step4_data->fetchObject()
		else
		{
			$form['step4_field1_fieldset']['step4_field1_question1'] = array(
				'#title' => '<strong>c. The moment capacity of the outstanding leg should be based on</strong>',
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
					'ddcl_osdag_response_step4_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_osdag_response_step4_form_next_submit'
				)
			);
		} //$step4_data = $form_step4_data->fetchObject()
	} //$form_step4_data
	return $form;
}
function ddcl_osdag_response_step4_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$form_state['redirect'] = 'ddcl-osdag-response/form/step3/' . $user->uid;
}
function ddcl_osdag_response_step4_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$query = db_select('ddcl_osdag_response_form_step4_feedback');
	$query->fields('ddcl_osdag_response_form_step4_feedback');
	$query->condition('uid', $user->uid);
	$form_step4_data = $query->execute()->fetchObject();
	//var_dump($form_step4_data);die;
	if ($form_step4_data)
	{
		$query = "UPDATE ddcl_osdag_response_form_step4_feedback SET question1=:question1, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step4_data->id
		));
		$form_step4_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step4_field1_fieldset']["step4_comment_count"]; $i++)
		{
			if ($v['step4_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_osdag_response_form_step4_feedback} (uid, question1, data_relation,inserted_on) VALUES (:uid,:question1, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-osdag-response/form/step5/' . $user->uid;
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
function ddcl_osdag_response_step5_form($form, &$form_state, $no_js_use = FALSE)
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
	$query = db_select('ddcl_osdag_response_form_step5_feedback');
	$query->fields('ddcl_osdag_response_form_step5_feedback');
	$query->condition('uid', $user->uid);
	$form_step5_data = $query->execute();
	if ($form_step5_data)
	{
		if ($step5_data = $form_step5_data->fetchObject())
		{
			$form['step5_field1_fieldset']['step5_field1_question1'] = array(
				'#title' => '<strong>c. Should the vertical leg of the seated angle be checked against buckling</strong>',
				'#type' => 'radios',
				'#options' => $step5_options,
				'#default_value' => $step5_data->question1,
				'#required' => true
			);
			$form['step5_field1_fieldset']['step5_field2_what_IS_code_clauses_and_references_should_be_followed'] = array(
				'#title' => 'If yes, what IS code clauses and references should be followed?',
				'#type' => 'textarea',
				'#default_value' => $step5_data->what_IS_code_clauses_and_references_should_be_followed,
				'#states' => array(
					'visible' => array(
						':input[name="step5_field1_fieldset\\[step5_field1_question1\\]"]' => array(
							array(
								'value' => t('Yes')
							)
						)
					)
				)
			);
			$earlier_comments = _fetch_exsiting_comments($step5_data->id, $step5_data->data_relation);
			$icom = 1;
			$form['step5_field1_fieldset']['exsiting_comments_title'] = array(
				'#markup' => 'Earlier comment<br>'
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
					'ddcl_osdag_response_step5_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_osdag_response_step5_form_next_submit'
				)
			);
		} //$step5_data = $form_step5_data->fetchObject()
		else
		{
			$form['step5_field1_fieldset']['step5_field1_question1'] = array(
				'#title' => '<strong>c. Should the vertical leg of the seated angle be checked against buckling</strong>',
				'#type' => 'radios',
				'#options' => $step5_options,
				//'#default_value' => $step5_data->question1,
				'#required' => true
			);
			$form['step5_field1_fieldset']['step5_field2_what_IS_code_clauses_and_references_should_be_followed'] = array(
				'#title' => 'If yes, what IS code clauses and references should be followed?',
				'#type' => 'textarea',
				//'#default_value' => $step5_data->if_yes,
				'#states' => array(
					'visible' => array(
						':input[name="step5_field1_fieldset\\[step5_field1_question1\\]"]' => array(
							array(
								'value' => t('Yes')
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
					'ddcl_osdag_response_step5_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_osdag_response_step5_form_next_submit'
				)
			);
		} //$step5_data = $form_step5_data->fetchObject()
	} //$form_step5_data
	return $form;
}
function ddcl_osdag_response_step5_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$form_state['redirect'] = 'ddcl-osdag-response/form/step4/' . $user->uid;
}
function ddcl_osdag_response_step5_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$query = db_select('ddcl_osdag_response_form_step5_feedback');
	$query->fields('ddcl_osdag_response_form_step5_feedback');
	$query->condition('uid', $user->uid);
	$form_step5_data = $query->execute()->fetchObject();
	//var_dump($form_step5_data);die;
	if ($form_step5_data)
	{
		$query = "UPDATE ddcl_osdag_response_form_step5_feedback SET question1=:question1, what_IS_code_clauses_and_references_should_be_followed=:if_yes, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step5_data->id
		));
		$form_step5_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step5_field1_fieldset']["step5_comment_count"]; $i++)
		{
			if ($v['step5_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_osdag_response_form_step5_feedback} (uid, question1,what_IS_code_clauses_and_references_should_be_followed, data_relation,inserted_on) VALUES (:uid,:question1,:if_yes, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-osdag-response/form/step6/' . $user->uid;
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
function ddcl_osdag_response_step6_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	// All the real form fields.
	$img_path = $base_url . '/' . drupal_get_path("module", "osdag_response_form") . '/images';
	$step6_options = array(
		'ISA 100 65x6 (include exact dim)' => 'ISA 100 65x6 (include exact dim)',
		'ISA 100 65x8 (include exact dim)' => 'ISA 100 65x8 (include exact dim)',
		'Any other recommendation' => t('Any other recommendation')
	);
	$step6_sub_options = array(
		'Based on Experience' => 'Based on Experience',
		'Based on Reasoning' => 'Based on Reasoning'
	);
	$form = array();
	$form['step6_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step6-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_osdag_response_form_step6_feedback');
	$query->fields('ddcl_osdag_response_form_step6_feedback');
	$query->condition('uid', $user->uid);
	$form_step6_data = $query->execute();
	if ($form_step6_data)
	{
		if ($step6_data = $form_step6_data->fetchObject())
		{
			$form['step6_field1_fieldset']['step6_field1_question1'] = array(
				'#title' => '<strong>e. Need guidelines for specifying Nominal Size of Top Cleat (/Clip) Angle</strong><br><br>
				<p><b>AISC suggests</b> : (Ref III - Page 10-84)<br>
						A 1/4-in.-thick angle with a 4-in, vertical leg dimension will generally be adequate 
							This translates to 6.35 mm thick angle with a 101.6 mm vertical leg Can use ISA 100 65x6 or ISA 100 65x8</p>
				<img src="' . $img_path . '/need-guidelines-for-specifying-nominal-size-of-top-cleat.png" style="width:80%;" />',
				'#type' => 'radios',
				'#options' => $step6_options,
				'#default_value' => $step6_data->question1,
				'#required' => true
			);
			$form['step6_field1_fieldset']['step6_field2_other_recomendation'] = array(
				'#title' => '',
				'#type' => 'radios',
				'#options' => $step6_sub_options,
				'#default_value' => $step6_data->sub_option2,
				'#states' => array(
					'visible' => array(
						':input[name="step6_field1_fieldset\\[step6_field1_question1\\]"]' => array(
							array(
								'value' => t('Any other recommendation')
							)
						)
					)
				)
			);
			$form['step6_field1_fieldset']['step6_field2_based_on_reasoning'] = array(
				'#title' => 'Comments/Reference',
				'#type' => 'textarea',
				'#default_value' => $step6_data->based_on_reasoning_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step6_field1_fieldset\\[step6_field2_other_recomendation\\]"]' => array(
							array(
								'value' => t('Based on Reasoning')
							)
						)
					)
				)
			);
			$earlier_comments = _fetch_exsiting_comments($step6_data->id, $step6_data->data_relation);
			$icom = 1;
			$form['step6_field1_fieldset']['exsiting_comments_title'] = array(
				'#markup' => 'Earlier comment<br>'
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
					'ddcl_osdag_response_step6_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_osdag_response_step6_form_next_submit'
				)
			);
		} //$step6_data = $form_step6_data->fetchObject()
		else
		{
			$form['step6_field1_fieldset']['step6_field1_question1'] = array(
				'#title' => '<strong>e. Need guidelines for specifying Nominal Size of Top Cleat (/Clip) Angle</strong><br><br>
				<p><b>AISC suggests</b> : (Ref III - Page 10-84)<br>
						A 1/4-in.-thick angle with a 4-in, vertical leg dimension will generally be adequate 
							This translates to 6.35 mm thick angle with a 101.6 mm vertical leg Can use ISA 100 65x6 or ISA 100 65x8</p>
				<img src="' . $img_path . '/need-guidelines-for-specifying-nominal-size-of-top-cleat.png" style="width:80%;" />',
				'#type' => 'radios',
				'#options' => $step6_options,
				//'#default_value' => $step6_data->question1,
				'#required' => true
			);
			$form['step6_field1_fieldset']['step6_field2_other_recomendation'] = array(
				'#title' => '',
				'#type' => 'radios',
				'#options' => $step6_sub_options,
				'#states' => array(
					'visible' => array(
						':input[name="step6_field1_fieldset\\[step6_field1_question1\\]"]' => array(
							array(
								'value' => t('Any other recommendation')
							)
						)
					)
				)
			);
			$form['step6_field1_fieldset']['step6_field2_based_on_reasoning'] = array(
				'#title' => 'Comments/Reference',
				'#type' => 'textarea',
				'#default_value' => $step6_data->any_other_recommendation,
				'#states' => array(
					'visible' => array(
						':input[name="step6_field1_fieldset\\[step6_field2_other_recomendation\\]"]' => array(
							array(
								'value' => t('Based on Reasoning')
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
					'ddcl_osdag_response_step6_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_osdag_response_step6_form_next_submit'
				)
			);
		} //$step6_data = $form_step6_data->fetchObject()
	} //$form_step6_data
	return $form;
}
function ddcl_osdag_response_step6_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$form_state['redirect'] = 'ddcl-osdag-response/form/step5/' . $user->uid;
}
function ddcl_osdag_response_step6_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$query = db_select('ddcl_osdag_response_form_step6_feedback');
	$query->fields('ddcl_osdag_response_form_step6_feedback');
	$query->condition('uid', $user->uid);
	$form_step6_data = $query->execute()->fetchObject();
	//var_dump($form_step6_data);die;
	if ($form_step6_data)
	{
		$query = "UPDATE ddcl_osdag_response_form_step6_feedback SET question1=:question1, sub_option2 =:sub_option2, based_on_reasoning_comment=:based_on_reasoning_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step6_field1_fieldset']['step6_field1_question1'],
			":sub_option2" => $v['step6_field1_fieldset']['step6_field2_other_recomendation'],
			":based_on_reasoning_comment" => $v['step6_field1_fieldset']['step6_field2_based_on_reasoning'],
			":data_relation" => "step6"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step6_data->id
		));
		$form_step6_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step6_field1_fieldset']["step6_comment_count"]; $i++)
		{
			if ($v['step6_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_osdag_response_form_step6_feedback} (uid, question1, sub_option2, based_on_reasoning_comment, data_relation, inserted_on) VALUES (:uid,:question1, :sub_option2, :based_on_reasoning_comment, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step6_field1_fieldset']['step6_field1_question1'],
			":sub_option2" => $v['step6_field1_fieldset']['step6_field2_other_recomendation'],
			":based_on_reasoning_comment" => $v['step6_field1_fieldset']['step6_field2_based_on_reasoning'],
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
	INSERT INTO ddcl_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-osdag-response/form/step7/' . $user->uid;
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
function ddcl_osdag_response_step7_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	// All the real form fields.
	$img_path = $base_url . '/' . drupal_get_path("module", "osdag_response_form") . '/images';
	$step7_options = array(
		'10mm' => '10mm' . '
					<p>Reference: Assume "gap" of 10 mm (Ref II - 12:00 min)
</p>
					<img src="' . $img_path . '/10mm.png" />
					<p>Reference: The clearance between the end of the beam and flanges of the
column should be minimum (between 2 and 5 mm) so that maximum
bearing length is available (Ref I - 5.7.3 page 367)
</p>
					<img src="' . $img_path . '/10mmsub.png" />',
		'Any other recommendation' => 'Any other recommendation'
	);
	$form = array();
	$form['step7_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step7-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_osdag_response_form_step7_feedback');
	$query->fields('ddcl_osdag_response_form_step7_feedback');
	$query->condition('uid', $user->uid);
	$form_step7_data = $query->execute();
	if ($form_step7_data)
	{
		if ($step7_data = $form_step7_data->fetchObject())
		{
			$form['step7_field1_fieldset']['step7_field1_question1'] = array(
				'#title' => 'The (assumed) clearance + tolerance between the beam and supporting (column or girder) will
be taken as input from the user. What should be the minimum value for this?
Ref II takes clearance = 10 mm
Ref I takes (clearance + tolerance) = 5 mm + 5 mm = 10mm [page 407, Ref I]',
				'#type' => 'radios',
				'#options' => $step7_options,
				'#default_value' => $step7_data->question1,
				'#required' => true
			);
			$form['step7_field1_fieldset']['step7_field2_any_other'] = array(
				'#type' => 'textarea',
				'#default_value' => $step7_data->any_other_recommendation,
				'#states' => array(
					'visible' => array(
						':input[name="step7_field1_fieldset\\[step7_field1_question1\\]"]' => array(
							array(
								'value' => t('Any other recommendation')
							)
						)
					)
				)
			);
			$earlier_comments = _fetch_exsiting_comments($step7_data->id, $step7_data->data_relation);
			$icom = 1;
			$form['step7_field1_fieldset']['exsiting_comments_title'] = array(
				'#markup' => 'Earlier comment<br>'
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
					'ddcl_osdag_response_step7_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_osdag_response_step7_form_next_submit'
				)
			);
		} //$step7_data = $form_step7_data->fetchObject()
		else
		{
			$form['step7_field1_fieldset']['step7_field1_question1'] = array(
				'#title' => 'The (assumed) clearance + tolerance between the beam and supporting (column or girder) will
be taken as input from the user. What should be the minimum value for this?
Ref II takes clearance = 10 mm
Ref I takes (clearance + tolerance) = 5 mm + 5 mm = 10mm [page 407, Ref I]',
				'#type' => 'radios',
				'#options' => $step7_options,
				//'#default_value' => $step7_data->question1,
				'#required' => true
			);
			$form['step7_field1_fieldset']['step7_field2_any_other'] = array(
				'#type' => 'textarea',
				'#states' => array(
					'visible' => array(
						':input[name="step7_field1_fieldset\\[step7_field1_question1\\]"]' => array(
							array(
								'value' => t('Any other recommendation')
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
					'ddcl_osdag_response_step7_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_osdag_response_step7_form_next_submit'
				)
			);
		} //$step7_data = $form_step7_data->fetchObject()
	} //$form_step7_data
	return $form;
}
function ddcl_osdag_response_step7_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$form_state['redirect'] = 'ddcl-osdag-response/form/step6/' . $user->uid;
}
function ddcl_osdag_response_step7_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$query = db_select('ddcl_osdag_response_form_step7_feedback');
	$query->fields('ddcl_osdag_response_form_step7_feedback');
	$query->condition('uid', $user->uid);
	$form_step7_data = $query->execute()->fetchObject();
	//var_dump($form_step7_data);die;
	if ($form_step7_data)
	{
		$query = "UPDATE ddcl_osdag_response_form_step7_feedback SET question1=:question1, any_other_recommendation=:any_other_recommendation, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step7_field1_fieldset']['step7_field1_question1'],
			":any_other_recommendation" => $v['step7_field1_fieldset']['step7_field2_any_other'],
			":data_relation" => "step7"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step7_data->id
		));
		$form_step7_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step7_field1_fieldset']["step7_comment_count"]; $i++)
		{
			if ($v['step7_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_osdag_response_form_step7_feedback} (uid, question1, any_other_recommendation, data_relation,inserted_on) VALUES (:uid,:question1,:any_other_recommendation, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step7_field1_fieldset']['step7_field1_question1'],
			":any_other_recommendation" => $v['step7_field1_fieldset']['step7_field2_any_other'],
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
	INSERT INTO ddcl_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-osdag-response/form/step8/' . $user->uid;
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
/***************************************** step 8 form starts **************************/
function ddcl_osdag_response_step8_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	// All the real form fields.
	$form = array();
	$form['step8_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#title' => '<strong>Any other recommendations/ Comments?</strong>',
		'#tree' => TRUE,
		'#prefix' => '<div id="step8-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_osdag_response_form_step8_feedback');
	$query->fields('ddcl_osdag_response_form_step8_feedback');
	$query->condition('uid', $user->uid);
	$form_step8_data = $query->execute();
	if ($form_step8_data)
	{
		if ($step8_data = $form_step8_data->fetchObject())
		{
			$form['step8_field1_fieldset']['step8_field1_question1'] = array(
				'#type' => 'hidden',
				'#default_value' => 'other recommendations/ Comments',
				'#required' => TRUE
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
			$form['previous2'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_osdag_response_step8_form_previous_submit'
				)
			);
			$form['next3'] = array(
				'#type' => 'submit',
				'#value' => 'Finish',
				'#submit' => array(
					'ddcl_osdag_response_step8_form_next_submit'
				)
			);
		} //$step8_data = $form_step8_data->fetchObject()
		else
		{
			$form['step8_field1_fieldset']['step8_field1_question1'] = array(
				'#type' => 'hidden',
				'#default_value' => 'other recommendations/ Comments',
				'#required' => TRUE
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
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_osdag_response_step8_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Finish',
				'#submit' => array(
					'ddcl_osdag_response_step8_form_next_submit'
				)
			);
		} //$step8_data = $form_step8_data->fetchObject()
	} //$form_step8_data
	return $form;
}
function ddcl_osdag_response_step8_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$form_state['redirect'] = 'ddcl-osdag-response/form/step6/' . $user->uid;
}
function ddcl_osdag_response_step8_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$query = db_select('ddcl_osdag_response_form_step8_feedback');
	$query->fields('ddcl_osdag_response_form_step8_feedback');
	$query->condition('uid', $user->uid);
	$form_step7_data = $query->execute()->fetchObject();
	//var_dump($form_step7_data);die;
	if ($form_step7_data)
	{
		$query = "UPDATE ddcl_osdag_response_form_step8_feedback SET question1=:question1, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step8_field1_fieldset']['step8_field1_question1'],
			":data_relation" => "step8"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step7_data->id
		));
		$form_step7_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step8_field1_fieldset']["step8_comment_count"]; $i++)
		{
			if ($v['step8_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step7_data->id,
					":number" => $i + $form_step7_comment_data->max + 1,
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
	} //$form_step7_data
	else
	{
		$query = "INSERT INTO {ddcl_osdag_response_form_step8_feedback} (uid, question1, data_relation,inserted_on) VALUES (:uid,:question1, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step8_field1_fieldset']['step8_field1_question1'],
			":data_relation" => "step8",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_osdag_response_form_step1_feedback SET submitted=:submitted WHERE uid=:uid ";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $$args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$comments = 0;
		for ($i = 0; $i <= $v['step8_field1_fieldset']["step8_comment_count"]; $i++)
		{
			if ($v['step8_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-osdag-response/success';
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
/***************************************** step 8 form ends **************************/
function ddcl_osdag_response_result_page()
{
	/* get pending proposals to be approved */
	$output .= "";
	$output .= l("Download All Result in CSV", "ddcl-osdag-response/form/result/download");
	$feedback_rows = array();
	$feedback_q = db_query('

select

step1.uid as "step1_uid",
step1.name as "step1_name",
step1.inserted_on as "step1_date",

step2.id as "step2_id",
step2.data_relation as "step2_data_relation",
step2.question1 as "step2_question1", 
step2.any_other_recommendation as "step2_any_other",

step3.id as "step3_id",
step3.data_relation as "step3_data_relation",
step3.question1 as "step3_question1", 
step3.question2 as "step3_question2",

step4.id as "step4_id",
step4.data_relation as "step4_data_relation",
step4.question1 as "step4_question1",

step5.id as "step5_id",
step5.data_relation as "step5_data_relation",
step5.question1 as "step5_question1", 
step5.what_IS_code_clauses_and_references_should_be_followed as "step5_question2",

step6.id as "step6_id",
step6.data_relation as "step6_data_relation",
step6.question1 as "step6_question1", 
step6.sub_option2 as "step6_question2",
step6.based_on_reasoning_comment as "step6_question3",

step7.id as "step7_id",
step7.data_relation as "step7_data_relation",
step7.question1 as "step7_question1",
step7.any_other_recommendation as "step7_question2",

step8.id as "step8_id",
step8.data_relation as "step8_data_relation",
step8.question1 as "step8_question1"

from ddcl_osdag_response_form_step1_feedback step1

join ddcl_osdag_response_form_step2_feedback step2 on step1.uid=step2.uid
join ddcl_osdag_response_form_step3_feedback step3 on step2.uid=step3.uid
join ddcl_osdag_response_form_step4_feedback step4 on step3.uid=step4.uid
join ddcl_osdag_response_form_step5_feedback step5 on step4.uid=step5.uid
join ddcl_osdag_response_form_step6_feedback step6 on step5.uid=step6.uid
join ddcl_osdag_response_form_step7_feedback step7 on step6.uid=step7.uid
join ddcl_osdag_response_form_step8_feedback step8 on step7.uid=step8.uid
	');
	$i = 1;
	
	while ($feedback_data = $feedback_q->fetchObject())
	{

		$feedback_rows[$feedback_data->uid] = array(
			$i,

			$feedback_data->step1_date,
			$feedback_data->step1_name,

			$feedback_data->step2_question1,
			$feedback_data->step2_any_other,
			_fetch_exsiting_comments_result($feedback_data->step2_id, $feedback_data->step2_data_relation),

			$feedback_data->step3_question1,
			$feedback_data->step3_question2,
			_fetch_exsiting_comments_result($feedback_data->step3_id, $feedback_data->step3_data_relation),

			$feedback_data->step4_question1,
			_fetch_exsiting_comments_result($feedback_data->step4_id, $feedback_data->step4_data_relation),

			$feedback_data->step5_question1,
			$feedback_data->step5_question2,
			_fetch_exsiting_comments_result($feedback_data->step5_id, $feedback_data->step5_data_relation),

			$feedback_data->step6_question1,
			$feedback_data->step6_question2,
			$feedback_data->step6_question3,
			_fetch_exsiting_comments_result($feedback_data->step6_id, $feedback_data->step6_data_relation),

			$feedback_data->step7_question1,
			$feedback_data->step7_question2,
			_fetch_exsiting_comments_result($feedback_data->step7_id, $feedback_data->step7_data_relation),

			$feedback_data->step8_question1,
			_fetch_exsiting_comments_result($feedback_data->step8_id, $feedback_data->step8_data_relation)
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
		'User',

		'a. What should be the minimum length of the connected leg (to the support) of the seated angle?',
		'Reasoning (including reference from literature):',
		'comments',
		'b. What should be the minimum thickness of the seated angle?',
		'What code provision(/s)/ reference text should be referred for this?',
		'comments',
		'c. The moment capacity of the outstanding leg should be based on',
		'comments',
		'd. Should the vertical leg of the seated angle be checked against buckling?',
		'If yes, what IS code clauses and references should be followed?',
		'comments',
		'e. Need guidelines for specifying Nominal Size of Top Cleat (/Clip) Angle',
		'Based on Reasoning',
		'Comments /references',
		'comments',
		'The (assumed) clearance + tolerance between the beam and supporting (column or girder) will
be taken as input from the user. What should be the minimum value for this?',
		'Any other',
		'comments',
		'Any other recomendation',
		'comments'
	);
	$output .= theme('table', array(
		'header' => $data_header,
		'rows' => $feedback_rows,
		'sticky' => FALSE
	));
	return $output;
}
function _fetch_exsiting_comments($step_id, $data_relation)
{
	$query_for_comment = db_query("SELECT * FROM ddcl_feedback_form_comment WHERE step1_form_id = :step1_form_id AND data_relation=:data_relation ORDER BY number ASC", array(
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
function _fetch_exsiting_comments_result($step_id, $data_relation)
{

	$query_for_comment = db_query("SELECT * FROM ddcl_feedback_form_comment WHERE step1_form_id = :step1_form_id AND data_relation=:data_relation ORDER BY number ASC", array(
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

function _fetch_exsiting_comments_result_csv($step_id, $data_relation)
{

	$query_for_comment = db_query("SELECT * FROM ddcl_feedback_form_comment WHERE step1_form_id = :step1_form_id AND data_relation=:data_relation ORDER BY number ASC", array(
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

function ddcl_result_generate_csv()
{
	global $user;
	if ($user->uid !=NULL)
	{
		$result_dir = $_SERVER['DOCUMENT_ROOT'] . base_path() . "osdag-response-form-result/{$user->uid}/";
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

			'a. What should be the minimum length of the connected leg (to the support) of the seated angle?',
			'Reasoning (including reference from literature):',
			'comments',
			'b. What should be the minimum thickness of the seated angle?',
			'What code provision(/s)/ reference text should be referred for this?',
			'comments',
			'c. The moment capacity of the outstanding leg should be based on',
			'comments',
			'd. Should the vertical leg of the seated angle be checked against buckling?',
			'If yes, what IS code clauses and references should be followed?',
			'comments',
			'e. Need guidelines for specifying Nominal Size of Top Cleat (/Clip) Angle',
			'Based on Reasoning',
			'Comments /references',
			'comments',
			'The (assumed) clearance + tolerance between the beam and supporting (column or girder) will
be taken as input from the user. What should be the minimum value for this?',
			'Any other',
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
step2.any_other_recommendation as "step2_any_other",

step3.id as "step3_id",
step3.data_relation as "step3_data_relation",
step3.question1 as "step3_question1", 
step3.question2 as "step3_question2",

step4.id as "step4_id",
step4.data_relation as "step4_data_relation",
step4.question1 as "step4_question1",

step5.id as "step5_id",
step5.data_relation as "step5_data_relation",
step5.question1 as "step5_question1", 
step5.what_IS_code_clauses_and_references_should_be_followed as "step5_question2",

step6.id as "step6_id",
step6.data_relation as "step6_data_relation",
step6.question1 as "step6_question1", 
step6.sub_option2 as "step6_question2",
step6.based_on_reasoning_comment as "step6_question3",

step7.id as "step7_id",
step7.data_relation as "step7_data_relation",
step7.question1 as "step7_question1",
step7.any_other_recommendation as "step7_question2",

step8.id as "step8_id",
step8.data_relation as "step8_data_relation",
step8.question1 as "step8_question1"

from ddcl_osdag_response_form_step1_feedback step1

join ddcl_osdag_response_form_step2_feedback step2 on step1.uid=step2.uid
join ddcl_osdag_response_form_step3_feedback step3 on step2.uid=step3.uid
join ddcl_osdag_response_form_step4_feedback step4 on step3.uid=step4.uid
join ddcl_osdag_response_form_step5_feedback step5 on step4.uid=step5.uid
join ddcl_osdag_response_form_step6_feedback step6 on step5.uid=step6.uid
join ddcl_osdag_response_form_step7_feedback step7 on step6.uid=step7.uid
join ddcl_osdag_response_form_step8_feedback step8 on step7.uid=step8.uid
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
				$feedback_data->step2_any_other,
				_fetch_exsiting_comments_result_csv($feedback_data->step2_id, $feedback_data->step2_data_relation),
				
				$feedback_data->step3_question1,
				$feedback_data->step3_question2,
				_fetch_exsiting_comments_result_csv($feedback_data->step3_id, $feedback_data->step3_data_relation),
				
				$feedback_data->step4_question1,
				_fetch_exsiting_comments_result_csv($feedback_data->step4_id, $feedback_data->step4_data_relation),
				
				$feedback_data->step5_question1,
				$feedback_data->step5_question2,
				_fetch_exsiting_comments_result_csv($feedback_data->step5_id, $feedback_data->step5_data_relation),
				
				$feedback_data->step6_question1,
				$feedback_data->step6_question2,
				$feedback_data->step6_question3,
				_fetch_exsiting_comments_result_csv($feedback_data->step6_id, $feedback_data->step6_data_relation),
				
				$feedback_data->step7_question1,
				$feedback_data->step7_question2,
				_fetch_exsiting_comments_result_csv($feedback_data->step7_id, $feedback_data->step7_data_relation),
				
				$feedback_data->step8_question1,
				_fetch_exsiting_comments_result_csv($feedback_data->step8_id, $feedback_data->step8_data_relation)
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
