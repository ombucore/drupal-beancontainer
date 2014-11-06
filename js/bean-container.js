(function ($) {

$(document).on('tiles.requestSuccess', function(e, tile) {
  var tab = $('#' + tile.domNode.attr('id')).parent('.tab-pane');
  if (tab.length > 0) {
    $('a[href="#' + tab.attr('id') + '"]').tab('show');
  }
});

$('a[data-toggle="tab"]').on('shown.bs.tab', function (e) {
  Drupal.behaviors.bootsGridContextual.attach($($(e.target).attr('href')), {});
});

})(jQuery);
