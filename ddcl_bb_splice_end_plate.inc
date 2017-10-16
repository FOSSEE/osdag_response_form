<?php
/**
 * Fetches and displays the form.
 */
function ddcl_bb_splice_end_plate_form_page()
{
	global $user;
	$data = '';
	if (!$user)
	{
		$data .= "Please login to give feedback";
	} //!$user
	/************* check if user has already submitted a proposal **************/
	$query = db_select('ddcl_bb_splice_end_plate_form_step1_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step1_feedback');
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
				$ddcl_bb_splice_end_plate_step1_form = drupal_get_form("ddcl_bb_splice_end_plate_step1_form");
				$data .= drupal_render($ddcl_bb_splice_end_plate_step1_form);
			}
		} //$feedback_data = $feedback_q->fetchObject()
		else
		{
			$ddcl_bb_splice_end_plate_step1_form = drupal_get_form("ddcl_bb_splice_end_plate_step1_form");
			$data .= drupal_render($ddcl_bb_splice_end_plate_step1_form);
		}
	} //$feedback_q
	//$data .= drupal_get_form('ddcl_bb_splice_end_plate_step1_form');
	return $data;
}
function ddcl_bb_splice_end_plate_step1_form($form, &$form_state, $no_js_use = FALSE)
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
	$query = db_select('ddcl_bb_splice_end_plate_form_step1_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step1_feedback');
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
			'ddcl_bb_splice_end_plate_step1_form_submit'
		)
	);
	return $form;
}
function ddcl_bb_splice_end_plate_step1_form_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$query = db_select('ddcl_bb_splice_end_plate_form_step1_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step1_feedback');
	$query->condition('uid', $user->uid);
	$form_step1_data = $query->execute();
	if ($form_step1_data)
	{
		if ($step1_data = $form_step1_data->fetchObject())
		{
			$query = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET name=:name, email_id=:email_id, institute=:institute, current_position=:current_position WHERE uid=:uid ";
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
			$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step1_feedback} (uid, name, email_id, institute, current_position, data_reletion, inserted_on) VALUES (:uid,:name,:email_id, :institute, :current_position, :data_reletion,:inserted_on)";
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
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step2/' . $user->uid;
}
/*************************************************************************/
/************************ Step 2 form starts *********************/
function ddcl_bb_splice_end_plate_step2_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('a. Number of bolts (N) > i. number of bolts in each column'));
	$markup_text = '
	<b> <strong> a. Number of bolts (N) </strong></br>
	
	<hr>
		&emsp; <b> i. Number of bolts in each column (n) </b> [Reference: equation 5.59, Design of steel structures by N. Subramanian]
<br>
		&emsp;&emsp;&emsp; <b> Assumption: </b> The configuration is assumed to have 2 columns of bolt.
<br>					
<br>
		<center>
				
		<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <mi>n</mi>
  <mo>=</mo>
  <msqrt>
    <mfrac>
      <mrow>
        <mn>6</mn>
        <mi>M</mi>
      </mrow>
      <mrow>
        <math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msup>
    <mi>n</mi>
    <mo>&#x2032;</mo>
  </msup>
        <mi>p</mi>
        <msub>
          <mi>V</mi>
          <mrow class="MJX-TeXAtom-ORD">
            <mi>s</mi>
            <mi>d</mi>
          </mrow>
        </msub>
      </mrow>
    </mfrac>
  </msqrt>
</math>
		</center>		
<br>

