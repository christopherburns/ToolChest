#ifndef CLOCK_H
#define CLOCK_H

#include "System.h"
#include "String.h"

#include <mach/mach_time.h>

namespace ToolChest
{
   class TimeStamp;
   class TimeDuration;
   class StopWatch;

   class Time
   {
   public:
      friend class TimeStamp;
      friend class TimeDuration;
      friend class StopWatch;

   private:

      ////////////////////////
      // System Clock Stuff //
      ////////////////////////

      /// Note: The APPLE code paths here are mostly untested.

      /// Reads the x86 cycle count register, works on any (post-Pentium) Intel
      /// processor. The rdtsc instruction is used to move the 64-bit counter value
      /// into the resgisters EDX:EAX.
      ///
      ///

      /// Returns the performance counter reading, converted to nanoseconds
      inline static uint64 readClockNS()
      {
         #if defined(___WINDOWS_NT)

            LARGE_INTEGER pCounter, freq;
            QueryPerformanceCounter(&pCounter);
            QueryPerformanceFrequency(&freq);
            uint64 nsReading = (uint64)((double)pCounter.QuadPart / (double)freq.QuadPart * 1000000000.0);
            return nsReading;

         #else

            static bool initialized = false;
            static mach_timebase_info_data_t sTimebaseInfo;
            static double conversionFactor;
            if (!initialized)
            {
               (void) mach_timebase_info(&sTimebaseInfo);
               conversionFactor = (double)sTimebaseInfo.numer / (double)sTimebaseInfo.denom;
               initialized = true;
            }
            return uint64(conversionFactor * mach_absolute_time());

         #endif
      }

      inline static uint64 readClockCounter()
      {
         #if defined(___OSX)
         return mach_absolute_time();
         #elif defined(___WINDOWS_NT)

         LARGE_INTEGER qwTime;
         QueryPerformanceCounter(&qwTime);
         return (uint64)qwTime.QuadPart;

         //uint64 clock = __rdtsc();
         //uint32 _[4]; __cpuid(_, 0); uint64 clock = __rdtsc(); __cpuid(_, 0);
         //return clock;

         #endif
      }

      /// Provides a scale factor to convert clock ticks (as read by readCycleCounter())
      /// to seconds. This value is the number of ticks per second (Hz)
      inline static double readClockFrequency()
      {
         #if defined(___OSX)
         static bool initialized = false;
         static mach_timebase_info_data_t sTimebaseInfo;

         if (!initialized)
         {
            (void) mach_timebase_info(&sTimebaseInfo);
            initialized = true;
         }

         /// I think this works.
         return sTimebaseInfo.numer / sTimebaseInfo.denom * 1e-9;

         #elif defined(___WINDOWS_NT)

         LARGE_INTEGER qwTicksPerSec;
         QueryPerformanceFrequency(&qwTicksPerSec);
         return (double)cast<uint64>(qwTicksPerSec.QuadPart);

         #else /// LINUX ?

         return 0.0;

         #endif
      }
   };


   /// Write-once, read-only timestamp, units are in CPU clock cycles
   class TimeStamp
   {
   private:
      uint64 _time;

   public:
      inline TimeStamp() : _time(Time::readClockNS()) {}
      inline uint64 Nanoseconds() const { return _time; }
   };


   /// Represents a finite scalar quantity of machine time
   class TimeDuration
   {
   private:
      uint64 _ns;

      /// Private constructor, from 64-bit integer
      inline TimeDuration(const uint64& t) : _ns(t) {}

   public:
      inline TimeDuration() : _ns(0) {}

      /// Basic arithmetic
      inline TimeDuration operator + (const TimeDuration& t) const
      { return TimeDuration(_ns + t._ns); }
      inline TimeDuration operator - (const TimeDuration& t) const
      { return TimeDuration(_ns - t._ns); }
      inline TimeDuration operator / (double f) const
      { return TimeDuration((uint64)((double)_ns / f)); }

      /// Interval between two time stamps
      friend inline TimeDuration operator - (const TimeStamp& ts0, const TimeStamp& ts1);

      inline double ToSeconds()      const { return (double)_ns / 1000000000.0; }
      inline double ToMilliseconds() const { return (double)_ns / 1000000.0; }
      inline double ToMicroseconds() const { return (double)_ns / 1000.0; }
      inline uint64 ToNanoseconds()  const { return _ns; }

      /// Pretty printing
      inline String ToString(int prec = 3) { return String(_ns) + " ns"; }
   };

   /// Interval between two time stamps
   inline TimeDuration operator - (const TimeStamp& ts0, const TimeStamp& ts1)
   { return TimeDuration(ts0.Nanoseconds() - ts1.Nanoseconds()); }


   /// Stop Watch abstraction, used for precision timing of code
   class StopWatch
   {
   private:
      TimeStamp _startTime, _stopTime;

   public:

      inline void Start() { Reset(); }
      inline void Stop() { new (&_stopTime) TimeStamp(); } 
      inline TimeDuration ReadTime() const { return _stopTime - _startTime; }
      inline void Reset()
      {
         // Create new TimeStamps (placement new)
         new (&_startTime) TimeStamp();
         new (&_stopTime) TimeStamp();
      }
   };

}; // namespace ToolChest

/// Some handy-dandy macros for timing code sections
#define START_TIMER(t) StopWatch timer_##t; timer_##t.start();
#define STOP_AND_REPORT_TIMER(t, s) \
timer_##t.stop(); \
printf("%s: %.4f s (%.6f ms)\n", s, timer_##t.readTime().toSeconds(), timer_##t.readTime().toMilliseconds());


#endif   // CLOCK_H
