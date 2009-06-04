#include "DllChat.h"

#ifndef __MESSAGEDATA__
#define __MESSAGEDATA__

class CHAT_API cxObj
{
    public:
        virtual ~cxObj( void )=0;
};

class CHAT_API MessageData
{
    public:
        char *lpszSource;
        char *lpszTarget;
        char *lpszFullCmd;
        cxObj *lParam;
        int  nOpCode;

        MessageData(  void );
        virtual ~MessageData( void );

        void reset( void );
};

class CHAT_API cxString : public cxObj
{
    private:
        char *str;
    public:
        cxString( char *_str );
        cxString( void );
        cxString( cxString & cxStr ) ;
        int GetLen( void );
        char *GetStr( void );
        void SetStr( char *_str );
        virtual ~cxString();
};

class CHAT_API MediaRequest  : public cxObj
{
    private:
        char            *lpszChannel;
        char            *lpszName;
        char            *lpszIp;
        int             mc_MediaClass;
        int             pt_Protocol;
        int             vs_MediaSetOption;
        int             vn_MediaNotify;
        unsigned long   PacketLen;
        unsigned long   HeaderLen;
        char            *lpHeader;
        unsigned long   nMediaID;
        unsigned long   nMediaPort;
        unsigned long   wData1;
        unsigned long   wData2;

        ULONG   uBitrate;
        ULONG   nHeaderSegs;
        ULONG   uHeaderMaxSegSize;
        ULONG   wHeaderChksum;

        void *pHeaderSegsCtrlVector;      // Vetor para controle de segmentos de Header (viewer)

        void init(void);

    public:
        /* Constructors */
        MediaRequest( char *_lpszChannel, int MediaClass, int Protocol, unsigned long nPacketLen, unsigned long nHeaderLen, void *_lpHeader );
        MediaRequest( char *_lpszChannel, char *MediaClass, char *Protocol, unsigned long nPacketLen, unsigned long nHeaderLen, void *_lpHeader );
        MediaRequest( char *_lpszChannel, int MediaClass, unsigned long nID, unsigned long nPort );
        MediaRequest( char *_lpszChannel, char * MediaClass, unsigned long nID, unsigned long nPort );
        MediaRequest( unsigned long nID, int Protocol, unsigned long nPort = 0 );
        MediaRequest( unsigned long nID, char *Protocol, unsigned long nPort = 0 );
        MediaRequest( unsigned long nID, unsigned long nPacketLen, unsigned long nPort, unsigned long nHeaderLen, void *_lpHeader );
        MediaRequest( void );
        virtual ~MediaRequest( void );

        void reset(void);

        /* Private Access Functions */
        char * GetMediaClass      ( void );
        char * GetProtocol        ( void );
        char * GetMediaNotify     ( void );
        char * GetMediaSetOption  ( void );

        char * GetChannel         (void) { return lpszChannel;       }
        char * GetName            (void) { return lpszName;          }
        char * GetIp              (void) { return lpszIp;            }
        char * GetNickname        (void) { return lpszName;          }
        char * GetHeaderBuffer    (void) { return lpHeader;          }
        ULONG  GetPacketLen       (void) { return PacketLen;         }
        ULONG  GetHeaderLen       (void) { return HeaderLen;         }
        ULONG  GetMediaID         (void) { return nMediaID;          }
        ULONG  GetMediaPort       (void) { return nMediaPort;        }
        ULONG  GetData1           (void) { return wData1;            }
        ULONG  GetData2           (void) { return wData2;            }

        ULONG  GetBitrate         (void) { return uBitrate;          }
        ULONG  GetHeaderNSegs     (void) { return nHeaderSegs;       }
        ULONG  GetHeaderMaxSegSize(void) { return uHeaderMaxSegSize; }
        ULONG  GetHeaderChksum    (void) { return wHeaderChksum;     }

        void SetBitrate         (ULONG _val)  { uBitrate          = _val;    }
        void SetHeaderNSegs     (ULONG _val)  { nHeaderSegs       = _val;    }
        void SetHeaderMaxSegSize(ULONG _val)  { uHeaderMaxSegSize = _val;    }
        void SetHeaderChksum    (ULONG _val)  { wHeaderChksum     = _val;    }

