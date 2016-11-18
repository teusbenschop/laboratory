$(document).ready(function() {
  $("body").mousemove(function(e){
    $(".splash").css('background-position',(e.pageX-25)+'px '+(e.pageY-25)+'px');
    $(".splash2").css('left',(e.pageX-60)+'px');
    $(".splash2").css('top',(e.pageY-65)+'px');
  });
});
