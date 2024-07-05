<?php

/**
 * Renders the content using the jQuery UI Tabs widget.
 */
class QuickUiTabs extends QuickRenderer {

  public static function optionsForm($qt) {
    $form = array();
    $form['history'] = array(
      '#type' => 'checkbox',
      '#title' => 'History',
      '#description' => t('Store tab state in the URL allowing for browser back / forward and bookmarks.'),
      '#default_value' => (isset($qt->renderer) && $qt->renderer == 'ui_tabs' && isset($qt->options['history']) && $qt->options['history']),
    );
    return $form;
  }

  public function render() {
    $quickset = $this->quickset;

    $active_tab = $quickset->getActiveTab();
    $tabs = $this->build_tablinks($active_tab);
    $qt_name = $quickset->getName();
    $render_array = array(
      '#attached' => $this->add_attached(),
      'content' => array(
        '#theme' => 'qt_ui_tabs',
        '#options' => array('attributes' => array(
          'id' => 'quicktabs-' . $qt_name,
          'class' => 'quicktabs-ui-wrapper',
        )),
        'tabs' => array('#theme' => 'qt_ui_tabs_tabset', '#options' => array('active' => $active_tab), 'tablinks' => $tabs),
        'divs' => array(),
      ),
    );
    foreach ($quickset->getContents() as $key => $tab) {
      if (!empty($tab)) {
        $attribs = array(
          'id' => 'qt-'. $qt_name .'-ui-tabs' . ($key+1),
        );
        $render_array['content']['divs'][] = array(
          '#prefix' => '<div '. backdrop_attributes($attribs) .'>',
          '#suffix' => '</div>',
          'content' => $tab->render(),
        );
      }
    }
    return $render_array;
  }

  /**
   * Build the actual tab links, with appropriate href, title and attributes.
   *
   * @param $active_tab The index of the active tab.
   */
  protected function build_tablinks($active_tab) {
    $tabs = array();
    $qt_name = $this->quickset->getName();
    foreach ($this->quickset->getContents() as $i => $tab) {
      if (!empty($tab)) {
        // If we use l() here or a render array of type 'link', the '#' symbol will
        // be escaped. Sad panda is sad.
        $href = '#qt-'. $qt_name .'-ui-tabs' . ($i+1);
        $quickset = $this->quickset;
        $attributes = array();
        $attributes['class'][] = backdrop_html_class($tab->getTitle());
        backdrop_alter('quicktabs_tablinks_attributes', $attributes, $quickset, $i);
        $attributes['href'] = $href;
        $tablink = array(
          '#markup' => '<a '. backdrop_attributes($attributes) .'>'. check_plain($this->quickset->translateString($tab->getTitle(), 'tab', $i)) .'</a>',
        );
        $tabs[$i] = $tablink;
      }
    }
    return $tabs;
  }

  /**
   * Add any necessary js, css and libraries for the render array.
   */
  protected function add_attached() {
    $active_tab = $this->quickset->getActiveTab();
    $settings = $this->quickset->getSettings();
    $options = $settings['options'];

    $attached = array(
      'library' => array(
        array('quicktabs', 'quicktabs.ui_tabs'),
      ),
    );

    $javascript = backdrop_add_js();
    if (isset($javascript['settings']['data'])) {
      foreach ($javascript['settings']['data'] as $key => $settings) {
        if (key($settings) == 'quicktabs') {
          $qtkey = $key;
          break;
        }
      }
    }

    if ($options['history']) {
      $attached['library'][] = array('quicktabs', 'quicktabs.bbq');
    }

    $name = $this->quickset->getName();
    if (!isset($qtkey) || !array_key_exists('qt_' . $name, $javascript['settings']['data'][$qtkey]['quicktabs'])) {
      $quicktabs_array = array('name' => $name, 'active_tab' => $this->quickset->getActiveTab(), 'history' => $options['history']);
      $attached['js'][] = array('data' => array('quicktabs' => array('qt_'. $name => $quicktabs_array)), 'type' => 'setting');
    }
    return $attached;
  }
}
