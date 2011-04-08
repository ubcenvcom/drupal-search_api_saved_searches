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
  // Set default options.
  $settings->options += array(
    'default_true' => TRUE,
    'ids_list' => array(),
    'user_select_interval' => TRUE,
    'interval_options' => array(
      60 * 60 * 24     => t('Daily'),
      60 * 60 * 24 * 7 => t('Weekly'),
    ),
    'set_interval' => 60 * 60 * 24,
    'mail' => array(),
  );
  $settings->options['mail'] += array(
    'html' => FALSE,
    'title' => t('New results for your saved search at [site:name]'),
    'body' => t('[user:name],

There are new results for your saved search on [site:name]:

[search-api-saved-searches:results]

You can configure your saved searches at the following address:
[user:search-api-saved-searches-url]

--  [site:name] team'),
    'results' => "[search-api-saved-search:name]:\n[search-api-saved-search:items]\n" .
        t('Delete this saved search: !url', array('!url' => '[search-api-saved-search:delete-url]')),
    'result' => NULL,
  );
  if (!isset($settings->options['mail']['result'])) {
    // This is done extra, if necessary, as it requires some computing.
    // And some guessing.
    $tokens = token_info();
    $type = $index->entity_type;
    if (empty($tokens['types'][$type])) {
      foreach ($tokens['types'] as $k => $v) {
        if (isset($v['needs-data']) && $v['needs-data'] == $type) {
          $type = $k;
          break;
        }
      }
    }
    $entity_info = entity_get_info($type);
    if (isset($entity_info['entity keys']['label']) && isset($tokens['tokens'][$type][$entity_info['entity keys']['label']])) {
      $label = '[' . $type . ':' . $entity_info['entity keys']['label'] . ']';
    }
    elseif (isset($tokens['tokens'][$type][$entity_info['entity keys']['id']])) {
      $label = '[' . $type . ':' . $entity_info['entity keys']['id'] . ']';
    }
    elseif (isset($tokens['tokens'][$type]['label'])) {
      $label = "[$type:label]";
    }
    elseif (isset($tokens['tokens'][$type]['name'])) {
      $label = "[$type:name]";
    }
    else {
      $label = '';
    }
    if (isset($tokens['tokens'][$type]['url'])) {
      $url = " ([$type:url])";
    }
    else {
      $url = '';
    }

    $settings->options['mail']['result'] = "- $label$url";
  }
  $form_state['settings'] = $settings;
  $options = $settings->options;

  $form['#tree'] = TRUE;
  $form['enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable'),
    '#description' => t('Enable saved searches for this index.'),
    '#default_value' => $settings->enabled,
  );
  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Settings'),
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
    '#collapsible' => TRUE,
  );
  $form['options']['interval']['user_select_interval'] = array(
    '#type' => 'checkbox',
    '#title' => t('Let users select the interval'),
    '#default_value' => $options['user_select_interval'],
  );
  $interval_options = array();
  foreach ($options['interval_options'] as $k => $v) {
    $interval_options[] = "$k | $v";
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
    '#default_value' => isset($interval_options[$options['set_interval']]) ? $options['set_interval'] : 0,
    '#states' => array(
      'visible' => array(
        ':input[name="options[interval][set_interval]"]' => array(
          'visible' => TRUE,
          'value' => '0',
        ),
      ),
    ),
  );

  $form['options']['mail'] = array(
    '#type' => 'fieldset',
    '#title' => t('E-mail format'),
    '#collapsible' => TRUE,
  );
  $form['options']['mail']['html'] = array(
    '#type' => 'checkbox',
    '#title' => t('Send HTML mails'),
    '#description' => t('Use full HTML for the mail body, instead of sending it in plain text.'),
    '#default_value' => $options['mail']['html'],
  );
  $form['options']['mail']['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#description' => t("Enter the notification mail's subject.") . ' ' . t('Available variables are: @vars.',
        array('@vars' => '[site:name], [user:name], [user:mail]')),
    '#default_value' => $options['mail']['title'],
  );
  $form['options']['mail']['body'] = array(
    '#type' => 'textarea',
    '#title' => t('Body'),
    '#description' => t("Enter the notification mail's body.") . ' ' . t('Available variables are: @vars.',
        array('@vars' => '[site:name], [site:url], [user:name], [user:mail], [site:url-brief], ' .
            '[search-api-saved-searches:results], [user:search-api-saved-searches-url]')) . ' ' .
        t('The replacement for [search-api-saved-searches:results] can be configured below.'),
    '#default_value' => $options['mail']['body'],
    '#rows' => 12,
  );
  $form['options']['mail']['results'] = array(
    '#type' => 'textarea',
    '#title' => t('Results replacement'),
    '#description' => t('Enter the text that will replace the [search-api-saved-searches:results] variable above. If there are new results for several saved searches of a single user, this text will be inserted multiple times.') . ' ' .
        t('Available variables are: @vars.',
        array('@vars' => '[site:url], [search-api-saved-search:name], [search-api-saved-search:created], ' .
            '[search-api-saved-search:items], [search-api-saved-search:edit-url], [search-api-saved-search:delete-url]')) . ' ' .
        t('The replacement for [search-api-saved-search:items] can be configured below.'),
    '#default_value' => $options['mail']['results'],
    '#rows' => 5,
  );
  $form['options']['mail']['result'] = array(
    '#type' => 'textarea',
    '#title' => t('Result item replacement'),
    '#description' => t('Enter the text that will replace the [search-api-saved-search:items] variable above. This text will be inserted on a new line once for every new result.') . ' ' .
        t('Available variables are: @vars.',
        array('@vars' => '[site:url], [search-api-saved-search-result:label], [search-api-saved-search-result:url], ' .
            '[search-api-saved-search:edit-url], [search-api-saved-search:delete-url]')),
    '#default_value' => $options['mail']['result'],
    '#rows' => 2,
  );

  // @todo Access options? Maximum number of saved searches per user, maximum
  //   number of results to create saved search, … ?

  // @todo Options for creating a creation form.

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
  $values = &$form_state['values'];
  $interval_options = $values['options']['interval']['interval_options'];
  $values['options']['interval']['interval_options'] = array();
  foreach (explode("\n", $interval_options) as $line) {
    $parts = explode('|', $line, 2);
    if (count($parts) == 1) {
      $k = $v = trim($line);
    }
    else {
      list($k, $v) = array_map('trim', $parts);
    }
    $values['options']['interval']['interval_options'][$k] = $v;
  }
  if ($values['options']['interval']['user_select_interval']) {
    foreach ($values['options']['interval']['interval_options'] as $k => $v) {
      if (!preg_match('/^-?\d+$/', $k)) {
        form_error($form['options']['interval']['interval_options'], t('"@input" is not a valid integer. Only integers can be used as intervals.', array('@input' => $k)));
      }
    }
  }
  elseif ($values['options']['interval']['set_interval'] == 0) {
    $v = $values['options']['interval']['set_interval_custom'];
    if (!preg_match('/^-?\d+$/', $v)) {
      form_error($form['options']['interval']['set_interval_custom'], t('"@input" is not a valid integer. Only integers can be used as intervals.', array('@input' => $v)));
    }
  }
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

  $options = $values['options'];
  $options += $options['interval'];
  unset($options['interval']);
  if ($options['set_interval'] == 0) {
    $options['set_interval'] = $options['set_interval_custom'];
  }
  unset($options['set_interval_custom']);

  $settings->enabled = $values['enabled'];
  $settings->options = $options;

  $ret = $settings->save();
  if ($ret === FALSE) {
    drupal_set_message('An error occurred while trying to save the settings. Check the logs for details.', 'error');
    $form_state['rebuild'] = TRUE;
  }
  else {
    drupal_set_message('The settings were successfully saved.');
  }
}