        BOOL IsViewerCtrlRdy    (void);
        BOOL AddHeaderSeg       (ULONG _mediaID, ULONG _seg, ULONG _segsize, LPCSTR _pHeaderSeg);

        void SetMediaClass      (int mc)      { mc_MediaClass = mc;     }
        void SetMediaSetOption  (int vs)      { vs_MediaSetOption = vs; }
        void SetMediaNotify     (int vn)      { vn_MediaNotify = vn;    }
        void SetHeaderLen       (int nLen)    { HeaderLen = nLen;       }
        void SetPacketLen       (int nLen)    { PacketLen = nLen;       }
        void SetMediaID         (int nID)     { nMediaID = nID;         }
        void SetMediaPort       (int nPort)   { nMediaPort = nPort;     }
        void SetData1           (ULONG wData) { wData1 = wData;         }
        void SetData2           (ULONG wData) { wData2 = wData;         }
        void SetProtocol        (int pt)      { pt_Protocol = pt;       }

        void SetChannel ( LPCSTR str );
        void SetName    ( LPCSTR str );
        void SetIp      ( LPCSTR str );
        void SetNickname( LPCSTR str );
        void SetHeader  ( void *buf, int Len );

        void SetMediaClass     ( LPCSTR vc );
        void SetProtocol       ( LPCSTR pt );
        void SetMediaSetOption ( LPCSTR vs );
        void SetMediaNotify    ( LPCSTR vn );

        MediaRequest & operator=( MediaRequest &vr );
};

class CHAT_API MediaQuery : public cxObj
{
    private:
        char *lpszOwner;
        char *lpszChannel;
        char *lpszMediaName;
        int   vcMediaClass;
        int   nMediaID;
        int   nBitrate;

    public:
        MediaQuery( void );
        ~MediaQuery( void );
        void SetOwner     ( char *str   );
        void SetChannel   ( char *str   );
        void SetMediaName ( char *str   );
        void SetMediaClass( int   vc    );
        void SetMediaClass( char *vc    );
        void SetMediaID   ( int nID     );
        void SetBitrate   ( int bitrate );

        char *GetOwner     ( void );
        char *GetChannel   ( void );
        char *GetMediaName ( void );
        char *GetMediaClass( void );
        int   GetMediaID   ( void );
        int   GetBitrate   ( void );

        MediaQuery & operator=( MediaQuery & vq );
};

class CHAT_API MediaRcvUser  : public cxObj
{
    private:
        char *lpszNickname;
        int   nStatus;

    public:
        MediaRcvUser ( void );
        ~MediaRcvUser( void );
        void SetNickname( char *str    );
        void SetStatus  ( char *status );
        void SetStatus  ( int  status  );

        char *GetNickname( void );
        char *GetStatus  ( void );

        MediaRcvUser & operator=( MediaRcvUser & vru );
};

class CHAT_API MediaPtP : public cxObj
{
    private:
        char          *pszNickTarget;
        int            mc_MediaClass;
        char          *pszIPLocalSource;
        char          *pszIPSrvSource;
        unsigned long  nRcvSourcePort;
        unsigned long  nSendSourcePort;
        unsigned long  nRcvUDPPingSourcePort;
        unsigned long  nSourceBitrate;

    public:
        MediaPtP( void );
        ~MediaPtP( void );

        void SetNickTarget( char *str );
        void SetIPLocalSource( char *ip );
        void SetIPSrvSource( char *ip );
        void SetMediaClass( char *mc );
        void SetMediaClass( int   mc );
        void SetRcvSourcePort( unsigned long port );
        void SetSendSourcePort( unsigned long port );
        void SetUDPPingSourcePort( unsigned long port );
        void SetSourceBitrate( unsigned long bitrate );

        char *GetNickTarget( void );
        char *GetIPLocalSource( void );
        char *GetIPSrvSource( void );
        char *GetMediaClass( void );
        unsigned long GetRcvSourcePort( void );
        unsigned long GetSendSourcePort( void );
        unsigned long GetUDPPingSourcePort( void );
        unsigned long GetSourceBitrate( void );

        MediaPtP & operator=( MediaPtP &mptp );
};

#endif /* __MESSAGEDATA__ */
