
#include "TestCase.h"

/**
 * @author David Moss
 */
configuration TestSpiC {
}

implementation {

  components new TestCaseC() as SpiTestC;
  
  components TestSpiP,
      BlazeSpiC,
      HplCC1100PinsC,
      LedsC;
      
  TestSpiP.CSN -> HplCC1100PinsC.Csn;
  TestSpiP.PARTNUM -> BlazeSpiC.PARTNUM;
  TestSpiP.SNOP -> BlazeSpiC.SNOP;
  TestSpiP.SpiTest -> SpiTestC;
  TestSpiP.Resource -> BlazeSpiC;
  TestSpiP.Leds -> LedsC;
  
}
