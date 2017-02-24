$ (document).ready (function ()
{
  var spans = $ ("span");
  spans.each (function () {
    var classname = $ (this).attr ("class");
    var index = classname.indexOf ("0");
    if (index > 0) {
      console.log (classname);
      var name = classname.substring (2);
      var bits = name.split ("0");
      var font_style = null;
      var font_variant = null;
      for (i = 0; i < bits.length; i++) {
        name = bits [i];
        console.log (name);
        var element = document.createElement("span");
        element.className = name;
        document.body.appendChild (element);
        var properties = window.getComputedStyle (element, null);
        var value = properties.getPropertyValue ("font-style");
        console.log (value);
        if (value != "normal") font_style = value;
        value = properties.getPropertyValue ("font-variant");
        console.log (value);
        if (value != "normal") font_variant = value;
      }
      if (font_style) $.stylesheet ("." + classname, "font-style", font_style);
      if (font_variant) $.stylesheet ("." + classname, "font-variant", font_variant);
    }
  });
});
