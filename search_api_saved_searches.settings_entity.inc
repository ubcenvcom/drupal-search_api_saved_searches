<?php

/**
 * @file Contains the entity class for stored "Saved searches" settings of
 *   search indexes.
 */

/**
 * Class representing "Saved searches" settings.
 */
class SearchApiSavedSearchesSettings extends Entity {

  // Database values that will be set when object is loaded

  /**
   * @var integer
   */
  public $id;

  /**
   * @var string
   */
  public $delta;

  /**
   * @var string
   */
  public $index_id;

  /**
   * @var array
   */
  public $options;

  /**
   * Constructor as a helper to the parent constructor.
   */
  public function __construct(array $values = array()) {
    parent::__construct($values, 'search_api_saved_searches_settings');
  }

}
