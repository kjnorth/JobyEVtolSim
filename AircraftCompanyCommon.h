/** Define data that is common to each aircraft company.
 * A loop step of the simulation will be treated as 0.2 seconds
 * to meet our minimum resolution requirement. Data values will
 * be in units of 0.02 second ticks.
 * 
 * If each tick is 0.02s, then there will be 50 ticks per simulation minute.
 * 
 * Note that the FLIGHT_DUR_TICKS come from the following equation:
 *  ((Battery capacity [kWh] / energy use at cruise [kWh/mile]) / cruise speed [miles/hour]) * 60 [min/hour] = flightDurMin
 *  => FLIGHT_DUR_TICKS = flightDurMin * LOOP_TICKS_PER_MIN
 */

#pragma once

#include <cstdint>

#include "Aircraft.h"

#define LOOP_TICKS_PER_MIN          (50u)

#define ALPHA_CRUISE_SPEED_MPH      (120u)
#define ALPHA_FLIGHT_DUR_TICKS      (100u * LOOP_TICKS_PER_MIN) // 100 min
#define ALPHA_CHARGE_DUR_TICKS      (36u * LOOP_TICKS_PER_MIN)
#define ALPHA_PASSENGER_COUNT       (4u)
#define ALPHA_FAULT_PROB_PER_HR     (0.25f) // fault probability per hour

#define BRAVO_CRUISE_SPEED_MPH      (100u)
#define BRAVO_FLIGHT_DUR_TICKS      (40u * LOOP_TICKS_PER_MIN)
#define BRAVO_CHARGE_DUR_TICKS      (12u * LOOP_TICKS_PER_MIN)
#define BRAVO_PASSENGER_COUNT       (5u)
#define BRAVO_FAULT_PROB_PER_HR     (0.10f)

#define CHARLIE_CRUISE_SPEED_MPH    (160u)
#define CHARLIE_FLIGHT_DUR_TICKS    ((uint32_t) (37.5f * LOOP_TICKS_PER_MIN)) // casting as uint32 to ensure data type aligns with the structure defined below
#define CHARLIE_CHARGE_DUR_TICKS    (48u * LOOP_TICKS_PER_MIN)
#define CHARLIE_PASSENGER_COUNT     (3u)
#define CHARLIE_FAULT_PROB_PER_HR   (0.05f)

#define DELTA_CRUISE_SPEED_MPH      (90u)
#define DELTA_FLIGHT_DUR_TICKS      (100u * LOOP_TICKS_PER_MIN)
#define DELTA_CHARGE_DUR_TICKS      ((uint32_t) (37.2f * LOOP_TICKS_PER_MIN)) // casting as uint32 to ensure data type aligns with the structure defined below
#define DELTA_PASSENGER_COUNT       (2u)
#define DELTA_FAULT_PROB_PER_HR     (0.22f)

#define ECHO_CRUISE_SPEED_MPH       (30u)
#define ECHO_FLIGHT_DUR_TICKS       ((uint32_t) (51.72f * LOOP_TICKS_PER_MIN)) // casting as uint32 to ensure data type aligns with the structure defined below
#define ECHO_CHARGE_DUR_TICKS       (18u * LOOP_TICKS_PER_MIN)
#define ECHO_PASSENGER_COUNT        (2u)
#define ECHO_FAULT_PROB_PER_HR      (0.61f)

typedef struct {
  uint32_t cruiseSpeedMph;
  uint32_t flightDurationTicks;
  uint32_t chargeDurationTicks;
  uint32_t passengerCount;
  float faultProbabilityPerHour;
} aircraft_company_common_t;

bool IsBatteryDead(Aircraft* plane);
bool IsChargingComplete(Aircraft* plane);
uint32_t GetCruiseSpeed(aircraft_id_t id);
uint32_t GetPassengerCount(aircraft_id_t id);
float GetFaultProbabilityPerHour(aircraft_id_t id);