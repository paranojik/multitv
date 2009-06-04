#pragma once
#ifndef __ASLISTENER__
#define __ASLISTENER__

class CxAppSharingListener
{
    public:
    virtual void OnASEvent( int nEvent, void *pParams, unsigned int nSize ) = 0;
};

typedef CxAppSharingListener ASListener;
#endif //__ASLISTENER__
