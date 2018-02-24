
#define OTA true

#define TIME true

#define DELAY_1_TYPE "bus"                       // the type of the transport (metros, rers, tramways or bus)
#define DELAY_1_LINE "131"                       // the code of the line (consult https://api-ratp.pierre-grimaud.fr/v3/lines/{type})
#define DELAY_1_STATION "les+coquettes"          // the slug of the station (consult http://../stations/{type}/{code})
#define DELAY_1_WAY "R"                          // A or R - way (consult https://api-ratp.pierre-grimaud.fr/v3/destinations/{type}/{code})

#define DELAY_2_TYPE "metros"                    // the type of the transport (metros, rers, tramways or bus)
#define DELAY_2_LINE "7"                         // the code of the line (consult https://api-ratp.pierre-grimaud.fr/v3/lines/{type})
#define DELAY_2_STATION "villejuif+leo+lagrange" // the slug of the station (consult http://../stations/{type}/{code})
#define DELAY_2_WAY "R"                          // A or R - way (consult https://api-ratp.pierre-grimaud.fr/v3/destinations/{type}/{code})

#define TRAFFIC_1 "tramways"                     // all metros
#define TRAFFIC_2 "metros/7"                     // line 7 of metros
#define TRAFFIC_3 "metros/8"                     // line 8 of metros
#define TRAFFIC_4 "metros/10"                    // line 10 of metros
#define TRAFFIC_5 "rers/A"                       // the RER A
#define TRAFFIC_6 ""
#define TRAFFIC_7 ""
#define TRAFFIC_8 ""
