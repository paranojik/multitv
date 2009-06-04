#ifndef MACROS_H
#define MACROS_H

#define SAFE_DELETE(p) { if(p) { delete (p); (p)=NULL; } }

#endif

