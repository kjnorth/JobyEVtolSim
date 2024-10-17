#include <iostream>

#include "Output.h"

#include "AircraftCompanyCommon.h"

enum dataSumIndicies {
  TOTAL_FLIGHT_TIME_MIN_IDX = 0u,
  TOTAL_NUM_FLIGHTS_IDX,
  TOTAL_CHARGE_TIME_MIN_IDX,
  TOTAL_CHARGE_SESSIONS_IDX,
  TOTAL_NUM_FAULTS_IDX,

  ALL_DATA_SUMS
};

typedef struct {
  float avgMinPerFlight; // use this to calculate and write average miles per flight to the output
  float avgMinPerChargeSession;
  uint32_t totalNumFaults;
  float totalPassengerMiles;
} data_avgs_and_totals_t;

/*
  Keep track of the following statistics per vehicle type: 
  - average flight time per flight
  - average distance traveled per flight ... This can be computed from the average flight time per flight. No need to store!
  - average time charging per charge session
  - total number of faults
  - total number of passenger miles.
*/
void ComputeDataAveragesAndTotals(data_avgs_and_totals_t* averagesAndTotals, Aircraft* aircrafts) {
  // first, sum up the data
  float dataSums[TOTAL_AIRCRAFT_COMPANIES][ALL_DATA_SUMS] = {0};
  // in this loop, i increments through the 20 aircraft in the simulation
  for (uint8_t i = 0; i < TOTAL_AIRCRAFTS_IN_SIMULATION; i++) {
    // add up flight times, num flights, time charging, num charge sessions, and faults
    aircraft_id_t curId = aircrafts[i].m_id; // aircraft id Alpha, Bravo, Charlie, Delta, or Echo
    dataSums[curId][TOTAL_FLIGHT_TIME_MIN_IDX] += ((float) aircrafts[i].m_airTimeTicks / (float) LOOP_TICKS_PER_MIN);
    dataSums[curId][TOTAL_NUM_FLIGHTS_IDX] += (float) aircrafts[i].m_numFlights;
    dataSums[curId][TOTAL_CHARGE_TIME_MIN_IDX] += ((float) aircrafts[i].m_chargeTimeTicks / (float) LOOP_TICKS_PER_MIN);
    dataSums[curId][TOTAL_CHARGE_SESSIONS_IDX] += (float) aircrafts[i].m_numChargeSessions;
    dataSums[curId][TOTAL_NUM_FAULTS_IDX] += (float) aircrafts[i].m_numFaults;
  }

  // now compute the averages/totals
  // in this loop, i increments through the 5 aircraft companies
  for (uint8_t i = 0; i < TOTAL_AIRCRAFT_COMPANIES; i++) {
    uint32_t curVehicleTypeCruiseSpeedMph = GetCruiseSpeedMph((aircraft_id_t) i);

    averagesAndTotals[i].avgMinPerFlight =
        dataSums[i][TOTAL_FLIGHT_TIME_MIN_IDX] / dataSums[i][TOTAL_NUM_FLIGHTS_IDX];

    averagesAndTotals[i].avgMinPerChargeSession =
        dataSums[i][TOTAL_CHARGE_TIME_MIN_IDX] / dataSums[i][TOTAL_CHARGE_SESSIONS_IDX];

    averagesAndTotals[i].totalNumFaults = dataSums[i][TOTAL_NUM_FAULTS_IDX]; // this is redundant, but here to simplify the passing of arguments around

    // (cruiseSpeed miles/hour * (1hour / 60min) * totalFlightTimeMin * num passengers) = passenger miles
    averagesAndTotals[i].totalPassengerMiles =
        ((curVehicleTypeCruiseSpeedMph * dataSums[i][TOTAL_FLIGHT_TIME_MIN_IDX]) / 60.0f) *
        GetPassengerCount((aircraft_id_t) i);
  }
}

/** Function to test that all of the calculations above are working correctly.
 * First, create an array of 20 vehicle types - 4 of each vehicle.
 * Configure each vehicle such that it has completed 1 full flight, 1 full charge,
 * and had 1 fault.
 * 
 * The average flight time for each vehicle type should be that of 1 full flight,
 * and similarly for the average charge time. The total faults for each vehicle
 * type should be 4.
 * 
 * Expected total num passenger miles calculations:
 * Alpha - 4 flights * 100 min/flight * 1/60 hr/min * 120 miles/hr * 4 passengers = 3200
 * Bravo - 4 flights * 40 min/flight * 1/60 hr/min * 100 miles/hr * 5 passengers = 1333.333
 * Charlie - 4 flights * 37.5 min/flight * 1/60 hr/min * 160 miles/hr * 3 passengers = 1200
 * Delta - 4 flights * 100 min/flight * 1/60 hr/min * 90 miles/hr * 2 passengers = 1200
 * Echo - 4 flights * 51.72 min/flight * 1/60 hr/min * 30 miles/hr * 2 passengers = 206.88
 */
void TestCalculations(void) {
  Aircraft aircrafts[TOTAL_AIRCRAFTS_IN_SIMULATION];
  for (uint8_t i = 0; i < TOTAL_AIRCRAFTS_IN_SIMULATION; i++) {
    aircraft_id_t newId = (aircraft_id_t) (i % TOTAL_AIRCRAFT_COMPANIES);
    aircrafts[i].m_id = newId;
    aircrafts[i].m_airTimeTicks = GetFlightDurTicks(newId);
    aircrafts[i].m_numFlights = 1;
    aircrafts[i].m_chargeTimeTicks = GetChargeDurTicks(newId);
    aircrafts[i].m_numChargeSessions = 1;
    aircrafts[i].m_numFaults = 1;
  }

  data_avgs_and_totals_t averagesAndTotals[TOTAL_AIRCRAFT_COMPANIES]; // calculations returned into this array
  ComputeDataAveragesAndTotals(averagesAndTotals, aircrafts);

  // output to terminal
  for (uint8_t i = 0; i < TOTAL_AIRCRAFT_COMPANIES; i++) {
    float avgDistanceTraveled = (GetCruiseSpeedMph((aircraft_id_t) i) * averagesAndTotals[i].avgMinPerFlight) / 60.0f;
    std::cout <<
      "vehicle type: " << +i << ", " <<
      "avg flight time: " << averagesAndTotals[i].avgMinPerFlight << " min, " <<
      "avg distance traveled: " << avgDistanceTraveled << " miles,\n" <<
      "avg time charging: " << averagesAndTotals[i].avgMinPerChargeSession << " min, " <<
      "total num faults: " << averagesAndTotals[i].totalNumFaults << ", " <<
      "total passenger miles: " << averagesAndTotals[i].totalPassengerMiles << " miles" <<
      "\n------------------------------------------------------------------------------------" <<
      std::endl;
  }
}

void WriteDataToOutputFile(Aircraft* aircrafts) {
  
}