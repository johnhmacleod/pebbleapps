var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function locationSuccess(pos) {
  // Construct URL
  var url = "http://www.bom.gov.au/nsw/observations/sydney.shtml";

  // Send request to BoM
  xhrRequest(url, 'GET', 
    function(responseText) {

      // Assemble dictionary using our keys
      var wind_time = responseText.substr(responseText.search("obs-datetime obs-station-north-head")+40, 5);
      var wind_dir_long= responseText.substr(responseText.search("obs-wind obs-wind-dir obs-station-north-head") + 46 , 10);
      var wind_dir = wind_dir_long.substring(0, wind_dir_long.search("<"));
      var wind_speed_long = responseText.substr(responseText.search("obs-wind obs-wind-spd-kts obs-station-north-head") + 50, 10);
      var wind_speed = wind_speed_long.substring(0, wind_speed_long.search("<"));
      wind_speed_long = responseText.substr(responseText.search("obs-wind obs-wind-gust-kts obs-station-north-head") + 51, 10);
      wind_speed = wind_speed + "-" + wind_speed_long.substring(0, wind_speed_long.search("<"));

      // Assemble dictionary using our keys
      var wind_time1 = responseText.substr(responseText.search("obs-datetime obs-station-sydney-harbour")+44, 5);
      wind_dir_long = responseText.substr(responseText.search("obs-wind obs-wind-dir obs-station-sydney-harbour") + 50 , 10);
      var wind_dir1 = wind_dir_long.substring(0, wind_dir_long.search("<"));
      wind_speed_long = responseText.substr(responseText.search("obs-wind obs-wind-spd-kts obs-station-sydney-harbour") + 54, 10);
      var wind_speed1 = wind_speed_long.substring(0, wind_speed_long.search("<"));
      wind_speed_long = responseText.substr(responseText.search("obs-wind obs-wind-gust-kts obs-station-sydney-harbour") + 55, 10);
      wind_speed1 = wind_speed1 + "-" + wind_speed_long.substring(0, wind_speed_long.search("<"));

      // Assemble dictionary using our keys
      var wind_time2 = responseText.substr(responseText.search("obs-datetime obs-station-fort-denison")+42, 5);
      wind_dir_long= responseText.substr(responseText.search("obs-wind obs-wind-dir obs-station-fort-denison") + 48 , 10);
      var wind_dir2 = wind_dir_long.substring(0, wind_dir_long.search("<"));
      wind_speed_long = responseText.substr(responseText.search("obs-wind obs-wind-spd-kts obs-station-fort-denison") + 52, 10);
      var wind_speed2 = wind_speed_long.substring(0, wind_speed_long.search("<"));
      wind_speed_long = responseText.substr(responseText.search("obs-wind obs-wind-gust-kts obs-station-fort-denison") + 53, 10);
      wind_speed2 = wind_speed2 + "-" + wind_speed_long.substring(0, wind_speed_long.search("<"));

      
      var dictionary = {
        "KEY_WIND_SPEED": wind_speed,
        "KEY_WIND_DIR": wind_dir,
        "KEY_TIME" : wind_time,
        "KEY_WIND_SPEED1": wind_speed1,
        "KEY_WIND_DIR1": wind_dir1,
        "KEY_TIME1" : wind_time1,
        "KEY_WIND_SPEED2": wind_speed2,
        "KEY_WIND_DIR2": wind_dir2,
        "KEY_TIME2" : wind_time2
      };

      // Send to Pebble
      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log("Weather info sent to Pebble successfully!");
        },
        function(e) {
          console.log("Error sending weather info to Pebble!");
        }
      );
    }      
  );
}

function locationError(err) {
  console.log("Error requesting location!");
}

function getWeather() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log("PebbleKit JS ready!");

    // Get the initial weather
    getWeather();
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log("AppMessage received!");
    getWeather();
  }                     
);


