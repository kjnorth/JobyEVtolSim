#include "AircraftCompanyCommon.h"

/** Define data common to aircraft of the same company.
 * Use the aircraft id to index through this array. */
const aircraft_company_common_t G_AircraftCompanyCommon[TOTAL_AIRCRAFT_COMPANIES] = {
  {ALPHA_CRUISE_SPEED_MPH, ALPHA_FLIGHT_DUR_TICKS, ALPHA_CHARGE_DUR_TICKS, ALPHA_PASSENGER_COUNT, ALPHA_FAULT_PROB_PER_HR},
  {BRAVO_CRUISE_SPEED_MPH, BRAVO_FLIGHT_DUR_TICKS, BRAVO_CHARGE_DUR_TICKS, BRAVO_PASSENGER_COUNT, BRAVO_FAULT_PROB_PER_HR},
  {CHARLIE_CRUISE_SPEED_MPH, CHARLIE_FLIGHT_DUR_TICKS, CHARLIE_CHARGE_DUR_TICKS, CHARLIE_PASSENGER_COUNT, CHARLIE_FAULT_PROB_PER_HR},
  {DELTA_CRUISE_SPEED_MPH, DELTA_FLIGHT_DUR_TICKS, DELTA_CHARGE_DUR_TICKS, DELTA_PASSENGER_COUNT, DELTA_FAULT_PROB_PER_HR},
  {ECHO_CRUISE_SPEED_MPH, ECHO_FLIGHT_DUR_TICKS, ECHO_CHARGE_DUR_TICKS, ECHO_PASSENGER_COUNT, ECHO_FAULT_PROB_PER_HR},
};

bool IsBatteryDead(Aircraft* plane) {
    return ((plane->m_airTimeTicks % G_AircraftCompanyCommon[plane->m_id].flightDurationTicks) == 0);
}

bool IsChargingComplete(Aircraft* plane) {
    return ((plane->m_chargeTimeTicks % G_AircraftCompanyCommon[plane->m_id].chargeDurationTicks) == 0);
}

uint32_t GetFlightDurTicks(aircraft_id_t id) {
    return G_AircraftCompanyCommon[id].flightDurationTicks;
}

uint32_t GetChargeDurTicks(aircraft_id_t id) {
    return G_AircraftCompanyCommon[id].chargeDurationTicks;
}

uint32_t GetCruiseSpeedMph(aircraft_id_t id) {
    return G_AircraftCompanyCommon[id].cruiseSpeedMph;
}

uint32_t GetPassengerCount(aircraft_id_t id) {
    return G_AircraftCompanyCommon[id].passengerCount;
}

float GetFaultProbabilityPerHour(aircraft_id_t id) {
    return G_AircraftCompanyCommon[id].faultProbabilityPerHour;
}