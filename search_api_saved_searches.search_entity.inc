<?php

/**
 * @file
 * Contains the entity class for saved searches.
 */

/**
 * Class representing "Saved searches" settings.
 */
class SearchApiSavedSearch extends Entity {

  /**
   * The settings this saved search uses.
   *
   * @var SearchApiSavedSearchesSettings
   */
  protected $settings;

  /**
   * The search index this saved search uses.
   *
   * @var SearchApiIndex
   */
  protected $index;

  // Database values that will be set when object is loaded

  /**
   * @var integer
   */
  public $id;

  /**
   * @var integer
   */
  public $uid;

  /**
   * @var string
   */
  public $settings_id;

  /**
   * @var string
   */
  public $name;

  /**
   * @var string
   */
  public $mail;

  /**
   * @var integer
   */
  public $created;

  /**
   * @var boolean
   */
  public $enabled;

  /**
   * @var array
   */
  public $options;

  /**
   * Constructor as a helper to the parent constructor.
   */
  public function __construct(array $values = array()) {
    parent::__construct($values, 'search_api_saved_search');
  }

  /**
   * @return SearchApiIndex
   *   The index this saved search uses.
   *
   * @throws SearchApiException
   *   If the index doesn't exist.
   */
  public function settings() {
    if (!isset($this->settings)) {
      $this->settings = search_api_saved_searches_settings_load($this->settings_id);
    }
    if (!$this->settings) {
      throw new SearchApiException(t("The saved search settings with the ID %id don't exist, but are used by an existing saved search.", array('%id' => $this->settings_id)));
    }
    return $this->settings;
  }

  /**
   * @return SearchApiIndex
   *   The index this saved search uses.
   *
   * @throws SearchApiException
   *   If the index doesn't exist.
   */
  public function index() {
    if (!isset($this->index)) {
      $this->index = search_api_index_load($this->settings()->index_id);
    }
    if (!$this->index) {
      throw new SearchApiException(t("The index with the ID %id doesn't exist, but has saved search settings attached.", array('%id' => $this->settings()->index_id)));
    }
    return $this->index;
  }

}
