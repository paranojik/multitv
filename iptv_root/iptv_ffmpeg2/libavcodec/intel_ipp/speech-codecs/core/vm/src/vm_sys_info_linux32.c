/*
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the terms of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the terms of that agreement.
//       Copyright(c) 2003-2007 Intel Corporation. All Rights Reserved.
//
*/


#ifdef LINUX32

#include "vm_sys_info.h"
#include "vm_file.h"
#include <time.h>
#include <sys/utsname.h>
#include <unistd.h>

#ifdef OSX32
#include <sys/syslimits.h>
/* to access to sysctl function */
#include <sys/types.h>
#include <sys/sysctl.h>

/*
 * retrieve information about integer parameter
 * defined as CTL_... (ctl_class) . ENTRY (ctl_entry)
 * return value in res.
 *  status : 1  - OK
 *           0 - operation failed
 */
Ipp32u osx_sysctl_entry_32u( int ctl_class, int ctl_entry, Ipp32u *res ) {
  int dcb[2];
  size_t i;
   dcb[0] = ctl_class;  dcb[1] = ctl_entry;
   i = sizeof(res[0]);
  return (sysctl(dcb, 2, res, &i, NULL, 0) != -1) ? 1 : 0;
}

#else
#include <sys/sysinfo.h>


#endif /* OSX32 */

void vm_sys_info_get_date(vm_char *m_date, DateFormat df)
{
    time_t ltime;
    struct tm *today;
    size_t len = 0;

    /* check error(s) */
    if (NULL == m_date)
        return;

    len = vm_string_strlen(m_date);

    time(&ltime);
    today = localtime(&ltime);

    switch (df)
    {
    case DDMMYY:
        strftime(m_date, len, "%d/%m/%Y", today);
        break;

    case MMDDYY:
        strftime(m_date, len, "%m/%d/%Y", today);
        break;

    case YYMMDD:
        strftime(m_date, len, "%Y/%m/%d", today);
        break;

    default:
        strftime(m_date, len, "%m/%d/%Y", today);
        break;
    }
} /* void vm_sys_info_get_date(vm_char *m_date, DateFormat df) */

void vm_sys_info_get_time(vm_char *m_time, TimeFormat tf)
{
    time_t ltime;
    struct tm *today;

    /* check error(s) */
    if (NULL == m_time)
        return;

    time(&ltime);
    today = localtime(&ltime);

    switch (tf)
    {
    case HHMMSS:
        strftime(m_time, 128, "%H:%M:%S", today);
        break;

    case HHMM:
        strftime(m_time, 128, "%H:%M", today);
        break;

    default:
        strftime(m_time, 128, "%H:%M:%S", today);
        break;
    }
} /* void vm_sys_info_get_time(vm_char *m_time, TimeFormat tf) */

Ipp32u vm_sys_info_get_cpu_num(void)
{
  Ipp32u cpu_num = 0;

#ifdef OSX32
  return (osx_sysctl_entry_32u(CTL_HW, HW_NCPU, &cpu_num)) ? cpu_num : 1;
#else
    vm_file *pfile = NULL;
    vm_char buf[PATH_MAX];

    pfile = vm_file_fopen(VM_STRING("/proc/cpuinfo"), "r");
    if (!pfile)
        return 1;

    while ((vm_file_fgets(buf, PATH_MAX, pfile)))
    {
        if (!vm_string_strncmp(buf, VM_STRING("processor"), 9))
            cpu_num++;
    }
    fclose(pfile);
    return (cpu_num) ? cpu_num : 1;
#endif
} /* Ipp32u vm_sys_info_get_cpu_num(void) */

void vm_sys_info_get_cpu_name(vm_char *cpu_name)
{
#ifdef OSX32
  size_t pv;
  char s[128], k[128];
  pv = 128;
  if (sysctlbyname( "machdep.cpu.vendor", s, &pv, NULL, 0 ) == -1)
    vm_string_strcpy( s, (vm_char *)"Problem determine vendor" );
  pv = 128;
  if (sysctlbyname( "machdep.cpu.brand_string", k, &pv, NULL, 0 ) == -1)
    vm_string_strcpy( k, (vm_char *)"Problem determine brand string" );
  vm_string_sprintf( cpu_name, VM_STRING("%s %s"), k, s );
#else
    FILE *pFile = NULL;
    vm_char buf[PATH_MAX];
    vm_char tmp_buf[PATH_MAX];
    size_t len;

    /* check error(s) */
    if (NULL == cpu_name)
        return;

    pFile = vm_file_fopen(VM_STRING("/proc/cpuinfo"), "r");
    if (!pFile)
        return;

    while ((vm_file_fgets(buf, PATH_MAX, pFile)))
    {
        if (!vm_string_strncmp(buf, VM_STRING("vendor_id"), 9))
        {
            vm_string_strncpy(tmp_buf, (vm_char*)(buf + 12), vm_string_strlen(buf) - 13);
        }
        else if (!vm_string_strncmp(buf, VM_STRING("model name"), 10))
        {
            if ((len = vm_string_strlen(buf) - 14) > 8)
                vm_string_strncpy(cpu_name, (vm_char *)(buf + 13), len);
            else
                vm_string_sprintf(cpu_name, VM_STRING("%s"), tmp_buf);
        }
    }
    fclose(pFile);
#endif
} /* void vm_sys_info_get_cpu_name(vm_char *cpu_name) */

