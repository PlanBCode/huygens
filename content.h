/*#define NUM_START_OBJECTS 4
#define NUM_ROUTES 6
#define MAX_ROUTE_STEPS 7
#define NUM_OBJECTS 5
enum objects { a, b, c, d, e };
// led pins along routes to light objects (pins 30 - 53) 
// add ALL leds pins to make sure they are pulled down when off!
int objectLedsPins[] = { 
  //52,50,48,46,44,42,40,38,36,34,32,30,53,51,49,47,45,43,41,39
  38,36,34,32,30,53,51,49,47,45,43,41,39,  52,50,48,46,44,42,40,
}; 
float objectPositions[] = { 
  0.2,  0.3,  0.5,  0.75,  1
}; 
String objectTracks[]  = { 
  "1.MP3",  "2.MP3",  "3.MP3",  "4.MP3",  "5.MP3", "5.MP3"
}; // tracks per object along routes
int routes[] = {
  a, b, c,  d,  e, -1,
  a, c, e, -1, -1, -1,
  b, c, d, -1, -1, -1,
  b, d, e, -1, -1, -1,
  c, d, e,  a, -1, -1,
  d, e, a, -1, -1, -1
};
int routeStartObjects[] = {a,b,c,d}; // objects in closed that people select
int currentStartObject   = a;
*/

#define NUM_START_OBJECTS 4
#define NUM_ROUTES 6
#define MAX_ROUTE_STEPS 6
#define NUM_OBJECTS 23

enum objects {
  gps,
  atoomklok,
  kwarts,
  grasshopper,
  onrust,
  greenwich,
  theodoliet,
  waterpas,
  marschannels,
  seti,
  drake,
  ureymiller,
  panspermia,
  micromegas,
  kosmo,
  voyager,
  marswagen,
  telescoop,
  huygensoculair,
  beamer,
  film,
  zoetrope,
  toverlantaarn,
};
// led pins along routes to light objects (pins 30 - 53) 
// add ALL leds pins to make sure they are pulled down when off!
int objectLedsPins[]  = { 
  52,50,48,46,44,42,40,38,36,34,32,30,53,51,49,47,45,43,41,39
}; 
// positions of objects along routes. from 0 (bottom) to 1 (top)
float objectPositions[] = { 
  1.00,
  0.84,
  0.76,
  0.18,
  0.00,
  0.64,
  0.36,
  0.00,
  0.69,
  1.00,
  0.85,
  0.83,
  0.48,
  0.26,
  0.00,
  0.90,
  1.00,
  0.96,
  0.00,
  1.00,
  0.91,
  0.49,
  0.40,
}; 
// audio tracks per object along routes
String objectTracks[]  = { 
  "01gps.mp3",
  "02atoomklok.mp3",
  "03kwarts.mp3",
  "04grasshopper.mp3",
  "05onrust.mp3",
  "06greenwich.mp3",
  "07theodoliet.mp3",
  "08waterpas.mp3",
  "09marschannels.mp3",
  "10seti.mp3",
  "11drake.mp3",
  "12ureymiller.mp3",
  "13panspermia.mp3",
  "14micromegas.mp3",
  "15kosmo.mp3",
  "16voyager.mp3",
  "17marswagen.mp3",
  "18telescoop.mp3",
  "19huygensoculair.mp3",
  "20beamer.mp3",
  "21film.mp3",
  "22zoetrope.mp3",
  "23toverlantaarn.mp3",
}; 
// actual routes. from start objects (bottom) to top
// each line a route of a fixed number of objects (MAX_ROUTE_STEPS), use -1 for emtpty
int routes[] = {
  onrust,          grasshopper,   kwarts,     atoomklok,   gps,      -1,           -1,
  waterpas,        theodoliet,    greenwich,  gps,         -1,       -1,           -1, 
  kosmo,           micromegas,    panspermia, ureymiller,  drake,    seti,         marschannels,
  kosmo,           micromegas,    panspermia, marswagen,   voyager,  marschannels, -1,
  huygensoculair,  telescoop,     marswagen,  voyager,     -1,       -1,           -1,
  huygensoculair,  toverlantaarn, zoetrope,   film,        beamer,   -1,           -1
};
// start objects in the closet, from left to right
int routeStartObjects[] = {huygensoculair,onrust,kosmo,waterpas}; 

int currentStartObject = huygensoculair;
