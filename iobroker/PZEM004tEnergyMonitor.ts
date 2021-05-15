/*
  Typescript script for integration in iobroker. Put this into your global scripts,
  and create another script that instantiates the energy monitor script like this:

  var pzemMonitor:PZEM004tEnergyMonitor = new PZEM004tEnergyMonitor("mqtt.0.energy", '0_userdata.0.energy');
*/

class PZEM004tEnergyMonitor
{
    private pathEnergyOffset:string = "energyOffset";
    private pathEnergy:string = "energy";
    private threshold:number = 1;

    private devicePath:string;
    private path:string;

    private phases:string[] =
        [
            "phase1",
            "phase2",
            "phase3"
        ];

    private energyOffsets:number[] = [ 0,0,0 ];

    constructor(readAddress:string, writeAddress:string)
    {
        this.devicePath = readAddress + ".";
        this.path = writeAddress + ".";

        for(var i:number=0; i<3; i++)
        {
            var phaseNr:number = i+1;

            var savedEnergyOffsetPath:string = this.path + "phase" + phaseNr + "." + this.pathEnergyOffset;
            var deviceEnergyOffsetPath:string = this.devicePath + "phase" + phaseNr + "." + this.pathEnergyOffset;

            createState(savedEnergyOffsetPath, 0, {type: 'number', name: ('Saved energy offset for phase ' + phaseNr)  });
            createState(this.path + "phase" + phaseNr + "." + this.pathEnergy, 0, {type: 'number', name: ('Total energy for phase ' + phaseNr)  });
            createState(this.path + "phasesCombined." + this.pathEnergy, 0, {type: 'number', name: ('Total energy of all phases combined')  });

            var energyOffsetDevice:number = getState(deviceEnergyOffsetPath).val;
            var savedEnergyOffset:number =  getState(savedEnergyOffsetPath).val;
            var energyOffset:number = Math.max(energyOffsetDevice, savedEnergyOffset);

            setState(savedEnergyOffsetPath, energyOffset);
            setState(deviceEnergyOffsetPath, energyOffset);
            this.energyOffsets[i] = energyOffset;

            log("pzem004t: Initialized phase " + phaseNr + " offset: " + energyOffset);
        }

        schedule("* * * * * *", () =>
        {
            this.update();
        });
    }

    private getTotalEnergyDelta():number
    {
        var delta:number = 0;
        for(var i:number=0; i<3; i++)
        {
            var phaseNr:number = i+1;
            delta = delta + (getState(this.devicePath + "phase" + phaseNr + "." + this.pathEnergy).val - this.energyOffsets[i]);
        }
        return delta;
    }

    private updateOffsets():void
    {
        for(var i:number=0; i<3; i++)
        {
            var phaseNr:number = i+1;
            var energy:number = this.energyOffsets[i] + getState(this.devicePath + "phase" + phaseNr + "." + this.pathEnergy).val;

            this.energyOffsets[i] = energy;
            setState(this.path + "phase" + phaseNr + "." + this.pathEnergyOffset, energy);
            setState(this.devicePath + "phase" + phaseNr + "." + this.pathEnergyOffset, energy);

            log("Set new energy offset for phase " + phaseNr + ": " + energy);
        }
    }

    private update():void
    {
        if(this.getTotalEnergyDelta() > this.threshold)
        {
            log("################ UPDATE OFFSETS");
            this.updateOffsets();
            setState(this.devicePath + "control.resetEnergy", "confirm");
        }

        var totalEnergy:number = 0;
        for(var i:number=0; i<3; i++)
        {
            var phaseNr:number = i+1;
            var energy:number = this.energyOffsets[i] + getState(this.devicePath + "phase" + phaseNr + "." + this.pathEnergy).val;

            setState(this.path + "phase" + phaseNr + "." + this.pathEnergy, this.round(energy));
            totalEnergy = totalEnergy + energy;
        }
        setState(this.path + "phasesCombined." + this.pathEnergy, this.round(totalEnergy));
    }

    private round(value:number)
    {
        return Math.round(value * 100) / 100;
    }
}







//