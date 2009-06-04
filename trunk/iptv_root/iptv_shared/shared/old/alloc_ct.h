#ifdef __cplusplus
extern "C" {
#endif

BOOL InitMemoryManager(void);
void KillMemoryManager(void);

HGLOBAL GlobalAlloc_ap(DWORD size);
HGLOBAL GlobalReAlloc_ap(HGLOBAL mHandle, DWORD size);
HGLOBAL GlobalFree_ap(HGLOBAL mHandle);

void far * GlobalLock_ap(HGLOBAL mHandle);
int  GlobalUnlock_ap(HGLOBAL mHandle);

HGLOBAL GAllocPtr(DWORD size, void far * (far *ptr));
HGLOBAL GFreePtr(HGLOBAL handle);

#ifdef __cplusplus
}
#endif
