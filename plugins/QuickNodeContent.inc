<?php

/**
 * Class for tab content of type "node" - this is for rendering a node as tab
 * content.
 */
class QuickNodeContent extends QuickContent {

  public static function getType() {
    return 'node';
  }

  public function optionsForm($delta, $qt) {
    $tab = $this->settings;
    $form = array();

    $nid_default = '';
    if (!empty($tab['nid'])) {
      $title = db_query("SELECT title FROM {node} WHERE nid = :nid", array(':nid' => $tab['nid']))->fetchField();
      $nid_default = $title . ' [' . $tab['nid'] . ']';
    }

    $form['node']['nid'] = array(
      '#type' => 'textfield',
      '#title' => t('Node'),
      '#description' => t('The node ID of the node. Start typing to see a list of suggestions.'),
      '#placeholder' => ' ',
      '#autocomplete_path' => 'node/autocomplete',
      '#maxlength' => 512,
      '#size' => 50,
      '#default_value' => $nid_default,
      '#element_validate' => array(array($this, 'elementValidateNid')),
    );
    $entity_info = entity_get_info('node');
    $view_modes = array();
    foreach ($entity_info['view modes'] as $view_mode_name => $view_mode) {
      $view_modes[$view_mode_name] = $view_mode['label'];
    }
    $form['node']['view_mode'] = array(
      '#type' => 'select',
      '#title' => t('View mode'),
      '#options' => $view_modes,
      '#default_value' => isset($tab['view_mode']) ? $tab['view_mode'] : 'full',
    );
    $form['node']['hide_title'] = array(
      '#type' => 'checkbox',
      '#title' => t('Hide the title of this node'),
      '#default_value' => isset($tab['hide_title']) ? $tab['hide_title'] : 1,
    );
    if (module_exists('translation')) {
      $form['node']['translate'] = array(
        '#type' => 'checkbox',
        '#default_value' => !empty($tab['translate']),
        '#title' => t('Load translated version of the content if available'),
      );
    }

    return $form;
  }

  /**
   * Callback for Nid element validation.
   */
  public function elementValidateNid($element, &$form_state, $form) {
    $delta = $element['#parents'][1];
    if ($form_state['values']['tabs'][$delta]['type'] != 'node') {
      return;
    }

    module_load_include('inc', 'node', 'node.pages');
    if (empty($element['#value'])) {
      form_error($element, t('A node is required.'));
    }
    if (strpos($element['#value'], '%') === 0) {
      form_error($element, t('"%" may not be used for the first segment of a path.'));
    }
    if ($nid = node_autocomplete_validate($element['#value'])) {
      $form_state['values']['tabs'][$delta]['node']['nid'] = $nid;
    }
    else {
      form_error($element, t('Sorry, no matching content was found for <em>%nid</em>.', array('%nid' => $element['#value'])));
    }
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
      list($item['nid'], $item['view_mode'], $item['hide_title']) = $args;
    }
    $output = array();
    if (isset($item['nid'])) {
      $node = node_load($item['nid']);
      if (!empty($node)) {
        if (node_access('view', $node)) {
          $buildmode = $item['view_mode'];
          // Look for and load translated node if translations are available.
          if (module_exists('translation') && !empty($item['translate'])) {
            $node = $this->loadTranslatedNode();
          } else {
            $node = node_load($item['nid']);
          }
          $nstruct = node_view($node, $buildmode);
          if ($item['hide_title']) {
            $nstruct['#node']->title = '';
          }
          $output = $nstruct;
        }
        elseif (!$hide_empty) {
          $output = array('#markup' => theme('quicktabs_tab_access_denied', array('tab' => $item)));
        }
      }
    }
    return $output;
  }

  public function getAjaxKeys() {
    return array('nid', 'view_mode', 'hide_title');
  }

  public function getUniqueKeys() {
    return array('nid');
  }

  /**
   * Load translated node
   *
   * @return Node
   *   The Node object.
   */
  protected function loadTranslatedNode() {
    global $language;

    $node = node_load($this->settings['nid']);

    if (!empty($node->tnid) && !empty($node->langcode)) {
      $translations = translation_node_get_translations($node->tnid);

      foreach ($translations as $code => $translation) {
        if ($code == $language->langcode) {
          return node_load($translation->nid);
        }
      }
    }

    return $node;
  }
}
