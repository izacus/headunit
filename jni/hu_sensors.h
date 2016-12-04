// This file contains payloads to be sent to AA for different sensor events

#ifndef HU_SENSORS_H
#define HU_SENSORS_H

#include <stdint.h>

#include "pb_encode.h"
#include "gen/sensors.pb.h"

typedef struct {
    uint64_t    timestamp;
    int32_t     latitude;  /** Decimal latitude * 1E7 **/
    int32_t     longitude; /** Decimal longtitude * 1E7 **/
    uint32_t    accuracy;   /** Accuracy in m * 1E3 **/
    bool        altitude_valid; /** Whether we have a 3D fix **/
    int32_t     altitude;   /** Altitude in m * 1E2 **/
    int32_t     speed;      /** Speed in km/h * 1E3 **/
    int32_t     bearing;    /** Bearing in degrees * 1E6 **/
} hu_location_t;

static bool pb_encode_nightmode(pb_ostream_t *stream, const pb_field_t* field, void* const *arg) {
    if (!pb_encode_tag_for_field(stream, field)) {
        return false;
    }

    int nightmode = *((int*)*arg);
    HU_SensorEvent_DayNightMode dnm = HU_SensorEvent_DayNightMode_init_zero;
    dnm.isNight = nightmode;
    return pb_encode_submessage(stream, HU_SensorEvent_DayNightMode_fields, &dnm);
}

static int hu_fill_night_mode_message(uint8_t* buffer, int buffer_len, int nightmode) {
    if (buffer_len < 6) return 0;

    // Write header first
    buffer[0] = 0x80;
    buffer[1] = 0x03;   // Touch event

    pb_ostream_t ostream = pb_ostream_from_buffer(buffer + 2, buffer_len - 2);
    HU_SensorEvent event = HU_SensorEvent_init_zero;
    event.nightMode.funcs.encode = &pb_encode_nightmode;
    event.nightMode.arg = &nightmode;
    bool status = pb_encode(&ostream, HU_SensorEvent_fields, &event);

    if (!status) return 0;
    return 2 + ostream.bytes_written;
}

static bool pb_encode_locationdata(pb_ostream_t* stream, const pb_field_t* field, void* const *arg) {
    if (!pb_encode_tag_for_field(stream, field)) {
        return false;
    }

    hu_location_t* location_info = (hu_location_t*)*arg;
    HU_SensorEvent_LocationData ld = HU_SensorEvent_LocationData_init_zero;
    ld.has_timestamp = true;
    ld.has_latitude = true;
    ld.has_longitude = true;
    ld.has_accuracy = true;
    ld.has_speed = true;
    ld.has_bearing = true;

    ld.timestamp = location_info->timestamp;
    ld.latitude = location_info->latitude;
    ld.longitude = location_info->longitude;
    ld.accuracy = location_info->accuracy;
    ld.speed = location_info->speed;
    ld.bearing = location_info->bearing;

    if (location_info->altitude_valid) {
        ld.has_altitude = true;
        ld.altitude = location_info->altitude;
    }

    return pb_encode_submessage(stream, HU_SensorEvent_LocationData_fields, &ld);
}

static int hu_fill_location_message(uint8_t* buffer, int buffer_len, hu_location_t location_info) {
    if (buffer_len < 2) return 0;
    // Header
    buffer[0] = 0x80;
    buffer[1] = 0x03;

    pb_ostream_t ostream = pb_ostream_from_buffer(buffer + 2, buffer_len - 2);
    HU_SensorEvent event = HU_SensorEvent_init_zero;
    event.locationData.funcs.encode = &pb_encode_locationdata;
    event.locationData.arg = &location_info;
    bool status = pb_encode(&ostream, HU_SensorEvent_fields, &event);

    if (!status) return 0;
    return 2 + ostream.bytes_written;
}



#endif