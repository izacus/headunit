// This file contains payloads to be sent to AA for different sensor events

#ifndef HU_SENSORS_H
#define HU_SENSORS_H

#include "pb_encode.h"
#include "gen/sensors.pb.h"

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

#endif