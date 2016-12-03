// This file contains payloads to be sent to AA for different sensor events

#ifndef HU_SENSORS_H
#define HU_SENSORS_H

static int hu_fill_night_mode_message(uint8_t* buffer, int buffer_len, int nightmode) {
    // Size sanity check
    if (buffer_len < 6) return 0;

    int buffCount = 0;
    buffer[buffCount++] = 0x80;
    buffer[buffCount++] = 0x03;
    buffer[buffCount++] = 0x52;
    buffer[buffCount++] = 0x02;
    buffer[buffCount++] = 0x08;
    buffer[buffCount++] = (nightmode ? 0x01 : 0x00);
    return buffCount;
}

#endif