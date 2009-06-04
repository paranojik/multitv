#ifndef __XFILE_
#define __XFILE_


class XFile
{
public:

    XFile (LPCSTR _fname=NULL);
    ~XFile(void);

    void   SetName     (LPCSTR _fname);
    LPCSTR GetName     (LPCSTR _fname)   { return m_filename;   }
    BOOL   IsOpen      (void)            { return m_flOpen;     }
    int    GetLastError(void)            { return m_iLastError; }
    BOOL   DoExist     (void);

    BOOL   open        (UINT _flags=0, UINT _mode=0);
    BOOL   close       (void);
    int    read        (void *_buf, UINT _size);
    int    write       (void *_buf, UINT _size);
    long   seek        (long _offset, int _nOrigin);
    long   tell        (void);

    BOOL   erase       (void);
    BOOL   rename      (LPCSTR _newname);
    long   getsize     (void);
    time_t gettime     (void);
    BOOL   settime     (time_t _ftime);

    BOOL   writetxt    (LPCSTR _buf);
    BOOL   readtxt     (LPSTR _buf, UINT _tbuf);
    int    readrandom  (void *_buf, UINT _treg, UINT _nreg);

    void   getreduz    (LPSTR _reduz, UINT _size);
    void   seppath     (LPSTR _path, LPSTR _filename);
    void   sepext      (LPSTR _path, LPSTR _ext);

private:

    BOOL  m_flOpen;
    ULONG m_hfile;

    int   m_iLastError;

    char  m_filename[TPATH+1];
};



#endif  // __XFILE_
