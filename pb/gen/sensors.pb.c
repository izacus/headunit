/* Automatically generated nanopb constant definitions */
/* Generated by nanopb-0.3.8-dev at Sat Dec  3 23:05:59 2016. */

#include "sensors.pb.h"

/* @@protoc_insertion_point(includes) */
#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif



const pb_field_t HU_SensorEvent_fields[3] = {
    PB_FIELD(  1, MESSAGE , REPEATED, CALLBACK, FIRST, HU_SensorEvent, locationData, locationData, &HU_SensorEvent_LocationData_fields),
    PB_FIELD( 10, MESSAGE , REPEATED, CALLBACK, OTHER, HU_SensorEvent, nightMode, locationData, &HU_SensorEvent_DayNightMode_fields),
    PB_LAST_FIELD
};

const pb_field_t HU_SensorEvent_DayNightMode_fields[2] = {
    PB_FIELD(  1, BOOL    , REQUIRED, STATIC  , FIRST, HU_SensorEvent_DayNightMode, isNight, isNight, 0),
    PB_LAST_FIELD
};

const pb_field_t HU_SensorEvent_LocationData_fields[8] = {
    PB_FIELD(  1, UINT64  , OPTIONAL, STATIC  , FIRST, HU_SensorEvent_LocationData, timestamp, timestamp, 0),
    PB_FIELD(  2, INT32   , OPTIONAL, STATIC  , OTHER, HU_SensorEvent_LocationData, latitude, timestamp, 0),
    PB_FIELD(  3, INT32   , OPTIONAL, STATIC  , OTHER, HU_SensorEvent_LocationData, longitude, latitude, 0),
    PB_FIELD(  4, UINT32  , OPTIONAL, STATIC  , OTHER, HU_SensorEvent_LocationData, accuracy, longitude, 0),
    PB_FIELD(  5, INT32   , OPTIONAL, STATIC  , OTHER, HU_SensorEvent_LocationData, altitude, accuracy, 0),
    PB_FIELD(  6, INT32   , OPTIONAL, STATIC  , OTHER, HU_SensorEvent_LocationData, speed, altitude, 0),
    PB_FIELD(  7, INT32   , OPTIONAL, STATIC  , OTHER, HU_SensorEvent_LocationData, bearing, speed, 0),
    PB_LAST_FIELD
};


/* @@protoc_insertion_point(eof) */
