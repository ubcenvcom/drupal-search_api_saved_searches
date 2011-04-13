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
 * - [search-api-saved-search:activate-url]
 * - [search-api-saved-search:edit-url]
 * - [search-api-saved-search:delete-url]
 * - [search-api-saved-search-result:label]
 * - [search-api-saved-search-result:url]
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
  $search['activate-url'] = array(
    'name' => t('Activate URL'),
    'description' => t('The URL for activating the saved search, if necessary.'),
  );
  $search['edit-url'] = array(
    'name' => t('Edit URL'),
    'description' => t('The URL for editing the saved search.'),
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
