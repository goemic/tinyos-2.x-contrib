#include <Timer.h>
#define AM_BOUNCEPACKET 0xCA
configuration TestLplPacketAppC
{
}
implementation
{
    components MainC, TestLplC as TestLpl;
    components UserButtonC;
    components RandomC;
    components LedsC;

    components QuantoLogRawUARTC as CLog;

    components ActiveMessageC;
    components new AMReceiverC(AM_BOUNCEPACKET);
    
    components ResourceContextsC;

    TestLpl.Boot -> MainC;
    TestLpl.Leds -> LedsC;

    TestLpl.AMControl -> ActiveMessageC;
    TestLpl.Receive -> AMReceiverC;

    TestLpl.QuantoLog -> CLog;
    TestLpl.UserButtonNotify -> UserButtonC;

    TestLpl.CPUContext -> ResourceContextsC.CPUContext;

    components CC2420ActiveMessageC;
    TestLpl.LowPowerListening -> CC2420ActiveMessageC;
}
 