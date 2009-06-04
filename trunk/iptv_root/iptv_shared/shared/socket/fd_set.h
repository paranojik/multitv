#ifndef _FD_SET_STR
#define _FD_SET_STR


struct Myfd_set
{
    u_int  fd_count;               /* how many are SET? */
    SOCKET fd_array[FD_SETSIZE];   /* an array of SOCKETs */

    Myfd_set(void);

    UINT getnfds(void) { return fd_count; }

    void zero (void);
    void clr  (SOCKET _fd);
    BOOL set  (SOCKET _fd);
    UINT isset(SOCKET _fd);
};



#endif  // _FD_SET_STR

