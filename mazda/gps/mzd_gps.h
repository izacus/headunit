#ifndef MZD_GPS_H
#define MZD_GPS_H

#include "hu_sensors.h"

#define GPS_POSITION_AVAILABLE 1
#define GPS_POSITION_NOT_AVAILABLE 0

/** Starts GPS data collection thread. It'll read the serial port and store last location. **/
void mzd_gps_start();
/** Get last retrieved location. **/
int mzd_gps_get_location(hu_location_t* location);
/** Stop the GPS collection thread. **/
void mzd_gps_stop();

#endif