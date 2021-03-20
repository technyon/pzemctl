#include "Pzem004t.h"

namespace hw
{
    Pzem004t::Pzem004t(HardwareSerial* serial1, HardwareSerial* serial2, HardwareSerial* serial3)
    : _pzem1(serial1),
      _pzem2(serial2),
      _pzem3(serial3)
    {
        _pzem1.setAddress(0xf8);
        _pzem2.setAddress(0xf8);
        _pzem3.setAddress(0xf8);

        _pzem1.resetEnergy();
        _pzem2.resetEnergy();
        _pzem3.resetEnergy();
    }

    void Pzem004t::update()
    {
        updateValues(_pzem1, _values1);
        updateValues(_pzem2, _values2);
        updateValues(_pzem3, _values3);

    }

    void Pzem004t::updateValues(PZEM004Tv30& pzem, pzem004tvalues& values)
    {
        values.voltage = pzem.voltage();
        values.current = pzem.current();
        values.power = pzem.power();
        values.energy = pzem.energy();
        values.frequency = pzem.frequency();
        values.pf = pzem.pf();
    }

    const pzem004tvalues &Pzem004t::values1()
    {
        return _values1;
    }
    const pzem004tvalues &Pzem004t::values2()
    {
        return _values2;
    }
    const pzem004tvalues &Pzem004t::values3()
    {
        return _values3;
    }

    void Pzem004t::resetEnergy()
    {
        Serial.println(F("Resetting energy"));
        _pzem1.resetEnergy();
        _pzem2.resetEnergy();
        _pzem3.resetEnergy();
    }

}