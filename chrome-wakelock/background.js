// Called when the user clicks on the browser action.
chrome.browserAction.onClicked.addListener(function(tab) {
  console.log ("Requesting a wake lock");
  chrome.tabs.executeScript({
    code: 'chrome.power.requestKeepAwake ("display")'
  });
  chrome.power.requestKeepAwake ("system");
  chrome.power.requestKeepAwake ("display");
});