&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					where, <br>
					&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
						<math xmlns="http://www.w3.org/1998/Math/MathML">
						  <msub>
							<mi>M</mi>
							<mrow class="MJX-TeXAtom-ORD">
						  </msub>
						</math> - Applied moment (External factored moment + Moment due to factored axial force) kN-mm <br>

					&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
						<math xmlns="http://www.w3.org/1998/Math/MathML">
  							<msup>
    							<mi>n</mi>
    							<mo>&#x2032;</mo>
  							</msup>
						</math> - number of columns of bolts (assumed to be 2) <br>

					&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
						<math xmlns="http://www.w3.org/1998/Math/MathML">
						  <msub>
							<mi>V</mi>
							<mrow class="MJX-TeXAtom-ORD">
							  <mi>s</mi>
							  <mi>d</mi>
							</mrow>
						  </msub>
						</math> - design shear strength of a single bolt (kN) <br>

					&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
						<math xmlns="http://www.w3.org/1998/Math/MathML">
						  <msub>
							<mi>p</mi>
							<mrow class="MJX-TeXAtom-ORD">
							</mrow>
						  </msub>
						</math> - minimum pitch distance i.e. <math xmlns="http://www.w3.org/1998/Math/MathML" 
 							 <mn>2.5</mn>
  							<mi>d</mi>
							</math> (mm) <br>
				<br>

				&emsp;&emsp;&emsp;  Therefore, the total number of bolts (N) is,
				<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
					  <mi>N</mi>
					  <mo>=</mo>
  					  <mn>2</mn>
					  <mi>n</mi>
				</math>		

					
			<hr>';
	$form = array();
	$form['step2_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step2-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_bb_splice_end_plate_form_step2_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step2_feedback');
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
			$earlier_comments = _fetch_existing_ddcl_bb_splice_end_plate_comments($step2_data->id, $step2_data->data_relation);
			$icom = 1;
			$form['step2_field1_fieldset']['existing_comments_title'] = array(
				'#markup' => '<hr><strong>Earlier comments:</strong><hr><br>'
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step2_field1_fieldset']['existing_comments' . $icom] = array(
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
					'ddcl_bb_splice_end_plate_step2_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step2_form_next_submit'
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
					'ddcl_bb_splice_end_plate_step2_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step2_form_next_submit'
				)
			);
		} //$step2_data = $form_step2_data->fetchObject()
	} //$form_step2_data
	return $form;
}
function ddcl_bb_splice_end_plate_step2_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step2_field1_fieldset']['step2_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step2_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step2_feedback');
	$query->condition('uid', $user->uid);
	$form_step2_data = $query->execute()->fetchObject();
	if ($form_step2_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step2_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step2_data->id
		));
		$form_step2_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step2_field1_fieldset']["step2_comment_count"]; $i++)
		{
			if ($v['step2_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step2_feedback} (uid, question1,question_ok_not_ok,question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1,:question_ok_not_ok,:question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step1/' . $user->uid;
}
function ddcl_bb_splice_end_plate_step2_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step2_field1_fieldset']['step2_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step2_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step2_feedback');
	$query->condition('uid', $user->uid);
	$form_step2_data = $query->execute()->fetchObject();
	if ($form_step2_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step2_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step2_data->id
		));
		$form_step2_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step2_field1_fieldset']["step2_comment_count"]; $i++)
		{
			if ($v['step2_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step2_feedback} (uid, question1,question_ok_not_ok,question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1,:question_ok_not_ok,:question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step3/' . $user->uid;
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
function ddcl_bb_splice_end_plate_step3_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	$img_path = $base_url . '/' . drupal_get_path("module", "osdag_response_form") . '/images';
	drupal_set_title(t('a. Number of bolts > ii. Selecting configuration based on number of bolts > 1. N=8 > 2. N=12 > 3. N=16'));
	$markup_text = '
	<b> <strong> a. Number of bolts  </strong> </b>
	<hr>
	&emsp; <b> <strong> ii. Selecting configuration based on number of bolts </strong> </b> <br>	
	<br>
	&emsp;&emsp;&emsp;&emsp; <b> 1. If number of bolts; N &le; 8, take N = 8 (Figure 1)
	</b><br>
	&emsp;&emsp;&emsp;&emsp; <b> 2. If number of bolts; 8 &lt N &le; 12, take N = 12 (Figure 2)
	</b><br>
	&emsp;&emsp;&emsp;&emsp; <b> 3. If number of bolts; 12 &lt N &le; 16, take N = 16 (Figure 3)
	<br>
	<br>
			
			&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
			&emsp;&emsp;&emsp;&emsp;&emsp;
			<img src="' . $img_path . '/4_bolts.png" width="25%" height="30%" align="centre"/><br>
			&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
			&emsp;&emsp;&emsp;
			<b>Figure 1:</b> configuration with 8 bolts (unstiffened and stiffened)
			<hr>
			

			&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
			&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
			<img src="' . $img_path . '/MRE_1_2.png" width="15%" height="20%" align="centre"/><br>
			
			&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
			&emsp;&emsp;&emsp;
			&emsp;&emsp;<b>Figure 2:</b> configuration with 12 bolts 
			<hr>
			

			&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
			
			<img src="' . $img_path . '/8_bolts.png" width="25%" height="30%" align="centre"/><br>
			<br>
			&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
			&emsp;&emsp;&emsp;
			<b>Figure 3:</b> configuration with 16 bolts (unstiffened and stiffened)
			

';
	$markup_text1 = '';
	$tooltip1 = '';
	// All the real form fields.
	$form = array();
	$form['step3_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step3-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_bb_splice_end_plate_form_step3_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step3_feedback');
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
			$earlier_comments = _fetch_existing_ddcl_bb_splice_end_plate_comments($step3_data->id, $step3_data->data_relation);
			$icom = 1;
			$form['step3_field1_fieldset']['existing_comments_title'] = array(
				'#markup' => '<hr><strong>Earlier comment</strong><hr><br>'
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step3_field1_fieldset']['existing_comments' . $icom] = array(
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
					'ddcl_bb_splice_end_plate_step3_form_previous_submit'
				)
			);
			$form['next3'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step3_form_next_submit'
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
					'ddcl_bb_splice_end_plate_step3_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step3_form_next_submit'
				)
			);
		} //$step3_data = $form_step3_data->fetchObject()
	} //$form_step3_data
	return $form;
}
function ddcl_bb_splice_end_plate_step3_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step3_field1_fieldset']['step3_field1_question1'] = 'question 3';
	$query = db_select('ddcl_bb_splice_end_plate_form_step3_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step3_feedback');
	$query->condition('uid', $user->uid);
	$form_step3_data = $query->execute()->fetchObject();
	//var_dump($form_step2_data);die;
	if ($form_step3_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step3_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step3_data->id
		));
		$form_step3_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step3_field1_fieldset']["step3_comment_count"]; $i++)
		{
			if ($v['step3_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step3_feedback} (uid, question1,question_ok_not_ok,question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok,:question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step2/' . $user->uid;
}
function ddcl_bb_splice_end_plate_step3_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step3_field1_fieldset']['step3_field1_question1'] = 'question 3';
	$query = db_select('ddcl_bb_splice_end_plate_form_step3_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step3_feedback');
	$query->condition('uid', $user->uid);
	$form_step3_data = $query->execute()->fetchObject();
	//var_dump($form_step2_data);die;
	if ($form_step3_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step3_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step3_data->id
		));
		$form_step3_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step3_field1_fieldset']["step3_comment_count"]; $i++)
		{
			if ($v['step3_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step3_feedback} (uid, question1,question_ok_not_ok,question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok,:question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step4/' . $user->uid;
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
function ddcl_bb_splice_end_plate_step4_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('b. Material strength > i. Yield stress limits > ii. Ultimate strength limits'));
	$markup_text = '
	<b> <strong> b. Material Strength </strong></b><br>
	<hr>
	&emsp; <b> The values of material strength will be subjected to the following limits <br>
	<br>
		&emsp; <b> i. Yield Stress limits </b>[Reference: Table-1, IS 800] <br>					
					<center>
					<math xmlns="http://www.w3.org/1998/Math/MathML">
					<mn>165</mn>
					<mo>&le;</mo>
					<msub>
						<mi>f</mi>
						<mi>y</mi>					  
					</msub>
					<mo>&le;</mo>
					<mn>450</mn>
					</math>
					</center>		
		<br>
		&emsp; <b> ii. Ultimate Strength limits </b> [Reference: Table-1, IS 800] <br>
					<center>
					<math xmlns="http://www.w3.org/1998/Math/MathML">
					<mn>290</mn>
					<mo>&le;</mo>
					<msub>
						<mi>f</mi>
						<mi>u</mi>
					</msub>
					<mo>&le;</mo>
					<mn>590</mn>
					</math>
					</center>					
	<hr>';
	// All the real form fields.
	$form = array();
	$form['step4_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step4-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_bb_splice_end_plate_form_step4_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step4_feedback');
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
			$earlier_comments = _fetch_existing_ddcl_bb_splice_end_plate_comments($step4_data->id, $step4_data->data_relation);
			$icom = 1;
			$form['step4_field1_fieldset']['existing_comments_title'] = array(
				'#markup' => '<hr>Earlier comment<hr><br>'
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step4_field1_fieldset']['existing_comments' . $icom] = array(
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
					'ddcl_bb_splice_end_plate_step4_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step4_form_next_submit'
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
					'ddcl_bb_splice_end_plate_step4_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step4_form_next_submit'
				)
			);
		} //$step4_data = $form_step4_data->fetchObject()
	} //$form_step4_data
	return $form;
}
function ddcl_bb_splice_end_plate_step4_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step4_field1_fieldset']['step4_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step4_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step4_feedback');
	$query->condition('uid', $user->uid);
	$form_step4_data = $query->execute()->fetchObject();
	//var_dump($form_step4_data);die;
	if ($form_step4_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step4_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step4_data->id
		));
		$form_step4_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step4_field1_fieldset']["step4_comment_count"]; $i++)
		{
			if ($v['step4_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step4_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step3/' . $user->uid;
}
function ddcl_bb_splice_end_plate_step4_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step4_field1_fieldset']['step4_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step4_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step4_feedback');
	$query->condition('uid', $user->uid);
	$form_step4_data = $query->execute()->fetchObject();
	//var_dump($form_step4_data);die;
	if ($form_step4_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step4_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step4_data->id
		));
		$form_step4_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step4_field1_fieldset']["step4_comment_count"]; $i++)
		{
			if ($v['step4_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step4_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step5/' . $user->uid;
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
function ddcl_bb_splice_end_plate_step5_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	$img_path = $base_url . '/' . drupal_get_path("module", "osdag_response_form") . '/images';
	drupal_set_title(t('c. Detailing checks > i. Pitch distance > ii. Gauge distance > iii. Gauge1 distance'));
	$markup_text = '
	<b><strong> c. Detailing checks </strong></b> <br>
	<hr>
	&emsp; <b><strong> i. Pitch distance (p) </strong></b> [Reference: Cl.10.2.2, IS 800] <br>	
	&emsp;
	&emsp;&emsp;&emsp; This check will be the same as implemented for the shear connection modules (already reviewed).

	<br>
	<hr>
	
	&emsp; <b><strong> ii. Gauge distance (g) </strong></b> [Reference: Cl.10.2.3.1, IS 800] <br>
	&emsp;
	&emsp;&emsp;&emsp; This check will be the same as implemented for the shear connection modules (already reviewed).
	
	<br>
	<hr>
	
	&emsp; <b><strong> iii. Gauge1 distance (<mi>g</mi> <mi>1</mi>) </strong></b> [Reference: SCI - Steel Designers Manual, page 733, 6th Edition (2003)] <br>
	&emsp;
	&emsp;&emsp;&emsp; Gauge1  (also called cross centre gauge distance) values will be subject to the following limits (in mm):
	<br>
		<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
			<msub>
				<mi>g</mi>
				<mi>1</mi>
			</msub>
			<mo>&gt;</mo>
			<mn>90</mn>
		</math>
		
		<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
			<msub>
				<mi>g</mi>
				<mi>1</mi>
			</msub>
			<mo>&lt;</mo>
			<mn>140</mn>
		</math>
		
		&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
		where, <br>
			&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
			<math xmlns="http://www.w3.org/1998/Math/MathML">
				<msub>
					<mi>g</mi>
					<mi>1</mi>
				</msub>			
			</math> - gauge1 <br><br>
			
			
			&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
			&emsp;&emsp;&emsp;&emsp;&emsp;
			<img src="' . $img_path . '/g_1.png" width="30%" height="50%" align="middle"/><br>
			&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
			&emsp;&emsp;<b><strong>Figure:</strong></b> beam to beam end plate splice connection showing gauge1 distance<br>
	
	<hr>';
	// All the real form fields.
	$form = array();
	$form['step5_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step5-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_bb_splice_end_plate_form_step5_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step5_feedback');
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
			$earlier_comments = _fetch_existing_ddcl_bb_splice_end_plate_comments($step5_data->id, $step5_data->data_relation);
			$icom = 1;
			$form['step5_field1_fieldset']['existing_comments_title'] = array(
				'#markup' => '<hr><strong>Earlier comment</strong><hr><br>'
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step5_field1_fieldset']['existing_comments' . $icom] = array(
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
					'ddcl_bb_splice_end_plate_step5_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step5_form_next_submit'
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
					'ddcl_bb_splice_end_plate_step5_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step5_form_next_submit'
				)
			);
		} //$step5_data = $form_step5_data->fetchObject()
	} //$form_step5_data
	return $form;
}
function ddcl_bb_splice_end_plate_step5_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step5_field1_fieldset']['step5_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step5_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step5_feedback');
	$query->condition('uid', $user->uid);
	$form_step5_data = $query->execute()->fetchObject();
	//var_dump($form_step5_data);die;
	if ($form_step5_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step5_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step5_data->id
		));
		$form_step5_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step5_field1_fieldset']["step5_comment_count"]; $i++)
		{
			if ($v['step5_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step5_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation, inserted_on) VALUES (:uid, :question1, :question_ok_not_ok,:question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step4/' . $user->uid;
}
function ddcl_bb_splice_end_plate_step5_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step5_field1_fieldset']['step5_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step5_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step5_feedback');
	$query->condition('uid', $user->uid);
	$form_step5_data = $query->execute()->fetchObject();
	//var_dump($form_step5_data);die;
	if ($form_step5_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step5_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step5_data->id
		));
		$form_step5_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step5_field1_fieldset']["step5_comment_count"]; $i++)
		{
			if ($v['step5_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step5_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation, inserted_on) VALUES (:uid, :question1, :question_ok_not_ok,:question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step6/' . $user->uid;
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
function ddcl_bb_splice_end_plate_step6_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	$img_path = $base_url . '/' . drupal_get_path("module", "osdag_response_form") . '/images';
	drupal_set_title(t('c. Detailing checks > iv. End distance > v. Edge distance > vi. lv'));
	$markup_text = '
	<b> <strong> c. Detailing checks </strong></b> <br>
	<hr>
	&emsp; <b><strong> iv. End distance (e<sub>1</sub>) </strong></b> [Reference: Cl.10.2.4.2 & 10.2.4.3, IS 800] <br>	
	&emsp;
	&emsp;&emsp;&emsp; This check will be the same as implemented for the shear connection modules (already reviewed).

	<br>
	<hr>
	
	&emsp; <b><strong> v. Edge distance (e<sub>2</sub>) </strong></b> [Reference: Cl.10.2.4.2 & 10.2.4.3, IS 800] <br>
	&emsp;
	&emsp;&emsp;&emsp; This check will be the same as implemented for the shear connection modules (already reviewed).
	
	<br>
	<hr>
	
	&emsp; <b><strong> vi. Distance between the beam flange or toe of weld to the centre of the nearer bolt ( l<sub>v</sub> ) </strong></b> [Reference: AISC design guide 16] <br>
	&emsp;
	&emsp;&emsp;&emsp; l<sub>v</sub> values will be subject to the following limits (in mm):
	<br>
	<br>
	&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp; <b> 1. For bolts upto 24 mm diameter </b>
	&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
	<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
			<msub>
				<mi>l</mi>
				<mi>v</mi>
			</msub>
			<mo>&ge;</mo>
			<mn> (d + 12.5) mm </mn>
		</math>	
	<br>
	&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp; <b> 2. For bolts greater than 24 mm diameter </b>
	&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
	<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
			<msub>
				<mi>l</mi>
				<mi>v</mi>
			</msub>
			<mo>&ge;</mo>
			<mn> (d + 18.75) mm</mn>
		</math>	
	
	<br>
	&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp; <b> 3. In general (bolt of any diameter)</b>
	&emsp;&emsp;&emsp;&emsp;
	<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
			<msub>
				<mi>l</mi>
				<mi>v</mi>
			</msub>
			<mo>&ge;</mo>
			<mn> 50 to 62.5 mm</mn>
		</math>	
		
		&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
		where, <br>
			&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
			<math xmlns="http://www.w3.org/1998/Math/MathML">
				
					<mi>d</mi>
						
			</math> - diameter of bolt <br><br>
			
			&emsp;&emsp;&emsp;&emsp;
			&emsp;&emsp;&emsp;&emsp;&emsp;
			<img src="' . $img_path . '/l_v.png" width="50%" height="40%" align="middle"/><br>
			<br>
			&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
			<b>Figure:</b> distance between the beam flange or toe of weld to the centre of the nearer bolt <mi>l</mi> <mi>v</mi> <br>
		
	<hr>';
	// All the real form fields.
	$form = array();
	$form['step6_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step6-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_bb_splice_end_plate_form_step6_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step6_feedback');
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
			$earlier_comments = _fetch_existing_ddcl_bb_splice_end_plate_comments($step6_data->id, $step6_data->data_relation);
			$icom = 1;
			$form['step6_field1_fieldset']['existing_comments_title'] = array(
				'#markup' => '<hr>Earlier comment<hr><br>'
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step6_field1_fieldset']['existing_comments' . $icom] = array(
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
					'ddcl_bb_splice_end_plate_step6_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step6_form_next_submit'
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
					'ddcl_bb_splice_end_plate_step6_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step6_form_next_submit'
				)
			);
		} //$step6_data = $form_step6_data->fetchObject()
	} //$form_step6_data
	return $form;
}
function ddcl_bb_splice_end_plate_step6_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step6_field1_fieldset']['step6_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step6_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step6_feedback');
	$query->condition('uid', $user->uid);
	$form_step6_data = $query->execute()->fetchObject();
	//var_dump($form_step6_data);die;
	if ($form_step6_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step6_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step6_data->id
		));
		$form_step6_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step6_field1_fieldset']["step6_comment_count"]; $i++)
		{
			if ($v['step6_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step6_feedback} (uid, question1, question_ok_not_ok,question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step5/' . $user->uid;
}
function ddcl_bb_splice_end_plate_step6_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step6_field1_fieldset']['step6_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step6_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step6_feedback');
	$query->condition('uid', $user->uid);
	$form_step6_data = $query->execute()->fetchObject();
	//var_dump($form_step6_data);die;
	if ($form_step6_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step6_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step6_data->id
		));
		$form_step6_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step6_field1_fieldset']["step6_comment_count"]; $i++)
		{
			if ($v['step6_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step6_feedback} (uid, question1, question_ok_not_ok,question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step7/' . $user->uid;
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
function ddcl_bb_splice_end_plate_step7_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('d. Plate dimensions > i. Minimum plate height > ii. Plate width > 1. Minimum plate width > 2. Maximum plate width'));
	$markup_text = '
	<b> <strong> d. Plate dimensions </strong> </b> <br>
	<hr>
	&emsp;&emsp;<b> <strong> i. Minimum plate height ( h<sub>p<sub> minimum</sub></sub> ) </strong> [Reference: based on reasoning]</b> <br>
	&emsp;
	
	<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>h</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <msub>
        <mi>p</mi>
        <mrow class="MJX-TeXAtom-ORD">
          <mi>m</mi>
          <mi>i</mi>
          <mi>n</mi>
          <mi>i</mi>
          <mi>m</mi>
          <mi>u</mi>
          <mi>m</mi>
        </mrow>
      </msub>
    </mrow>
  </msub>
  <mo>=</mo>
  <mi>D</mi>
  <mo>+</mo>
  <mn>2</mn>
  <msub>
    <mi>l</mi>
    <mi>v</mi>
  </msub>
  <mo>+</mo>
  <mn>2</mn>
  <mo stretchy="false">(</mo>
  <msub>
    <mi>n</mi>
    <mi>r</mi>
  </msub>
  <mo>&#x2212;</mo>
  <mn>1</mn>
  <mo stretchy="false">)</mo>
  <mi>p</mi>
  <mo>+</mo>
  <msub>
    <mn>2</mn>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>e</mi>
      <mn>1</mn>
    </mrow>
  </msub>
</math>
		<br>
		</centre>
		<br>
	&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					where, <br>
					&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
						<math xmlns="http://www.w3.org/1998/Math/MathML">
						  <msub>
							<mi>D</mi>
							<mrow class="MJX-TeXAtom-ORD">
						  </msub>
						</math> - depth of beam <br>

					&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
						<math xmlns="http://www.w3.org/1998/Math/MathML">
  							<msub>
								<mi>l</mi>
								<mi>v</mi>
							</msub>
						</math> - distance between the beam flange or toe of weld to the centre of the nearer bolt <br>

					&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
						<math xmlns="http://www.w3.org/1998/Math/MathML">
						  <msub>
								<mi>n</mi>
								<mi>r</mi>
							</msub>
						</math> - number of row(s) of bolt above or below the beam flange <br>

					&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
						<math xmlns="http://www.w3.org/1998/Math/MathML">
						  <msub>
							<mi>p</mi>
							<mrow class="MJX-TeXAtom-ORD">
							</mrow>
						  </msub>
						</math> - minimum pitch distance <br>
					
					&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
						<math xmlns="http://www.w3.org/1998/Math/MathML">
						  <msub>
								<mi>e</mi>
								<mi>1</mi>
							</msub>
						</math> - minimum end distance <br>
	<hr>
		&emsp;&emsp;<b><strong> ii. Plate width ( w<sub>p </sub>) </strong> </b> <br>
		<br>
		&emsp;&emsp;&emsp;&emsp;<b><strong> 1. Minimum plate width - w<sub>p<sub> minimum</sub></sub> </strong> [Reference: based on reasoning] </b> <br>
	&emsp;
	
	<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>w</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <msub>
        <mi>p</mi>
        <mrow class="MJX-TeXAtom-ORD">
          <mi>m</mi>
          <mi>i</mi>
          <mi>n</mi>
          <mi>i</mi>
          <mi>m</mi>
          <mi>u</mi>
          <mi>m</mi>
        </mrow>
      </msub>
    </mrow>
  </msub>
  <mo>=</mo>
  <msub>
    <mi>g</mi>
    <mn>1</mn>
  </msub>
  <mo>+</mo>
  <mo stretchy="false">(</mo>
  <mi>n</mi>
  <mo>&#x2212;<!-- − --></mo>
  <mn>2</mn>
  <mo stretchy="false">)</mo>
  <mi>g</mi>
  <mo>+</mo>
  <mn>2</mn>
  <msub>
    <mi>e</mi>
    <mn>2</mn>
  </msub>
</math>
		<br>
		</centre>
		<br>
	&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					where, <br>
					&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
						<math xmlns="http://www.w3.org/1998/Math/MathML">
						  <msub>
								<mi>g</mi>
								<mi>1</mi>
							</msub>
						</math> - cross centre gauge <br>

					&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
						<math xmlns="http://www.w3.org/1998/Math/MathML">
  							<msub>
								<mi>n</mi>
								<mrow class="MJX-TeXAtom-ORD">
								</mrow>
							</msub>
						</math> - number of columns of bolt on either sides <br>

					&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
						<math xmlns="http://www.w3.org/1998/Math/MathML">
						  <msub>
							<mi>g</mi>
							<mrow class="MJX-TeXAtom-ORD">
							</mrow>
						  </msub>
						</math> - minimum gauge distance <br>
					
					&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
						<math xmlns="http://www.w3.org/1998/Math/MathML">
						  <msub>
								<mi>e</mi>
								<mi>2</mi>
							</msub>
						</math> - minimum edge distance <br>
		<br>
		&emsp;&emsp;&emsp;&emsp;<b><strong> 2. Maximum plate width - w<sub>p<sub> maximum</sub></sub> </strong>  [Reference: AISC design guide 16] </b> <br>
		


	&emsp;
	
	<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>w</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <msub>
        <mi>p</mi>
        <mrow class="MJX-TeXAtom-ORD">
          <mi>m</mi>
          <mi>a</mi>
          <mi>x</mi>
          <mi>i</mi>
          <mi>m</mi>
          <mi>u</mi>
          <mi>m</mi>
        </mrow>
      </msub>
    </mrow>
  </msub>
  <mo>=</mo>
  <msub>
    <mi>B</mi>
    <mi>f</mi>
  </msub>
  <mo>+</mo>
  <mn>50</mn>
  <mi>mm</mi>
</math>
		</centre>
		<br>
	&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					where, <br>
					&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
						<math xmlns="http://www.w3.org/1998/Math/MathML">
							<msub>
								<mi>B</mi>
								<mi>f</mi>
							</msub>
						</math> - width of beam flange <br>
	
	<hr>';
	// All the real form fields.
	$form = array();
	$form['step7_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step7-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_bb_splice_end_plate_form_step7_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step7_feedback');
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
			$earlier_comments = _fetch_existing_ddcl_bb_splice_end_plate_comments($step7_data->id, $step7_data->data_relation);
			$icom = 1;
			$form['step7_field1_fieldset']['existing_comments_title'] = array(
				'#markup' => '<hr>Earlier comment<hr><br>'
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step7_field1_fieldset']['existing_comments' . $icom] = array(
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
					'ddcl_bb_splice_end_plate_step7_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step7_form_next_submit'
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
					'ddcl_bb_splice_end_plate_step7_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step7_form_next_submit'
				)
			);
		} //$step7_data = $form_step7_data->fetchObject()
	} //$form_step7_data
	return $form;
}
function ddcl_bb_splice_end_plate_step7_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step7_field1_fieldset']['step7_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step7_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step7_feedback');
	$query->condition('uid', $user->uid);
	$form_step7_data = $query->execute()->fetchObject();
	//var_dump($form_step7_data);die;
	if ($form_step7_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step7_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step7_data->id
		));
		$form_step7_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step7_field1_fieldset']["step7_comment_count"]; $i++)
		{
			if ($v['step7_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step7_feedback} (uid, question1,question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step6/' . $user->uid;
}
function ddcl_bb_splice_end_plate_step7_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step7_field1_fieldset']['step7_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step7_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step7_feedback');
	$query->condition('uid', $user->uid);
	$form_step7_data = $query->execute()->fetchObject();
	//var_dump($form_step7_data);die;
	if ($form_step7_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step7_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step7_data->id
		));
		$form_step7_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step7_field1_fieldset']["step7_comment_count"]; $i++)
		{
			if ($v['step7_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step7_feedback} (uid, question1,question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step8/' . $user->uid;
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
function ddcl_bb_splice_end_plate_step8_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('e. Bolt forces > i. Tension in bolts > ii. Prying forces'));
	$img_path = $base_url . '/' . drupal_get_path("module", "osdag_response_form") . '/images';
	$markup_text = '
	<b><strong> e. Bolt forces </strong> </b> <br>
	&emsp;&emsp; <b><strong> Assumption: </strong></b> The line of rotation or neutral axis is assumed to pass through the centre of the bottom flange. [Reference: example <br>
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp; <b> 5.23, Design of steel structures - N. Subramanian ] <br>
<br>					
<hr>

	&emsp;&emsp;&emsp; <b><strong> i. Tension in i<sup> th </sup> bolt ( T<sub>i</sub> ) </strong></b> [Reference: equation 6.35, page 6.39 - M.L Gambhir] </br>
  <br>
<br>
				<center>
				<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>T</mi>
    <mi>i</mi>
  </msub>
  <mo>=</mo>
  <mi>M</mi>
  <msub>
    <mi>y</mi>
    <mi>i</mi>
  </msub>
  <mrow class="MJX-TeXAtom-ORD">
    <mo>/</mo>
  </mrow>
  <munderover>
    <mo>&#x2211;<!-- ∑ --></mo>
    <mrow class="MJX-TeXAtom-ORD">

    </mrow>
    <mrow class="MJX-TeXAtom-ORD">

    </mrow>
  </munderover>
  <msup>
    <mi>y</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mn>2</mn>
    </mrow>
  </msup>
</math>				</math>
				</center> <br>
				
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
				where, 
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
					  <mi>M</mi>
					</math> - Applied moment (External factored moment + Moment due to factored axial force)  		
				<br>				
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
						<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>y</mi>
								<mi>i</mi>
							</msub>
						</math> - distance of i<sup> th </sup> bolt from the centre of rotation (i.e from assumed 							neutral axis)  <br>
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;				
<math xmlns="http://www.w3.org/1998/Math/MathML" align="left">
  <munderover>
    <mo>&#x2211;<!-- ∑ --></mo>
    <mrow class="MJX-TeXAtom-ORD">

    </mrow>
    <mrow class="MJX-TeXAtom-ORD">

    </mrow>
  </munderover>
  <mi>y</mi>
  <mo>=</mo>
  <msub>
    <mi>y</mi>
    <mn>1</mn>
  </msub>
  <mo>+</mo>
  <msub>
    <mi>y</mi>
    <mn>2</mn>
  </msub>
  <mo>+</mo>
  <mrow class="MJX-TeXAtom-ORD">
    <mo>.</mo>
    <mo>.</mo>
    <mo>.</mo>
    <mo>.</mo>
  </mrow>
  <mo>+</mo>
  <msub>
    <mi>y</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <msub>
        <mi>N</mi>
        <mi>L</mi>
      </msub>
      <mo>&#x2212;<!-- − --></mo>
      <mn>1</mn>
    </mrow>
  </msub>
</math>
									

<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
					  <mi>L</mi>
					</math> - last bolt above the bottom flange 		
				<br>				


					
<hr>		

	&emsp;&emsp;&emsp; <b><strong> ii. Prying forces (Q) </strong></b> [ Reference: Cl 10.4.7 - IS 800 ]<b><br>
	
	<br>
	<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <mi>Q</mi>
  <mo>=</mo>
  <msub>
    <mi>l</mi>
    <mi>v</mi>
  </msub>
  <mrow class="MJX-TeXAtom-ORD">
    <mo>/</mo>
  </mrow>
  <mn>2</mn>
  <msub>
    <mi>l</mi>
    <mi>e</mi>
  </msub>
  <mtext>&#xA0;</mtext>
  <mo stretchy="false">[</mo>
  <msub>
    <mi>T</mi>
    <mi>e</mi>
  </msub>
  <mo>&#x2212;<!-- − --></mo>
  <mo stretchy="false">(</mo>
  <mi>&#x03B2;<!-- β --></mi>
  <mi>&#x03B7;<!-- η --></mi>
  <msub>
    <mi>f</mi>
    <mn>0</mn>
  </msub>
  <msub>
    <mi>b</mi>
    <mi>e</mi>
  </msub>
  <msup>
    <mi>t</mi>
    <mn>4</mn>
  </msup>
  <mtext>&#xA0;</mtext>
  <mrow class="MJX-TeXAtom-ORD">
    <mo>/</mo>
  </mrow>
  <mtext>&#xA0;</mtext>
  <mn>27</mn>
  <msub>
    <mi>l</mi>
    <mi>e</mi>
  </msub>
  <msubsup>
    <mi>l</mi>
    <mi>v</mi>
    <mn>2</mn>
  </msubsup>
  <mo stretchy="false">)</mo>
  <mo stretchy="false">]</mo>
</math>
</b>
<br>

&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
				where,
				</b>  
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>T</mi>
								<mi>e</mi>
							</msub>
					</math> - Tension acting on beam flange
				</b>
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
				<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>l</mi>
								<mi>v</mi>
							</msub>
					</math> - distance between the beam flange or toe of weld to the centre of the nearer bolt
				<br>			
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
				<math xmlns="http://www.w3.org/1998/Math/MathML" align="left">
  <msub>
    <mi>l</mi>
    <mi>e</mi>
  </msub>
  <mo>=</mo>
  <mi>m</mi>
  <mi>i</mi>
  <mi>n</mi>
  <mi>i</mi>
  <mi>m</mi>
  <mi>u</mi>
  <mi>m</mi>
  <mtext>&#xA0;</mtext>
  <mo stretchy="false">(</mo>
  <mi>e</mi>
  <mo>,</mo>
  <mn>1.1</mn>
  <mtext>&#xA0;</mtext>
  <mi>t</mi>
  <mtext>&#xA0;</mtext>
  <msqrt>
    <mi>&#x03B2;<!-- β --></mi>
    <msub>
      <mi>f</mi>
      <mn>0</mn>
    </msub>
    <mtext>&#xA0;</mtext>
    <mrow class="MJX-TeXAtom-ORD">
      <mo>/</mo>
    </mrow>
    <mtext>&#xA0;</mtext>
    <msub>
      <mi>f</mi>
      <mi>y</mi>
    </msub>
  </msqrt>
  <mo stretchy="false">)</mo>
</math>	<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
				<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>b</mi>
								<mi>e</mi>
							</msub>
					</math> - width of end plate
	<br>
	
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
				<math xmlns="http://www.w3.org/1998/Math/MathML">
					  <mi>t</mi>
				</math> - thickness of end plate 
	<br>
	
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
				<math xmlns="http://www.w3.org/1998/Math/MathML">
					 <mi>&#x03B2;<!-- β --></mi>
				</math> - 2 for non-pretensioned bolt and 1 for pre-tensioned bolt 
	<br>
	
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
				<math xmlns="http://www.w3.org/1998/Math/MathML">
					 <mi>&#x03B7;<!-- η --></mi>
				</math> -1.5
	<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
				<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>f</mi>
								<mi>o</mi>
							</msub>
					</math> - proof stress [Reference: table 5.11, Design of steel structures - N. Subramanian]
	<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
				<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>f</mi>
								<mi>y</mi>
							</msub>
					</math> - yield stress of bolt



		
	<hr>';
	// All the real form fields.
	$form = array();
	$form['step8_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step8-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_bb_splice_end_plate_form_step8_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step8_feedback');
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
			$earlier_comments = _fetch_existing_ddcl_bb_splice_end_plate_comments($step8_data->id, $step8_data->data_relation);
			$icom = 1;
			$form['step8_field1_fieldset']['existing_comments_title'] = array(
				'#markup' => '<hr>Earlier comment<hr><br>'
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step8_field1_fieldset']['existing_comments' . $icom] = array(
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
					'ddcl_bb_splice_end_plate_step8_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step8_form_next_submit'
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
					'ddcl_bb_splice_end_plate_step8_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step8_form_next_submit'
				)
			);
		} //$step8_data = $form_step8_data->fetchObject()
	} //$form_step8_data
	return $form;
}
function ddcl_bb_splice_end_plate_step8_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step8_field1_fieldset']['step8_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step8_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step8_feedback');
	$query->condition('uid', $user->uid);
	$form_step8_data = $query->execute()->fetchObject();
	//var_dump($form_step8_data);die;
	if ($form_step8_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step8_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step8_data->id
		));
		$form_step8_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step8_field1_fieldset']["step8_comment_count"]; $i++)
		{
			if ($v['step8_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step8_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step7/' . $user->uid;
}
function ddcl_bb_splice_end_plate_step8_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step8_field1_fieldset']['step8_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step8_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step8_feedback');
	$query->condition('uid', $user->uid);
	$form_step8_data = $query->execute()->fetchObject();
	//var_dump($form_step8_data);die;
	if ($form_step8_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step8_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step8_data->id
		));
		$form_step8_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step8_field1_fieldset']["step8_comment_count"]; $i++)
		{
			if ($v['step8_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step8_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step9/' . $user->uid;
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
function ddcl_bb_splice_end_plate_step9_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('f. Bolt > i. Tension capacity of bolt > 1. Tension capacity of HSFG bolt > 2. Tension capacity of bearing bolt'));
	$markup_text = '
	<b><strong> f. Bolt </strong></b> <br>
	<hr>
	&emsp; <b><strong> i. Tension capacity of bolt </strong><br>
	<br>
	&emsp;&emsp;&emsp;&emsp;&emsp; <b> 1. Tension capacity of HSFG bolt  (T<sub>df</sub>) </b> [Reference: Cl 10.4.5, IS 800]
	<br>
<br>
	<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>T</mi>
    <mi>f</mi>
  </msub>
  <mo>&#x2264;<!-- ≤ --></mo>
  <msub>
    <mi>T</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>d</mi>
      <mi>f</mi>
    </mrow>
  </msub>
</math>
<br>

&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
				where,  
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>T</mi>
								<mi>f</mi>
							</msub>
					</math> - Tension in bolt due to - moment + prying force ( i.e. T + Q )
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>T</mi>
								<mi>df</mi>
							</msub>
					</math> - Tension capacity of HSFG bolt, calculated as;
<br>
<br>
				
<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>T</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>d</mi>
      <mi>f</mi>
    </mrow>
  </msub>
  <mo>=</mo>
  <mn>0.9</mn>
  <msub>
    <mi>f</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>u</mi>
      <mi>b</mi>
    </mrow>
  </msub>
  <msub>
    <mi>A</mi>
    <mi>n</mi>
  </msub>
  <mrow class="MJX-TeXAtom-ORD">
    <mo>/</mo>
  </mrow>
  <msub>
    <mrow class="MJX-TeXAtom-ORD">
      <mo>&#x3B3;</mo>
    </mrow>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>m</mi>
      <mi>f</mi>
    </mrow>
  </msub>
  <mo>&#x2264;<!-- ≤ --></mo>
  <msub>
    <mi>f</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>y</mi>
      <mi>b</mi>
    </mrow>
  </msub>
  <msub>
    <mi>A</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>s</mi>
      <mi>b</mi>
    </mrow>
  </msub>
  <mo stretchy="false">(</mo>
  <msub>
    <mrow class="MJX-TeXAtom-ORD">
      <mo>&#x3B3;</mo>
    </mrow>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>m</mi>
      <mi>l</mi>
    </mrow>
  </msub>
  <mrow class="MJX-TeXAtom-ORD">
    <mo>/</mo>
  </mrow>
  <msub>
    <mrow class="MJX-TeXAtom-ORD">
      <mo>&#x3B3;</mo>
    </mrow>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>m</mi>
      <mi>o</mi>
    </mrow>
  </msub>
  <mo stretchy="false">)</mo>
</math>
				<br>		
					&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>f</mi>
								<mi>ub</mi>
							</msub>
					</math> - ultimate tensile stress of bolt
				<br>		
					&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>f</mi>
								<mi>yb</mi>
							</msub>
					</math> - yield stress of bolt
				<br>		
					&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>A</mi>
								<mi>n</mi>
							</msub>
					</math> - net tensile stress area of bolt
				<br>		
					&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>A</mi>
								<mi>sb</mi>
							</msub>
					</math> - shank area of the bolt
				<br>	
<hr>
	&emsp;&emsp;&emsp;&emsp;&emsp; <b> 2. Tension capacity of bearing bolt  (T<sub>db</sub>) </b> [Reference: Cl 10.3.5, IS 800]
	<br>
<br>
	<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>T</mi>
    <mi>b</mi>
  </msub>
  <mo>&#x2264;<!-- ≤ --></mo>
  <msub>
    <mi>T</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>d</mi>
      <mi>b</mi>
    </mrow>
  </msub>
</math>
<br>

&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
				where,  
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>T</mi>
								<mi>b</mi>
							</msub>
					</math> - Tension in bolt due to - moment + prying force ( i.e. T + Q )
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>T</mi>
								<mi>db</mi>
							</msub>
					</math> - Tension capacity of bearing bolt, calculated as;
<br>
<br>
				
<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>T</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>d</mi>
      <mi>f</mi>
    </mrow>
  </msub>
  <mo>=</mo>
  <mn>0.9</mn>
  <msub>
    <mi>f</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>u</mi>
      <mi>b</mi>
    </mrow>
  </msub>
  <msub>
    <mi>A</mi>
    <mi>n</mi>
  </msub>
  <mrow class="MJX-TeXAtom-ORD">
    <mo>/</mo>
  </mrow>
  <msub>
    <mrow class="MJX-TeXAtom-ORD">
      <mo>&#x3B3;</mo>
    </mrow>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>m</mi>
      <mi>b</mi>
    </mrow>
  </msub>
  <mo>&#x2264;<!-- ≤ --></mo>
  <msub>
    <mi>f</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>y</mi>
      <mi>b</mi>
    </mrow>
  </msub>
  <msub>
    <mi>A</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>s</mi>
      <mi>b</mi>
    </mrow>
  </msub>
  <mo stretchy="false">(</mo>
  <msub>
    <mrow class="MJX-TeXAtom-ORD">
      <mo>&#x3B3;</mo>
    </mrow>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>m</mi>
      <mi>b</mi>
    </mrow>
  </msub>
  <mrow class="MJX-TeXAtom-ORD">
    <mo>/</mo>
  </mrow>
  <msub>
    <mrow class="MJX-TeXAtom-ORD">
      <mo>&#x3B3;</mo>
    </mrow>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>m</mi>
      <mi>o</mi>
    </mrow>
  </msub>
  <mo stretchy="false">)</mo>
</math>
				<br>		
					&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>f</mi>
								<mi>ub</mi>
							</msub>
					</math> - ultimate tensile stress of bolt
				<br>		
					&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>f</mi>
								<mi>yb</mi>
							</msub>
					</math> - yield stress of bolt
				<br>		
					&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>A</mi>
								<mi>n</mi>
							</msub>
					</math> - net tensile stress area of bolt
				<br>		
					&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>A</mi>
								<mi>sb</mi>
							</msub>
					</math> - shank area of the bolt
				<br>		

				



				
	<hr>';
	// All the real form fields.
	$form = array();
	$form['step9_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step9-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_bb_splice_end_plate_form_step9_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step9_feedback');
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
			$earlier_comments = _fetch_existing_ddcl_bb_splice_end_plate_comments($step9_data->id, $step9_data->data_relation);
			$icom = 1;
			$form['step9_field1_fieldset']['existing_comments_title'] = array(
				'#markup' => '<hr><strong>Earlier comment</strong><hr><br>'
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step9_field1_fieldset']['existing_comments' . $icom] = array(
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
					'ddcl_bb_splice_end_plate_step9_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step9_form_next_submit'
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
					'ddcl_bb_splice_end_plate_step9_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step9_form_next_submit'
				)
			);
		} //$step9_data = $form_step9_data->fetchObject()
	} //$form_step9_data
	return $form;
}
function ddcl_bb_splice_end_plate_step9_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step9_field1_fieldset']['step9_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step9_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step9_feedback');
	$query->condition('uid', $user->uid);
	$form_step9_data = $query->execute()->fetchObject();
	//var_dump($form_step9_data);die;
	if ($form_step9_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step9_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step9_data->id
		));
		$form_step9_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step9_field1_fieldset']["step9_comment_count"]; $i++)
		{
			if ($v['step9_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step9_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step8/' . $user->uid;
}
function ddcl_bb_splice_end_plate_step9_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step9_field1_fieldset']['step9_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step9_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step9_feedback');
	$query->condition('uid', $user->uid);
	$form_step9_data = $query->execute()->fetchObject();
	//var_dump($form_step9_data);die;
	if ($form_step9_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step9_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step9_data->id
		));
		$form_step9_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step9_field1_fieldset']["step9_comment_count"]; $i++)
		{
			if ($v['step9_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step9_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step10/' . $user->uid;
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
function ddcl_bb_splice_end_plate_step10_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('f. Bolt > ii. Shear and bearing capacities of bolt > 1. HSFG bolt > 2. Bearing bolt'));
	$img_path = $base_url . '/' . drupal_get_path("module", "osdag_response_form") . '/images';
	$markup_text = '
	
	
	<b><strong> f. Bolt </strong></b> <br>
	&emsp; <b><strong> ii. Shear and bearing capacities of bolt </strong><br>
<hr>
	&emsp;&emsp;&emsp;<b> 1. HSFG bolt </b><br>
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp; This check will be the same as implemented for the shear connection modules (already reviewed).

<hr>
&emsp;&emsp;&emsp;<b> 2. Bearing bolt (also known as black bolt) </b><br>
&emsp;&emsp;&emsp;&emsp;&emsp;&emsp; This check will be the same as implemented for the shear connection modules (already reviewed).
	

			
	<hr>';
	// All the real form fields.
	$form = array();
	$form['step10_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step10-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_bb_splice_end_plate_form_step10_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step10_feedback');
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
			$earlier_comments = _fetch_existing_ddcl_bb_splice_end_plate_comments($step10_data->id, $step10_data->data_relation);
			$icom = 1;
			$form['step10_field1_fieldset']['existing_comments_title'] = array(
				'#markup' => '<hr><strong>Earlier comment</strong><hr><br>'
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step10_field1_fieldset']['existing_comments' . $icom] = array(
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
					'ddcl_bb_splice_end_plate_step10_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step10_form_next_submit'
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
					'ddcl_bb_splice_end_plate_step10_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step10_form_next_submit'
				)
			);
		} //$step10_data = $form_step10_data->fetchObject()
	} //$form_step10_data
	return $form;
}
function ddcl_bb_splice_end_plate_step10_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step10_field1_fieldset']['step10_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step10_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step10_feedback');
	$query->condition('uid', $user->uid);
	$form_step10_data = $query->execute()->fetchObject();
	//var_dump($form_step10_data);die;
	if ($form_step10_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step10_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step10_data->id
		));
		$form_step10_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step10_field1_fieldset']["step10_comment_count"]; $i++)
		{
			if ($v['step10_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step10_feedback} (uid, question1,  question_ok_not_ok,question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step9/' . $user->uid;
}
function ddcl_bb_splice_end_plate_step10_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step10_field1_fieldset']['step10_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step10_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step10_feedback');
	$query->condition('uid', $user->uid);
	$form_step10_data = $query->execute()->fetchObject();
	//var_dump($form_step10_data);die;
	if ($form_step10_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step10_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step10_data->id
		));
		$form_step10_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step10_field1_fieldset']["step10_comment_count"]; $i++)
		{
			if ($v['step10_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step10_feedback} (uid, question1,  question_ok_not_ok,question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step11/' . $user->uid;
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
function ddcl_bb_splice_end_plate_step11_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('f. Bolt > iii. Combined shear and tension capacity of bolt > 1. HSFG bolt > 2. Bearing bolt'));
	$markup_text = '
	
	<b><strong> f. Bolt </strong></b> <br>
	&emsp;&emsp; <b><strong> iii. Combined shear and tension capacity of bolt </strong></b><br>
	<hr>
	
	&emsp;&emsp;&emsp;&emsp; <b><strong> 1. HSFG bolt </strong> [Reference: Cl 10.4.6 - IS 800] </b><br>
	<br>
	<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <mo stretchy="false">(</mo>
  <msub>
    <mi>V</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>s</mi>
      <mi>f</mi>
    </mrow>
  </msub>
  <mrow class="MJX-TeXAtom-ORD">
    <mo>/</mo>
  </mrow>
  <msub>
    <mi>V</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>d</mi>
      <mi>f</mi>
    </mrow>
  </msub>
  <msup>
    <mo stretchy="false">)</mo>
    <mn>2</mn>
  </msup>
  <mo>+</mo>
  <mo stretchy="false">(</mo>
  <msub>
    <mi>T</mi>
    <mi>f</mi>
  </msub>
  <mrow class="MJX-TeXAtom-ORD">
    <mo>/</mo>
  </mrow>
  <msub>
    <mi>T</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>d</mi>
      <mi>f</mi>
    </mrow>
  </msub>
  <msup>
    <mo stretchy="false">)</mo>
    <mn>2</mn>
  </msup>
  <mtext>&#xA0;</mtext>
  <mo>&#x2264;<!-- ≤ --></mo>
  <mtext>&#xA0;</mtext>
  <mn>1.0</mn>
</math>


				&emsp;&emsp;&emsp;&emsp;&emsp;
				where,  
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>V</mi>
								<mi>sf</mi>
							</msub>
					</math> - applied factored shear at design load
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>V</mi>
								<mi>df</mi>
							</msub>
					</math> - design shear strength
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>T</mi>
								<mi>f</mi>
							</msub>
					</math> - externally applied factored tension at design load (tension due to moment + prying force)
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>T</mi>
								<mi>df</mi>
							</msub>
					</math> - design tension strength				
<hr>

				&emsp;&emsp;&emsp;&emsp; <b><strong> 2. Bearing bolt </strong> [Reference: Cl 10.3.6 - IS 800] </b><br>
	<br>
	<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <mo stretchy="false">(</mo>
  <msub>
    <mi>V</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>s</mi>
      <mi>b</mi>
    </mrow>
  </msub>
  <mrow class="MJX-TeXAtom-ORD">
    <mo>/</mo>
  </mrow>
  <msub>
    <mi>V</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>d</mi>
      <mi>b</mi>
    </mrow>
  </msub>
  <msup>
    <mo stretchy="false">)</mo>
    <mn>2</mn>
  </msup>
  <mo>+</mo>
  <mo stretchy="false">(</mo>
  <msub>
    <mi>T</mi>
    <mi>b</mi>
  </msub>
  <mrow class="MJX-TeXAtom-ORD">
    <mo>/</mo>
  </mrow>
  <msub>
    <mi>T</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>d</mi>
      <mi>b</mi>
    </mrow>
  </msub>
  <msup>
    <mo stretchy="false">)</mo>
    <mn>2</mn>
  </msup>
  <mtext>&#xA0;</mtext>
  <mo>&#x2264;<!-- ≤ --></mo>
  <mtext>&#xA0;</mtext>
  <mn>1.0</mn>
</math>


				&emsp;&emsp;&emsp;&emsp;&emsp;
				where,  
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>V</mi>
								<mi>sb</mi>
							</msub>
					</math> - applied factored shear at design load
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>V</mi>
								<mi>db</mi>
							</msub>
					</math> - design shear strength
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>T</mi>
								<mi>b</mi>
							</msub>
					</math> - externally applied factored tension at design load (tension due to moment + prying force)
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>T</mi>
								<mi>db</mi>
							</msub>
					</math> - design tension strength				
			
	<hr>';
	// All the real form fields.
	$form = array();
	$form['step11_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step11-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_bb_splice_end_plate_form_step11_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step11_feedback');
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
			$earlier_comments = _fetch_existing_ddcl_bb_splice_end_plate_comments($step11_data->id, $step11_data->data_relation);
			$icom = 1;
			$form['step11_field1_fieldset']['existing_comments_title'] = array(
				'#markup' => 'Earlier comment<br>'
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step11_field1_fieldset']['existing_comments' . $icom] = array(
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
					'ddcl_bb_splice_end_plate_step11_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step11_form_next_submit'
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
					'ddcl_bb_splice_end_plate_step11_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step11_form_next_submit'
				)
			);
		} //$step11_data = $form_step11_data->fetchObject()
	} //$form_step11_data
	return $form;
}
function ddcl_bb_splice_end_plate_step11_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step11_field1_fieldset']['step11_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step11_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step11_feedback');
	$query->condition('uid', $user->uid);
	$form_step11_data = $query->execute()->fetchObject();
	//var_dump($form_step11_data);die;
	if ($form_step11_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step11_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step11_data->id
		));
		$form_step11_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step11_field1_fieldset']["step11_comment_count"]; $i++)
		{
			if ($v['step11_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step11_feedback} (uid, question1,question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step10/' . $user->uid;
}
function ddcl_bb_splice_end_plate_step11_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step11_field1_fieldset']['step11_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step11_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step11_feedback');
	$query->condition('uid', $user->uid);
	$form_step11_data = $query->execute()->fetchObject();
	//var_dump($form_step11_data);die;
	if ($form_step11_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step11_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step11_data->id
		));
		$form_step11_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step11_field1_fieldset']["step11_comment_count"]; $i++)
		{
			if ($v['step11_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step11_feedback} (uid, question1,question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step12/' . $user->uid;
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
function ddcl_bb_splice_end_plate_step12_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	$img_path = $base_url . '/' . drupal_get_path("module", "osdag_response_form") . '/images';
	drupal_set_title(t('g. End plate > i. Thickness of end plate'));
	$markup_text = '
	
	<b><strong> g. End plate </strong></b> <br>
	&emsp; <b><strong> i. Thickness of end plate </strong> [Reference: equation 5.19 and example 5.23, Design of steel structures - N. Subramanian] </b> 
<hr>
	
			&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
			
			<img src="' . $img_path . '/prying.jpg" width="50%" height="60%" align="middle"/><br>
			<br>
			&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
			&emsp;&emsp;&emsp;
			&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;<b>

			&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
			&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
			<img src="' . $img_path . '/tp.jpg" width="80%" height="90%" align="middle"/><br>
			<br>
			&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
			&emsp;&emsp;&emsp;
			&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;<b>
			<hr>

	&emsp;&emsp;&emsp; The plastic moment capacity of end plate (M<sub>p</sub>) is given as; <br><br>

	<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>M</mi>
    <mi>p</mi>
  </msub>
  <mo>=</mo>
  <mo stretchy="false">(</mo>
  <msub>
    <mi>f</mi>
    <mi>y</mi>
  </msub>
  <mrow class="MJX-TeXAtom-ORD">
    <mo>/</mo>
  </mrow>
  <mn>1.10</mn>
  <mo stretchy="false">)</mo>
  <mo stretchy="false">(</mo>
  <mi>w</mi>
  <msup>
    <mi>T</mi>
    <mn>2</mn>
  </msup>
  <mrow class="MJX-TeXAtom-ORD">
    <mo>/</mo>
  </mrow>
  <mn>4</mn>
  <mo stretchy="false">)</mo>
</math>	
<br>

	&emsp;&emsp;&emsp; Minimum thickness of end plate ( t<sub>p<sub>minimum</sub></sub> ) from bending criteria is given as; <br><br>
	<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>t</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <msub>
        <mi>p</mi>
        <mrow class="MJX-TeXAtom-ORD">
          <mi>m</mi>
          <mi>i</mi>
          <mi>n</mi>
          <mi>i</mi>
          <mi>m</mi>
          <mi>u</mi>
          <mi>m</mi>
        </mrow>
      </msub>
    </mrow>
  </msub>
  <mo>=</mo>
  <msqrt>
    <mn>4</mn>
    <mo>&#x2217;</mo>
    <mn>1.10</mn>
    <msub>
      <mi>M</mi>
      <mi>p</mi>
    </msub>
    <mrow class="MJX-TeXAtom-ORD">
      <mo>/</mo>
    </mrow>
    <msub>
      <mi>f</mi>
      <mi>y</mi>
    </msub>
    <msub>
      <mi>b</mi>
      <mi>e</mi>
    </msub>
  </msqrt>
</math>
		&emsp;&emsp;&emsp;&emsp;&emsp;
				where,  
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
					  <msub>
								<mi>M</mi>
								<mi>p</mi>
					</msub>
					</math> - Plastic moment capacity of plate [Reference: equation 5.17, page 350 - N. Subramanian]
				&emsp;&emsp;&emsp;&emsp;&emsp;
				
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>f</mi>
								<mi>y</mi>
							</msub>
					</math> - yield stress of plate material
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>b</mi>
								<mi>e</mi>
							</msub>
					</math> = w<sub>p</sub> / 2
				<br>
				
 			&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>w</mi>
								<mi>p</mi>
							</msub>
					</math> - width of plate	
					
	<hr>';
	// All the real form fields.
	$form = array();
	$form['step12_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step12-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_bb_splice_end_plate_form_step12_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step12_feedback');
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
			$earlier_comments = _fetch_existing_ddcl_bb_splice_end_plate_comments($step12_data->id, $step12_data->data_relation);
			$icom = 1;
			$form['step12_field1_fieldset']['existing_comments_title'] = array(
				'#markup' => '<hr><strong>Earlier comment<strong><hr><br>'
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step12_field1_fieldset']['existing_comments' . $icom] = array(
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
					'ddcl_bb_splice_end_plate_step12_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step12_form_next_submit'
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
					'ddcl_bb_splice_end_plate_step12_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step12_form_next_submit'
				)
			);
		} //$step12_data = $form_step12_data->fetchObject()
	} //$form_step12_data
	return $form;
}
function ddcl_bb_splice_end_plate_step12_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step12_field1_fieldset']['step12_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step12_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step12_feedback');
	$query->condition('uid', $user->uid);
	$form_step12_data = $query->execute()->fetchObject();
	//var_dump($form_step12_data);die;
	if ($form_step12_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step12_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step12_data->id
		));
		$form_step12_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step12_field1_fieldset']["step12_comment_count"]; $i++)
		{
			if ($v['step12_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step12_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step11/' . $user->uid;
}
function ddcl_bb_splice_end_plate_step12_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step12_field1_fieldset']['step12_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step12_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step12_feedback');
	$query->condition('uid', $user->uid);
	$form_step12_data = $query->execute()->fetchObject();
	//var_dump($form_step12_data);die;
	if ($form_step12_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step12_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step12_data->id
		));
		$form_step12_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step12_field1_fieldset']["step12_comment_count"]; $i++)
		{
			if ($v['step12_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step12_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok, :question_not_ok_comment, :data_relation,:inserted_on)";
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
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step13/' . $user->uid;
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
function ddcl_bb_splice_end_plate_step13_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	$img_path = $base_url . '/' . drupal_get_path("module", "osdag_response_form") . '/images';
	drupal_set_title(t('g. End plate > ii. Shear yielding of end plate > ii. Shear rupture of end plate'));	
	$markup_text = '
	
	
	<b><strong> g. End plate </strong></b> <br>
	&emsp; <b><strong> ii. Shear yielding of end plate </strong></b> [Reference: Cl. 8.4.1, IS 800] <br>
	<hr>
	<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>V</mi>
    <mi>d</mi>
  </msub>
  <mo>&#x2265;<!-- ≥ --></mo>
  <mi>V</mi>
</math>
	<br>
<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>V</mi>
    <mi>d</mi>
  </msub>
  <mo>=</mo>
  <mn>0.6</mn>
  <msub>
    <mi>A</mi>
    <mi>v</mi>
  </msub>
  <msub>
    <mi>f</mi>
    <mi>y</mi>
  </msub>
  <mrow class="MJX-TeXAtom-ORD">
    <mo>/</mo>
  </mrow>
  <msqrt>
    <mn>3</mn>
  </msqrt>
  <msub>
    <mi>&#x03B3;<!-- γ --></mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>m</mi>
      <mn>0</mn>
    </mrow>
  </msub>
</math>

		&emsp;&emsp;&emsp;&emsp;&emsp;
				where,  
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
					  <mi>V</mi>
					</math> - Factored external shear force 
				&emsp;&emsp;&emsp;&emsp;&emsp;
				
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>A</mi>
								<mi>v</mi>
							</msub>
					</math> - gross shear area of end plate 
				<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>A</mi>
    <mi>v</mi>
  </msub>
  <mo>=</mo>
  <msub>
    <mi>w</mi>
    <mi>p</mi>
  </msub>
  <mo>&#x2217;<!-- ∗ --></mo>
  <msub>
    <mi>t</mi>
    <mi>p</mi>
  </msub>
</math>
				<br>	
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>w</mi>
								<mi>p</mi>
							</msub>
					</math> - width of end plate 
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>t</mi>
								<mi>p</mi>
							</msub>
					</math> - minimum required thickness of end plate (as calculated in previous step i.e. Step g. > i)  
				<br>		
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>f</mi>
								<mi>y</mi>
							</msub>
					</math> - yield stress of end plate
				
				<br>
				<hr>
&emsp; <b><strong> iii. Shear rupture of end plate </strong></b> [Reference: Cl. 8.4.1, IS 800] <br>
<br>
	<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>R</mi>
    <mi>n</mi>
  </msub>
  <mo>&#x2265;<!-- ≥ --></mo>
  <mi>V</mi>
</math>
	<br>
<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>R</mi>
    <mi>n</mi>
  </msub>
  <mo>=</mo>
  <mn>0.6</mn>
  <msub>
    <mi>f</mi>
    <mi>u</mi>
  </msub>
  <msub>
    <mi>A</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>v</mi>
      <mi>n</mi>
    </mrow>
  </msub>
</math>
		&emsp;&emsp;&emsp;&emsp;&emsp;
				where,  
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
					  <mi>V</mi>
					</math> - Factored external shear force 
				&emsp;&emsp;&emsp;&emsp;&emsp;
				
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>A</mi>
								<mi>vn</mi>
							</msub>
					</math> - net area of end plate = (w<sub>p</sub> t<sub>p</sub>) - (n d<sub>0</sub>)
				<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>A</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>v</mi>
      <mi>n</mi>
    </mrow>
  </msub>
  <mo>=</mo>
  <mo stretchy="false">(</mo>
  <msub>
    <mi>w</mi>
    <mi>p</mi>
  </msub>
  <msub>
    <mi>t</mi>
    <mi>p</mi>
  </msub>
  <mo stretchy="false">)</mo>
  <mo>&#x2212;<!-- − --></mo>
  <mo stretchy="false">(</mo>
  <mi>n</mi>
  <msub>
    <mi>d</mi>
    <mn>0</mn>
  </msub>
  <mo stretchy="false">)</mo>
</math>
				<br>	
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>w</mi>
								<mi>p</mi>
							</msub>
					</math> - width of end plate 
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>t</mi>
								<mi>p</mi>
							</msub>
					</math> - minimum required thickness of end plate (as calculated in previous step i.e. Step g. > i)  
<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
				<math xmlns="http://www.w3.org/1998/Math/MathML">
					  <mi>n</mi>
					</math> - total number of bolts provided 
				
				<br>		
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>d</mi>
								<mi>0</mi>
							</msub>
					</math> - diameter of hole
				
				<br>


		
	
	<hr>';
	// All the real form fields.
	$form = array();
	$form['step13_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step13-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_bb_splice_end_plate_form_step13_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step13_feedback');
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
			$earlier_comments = _fetch_existing_ddcl_bb_splice_end_plate_comments($step13_data->id, $step13_data->data_relation);
			$icom = 1;
			$form['step13_field1_fieldset']['existing_comments_title'] = array(
				'#markup' => 'Earlier comment<br>'
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step13_field1_fieldset']['existing_comments' . $icom] = array(
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
					'ddcl_bb_splice_end_plate_step13_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step13_form_next_submit'
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
					'ddcl_bb_splice_end_plate_step13_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step13_form_next_submit'
				)
			);
		} //$step13_data = $form_step13_data->fetchObject()
	} //$form_step13_data
	return $form;
}
function ddcl_bb_splice_end_plate_step13_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step13_field1_fieldset']['step13_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step13_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step13_feedback');
	$query->condition('uid', $user->uid);
	$form_step13_data = $query->execute()->fetchObject();
	//var_dump($form_step12_data);die;
	if ($form_step13_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step13_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step13_data->id
		));
		$form_step13_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step13_field1_fieldset']["step13_comment_count"]; $i++)
		{
			if ($v['step13_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step13_data->id,
					":number" => $i + $form_step13_comment_data->max + 1,
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
	} //$form_step13_data
	else
	{
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step13_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1,  :question_ok_not_ok, :question_not_ok_comment,:data_relation,:inserted_on)";
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
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
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
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step12/' . $user->uid;
}
function ddcl_bb_splice_end_plate_step13_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step13_field1_fieldset']['step13_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step13_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step13_feedback');
	$query->condition('uid', $user->uid);
	$form_step13_data = $query->execute()->fetchObject();
	//var_dump($form_step13_data);die;
	if ($form_step13_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step13_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step13_data->id
		));
		$form_step13_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step13_field1_fieldset']["step13_comment_count"]; $i++)
		{
			if ($v['step13_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step13_data->id,
					":number" => $i + $form_step13_comment_data->max + 1,
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
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step13_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok,:question_not_ok_comment, :data_relation,:inserted_on)";
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
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
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
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
	//$form_state['redirect'] = 'ddcl-bb-splice-end-plate/success';
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step14/' . $user->uid;
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
function ddcl_bb_splice_end_plate_step14_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('g. End plate > iv. Block shear capacity of end plate'));
	$img_path = '';
	$markup_text = '
	&emsp; <b><strong> iv. Block shear capacity of end plate </strong> [Reference: Cl 6.4.1 - IS 800] <br>
	<br>
	&emsp;&emsp;&emsp;  This check will be the same as implemented for the shear connection modules (already reviewed). 
	<br> 
		
	<hr>';
	// All the real form fields.
	$form = array();
	$form['step14_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step14-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_bb_splice_end_plate_form_step14_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step14_feedback');
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
			$earlier_comments = _fetch_existing_ddcl_bb_splice_end_plate_comments($step14_data->id, $step14_data->data_relation);
			$icom = 1;
			$form['step14_field1_fieldset']['existing_comments_title'] = array(
				'#markup' => 'Earlier comment<br>'
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step14_field1_fieldset']['existing_comments' . $icom] = array(
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
					'ddcl_bb_splice_end_plate_step14_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step14_form_next_submit'
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
					'ddcl_bb_splice_end_plate_step14_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step14_form_next_submit'
				)
			);
		} //$step14_data = $form_step14_data->fetchObject()
	} //$form_step14_data
	return $form;
}
function ddcl_bb_splice_end_plate_step14_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step14_field1_fieldset']['step14_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step14_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step14_feedback');
	$query->condition('uid', $user->uid);
	$form_step14_data = $query->execute()->fetchObject();
	//var_dump($form_step14_data);die;
	if ($form_step14_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step14_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step14_data->id
		));
		$form_step14_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step14_field1_fieldset']["step14_comment_count"]; $i++)
		{
			if ($v['step14_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step14_data->id,
					":number" => $i + $form_step14_comment_data->max + 1,
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
	} //$form_step14_data
	else
	{
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step14_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1,  :question_ok_not_ok, :question_not_ok_comment,:data_relation,:inserted_on)";
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
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
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
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step13/' . $user->uid;
}
function ddcl_bb_splice_end_plate_step14_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step14_field1_fieldset']['step14_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step14_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step14_feedback');
	$query->condition('uid', $user->uid);
	$form_step14_data = $query->execute()->fetchObject();
	//var_dump($form_step14_data);die;
	if ($form_step14_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step14_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step7_data->id
		));
		$form_step14_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step14_field1_fieldset']["step14_comment_count"]; $i++)
		{
			if ($v['step14_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step14_data->id,
					":number" => $i + $form_step14_comment_data->max + 1,
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
	} //$form_step14_data
	else
	{
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step14_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok,:question_not_ok_comment, :data_relation,:inserted_on)";
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
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
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
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
	//$form_state['redirect'] = 'ddcl-bb-splice-end-plate/success';
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step15/' . $user->uid;
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
function ddcl_bb_splice_end_plate_step15_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('h. Member checks > i. Strength of flange under compression'));
	$img_path = '';
	$markup_text = '
	<b><strong> h. Member checks </strong></b> <br>
<hr>
	&emsp;&emsp; <b><strong> i. Strength of flange under compression </strong> [Reference: example 5.23 & 5.27, Design of steel structures - N. Subramanian ]<br>

<br>
	<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <mi>C</mi>
  <mi>a</mi>
  <mi>p</mi>
  <mi>a</mi>
  <mi>c</mi>
  <mi>i</mi>
  <mi>t</mi>
  <mi>y</mi>
  <mtext>&#xA0;</mtext>
  <mi>o</mi>
  <mi>f</mi>
  <mtext>&#xA0;</mtext>
  <mi>b</mi>
  <mi>e</mi>
  <mi>a</mi>
  <mi>m</mi>
  <mtext>&#xA0;</mtext>
  <mi>f</mi>
  <mi>l</mi>
  <mi>a</mi>
  <mi>n</mi>
  <mi>g</mi>
  <mi>e</mi>
  <mtext>&#xA0;</mtext>
  <mo>&#x2265;<!-- ≥ --></mo>
  <mtext>&#xA0;</mtext>
  <mi>F</mi>
  <mi>o</mi>
  <mi>r</mi>
  <mi>c</mi>
  <mi>e</mi>
  <mtext>&#xA0;</mtext>
  <mi>i</mi>
  <mi>n</mi>
  <mtext>&#xA0;</mtext>
  <mi>f</mi>
  <mi>l</mi>
  <mi>a</mi>
  <mi>n</mi>
  <mi>g</mi>
  <mi>e</mi>
  <mtext>&#xA0;</mtext>
  <mo stretchy="false">(</mo>
  <msub>
    <mi>F</mi>
    <mi>c</mi>
  </msub>
  <mo stretchy="false">)</mo>
</math>
<br>
	<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <mi>C</mi>
  <mi>a</mi>
  <mi>p</mi>
  <mi>a</mi>
  <mi>c</mi>
  <mi>i</mi>
  <mi>t</mi>
  <mi>y</mi>
  <mtext>&#xA0;</mtext>
  <mi>o</mi>
  <mi>f</mi>
  <mtext>&#xA0;</mtext>
  <mi>b</mi>
  <mi>e</mi>
  <mi>a</mi>
  <mi>m</mi>
  <mtext>&#xA0;</mtext>
  <mi>f</mi>
  <mi>l</mi>
  <mi>a</mi>
  <mi>n</mi>
  <mi>g</mi>
  <mi>e</mi>
  <mo>=</mo>
  <mo stretchy="false">(</mo>
  <msub>
    <mi>f</mi>
    <mi>y</mi>
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
  <mo stretchy="false">)</mo>
  <msub>
    <mi>A</mi>
    <mi>f</mi>
  </msub>
</math>
	<br>

	<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <mi>F</mi>
  <mi>o</mi>
  <mi>r</mi>
  <mi>c</mi>
  <mi>e</mi>
  <mtext>&#xA0;</mtext>
  <mi>i</mi>
  <mi>n</mi>
  <mtext>&#xA0;</mtext>
  <mi>f</mi>
  <mi>l</mi>
  <mi>a</mi>
  <mi>n</mi>
  <mi>g</mi>
  <mi>e</mi>
  <mtext>&#xA0;</mtext>
  <mo stretchy="false">(</mo>
  <msub>
    <mi>F</mi>
    <mi>c</mi>
  </msub>
  <mo stretchy="false">)</mo>
  <mo>=</mo>
  <mi>B</mi>
  <mo>.</mo>
  <mi>M</mi>
  <mrow class="MJX-TeXAtom-ORD">
    <mo>/</mo>
  </mrow>
  <mo stretchy="false">(</mo>
  <mi>D</mi>
  <mo>&#x2212;<!-- − --></mo>
  <msub>
    <mi>t</mi>
    <mi>f</mi>
  </msub>
  <mo stretchy="false">)</mo>
</math>
			&emsp;&emsp;&emsp;
			where,  
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>A</mi>
								<mi>f</mi>
							</msub>
					</math> - area of the beam flange 
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
					  <mi>D</mi>
					</math> - depth of beam 
				&emsp;&emsp;&emsp;&emsp;&emsp;
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
					  <msub>
								<mi>t</mi>
								<mi>f</mi>
							</msub>
					</math> - thickness of beam flange
				&emsp;&emsp;&emsp;&emsp;&emsp;
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>f</mi>
								<mi>y</mi>
							</msub>
					</math> - yield stress of beam material 
		
	
	<hr>';
	// All the real form fields.
	$form = array();
	$form['step15_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step15-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_bb_splice_end_plate_form_step15_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step15_feedback');
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
			$earlier_comments = _fetch_existing_ddcl_bb_splice_end_plate_comments($step15_data->id, $step15_data->data_relation);
			$icom = 1;
			$form['step15_field1_fieldset']['existing_comments_title'] = array(
				'#markup' => 'Earlier comment<br>'
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step15_field1_fieldset']['existing_comments' . $icom] = array(
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
					'ddcl_bb_splice_end_plate_step15_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step15_form_next_submit'
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
					'ddcl_bb_splice_end_plate_step15_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step15_form_next_submit'
				)
			);
		} //$step15_data = $form_step15_data->fetchObject()
	} //$form_step15_data
	return $form;
}
function ddcl_bb_splice_end_plate_step15_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step15_field1_fieldset']['step15_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step15_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step15_feedback');
	$query->condition('uid', $user->uid);
	$form_step15_data = $query->execute()->fetchObject();
	//var_dump($form_step15_data);die;
	if ($form_step15_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step15_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step15_data->id
		));
		$form_step15_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step15_field1_fieldset']["step15_comment_count"]; $i++)
		{
			if ($v['step15_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step15_data->id,
					":number" => $i + $form_step15_comment_data->max + 1,
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
	} //$form_step15_data
	else
	{
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step15_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1,  :question_ok_not_ok, :question_not_ok_comment,:data_relation,:inserted_on)";
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
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
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
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step14/' . $user->uid;
}
function ddcl_bb_splice_end_plate_step15_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step15_field1_fieldset']['step15_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step15_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step15_feedback');
	$query->condition('uid', $user->uid);
	$form_step15_data = $query->execute()->fetchObject();
	//var_dump($form_step15_data);die;
	if ($form_step15_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step15_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step7_data->id
		));
		$form_step15_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step15_field1_fieldset']["step15_comment_count"]; $i++)
		{
			if ($v['step15_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step15_data->id,
					":number" => $i + $form_step15_comment_data->max + 1,
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
	} //$form_step15_data
	else
	{
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step15_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok,:question_not_ok_comment, :data_relation,:inserted_on)";
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
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
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
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
	//$form_state['redirect'] = 'ddcl-bb-splice-end-plate/success';
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step16/' . $user->uid;
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
function ddcl_bb_splice_end_plate_step16_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	$img_path = $base_url . '/' . drupal_get_path("module", "osdag_response_form") . '/images';
	drupal_set_title(t('i. Design of weld > i. Weld profile'));
	$markup_text = '
	<b><strong> i. Design of weld </strong></b> <br>
	<hr>
	&emsp;<b><strong> i. Weld profile </strong> [Reference: based on reasoning] </b> <br>
	<br>
	&emsp;&emsp;&emsp; <b><strong> Assumption: </strong></b> Weld is provided throughout the perimeter of I section. However, the size of weld at flange will be greater than the   </br>
&emsp;&emsp;&emsp; <b> size of weld at web. </br></b>
&emsp;&emsp;&emsp; <b> Weld at flange resists bending moment whereas weld at beam web resists shear + axial force   </br>
	<br>
			
			&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
			&emsp;&emsp;&emsp;&emsp;&emsp;
			<img src="' . $img_path . '/weld_side.png" width="30%" height="45%" align="middle"/><br>
			<br>
			&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
			&emsp;&emsp;&emsp;
			&emsp;&emsp;&emsp;&emsp;<b>Figure:</b> Weld profile (side view)
			<br>
			<br>
			<br>
			&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
			
			<img src="' . $img_path . '/weld_front.png" width="60%" height="90%" align="middle"/><br>
			<br>
			&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
			&emsp;&emsp;&emsp;
			&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;<b>Figure:</b> Weld profile (front view)
			<br>
			<br>

	
	<hr>';
	// All the real form fields.
	$form = array();
	$form['step16_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step16-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_bb_splice_end_plate_form_step16_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step16_feedback');
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
			$earlier_comments = _fetch_existing_ddcl_bb_splice_end_plate_comments($step16_data->id, $step16_data->data_relation);
			$icom = 1;
			$form['step16_field1_fieldset']['existing_comments_title'] = array(
				'#markup' => 'Earlier comment<br>'
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step16_field1_fieldset']['existing_comments' . $icom] = array(
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
					'ddcl_bb_splice_end_plate_step16_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step16_form_next_submit'
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
					'ddcl_bb_splice_end_plate_step16_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step16_form_next_submit'
				)
			);
		} //$step16_data = $form_step16_data->fetchObject()
	} //$form_step16_data
	return $form;
}
function ddcl_bb_splice_end_plate_step16_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step16_field1_fieldset']['step16_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step16_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step16_feedback');
	$query->condition('uid', $user->uid);
	$form_step16_data = $query->execute()->fetchObject();
	//var_dump($form_step16_data);die;
	if ($form_step16_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step16_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step16_data->id
		));
		$form_step16_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step16_field1_fieldset']["step16_comment_count"]; $i++)
		{
			if ($v['step16_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step16_data->id,
					":number" => $i + $form_step16_comment_data->max + 1,
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
	} //$form_step16_data
	else
	{
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step16_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1,  :question_ok_not_ok, :question_not_ok_comment,:data_relation,:inserted_on)";
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
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
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
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step15/' . $user->uid;
}
function ddcl_bb_splice_end_plate_step16_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step16_field1_fieldset']['step16_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step16_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step16_feedback');
	$query->condition('uid', $user->uid);
	$form_step16_data = $query->execute()->fetchObject();
	//var_dump($form_step16_data);die;
	if ($form_step16_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step16_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
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
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step7_data->id
		));
		$form_step16_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step16_field1_fieldset']["step16_comment_count"]; $i++)
		{
			if ($v['step16_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step16_data->id,
					":number" => $i + $form_step16_comment_data->max + 1,
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
	} //$form_step16_data
	else
	{
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step16_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok,:question_not_ok_comment, :data_relation,:inserted_on)";
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
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
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
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
	//$form_state['redirect'] = 'ddcl-bb-splice-end-plate/success';
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step17/' . $user->uid;
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
function ddcl_bb_splice_end_plate_step17_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('i. Design of weld > ii. Size of weld at beam flange > 1. Minimum weld thickness'));
	$img_path = '';
	$markup_text = '
	&emsp; <b><strong> ii. Size of weld at beam flange </strong></b> <br>
	<hr>
	&emsp;&emsp;&emsp; <b><strong> 1. Minimum weld thickness ( t<sub>w<sub>minimum</sub></sub> ) </strong> - for dropdown list of weld thickness in Gui [Reference: Table 21, IS 800] <br>
	&emsp;&emsp;&emsp;&emsp; Minimum weld thickness depends on the thickness of the thicker part
	<br>
	<br>
	&emsp;&emsp;&emsp;&emsp; For weld at flange, <math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>t</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>t</mi>
      <mi>h</mi>
      <mi>i</mi>
      <mi>c</mi>
      <mi>k</mi>
      <mi>e</mi>
      <mi>r</mi>
    </mrow>
  </msub>
  <mo>=</mo>
  <mi>m</mi>
  <mi>a</mi>
  <mi>x</mi>
  <mi>i</mi>
  <mi>m</mi>
  <mi>u</mi>
  <mi>m</mi>
  <mo stretchy="false">(</mo>
  <msub>
    <mi>T</mi>
    <mi>f</mi>
  </msub>
  <mo>,</mo>
  <mtext>&#xA0;</mtext>
  <msub>
    <mi>t</mi>
    <mi>w</mi>
  </msub>
  <mo>,</mo>
  <mtext>&#xA0;</mtext>
  <msub>
    <mi>t</mi>
    <mi>p</mi>
  </msub>
  <mo stretchy="false">)</mo>
</math> <br>


<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <mi>I</mi>
  <mi>f</mi>
  <mo>,</mo>
  <msub>
    <mi>t</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>t</mi>
      <mi>h</mi>
      <mi>i</mi>
      <mi>c</mi>
      <mi>k</mi>
      <mi>e</mi>
      <mi>r</mi>
    </mrow>
  </msub>
  <mo>&#x2264;</mo>
  <mn>10</mn>
  <mo>mm</mo>
  <mo>,</mo>
  <msub>
    <mi>t</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <msub>
        <mi>w</mi>
        <mrow class="MJX-TeXAtom-ORD">
          <mi>m</mi>
          <mi>i</mi>
          <mi>n</mi>
          <mi>i</mi>
          <mi>m</mi>
          <mi>u</mi>
          <mi>m</mi>
        </mrow>
      </msub>
    </mrow>
  </msub>
  <mo>=</mo>
  <mn>3</mn>
  <mo>mm</mo>
</math>

<br>
<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <mn>10</mn>
  <mo>mm</mo>
  <mo>&#x2264;</mo>
  <msub>
    <mi>t</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>t</mi>
      <mi>h</mi>
      <mi>i</mi>
      <mi>c</mi>
      <mi>k</mi>
      <mi>e</mi>
      <mi>r</mi>
    </mrow>
  </msub>
  <mo>&#x2264;</mo>
  <mn>20</mn>
  <mo>mm</mo>
  <mo>,</mo>
  <msub>
    <mi>t</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <msub>
        <mi>w</mi>
        <mrow class="MJX-TeXAtom-ORD">
          <mi>m</mi>
          <mi>i</mi>
          <mi>n</mi>
          <mi>i</mi>
          <mi>m</mi>
          <mi>u</mi>
          <mi>m</mi>
        </mrow>
      </msub>
    </mrow>
  </msub>
  <mo>=</mo>
  <mn>5</mn>
  <mo>mm</mo>
</math>

<br>
<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <mn>20</mn>
  <mo>mm</mo>
  <mo>&#x2264;</mo>
  <msub>
    <mi>t</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>t</mi>
      <mi>h</mi>
      <mi>i</mi>
      <mi>c</mi>
      <mi>k</mi>
      <mi>e</mi>
      <mi>r</mi>
    </mrow>
  </msub>
  <mo>&#x2264;</mo>
  <mn>32</mn>
  <mo>mm</mo>
  <mo>,</mo>
  <msub>
    <mi>t</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <msub>
        <mi>w</mi>
        <mrow class="MJX-TeXAtom-ORD">
          <mi>m</mi>
          <mi>i</mi>
          <mi>n</mi>
          <mi>i</mi>
          <mi>m</mi>
          <mi>u</mi>
          <mi>m</mi>
        </mrow>
      </msub>
    </mrow>
  </msub>
  <mo>=</mo>
  <mn>6</mn>
  <mo>mm</mo>
</math>

<br>
<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <mn>32</mn>
  <mo>mm</mo>
  <mo>&#x2264;</mo>
  <msub>
    <mi>t</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>t</mi>
      <mi>h</mi>
      <mi>i</mi>
      <mi>c</mi>
      <mi>k</mi>
      <mi>e</mi>
      <mi>r</mi>
    </mrow>
  </msub>
  <mo>&#x2264;</mo>
  <mn>50</mn>
  <mo>mm</mo>
  <mo>,</mo>
  <msub>
    <mi>t</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <msub>
        <mi>w</mi>
        <mrow class="MJX-TeXAtom-ORD">
          <mi>m</mi>
          <mi>i</mi>
          <mi>n</mi>
          <mi>i</mi>
          <mi>m</mi>
          <mi>u</mi>
          <mi>m</mi>
        </mrow>
      </msub>
    </mrow>
  </msub>
  <mo>=</mo>
  <mn>8</mn>
  <mo>mm</mo>
</math>	
	
	<hr>';
	// All the real form fields.
	$form = array();
	$form['step17_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step17-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_bb_splice_end_plate_form_step17_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step17_feedback');
	$query->condition('uid', $user->uid);
	$form_step17_data = $query->execute();
	if ($form_step17_data)
	{
		if ($step17_data = $form_step17_data->fetchObject())
		{
			$form['step17_field1_fieldset']['step17_field1_question1'] = array(
				'#markup' => $markup_text
			);
			$form['step17_field1_fieldset']['step17_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				'#default_value' => $step17_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step17_field1_fieldset']['step17_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				'#default_value' => $step17_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step17_field1_fieldset\\[step17_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			$earlier_comments = _fetch_existing_ddcl_bb_splice_end_plate_comments($step17_data->id, $step17_data->data_relation);
			$icom = 1;
			$form['step17_field1_fieldset']['existing_comments_title'] = array(
				'#markup' => 'Earlier comment<br>'
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step17_field1_fieldset']['existing_comments' . $icom] = array(
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
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step17_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step17_form_next_submit'
				)
			);
		} //$step17_data = $form_step17_data->fetchObject()
		else
		{
			$form['step17_field1_fieldset']['step17_field1_question1'] = array(
				'#markup' => $markup_text
			);
			$form['step17_field1_fieldset']['step17_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				//'#default_value' => $step17_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step17_field1_fieldset']['step17_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				//'#default_value' => $step17_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step17_field1_fieldset\\[step17_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
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
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step17_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step17_form_next_submit'
				)
			);
		} //$step17_data = $form_step17_data->fetchObject()
	} //$form_step17_data
	return $form;
}
function ddcl_bb_splice_end_plate_step17_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step17_field1_fieldset']['step17_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step17_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step17_feedback');
	$query->condition('uid', $user->uid);
	$form_step17_data = $query->execute()->fetchObject();
	//var_dump($form_step17_data);die;
	if ($form_step17_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step17_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step17_field1_fieldset']['step17_field1_question1'],
			":question_ok_not_ok" => $v['step17_field1_fieldset']['step17_field1_question_ok'],
			":question_not_ok_comment" => $v['step17_field1_fieldset']['step17_field1_question_not_ok_comment'],
			":data_relation" => "step17"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step17_data->id
		));
		$form_step17_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step17_field1_fieldset']["step17_comment_count"]; $i++)
		{
			if ($v['step17_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step17_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1,  :question_ok_not_ok, :question_not_ok_comment,:data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step17_field1_fieldset']['step17_field1_question1'],
			":question_ok_not_ok" => $v['step17_field1_fieldset']['step17_field1_question_ok'],
			":question_not_ok_comment" => $v['step17_field1_fieldset']['step17_field1_question_not_ok_comment'],
			":data_relation" => "step17",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
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
		for ($i = 0; $i <= $v['step17_field1_fieldset']["step17_comment_count"]; $i++)
		{
			if ($v['step17_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step16/' . $user->uid;
}
function ddcl_bb_splice_end_plate_step17_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step17_field1_fieldset']['step17_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step17_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step17_feedback');
	$query->condition('uid', $user->uid);
	$form_step17_data = $query->execute()->fetchObject();
	//var_dump($form_step17_data);die;
	if ($form_step17_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step17_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step17_field1_fieldset']['step17_field1_question1'],
			":question_ok_not_ok" => $v['step17_field1_fieldset']['step17_field1_question_ok'],
			":question_not_ok_comment" => $v['step17_field1_fieldset']['step17_field1_question_not_ok_comment'],
			":data_relation" => "step17"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step7_data->id
		));
		$form_step17_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step17_field1_fieldset']["step17_comment_count"]; $i++)
		{
			if ($v['step17_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step17_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok,:question_not_ok_comment, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step17_field1_fieldset']['step17_field1_question1'],
			":question_ok_not_ok" => $v['step17_field1_fieldset']['step17_field1_question_ok'],
			":question_not_ok_comment" => $v['step17_field1_fieldset']['step17_field1_question_not_ok_comment'],
			":data_relation" => "step17",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
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
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
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
	//$form_state['redirect'] = 'ddcl-bb-splice-end-plate/success';
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step18/' . $user->uid;
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
/***************************************** step 17 form ends ****************************/
/************************ Step 18 form starts *********************/
function ddcl_bb_splice_end_plate_step18_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('i. Design of weld > ii. Size of weld at beam flange > 2. Capacity of unit weld > 3. Effective weld length > 4. Area of weld'));
	$img_path = '';
	$markup_text = '
	&emsp;&emsp; <b><strong> 2. Capacity of unit weld </strong> [Reference: Clause 10.5.7, IS 800] </b> <br>
	<br>
	<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <mi>C</mi>
  <mi>a</mi>
  <mi>p</mi>
  <mi>a</mi>
  <mi>c</mi>
  <mi>i</mi>
  <mi>t</mi>
  <mi>y</mi>
  <mtext>&#xA0;</mtext>
  <mi>o</mi>
  <mi>f</mi>
  <mtext>&#xA0;</mtext>
  <mi>w</mi>
  <mi>e</mi>
  <mi>l</mi>
  <mi>d</mi>
  <mo stretchy="false">(</mo>
  <mi>u</mi>
  <mi>n</mi>
  <mi>i</mi>
  <mi>t</mi>
  <mtext>&#xA0;</mtext>
  <mi>t</mi>
  <mi>h</mi>
  <mi>r</mi>
  <mi>o</mi>
  <mi>a</mi>
  <mi>t</mi>
  <mtext>&#xA0;</mtext>
  <mi>t</mi>
  <mi>h</mi>
  <mi>i</mi>
  <mi>c</mi>
  <mi>k</mi>
  <mi>n</mi>
  <mi>e</mi>
  <mi>s</mi>
  <mi>s</mi>
  <mo stretchy="false">)</mo>
  <mo>=</mo>
  <mi>K</mi>
  <msub>
    <mi>f</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>u</mi>
    </mrow>
  </msub>
  <mrow class="MJX-TeXAtom-ORD">
    <mo>/</mo>
  </mrow>
  <msqrt>
    <mn>3</mn>
  </msqrt>
  <msub>
    <mrow class="MJX-TeXAtom-ORD">
      <mo>&#x3B3;</mo>
    </mrow>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>m</mi>
      <mi>w</mi>
    </mrow>
  </msub>
</math>
	
	&emsp;&emsp;&emsp;&emsp;&emsp;
	where,  
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<mi>K</mi>
  							<mo>=</mo>
  							<mn>0.7</mn>
					</math> - constant [Reference: Table 22, IS 800]
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>f</mi>
								<mi>u</mi>
							</msub>
					</math> - ultimate strength of weld material
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
							    <mi>&#x03B3;<!-- γ --></mi>
							    <mrow class="MJX-TeXAtom-ORD">
							      <mi>m</mi>
							      <mi>w</mi>
							    </mrow>
							  </msub>
					</math> - partial safety factor [Reference: Table 5, IS 800]
<br>
<hr>
	&emsp;&emsp; <b><strong> 3. Effective weld length ( L<sub>effective</sub> ) </strong> [Reference: based on weld profile and Cl. 10.5.4, IS 800] </b> <br>
	<br>
	&emsp;&emsp;&emsp; <b> at flange
	<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>L</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>e</mi>
      <mi>f</mi>
      <mi>f</mi>
      <mi>e</mi>
      <mi>c</mi>
      <mi>t</mi>
      <mi>i</mi>
      <mi>v</mi>
      <mi>e</mi>
    </mrow>
  </msub>
  <mo>=</mo>
  <mn>2</mn>
  <mtext>&#xA0;</mtext>
  <mo stretchy="false">[</mo>
  <mn>2</mn>
  <msub>
    <mi>b</mi>
    <mi>f</mi>
  </msub>
  <mtext>&#xA0;</mtext>
  <mo>+</mo>
  <mtext>&#xA0;</mtext>
  <mn>2</mn>
  <mo stretchy="false">(</mo>
  <msub>
    <mi>b</mi>
    <mi>f</mi>
  </msub>
  <mo>&#x2212;<!-- − --></mo>
  <msub>
    <mi>t</mi>
    <mi>w</mi>
  </msub>
  <mo stretchy="false">)</mo>
  <mtext>&#xA0;</mtext>
  <mo>+</mo>
  <mn>4</mn>
  <msub>
    <mi>T</mi>
    <mi>f</mi>
  </msub>
  <mtext>&#xA0;</mtext>
  <mo>+</mo>
  <mn>2</mn>
  <msub>
    <mi>t</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>w</mi>
      <mi>e</mi>
      <mi>l</mi>
      <mi>d</mi>
    </mrow>
  </msub>
  <mo stretchy="false">]</mo>
</math>
<br>
	&emsp;&emsp;&emsp;  at web
	<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>L</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>e</mi>
      <mi>f</mi>
      <mi>f</mi>
      <mi>e</mi>
      <mi>c</mi>
      <mi>t</mi>
      <mi>i</mi>
      <mi>v</mi>
      <mi>e</mi>
    </mrow>
  </msub>
  <mo>=</mo>
  <mn>2</mn>
  <mtext>&#xA0;</mtext>
  <mo stretchy="false">[</mo>
  <mi>D</mi>
  <mo>&#x2212;<!-- − --></mo>
  <mn>2</mn>
  <msub>
    <mi>t</mi>
    <mi>f</mi>
  </msub>
  <mo stretchy="false">]</mo>
</math>
	<br>
	<hr>
	&emsp;&emsp; <b><strong> 4. Area of weld ( A<sub>weld</sub> ) </strong></b> [Reference: Cl 10.5.3, IS 800] </b> <br>
	<br>
	<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>A</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>w</mi>
      <mi>e</mi>
      <mi>l</mi>
      <mi>d</mi>
    </mrow>
  </msub>
  <mo>=</mo>
  <msub>
    <mi>L</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>e</mi>
      <mi>f</mi>
      <mi>f</mi>
      <mi>e</mi>
      <mi>c</mi>
      <mi>t</mi>
      <mi>i</mi>
      <mi>v</mi>
      <mi>e</mi>
    </mrow>
  </msub>
  <mo>&#x2217;<!-- ∗ --></mo>
  <mi>e</mi>
  <mi>f</mi>
  <mi>f</mi>
  <mi>e</mi>
  <mi>c</mi>
  <mi>t</mi>
  <mi>i</mi>
  <mi>v</mi>
  <mi>e</mi>
  <mtext>&#xA0;</mtext>
  <mi>t</mi>
  <mi>h</mi>
  <mi>r</mi>
  <mi>o</mi>
  <mi>a</mi>
  <mi>t</mi>
  <mtext>&#xA0;</mtext>
  <mi>t</mi>
  <mi>h</mi>
  <mi>i</mi>
  <mi>c</mi>
  <mi>k</mi>
  <mi>n</mi>
  <mi>e</mi>
  <mi>s</mi>
  <mi>s</mi>
</math>
	<br>
	&emsp;&emsp;&emsp;&emsp;&emsp;
	where,
		<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					
						  	effective throat thickness
					</math> - is greater than 3mm and less than equal to 0.7t or 1.0t
		<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<mo>t</mo>
					</math> - thickness of thinner plate of elements being welded
	
	<hr>';
	// All the real form fields.
	$form = array();
	$form['step18_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step18-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_bb_splice_end_plate_form_step18_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step18_feedback');
	$query->condition('uid', $user->uid);
	$form_step18_data = $query->execute();
	if ($form_step18_data)
	{
		if ($step18_data = $form_step18_data->fetchObject())
		{
			$form['step18_field1_fieldset']['step18_field1_question1'] = array(
				'#markup' => $markup_text
			);
			$form['step18_field1_fieldset']['step18_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				'#default_value' => $step18_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step18_field1_fieldset']['step18_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				'#default_value' => $step18_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step18_field1_fieldset\\[step18_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			$earlier_comments = _fetch_existing_ddcl_bb_splice_end_plate_comments($step18_data->id, $step18_data->data_relation);
			$icom = 1;
			$form['step18_field1_fieldset']['existing_comments_title'] = array(
				'#markup' => 'Earlier comment<br>'
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step18_field1_fieldset']['existing_comments' . $icom] = array(
					'#markup' => $rowcomment
				);
				$icom++;
			} //$earlier_comments as $rowcomment
			if (empty($form_state['step18_num_comment']))
			{
				$form_state['step18_num_comment'] = 1;
			} //empty($form_state['step18_num_comment'])
			$step18_temp1 = 0;
			for ($step18_i = 0; $step18_i < $form_state['step18_num_comment']; $step18_i++)
			{
				$step18_temp1 = $step18_i;
				$form['step18_field1_fieldset'][$step18_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step18_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step18_field1_fieldset']["step18_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step18_temp1
			);
			$form['step18_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step18_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step18_comment_add_more_callback',
					'wrapper' => 'step18-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step18_num_comment'] > 1)
			{
				$form['step18_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step18_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step18_comment_add_more_callback',
						'wrapper' => 'step18-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step18_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step18_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step18_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step18_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step18_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step18_form_next_submit'
				)
			);
		} //$step18_data = $form_step18_data->fetchObject()
		else
		{
			$form['step18_field1_fieldset']['step18_field1_question1'] = array(
				'#markup' => $markup_text
			);
			$form['step18_field1_fieldset']['step18_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				//'#default_value' => $step18_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step18_field1_fieldset']['step18_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				//'#default_value' => $step18_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step18_field1_fieldset\\[step18_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			if (empty($form_state['step18_num_comment']))
			{
				$form_state['step18_num_comment'] = 1;
			} //empty($form_state['step18_num_comment'])
			$step18_temp1 = 0;
			for ($step18_i = 0; $step18_i < $form_state['step18_num_comment']; $step18_i++)
			{
				$step18_temp1 = $step18_i;
				$form['step18_field1_fieldset'][$step18_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step18_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step18_field1_fieldset']["step18_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step18_temp1
			);
			$form['step18_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step18_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step18_comment_add_more_callback',
					'wrapper' => 'step18-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step18_num_comment'] > 1)
			{
				$form['step18_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step18_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step18_comment_add_more_callback',
						'wrapper' => 'step18-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step18_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step18_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step18_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step18_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step18_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step18_form_next_submit'
				)
			);
		} //$step18_data = $form_step18_data->fetchObject()
	} //$form_step18_data
	return $form;
}
function ddcl_bb_splice_end_plate_step18_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step18_field1_fieldset']['step18_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step18_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step18_feedback');
	$query->condition('uid', $user->uid);
	$form_step18_data = $query->execute()->fetchObject();
	//var_dump($form_step18_data);die;
	if ($form_step18_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step18_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step18_field1_fieldset']['step18_field1_question1'],
			":question_ok_not_ok" => $v['step18_field1_fieldset']['step18_field1_question_ok'],
			":question_not_ok_comment" => $v['step18_field1_fieldset']['step18_field1_question_not_ok_comment'],
			":data_relation" => "step18"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step18_data->id
		));
		$form_step18_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step18_field1_fieldset']["step18_comment_count"]; $i++)
		{
			if ($v['step18_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step18_data->id,
					":number" => $i + $form_step18_comment_data->max + 1,
					":comment" => $v['step18_field1_fieldset'][$i]["comment"],
					":data_relation" => "step18",
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
			} //$v['step18_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step18_field1_fieldset']["step18_comment_count"]; $i++
	} //$form_step18_data
	else
	{
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step18_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1,  :question_ok_not_ok, :question_not_ok_comment,:data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step18_field1_fieldset']['step18_field1_question1'],
			":question_ok_not_ok" => $v['step18_field1_fieldset']['step18_field1_question_ok'],
			":question_not_ok_comment" => $v['step18_field1_fieldset']['step18_field1_question_not_ok_comment'],
			":data_relation" => "step18",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
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
		for ($i = 0; $i <= $v['step18_field1_fieldset']["step18_comment_count"]; $i++)
		{
			if ($v['step18_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step18_field1_fieldset'][$i]["comment"],
					":data_relation" => "step18",
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
			} //$v['step18_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step18_field1_fieldset']["step18_comment_count"]; $i++
	}
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step17/' . $user->uid;
}
function ddcl_bb_splice_end_plate_step18_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step18_field1_fieldset']['step18_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step18_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step18_feedback');
	$query->condition('uid', $user->uid);
	$form_step18_data = $query->execute()->fetchObject();
	//var_dump($form_step18_data);die;
	if ($form_step18_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step18_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step18_field1_fieldset']['step18_field1_question1'],
			":question_ok_not_ok" => $v['step18_field1_fieldset']['step18_field1_question_ok'],
			":question_not_ok_comment" => $v['step18_field1_fieldset']['step18_field1_question_not_ok_comment'],
			":data_relation" => "step18"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step7_data->id
		));
		$form_step18_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step18_field1_fieldset']["step18_comment_count"]; $i++)
		{
			if ($v['step18_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step18_data->id,
					":number" => $i + $form_step18_comment_data->max + 1,
					":comment" => $v['step18_field1_fieldset'][$i]["comment"],
					":data_relation" => "step18",
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
			} //$v['step18_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step18_field1_fieldset']["step18_comment_count"]; $i++
	} //$form_step18_data
	else
	{
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step18_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok,:question_not_ok_comment, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step18_field1_fieldset']['step18_field1_question1'],
			":question_ok_not_ok" => $v['step18_field1_fieldset']['step18_field1_question_ok'],
			":question_not_ok_comment" => $v['step18_field1_fieldset']['step18_field1_question_not_ok_comment'],
			":data_relation" => "step18",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
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
		for ($i = 0; $i <= $v['step18_field1_fieldset']["step18_comment_count"]; $i++)
		{
			if ($v['step18_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step18_field1_fieldset'][$i]["comment"],
					":data_relation" => "step18",
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
			} //$v['step18_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step18_field1_fieldset']["step18_comment_count"]; $i++
	}
	//$form_state['redirect'] = 'ddcl-bb-splice-end-plate/success';
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step19/' . $user->uid;
}
/***************************************/
function step18_comment_add_more_add_one($form, &$form_state)
{
	$form_state['step18_num_comment']++;
	$form_state['rebuild'] = TRUE;
	//$form_state['no_redirect'] = TRUE;
}
function step18_comment_add_more_remove_one($form, &$form_state)
{
	if ($form_state['step18_num_comment'] > 1)
	{
		$form_state['step18_num_comment']--;
	} //$form_state['step18_num_comment'] > 1
	$form_state['rebuild'] = TRUE;
}
function step18_comment_add_more_callback($form, &$form_state)
{
	return $form['step18_field1_fieldset'];
}
/***************************************/
/***************************************** step 18 form ends ****************************/
/************************ Step 19 form starts *********************/
function ddcl_bb_splice_end_plate_step19_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('i. Design of weld > ii. Size of weld at beam flange > 5. Stress on weld > 6. Size of weld (provided)'));
	$img_path = '';
	$markup_text = '
	
	&emsp;&emsp; <b><strong> 5. Stresses on weld </strong></b> <br>
	<br>
	&emsp;&emsp;&emsp;&emsp; <b> a. Direct stress (DS) </b> <br>
	<br>
		<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <mi>D</mi>
  <mi>S</mi>
  <mo>=</mo>
  <mn>0</mn>
</math>

	&emsp;&emsp;&emsp;&emsp; <b> There is no direct stress acting on weld at beam flange (assumed) </b> <br>

	<br>
	<br>
	&emsp;&emsp;&emsp;&emsp; <b> b. Bending stress (BS) </b> <br>
	<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <mi>B</mi>
  <mi>S</mi>
  <mo>=</mo>
  <msub>
    <mi>M</mi>
    <mi>u</mi>
  </msub>
  <mrow class="MJX-TeXAtom-ORD">
    <mo>/</mo>
  </mrow>
  <mi>Z</mi>
</math>
	<br>

	&emsp;&emsp;&emsp;&emsp;&emsp;
	where,  
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>M</mi>
								<mi>u</mi>
							</msub>
					</math> - factored bending moment
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	 <mi>Z</mi>
  <mo>=</mo>
  <msub>
    <mi>I</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>z</mi>
      <mi>z</mi>
    </mrow>
  </msub>
  <mrow class="MJX-TeXAtom-ORD">
    <mo>/</mo>
  </mrow>
  <mi>y</mi>
					</math> - section modulus of extreme fibre [Reference: Design of steel structures by N. Subramanian, table 6.7]

	<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>I</mi>
								<mi>zz</mi>
							</msub>
					</math> - polar moment of inertia of weld
	<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
				<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	
								<mi>y</mi>
							
					</math> = Beam depth /2	
		

 
	<br>
	<br>
	&emsp;&emsp;&emsp;&emsp; <b><strong> c. Resultant (R) </strong></b> <br>
	<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <mi>R</mi>
  <mo>=</mo>
  <msqrt>
    <mi>D</mi>
    <msup>
      <mi>S</mi>
      <mn>2</mn>
    </msup>
    <mo>+</mo>
    <mi>B</mi>
    <msup>
      <mi>S</mi>
      <mn>2</mn>
    </msup>
  </msqrt>
</math>
<br>
	&emsp;&emsp;&emsp;&emsp;&emsp;
	where,  
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						
								<mi>DS</mi>
							
					</math> - direct stress acting on weld (as calculated above)
				<br>
				
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	
								<mo>BS</mo>
							
					</math> - bending stress acting on weld (as calculated above)
				<br>
<hr>
&emsp;&emsp; <b><strong> 6. Size of weld (provided) </strong></b> <br>
	<br>
	<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>t</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>w</mi>
      <mi>e</mi>
      <mi>l</mi>
      <mi>d</mi>
    </mrow>
  </msub>
  <mo>=</mo>
  <mi>R</mi>
  <mi>e</mi>
  <mi>s</mi>
  <mi>u</mi>
  <mi>l</mi>
  <mi>t</mi>
  <mi>a</mi>
  <mi>n</mi>
  <mi>t</mi>
  <mtext>&#xA0;</mtext>
  <mrow class="MJX-TeXAtom-ORD">
    <mo>/</mo>
  </mrow>
  <mtext>&#xA0;</mtext>
  <mi>c</mi>
  <mi>a</mi>
  <mi>p</mi>
  <mi>a</mi>
  <mi>c</mi>
  <mi>i</mi>
  <mi>t</mi>
  <mi>y</mi>
  <mtext>&#xA0;</mtext>
  <mi>o</mi>
  <mi>f</mi>
  <mtext>&#xA0;</mtext>
  <mi>u</mi>
  <mi>n</mi>
  <mi>i</mi>
  <mi>t</mi>
  <mtext>&#xA0;</mtext>
  <mi>w</mi>
  <mi>e</mi>
  <mi>l</mi>
  <mi>d</mi>
</math>
<br>
&emsp;&emsp;&emsp;&emsp;&emsp;
	where,  
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						<msub>
								<mi>t</mi>
								<mi>weld</mi>
						</msub>	
					</math> - actual required weld thickness 
				<br>
	
	
	<hr>';
	// All the real form fields.
	$form = array();
	$form['step19_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step19-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_bb_splice_end_plate_form_step19_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step19_feedback');
	$query->condition('uid', $user->uid);
	$form_step19_data = $query->execute();
	if ($form_step19_data)
	{
		if ($step19_data = $form_step19_data->fetchObject())
		{
			$form['step19_field1_fieldset']['step19_field1_question1'] = array(
				'#markup' => $markup_text
			);
			$form['step19_field1_fieldset']['step19_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				'#default_value' => $step19_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step19_field1_fieldset']['step19_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				'#default_value' => $step19_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step19_field1_fieldset\\[step19_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			$earlier_comments = _fetch_existing_ddcl_bb_splice_end_plate_comments($step19_data->id, $step19_data->data_relation);
			$icom = 1;
			$form['step19_field1_fieldset']['existing_comments_title'] = array(
				'#markup' => 'Earlier comment<br>'
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step19_field1_fieldset']['existing_comments' . $icom] = array(
					'#markup' => $rowcomment
				);
				$icom++;
			} //$earlier_comments as $rowcomment
			if (empty($form_state['step19_num_comment']))
			{
				$form_state['step19_num_comment'] = 1;
			} //empty($form_state['step19_num_comment'])
			$step19_temp1 = 0;
			for ($step19_i = 0; $step19_i < $form_state['step19_num_comment']; $step19_i++)
			{
				$step19_temp1 = $step19_i;
				$form['step19_field1_fieldset'][$step19_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step19_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step19_field1_fieldset']["step19_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step19_temp1
			);
			$form['step19_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step19_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step19_comment_add_more_callback',
					'wrapper' => 'step19-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step19_num_comment'] > 1)
			{
				$form['step19_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step19_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step19_comment_add_more_callback',
						'wrapper' => 'step19-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step19_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step19_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step19_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step19_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step19_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step19_form_next_submit'
				)
			);
		} //$step19_data = $form_step19_data->fetchObject()
		else
		{
			$form['step19_field1_fieldset']['step19_field1_question1'] = array(
				'#markup' => $markup_text
			);
			$form['step19_field1_fieldset']['step19_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				//'#default_value' => $step19_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step19_field1_fieldset']['step19_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				//'#default_value' => $step19_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step19_field1_fieldset\\[step19_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			if (empty($form_state['step19_num_comment']))
			{
				$form_state['step19_num_comment'] = 1;
			} //empty($form_state['step19_num_comment'])
			$step19_temp1 = 0;
			for ($step19_i = 0; $step19_i < $form_state['step19_num_comment']; $step19_i++)
			{
				$step19_temp1 = $step19_i;
				$form['step19_field1_fieldset'][$step19_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step19_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step19_field1_fieldset']["step19_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step19_temp1
			);
			$form['step19_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step19_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step19_comment_add_more_callback',
					'wrapper' => 'step19-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step19_num_comment'] > 1)
			{
				$form['step19_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step19_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step19_comment_add_more_callback',
						'wrapper' => 'step19-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step19_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step19_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step19_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step19_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step19_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step19_form_next_submit'
				)
			);
		} //$step19_data = $form_step19_data->fetchObject()
	} //$form_step19_data
	return $form;
}
function ddcl_bb_splice_end_plate_step19_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step19_field1_fieldset']['step19_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step19_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step19_feedback');
	$query->condition('uid', $user->uid);
	$form_step19_data = $query->execute()->fetchObject();
	//var_dump($form_step19_data);die;
	if ($form_step19_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step19_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step19_field1_fieldset']['step19_field1_question1'],
			":question_ok_not_ok" => $v['step19_field1_fieldset']['step19_field1_question_ok'],
			":question_not_ok_comment" => $v['step19_field1_fieldset']['step19_field1_question_not_ok_comment'],
			":data_relation" => "step19"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step19_data->id
		));
		$form_step19_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step19_field1_fieldset']["step19_comment_count"]; $i++)
		{
			if ($v['step19_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step19_data->id,
					":number" => $i + $form_step19_comment_data->max + 1,
					":comment" => $v['step19_field1_fieldset'][$i]["comment"],
					":data_relation" => "step19",
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
			} //$v['step19_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step19_field1_fieldset']["step19_comment_count"]; $i++
	} //$form_step19_data
	else
	{
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step19_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1,  :question_ok_not_ok, :question_not_ok_comment,:data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step19_field1_fieldset']['step19_field1_question1'],
			":question_ok_not_ok" => $v['step19_field1_fieldset']['step19_field1_question_ok'],
			":question_not_ok_comment" => $v['step19_field1_fieldset']['step19_field1_question_not_ok_comment'],
			":data_relation" => "step19",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
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
		for ($i = 0; $i <= $v['step19_field1_fieldset']["step19_comment_count"]; $i++)
		{
			if ($v['step19_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step19_field1_fieldset'][$i]["comment"],
					":data_relation" => "step19",
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
			} //$v['step19_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step19_field1_fieldset']["step19_comment_count"]; $i++
	}
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step18/' . $user->uid;
}
function ddcl_bb_splice_end_plate_step19_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step19_field1_fieldset']['step19_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step19_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step19_feedback');
	$query->condition('uid', $user->uid);
	$form_step19_data = $query->execute()->fetchObject();
	//var_dump($form_step19_data);die;
	if ($form_step19_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step19_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step19_field1_fieldset']['step19_field1_question1'],
			":question_ok_not_ok" => $v['step19_field1_fieldset']['step19_field1_question_ok'],
			":question_not_ok_comment" => $v['step19_field1_fieldset']['step19_field1_question_not_ok_comment'],
			":data_relation" => "step19"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step7_data->id
		));
		$form_step19_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step19_field1_fieldset']["step19_comment_count"]; $i++)
		{
			if ($v['step19_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step19_data->id,
					":number" => $i + $form_step19_comment_data->max + 1,
					":comment" => $v['step19_field1_fieldset'][$i]["comment"],
					":data_relation" => "step19",
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
			} //$v['step19_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step19_field1_fieldset']["step19_comment_count"]; $i++
	} //$form_step19_data
	else
	{
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step19_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok,:question_not_ok_comment, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step19_field1_fieldset']['step19_field1_question1'],
			":question_ok_not_ok" => $v['step19_field1_fieldset']['step19_field1_question_ok'],
			":question_not_ok_comment" => $v['step19_field1_fieldset']['step19_field1_question_not_ok_comment'],
			":data_relation" => "step19",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
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
		for ($i = 0; $i <= $v['step19_field1_fieldset']["step19_comment_count"]; $i++)
		{
			if ($v['step19_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step19_field1_fieldset'][$i]["comment"],
					":data_relation" => "step19",
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
			} //$v['step19_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step19_field1_fieldset']["step19_comment_count"]; $i++
	}
	//$form_state['redirect'] = 'ddcl-bb-splice-end-plate/success';
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step20/' . $user->uid;
}
/***************************************/
function step19_comment_add_more_add_one($form, &$form_state)
{
	$form_state['step19_num_comment']++;
	$form_state['rebuild'] = TRUE;
	//$form_state['no_redirect'] = TRUE;
}
function step19_comment_add_more_remove_one($form, &$form_state)
{
	if ($form_state['step19_num_comment'] > 1)
	{
		$form_state['step19_num_comment']--;
	} //$form_state['step19_num_comment'] > 1
	$form_state['rebuild'] = TRUE;
}
function step19_comment_add_more_callback($form, &$form_state)
{
	return $form['step19_field1_fieldset'];
}
/***************************************/
/***************************************** step 19 form ends ****************************/
/************************ Step 20 form starts *********************/
function ddcl_bb_splice_end_plate_step20_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('i. Design of weld > iii. Size of weld at beam web > 1. Minimum weld thickness > 2. Size of weld (provided)'));
	$img_path = '';
	$markup_text = '
	&emsp;&emsp; <b><strong> iii. Size of weld at beam web </strong></b> <br>

	<hr>
	&emsp;&emsp;&emsp; <b><strong> 1. Minimum weld thickness ( t<sub>w<sub>minimum</sub></sub> ) </strong> - for dropdown list of weld thickness in Gui [Reference: Table 21, IS 800] <br>
	&emsp;&emsp;&emsp;&emsp;  Minimum weld thickness depends on the thickness of the thicker part 
	<br>
	&emsp;&emsp;&emsp;&emsp; For weld at web <math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>t</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>t</mi>
      <mi>h</mi>
      <mi>i</mi>
      <mi>c</mi>
      <mi>k</mi>
      <mi>e</mi>
      <mi>r</mi>
    </mrow>
  </msub>
  <mo>=</mo>
  <mi>m</mi>
  <mi>a</mi>
  <mi>x</mi>
  <mi>i</mi>
  <mi>m</mi>
  <mi>u</mi>
  <mi>m</mi>
  <mo stretchy="false">(</mo>
  <msub>
    <mi>t</mi>
    <mi>w</mi>
  </msub>
  <mo>,</mo>
  <msub>
    <mi>t</mi>
    <mi>p</mi>
  </msub>
  <mo stretchy="false">)</mo>
</math>
</math> <br>


<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <mi>I</mi>
  <mi>f</mi>
  <mo>,</mo>
  <msub>
    <mi>t</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>t</mi>
      <mi>h</mi>
      <mi>i</mi>
      <mi>c</mi>
      <mi>k</mi>
      <mi>e</mi>
      <mi>r</mi>
    </mrow>
  </msub>
  <mo>&#x2264;</mo>
  <mn>10</mn>
  <mi>m</mi>
  <mi>m</mi>
  <mo>,</mo>
  <msub>
    <mi>t</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <msub>
        <mi>w</mi>
        <mrow class="MJX-TeXAtom-ORD">
          <mi>m</mi>
          <mi>i</mi>
          <mi>n</mi>
          <mi>i</mi>
          <mi>m</mi>
          <mi>u</mi>
          <mi>m</mi>
        </mrow>
      </msub>
    </mrow>
  </msub>
  <mo>=</mo>
  <mn>3</mn>
  <mi>m</mi>
  <mi>m</mi>
</math>
<br>

<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <mn>10</mn>
  <mi>m</mi>
  <mi>m</mi>
  <mo>&#x2264;</mo>
  <msub>
    <mi>t</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>t</mi>
      <mi>h</mi>
      <mi>i</mi>
      <mi>c</mi>
      <mi>k</mi>
      <mi>e</mi>
      <mi>r</mi>
    </mrow>
  </msub>
  <mo>&#x2264;</mo>
  <mn>20</mn>
  <mi>m</mi>
  <mi>m</mi>
  <mo>,</mo>
  <msub>
    <mi>t</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <msub>
        <mi>w</mi>
        <mrow class="MJX-TeXAtom-ORD">
          <mi>m</mi>
          <mi>i</mi>
          <mi>n</mi>
          <mi>i</mi>
          <mi>m</mi>
          <mi>u</mi>
          <mi>m</mi>
        </mrow>
      </msub>
    </mrow>
  </msub>
  <mo>=</mo>
  <mn>5</mn>
  <mi>m</mi>
  <mi>m</mi>
</math>
<br>

<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <mn>20</mn>
  <mi>m</mi>
  <mi>m</mi>
  <mo>&#x2264;</mo>
  <msub>
    <mi>t</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>t</mi>
      <mi>h</mi>
      <mi>i</mi>
      <mi>c</mi>
      <mi>k</mi>
      <mi>e</mi>
      <mi>r</mi>
    </mrow>
  </msub>
  <mo>&#x2264;</mo>
  <mn>32</mn>
  <mi>m</mi>
  <mi>m</mi>
  <mo>,</mo>
  <msub>
    <mi>t</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <msub>
        <mi>w</mi>
        <mrow class="MJX-TeXAtom-ORD">
          <mi>m</mi>
          <mi>i</mi>
          <mi>n</mi>
          <mi>i</mi>
          <mi>m</mi>
          <mi>u</mi>
          <mi>m</mi>
        </mrow>
      </msub>
    </mrow>
  </msub>
  <mo>=</mo>
  <mn>6</mn>
  <mi>m</mi>
  <mi>m</mi>
</math>

<br>
<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <mn>32</mn>
  <mi>m</mi>
  <mi>m</mi>
  <mo>&#x2264;</mo>
  <msub>
    <mi>t</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>t</mi>
      <mi>h</mi>
      <mi>i</mi>
      <mi>c</mi>
      <mi>k</mi>
      <mi>e</mi>
      <mi>r</mi>
    </mrow>
  </msub>
  <mo>&#x2264;</mo>
  <mn>50</mn>
  <mi>m</mi>
  <mi>m</mi>
  <mo>,</mo>
  <msub>
    <mi>t</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <msub>
        <mi>w</mi>
        <mrow class="MJX-TeXAtom-ORD">
          <mi>m</mi>
          <mi>i</mi>
          <mi>n</mi>
          <mi>i</mi>
          <mi>m</mi>
          <mi>u</mi>
          <mi>m</mi>
        </mrow>
      </msub>
    </mrow>
  </msub>
  <mo>=</mo>
  <mn>8</mn>
  <mi>m</mi>
  <mi>m</mi>
</math><br>
<hr>

	&emsp;&emsp;&emsp; <b> 2. Size of weld ( provided ) <br></b>
	<br>

	<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>t</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>w</mi>
      <mi>e</mi>
      <mi>l</mi>
      <mi>d</mi>
    </mrow>
  </msub>
  <mo>=</mo>
  <mi>m</mi>
  <mi>i</mi>
  <mi>n</mi>
  <mi>i</mi>
  <mi>m</mi>
  <mi>u</mi>
  <mi>m</mi>
  <mtext>&#xA0;</mtext>
  <mo stretchy="false">(</mo>
  <msub>
    <mi>t</mi>
    <mi>w</mi>
  </msub>
  <mo>,</mo>
  <mtext>&#xA0;</mtext>
  <msub>
    <mi>t</mi>
    <mi>p</mi>
  </msub>
  <mo>,</mo>
  <mtext>&#xA0;</mtext>
  <msub>
    <mi>t</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>w</mi>
      <mi>e</mi>
      <mi>l</mi>
      <msub>
        <mi>d</mi>
        <mrow class="MJX-TeXAtom-ORD">
          <mi>f</mi>
          <mi>l</mi>
          <mi>a</mi>
          <mi>n</mi>
          <mi>g</mi>
          <mi>e</mi>
        </mrow>
      </msub>
    </mrow>
  </msub>
  <mo stretchy="false">)</mo>
</math>
&emsp;&emsp;&emsp;&emsp;&emsp;
	where,  
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML" align="left">
  <msub>
    <mi>t</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>w</mi>
      <mi>e</mi>
      <mi>l</mi>
      <msub>
        <mi>d</mi>
        <mrow class="MJX-TeXAtom-ORD">
          <mi>f</mi>
          <mi>l</mi>
          <mi>a</mi>
          <mi>n</mi>
          <mi>g</mi>
          <mi>e</mi>
        </mrow>
      </msub>
    </mrow>
  </msub>
</math> - weld provided at beam flange
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>t</mi>
								<mi>w</mi>
							</msub>
					</math> - thickness of beam web	
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						  	<msub>
								<mi>t</mi>
								<mi>p</mi>
							</msub>
					</math> - thickness of end plate	

	
	<hr>';
	// All the real form fields.
	$form = array();
	$form['step20_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step20-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_bb_splice_end_plate_form_step20_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step20_feedback');
	$query->condition('uid', $user->uid);
	$form_step20_data = $query->execute();
	if ($form_step20_data)
	{
		if ($step20_data = $form_step20_data->fetchObject())
		{
			$form['step20_field1_fieldset']['step20_field1_question1'] = array(
				'#markup' => $markup_text
			);
			$form['step20_field1_fieldset']['step20_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				'#default_value' => $step20_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step20_field1_fieldset']['step20_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				'#default_value' => $step20_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step20_field1_fieldset\\[step20_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			$earlier_comments = _fetch_existing_ddcl_bb_splice_end_plate_comments($step20_data->id, $step20_data->data_relation);
			$icom = 1;
			$form['step20_field1_fieldset']['existing_comments_title'] = array(
				'#markup' => 'Earlier comment<br>'
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step20_field1_fieldset']['existing_comments' . $icom] = array(
					'#markup' => $rowcomment
				);
				$icom++;
			} //$earlier_comments as $rowcomment
			if (empty($form_state['step20_num_comment']))
			{
				$form_state['step20_num_comment'] = 1;
			} //empty($form_state['step20_num_comment'])
			$step20_temp1 = 0;
			for ($step20_i = 0; $step20_i < $form_state['step20_num_comment']; $step20_i++)
			{
				$step20_temp1 = $step20_i;
				$form['step20_field1_fieldset'][$step20_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step20_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step20_field1_fieldset']["step20_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step20_temp1
			);
			$form['step20_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step20_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step20_comment_add_more_callback',
					'wrapper' => 'step20-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step20_num_comment'] > 1)
			{
				$form['step20_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step20_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step20_comment_add_more_callback',
						'wrapper' => 'step20-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step20_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step20_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step20_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step20_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step20_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step20_form_next_submit'
				)
			);
		} //$step20_data = $form_step20_data->fetchObject()
		else
		{
			$form['step20_field1_fieldset']['step20_field1_question1'] = array(
				'#markup' => $markup_text
			);
			$form['step20_field1_fieldset']['step20_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				//'#default_value' => $step20_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step20_field1_fieldset']['step20_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				//'#default_value' => $step20_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step20_field1_fieldset\\[step20_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			if (empty($form_state['step20_num_comment']))
			{
				$form_state['step20_num_comment'] = 1;
			} //empty($form_state['step20_num_comment'])
			$step20_temp1 = 0;
			for ($step20_i = 0; $step20_i < $form_state['step20_num_comment']; $step20_i++)
			{
				$step20_temp1 = $step20_i;
				$form['step20_field1_fieldset'][$step20_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step20_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step20_field1_fieldset']["step20_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step20_temp1
			);
			$form['step20_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step20_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step20_comment_add_more_callback',
					'wrapper' => 'step20-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step20_num_comment'] > 1)
			{
				$form['step20_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step20_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step20_comment_add_more_callback',
						'wrapper' => 'step20-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step20_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step20_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step20_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step20_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step20_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step20_form_next_submit'
				)
			);
		} //$step20_data = $form_step20_data->fetchObject()
	} //$form_step20_data
	return $form;
}
function ddcl_bb_splice_end_plate_step20_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step20_field1_fieldset']['step20_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step20_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step20_feedback');
	$query->condition('uid', $user->uid);
	$form_step20_data = $query->execute()->fetchObject();
	//var_dump($form_step20_data);die;
	if ($form_step20_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step20_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step20_field1_fieldset']['step20_field1_question1'],
			":question_ok_not_ok" => $v['step20_field1_fieldset']['step20_field1_question_ok'],
			":question_not_ok_comment" => $v['step20_field1_fieldset']['step20_field1_question_not_ok_comment'],
			":data_relation" => "step20"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step20_data->id
		));
		$form_step20_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step20_field1_fieldset']["step20_comment_count"]; $i++)
		{
			if ($v['step20_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step20_data->id,
					":number" => $i + $form_step20_comment_data->max + 1,
					":comment" => $v['step20_field1_fieldset'][$i]["comment"],
					":data_relation" => "step20",
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
			} //$v['step20_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step20_field1_fieldset']["step20_comment_count"]; $i++
	} //$form_step20_data
	else
	{
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step20_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1,  :question_ok_not_ok, :question_not_ok_comment,:data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step20_field1_fieldset']['step20_field1_question1'],
			":question_ok_not_ok" => $v['step20_field1_fieldset']['step20_field1_question_ok'],
			":question_not_ok_comment" => $v['step20_field1_fieldset']['step20_field1_question_not_ok_comment'],
			":data_relation" => "step20",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
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
		for ($i = 0; $i <= $v['step20_field1_fieldset']["step20_comment_count"]; $i++)
		{
			if ($v['step20_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step20_field1_fieldset'][$i]["comment"],
					":data_relation" => "step20",
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
			} //$v['step20_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step20_field1_fieldset']["step20_comment_count"]; $i++
	}
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step19/' . $user->uid;
}
function ddcl_bb_splice_end_plate_step20_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step20_field1_fieldset']['step20_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step20_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step20_feedback');
	$query->condition('uid', $user->uid);
	$form_step20_data = $query->execute()->fetchObject();
	//var_dump($form_step20_data);die;
	if ($form_step20_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step20_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step20_field1_fieldset']['step20_field1_question1'],
			":question_ok_not_ok" => $v['step20_field1_fieldset']['step20_field1_question_ok'],
			":question_not_ok_comment" => $v['step20_field1_fieldset']['step20_field1_question_not_ok_comment'],
			":data_relation" => "step20"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step7_data->id
		));
		$form_step20_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step20_field1_fieldset']["step20_comment_count"]; $i++)
		{
			if ($v['step20_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step20_data->id,
					":number" => $i + $form_step20_comment_data->max + 1,
					":comment" => $v['step20_field1_fieldset'][$i]["comment"],
					":data_relation" => "step20",
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
			} //$v['step20_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step20_field1_fieldset']["step20_comment_count"]; $i++
	} //$form_step20_data
	else
	{
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step20_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok,:question_not_ok_comment, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step20_field1_fieldset']['step20_field1_question1'],
			":question_ok_not_ok" => $v['step20_field1_fieldset']['step20_field1_question_ok'],
			":question_not_ok_comment" => $v['step20_field1_fieldset']['step20_field1_question_not_ok_comment'],
			":data_relation" => "step20",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
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
		for ($i = 0; $i <= $v['step20_field1_fieldset']["step20_comment_count"]; $i++)
		{
			if ($v['step20_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step20_field1_fieldset'][$i]["comment"],
					":data_relation" => "step20",
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
			} //$v['step20_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step20_field1_fieldset']["step20_comment_count"]; $i++
	}
	//$form_state['redirect'] = 'ddcl-bb-splice-end-plate/success';
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step21/' . $user->uid;
}
/***************************************/
function step20_comment_add_more_add_one($form, &$form_state)
{
	$form_state['step20_num_comment']++;
	$form_state['rebuild'] = TRUE;
	//$form_state['no_redirect'] = TRUE;
}
function step20_comment_add_more_remove_one($form, &$form_state)
{
	if ($form_state['step20_num_comment'] > 1)
	{
		$form_state['step20_num_comment']--;
	} //$form_state['step20_num_comment'] > 1
	$form_state['rebuild'] = TRUE;
}
function step20_comment_add_more_callback($form, &$form_state)
{
	return $form['step20_field1_fieldset'];
}
/***************************************/
/***************************************** step 20 form ends ****************************/
/************************ Step 21 form starts *********************/
function ddcl_bb_splice_end_plate_step21_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('j. Weld checks > i. Maximum weld thickness > 1. at flange > 2. at web > ii. Check for stresses in weld due to individual force (weld at flange) > 1. Normal stress > 2. Shear stress > 3. Combined stress'));
	$img_path = '';
	$markup_text = '
	<b><strong> j. Weld checks </strong></b> <br>
	<hr>
	&emsp;&emsp; <b><strong> i. Maximum weld thickness </strong></b> [Reference: Cl 10.5.3, IS 800] <br>
	<br>
	&emsp;&emsp;&emsp; <b> 1. Weld connecting beam flange to end plate </b> <br>
	<br>
		<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>t</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>w</mi>
      <mi>e</mi>
      <mi>l</mi>
      <mi>d</mi>
    </mrow>
  </msub>
  <mo>&#x2264;<!-- ≤ --></mo>
  <mi>m</mi>
  <mi>i</mi>
  <mi>n</mi>
  <mi>i</mi>
  <mi>m</mi>
  <mi>u</mi>
  <mi>m</mi>
  <mo stretchy="false">(</mo>
  <msub>
    <mi>T</mi>
    <mi>f</mi>
  </msub>
  <mtext>&#xA0;</mtext>
  <mo>,</mo>
  <mtext>&#xA0;</mtext>
  <msub>
    <mi>t</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>p</mi>
    </mrow>
  </msub>
  <mo stretchy="false">)</mo>
</math>
<br>
	&emsp;&emsp;&emsp; <b> 2. Weld connecting beam web to end plate </b> <br>
	<br>
	<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>t</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>w</mi>
      <mi>e</mi>
      <mi>l</mi>
      <mi>d</mi>
    </mrow>
  </msub>
  <mo>&#x2264;<!-- ≤ --></mo>
  <mi>m</mi>
  <mi>i</mi>
  <mi>n</mi>
  <mi>i</mi>
  <mi>m</mi>
  <mi>u</mi>
  <mi>m</mi>
  <mo stretchy="false">(</mo>
  <msub>
    <mi>t</mi>
    <mi>w</mi>
  </msub>
  <mtext>&#xA0;</mtext>
  <mo>,</mo>
  <mtext>&#xA0;</mtext>
  <msub>
    <mi>t</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>p</mi>
    </mrow>
  </msub>
  <mo stretchy="false">)</mo>
</math>
<hr>
	&emsp;&emsp; <b><strong> ii. Check for stresses in weld due to individual forces (weld at flange) </strong> [Reference: Cl 10.5.9, IS 800] </b> <br>
	<br>
	&emsp;&emsp;&emsp; <b> 1. Check for normal stress </b> <br>
	<br>
		<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>f</mi>
    <mi>a</mi>
  </msub>
  <mo>=</mo>
  <msub>
    <mi>F</mi>
    <mi>c</mi>
  </msub>
  <mtext>&#xA0;</mtext>
  <mrow class="MJX-TeXAtom-ORD">
    <mo>/</mo>
  </mrow>
  <mtext>&#xA0;</mtext>
  <msub>
    <mi>t</mi>
    <mi>t</mi>
  </msub>
  <msub>
    <mi>l</mi>
    <mi>w</mi>
  </msub>
</math>
<br>
&emsp;&emsp;&emsp;&emsp;&emsp;
	where,  
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						<msub>
								<mi>F</mi>
								<mi>c</mi>
						</msub>	
					</math> - Force in flange 
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						<msub>
								<mi>t</mi>
								<mi>t</mi>
						</msub>	
					</math> - effective throat thickness of weld 
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						<msub>
								<mi>l</mi>
								<mi>w</mi>
						</msub>	
					</math> - effective length of weld at flange 
				<br>

	
	<hr>';
	// All the real form fields.
	$form = array();
	$form['step21_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step21-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_bb_splice_end_plate_form_step21_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step21_feedback');
	$query->condition('uid', $user->uid);
	$form_step21_data = $query->execute();
	if ($form_step21_data)
	{
		if ($step21_data = $form_step21_data->fetchObject())
		{
			$form['step21_field1_fieldset']['step21_field1_question1'] = array(
				'#markup' => $markup_text
			);
			$form['step21_field1_fieldset']['step21_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				'#default_value' => $step21_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step21_field1_fieldset']['step21_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				'#default_value' => $step21_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step21_field1_fieldset\\[step21_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			$earlier_comments = _fetch_existing_ddcl_bb_splice_end_plate_comments($step21_data->id, $step21_data->data_relation);
			$icom = 1;
			$form['step21_field1_fieldset']['existing_comments_title'] = array(
				'#markup' => 'Earlier comment<br>'
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step21_field1_fieldset']['existing_comments' . $icom] = array(
					'#markup' => $rowcomment
				);
				$icom++;
			} //$earlier_comments as $rowcomment
			if (empty($form_state['step21_num_comment']))
			{
				$form_state['step21_num_comment'] = 1;
			} //empty($form_state['step21_num_comment'])
			$step21_temp1 = 0;
			for ($step21_i = 0; $step21_i < $form_state['step21_num_comment']; $step21_i++)
			{
				$step21_temp1 = $step21_i;
				$form['step21_field1_fieldset'][$step21_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step21_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step21_field1_fieldset']["step21_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step21_temp1
			);
			$form['step21_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step21_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step21_comment_add_more_callback',
					'wrapper' => 'step21-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step21_num_comment'] > 1)
			{
				$form['step21_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step21_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step21_comment_add_more_callback',
						'wrapper' => 'step21-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step21_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step21_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step21_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step21_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step21_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step21_form_next_submit'
				)
			);
		} //$step21_data = $form_step21_data->fetchObject()
		else
		{
			$form['step21_field1_fieldset']['step21_field1_question1'] = array(
				'#markup' => $markup_text
			);
			$form['step21_field1_fieldset']['step21_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				//'#default_value' => $step21_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step21_field1_fieldset']['step21_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				//'#default_value' => $step21_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step21_field1_fieldset\\[step21_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			if (empty($form_state['step21_num_comment']))
			{
				$form_state['step21_num_comment'] = 1;
			} //empty($form_state['step21_num_comment'])
			$step21_temp1 = 0;
			for ($step21_i = 0; $step21_i < $form_state['step21_num_comment']; $step21_i++)
			{
				$step21_temp1 = $step21_i;
				$form['step21_field1_fieldset'][$step21_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step21_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step21_field1_fieldset']["step21_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step21_temp1
			);
			$form['step21_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step21_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step21_comment_add_more_callback',
					'wrapper' => 'step21-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step21_num_comment'] > 1)
			{
				$form['step21_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step21_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step21_comment_add_more_callback',
						'wrapper' => 'step21-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step21_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step21_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step21_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step21_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step21_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step21_form_next_submit'
				)
			);
		} //$step21_data = $form_step21_data->fetchObject()
	} //$form_step21_data
	return $form;
}
function ddcl_bb_splice_end_plate_step21_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step21_field1_fieldset']['step21_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step21_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step21_feedback');
	$query->condition('uid', $user->uid);
	$form_step21_data = $query->execute()->fetchObject();
	//var_dump($form_step21_data);die;
	if ($form_step21_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step21_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step21_field1_fieldset']['step21_field1_question1'],
			":question_ok_not_ok" => $v['step21_field1_fieldset']['step21_field1_question_ok'],
			":question_not_ok_comment" => $v['step21_field1_fieldset']['step21_field1_question_not_ok_comment'],
			":data_relation" => "step21"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step21_data->id
		));
		$form_step21_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step21_field1_fieldset']["step21_comment_count"]; $i++)
		{
			if ($v['step21_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step21_data->id,
					":number" => $i + $form_step21_comment_data->max + 1,
					":comment" => $v['step21_field1_fieldset'][$i]["comment"],
					":data_relation" => "step21",
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
			} //$v['step21_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step21_field1_fieldset']["step21_comment_count"]; $i++
	} //$form_step21_data
	else
	{
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step21_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1,  :question_ok_not_ok, :question_not_ok_comment,:data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step21_field1_fieldset']['step21_field1_question1'],
			":question_ok_not_ok" => $v['step21_field1_fieldset']['step21_field1_question_ok'],
			":question_not_ok_comment" => $v['step21_field1_fieldset']['step21_field1_question_not_ok_comment'],
			":data_relation" => "step21",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
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
		for ($i = 0; $i <= $v['step21_field1_fieldset']["step21_comment_count"]; $i++)
		{
			if ($v['step21_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step21_field1_fieldset'][$i]["comment"],
					":data_relation" => "step21",
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
			} //$v['step21_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step21_field1_fieldset']["step21_comment_count"]; $i++
	}
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step20/' . $user->uid;
}
function ddcl_bb_splice_end_plate_step21_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step21_field1_fieldset']['step21_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step21_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step21_feedback');
	$query->condition('uid', $user->uid);
	$form_step21_data = $query->execute()->fetchObject();
	//var_dump($form_step21_data);die;
	if ($form_step21_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step21_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step21_field1_fieldset']['step21_field1_question1'],
			":question_ok_not_ok" => $v['step21_field1_fieldset']['step21_field1_question_ok'],
			":question_not_ok_comment" => $v['step21_field1_fieldset']['step21_field1_question_not_ok_comment'],
			":data_relation" => "step21"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step7_data->id
		));
		$form_step21_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step21_field1_fieldset']["step21_comment_count"]; $i++)
		{
			if ($v['step21_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step21_data->id,
					":number" => $i + $form_step21_comment_data->max + 1,
					":comment" => $v['step21_field1_fieldset'][$i]["comment"],
					":data_relation" => "step21",
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
			} //$v['step21_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step21_field1_fieldset']["step21_comment_count"]; $i++
	} //$form_step21_data
	else
	{
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step21_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok,:question_not_ok_comment, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step21_field1_fieldset']['step21_field1_question1'],
			":question_ok_not_ok" => $v['step21_field1_fieldset']['step21_field1_question_ok'],
			":question_not_ok_comment" => $v['step21_field1_fieldset']['step21_field1_question_not_ok_comment'],
			":data_relation" => "step21",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
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
		for ($i = 0; $i <= $v['step21_field1_fieldset']["step21_comment_count"]; $i++)
		{
			if ($v['step21_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step21_field1_fieldset'][$i]["comment"],
					":data_relation" => "step21",
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
			} //$v['step21_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step21_field1_fieldset']["step21_comment_count"]; $i++
	}
	//$form_state['redirect'] = 'ddcl-bb-splice-end-plate/success';
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step22/' . $user->uid;
}
/***************************************/
function step21_comment_add_more_add_one($form, &$form_state)
{
	$form_state['step21_num_comment']++;
	$form_state['rebuild'] = TRUE;
	//$form_state['no_redirect'] = TRUE;
}
function step21_comment_add_more_remove_one($form, &$form_state)
{
	if ($form_state['step21_num_comment'] > 1)
	{
		$form_state['step21_num_comment']--;
	} //$form_state['step21_num_comment'] > 1
	$form_state['rebuild'] = TRUE;
}
function step21_comment_add_more_callback($form, &$form_state)
{
	return $form['step21_field1_fieldset'];
}
/***************************************/
/***************************************** step 21 form ends ****************************/

/************************ Step 22 form starts *********************/
function ddcl_bb_splice_end_plate_step22_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('j. Weld checks > ii. Check for stresses in weld due to individual force (weld at beam web) > 1. Normal stress > 2. Shear stress > 3. Combined stress '));
	$img_path = '';
	$markup_text = '
	<b><strong> j. Weld checks </strong></b> <br>
	<hr>
	&emsp;&emsp; <b><strong> ii. Check for stresses in weld due to individual forces (weld at beam web) </strong> [Reference: Cl 10.5.9, IS 800] </b> <br>
	<br>
	&emsp;&emsp;&emsp; <b> 1. Check for normal stress </b> <br>
	<br>
		<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>f</mi>
    <mi>a</mi>
  </msub>
  <mo>=</mo>
  
    <mi>H</mi>
    
  
  <mtext>&#xA0;</mtext>
  <mrow class="MJX-TeXAtom-ORD">
    <mo>/</mo>
  </mrow>
  <mtext>&#xA0;</mtext>
  <msub>
    <mi>t</mi>
    <mi>t</mi>
  </msub>
  <msub>
    <mi>l</mi>
    <mi>w</mi>
  </msub>
