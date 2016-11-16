// Register inline class.
var BackgroundClass = Quill.import('attributors/class/background');
delete BackgroundClass.whitelist;
Quill.register(BackgroundClass, true);

// Register block class.
var AlignClass = Quill.import('attributors/class/align');
delete AlignClass.whitelist;
Quill.register(AlignClass, true);

// Instantiate editor.
quill = new Quill ('#editor', { });

if (!quill.hasFocus ()) quill.focus ();

// Remove generated containers.
$ ("#toolbar-container").remove ();
$ (".ql-tooltip").remove ();
$ (".ql-clipboard").hide ();
