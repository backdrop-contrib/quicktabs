/**
 * @file
 * Preserves URL hash for forms and pager links within quicktabs.
 */
(function($) {

Backdrop.behaviors.quicktabsPreserveHash = {
  attach: function(context, settings) {
    // Update links and forms to include current hash.
    function updateHashInElements() {
      var hash = window.location.hash;

      if (!hash) {
        return;
      }

      // Handle exposed forms within quicktabs.
      $('.quicktabs_main form, .quicktabs-tabpage form, .quick-accordion form, .quicktabs-ui-wrapper form', context).each(function() {
        var $form = $(this);
        // Always use current hash on submit.
        $form.off('submit.quicktabs-hash').on('submit.quicktabs-hash', function() {
          var currentHash = window.location.hash;
          if (currentHash) {
            var action = $(this).attr('action');
            if (action && action.indexOf('#') === -1) {
              $(this).attr('action', action + currentHash);
            }
          }
        });
      });

      // Handle pager and sort/filter links within quicktabs containers.
      var $links = $('.quicktabs_main .pager a, .quicktabs-tabpage .pager a, .quick-accordion .pager a, .quicktabs-ui-wrapper .pager a, .quicktabs_main .views-table th.views-field a, .quicktabs-tabpage .views-table th.views-field a, .quick-accordion .views-table th.views-field a, .quicktabs-ui-wrapper .views-table th.views-field a', context);
      $links.each(function() {
        var $link = $(this);
        var href = $link.attr('href');
        if (href) {
          // Remove any existing hash first.
          href = href.split('#')[0];
          // Add current hash.
          $link.attr('href', href + hash);
        }
      });
    }

    // Run on initial attach.
    updateHashInElements();

    // Re-run whenever hash changes (when tabs are switched)
    $(window).off('hashchange.quicktabs-preserve').on('hashchange.quicktabs-preserve', function() {
      updateHashInElements();
    });
  }
};

})(jQuery);