</math>
<br>
&emsp;&emsp;&emsp;&emsp;&emsp;
	where,  
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						
								<mi>H</mi>
								
						
					</math> - factored applied axial force 
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						<msub>
								<mi>t</mi>
								<mi>t</mi>
						</msub>	
					</math> - effective throat thickness of weld 
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						<msub>
								<mi>l</mi>
								<mi>w</mi>
						</msub>	
					</math> - effective length of weld at beam web 
				<br>
	<br>
	&emsp;&emsp;&emsp; <b> 2. Check for shear stress </b> <br>
	<br>
	<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <mi>q</mi>
  <mo>=</mo>
  <mi>V</mi>
  <mrow class="MJX-TeXAtom-ORD">
    <mo>/</mo>
  </mrow>
  <mtext>&#xA0;</mtext>
  <msub>
    <mi>t</mi>
    <mi>t</mi>
  </msub>
  <msub>
    <mi>l</mi>
    <mi>w</mi>
  </msub>
</math>
<br>
&emsp;&emsp;&emsp;&emsp;&emsp;
	where,  
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">

								<mi>V</mi>


					</math> - factored shear force 
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						<msub>
								<mi>t</mi>
								<mi>t</mi>
						</msub>	
					</math> - effective throat thickness of weld 
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						<msub>
								<mi>l</mi>
								<mi>w</mi>
						</msub>	
					</math> - effective length of weld at beam web 
