<?php

/**
 * Class for tab content of type "block" - this is for rendering a block as tab
 * content.
 */
class QuickBlockContent extends QuickContent {

  public static function getType() {
    return 'block';
  }

  public function optionsForm($delta, $qt) {
    $tab = $this->settings;
    $form = array();
    $blocks = quicktabs_get_blocks();

    if ($blocks) {
      $form['block']['bid'] = array(
        '#type' => 'select',
        '#options' => $blocks,
        '#default_value' => isset($tab['bid']) ? $tab['bid'] : '',
        '#title' => t('Select a block'),
      );
      $form['block']['hide_title'] = array(
        '#type' => 'checkbox',
        '#title' => t('Hide the title of this block'),
        '#default_value' => isset($tab['hide_title']) ? $tab['hide_title'] : 1,
      );
    }
    else {
      $form['block']['bid'] = array(
        '#type' => 'markup',
        '#markup' => t('No custom blocks found. <a href="!url">Add a custom block</a> first.', array('!url' => url('admin/structure/block'))),
      );
    }

    return $form;
  }

  public function render($hide_empty = FALSE, $args = array()) {
    if ($this->rendered_content) {
      return $this->rendered_content;
    }
    $output = array();
    $item = $this->settings;

    if (!empty($args)) {
      // The args have been passed in from an ajax request.
      $qt_name = array_shift($args);
      list($item['bid'], $item['hide_title']) = $args;

      // Ensure the block is assigned to the requested quicktabs block. This test prevents
      // AJAX access to blocks that have not been added to an AJAX-enabled quicktabs block.
      $break = TRUE;
      $quicktabs = quicktabs_load($qt_name);
      // Ensure AJAX is enabled for the quicktabs block.
      if (!empty($quicktabs) && $quicktabs->ajax == 1) {
        // Ensure the requested tab has been added to the quicktabs block.
        foreach ($quicktabs->tabs as $quicktab) {
          if (isset($quicktab['bid']) && ($quicktab['bid'] == $item['bid'])) {
            $break = FALSE;
            break;
          }
        }
      }
      if ($break == TRUE) {
        if (!$hide_empty) {
          $output['#markup'] = theme('quicktabs_tab_access_denied', $item);
        }
        return $output;
      }
    }

    if (isset($item['bid'])) {
      if (module_exists('block')) {
        $pos = strpos($item['bid'], '_delta_');
        $module = backdrop_substr($item['bid'], 0, $pos);
        $delta = backdrop_substr($item['bid'], $pos + 7);
        $block_config = config('block.custom.' . $delta);

        // Make sure the user can access the block.
        if ($this->accessBlock($module, $delta)) {
          global $language;
          if (language_multilingual() && !$language->default && $block_config->get('translations')) {
            $title = (!$item['hide_title']) ? '<h3>' . $block_config->get('translations.' . $language->langcode . '.title') . '</h3>' : '';
            $text = $block_config->get('translations.' . $language->langcode . '.body.value');
            $format = $block_config->get('translations.' . $language->langcode . '.body.format');
          }
          else {
            $title = (!$item['hide_title']) ? '<h3>' . $block_config->get('title') . '</h3>' : '';
            $text = $block_config->get('body.value');
            $format = $block_config->get('body.format');
          }
          $output['#markup'] = $title . check_markup($text, $format);
        }
        elseif (!$hide_empty) {
          $output['#markup'] = theme('quicktabs_tab_access_denied', $item);
        }
      }
      elseif (!$hide_empty) {
        $output['#markup'] = t('Block module is not enabled, cannot display content.');
      }
    }
    $this->rendered_content = $output;
    return $output;
  }

  public function getAjaxKeys() {
    return array('bid', 'hide_title');
  }

  public function getUniqueKeys() {
    return array('bid');
  }

  /**
   * Checks if the current user can access a block.
   */
  private function accessBlock($module, $delta) {
    // Return whether the user can access the block.
    // @todo For now, only custom blocks display and they don't have their
    // own access control. Quicktabs would need its own access layer.
    return TRUE;
  }
}

