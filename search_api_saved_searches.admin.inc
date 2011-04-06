<?php

/**
 * @file Admin UI functions and form callbacks for saved searches.
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
        'input[name="enabled"]' => array('checked' =>FALSE),
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

  // @todo Add e-mail text options and options for creating a creation form.

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save settings'),
  );

  return $form;
}