<br>
<br>
	&emsp;&emsp;&emsp; <b> 3. Combination of stress [Reference: Cl 10.5.10.1.1, IS 800]</b> <br>
	<br>
	<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>f</mi>
    <mi>e</mi>
  </msub>
  <mo>&#x2264;<!-- ≤ --></mo>
  <msub>
    <mi>f</mi>
    <mi>u</mi>
  </msub>
  <mrow class="MJX-TeXAtom-ORD">
    <mo>/</mo>
  </mrow>
  <msqrt>
    <mn>3</mn>
  </msqrt>
  <mtext>&#xA0;</mtext>
  <msub>
    <mi>&#x03B3;<!-- γ --></mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>m</mi>
      <mi>w</mi>
    </mrow>
  </msub>
</math>
&emsp;&emsp;&emsp;&emsp;&emsp;
	where,  
				<br>
				<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>f</mi>
    <mi>e</mi>
  </msub>
  <mo>=</mo>
  <msqrt>
    <msubsup>
      <mi>f</mi>
      <mi>a</mi>
      <mn>2</mn>
    </msubsup>
    <mo>+</mo>
    <mn>3</mn>
    <msup>
      <mi>q</mi>
      <mn>2</mn>
    </msup>
  </msqrt>
</math>

		
	<hr>';
	// All the real form fields.
	$form = array();
	$form['step22_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step22-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_bb_splice_end_plate_form_step22_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step22_feedback');
	$query->condition('uid', $user->uid);
	$form_step22_data = $query->execute();
	if ($form_step22_data)
	{
		if ($step22_data = $form_step22_data->fetchObject())
		{
			$form['step22_field1_fieldset']['step22_field1_question1'] = array(
				'#markup' => $markup_text
			);
			$form['step22_field1_fieldset']['step22_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				'#default_value' => $step22_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step22_field1_fieldset']['step22_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				'#default_value' => $step22_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step22_field1_fieldset\\[step22_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			$earlier_comments = _fetch_existing_ddcl_bb_splice_end_plate_comments($step22_data->id, $step22_data->data_relation);
			$icom = 1;
			$form['step22_field1_fieldset']['existing_comments_title'] = array(
				'#markup' => 'Earlier comment<br>'
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step22_field1_fieldset']['existing_comments' . $icom] = array(
					'#markup' => $rowcomment
				);
				$icom++;
			} //$earlier_comments as $rowcomment
			if (empty($form_state['step22_num_comment']))
			{
				$form_state['step22_num_comment'] = 1;
			} //empty($form_state['step22_num_comment'])
			$step22_temp1 = 0;
			for ($step22_i = 0; $step22_i < $form_state['step22_num_comment']; $step22_i++)
			{
				$step22_temp1 = $step22_i;
				$form['step22_field1_fieldset'][$step22_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step22_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step22_field1_fieldset']["step22_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step22_temp1
			);
			$form['step22_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step22_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step22_comment_add_more_callback',
					'wrapper' => 'step22-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step22_num_comment'] > 1)
			{
				$form['step22_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step22_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step22_comment_add_more_callback',
						'wrapper' => 'step22-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step22_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step22_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step22_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step22_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step22_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step22_form_next_submit'
				)
			);
		} //$step22_data = $form_step22_data->fetchObject()
		else
		{
			$form['step22_field1_fieldset']['step22_field1_question1'] = array(
				'#markup' => $markup_text
			);
			$form['step22_field1_fieldset']['step22_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				//'#default_value' => $step22_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step22_field1_fieldset']['step22_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				//'#default_value' => $step22_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step22_field1_fieldset\\[step22_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			if (empty($form_state['step22_num_comment']))
			{
				$form_state['step22_num_comment'] = 1;
			} //empty($form_state['step22_num_comment'])
			$step22_temp1 = 0;
			for ($step22_i = 0; $step22_i < $form_state['step22_num_comment']; $step22_i++)
			{
				$step22_temp1 = $step22_i;
				$form['step22_field1_fieldset'][$step22_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step22_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step22_field1_fieldset']["step22_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step22_temp1
			);
			$form['step22_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step22_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step22_comment_add_more_callback',
					'wrapper' => 'step22-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step22_num_comment'] > 1)
			{
				$form['step22_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step22_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step22_comment_add_more_callback',
						'wrapper' => 'step22-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step22_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step22_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step22_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step22_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step22_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step22_form_next_submit'
				)
			);
		} //$step22_data = $form_step22_data->fetchObject()
	} //$form_step22_data
	return $form;
}
function ddcl_bb_splice_end_plate_step22_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step22_field1_fieldset']['step22_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step22_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step22_feedback');
	$query->condition('uid', $user->uid);
	$form_step22_data = $query->execute()->fetchObject();
	//var_dump($form_step22_data);die;
	if ($form_step22_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step22_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step22_field1_fieldset']['step22_field1_question1'],
			":question_ok_not_ok" => $v['step22_field1_fieldset']['step22_field1_question_ok'],
			":question_not_ok_comment" => $v['step22_field1_fieldset']['step22_field1_question_not_ok_comment'],
			":data_relation" => "step22"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step22_data->id
		));
		$form_step22_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step22_field1_fieldset']["step22_comment_count"]; $i++)
		{
			if ($v['step22_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step22_data->id,
					":number" => $i + $form_step22_comment_data->max + 1,
					":comment" => $v['step22_field1_fieldset'][$i]["comment"],
					":data_relation" => "step22",
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
			} //$v['step22_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step22_field1_fieldset']["step22_comment_count"]; $i++
	} //$form_step22_data
	else
	{
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step22_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1,  :question_ok_not_ok, :question_not_ok_comment,:data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step22_field1_fieldset']['step22_field1_question1'],
			":question_ok_not_ok" => $v['step22_field1_fieldset']['step22_field1_question_ok'],
			":question_not_ok_comment" => $v['step22_field1_fieldset']['step22_field1_question_not_ok_comment'],
			":data_relation" => "step22",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
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
		for ($i = 0; $i <= $v['step22_field1_fieldset']["step22_comment_count"]; $i++)
		{
			if ($v['step22_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step22_field1_fieldset'][$i]["comment"],
					":data_relation" => "step22",
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
			} //$v['step22_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step22_field1_fieldset']["step22_comment_count"]; $i++
	}
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step21/' . $user->uid;
}
function ddcl_bb_splice_end_plate_step22_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step22_field1_fieldset']['step22_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step22_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step22_feedback');
	$query->condition('uid', $user->uid);
	$form_step22_data = $query->execute()->fetchObject();
	//var_dump($form_step22_data);die;
	if ($form_step22_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step22_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step22_field1_fieldset']['step22_field1_question1'],
			":question_ok_not_ok" => $v['step22_field1_fieldset']['step22_field1_question_ok'],
			":question_not_ok_comment" => $v['step22_field1_fieldset']['step22_field1_question_not_ok_comment'],
			":data_relation" => "step22"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step7_data->id
		));
		$form_step22_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step22_field1_fieldset']["step22_comment_count"]; $i++)
		{
			if ($v['step22_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step22_data->id,
					":number" => $i + $form_step22_comment_data->max + 1,
					":comment" => $v['step22_field1_fieldset'][$i]["comment"],
					":data_relation" => "step22",
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
			} //$v['step22_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step22_field1_fieldset']["step22_comment_count"]; $i++
	} //$form_step22_data
	else
	{
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step22_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok,:question_not_ok_comment, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step22_field1_fieldset']['step22_field1_question1'],
			":question_ok_not_ok" => $v['step22_field1_fieldset']['step22_field1_question_ok'],
			":question_not_ok_comment" => $v['step22_field1_fieldset']['step22_field1_question_not_ok_comment'],
			":data_relation" => "step22",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
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
		for ($i = 0; $i <= $v['step22_field1_fieldset']["step22_comment_count"]; $i++)
		{
			if ($v['step22_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step22_field1_fieldset'][$i]["comment"],
					":data_relation" => "step22",
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
			} //$v['step22_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step22_field1_fieldset']["step22_comment_count"]; $i++
	}
	//$form_state['redirect'] = 'ddcl-bb-splice-end-plate/success';
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step23/' . $user->uid;
}
/***************************************/
function step22_comment_add_more_add_one($form, &$form_state)
{
	$form_state['step22_num_comment']++;
	$form_state['rebuild'] = TRUE;
	//$form_state['no_redirect'] = TRUE;
}
function step22_comment_add_more_remove_one($form, &$form_state)
{
	if ($form_state['step22_num_comment'] > 1)
	{
		$form_state['step22_num_comment']--;
	} //$form_state['step22_num_comment'] > 1
	$form_state['rebuild'] = TRUE;
}
function step22_comment_add_more_callback($form, &$form_state)
{
	return $form['step22_field1_fieldset'];
}
/***************************************/
/***************************************** step 22 form ends ****************************/

/************************ Step 23 form starts *********************/
function ddcl_bb_splice_end_plate_step23_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	$img_path = $base_url . '/' . drupal_get_path("module", "osdag_response_form") . '/images';
	drupal_set_title(t('k. Design of stiffener (optional) > i. Stiffener configuration > ii. height of stiffener > iii. length of stiffener'));
	
	$markup_text = '
	<b><strong> k. Design of stiffener (optional) </strong> [Reference: AISC design guide 4, page 16] </b> <br>
	<hr>
	&emsp;&emsp; <b><strong> i. Stiffener configuration  </strong></b> <br>
	<br>
	&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
			
			<img src="' . $img_path . '/Stiffner.png" width="65%" height="40%" align="middle"/><br>
			
			&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
			&emsp;&emsp;&emsp;
			&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;<b>Figure:</b> stiffener configuration
			<br>
			<br>

	&emsp;&emsp; <b><strong> ii. height of stiffener ( h<sub>st</sub> ) </strong> mm. </b> <br>
	<br>
	<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>h</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>s</mi>
      <mi>t</mi>
    </mrow>
  </msub>
  <mo>=</mo>
  <msub>
    <mi>l</mi>
    <mi>v</mi>
  </msub>
  <mo>+</mo>
  <mo stretchy="false">(</mo>
  <msub>
    <mi>n</mi>
    <mi>r</mi>
  </msub>
  <mo>&#x2212;<!-- − --></mo>
  <mn>1</mn>
  <mo stretchy="false">)</mo>
  <mi>p</mi>
  <mo>+</mo>
  <mi>e</mi>
</math>
<br>
	&emsp;&emsp;&emsp;&emsp;&emsp;
	where,  
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						<msub>
								<mi>l</mi>
								<mi>v</mi>
						</msub>	
					</math> - distance between the edge of beam flange and centre of the nearer bolt  
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						<msub>
								<mi>n</mi>
								<mi>r</mi>
						</msub>	
					</math> - number of row(s) of bolt above or below the beam flange
				<br>	
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						
								<mi>p</mi>
								
						
					</math> - pitch distance  
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						
								<mi>e</mi>
								
							
					</math> - end distance  
				<br>
	<br>
	&emsp;&emsp; <b><strong> iii. length of stiffener ( l<sub>st</sub> ) </strong> [Reference: based on geometry] mm. </b> <br>
	<br>
	<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>l</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>s</mi>
      <mi>t</mi>
    </mrow>
  </msub>
  <mo>=</mo>
  <mo stretchy="false">[</mo>
  <mo stretchy="false">(</mo>
  <msub>
    <mi>h</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>s</mi>
      <mi>t</mi>
    </mrow>
  </msub>
  <mo>&#x2212;<!-- − --></mo>
  <mn>25</mn>
  <mo stretchy="false">)</mo>
  <mrow class="MJX-TeXAtom-ORD">
    <mo>/</mo>
  </mrow>
  <mi>t</mi>
  <mi>a</mi>
  <mi>n</mi>
  <mn>30</mn>
  <mo stretchy="false">]</mo>
  <mo>+</mo>
  <mn>25</mn>
</math>

	<br>
		
	<hr>';
	// All the real form fields.
	$form = array();
	$form['step23_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step23-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_bb_splice_end_plate_form_step23_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step23_feedback');
	$query->condition('uid', $user->uid);
	$form_step23_data = $query->execute();
	if ($form_step23_data)
	{
		if ($step23_data = $form_step23_data->fetchObject())
		{
			$form['step23_field1_fieldset']['step23_field1_question1'] = array(
				'#markup' => $markup_text
			);
			$form['step23_field1_fieldset']['step23_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				'#default_value' => $step23_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step23_field1_fieldset']['step23_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				'#default_value' => $step23_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step23_field1_fieldset\\[step23_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			$earlier_comments = _fetch_existing_ddcl_bb_splice_end_plate_comments($step23_data->id, $step23_data->data_relation);
			$icom = 1;
			$form['step23_field1_fieldset']['existing_comments_title'] = array(
				'#markup' => 'Earlier comment<br>'
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step23_field1_fieldset']['existing_comments' . $icom] = array(
					'#markup' => $rowcomment
				);
				$icom++;
			} //$earlier_comments as $rowcomment
			if (empty($form_state['step23_num_comment']))
			{
				$form_state['step23_num_comment'] = 1;
			} //empty($form_state['step23_num_comment'])
			$step23_temp1 = 0;
			for ($step23_i = 0; $step23_i < $form_state['step23_num_comment']; $step23_i++)
			{
				$step23_temp1 = $step23_i;
				$form['step23_field1_fieldset'][$step23_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step23_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step23_field1_fieldset']["step23_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step23_temp1
			);
			$form['step23_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step23_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step23_comment_add_more_callback',
					'wrapper' => 'step23-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step23_num_comment'] > 1)
			{
				$form['step23_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step23_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step23_comment_add_more_callback',
						'wrapper' => 'step23-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step23_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step23_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step23_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step23_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step23_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step23_form_next_submit'
				)
			);
		} //$step23_data = $form_step23_data->fetchObject()
		else
		{
			$form['step23_field1_fieldset']['step23_field1_question1'] = array(
				'#markup' => $markup_text
			);
			$form['step23_field1_fieldset']['step23_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				//'#default_value' => $step23_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step23_field1_fieldset']['step23_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				//'#default_value' => $step23_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step23_field1_fieldset\\[step23_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			if (empty($form_state['step23_num_comment']))
			{
				$form_state['step23_num_comment'] = 1;
			} //empty($form_state['step23_num_comment'])
			$step23_temp1 = 0;
			for ($step23_i = 0; $step23_i < $form_state['step23_num_comment']; $step23_i++)
			{
				$step23_temp1 = $step23_i;
				$form['step23_field1_fieldset'][$step23_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step23_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step23_field1_fieldset']["step23_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step23_temp1
			);
			$form['step23_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step23_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step23_comment_add_more_callback',
					'wrapper' => 'step23-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step23_num_comment'] > 1)
			{
				$form['step23_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step23_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step23_comment_add_more_callback',
						'wrapper' => 'step23-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step23_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step23_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step23_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step23_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step23_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step23_form_next_submit'
				)
			);
		} //$step23_data = $form_step23_data->fetchObject()
	} //$form_step23_data
	return $form;
}
function ddcl_bb_splice_end_plate_step23_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step23_field1_fieldset']['step23_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step23_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step23_feedback');
	$query->condition('uid', $user->uid);
	$form_step23_data = $query->execute()->fetchObject();
	//var_dump($form_step23_data);die;
	if ($form_step23_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step23_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step23_field1_fieldset']['step23_field1_question1'],
			":question_ok_not_ok" => $v['step23_field1_fieldset']['step23_field1_question_ok'],
			":question_not_ok_comment" => $v['step23_field1_fieldset']['step23_field1_question_not_ok_comment'],
			":data_relation" => "step23"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step23_data->id
		));
		$form_step23_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step23_field1_fieldset']["step23_comment_count"]; $i++)
		{
			if ($v['step23_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step23_data->id,
					":number" => $i + $form_step23_comment_data->max + 1,
					":comment" => $v['step23_field1_fieldset'][$i]["comment"],
					":data_relation" => "step23",
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
			} //$v['step23_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step23_field1_fieldset']["step23_comment_count"]; $i++
	} //$form_step23_data
	else
	{
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step23_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1,  :question_ok_not_ok, :question_not_ok_comment,:data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step23_field1_fieldset']['step23_field1_question1'],
			":question_ok_not_ok" => $v['step23_field1_fieldset']['step23_field1_question_ok'],
			":question_not_ok_comment" => $v['step23_field1_fieldset']['step23_field1_question_not_ok_comment'],
			":data_relation" => "step23",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
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
		for ($i = 0; $i <= $v['step23_field1_fieldset']["step23_comment_count"]; $i++)
		{
			if ($v['step23_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step23_field1_fieldset'][$i]["comment"],
					":data_relation" => "step23",
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
			} //$v['step23_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step23_field1_fieldset']["step23_comment_count"]; $i++
	}
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step22/' . $user->uid;
}
function ddcl_bb_splice_end_plate_step23_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step23_field1_fieldset']['step23_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step23_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step23_feedback');
	$query->condition('uid', $user->uid);
	$form_step23_data = $query->execute()->fetchObject();
	//var_dump($form_step23_data);die;
	if ($form_step23_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step23_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step23_field1_fieldset']['step23_field1_question1'],
			":question_ok_not_ok" => $v['step23_field1_fieldset']['step23_field1_question_ok'],
			":question_not_ok_comment" => $v['step23_field1_fieldset']['step23_field1_question_not_ok_comment'],
			":data_relation" => "step23"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step7_data->id
		));
		$form_step23_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step23_field1_fieldset']["step23_comment_count"]; $i++)
		{
			if ($v['step23_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step23_data->id,
					":number" => $i + $form_step23_comment_data->max + 1,
					":comment" => $v['step23_field1_fieldset'][$i]["comment"],
					":data_relation" => "step23",
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
			} //$v['step23_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step23_field1_fieldset']["step23_comment_count"]; $i++
	} //$form_step23_data
	else
	{
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step23_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok,:question_not_ok_comment, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step23_field1_fieldset']['step23_field1_question1'],
			":question_ok_not_ok" => $v['step23_field1_fieldset']['step23_field1_question_ok'],
			":question_not_ok_comment" => $v['step23_field1_fieldset']['step23_field1_question_not_ok_comment'],
			":data_relation" => "step23",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
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
		for ($i = 0; $i <= $v['step23_field1_fieldset']["step23_comment_count"]; $i++)
		{
			if ($v['step23_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step23_field1_fieldset'][$i]["comment"],
					":data_relation" => "step23",
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
			} //$v['step23_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step23_field1_fieldset']["step23_comment_count"]; $i++
	}
	//$form_state['redirect'] = 'ddcl-bb-splice-end-plate/success';
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step24/' . $user->uid;
}
/***************************************/
function step23_comment_add_more_add_one($form, &$form_state)
{
	$form_state['step23_num_comment']++;
	$form_state['rebuild'] = TRUE;
	//$form_state['no_redirect'] = TRUE;
}
function step23_comment_add_more_remove_one($form, &$form_state)
{
	if ($form_state['step23_num_comment'] > 1)
	{
		$form_state['step23_num_comment']--;
	} //$form_state['step23_num_comment'] > 1
	$form_state['rebuild'] = TRUE;
}
function step23_comment_add_more_callback($form, &$form_state)
{
	return $form['step23_field1_fieldset'];
}
/***************************************/
/***************************************** step 23 form ends ****************************/

/************************ Step 24 form starts *********************/
function ddcl_bb_splice_end_plate_step24_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('k. Design of stiffener (optional) > iv. thickness of stiffener > v. check of stiffener against local buckling '));
	$img_path = '';
	$markup_text = '
	<b><strong> k. Design of stiffener (optional) </strong></b> <br>
	<hr>
	&emsp;&emsp; <b><strong> iv. thickness of stiffener ( t<sub>s</sub> ) </strong> mm. </b> <br>
	<br>
	<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>t</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>s</mi>
      <mn>1</mn>
    </mrow>
  </msub>
  <mo>=</mo>
  <msub>
    <mi>t</mi>
    <mi>w</mi>
  </msub>
