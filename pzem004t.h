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
        float voltage;
        float current;
        float power;
        float energy;
        float frequency;
        float pf;
    };

    class pzem004t
    {
    public:
        explicit pzem004t(HardwareSerial* serial1, HardwareSerial* serial2, HardwareSerial* serial3);
        virtual ~pzem004t() = default;

        void update();

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
