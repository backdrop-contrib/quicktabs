(function ($) {
Backdrop.settings.views = Backdrop.settings.views || {'ajax_path': '/views/ajax'};

Backdrop.quicktabs = Backdrop.quicktabs || {};

Backdrop.quicktabs.getQTName = function (el) {
  return el.id.substring(el.id.indexOf('-') +1);
}

Backdrop.behaviors.quicktabs = {
  attach: function (context, settings) {
    $.extend(true, Backdrop.settings, settings);
    $('.quicktabs-wrapper', context).once(function(){
      Backdrop.quicktabs.prepare(this);
    });
  }
}

// Setting up the inital behaviours
Backdrop.quicktabs.prepare = function(el) {
  // el.id format: "quicktabs-$name"
  var qt_name = Backdrop.quicktabs.getQTName(el);
  var $ul = $(el).find('ul.quicktabs-tabs:first');

  $("ul.quicktabs-tabs li a span#active-quicktabs-tab").remove();

  $ul.find('li a').each(function(i, element){
    element.myTabIndex = i;
    element.qt_name = qt_name;

    var tab = new Backdrop.quicktabs.tab(element);
    var parent_li = $(element).parents('li').get(0);
    if ($(parent_li).hasClass('active')) {
      $(element).addClass('quicktabs-loaded');
      $(element).append('<span id="active-quicktabs-tab" class="element-invisible">' + Backdrop.t('(active tab)') + '</span>');
    }
    $(element).once(function() {$(this).bind('click', {tab: tab}, Backdrop.quicktabs.clickHandler);});
  });
}

Backdrop.quicktabs.clickHandler = function(event) {
  var tab = event.data.tab;
  var element = this;
  // Set clicked tab to active.
  $(this).parents('li').siblings().removeClass('active');
  $(this).parents('li').addClass('active');

  // Set clicked tab to aria-selected.
  $(this).parents('li').siblings().attr('aria-selected', 'false');
  $(this).parents('li').attr('aria-selected', 'true');

  $("ul.quicktabs-tabs li a span#active-quicktabs-tab").remove();
  $(this).append('<span id="active-quicktabs-tab" class="element-invisible">' + Backdrop.t('(active tab)') + '</span>');

  // Hide all tabpages.
  tab.container.children().addClass('quicktabs-hide');

  if (!tab.tabpage.hasClass("quicktabs-tabpage")) {
    tab = new Backdrop.quicktabs.tab(element);
  }

  tab.tabpage.removeClass('quicktabs-hide');
  $(element).trigger('switchtab');
  return false;
}

// Constructor for an individual tab
Backdrop.quicktabs.tab = function (el) {
  this.element = el;
  this.tabIndex = el.myTabIndex;
  var qtKey = 'qt_' + el.qt_name;
  var i = 0;
  for (var key in Backdrop.settings.quicktabs[qtKey].tabs) {
    if (i == this.tabIndex) {
      this.tabObj = Backdrop.settings.quicktabs[qtKey].tabs[key];
      this.tabKey = key;
    }
    i++;
  }
  this.tabpage_id = 'quicktabs-tabpage-' + el.qt_name + '-' + this.tabKey;
  this.container = $('#quicktabs-container-' + el.qt_name);
  this.tabpage = this.container.find('#' + this.tabpage_id);
}

if (Backdrop.ajax) {
  /**
   * Handle an event that triggers an AJAX response.
   *
   * We unfortunately need to override this function, which originally comes from
   * misc/ajax.js, in order to be able to cache loaded tabs, i.e. once a tab
   * content has loaded it should not need to be loaded again.
   *
   * I have removed all comments that were in the original core function, so that
   * the only comments inside this function relate to the Quicktabs modification
   * of it.
   */
  Backdrop.ajax.prototype.eventResponse = function (element, event) {
    var ajax = this;

    if (ajax.ajaxing) {
      return false;
    }

    try {
      if (ajax.form) {
        if (ajax.setClick) {
          element.form.clk = element;
        }

        ajax.form.ajaxSubmit(ajax.options);
      }
      else {
        // Do not perform an ajax request for already loaded Quicktabs content.
        if (!$(element).hasClass('quicktabs-loaded')) {
          ajax.beforeSerialize(ajax.element, ajax.options);
          $.ajax(ajax.options);
          if ($(element).parents('ul').hasClass('quicktabs-tabs')) {
            $(element).addClass('quicktabs-loaded');
          }
        }
      }
    }
    catch (e) {
      ajax.ajaxing = false;
      alert("An error occurred while attempting to process " + ajax.options.url + ": " + e.message);
    }
    return false;
  };
}


})(jQuery);
