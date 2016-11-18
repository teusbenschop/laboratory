$(document).ready(function() {
		$("body").mousemove(function(e){
      $(".splash").css('background-position',(e.pageX-25)+'px '+(e.pageY-25)+'px');
    });
});
