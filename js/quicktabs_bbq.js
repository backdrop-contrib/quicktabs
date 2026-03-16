/**
 * @file
 * Implements history using the BBQ plugin.
 * See https://github.com/cowboy/jquery-bbq
 */
(function($) {

Backdrop.quicktabsBbq = function($tabset, clickSelector, changeSelector, clickHandler) {

  changeSelector = changeSelector || clickSelector;

  // Find all clickable elements.
  // Supports both direct selector string and jQuery object used by quicktabs
  // renderer to handle both 'li a' and 'li button' possible markup.
  var $clickElements = typeof clickSelector === 'string' ? $(clickSelector, $tabset) : $tabset.find(clickSelector);

  // Define our own click handler for the tabs, overriding the default.
  $clickElements.each(function(i, el){
    this.tabIndex = i;
    $(this).click(function(e){
      e.preventDefault();
      var state = {},
        id = $tabset.attr('id'), // qt container id
        idx = this.tabIndex; // tab index

      state[id] = idx;
      $.bbq.pushState(state);
    });
  });

  $(window).bind('hashchange', function(e) {
    $tabset.each(function() {
      var idx = $.bbq.getState(this.id, true);

      // Find the active element using the changeSelector
      var $active_element;
      if (typeof changeSelector === 'string') {
        $active_element = $(this).find(changeSelector).eq(idx);
      } else {
        $active_element = $clickElements.eq(idx);
      }

      $active_element.triggerHandler('change');

      // If a custom click handler is provided, call it.
      // Quicktabs renderer provides a custom click handler to handle history
      // because it doesn't rely on jQuery UI's built-in behavior.
      if (clickHandler && typeof clickHandler === 'function') {
        clickHandler($active_element, idx);
      }
    });
  });

  $(window).trigger('hashchange');
}

})(jQuery);