</math>


	<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>t</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>s</mi>
      <mn>2</mn>
    </mrow>
  </msub>
  <mo>&gt;</mo>
  <mo stretchy="false">[</mo>
  <mo stretchy="false">(</mo>
  <msub>
    <mi>f</mi>
    <mi>y</mi>
  </msub>
  <msub>
    <mo stretchy="false">)</mo>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>b</mi>
      <mi>e</mi>
      <mi>a</mi>
      <mi>m</mi>
    </mrow>
  </msub>
  <mtext>&#xA0;</mtext>
  <mrow class="MJX-TeXAtom-ORD">
    <mo>/</mo>
  </mrow>
  <mtext>&#xA0;</mtext>
  <mo stretchy="false">(</mo>
  <msub>
    <mi>f</mi>
    <mi>y</mi>
  </msub>
  <msub>
    <mo stretchy="false">)</mo>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>s</mi>
      <mi>t</mi>
      <mi>i</mi>
      <mi>f</mi>
      <mi>f</mi>
      <mi>e</mi>
      <mi>n</mi>
      <mi>e</mi>
      <mi>r</mi>
    </mrow>
  </msub>
  <mo stretchy="false">]</mo>
  <mtext>&#xA0;</mtext>
  <msub>
    <mi>t</mi>
    <mi>w</mi>
  </msub>
