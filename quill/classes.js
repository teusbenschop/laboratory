$ (document).ready (function ()
{
  $ ("#selector").on ("change", function () {
    handleSelection (this);
  });
});

var BackgroundClass = Quill.import('attributors/class/background');
Quill.register(BackgroundClass, true);

var ColorClass = Quill.import('attributors/class/color');
Quill.register(ColorClass, true);

var SizeStyle = Quill.import('attributors/style/size');
Quill.register(SizeStyle, true);

var quill = new Quill('#editor-container', {
  modules: {
    toolbar: '#toolbar-container'
  },
  placeholder: 'The chapter is empty...',
  theme: 'snow'
});

function handleSelection (selector)
{
  console.log (selector.value);
  var range = quill.getSelection();
  if (range) {
    if (range.length == 0) {
      console.log('User cursor is at index', range.index);
    } else {
      var text = quill.getText(range.index, range.length);
      console.log('User has highlighted: ', text);
      var format = quill.getFormat (range);
      console.log (format);
    }
  } else {
    console.log('User cursor is not in editor');
  }
  quill.format ("background", selector.value);
  
  quill.formatLine(40, 10, 'align', 'center');
}
