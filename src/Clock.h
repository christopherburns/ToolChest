#pragma once

#ifndef CLOCK_H
#define CLOCK_H

#include "System.h"
#include "String.h"

namespace Burns
{
   class TimeStamp;
   class TimeDuration;
   class StopWatch;

   /// Write-once, read-only timestamp, units are in CPU clock cycles
   class TimeStamp
   {
   private:
      uint64 _time;

   public:
      FINLINE TimeStamp() : _time(readClockNS()) {}
      FINLINE uint64 Nanoseconds() const { return _time; }
   };


   /// Represents a finite scalar quantity of machine time
   class TimeDuration
   {
   private:
      uint64 _ns;

      /// Private constructor, from 64-bit integer
      FINLINE TimeDuration(const uint64& t) : _ns(t) {}

   public:
      FINLINE TimeDuration() : _ns(0) {}

      /// Basic arithmetic
      FINLINE TimeDuration operator + (const TimeDuration& t) const
      { return TimeDuration(_ns + t._ns); }
      FINLINE TimeDuration operator - (const TimeDuration& t) const
      { return TimeDuration(_ns - t._ns); }
      FINLINE TimeDuration operator / (double f) const
      { return TimeDuration((uint64)((double)_ns / f)); }

      /// Interval between two time stamps
      friend FINLINE TimeDuration operator - (const TimeStamp& ts0, const TimeStamp& ts1);

      FINLINE double ToSeconds()      const { return (double)_ns / 1000000000.0; }
      FINLINE double ToMilliseconds() const { return (double)_ns / 1000000.0; }
      FINLINE double ToMicroseconds() const { return (double)_ns / 1000.0; }
      FINLINE uint64 ToNanoseconds()  const { return _ns; }

      /// Pretty printing
      FINLINE String ToString(int prec = 3) { return String(_ns) + " ns"; }
   };

   /// Interval between two time stamps
   FINLINE TimeDuration operator - (const TimeStamp& ts0, const TimeStamp& ts1)
   { return TimeDuration(ts0.Nanoseconds() - ts1.Nanoseconds()); }


   /// Stop Watch abstraction, used for precision timing of code
   class StopWatch
   {
   private:
      TimeStamp _startTime, _stopTime;

   public:

      FINLINE void Start() { Reset(); }
      FINLINE void Stop() { new (&_stopTime) TimeStamp(); } 
      FINLINE TimeDuration ReadTime() const { return _stopTime - _startTime; }
      FINLINE void Reset()
      {
         // Create new TimeStamps (placement new)
         new (&_startTime) TimeStamp();
         new (&_stopTime) TimeStamp();
      }
   };

}; // namespace Burns

/// Some handy-dandy macros for timing code sections
#define START_TIMER(t) StopWatch timer_##t; timer_##t.start();
#define STOP_AND_REPORT_TIMER(t, s) \
timer_##t.stop(); \
printf("%s: %.4f s (%.6f ms)\n", s, timer_##t.readTime().toSeconds(), timer_##t.readTime().toMilliseconds());


#endif   // CLOCK_H
