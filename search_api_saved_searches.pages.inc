<?php

/**
 * @file
 * User UI functions and form callbacks for saved searches.
 */

/**
 * Page callback for listing all saved searches of a user.
 *
 * @param $account
 *   The user of which to list saved searches.
 */
function search_api_saved_searches_user_listing($account) {
  $base_path = 'user/' . $account->uid . '/saved-searches/';
  $header = array(
    t('Name'),
    t('Created'),
    t('Last executed'),
    t('Interval'),
    array('data' => t('Operations'), 'colspan' => 2),
  );
  $intervals = array(
    60 * 60 => t('Hourly'),
    60 * 60 * 24 => t('Daily'),
    60 * 60 * 24 * 7 => t('Weekly'),
  );
  $searches = search_api_saved_search_load_multiple(FALSE, array('uid' => $account->uid));
  $rows = array();
  foreach ($searches as $search) {
    if (empty($search->options['page'])) {
      $name = check_plain($search->name);
    }
    else {
      $name = l($search->name, $search->options['page']['path'], $search->options['page']);
    }
    $created = format_date($search->created, 'short');
    $last_execute = t('@time ago', array('@time' => format_interval(REQUEST_TIME - $search->last_execute)));
    $interval = format_interval($search->notify_interval, 1);
    $path = $base_path . $search->id;
    $rows[] = array(
      $name,
      $created,
      $last_execute,
      $interval,
      l(t('edit'), $path),
      l(t('delete'), $path . '/delete'),
    );
  }

  $render = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('No searches were saved yet.'),
    '#sticky' => TRUE,
  );
  return $render;
}
