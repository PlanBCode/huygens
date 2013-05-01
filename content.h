/*#define NUM_START_OBJECTS 4
#define NUM_ROUTES 6
#define MAX_ROUTE_STEPS 6
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
#define MAX_ROUTE_STEPS 7
#define NUM_OBJECTS 24

enum objects {
  gps,
  atoomklok,
  kwarts,
  grasshopper,
  greenwich,
  theodoliet,
  marschannels,
  seti,
  drake,
  ureymiller,
  panspermia,
  micromegas,
  voyager,
  marswagen,
  telescoop,
  beamer,
  film,
  zoetrope,
  toverlantaarn,
  
  huygensoculair,
  huygensoculair2,
  onrust,
  kosmo,
  waterpas,
};

String objectNames[] = {
  "gps / tomtom",
  "atoomklok",
  "kwarts / wekker",
  "grasshopper / escapement",
  "greenwich / mediaan",
  "theodoliet",
  "marschannels",
  "seti",
  "drake / formule",
  "ureymiller",
  "panspermia / meteoriet",
  "micromegas",
  "voyager",
  "marswagen",
  "telescoop",
  "beamer",
  "film",
  "zoetrope",
  "toverlantaarn",
  
  "huygensoculair",
  "huygensoculair2",
  "onrust",
  "kosmo",
  "waterpas",
};

//int routeStartObjects[] = {huygensoculair,onrust,kosmo,waterpas}; 


// led pins along routes to light objects (pins 30 - 53) 
// add ALL leds pins to make sure they are pulled down when off!
int objectLedsPins[]  = { 
  52,50,48,46,44,42,40,38,36,34,32,30,53,51,49,47,45,43,41,
  37, 37, 35, 33, 31, 
  39
}; 
// positions of objects along routes. from 0 (bottom) to 1 (top)
float objectPositions[] = { 
  1.00, // gps / tom tom
  0.84, // atoomklok
  0.76, // kwarts / wekker
  0.26, //0.18, grasshopper / escapement 
  0.70, //0.64, 
  0.40, //0.36, // theoliet
  0.65, //0.69, // marschannels / mars globe
  1.00, // seti
  0.85, // drake / formule
  0.80, //0.83, ureymiller
  0.40, //0.48 (meteoriet)
  0.26, // micromegas
  0.90, // voyager
  1.00, // marswagen
  0.51, // telescoop
  1.00, // beamer
  0.91, // film rol
  0.57, //0.49, // zoetrope
  0.36, //0.40, // toverlantaarn
  
  0.00,
  0.00,
  0.00,
  0.00,
  0.00,
}; 
// audio tracks per object along routes
String objectTracks[]  = { 
  "001.mp3",
  "002.mp3",
  "003.mp3",
  "004.mp3",
  "006.mp3",
  "007.mp3",
  "017.mp3",
  "010.mp3",
  "011.mp3",
  "012.mp3",
  "013.mp3",
  "014.mp3",
  "016.mp3",
  "009.mp3",
  "018.mp3",
  "020.mp3",
  "021.mp3",
  "022.mp3",
  "023.mp3",
  
  "019.mp3",
  "024.mp3",
  "005.mp3",
  "015.mp3",
  "008.mp3",
}; 
// actual routes. from start objects (bottom) to top
// each line a route of a fixed number of objects (MAX_ROUTE_STEPS), use -1 for emtpty
int routes[] = {
  onrust,          grasshopper,   kwarts,     atoomklok,   gps,      -1,           -1, //
  waterpas,        theodoliet,    greenwich,  gps,         -1,       -1,           -1, //
  kosmo,           micromegas,    panspermia, ureymiller,  drake,    marswagen,    seti, //
  kosmo,           micromegas,    panspermia, marschannels,voyager,  marswagen,    -1, //
  huygensoculair,  telescoop,     voyager,    marswagen,   -1,       -1,           -1, //
  huygensoculair2, toverlantaarn, zoetrope,   film,        beamer,   -1,           -1  //
};
// start objects per route (to link start object in closet to routes)
int startObjectsPerRoute[] = {
  onrust,
  waterpas,
  kosmo,
  kosmo,
  huygensoculair,
  huygensoculair,
};
// start objects in the closet, from left to right
int routeStartObjects[] = {huygensoculair,onrust,kosmo,waterpas}; 

int currentStartObject = huygensoculair;
