// Called when the user clicks on the browser action.
chrome.browserAction.onClicked.addListener(function(tab) {
  console.log ("Requesting a wake lock");
  chrome.power.requestKeepAwake ("system");
  chrome.power.requestKeepAwake ("display");
  var url = "http://bibledit.org:8080";
  chrome.tabs.create ({ url: url });
});
