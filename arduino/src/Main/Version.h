#ifndef Version_h
#define Version_h

#define ArduinoVersion 2

#define MOUSSAILLON_ID 314
#define TELEMAQUE_ID 315

#define IS_MOUSSAILLON (BOAT_ID == MOUSSAILLON_ID)
#define IS_TELEMAQUE (BOAT_ID == TELEMAQUE_ID)

#if IS_MOUSSAILLON
#define BOAT_NAME "moussaillon"
#elif IS_TELEMAQUE
#define BOAT_NAME "telemaque"
#else
#error No boat defined
#endif

#endif // Version_h
