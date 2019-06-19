// EPOS Conversion Utility Declarations

#ifndef __convert_h
#define __convert_h

#include <system/config.h>

__BEGIN_UTIL

namespace Convert
{

template<unsigned long FREQUENCY, typename Count, typename Time, typename Temporary = unsigned long long>
inline Count s2count(const Time & t) { return static_cast<Temporary>(t) * FREQUENCY / 1; }
template<unsigned long FREQUENCY, typename Count, typename Time, typename Temporary = unsigned long long>
inline Count ms2count(const Time & t) { return static_cast<Temporary>(t) * FREQUENCY / 1000; }
template<unsigned long FREQUENCY, typename Count, typename Time, typename Temporary = unsigned long long>
inline Count us2count(const Time & t) { return static_cast<Temporary>(t) * FREQUENCY / 1000000; }

template<unsigned long FREQUENCY, typename Count, typename Time, typename Temporary = unsigned long long>
inline Time count2s(const Count & c) { return static_cast<Temporary>(c) * 1 / FREQUENCY; }
template<unsigned long FREQUENCY, typename Count, typename Time, typename Temporary = unsigned long long>
inline Time count2ms(const Count & c) { return static_cast<Temporary>(c) * 1000 / FREQUENCY; }
template<unsigned long FREQUENCY, typename Count, typename Time, typename Temporary = unsigned long long>
inline Time count2us(const Count & c) { return static_cast<Temporary>(c) * 1000000 / FREQUENCY; }

};

__END_UTIL

#endif