</math>

	<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>t</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>s</mi>
    </mrow>
  </msub>
  <mo>=</mo>
  <mi>m</mi>
  <mi>a</mi>
  <mi>x</mi>
  <mi>i</mi>
  <mi>m</mi>
  <mi>u</mi>
  <mi>m</mi>
  <mo stretchy="false">(</mo>
  <msub>
    <mi>t</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>s</mi>
      <mn>1</mn>
    </mrow>
  </msub>
  <mtext>&#xA0;</mtext>
  <mo>,</mo>
  <mtext>&#xA0;</mtext>
  <msub>
    <mi>t</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>s</mi>
      <mn>2</mn>
    </mrow>
  </msub>
  <mo stretchy="false">)</mo>
</math>
	&emsp;&emsp;&emsp;&emsp;&emsp;
	where,  
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						<msub>
								<mi>t</mi>
								<mi>w</mi>
						</msub>	
					</math> - thickness of beam web
<br>
	<br>
	&emsp;&emsp; <b><strong> v. check of stiffener against local buckling </strong> </b> <br>
	<br>
	<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <msub>
    <mi>t</mi>
    <mi>s</mi>
  </msub>
  <mo>=</mo>
  <mn>1.79</mn>
  <mtext>&#xA0;</mtext>
  <msub>
    <mi>h</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>s</mi>
      <mi>t</mi>
    </mrow>
  </msub>
  <mtext>&#xA0;</mtext>
  <msub>
    <mi>f</mi>
    <mrow class="MJX-TeXAtom-ORD">
      <msub>
        <mi>y</mi>
        <mrow class="MJX-TeXAtom-ORD">
          <mi>s</mi>
          <mi>t</mi>
          <mi>i</mi>
          <mi>f</mi>
          <mi>f</mi>
          <mi>e</mi>
          <mi>n</mi>
          <mi>e</mi>
          <mi>r</mi>
        </mrow>
      </msub>
    </mrow>
  </msub>
  <mrow class="MJX-TeXAtom-ORD">
    <mo>/</mo>
  </mrow>
  <mi>E</mi>
</math>
	
	<hr>';
	// All the real form fields.
	$form = array();
	$form['step24_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step24-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_bb_splice_end_plate_form_step24_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step24_feedback');
	$query->condition('uid', $user->uid);
	$form_step24_data = $query->execute();
	if ($form_step24_data)
	{
		if ($step24_data = $form_step24_data->fetchObject())
		{
			$form['step24_field1_fieldset']['step24_field1_question1'] = array(
				'#markup' => $markup_text
			);
			$form['step24_field1_fieldset']['step24_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				'#default_value' => $step24_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step24_field1_fieldset']['step24_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				'#default_value' => $step24_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step24_field1_fieldset\\[step24_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			$earlier_comments = _fetch_existing_ddcl_bb_splice_end_plate_comments($step24_data->id, $step24_data->data_relation);
			$icom = 1;
			$form['step24_field1_fieldset']['existing_comments_title'] = array(
				'#markup' => 'Earlier comment<br>'
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step24_field1_fieldset']['existing_comments' . $icom] = array(
					'#markup' => $rowcomment
				);
				$icom++;
			} //$earlier_comments as $rowcomment
			if (empty($form_state['step24_num_comment']))
			{
				$form_state['step24_num_comment'] = 1;
			} //empty($form_state['step24_num_comment'])
			$step24_temp1 = 0;
			for ($step24_i = 0; $step24_i < $form_state['step24_num_comment']; $step24_i++)
			{
				$step24_temp1 = $step24_i;
				$form['step24_field1_fieldset'][$step24_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step24_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step24_field1_fieldset']["step24_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step24_temp1
			);
			$form['step24_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step24_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step24_comment_add_more_callback',
					'wrapper' => 'step24-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step24_num_comment'] > 1)
			{
				$form['step24_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step24_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step24_comment_add_more_callback',
						'wrapper' => 'step24-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step24_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step24_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step24_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step24_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step24_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step24_form_next_submit'
				)
			);
		} //$step24_data = $form_step24_data->fetchObject()
		else
		{
			$form['step24_field1_fieldset']['step24_field1_question1'] = array(
				'#markup' => $markup_text
			);
			$form['step24_field1_fieldset']['step24_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				//'#default_value' => $step24_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step24_field1_fieldset']['step24_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				//'#default_value' => $step24_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step24_field1_fieldset\\[step24_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			if (empty($form_state['step24_num_comment']))
			{
				$form_state['step24_num_comment'] = 1;
			} //empty($form_state['step24_num_comment'])
			$step24_temp1 = 0;
			for ($step24_i = 0; $step24_i < $form_state['step24_num_comment']; $step24_i++)
			{
				$step24_temp1 = $step24_i;
				$form['step24_field1_fieldset'][$step24_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step24_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step24_field1_fieldset']["step24_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step24_temp1
			);
			$form['step24_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step24_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step24_comment_add_more_callback',
					'wrapper' => 'step24-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step24_num_comment'] > 1)
			{
				$form['step24_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step24_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step24_comment_add_more_callback',
						'wrapper' => 'step24-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step24_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step24_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step24_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step24_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step24_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step24_form_next_submit'
				)
			);
		} //$step24_data = $form_step24_data->fetchObject()
	} //$form_step24_data
	return $form;
}
function ddcl_bb_splice_end_plate_step24_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step24_field1_fieldset']['step24_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step24_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step24_feedback');
	$query->condition('uid', $user->uid);
	$form_step24_data = $query->execute()->fetchObject();
	//var_dump($form_step24_data);die;
	if ($form_step24_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step24_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step24_field1_fieldset']['step24_field1_question1'],
			":question_ok_not_ok" => $v['step24_field1_fieldset']['step24_field1_question_ok'],
			":question_not_ok_comment" => $v['step24_field1_fieldset']['step24_field1_question_not_ok_comment'],
			":data_relation" => "step24"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step24_data->id
		));
		$form_step24_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step24_field1_fieldset']["step24_comment_count"]; $i++)
		{
			if ($v['step24_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step24_data->id,
					":number" => $i + $form_step24_comment_data->max + 1,
					":comment" => $v['step24_field1_fieldset'][$i]["comment"],
					":data_relation" => "step24",
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
			} //$v['step24_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step24_field1_fieldset']["step24_comment_count"]; $i++
	} //$form_step24_data
	else
	{
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step24_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1,  :question_ok_not_ok, :question_not_ok_comment,:data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step24_field1_fieldset']['step24_field1_question1'],
			":question_ok_not_ok" => $v['step24_field1_fieldset']['step24_field1_question_ok'],
			":question_not_ok_comment" => $v['step24_field1_fieldset']['step24_field1_question_not_ok_comment'],
			":data_relation" => "step24",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
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
		for ($i = 0; $i <= $v['step24_field1_fieldset']["step24_comment_count"]; $i++)
		{
			if ($v['step24_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step24_field1_fieldset'][$i]["comment"],
					":data_relation" => "step24",
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
			} //$v['step24_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step24_field1_fieldset']["step24_comment_count"]; $i++
	}
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step23/' . $user->uid;
}
function ddcl_bb_splice_end_plate_step24_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step24_field1_fieldset']['step24_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step24_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step24_feedback');
	$query->condition('uid', $user->uid);
	$form_step24_data = $query->execute()->fetchObject();
	//var_dump($form_step24_data);die;
	if ($form_step24_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step24_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step24_field1_fieldset']['step24_field1_question1'],
			":question_ok_not_ok" => $v['step24_field1_fieldset']['step24_field1_question_ok'],
			":question_not_ok_comment" => $v['step24_field1_fieldset']['step24_field1_question_not_ok_comment'],
			":data_relation" => "step24"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step7_data->id
		));
		$form_step24_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step24_field1_fieldset']["step24_comment_count"]; $i++)
		{
			if ($v['step24_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step24_data->id,
					":number" => $i + $form_step24_comment_data->max + 1,
					":comment" => $v['step24_field1_fieldset'][$i]["comment"],
					":data_relation" => "step24",
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
			} //$v['step24_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step24_field1_fieldset']["step24_comment_count"]; $i++
	} //$form_step24_data
	else
	{
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step24_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok,:question_not_ok_comment, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step24_field1_fieldset']['step24_field1_question1'],
			":question_ok_not_ok" => $v['step24_field1_fieldset']['step24_field1_question_ok'],
			":question_not_ok_comment" => $v['step24_field1_fieldset']['step24_field1_question_not_ok_comment'],
			":data_relation" => "step24",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
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
		for ($i = 0; $i <= $v['step24_field1_fieldset']["step24_comment_count"]; $i++)
		{
			if ($v['step24_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step24_field1_fieldset'][$i]["comment"],
					":data_relation" => "step24",
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
			} //$v['step24_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step24_field1_fieldset']["step24_comment_count"]; $i++
	}
	//$form_state['redirect'] = 'ddcl-bb-splice-end-plate/success';
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step25/' . $user->uid;
}
/***************************************/
function step24_comment_add_more_add_one($form, &$form_state)
{
	$form_state['step24_num_comment']++;
	$form_state['rebuild'] = TRUE;
	//$form_state['no_redirect'] = TRUE;
}
function step24_comment_add_more_remove_one($form, &$form_state)
{
	if ($form_state['step24_num_comment'] > 1)
	{
		$form_state['step24_num_comment']--;
	} //$form_state['step24_num_comment'] > 1
	$form_state['rebuild'] = TRUE;
}
function step24_comment_add_more_callback($form, &$form_state)
{
	return $form['step24_field1_fieldset'];
}
/***************************************/
/***************************************** step 24 form ends ****************************/

/************************ Step 25 form starts *********************/
function ddcl_bb_splice_end_plate_step25_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('l. Additional checks > i. check for shear rupture strength of bolts in compression > ii. check for bolt bearing/tear out failure of end plate'));
	$img_path = '';
	$markup_text = '
	<b><strong> l. Additional checks </strong></b> <br>
	<hr>
	&emsp; <b><strong> i. Check for shear rupture strength of bolts in compression </strong></b> [Reference: equation 3.17, AISC Design guide 4] <br>
	<br>
	<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <mi>V</mi>
  <mo>&#x2264;<!-- ≤ --></mo>
  <mi mathvariant="normal">&#x03A6;<!-- Φ --></mi>
  <mtext>&#xA0;</mtext>
  <msub>
    <mi>R</mi>
    <mi>n</mi>
  </msub>
