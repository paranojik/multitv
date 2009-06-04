#ifndef __SYSINFO
#define __SYSINFO


#ifdef __cplusplus
  extern "C" {
#endif


BOOL GetCPUUsage(double *_pCPUUsageAvg, double *_pCPUUsageMin, double *_pCPUUsageMax, UINT _nIterativ);


#ifdef __cplusplus
  }
#endif


#endif  // __SYSINFO
