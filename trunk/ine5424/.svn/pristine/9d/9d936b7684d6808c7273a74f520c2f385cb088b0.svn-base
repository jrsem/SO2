// EPOS Smart Transducer Declarations

#ifndef __tranducer_h
#define __tranducer_h

__BEGIN_SYS

class Transducer_Common
{
public:
    // Invalid sensor data
    enum { INVALID = -1 };

protected:
    Transducer_Common() {}
};

template<unsigned int U>
class Dummy_Transducer
{
private:
    class Dummy_Predictor
    {
    public:
        template<typename T>
        T predict(const T &) { return static_cast<T>(0); }
    };

public:
    typedef _UTIL::Observed Observed;
    typedef _UTIL::Observer Observer;

    typedef Dummy_Predictor Predictor;
//    struct Predictor_Configuration : public Predictor::Configuration {};

    static const unsigned int UNIT = U;
    static const int ERROR = 0;
    static const bool INTERRUPT = false;
    static const bool POLLING = false;
    static const bool predictive = false;

public:
    template<typename S>
    static void sense(unsigned int dev, S * data) {}
    template<typename S>
    static void actuate(unsigned int dev, S * data, const typename S::Value & command) {}

    static Power_Mode power() { return Power_Mode::FULL; }
    static void power(const Power_Mode & mode) {}

    static void attach(Observer * obs) { _observed.attach(obs); }
    static void detach(Observer * obs) { _observed.detach(obs); }

private:
    static Observed _observed;
};

__END_SYS

#endif