</math>

	<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <mi mathvariant="normal">&#x03A6;<!-- Φ --></mi>
  <mtext>&#xA0;</mtext>
  <msub>
    <mi>R</mi>
    <mi>n</mi>
  </msub>
  <mo>=</mo>
  <mn>0.75</mn>
  <mtext>&#xA0;</mtext>
  <msub>
    <mi>n</mi>
    <mi>b</mi>
  </msub>
  <mtext>&#xA0;</mtext>
  <msub>
    <mi>F</mi>
    <mi>v</mi>
  </msub>
  <mtext>&#xA0;</mtext>
  <msub>
    <mi>A</mi>
    <mi>b</mi>
  </msub>
</math>
	&emsp;&emsp;&emsp;&emsp;&emsp;
	where,  
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						<msub>
								<mi>n</mi>
								<mi>b</mi>
						</msub>	
					</math> - number of bolts near compression flange
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						<msub>
								<mi>F</mi>
								<mi>v</mi>
						</msub>	
					</math> - nominal shear strength of bolt
				<br>	
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						<msub>
								<mi>A</mi>
								<mi>b</mi>
						</msub>	
					</math> - area of bolt (area at thread)
				<br>
	<br>
	&emsp; <b><strong> ii. Check for bolt bearing/tear out failure of end plate </strong></b> [Reference: equation 3.18, AISC Design guide 4] <br>
	<br>
	<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <mi>V</mi>
  <mo>&#x2264;<!-- ≤ --></mo>
  <mo stretchy="false">[</mo>
  <mtext>&#xA0;</mtext>
  <mo stretchy="false">(</mo>
  <mi>n</mi>
  <mrow class="MJX-TeXAtom-ORD">
    <mo>&#x3A6;</mo>
  </mrow>
  <msub>
    <mi>R</mi>
    <mi>n</mi>
  </msub>
  <msub>
    <mo stretchy="false">)</mo>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>i</mi>
      <mi>n</mi>
      <mi>n</mi>
      <mi>e</mi>
      <mi>r</mi>
      <mtext>&#xA0;</mtext>
      <mi>b</mi>
      <mi>o</mi>
      <mi>l</mi>
      <mi>t</mi>
      <mi>s</mi>
    </mrow>
  </msub>
  <mo>+</mo>
  <mo stretchy="false">(</mo>
  <mi>n</mi>
  <mrow class="MJX-TeXAtom-ORD">
    <mo>&#x3A6;</mo>
  </mrow>
  <msub>
    <mi>R</mi>
    <mi>n</mi>
  </msub>
  <msub>
    <mo stretchy="false">)</mo>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>o</mi>
      <mi>u</mi>
      <mi>t</mi>
      <mi>e</mi>
      <mi>r</mi>
      <mtext>&#xA0;</mtext>
      <mi>b</mi>
      <mi>o</mi>
      <mi>l</mi>
      <mi>t</mi>
      <mi>s</mi>
    </mrow>
  </msub>
  <mtext>&#xA0;</mtext>
  <mo stretchy="false">]</mo>
</math><br>

	<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <mo stretchy="false">(</mo>
  <msub>
    <mi>R</mi>
    <mi>n</mi>
  </msub>
  <msub>
    <mo stretchy="false">)</mo>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>i</mi>
      <mi>n</mi>
      <mi>n</mi>
      <mi>e</mi>
      <mi>r</mi>
      <mtext>&#xA0;</mtext>
      <mi>b</mi>
      <mi>o</mi>
      <mi>l</mi>
      <mi>t</mi>
      <mi>s</mi>
    </mrow>
  </msub>
  <mo>=</mo>
  <mn>1.2</mn>
  <mtext>&#xA0;</mtext>
  <msub>
    <mi>L</mi>
    <mi>c</mi>
  </msub>
  <mtext>&#xA0;</mtext>
  <msub>
    <mi>t</mi>
    <mi>p</mi>
  </msub>
  <mtext>&#xA0;</mtext>
  <msub>
    <mi>f</mi>
    <mi>u</mi>
  </msub>
</math>

	<math xmlns="http://www.w3.org/1998/Math/MathML" display="block">
  <mo stretchy="false">(</mo>
  <msub>
    <mi>R</mi>
    <mi>n</mi>
  </msub>
  <msub>
    <mo stretchy="false">)</mo>
    <mrow class="MJX-TeXAtom-ORD">
      <mi>o</mi>
      <mi>u</mi>
      <mi>t</mi>
      <mi>e</mi>
      <mi>r</mi>
      <mtext>&#xA0;</mtext>
      <mi>b</mi>
      <mi>o</mi>
      <mi>l</mi>
      <mi>t</mi>
      <mi>s</mi>
    </mrow>
  </msub>
  <mo>=</mo>
  <mn>1.2</mn>
  <mtext>&#xA0;</mtext>
  <mo stretchy="false">(</mo>
  <msub>
    <mi>l</mi>
    <mi>v</mi>
  </msub>
  <mo>&#x2212;<!-- − --></mo>
  <mi>d</mi>
  <mrow class="MJX-TeXAtom-ORD">
    <mo>/</mo>
  </mrow>
  <mn>2</mn>
  <mo stretchy="false">)</mo>
  <mtext>&#xA0;</mtext>
  <msub>
    <mi>t</mi>
    <mi>p</mi>
  </msub>
  <mtext>&#xA0;</mtext>
  <msub>
    <mi>f</mi>
    <mi>u</mi>
  </msub>
