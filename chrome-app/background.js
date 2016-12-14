chrome.app.runtime.onLaunched.addListener(function() {
  chrome.app.window.create('index.html', { id: 'main' });
  //console.log ("Requesting a wake lock");
  //chrome.power.requestKeepAwake ("system");
  //chrome.power.requestKeepAwake ("display");
  //var url = "http://bibledit.org:8080";
  //chrome.tabs.create ({ url: url });
});


