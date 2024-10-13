/** Define data that is common to each aircraft company.
 * A loop step of the simulation will be treated as 0.2 seconds
 * to meet our minimum resolution requirement. Data values will
 * be in units of 0.2 second ticks.
 * 
 * If each tick is 0.2s, then there will be 5 ticks per simulation minute.
 * 
 * Common data:
 * 
 * Flight time until battery dies (loop step ticks)
 * Time to charge (loop step ticks)
 * Passenger count
 * Fault probability
 */

#pragma once

#include <cstdint>

#define LOOP_TICKS_PER_MIN        (5u)

#define ALPHA_CRUISE_SPEED_MPH    (120u)
#define ALPHA_FLIGHT_DUR_TICKS    (100u * LOOP_TICKS_PER_MIN) // 100 min * loop ticks per min
#define ALPHA_CHARGE_DUR_TICKS    (36u * LOOP_TICKS_PER_MIN)
#define ALPHA_PASSENGER_COUNT     (4u)
#define ALPHA_FAULT_PROBABILITY   (0.25f)

typedef struct {
  uint32_t cruiseSpeedMph;
  uint32_t flightDurationTicks;
  uint32_t chargeDurationTicks;
  uint32_t passengerCount;
  float faultProbability;
} aircraft_company_common_t;