</math>
	&emsp;&emsp;&emsp;&emsp;&emsp;
	where,  
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
					<mi mathvariant="normal">&#x03A6;<!-- Φ --></mi>
 					 <mo>=</mo>
  					<mn>0.75</mn>	
					</math> 
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						<msub>
								<mi>L</mi>
								<mi>c</mi>
						</msub>	
					</math> - length of connection
				<br>
				&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;
					<math xmlns="http://www.w3.org/1998/Math/MathML">
						
								<mi>n</mi>
						
					</math> - number of bolts inside or outside the beam flange


	<hr>';
	// All the real form fields.
	$form = array();
	$form['step25_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#tree' => TRUE,
		'#prefix' => '<div id="step25-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_bb_splice_end_plate_form_step25_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step25_feedback');
	$query->condition('uid', $user->uid);
	$form_step25_data = $query->execute();
	if ($form_step25_data)
	{
		if ($step25_data = $form_step25_data->fetchObject())
		{
			$form['step25_field1_fieldset']['step25_field1_question1'] = array(
				'#markup' => $markup_text
			);
			$form['step25_field1_fieldset']['step25_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				'#default_value' => $step25_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step25_field1_fieldset']['step25_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				'#default_value' => $step25_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step25_field1_fieldset\\[step25_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			$earlier_comments = _fetch_existing_ddcl_bb_splice_end_plate_comments($step25_data->id, $step25_data->data_relation);
			$icom = 1;
			$form['step25_field1_fieldset']['existing_comments_title'] = array(
				'#markup' => 'Earlier comment<br>'
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step25_field1_fieldset']['existing_comments' . $icom] = array(
					'#markup' => $rowcomment
				);
				$icom++;
			} //$earlier_comments as $rowcomment
			if (empty($form_state['step25_num_comment']))
			{
				$form_state['step25_num_comment'] = 1;
			} //empty($form_state['step25_num_comment'])
			$step25_temp1 = 0;
			for ($step25_i = 0; $step25_i < $form_state['step25_num_comment']; $step25_i++)
			{
				$step25_temp1 = $step25_i;
				$form['step25_field1_fieldset'][$step25_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step25_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step25_field1_fieldset']["step25_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step25_temp1
			);
			$form['step25_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step25_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step25_comment_add_more_callback',
					'wrapper' => 'step25-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step25_num_comment'] > 1)
			{
				$form['step25_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step25_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step25_comment_add_more_callback',
						'wrapper' => 'step25-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step25_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step25_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step25_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step25_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step25_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step25_form_next_submit'
				)
			);
		} //$step25_data = $form_step25_data->fetchObject()
		else
		{
			$form['step25_field1_fieldset']['step25_field1_question1'] = array(
				'#markup' => $markup_text
			);
			$form['step25_field1_fieldset']['step25_field1_question_ok'] = array(
				'#title' => '<strong>Is this ok/not ok</strong>',
				'#type' => 'radios',
				'#options' => array(
					'Ok' => 'Ok',
					'Not ok' => 'Not ok'
				),
				//'#default_value' => $step25_data->question_ok_not_ok,
				'#required' => true
			);
			$form['step25_field1_fieldset']['step25_field1_question_not_ok_comment'] = array(
				//'#title' => 'If not, ple?',
				'#type' => 'textarea',
				//'#default_value' => $step25_data->question_not_ok_comment,
				'#states' => array(
					'visible' => array(
						':input[name="step25_field1_fieldset\\[step25_field1_question_ok\\]"]' => array(
							array(
								'value' => t('Not ok')
							)
						)
					)
				)
			);
			if (empty($form_state['step25_num_comment']))
			{
				$form_state['step25_num_comment'] = 1;
			} //empty($form_state['step25_num_comment'])
			$step25_temp1 = 0;
			for ($step25_i = 0; $step25_i < $form_state['step25_num_comment']; $step25_i++)
			{
				$step25_temp1 = $step25_i;
				$form['step25_field1_fieldset'][$step25_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step25_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step25_field1_fieldset']["step25_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step25_temp1
			);
			$form['step25_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step25_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step25_comment_add_more_callback',
					'wrapper' => 'step25-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step25_num_comment'] > 1)
			{
				$form['step25_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step25_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step25_comment_add_more_callback',
						'wrapper' => 'step25-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step25_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step25_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step25_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step25_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form["step1_page_id"] = array(
				"#type" => "hidden",
				"#value" => "step1"
			);
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step25_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Save & Next',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step25_form_next_submit'
				)
			);
		} //$step25_data = $form_step25_data->fetchObject()
	} //$form_step25_data
	return $form;
}
function ddcl_bb_splice_end_plate_step25_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step25_field1_fieldset']['step25_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step25_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step25_feedback');
	$query->condition('uid', $user->uid);
	$form_step25_data = $query->execute()->fetchObject();
	//var_dump($form_step25_data);die;
	if ($form_step25_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step25_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step25_field1_fieldset']['step25_field1_question1'],
			":question_ok_not_ok" => $v['step25_field1_fieldset']['step25_field1_question_ok'],
			":question_not_ok_comment" => $v['step25_field1_fieldset']['step25_field1_question_not_ok_comment'],
			":data_relation" => "step25"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step25_data->id
		));
		$form_step25_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step25_field1_fieldset']["step25_comment_count"]; $i++)
		{
			if ($v['step25_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step25_data->id,
					":number" => $i + $form_step25_comment_data->max + 1,
					":comment" => $v['step25_field1_fieldset'][$i]["comment"],
					":data_relation" => "step25",
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
			} //$v['step25_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step25_field1_fieldset']["step25_comment_count"]; $i++
	} //$form_step25_data
	else
	{
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step25_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1,  :question_ok_not_ok, :question_not_ok_comment,:data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step25_field1_fieldset']['step25_field1_question1'],
			":question_ok_not_ok" => $v['step25_field1_fieldset']['step25_field1_question_ok'],
			":question_not_ok_comment" => $v['step25_field1_fieldset']['step25_field1_question_not_ok_comment'],
			":data_relation" => "step25",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
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
		for ($i = 0; $i <= $v['step25_field1_fieldset']["step25_comment_count"]; $i++)
		{
			if ($v['step25_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step25_field1_fieldset'][$i]["comment"],
					":data_relation" => "step25",
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
			} //$v['step25_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step25_field1_fieldset']["step25_comment_count"]; $i++
	}
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step24/' . $user->uid;
}
function ddcl_bb_splice_end_plate_step25_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step25_field1_fieldset']['step25_field1_question1'] = 'question 1';
	$query = db_select('ddcl_bb_splice_end_plate_form_step25_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step25_feedback');
	$query->condition('uid', $user->uid);
	$form_step25_data = $query->execute()->fetchObject();
	//var_dump($form_step25_data);die;
	if ($form_step25_data)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step25_feedback SET question1=:question1, question_ok_not_ok=:question_ok_not_ok, question_not_ok_comment=:question_not_ok_comment, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step25_field1_fieldset']['step25_field1_question1'],
			":question_ok_not_ok" => $v['step25_field1_fieldset']['step25_field1_question_ok'],
			":question_not_ok_comment" => $v['step25_field1_fieldset']['step25_field1_question_not_ok_comment'],
			":data_relation" => "step25"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step7_data->id
		));
		$form_step25_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step25_field1_fieldset']["step25_comment_count"]; $i++)
		{
			if ($v['step25_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step25_data->id,
					":number" => $i + $form_step25_comment_data->max + 1,
					":comment" => $v['step25_field1_fieldset'][$i]["comment"],
					":data_relation" => "step25",
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
			} //$v['step25_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step25_field1_fieldset']["step25_comment_count"]; $i++
	} //$form_step25_data
	else
	{
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step25_feedback} (uid, question1, question_ok_not_ok, question_not_ok_comment, data_relation,inserted_on) VALUES (:uid,:question1, :question_ok_not_ok,:question_not_ok_comment, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step25_field1_fieldset']['step25_field1_question1'],
			":question_ok_not_ok" => $v['step25_field1_fieldset']['step25_field1_question_ok'],
			":question_not_ok_comment" => $v['step25_field1_fieldset']['step25_field1_question_not_ok_comment'],
			":data_relation" => "step25",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
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
		for ($i = 0; $i <= $v['step25_field1_fieldset']["step25_comment_count"]; $i++)
		{
			if ($v['step25_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step25_field1_fieldset'][$i]["comment"],
					":data_relation" => "step25",
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
			} //$v['step25_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step25_field1_fieldset']["step25_comment_count"]; $i++
	}
	//$form_state['redirect'] = 'ddcl-bb-splice-end-plate/success';
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step26/' . $user->uid;
}
/***************************************/
function step25_comment_add_more_add_one($form, &$form_state)
{
	$form_state['step25_num_comment']++;
	$form_state['rebuild'] = TRUE;
	//$form_state['no_redirect'] = TRUE;
}
function step25_comment_add_more_remove_one($form, &$form_state)
{
	if ($form_state['step25_num_comment'] > 1)
	{
		$form_state['step25_num_comment']--;
	} //$form_state['step25_num_comment'] > 1
	$form_state['rebuild'] = TRUE;
}
function step25_comment_add_more_callback($form, &$form_state)
{
	return $form['step25_field1_fieldset'];
}
/***************************************/
/***************************************** step 25 form ends ****************************/

/************************ Step 26 form starts *********************/
function ddcl_bb_splice_end_plate_step26_form($form, &$form_state, $no_js_use = FALSE)
{
	global $base_url;
	global $user;
	drupal_set_title(t('Other recommendations'));
	// All the real form fields.
	$form = array();
	$form['step26_field1_fieldset'] = array(
		'#type' => 'fieldset',
		'#title' => '<strong>Any other recommendation/Comment?</strong>',
		'#tree' => TRUE,
		'#prefix' => '<div id="step26-field1-fieldset-wrapper">',
		'#suffix' => '</div>'
	);
	$query = db_select('ddcl_bb_splice_end_plate_form_step26_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step26_feedback');
	$query->condition('uid', $user->uid);
	$form_step26_data = $query->execute();
	if ($form_step26_data)
	{
		if ($step26_data = $form_step26_data->fetchObject())
		{
			$form['step26_field1_fieldset']['step26_field1_question1'] = array(
				'#type' => 'hidden',
				'#default_value' => 'other recommendations/ Comments',
				'#required' => TRUE
			);
			$earlier_comments = _fetch_existing_ddcl_bb_splice_end_plate_comments($step26_data->id, $step26_data->data_relation);
			$icom = 1;
			$form['step26_field1_fieldset']['existing_comments_title'] = array(
				'#markup' => ''
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step26_field1_fieldset']['existing_comments' . $icom] = array(
					'#markup' => $rowcomment
				);
				$icom++;
			} //$earlier_comments as $rowcomment
			if (empty($form_state['step26_num_comment']))
			{
				$form_state['step26_num_comment'] = 1;
			} //empty($form_state['step26_num_comment'])
			$step26_temp1 = 0;
			for ($step26_i = 0; $step26_i < $form_state['step26_num_comment']; $step26_i++)
			{
				$step26_temp1 = $step26_i;
				$form['step26_field1_fieldset'][$step26_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step26_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step26_field1_fieldset']["step26_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step26_temp1
			);
			$form['step26_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step26_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step26_comment_add_more_callback',
					'wrapper' => 'step26-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step26_num_comment'] > 1)
			{
				$form['step26_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step26_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step26_comment_add_more_callback',
						'wrapper' => 'step26-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step26_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step26_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step26_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step26_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form['previous2'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step26_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Submit',
				'#attributes' => array(
					"onclick" => "return confirm('Are you sure?')"
				),
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step26_form_next_submit'
				)
			);
			$form['save_submit'] = array(
				'#type' => 'submit',
				'#value' => 'Save as draft',
				'#attributes' => array(
					"onclick" => "return confirm('Are you sure?')"
				),
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step26_form_save_submit'
				)
			);
		} //$step26_data = $form_step26_data->fetchObject()
		else
		{
			$form['step26_field1_fieldset']['step26_field1_question1'] = array(
				'#type' => 'hidden',
				'#default_value' => 'other recommendations/ Comments',
				'#required' => TRUE
			);
			$earlier_comments = _fetch_existing_ddcl_bb_splice_end_plate_comments($step26_data->id, $step26_data->data_relation);
			$icom = 1;
			$form['step26_field1_fieldset']['existing_comments_title'] = array(
				'#markup' => ''
			);
			foreach ($earlier_comments as $rowcomment)
			{
				$form['step26_field1_fieldset']['existing_comments' . $icom] = array(
					'#markup' => $rowcomment
				);
				$icom++;
			} //$earlier_comments as $rowcomment
			if (empty($form_state['step26_num_comment']))
			{
				$form_state['step26_num_comment'] = 1;
			} //empty($form_state['step26_num_comment'])
			$step26_temp1 = 0;
			for ($step26_i = 0; $step26_i < $form_state['step26_num_comment']; $step26_i++)
			{
				$step26_temp1 = $step26_i;
				$form['step26_field1_fieldset'][$step26_i]["comment"] = array(
					"#type" => "textarea",
					"#title" => "Comment " . ($step26_temp1 + 1),
					"#default_value" => ""
				);
			} //$i = 0; $i < $form_state['step1_num_comment']; $i++
			$form['step26_field1_fieldset']["step26_comment_count"] = array(
				"#type" => "hidden",
				"#value" => $step26_temp1
			);
			$form['step26_field1_fieldset']['add_comment'] = array(
				'#type' => 'submit',
				'#value' => t('Add more comment'),
				'#limit_validation_errors' => array(),
				'#submit' => array(
					'step26_comment_add_more_add_one'
				),
				'#ajax' => array(
					'callback' => 'step26_comment_add_more_callback',
					'wrapper' => 'step26-field1-fieldset-wrapper'
				)
			);
			if ($form_state['step26_num_comment'] > 1)
			{
				$form['step26_field1_fieldset']['remove_comment'] = array(
					'#type' => 'submit',
					'#value' => t('Remove comment'),
					'#limit_validation_errors' => array(),
					'#submit' => array(
						'step26_comment_add_more_remove_one'
					),
					'#ajax' => array(
						'callback' => 'step26_comment_add_more_callback',
						'wrapper' => 'step26-field1-fieldset-wrapper'
					)
				);
			} //$form_state['step1_num_comment'] > 1
			if ($no_js_use)
			{
				if (!empty($form['step26_field1_fieldset']['remove_comment']['#ajax']))
				{
					unset($form['step26_field1_fieldset']['remove_comment']['#ajax']);
				} //!empty($form['step26_field1_fieldset']['remove_comment']['#ajax'])
				unset($form['step26_field1_fieldset']['add_comment']['#ajax']);
			} //$no_js_use
			$form['previous1'] = array(
				'#type' => 'submit',
				'#value' => 'Previous',
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step26_form_previous_submit'
				)
			);
			$form['next2'] = array(
				'#type' => 'submit',
				'#value' => 'Submit',
				'#attributes' => array(
					"onclick" => "return confirm('Are you sure?')"
				),
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step26_form_previous_submit'
				)
			);
			$form['save_submit'] = array(
				'#type' => 'submit',
				'#value' => 'Save as draft',
				'#attributes' => array(
					"onclick" => "return confirm('Are you sure?')"
				),
				'#submit' => array(
					'ddcl_bb_splice_end_plate_step26_form_save_submit'
				)
			);
		} //$step26_data = $form_step26_data->fetchObject()
	} //$form_step26_data
	return $form;
}
function ddcl_bb_splice_end_plate_step26_form_previous_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	/*$query = db_select('ddcl_bb_splice_end_plate_form_step8_feedback');
	$query->fields('ddcl_bb_splice_end_plate_form_step8_feedback');
	$query->condition('uid', $user->uid);
	$form_step7_data = $query->execute()->fetchObject();
	*/
	$query = "SELECT * FROM ddcl_bb_splice_end_plate_form_step26_feedback WHERE uid=:uid";
	$args = array(
		":uid" => $user->uid
	);
	$form_step26_data = db_query($query, $args);
	$form_step26_data_count = $form_step26_data->rowCount();
	$form_step26_data = $form_step26_data->fetchObject();	
	if ($form_step26_data_count != 0)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step26_feedback SET question1=:question1, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step26_field1_fieldset']['step26_field1_question1'],
			":data_relation" => "step26"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid ";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step26_data->id
		));
		$form_step26_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step26_field1_fieldset']["step26_comment_count"]; $i++)
		{
			if ($v['step26_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step26_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step26_form_id" => $form_step26_data->id,
					":number" => $i + $form_step26_comment_data->max + 1,
					":comment" => $v['step26_field1_fieldset'][$i]["comment"],
					":data_relation" => "step26",
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
			} //$v['step26_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step26_field1_fieldset']["step26_comment_count"]; $i++
	} //$form_step26_data
	else
	{
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step26_feedback} (uid, question1, data_relation,inserted_on) VALUES (:uid,:question1, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step26_field1_fieldset']['step26_field1_question1'],
			":data_relation" => "step26",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid ";
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
		for ($i = 0; $i <= $v['step26_field1_fieldset']["step26_comment_count"]; $i++)
		{
			if ($v['step26_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO 	ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step26_field1_fieldset'][$i]["comment"],
					":data_relation" => "step26",
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
			} //$v['step26_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step26_field1_fieldset']["step26_comment_count"]; $i++
	}
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/form/step25/' . $user->uid;
}
function ddcl_bb_splice_end_plate_step26_form_next_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];	
	$query = "SELECT * FROM ddcl_bb_splice_end_plate_form_step26_feedback WHERE uid=:uid";
	$args = array(
		":uid" => $user->uid
	);
	$form_step26_data = db_query($query, $args);
	$form_step26_data_count = $form_step26_data->rowCount();
	$form_step26_data = $form_step26_data->fetchObject();
	//var_dump($form_step26_data);die;
	if ($form_step26_data_count != 0)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step26_feedback SET question1=:question1, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step26_field1_fieldset']['step26_field1_question1'],
			":data_relation" => "step26"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid ";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 1
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step26_data->id
		));
		$form_step26_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step26_field1_fieldset']["step26_comment_count"]; $i++)
		{
			if ($v['step26_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $form_step26_data->id,
					":number" => $i + $form_step26_comment_data->max + 1,
					":comment" => $v['step26_field1_fieldset'][$i]["comment"],
					":data_relation" => "step26",
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
			} //$v['step26_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step26_field1_fieldset']["step26_comment_count"]; $i++
	} //$form_step26_data
	else
	{
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step26_feedback} (uid, question1, data_relation,inserted_on) VALUES (:uid,:question1, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step26_field1_fieldset']['step26_field1_question1'],
			":data_relation" => "step26",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid ";
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
		for ($i = 0; $i <= $v['step26_field1_fieldset']["step26_comment_count"]; $i++)
		{
			if ($v['step26_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO 	ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step26_field1_fieldset'][$i]["comment"],
					":data_relation" => "step26",
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
		} //$i = 0; $i <= $v['step26_field1_fieldset']["step26_comment_count"]; $i++
	}
	$form_state['redirect'] = 'ddcl-bb-splice-end-plate/success/' . $user->uid;
}
function ddcl_bb_splice_end_plate_step26_form_save_submit($form, &$form_state, $no_js_use = FALSE)
{
	global $user;
	$v = $form_state['values'];
	$v['step26_field1_fieldset']['step26_field1_question1'] = 'question 1';
	$query = "SELECT * FROM ddcl_bb_splice_end_plate_form_step26_feedback WHERE uid=:uid";
	$args = array(
		":uid" => $user->uid
	);
	$form_step26_data = db_query($query, $args);
	$form_step26_data_count = $form_step26_data->rowCount();
	$form_step26_data = $form_step26_data->fetchObject();
	if ($form_step26_data_count != 0)
	{
		$query = "UPDATE ddcl_bb_splice_end_plate_form_step26_feedback SET question1=:question1, data_relation=:data_relation WHERE uid=:uid ";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step26_field1_fieldset']['step26_field1_question1'],
			":data_relation" => "step26"
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
		$args_finish = array(
			":uid" => $user->uid,
			":submitted" => 0
		);
		/* storing the row id in $result */
		$resultfinish = db_query($queryfinish, $args_finish, array(
			'return' => Database::RETURN_INSERT_ID
		));
		/* For adding comments */
		$query = db_query(" SELECT MAX( number ) AS max FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id", array(
			':step1_form_id' => $form_step26_data->id
		));
		$form_step26_comment_data = $query->fetchObject();
		$comments = 0;
		for ($i = 0; $i <= $v['step26_field1_fieldset']["step26_comment_count"]; $i++)
		{
			if ($v['step26_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step26_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step26_form_id" => $form_step26_data->id,
					":number" => $i + $form_step26_comment_data->max + 1,
					":comment" => $v['step26_field1_fieldset'][$i]["comment"],
					":data_relation" => "step26",
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
			} //$v['step26_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step26_field1_fieldset']["step26_comment_count"]; $i++
	} //$form_step26_data
	else
	{
		$query = "INSERT INTO {ddcl_bb_splice_end_plate_form_step26_feedback} (uid, question1, data_relation,inserted_on) VALUES (:uid,:question1, :data_relation,:inserted_on)";
		$args = array(
			":uid" => $user->uid,
			":question1" => $v['step26_field1_fieldset']['step26_field1_question1'],
			":data_relation" => "step26",
			":inserted_on" => date('Y-m-d')
		);
		/* storing the row id in $result */
		$result = db_query($query, $args, array(
			'return' => Database::RETURN_INSERT_ID
		));
		$queryfinish = "UPDATE ddcl_bb_splice_end_plate_form_step1_feedback SET submitted=:submitted WHERE uid=:uid";
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
		for ($i = 0; $i <= $v['step26_field1_fieldset']["step26_comment_count"]; $i++)
		{
			if ($v['step26_field1_fieldset'][$i]["comment"] != "")
			{
				$commentquery = "
	INSERT INTO ddcl_bb_splice_end_plate_feedback_form_comment
	(step1_form_id, number, comment, data_relation, inserted_on)
	VALUES
	(:step1_form_id, :number, :comment, :data_relation, :inserted_on)
	";
				$commentargs = array(
					":step1_form_id" => $result,
					":number" => $i + 1,
					":comment" => $v['step26_field1_fieldset'][$i]["comment"],
					":data_relation" => "step26",
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
			} //$v['step26_field1_fieldset'][$i]["comment"] != ""
		} //$i = 0; $i <= $v['step26_field1_fieldset']["step26_comment_count"]; $i++
	}
	drupal_set_message(t('You have successfully saved your feedback. You can anytime edit your feedback form.'), 'status');
	drupal_goto('home');
}
/***************************************/
function step26_comment_add_more_add_one($form, &$form_state)
{
	$form_state['step26_num_comment']++;
	$form_state['rebuild'] = TRUE;
	//$form_state['no_redirect'] = TRUE;
}
function step26_comment_add_more_remove_one($form, &$form_state)
{
	if ($form_state['step26_num_comment'] > 1)
	{
		$form_state['step26_num_comment']--;
	} //$form_state['step26_num_comment'] > 1
	$form_state['rebuild'] = TRUE;
}
function step26_comment_add_more_callback($form, &$form_state)
{
	return $form['step26_field1_fieldset'];
}
/***************************************/
/****************************************** step 26 form ends ****************************/
/**
 * Displays simple confirmation page after finishing the whole process.
 */
function ddcl_bb_splice_end_plate_success()
{
	$data .= drupal_set_message(t('You have successfully submitted the feedback.'), 'status');
	$data .= drupal_goto('home');
	return $data;
}
/**********************************************/
function ddcl_bb_splice_end_plate_result_page()
{
	/* get pending proposals to be approved */
	$output = "";
	$output .= l("Download All Result in CSV", "ddcl-bb-splice-end-plate/form/result/download");
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
step17.question1 as "step17_question1",
step17.question_ok_not_ok as "step17_question_ok_not_ok",
step17.question_not_ok_comment as "step17_question_not_ok_comment",

step18.id as "step18_id",
step18.data_relation as "step18_data_relation",
step18.question1 as "step18_question1",
step18.question_ok_not_ok as "step18_question_ok_not_ok",
step18.question_not_ok_comment as "step18_question_not_ok_comment",

step19.id as "step19_id",
step19.data_relation as "step19_data_relation",
step19.question1 as "step19_question1",
step19.question_ok_not_ok as "step19_question_ok_not_ok",
step19.question_not_ok_comment as "step19_question_not_ok_comment",

step20.id as "step20_id",
step20.data_relation as "step20_data_relation",
step20.question1 as "step20_question1",
step20.question_ok_not_ok as "step20_question_ok_not_ok",
step20.question_not_ok_comment as "step20_question_not_ok_comment",

step21.id as "step21_id",
step21.data_relation as "step21_data_relation",
step21.question1 as "step21_question1",
step21.question_ok_not_ok as "step21_question_ok_not_ok",
step21.question_not_ok_comment as "step21_question_not_ok_comment",

step22.id as "step22_id",
step22.data_relation as "step22_data_relation",
step22.question1 as "step22_question1",
step22.question_ok_not_ok as "step22_question_ok_not_ok",
step22.question_not_ok_comment as "step22_question_not_ok_comment",

step23.id as "step23_id",
step23.data_relation as "step23_data_relation",
step23.question1 as "step23_question1",
step23.question_ok_not_ok as "step23_question_ok_not_ok",
step23.question_not_ok_comment as "step23_question_not_ok_comment",

step24.id as "step24_id",
step24.data_relation as "step24_data_relation",
step24.question1 as "step24_question1",
step24.question_ok_not_ok as "step24_question_ok_not_ok",
step24.question_not_ok_comment as "step24_question_not_ok_comment",

step25.id as "step25_id",
step25.data_relation as "step25_data_relation",
step25.question1 as "step25_question1",
step25.question_ok_not_ok as "step25_question_ok_not_ok",
step25.question_not_ok_comment as "step25_question_not_ok_comment",

step26.id as "step26_id",
step26.data_relation as "step26_data_relation",
step26.question1 as "step26_question1"


from ddcl_bb_splice_end_plate_form_step1_feedback step1

join ddcl_bb_splice_end_plate_form_step2_feedback step2 on step1.uid=step2.uid
join ddcl_bb_splice_end_plate_form_step3_feedback step3 on step2.uid=step3.uid
join ddcl_bb_splice_end_plate_form_step4_feedback step4 on step3.uid=step4.uid
join ddcl_bb_splice_end_plate_form_step5_feedback step5 on step4.uid=step5.uid
join ddcl_bb_splice_end_plate_form_step6_feedback step6 on step5.uid=step6.uid
join ddcl_bb_splice_end_plate_form_step7_feedback step7 on step6.uid=step7.uid
join ddcl_bb_splice_end_plate_form_step8_feedback step8 on step7.uid=step8.uid
join ddcl_bb_splice_end_plate_form_step9_feedback step9 on step8.uid=step9.uid
join ddcl_bb_splice_end_plate_form_step10_feedback step10 on step9.uid=step10.uid
join ddcl_bb_splice_end_plate_form_step11_feedback step11 on step10.uid=step11.uid
join ddcl_bb_splice_end_plate_form_step12_feedback step12 on step11.uid=step12.uid
join ddcl_bb_splice_end_plate_form_step13_feedback step13 on step12.uid=step13.uid
join ddcl_bb_splice_end_plate_form_step14_feedback step14 on step13.uid=step14.uid
join ddcl_bb_splice_end_plate_form_step15_feedback step15 on step14.uid=step15.uid
join ddcl_bb_splice_end_plate_form_step15_feedback step16 on step15.uid=step16.uid
join ddcl_bb_splice_end_plate_form_step15_feedback step17 on step16.uid=step17.uid
join ddcl_bb_splice_end_plate_form_step15_feedback step18 on step17.uid=step18.uid
join ddcl_bb_splice_end_plate_form_step15_feedback step19 on step18.uid=step19.uid
join ddcl_bb_splice_end_plate_form_step15_feedback step20 on step19.uid=step20.uid
join ddcl_bb_splice_end_plate_form_step15_feedback step21 on step20.uid=step21.uid
join ddcl_bb_splice_end_plate_form_step15_feedback step22 on step21.uid=step22.uid
join ddcl_bb_splice_end_plate_form_step15_feedback step23 on step22.uid=step23.uid
join ddcl_bb_splice_end_plate_form_step15_feedback step24 on step23.uid=step24.uid
join ddcl_bb_splice_end_plate_form_step15_feedback step25 on step24.uid=step25.uid
join ddcl_bb_splice_end_plate_form_step15_feedback step26 on step25.uid=step26.uid

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
			_fetch_ddcl_bb_splice_end_plate_existing_comments_result($feedback_data->step2_id, $feedback_data->step2_data_relation),
			$feedback_data->step3_question_ok_not_ok,
			$feedback_data->step3_question_not_ok_comment,
			_fetch_ddcl_bb_splice_end_plate_existing_comments_result($feedback_data->step3_id, $feedback_data->step3_data_relation),
			$feedback_data->step4_question_ok_not_ok,
			$feedback_data->step4_question_not_ok_comment,
			_fetch_ddcl_bb_splice_end_plate_existing_comments_result($feedback_data->step4_id, $feedback_data->step4_data_relation),
			$feedback_data->step5_question_ok_not_ok,
			$feedback_data->step5_question_not_ok_comment,
			_fetch_ddcl_bb_splice_end_plate_existing_comments_result($feedback_data->step5_id, $feedback_data->step5_data_relation),
			$feedback_data->step6_question_ok_not_ok,
			$feedback_data->step6_question_not_ok_comment,
			_fetch_ddcl_bb_splice_end_plate_existing_comments_result($feedback_data->step6_id, $feedback_data->step6_data_relation),
			$feedback_data->step7_question_ok_not_ok,
			$feedback_data->step7_question_not_ok_comment,
			_fetch_ddcl_bb_splice_end_plate_existing_comments_result($feedback_data->step7_id, $feedback_data->step7_data_relation),
			$feedback_data->step8_question_ok_not_ok,
			$feedback_data->step8_question_not_ok_comment,
			_fetch_ddcl_bb_splice_end_plate_existing_comments_result($feedback_data->step8_id, $feedback_data->step8_data_relation),
			$feedback_data->step9_question_ok_not_ok,
			$feedback_data->step9_question_not_ok_comment,
			_fetch_ddcl_bb_splice_end_plate_existing_comments_result($feedback_data->step9_id, $feedback_data->step9_data_relation),
			$feedback_data->step10_question_ok_not_ok,
			$feedback_data->step10_question_not_ok_comment,
			_fetch_ddcl_bb_splice_end_plate_existing_comments_result($feedback_data->step10_id, $feedback_data->step10_data_relation),
			$feedback_data->step11_question_ok_not_ok,
			$feedback_data->step11_question_not_ok_comment,
			_fetch_ddcl_bb_splice_end_plate_existing_comments_result($feedback_data->step11_id, $feedback_data->step11_data_relation),
			$feedback_data->step12_question_ok_not_ok,
			$feedback_data->step12_question_not_ok_comment,
			_fetch_ddcl_bb_splice_end_plate_existing_comments_result($feedback_data->step12_id, $feedback_data->step12_data_relation),
			$feedback_data->step13_question_ok_not_ok,
			$feedback_data->step13_question_not_ok_comment,
			_fetch_ddcl_bb_splice_end_plate_existing_comments_result($feedback_data->step13_id, $feedback_data->step13_data_relation),
			$feedback_data->step14_question_ok_not_ok,
			$feedback_data->step14_question_not_ok_comment,
			_fetch_ddcl_bb_splice_end_plate_existing_comments_result($feedback_data->step14_id, $feedback_data->step14_data_relation),
			$feedback_data->step15_question_ok_not_ok,
			$feedback_data->step15_question_not_ok_comment,
			_fetch_ddcl_bb_splice_end_plate_existing_comments_result($feedback_data->step15_id, $feedback_data->step15_data_relation),
			$feedback_data->step16_question_ok_not_ok,
			$feedback_data->step16_question_not_ok_comment,
			_fetch_ddcl_bb_splice_end_plate_existing_comments_result($feedback_data->step16_id, $feedback_data->step16_data_relation),
			$feedback_data->step17_question_ok_not_ok,
			$feedback_data->step17_question_not_ok_comment,
			_fetch_ddcl_bb_splice_end_plate_existing_comments_result($feedback_data->step17_id, $feedback_data->step17_data_relation),
			$feedback_data->step18_question_ok_not_ok,
			$feedback_data->step18_question_not_ok_comment,
			_fetch_ddcl_bb_splice_end_plate_existing_comments_result($feedback_data->step18_id, $feedback_data->step18_data_relation),
			$feedback_data->step19_question_ok_not_ok,
			$feedback_data->step19_question_not_ok_comment,
			_fetch_ddcl_bb_splice_end_plate_existing_comments_result($feedback_data->step19_id, $feedback_data->step19_data_relation),
			$feedback_data->step20_question_ok_not_ok,
			$feedback_data->step20_question_not_ok_comment,
			_fetch_ddcl_bb_splice_end_plate_existing_comments_result($feedback_data->step20_id, $feedback_data->step20_data_relation),
			$feedback_data->step21_question_ok_not_ok,
			$feedback_data->step21_question_not_ok_comment,
			_fetch_ddcl_bb_splice_end_plate_existing_comments_result($feedback_data->step21_id, $feedback_data->step21_data_relation),
			$feedback_data->step22_question_ok_not_ok,
			$feedback_data->step22_question_not_ok_comment,
			_fetch_ddcl_bb_splice_end_plate_existing_comments_result($feedback_data->step22_id, $feedback_data->step22_data_relation),
			$feedback_data->step23_question_ok_not_ok,
			$feedback_data->step23_question_not_ok_comment,
			_fetch_ddcl_bb_splice_end_plate_existing_comments_result($feedback_data->step23_id, $feedback_data->step23_data_relation),
			$feedback_data->step24_question_ok_not_ok,
			$feedback_data->step24_question_not_ok_comment,
			_fetch_ddcl_bb_splice_end_plate_existing_comments_result($feedback_data->step24_id, $feedback_data->step24_data_relation),
			$feedback_data->step25_question_ok_not_ok,
			$feedback_data->step25_question_not_ok_comment,
			_fetch_ddcl_bb_splice_end_plate_existing_comments_result($feedback_data->step25_id, $feedback_data->step25_data_relation),
			_fetch_ddcl_bb_splice_end_plate_existing_comments_result($feedback_data->step26_id, $feedback_data->step26_data_relation)
		);
		$i++;
	}
	//$pending_data = $pending_q->fetchObject()
	/* check if there are any pending proposals */
	if (!$feedback_q)
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
		'Page 2 Material Strength',
		'Page 2 Material Strength not-ok comment',
		'Page 2 comments',
		'Page 3 Bolt-shear-black bolt',
		'Page 3 Bolt-shear-black bolt not-ok comment',
		'Page 3 comments',
		'Page 4 Bolt-shear-HSFG',
		'Page 4 Bolt-shear-HSFG not-ok comment',
		'Page 4 comments',
		'Page 5 Bolt-bearing-capacity',
		'Page 5 Bolt-bearing-capacity not-ok comment',
		'Page 5 comments',
		'Page 6 Bolt value ',
		'Page 6 Bolt value not-ok comment',
		'Page 6 comments',
		'Page 7 Bolt-numbers-and-group-capacity',
		'Page 7 Bolt-numbers-and-group-capacity not-ok comment',
		'Page 7 comments',
		'Page 8 Plate-height',
		'Page 8 Plate-height not-ok comment',
		'Page 8 comments',
		'Page 9 Plate-thickness',
		'Page 9 Plate-thickness not-ok comment',
		'Page 9 comments',
		'Page 10 Plate-block-shear',
		'Page 10 Plate-block-shear not-ok comment',
		'Page 10 comments',
		'Page 11 Weld-shear-strength',
		'Page 11 Weld-shear-strength not-ok comment',
		'Page 11 comments',
		'Page 12 Weld-minimum-thickness',
		'Page 12 Weld-minimum-thickness not-ok comment',
		'Page 12 comments',
		'Page 13 Detailing-checks-spacing',
		'Page 13 Detailing-checks-spacing not-ok comment',
		'Page 13 comments',
		'Page 14 Detailing-checks-end_edge',
		'Page 14 Detailing-checks-end_edge not-ok comment',
		'Page 14 comments',
		'Page 15 Sample',
		'Page 15 Sample',
		'Page 15 comments',
		'Page 16 Sample',
		'Page 16 Sample',
		'Page 16 comments',
		'Page 17 Sample',
		'Page 17 Sample',
		'Page 17 comments',
		'Page 18 Sample',
		'Page 18 Sample',
		'Page 18 comments',
		'Page 19 Sample',
		'Page 19 Sample',
		'Page 19 comments',
		'Page 20 Sample',
		'Page 20 Sample',
		'Page 20 comments',
		'Page 21 Sample',
		'Page 21 Sample',
		'Page 21 comments',
		'Page 22 Sample',
		'Page 22 Sample',
		'Page 22 comments',
		'Page 23 Sample',
		'Page 23 Sample',
		'Page 23 comments',
		'Page 24 Sample',
		'Page 24 Sample',
		'Page 24 comments',
		'Page 25 Sample',
		'Page 25 Sample',
		'Page 25 comments',
		'Page 26 Any other comments'
	);
	$output .= theme('table', array(
		'header' => $data_header,
		'rows' => $feedback_rows,
		'sticky' => FALSE
	));
	return $output;
}
//displayed in user page
function _fetch_existing_ddcl_bb_splice_end_plate_comments($step_id, $data_relation)
{
	global $base_url;
	$query_for_comment = db_query("SELECT * FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id AND data_relation=:data_relation ORDER BY number ASC", array(
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
			<a href='{$base_url}/ddcl-bb-splice-end-plate/delete/{$row->data_relation}/{$row->id}'>Delete</a><br>" . $row->inserted_on . "</h6><br>";
			$i++;
		} //$row = $query_for_comment->fetchObject()
	} //$query_for_comment == NULL
	else
	{
		$comment[$i] = 'No earlier comments';
	}
	return $comment;
}
function ddcl_bb_splice_end_plate_delete_comment()
{
	global $base_url;
	$comment_id = arg(3);
	$comment_step = arg(2);
	if ($comment_id)
	{
		$query = "DELETE FROM {ddcl_bb_splice_end_plate_feedback_form_comment} WHERE id = :id";
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
	drupal_goto("{$base_url}/ddcl-bb-splice-end-plate/{$comment_step}/");
}
function _fetch_ddcl_bb_splice_end_plate_existing_comments($step_id, $data_relation)
{
	$query_for_comment = db_query("SELECT * FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id AND data_relation=:data_relation ORDER BY number ASC", array(
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
function _fetch_ddcl_bb_splice_end_plate_existing_comments_result($step_id, $data_relation)
{
	$query_for_comment = db_query("SELECT * FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id AND data_relation=:data_relation ORDER BY number ASC", array(
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
function _fetch_ddcl_bb_splice_end_plate_existing_comments_result_csv($step_id, $data_relation)
{
	$query_for_comment = db_query("SELECT * FROM ddcl_bb_splice_end_plate_feedback_form_comment WHERE step1_form_id = :step1_form_id AND data_relation=:data_relation ORDER BY number ASC", array(
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
function ddcl_bb_splice_end_plate_result_generate_csv()
{
	global $user;
	if ($user->uid != NULL)
	{
		$result_dir = $_SERVER['DOCUMENT_ROOT'] . base_path() . "osdag-downloads/ddcl-bb-splice-end-plate-result/{$user->uid}/";
		if (!file_exists($result_dir))
		{
			mkdir($result_dir, 0755, TRUE);
		} //!file_exists($result_dir)
		$result_file = $result_dir . "ddcl-bb-splice-end-plate-result-{$user->uid}" . ".csv";
		$csv_file = "ddcl-bb-splice-end-plate-result-{$user->uid}" . ".csv";
		$fp = fopen($result_file, "w");
		/* making the first row */
		$item = array(
			'No.',
		'Date of Submission',
		'Name',
		'Institute / Company',
		'Current Position',
		'Page 2 Material Strength',
		'Page 2 Material Strength not-ok comment',
		'Page 2 comments',
		'Page 3 Bolt-shear-black bolt',
		'Page 3 Bolt-shear-black bolt not-ok comment',
		'Page 3 comments',
		'Page 4 Bolt-shear-HSFG',
		'Page 4 Bolt-shear-HSFG not-ok comment',
		'Page 4 comments',
		'Page 5 Bolt-bearing-capacity',
		'Page 5 Bolt-bearing-capacity not-ok comment',
		'Page 5 comments',
		'Page 6 Bolt value ',
		'Page 6 Bolt value not-ok comment',
		'Page 6 comments',
		'Page 7 Bolt-numbers-and-group-capacity',
		'Page 7 Bolt-numbers-and-group-capacity not-ok comment',
		'Page 7 comments',
		'Page 8 Plate-height',
		'Page 8 Plate-height not-ok comment',
		'Page 8 comments',
		'Page 9 Plate-thickness',
		'Page 9 Plate-thickness not-ok comment',
		'Page 9 comments',
		'Page 10 Plate-block-shear',
		'Page 10 Plate-block-shear not-ok comment',
		'Page 10 comments',
		'Page 11 Weld-shear-strength',
		'Page 11 Weld-shear-strength not-ok comment',
		'Page 11 comments',
		'Page 12 Weld-minimum-thickness',
		'Page 12 Weld-minimum-thickness not-ok comment',
		'Page 12 comments',
		'Page 13 Detailing-checks-spacing',
		'Page 13 Detailing-checks-spacing not-ok comment',
		'Page 13 comments',
		'Page 14 Detailing-checks-end_edge',
		'Page 14 Detailing-checks-end_edge not-ok comment',
		'Page 14 comments',
		'Page 15 Sample',
		'Page 15 Sample',
		'Page 15 comments',
		'Page 16 Sample',
		'Page 16 Sample',
		'Page 16 comments',
		'Page 17 Sample',
		'Page 17 Sample',
		'Page 17 comments',
		'Page 18 Sample',
		'Page 18 Sample',
		'Page 18 comments',
		'Page 19 Sample',
		'Page 19 Sample',
		'Page 19 comments',
		'Page 20 Sample',
		'Page 20 Sample',
		'Page 20 comments',
		'Page 21 Sample',
		'Page 21 Sample',
		'Page 21 comments',
		'Page 22 Sample',
		'Page 22 Sample',
		'Page 22 comments',
		'Page 23 Sample',
		'Page 23 Sample',
		'Page 23 comments',
		'Page 24 Sample',
		'Page 24 Sample',
		'Page 24 comments',
		'Page 25 Sample',
		'Page 25 Sample',
		'Page 25 comments',
		'Page 26 Any other comments'
		);
		fputcsv($fp, $item);
		$query = 'select

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
step17.question1 as "step17_question1",
step17.question_ok_not_ok as "step17_question_ok_not_ok",
step17.question_not_ok_comment as "step17_question_not_ok_comment",

step18.id as "step18_id",
step18.data_relation as "step18_data_relation",
step18.question1 as "step18_question1",
step18.question_ok_not_ok as "step18_question_ok_not_ok",
step18.question_not_ok_comment as "step18_question_not_ok_comment",

step19.id as "step19_id",
step19.data_relation as "step19_data_relation",
step19.question1 as "step19_question1",
step19.question_ok_not_ok as "step19_question_ok_not_ok",
step19.question_not_ok_comment as "step19_question_not_ok_comment",

step20.id as "step20_id",
step20.data_relation as "step20_data_relation",
step20.question1 as "step20_question1",
step20.question_ok_not_ok as "step20_question_ok_not_ok",
step20.question_not_ok_comment as "step20_question_not_ok_comment",

step21.id as "step21_id",
step21.data_relation as "step21_data_relation",
step21.question1 as "step21_question1",
step21.question_ok_not_ok as "step21_question_ok_not_ok",
step21.question_not_ok_comment as "step21_question_not_ok_comment",

step22.id as "step22_id",
step22.data_relation as "step22_data_relation",
step22.question1 as "step22_question1",
step22.question_ok_not_ok as "step22_question_ok_not_ok",
step22.question_not_ok_comment as "step22_question_not_ok_comment",

step23.id as "step23_id",
step23.data_relation as "step23_data_relation",
step23.question1 as "step23_question1",
step23.question_ok_not_ok as "step23_question_ok_not_ok",
step23.question_not_ok_comment as "step23_question_not_ok_comment",

step24.id as "step24_id",
step24.data_relation as "step24_data_relation",
step24.question1 as "step24_question1",
step24.question_ok_not_ok as "step24_question_ok_not_ok",
step24.question_not_ok_comment as "step24_question_not_ok_comment",

step25.id as "step25_id",
step25.data_relation as "step25_data_relation",
step25.question1 as "step25_question1",
step25.question_ok_not_ok as "step25_question_ok_not_ok",
step25.question_not_ok_comment as "step25_question_not_ok_comment",

step26.id as "step26_id",
step26.data_relation as "step26_data_relation",
step26.question1 as "step26_question1"


from ddcl_bb_splice_end_plate_form_step1_feedback step1

join ddcl_bb_splice_end_plate_form_step2_feedback step2 on step1.uid=step2.uid
join ddcl_bb_splice_end_plate_form_step3_feedback step3 on step2.uid=step3.uid
join ddcl_bb_splice_end_plate_form_step4_feedback step4 on step3.uid=step4.uid
join ddcl_bb_splice_end_plate_form_step5_feedback step5 on step4.uid=step5.uid
join ddcl_bb_splice_end_plate_form_step6_feedback step6 on step5.uid=step6.uid
join ddcl_bb_splice_end_plate_form_step7_feedback step7 on step6.uid=step7.uid
join ddcl_bb_splice_end_plate_form_step8_feedback step8 on step7.uid=step8.uid
join ddcl_bb_splice_end_plate_form_step9_feedback step9 on step8.uid=step9.uid
join ddcl_bb_splice_end_plate_form_step10_feedback step10 on step9.uid=step10.uid
join ddcl_bb_splice_end_plate_form_step11_feedback step11 on step10.uid=step11.uid
join ddcl_bb_splice_end_plate_form_step12_feedback step12 on step11.uid=step12.uid
join ddcl_bb_splice_end_plate_form_step13_feedback step13 on step12.uid=step13.uid
join ddcl_bb_splice_end_plate_form_step14_feedback step14 on step13.uid=step14.uid
join ddcl_bb_splice_end_plate_form_step15_feedback step15 on step14.uid=step15.uid
join ddcl_bb_splice_end_plate_form_step15_feedback step16 on step15.uid=step16.uid
join ddcl_bb_splice_end_plate_form_step15_feedback step17 on step16.uid=step17.uid
join ddcl_bb_splice_end_plate_form_step15_feedback step18 on step17.uid=step18.uid
join ddcl_bb_splice_end_plate_form_step15_feedback step19 on step18.uid=step19.uid
join ddcl_bb_splice_end_plate_form_step15_feedback step20 on step19.uid=step20.uid
join ddcl_bb_splice_end_plate_form_step15_feedback step21 on step20.uid=step21.uid
join ddcl_bb_splice_end_plate_form_step15_feedback step22 on step21.uid=step22.uid
join ddcl_bb_splice_end_plate_form_step15_feedback step23 on step22.uid=step23.uid
join ddcl_bb_splice_end_plate_form_step15_feedback step24 on step23.uid=step24.uid
join ddcl_bb_splice_end_plate_form_step15_feedback step25 on step24.uid=step25.uid
join ddcl_bb_splice_end_plate_form_step15_feedback step26 on step25.uid=step26.uid

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
				_fetch_ddcl_bb_splice_end_plate_existing_comments_result_csv($feedback_data->step2_id, $feedback_data->step2_data_relation),
				$feedback_data->step3_question_ok_not_ok,
				$feedback_data->step3_question_not_ok_comment,
				_fetch_ddcl_bb_splice_end_plate_existing_comments_result_csv($feedback_data->step3_id, $feedback_data->step3_data_relation),
				$feedback_data->step4_question_ok_not_ok,
				$feedback_data->step4_question_not_ok_comment,
				_fetch_ddcl_bb_splice_end_plate_existing_comments_result_csv($feedback_data->step4_id, $feedback_data->step4_data_relation),
				$feedback_data->step5_question_ok_not_ok,
				$feedback_data->step5_question_not_ok_comment,
				_fetch_ddcl_bb_splice_end_plate_existing_comments_result_csv($feedback_data->step5_id, $feedback_data->step5_data_relation),
				$feedback_data->step6_question_ok_not_ok,
				$feedback_data->step6_question_not_ok_comment,
				_fetch_ddcl_bb_splice_end_plate_existing_comments_result_csv($feedback_data->step6_id, $feedback_data->step6_data_relation),
				$feedback_data->step7_question_ok_not_ok,
				$feedback_data->step7_question_not_ok_comment,
				_fetch_ddcl_bb_splice_end_plate_existing_comments_result_csv($feedback_data->step7_id, $feedback_data->step7_data_relation),
				$feedback_data->step8_question_ok_not_ok,
				$feedback_data->step8_question_not_ok_comment,
				_fetch_ddcl_bb_splice_end_plate_existing_comments_result_csv($feedback_data->step8_id, $feedback_data->step8_data_relation),
				$feedback_data->step9_question_ok_not_ok,
				$feedback_data->step9_question_not_ok_comment,
				_fetch_ddcl_bb_splice_end_plate_existing_comments_result_csv($feedback_data->step9_id, $feedback_data->step9_data_relation),
				$feedback_data->step10_question_ok_not_ok,
				$feedback_data->step10_question_not_ok_comment,
				_fetch_ddcl_bb_splice_end_plate_existing_comments_result_csv($feedback_data->step10_id, $feedback_data->step10_data_relation),
				$feedback_data->step11_question_ok_not_ok,
				$feedback_data->step11_question_not_ok_comment,
				_fetch_ddcl_bb_splice_end_plate_existing_comments_result_csv($feedback_data->step11_id, $feedback_data->step11_data_relation),
				$feedback_data->step12_question_ok_not_ok,
				$feedback_data->step12_question_not_ok_comment,
				_fetch_ddcl_bb_splice_end_plate_existing_comments_result_csv($feedback_data->step12_id, $feedback_data->step12_data_relation),
				$feedback_data->step13_question_ok_not_ok,
				$feedback_data->step13_question_not_ok_comment,
				_fetch_ddcl_bb_splice_end_plate_existing_comments_result_csv($feedback_data->step13_id, $feedback_data->step13_data_relation),
				$feedback_data->step14_question_ok_not_ok,
				$feedback_data->step14_question_not_ok_comment,
				_fetch_ddcl_bb_splice_end_plate_existing_comments_result_csv($feedback_data->step14_id, $feedback_data->step14_data_relation),
				$feedback_data->step15_question_ok_not_ok,
				$feedback_data->step15_question_not_ok_comment,
				_fetch_ddcl_bb_splice_end_plate_existing_comments_result_csv($feedback_data->step15_id, $feedback_data->step15_data_relation),
				$feedback_data->step16_question_ok_not_ok,
				$feedback_data->step16_question_not_ok_comment,
				_fetch_ddcl_bb_splice_end_plate_existing_comments_result_csv($feedback_data->step16_id, $feedback_data->step16_data_relation),
				$feedback_data->step17_question_ok_not_ok,
				$feedback_data->step17_question_not_ok_comment,
				_fetch_ddcl_bb_splice_end_plate_existing_comments_result_csv($feedback_data->step17_id, $feedback_data->step17_data_relation),
				$feedback_data->step18_question_ok_not_ok,
				$feedback_data->step18_question_not_ok_comment,
				_fetch_ddcl_bb_splice_end_plate_existing_comments_result_csv($feedback_data->step18_id, $feedback_data->step18_data_relation),
				$feedback_data->step19_question_ok_not_ok,
				$feedback_data->step19_question_not_ok_comment,
				_fetch_ddcl_bb_splice_end_plate_existing_comments_result_csv($feedback_data->step19_id, $feedback_data->step19_data_relation),
				$feedback_data->step20_question_ok_not_ok,
				$feedback_data->step20_question_not_ok_comment,
				_fetch_ddcl_bb_splice_end_plate_existing_comments_result_csv($feedback_data->step20_id, $feedback_data->step20_data_relation),
				$feedback_data->step21_question_ok_not_ok,
				$feedback_data->step21_question_not_ok_comment,
				_fetch_ddcl_bb_splice_end_plate_existing_comments_result_csv($feedback_data->step21_id, $feedback_data->step21_data_relation),
				$feedback_data->step22_question_ok_not_ok,
				$feedback_data->step22_question_not_ok_comment,
				_fetch_ddcl_bb_splice_end_plate_existing_comments_result_csv($feedback_data->step22_id, $feedback_data->step22_data_relation),
				$feedback_data->step23_question_ok_not_ok,
				$feedback_data->step23_question_not_ok_comment,
				_fetch_ddcl_bb_splice_end_plate_existing_comments_result_csv($feedback_data->step23_id, $feedback_data->step23_data_relation),
				$feedback_data->step24_question_ok_not_ok,
				$feedback_data->step24_question_not_ok_comment,
				_fetch_ddcl_bb_splice_end_plate_existing_comments_result_csv($feedback_data->step24_id, $feedback_data->step24_data_relation),
				$feedback_data->step25_question_ok_not_ok,
				$feedback_data->step25_question_not_ok_comment,
				_fetch_ddcl_bb_splice_end_plate_existing_comments_result_csv($feedback_data->step25_id, $feedback_data->step25_data_relation),
				_fetch_ddcl_bb_splice_end_plate_existing_comments_result_csv($feedback_data->step26_id, $feedback_data->step26_data_relation)
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

