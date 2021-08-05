
#include <Preferences.h>
#include <osw_hal.h>

#define PREFS_STEPS "PS"
#define PREFS_STEPS_DAYLASTCHECKED "D"
#define PREFS_STEPS_SUN "0"
#define PREFS_STEPS_MON "1"
#define PREFS_STEPS_TUE "2"
#define PREFS_STEPS_WED "3"
#define PREFS_STEPS_THU "4"
#define PREFS_STEPS_FRI "5"
#define PREFS_STEPS_SAT "6"
#define PREFS_STEPS_ALL "A"

/**
 * The steps statics assumes that the accelerometer is carrying the daily steps information.
 * Idea: remember when was the last time to store the steps of today. Do this whenever we read
 * the steps for the first time of the day.
 */

uint16_t OswHal::getStepsToday(void) {
#if defined(FEATURE_STATS_STEPS)
  // TODO: check if we need to carryover steps
  Preferences prefs;
  prefs.begin(PREFS_STEPS, false);

  // fetch the weekday
  uint32_t dayOfMonth = 0;
  uint32_t dayOfWeek = 0;            // 0 = Sunday, 1 = Monday, ... 6 = Saturday
  getDate(&dayOfMonth, &dayOfWeek);  //

  // TODO: check if we are running on a new day
  // TODO: read lastDayChecked)
  uint32_t dayLastChecked = prefs.getULong(PREFS_STEPS_DAYLASTCHECKED, 0);
  uint32_t dayToday = getLocalTime() / 60 / 60 / 24;

  // read the actual accelerometer
  uint32_t steps = getAccelStepCount();

  // checks if dayLastChecked is either yesterday, or some time in the past
  if (dayLastChecked != dayToday) {
    // update the day of week
    switch (dayOfWeek) {
      case 0:  // today is sunday, so store in saturday, a.s.o.
        prefs.putULong(PREFS_STEPS_SAT, steps);
      case 1:
        prefs.putULong(PREFS_STEPS_SUN, steps);
      case 2:
        prefs.putULong(PREFS_STEPS_MON, steps);
      case 3:
        prefs.putULong(PREFS_STEPS_TUE, steps);
      case 4:
        prefs.putULong(PREFS_STEPS_WED, steps);
      case 5:
        prefs.putULong(PREFS_STEPS_THU, steps);
      case 6:
        prefs.putULong(PREFS_STEPS_FRI, steps);
    }
    // update total steps
    uint32_t allSteps = prefs.getULong(PREFS_STEPS_ALL) + steps;
    prefs.putULong(PREFS_STEPS_ALL, allSteps);

    // update the day we last checked
    prefs.putULong(PREFS_STEPS_DAYLASTCHECKED, dayToday);
    resetAccelerometer();
    // reset steps

    steps = 0;
#ifdef DEBUG
    Serial.print("Step Stats: new day.");
#endif
  }

  prefs.end();

  // simply return steps data
  return steps;
#else
  getAccelStepCount();
#endif
}
uint16_t OswHal::getStepsOnDay(uint8_t dayOfWeek) {
  Preferences prefs;
  prefs.begin(PREFS_STEPS, true /*readonly*/);
  uint32_t steps = 0;
  switch (dayOfWeek) {
    case 0:  // read for sunday returns sunday, a.s.o.
      steps = prefs.getULong(PREFS_STEPS_SUN, 0);
    case 1:
      steps = prefs.getULong(PREFS_STEPS_MON, 0);
    case 2:
      steps = prefs.getULong(PREFS_STEPS_TUE, 0);
    case 3:
      steps = prefs.getULong(PREFS_STEPS_WED, 0);
    case 4:
      steps = prefs.getULong(PREFS_STEPS_THU, 0);
    case 5:
      steps = prefs.getULong(PREFS_STEPS_FRI, 0);
    case 6:
      steps = prefs.getULong(PREFS_STEPS_SAT, 0);
  }
  prefs.end();
  return steps;
}
uint32_t OswHal::getStepsTotal(void) {
#if defined(FEATURE_STATS_STEPS)
  Preferences prefs;
  prefs.begin(PREFS_STEPS, true /*readonly*/);
  uint32_t steps = 0;
  steps = prefs.getULong(PREFS_STEPS_ALL, 0) + getStepsToday();
  prefs.end();
  return steps;
#else
  return getStepsToday();
#endif
}