#ifndef __XVERSION
#define __XVERSION


#ifndef VERSIONLEN
#define VERSIONLEN          63      /* Length of version name.   */
#endif  // VERSIONLEN


class Version
{
public:
    Version(LPCSTR _version=NULL);

    void SetVersion(LPCSTR _version);
    int  compare   (LPCSTR _version);
    int  compare   (Version &_cmpversion);

    LPCSTR GetVersionStr(void) { return m_version;   }

    UINT   GetMajor     (void) { return m_major;     }
    UINT   GetWorkset   (void) { return m_workset;   }
    UINT   GetInterface (void) { return m_interface; }
    UINT   GetRelease   (void) { return m_release;   }

private:

    char m_version[VERSIONLEN+1];

    UINT m_major,
         m_workset,
         m_interface,
         m_release;
};



#ifdef _WIN32
LPCSTR GetFileVersion( LPSTR _filename, LPSTR _pversion, UINT _szversion );
#endif  // _WIN32

#endif  // __XVERSION


