<?php

/**
 * Class for tab content of type "callback" - this is for rendering the contents
 * of some menu callback function as tab content.
 */
class QuickCallbackContent extends QuickContent {

  public static function getType() {
    return 'callback';
  }

  public function __construct($item) {
    parent::__construct($item);

    if (isset($item['path'])) {
      $url_args = arg();
      $path = $item['path'];

      foreach ($url_args as $id => $arg) {
        $path = str_replace("%$id", $arg, $path);
      }
      $path = preg_replace(',/?(%\d),', '', $path);
      if (!empty($path)) {
        $this->settings['ajax_path'] = rawurlencode($path);
      }
      else {
        $this->settings['ajax_path'] = '';
      }
      $this->settings['actual_path'] = $path;
    }
  }

  public function optionsForm($delta, $qt) {
    $tab = $this->settings;
    $form = array();
    $form['callback']['path'] = array(
      '#type' => 'textfield',
      '#default_value' => isset($tab['path']) ? $tab['path'] : '',
      '#title' => t('Path'),
      '#element_validate' => array(array($this, 'elementValidatePath')),
    );
    $form['callback']['use_title'] = array(
      '#type' => 'checkbox',
      '#return_value'=>TRUE,
      '#title' => 'Use callback title',
      '#default_value' => isset($tab['use_title']) ? $tab['use_title'] : FALSE,
      '#description' => t('Should quicktabs use the rendered title of the callback?'),
    );
    return $form;
  }

  /**
   * Callback for path element validation.
   */
  public function elementValidatePath($element, &$form_state, $form) {
    $delta = $element['#parents'][1];
    if ($form_state['values']['tabs'][$delta]['type'] != 'callback') {
      return;
    }

    if (empty($element['#value']) || url_is_external($element['#value'])) {
      form_error($element, t('You must specify a valid path.'));
    }
    if (strpos($element['#value'], '%') === 0) {
      form_error($element, t('"%" may not be used for the first segment of a path.'));
    }
    // automatically remove '/' from path.
    $form_state['values']['tabs'][$delta]['callback']['path'] = trim($form_state['values']['tabs'][$delta]['callback']['path'], '/');
  }

  public function render($hide_empty = FALSE, $args = array()) {
    if ($this->rendered_content) {
      return $this->rendered_content;
    }
    $item = $this->settings;
    if (!empty($args)) {
      // The args have been passed in from an ajax request.
      // The first element of the args array is the qt_name, which we don't need
      // for this content type.
      array_shift($args);
      $item['actual_path'] = rawurldecode($args[0]);
      $_GET['q'] = $item['actual_path'];
    }

    $output = array();
    if (isset($item['actual_path'])) {
      // Retain the current page title as we'll need to set it back after
      // calling menu_execute_active_handler().
      $page_title = backdrop_get_title();
      $response = menu_execute_active_handler($item['actual_path'], FALSE);
      // Revert the page title.
      if(isset($this->settings['use_title']) && $this->settings['use_title'] > 0){
        $this->title = backdrop_get_title();
      }
      backdrop_set_title($page_title);

      if (!is_array($response)) {
        if (is_int($response)) {
          if (MENU_ACCESS_DENIED == $response && !$hide_empty) {
            $output['#markup'] = theme('quicktabs_tab_access_denied', array('tab' => $item));
          }
          // For any other integer response form the menu callback, we'll just
          // return an empty array.
        }
        else {
          $output = array('#markup' => $response);
        }
      }
      else {
        $output = $response;
      }
    }
    $this->rendered_content = $output;
    return $output;
  }

  public function getAjaxKeys() {
    return array('ajax_path');
  }

  public function getUniqueKeys() {
    return array('ajax_path');
  }
}
