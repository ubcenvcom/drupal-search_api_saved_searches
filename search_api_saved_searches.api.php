<?php

/**
 * @file
 * Hooks provided by the Search API saved searches module.
 */

/**
 * @addtogroup hooks
 * @{
 */

// @todo Document settings/searches CRUD hooks

/**
 * Allows other modules to alter or react on new results found for a saved
 * search. The results will then be used to send a mail to the saved search's
 * creator.
 *
 * @param array $results
 *   An array of entities representing new results for the search.
 * @param SearchApiSavedSearch $search
 *   The saved search that was executed.
 */
function hook_search_api_saved_searches_new_results_alter(array &$results, SearchApiSavedSearch $search) {
  // Remove all results with an ID that is a multiple of 6.
  foreach (array_keys($results) as $id) {
    if ($i % 6 == 0) {
      unset($results[$i]);
    }
  }
}

/**
 * @} End of "addtogroup hooks".
 */
