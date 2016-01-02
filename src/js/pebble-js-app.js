Pebble.addEventListener("ready",
  function(e) {
    console.log("PebbleKit JS ready!");
  }
);

Pebble.addEventListener("showConfiguration",
  function(e) {
    //Load the remote config page
    events = window.localStorage.getItem('events') ? "?events="+window.localStorage.getItem('events') : "?events=FIJASAGJAAXLGISAXAJAXAXLJAAAIABAIAJABAMANABAUAVAHGSAUAKEWAXACDSGTGCFSGTG";

    console.log('Show config ' + events);

    Pebble.openURL("https://dl.dropboxusercontent.com/u/72012857/settings.html"+events);
  }
);

Pebble.addEventListener("webviewclosed",
  function(e) {
    //Get JSON dictionary
    var configuration = JSON.parse(decodeURIComponent(e.response));
    console.log("Configuration window returned: " + configuration.events);

    window.localStorage.setItem('events', configuration.events);

    var transactionId = Pebble.sendAppMessage( {'events': configuration.events},
      function(e) {
        console.log('Successfully delivered message with transactionId='
          + e.data.transactionId);
      },
      function(e) {
        console.log('Unable to deliver message with transactionId='
          + e.data.transactionId
          + ' Error is: ' + e.error.message);
      }
    );
  }
);
