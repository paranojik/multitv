#ifndef ITHREAD_H
#define ITHREAD_H

class IThread
{
public:
   virtual bool IsThreadOk(void) = 0;
   virtual int  Resume(void) = 0;
   virtual int  Suspend(void) = 0;
   virtual void Exit(unsigned retval) = 0;
   virtual int  WaitForMe(unsigned long) = 0;
   virtual ~IThread(void) {}
};

typedef int (*ThreadFunction) (IThread*,void*);

void Wait( unsigned long t );  // time in milliseconds

IThread* CreateIThread( ThreadFunction func, void* closure = NULL, BOOL flResume=FALSE );
   
#endif
