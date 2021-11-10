char *confFile="BIKE.CFG";

/** Ändere die ID's der Sensoren hier
 * Sensor IDs können über die API abgefragt werden 
 * https://api.opensensemap.org/boxes/[:senseBoxID]
 */

// senseBox ID
// example : const char SENSEBOX_ID[] = "615f292bc031ff001b117192";
char *SENSEBOX_ID = "undef";

// Sensor ID's
// example : char tempID[] = "615f292bc031ff001b11719b";
char *tempID = "undef";
char *humiID = "undef";
char *distLeftID = "undef";
char *distRightID = "undef";
char *pm10ID = "undef";
char *pm25ID = "undef";
char *accXID = "undef";
char *accYID = "undef";
char *accZID = "undef";
char *speedID = "undef";
