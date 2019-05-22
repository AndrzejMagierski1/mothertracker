
#include "Arduino.h"
#include "RamMonitor.h"

RamMonitor ramMonitor;

void RamMonitor::report_ram_stat(const char* aname, uint32_t avalue)
{
  Serial.print(aname);
  Serial.print(": ");
  Serial.print((avalue + 512) / 1024);
  Serial.print(" Kb (");
  Serial.print((((float) avalue) / total()) * 100, 1);
  Serial.println("%)");
};

void RamMonitor::report_ram()
{
  //bool lowmem;
  //bool crash;

  Serial.println("==== memory report ====");

  report_ram_stat("free", adj_free());
  report_ram_stat("stack", stack_total());
  report_ram_stat("heap", heap_total());
/*
  lowmem = warning_lowmem();
  crash = warning_crash();
  if(lowmem || crash) {
    Serial.println();

    if(crash)
      Serial.println("**warning: stack and heap crash possible");
    else if(lowmem)
      Serial.println("**warning: unallocated memory running low");
  };
*/
  Serial.println();
};
