$ (document).ready (function ()
{
  $ ("#selector").on ("change", function () {
    handleSelection (this);
  });
});

let Parchment = Quill.import('parchment');

let ParagraphClass = new Parchment.Attributor.Class ('paragraph', 'b', { scope: Parchment.Scope.BLOCK });
Quill.register (ParagraphClass, true);

let CharacterClass = new Parchment.Attributor.Class ('character', 'i', { scope: Parchment.Scope.INLINE });
Quill.register (CharacterClass, true);

var quill = new Quill('#editor-container', {});
quill.focus ();

function handleSelection (selector)
{
  quill.format ("character", selector.value);
}
