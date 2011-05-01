<?php

/**
 * @file
 * Builds placeholder replacement tokens for user-related data.
 */

/**
 * Implements hook_token_info().
 *
 * Provides the following tokens:
 * - [user:search-api-saved-searches-url]
 * - [search-api-saved-searches:results]
 * - [search-api-saved-search:name]
 * - [search-api-saved-search:created]
 * - [search-api-saved-search:items]
 * - [search-api-saved-search:view-url]
 * - [search-api-saved-search:activate-url]
 * - [search-api-saved-search:delete-url]
 * - [search-api-saved-search-result:label]
 * - [search-api-saved-search-result:url]
 *
 * The used data types are the following:
 * - search_api_saved_searches: An array with values of type
 *   search_api_saved_search_info.
 * - search_api_saved_search_info: An associative array containing:
 *   - search: A SearchApiSavedSearch entity object.
 *   - results: An array of entities of the type that the index associated with
 *     the saved search contains.
 * - search_api_saved_search_result_info: An associative array containing:
 *   - search: A SearchApiSavedSearch entity object.
 *   - entity: An entity of the type corresponding to the saved search.
 *
 * @see search_api_saved_searches_tokens()
 */
function search_api_saved_searches_token_info() {
  $info['types']['search-api-saved-searches'] = array(
    'name' => t('Search API saved searches'),
    'description' => t('Tokens related to several saved searches at once.'),
    'needs-data' => 'search_api_saved_searches',
  );
  $info['types']['search-api-saved-search'] = array(
    'name' => t('Search API saved search'),
    'description' => t('Tokens related to a saved search.'),
    'needs-data' => 'search_api_saved_search_info',
  );
  $info['types']['search-api-saved-search-result'] = array(
    'name' => t('Search API saved search result'),
    'description' => t('Tokens related to a result of a saved search.'),
    'needs-data' => 'search_api_saved_search_result_info',
  );

  $tokens = &$info['tokens'];
  $tokens['user']['search-api-saved-searches-url'] = array(
    'name' => t('Config URL'),
    'description' => t('The URL under which a user can configure her saved searches.'),
  );
  $tokens['search-api-saved-searches']['results'] = array(
    'name' => t('Results'),
    'description' => t('Text listing new results for one or more saved searches.'),
  );
  $search = &$tokens['search-api-saved-search'];
  $search['name'] = array(
    'name' => t('Name'),
    'description' => t('The name of the saved search.'),
  );
  $search['created'] = array(
    'name' => t('Date created'),
    'description' => t('The date on which the saved search was created.'),
    'type' => 'date',
  );
  $search['items'] = array(
    'name' => t('Results'),
    'description' => t('Text listing new results for the saved search.'),
  );
  $search['view-url'] = array(
    'name' => t('View URL'),
    'description' => t('The URL of the page displaying this search, if any.'),
  );
  $search['activate-url'] = array(
    'name' => t('Activate URL'),
    'description' => t('The URL for activating the saved search, if necessary.'),
  );
  $search['delete-url'] = array(
    'name' => t('Delete URL'),
    'description' => t('The URL for deleting the saved search.'),
  );
  $tokens['search-api-saved-search-result']['label'] = array(
    'name' => t('Label'),
    'description' => t('The human-readable label for the result.'),
  );
  $tokens['search-api-saved-search-result']['url'] = array(
    'name' => t('URL'),
    'description' => t('The URL at which the result can be viewed.'),
  );

  return $info;
}

/**
 * Implements hook_tokens().
 *
 * @see search_api_saved_searches_token_info()
 */
function search_api_saved_searches_tokens($type, array $tokens, array $data = array(), array $options = array()) {
  $url_options = array('absolute' => TRUE);
  if (isset($options['language'])) {
    $url_options['language'] = $options['language'];
    $language_code = $options['language']->language;
  }
  else {
    $language_code = NULL;
  }
  $sanitize = !empty($options['sanitize']);

  $replacements = array();

  switch ($type) {
    case 'user':
      if (!empty($data['user']->uid)) {
        if (isset($tokens['search-api-saved-searches-url'])) {
          $replacements[$tokens['search-api-saved-searches-url']] = url('user/' . $data['user']->uid . '/saved-searches', $url_options);
        }
      }
      break;

    case 'search-api-saved-searches':
      if (!empty($data['search_api_saved_searches'])) {
        if (isset($tokens['results'])) {
          $results = array();
          foreach ($data['search_api_saved_searches'] as $info) {
            $search = $info['search'];
            $settings = $search->settings();
            if (empty($settings->options['mail']['notify']['results'])) {
              continue;
            }
            $text = $settings->options['mail']['notify']['results'];
            $params['user'] = $search->user();
            $params['search_api_saved_search_info'] = $info;
            $results[] = token_replace($text, $params, $options);
          }
          $text = implode("\n\n", $results);
          $replacements[$tokens['results']] = $sanitize ? check_plain($text) : $text;
        }
      }
      break;

    case 'search-api-saved-search':
      if (!empty($data['search_api_saved_search_info']['search'])) {
        $search = $data['search_api_saved_search_info']['search'];
        foreach ($tokens as $name => $original) {
          switch ($name) {
            case 'name':
              $replacements[$original] = $sanitize ? check_plain($search->name) : $search->name;
              break;
            case 'created':
              $replacements[$original] = format_date($search->created, 'medium', '', NULL, $language_code);
              break;
            case 'items':
              if (!empty($data['search_api_saved_search_info']['results'])) {
                $results = array();
                $settings = $search->settings();
                if (empty($settings->options['mail']['notify']['result'])) {
                  break;
                }
                $entity_type = $search->index()->entity_type;
                $text = $settings->options['mail']['notify']['result'];
                foreach ($data['search_api_saved_search_info']['results'] as $entity) {
                  $params = array(
                    $entity_type => $entity,
                    'search_api_saved_search_result_info' => array(
                      'search' => $search,
                      'entity' => $entity,
                    ),
                  );
                  $results[] = token_replace($text, $params, $options);
                }
                $text = implode("\n", $results);
                $replacements[$original] = $sanitize ? check_plain($text) : $text;
              }
              break;
            case 'view-url':
              $replacements[$original] = $search->url();
              break;
            case 'activate-url':
              $replacements[$original] = url('search-api/saved-search/' . $search->id . '/activate', $url_options);
              break;
            case 'delete-url':
              $replacements[$original] = url('search-api/saved-search/' . $search->id . '/delete', $url_options);
              break;
          }
        }
        if ($created_tokens = token_find_with_prefix($tokens, 'created')) {
          $replacements += token_generate('date', $created_tokens, array('date' => $search->created), $options);
        }
      }
      break;

    case 'search-api-saved-search-result':
      $d = $data['search_api_saved_search_result_info'];
      if (!empty($d['search']) && !empty($d['entity'])) {
        $search = $d['search'];
        $entity = $d['entity'];
        $entity_type = $search->index()->entity_type;
        foreach ($tokens as $name => $original) {
          if ($name == 'label' && ($label == entity_label($entity_type, $entity))) {
            $replacements[$original] = $label;
          }
          elseif ($name == 'url' && ($url == entity_uri($entity_type, $entity))) {
            if ($entity_type == 'file') {
              $url = array(
                'path' => file_create_url($url),
                'options' => array(),
              );
            }
            $url['options'] += $url_options;
            $replacements[$original] = url($url['path'], $url['options']);
          }
        }
      }
      break;
  }

  return $replacements;
}
