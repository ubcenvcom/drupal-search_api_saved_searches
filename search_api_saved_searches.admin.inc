<?php

/**
 * @file
 * Admin UI functions and form callbacks for saved searches.
 */

/**
 * Form builder for editing the saved search settings for an index.
 *
 * @param SearchApiIndex $index
 *   The index for which to edit settings.
 *
 * @see search_api_saved_searches_index_edit_validate()
 * @see search_api_saved_searches_index_edit_submit()
 * @ingroup forms
 */
function search_api_saved_searches_index_edit(array $form, array &$form_state, SearchApiIndex $index) {
  $settings = search_api_saved_searches_settings_load($index->machine_name);
  if (!$settings) {
    $settings = new SearchApiSavedSearchesSettings(array(
      'delta' => $index->machine_name,
      'index_id' => $index->machine_name,
      'enabled' => FALSE,
      'options' => array(),
    ));
  }
  $settings->options += array(
    'default_true' => TRUE,
    'ids_list' => array(),
    'user_select_interval' => TRUE,
    'interval_options' => array(
      60 * 60 * 24     => t('Daily'),
      60 * 60 * 24 * 7 => t('Weekly'),
    ),
    'set_interval' => 60 * 60 * 24,
  );
  $form_state['settings'] = $settings;
  $options = $settings->options;

  $form['enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable'),
    '#description' => t('Enable saved searches for this index.'),
    '#default_value' => $settings->enabled,
  );
  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Settings'),
    '#tree' => TRUE,
    '#states' => array(
      'invisible' => array(
        ':input[name="enabled"]' => array('checked' =>FALSE),
      ),
    ),
  );
  $search_ids = variable_get('search_api_saved_searches_search_ids', array());
  if (empty($search_ids[$index->machine_name]) || count($search_ids[$index->machine_name]) <= 1) {
    $form['options']['default_true'] = array(
      '#type' => 'value',
      '#value' => $options['default_true'],
    );
    $form['options']['ids_list'] = array(
      '#type' => 'value',
      '#value' => $options['ids_list'],
    );
  }
  else {
    $form['options']['default_true'] = array(
      '#type' => 'select',
      '#title' => t('Display for searches'),
      '#options' => array(
        TRUE => t('For all except the selected'),
        FALSE => t('Only for the selected'),
      ),
      '#default_value' => $options['default_true'],
    );
    $form['options']['ids_list'] = array(
      '#type' => 'select',
      '#title' => t('Search IDs'),
      '#options' => $search_ids[$index->machine_name],
      '#size' => min(4, count($search_ids[$index->machine_name])),
      '#multiple' => TRUE,
      '#default_value' => $options['ids_list'],
    );
  }

  $form['options']['interval'] = array(
    '#type' => 'fieldset',
    '#title' => t('Notification interval'),
  );
  $form['options']['interval']['user_select_interval'] = array(
    '#type' => 'checkbox',
    '#title' => t('Let users select the interval'),
    '#default_value' => $options['user_select_interval'],
  );
  $interval_options = array();
  foreach ($options['interval_options'] as $k => $v) {
    $interval_options[] = "$k|$v";
  }
  $interval_options = implode("\n", $interval_options);
  $form['options']['interval']['interval_options'] = array(
    '#type' => 'textarea',
    '#title' => t('Interval options'),
    '#description' => t('The possible intervals the user can choose from. Enter one value per line, in the format seconds|label.'),
    '#default_value' => $interval_options,
    '#states' => array(
      'visible' => array(
        ':input[name="options[interval][user_select_interval]"]' => array('checked' => TRUE),
      ),
    ),
  );
  $interval_options = array(
    60 * 60 * 24     => t('Daily'),
    60 * 60 * 24 * 7 => t('Weekly'),
    0 => t('Custom'),
  );
  $form['options']['interval']['set_interval'] = array(
    '#type' => 'select',
    '#title' => t('Fixed notification interval'),
    '#options' => $interval_options,
    '#default_value' => isset($interval_options[$options['set_interval']]) ? $options['set_interval'] : 0,
    '#states' => array(
      'visible' => array(
        ':input[name="options[interval][user_select_interval]"]' => array('checked' => FALSE),
      ),
    ),
  );
  $form['options']['interval']['set_interval_custom'] = array(
    '#type' => 'textfield',
    '#description' => t('Enter the custom notification interval, in seconds.'),
    '#options' => $interval_options,
    '#default_value' => isset($interval_options[$options['set_interval']]) ? $options['set_interval'] : 0,
    '#states' => array(
      'visible' => array(
        ':input[name="options[interval][set_interval]"]' => array(
          //'visible' => TRUE,
          'value' => '0',
        ),
      ),
    ),
  );

  // @todo Add e-mail text options and options for creating a creation form.

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save settings'),
  );

  return $form;
}

/**
 * Form validation handler for search_api_saved_searches_index_edit().
 *
 * @see search_api_saved_searches_index_edit()
 * @see search_api_saved_searches_index_edit_submit()
 */
function search_api_saved_searches_index_edit_validate(array $form, array &$form_state) {
  // @todo Validate something here?
}

/**
 * Form submission handler for search_api_saved_searches_index_edit().
 *
 * @see search_api_saved_searches_index_edit()
 * @see search_api_saved_searches_index_edit_vaidate()
 */
function search_api_saved_searches_index_edit_submit(array $form, array &$form_state) {
  $values = $form_state['values'];
  $settings = $form_state['settings'];

  $settings->enabled = $values['enabled'];
  $settings->options = $values['options'];

  $ret = $settings->save();
  if ($ret === FALSE) {
    drupal_set_message('An error occurred while trying to save the settings. Check the logs for details.', 'error');
    $form_state['rebuild'] = TRUE;
  }
  else {
    drupal_set_message('The settings were successfully saved.');
  }
}
