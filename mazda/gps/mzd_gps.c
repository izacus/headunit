#include "mzd_gps.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "minmea.h"

#define LOGTAG "mazda-gps"

#include "hu_uti.h"

static hu_location_t last_known_position = { 0 };
static pthread_mutex_t pos_mutex;
static bool running = false;

void* process_gps() {    
    FILE* fp;
    char* gps_line = NULL;
    size_t len = 0;
    ssize_t read;

    // GPS hardware is attached to ttymxc2 on CMU and communicates
    // via text NMEA protocol.
    fp = fopen("/dev/ttymxc2", "r");
    if (fp == NULL) return NULL;

    // We get commands in batches so we need to watch to properly group them. 
    // We'll need RMC, GGA packets - RMC carries most data, GGA carries altitude. They should be sent 
    // as part of the same batch. 
    bool got_rmc = false;
    bool got_gga = false;

    hu_location_t pos;

    while (running && ((read = getline(&gps_line, &len, fp)) != -1)) {
        switch(minmea_sentence_id(gps_line, false)) {
            case MINMEA_SENTENCE_RMC: {
                // Maybe we didn't get GGA before, dispatch the packet and continue.
                if (got_rmc) {
                    if (!got_gga) pos.altitude_valid = false;
                    pthread_mutex_lock(&pos_mutex);
                    last_known_position = pos;
                    pthread_mutex_unlock(&pos_mutex);
                    memset(&pos, 0, sizeof(pos));
                    logd("Had to send partial location due to missing GGA!");
                }

                struct minmea_sentence_rmc frame;
                if (minmea_parse_rmc(&frame, gps_line)) {
                    // Check for zero values and skip them
                    if (frame.latitude.value == 0 || frame.longitude.value == 0) continue;

                    struct timespec ts;
                    minmea_gettime(&ts, &frame.date, &frame.time);
                    pos.timestamp = (ts.tv_sec * 1000L) + (ts.tv_nsec / 1000000L);

                    logd("RMC [TS: %ld Lat: %f Lng: %f Spd: %f Cour: %f]",
                        pos.timestamp, minmea_tofloat(frame.latitude), minmea_tofloat(frame.longitude), minmea_tofloat(frame.speed) * 1.852f, minmea_tofloat(frame.course));

                    pos.latitude = (int32_t)(minmea_tocoord(&frame.latitude) * 1E7);
                    pos.longitude = (int32_t)(minmea_tocoord(&frame.longitude) *1E7);
                    pos.bearing = (int32_t)(minmea_rescale(&frame.course, 1E6));
                    pos.speed = minmea_rescale(&frame.speed, 1E3);
                    pos.speed = ((double)pos.speed * 1.852);   // knots to kmh
                    got_rmc = true;
                }

            } break;
            case MINMEA_SENTENCE_GGA: {
                struct minmea_sentence_gga frame;
                if (minmea_parse_gga(&frame, gps_line)) {
                    if (frame.altitude_units == 'M') {
                        logd("GGA: [Alt: %f]", minmea_tofloat(frame.altitude));

                        pos.altitude_valid = true;
                        pos.altitude = minmea_rescale(&frame.altitude, 1E2);
                    }
                    got_gga = true;
                }
            } break;
        }

        if (got_gga && got_rmc) {
            pthread_mutex_lock(&pos_mutex);
            last_known_position = pos;
            pthread_mutex_unlock(&pos_mutex);

            memset(&pos, 0, sizeof(pos));
            got_gga = false;
            got_rmc = false;
        }
    }

    fclose(fp);
}

int mzd_gps_get_location(hu_location_t* location) {
    int result;
    pthread_mutex_lock(&pos_mutex);
    if (last_known_position.timestamp > 0) {
        *location = last_known_position;
        result = GPS_POSITION_AVAILABLE;
    } else {
        result = GPS_POSITION_NOT_AVAILABLE;
    }    
    pthread_mutex_unlock(&pos_mutex);
    return result;    
}


void mzd_gps_start() {
    running = true;
    pthread_t gps_thread;
    pthread_create(&gps_thread, NULL, process_gps, NULL);
}

void mzd_gps_stop() {
    running = false;
}