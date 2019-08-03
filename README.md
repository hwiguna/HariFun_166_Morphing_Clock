# HariFun\_166\_Morphing\_Clock modified by SnowHead

Read what this code primary was all about on [Instructable](https://www.instructables.com/id/Morphing-Digital-Clock/).

**CHANGES BY SNOWHEAD**

- Changed from NTP to HTTP time request (via ESP82666HTTPClient library)
- No further mess around with timezones and DST settings. The default timezone "ip" (case sensitive!) will force the server to determine your location and DST state by analyzing your public IP. Only if you're connected via a foreign proxy to the internet you will have to set your local timezone regarding the list on [WTA](http://worldtimeapi.org/api/timezones). DST settings will always be detected automatically.
- Digits will not further morphed digit by digit but all in parallel.
- Easier selection of digit color.
- Patch for weak ESP8266-12E included


Click photo for a quick demo.

original: [![Morphing Clock](https://img.youtube.com/vi/i0M6F4wRxGc/0.jpg)](https://www.youtube.com/watch?v=i0M6F4wRxGc)

modified: [![Morphing Clock](https://img.youtube.com/vi/GLg5dzmM7W4/0.jpg)](https://youtu.be/GLg5dzmM7W4)