void vm_sys_info_get_vga_card(vm_char *vga_card)
{
    /* check error(s) */
    if (NULL == vga_card)
        return;
} /* void vm_sys_info_get_vga_card(vm_char *vga_card) */

void vm_sys_info_get_os_name(vm_char *os_name)
{
    struct utsname buf;

    /* check error(s) */
    if (NULL == os_name)
        return;

    uname(&buf);
    vm_string_sprintf(os_name, VM_STRING("%s %s"), buf.sysname, buf.release);

} /* void vm_sys_info_get_os_name(vm_char *os_name) */

void vm_sys_info_get_computer_name(vm_char *computer_name)
{
    /* check error(s) */
    if (NULL == computer_name)
        return;

    gethostname(computer_name, 128);

} /* void vm_sys_info_get_computer_name(vm_char *computer_name) */

void vm_sys_info_get_program_name(vm_char *program_name)
{
    /* check error(s) */
    if (NULL == program_name)
        return;

#ifdef OSX32
    program_name = (vm_char *)getprogname();
#else
    vm_char path[PATH_MAX] = {0,};
    size_t i = 0;

    readlink("/proc/self/exe", path, sizeof(path));
    i = vm_string_strrchr(path, (vm_char)('/')) - path + 1;
    vm_string_strncpy(program_name, path + i, vm_string_strlen(path) - i);
#endif /* OSX32 */

} /* void vm_sys_info_get_program_name(vm_char *program_name) */

void vm_sys_info_get_program_path(vm_char *program_path)
{
    vm_char path[ PATH_MAX ] = {0,};
    size_t i = 0;

#ifndef OSX32
    /* check error(s) */
    if (NULL == program_path)
        return;

    readlink("/proc/self/exe", path, sizeof(path));
    i = vm_string_strrchr(path, (vm_char)('/')) - path + 1;
    vm_string_strncpy(program_path, path, i-1);
#else /* for OSX32 */
    if ( getcwd( path, PATH_MAX ) != NULL )
      vm_string_strcpy( program_path, path );
#endif /* OSX32 */

} /* void vm_sys_info_get_program_path(vm_char *program_path) */

void vm_sys_info_get_program_description(vm_char *program_description)
{
    /* check error(s) */
    if (NULL == program_description)
        return;

} /* void vm_sys_info_get_program_description(vm_char *program_description) */

Ipp32u vm_sys_info_get_cpu_speed(void)
{
#ifdef OSX32
    Ipp32u freq;
    return (osx_sysctl_entry_32u(CTL_HW, HW_CPU_FREQ, &freq)) ? (Ipp32u)(freq/1000000) : 1000;
#else
    Ipp64f ret = 0;
    FILE *pFile = NULL;
    vm_char buf[PATH_MAX];

    pFile = vm_file_fopen(VM_STRING("/proc/cpuinfo"), "r" );
    if (!pFile)
        return 1000;

    while ((vm_file_fgets(buf, PATH_MAX, pFile)))
    {
        if (!vm_string_strncmp(buf, VM_STRING("cpu MHz"), 7))
        {
            ret = vm_string_atol((vm_char *)(buf + 10));
            break;
        }
    }
    fclose(pFile);
    return ((Ipp32u) ret);
#endif
} /* Ipp32u vm_sys_info_get_cpu_speed(void) */

Ipp32u vm_sys_info_get_mem_size(void)
{
#ifndef OSX32
    struct sysinfo info;
    sysinfo(&info);
    return (Ipp32u)((Ipp64f)info.totalram / (1024 * 1024) + 0.5);
#else
    Ipp32u bts;
    return (osx_sysctl_entry_32u(CTL_HW, HW_PHYSMEM, &bts)) ? (Ipp32u)(bts/(1024*1024+0.5)) : 1000;
#endif /* OSX32 */

} /* Ipp32u vm_sys_info_get_mem_size(void) */


#ifdef UMC_VERSION_INFO
/* Functions to obtain UMC version information */
vm_char *vm_get_version_string( void ) {
  return umc_version_string;
  }

Ipp64u vm_get_version_number( void ) {
  return umc_version_number;
  }
#endif
#else
# pragma warning( disable: 4206 )
#endif /* LINUX32 */
