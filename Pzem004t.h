//
// Created by daphreak on 07.03.21.
//

#pragma once
#include <PZEM004Tv30.h>
#include <Arduino.h>

namespace hw
{
    struct pzem004tvalues
    {
        float voltage;   // Volt     1  2     4
        float current;   // Ampere   1     3
        float power;     // Watt        2  3
        float energy;    // kw/h           3
        float frequency; // Hz       1  2     4
        float pf;        //                   4
        bool error;     // false = ok;  true = error
    };

    class Pzem004t
    {
    public:
        explicit Pzem004t(HardwareSerial* serial1, HardwareSerial* serial2, HardwareSerial* serial3);
        virtual ~Pzem004t() = default;

        void update();
        void resetEnergy();

        const pzem004tvalues& values1();
        const pzem004tvalues& values2();
        const pzem004tvalues& values3();

    private:
        void updateValues(PZEM004Tv30& pzem, pzem004tvalues& values);

        PZEM004Tv30 _pzem1;
        PZEM004Tv30 _pzem2;
        PZEM004Tv30 _pzem3;

        pzem004tvalues _values1;
        pzem004tvalues _values2;
        pzem004tvalues _values3;
    };
}
