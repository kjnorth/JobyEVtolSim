/* Defines the E-Vtol Aircraft class */

#pragma once

#include <iostream>
#include <cstdint>

typedef enum {
  AIRCRAFT_ID_ALPHA = 0u,
  AIRCRAFT_ID_BRAVO,
  AIRCRAFT_ID_CHARLIE,
  AIRCRAFT_ID_DELTA,
  AIRCRAFT_ID_ECHO,
} aircraft_id_t;

typedef enum {
  AIRCRAFT_STATE_IDLE = 0u,
  AIRCRAFT_STATE_FLYING,
  AIRCRAFT_STATE_CHARGING,
  AIRCRAFT_STATE_WAITING_TO_CHARGE,
} aircraft_state_t;

class Aircraft
{
public:
  Aircraft(aircraft_id_t id);
  void print(uint32_t cruiseSpeedMph, uint32_t numPassengers) const;

  aircraft_id_t m_id;
  aircraft_state_t m_state;
  uint32_t m_airTimeTicks; // this is in units of loop-step-ticks
  uint32_t m_chargeTimeTicks; // this is in units of loop-step-ticks
  uint32_t m_numFaults;
};

Aircraft::Aircraft(aircraft_id_t id)
{
  m_id = id;
  m_state = AIRCRAFT_STATE_IDLE;
  m_airTimeTicks = 0;
  m_chargeTimeTicks = 0;
  m_numFaults = 0;
}

inline void Aircraft::print(uint32_t cruiseSpeedMph, uint32_t numPassengers) const {
  float airTimeMin = m_airTimeTicks / (float) LOOP_TICKS_PER_MIN;
  float chargeTimeMin = m_chargeTimeTicks / (float) LOOP_TICKS_PER_MIN;

  // speed in miles/hour * (1hour / 60min) * airTimeMin
  float distanceTraveled = (cruiseSpeedMph * airTimeMin) / 60.0f;
  float passengerMiles = numPassengers * distanceTraveled;

  std::cout <<
    "Aircraft ID: " << m_id <<
    "\nState: " << m_state <<
    "\nFlight time: " << airTimeMin <<
    "\nCharge time: " << chargeTimeMin <<
    "\nNum faults: " << m_numFaults <<
    "\nDistance traveled: " << distanceTraveled <<
    "\nPassenger miles: " << passengerMiles <<
    "\n-------------------\n" <<
    std::endl;
};