/* Defines the E-Vtol Aircraft class */

#pragma once

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
  AIRCRAFT_STATE_WAITING_TO_CHARGE,
  AIRCRAFT_STATE_CHARGING,
} aircraft_state_t;

class Aircraft
{
private:
  /* data */
public:
  aircraft_id_t m_id;
  aircraft_state_t m_state;
  uint32_t m_airTimeTicks; // this is in units of loop-step-ticks
  uint32_t m_chargeTimeTicks; // this is in units of loop-step-ticks
  uint32_t m_numFaults;

  Aircraft(aircraft_id_t id);
};

Aircraft::Aircraft(aircraft_id_t id)
{
  m_id = id;
  m_state = AIRCRAFT_STATE_IDLE;
  m_airTimeTicks = 0;
  m_chargeTimeTicks = 0;
  m_numFaults = 0;
}