#include "mzd_gps.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "minmea.h"

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
    // We'll need RMC, GGA and VTG packet for all the data. They should be sent 
    // as part of the same batch. 
    bool got_rmc = false;
    bool got_gga = false;
    bool got_vtg = false;

    hu_location_t pos;

    while (running && ((read = getline(&gps_line, &len, fp)) != -1)) {
        switch(minmea_sentence_id(gps_line, true)) {
            case MINMEA_SENTENCE_RMC: {
                // Maybe we didn't get GGA before, dispatch the packet and continue.
                if (got_rmc) {
                    if (!got_gga) pos.altitude_valid = false;
                    if (!got_vtg) pos.speed = 0;
                    pthread_mutex_lock(&pos_mutex);
                    last_known_position = pos;
                    pthread_mutex_unlock(&pos_mutex);
                }

                struct minmea_sentence_rmc frame;
                if (minmea_parse_rmc(&frame, gps_line)) {
                    struct timespec ts;
                    minmea_gettime(&ts, &frame.date, &frame.time);
                    pos.timestamp = (ts.tv_sec * 1000L) + (ts.tv_nsec / 1000000L);
                    pos.latitude = (int32_t)(minmea_tocoord(&frame.latitude) * 1E7);
                    pos.longitude = (int32_t)(minmea_tocoord(&frame.longitude) *1E7);
                    pos.speed = minmea_rescale(&frame.speed, 1E3);
                    pos.speed = pos.speed * 1852;   // knots to kmh
                    pos.accuracy = 0;
                    got_rmc = true;
                }

            } break;
            case MINMEA_SENTENCE_GGA: {
                struct minmea_sentence_gga frame;
                if (minmea_parse_gga(&frame, gps_line)) {
                    if (frame.altitude_units == 'M') {
                        pos.altitude_valid = true;
                        pos.altitude = minmea_rescale(&frame.altitude, 1E2);
                    }
                    got_gga = true;
                }
            } break;
            case MINMEA_SENTENCE_VTG: {
                struct minmea_sentence_vtg frame;
                if (minmea_parse_vtg(&frame, gps_line)) {
                    pos.speed = minmea_rescale(&frame.speed_kph, 1E3);
                    pos.bearing = minmea_rescale(&frame.true_track_degrees, 1E6);
                    got_vtg = true;
                }
            } break;
        }

        if (got_gga && got_rmc && got_vtg) {
            pthread_mutex_lock(&pos_mutex);
            last_known_position = pos;
            pthread_mutex_unlock(&pos_mutex);

            pos.altitude_valid = false;
            got_gga = false;
            got_rmc = false;
            got_vtg = false;
        }
    }

    fclose(fp);
}

int mzd_gps_get_location(hu_location_t* location) {
    int result;
    pthread_mutex_lock(&pos_mutex);
    if (last_known_position.timestamp > 0) {
        *location = last_known_position;
        result = POSITION_AVAILABLE;
    } else {
        result = POSITION_NOT_AVAILABLE;
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