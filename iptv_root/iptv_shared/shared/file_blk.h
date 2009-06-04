#ifndef _FILE_BLK
#define _FILE_BLK


#define FBLK_MODE_TXT     0
#define FBLK_MODE_BIN     1


class far FileBlock
{
   private:
      char  Source[TPATH+1];

      BOOL  FileOpen,
            FileBlkOk,
            TargetOpen,
            flEof;

      UINT  DOSFileDate,
            DOSFileTime;

      int   SaveMode;

      long  OffRead,
            OffLine,
            OffBegBuffer,
            OffLineWrite,
            OffInsert,
            SizeRead,
            FileLength;

      LPSTR    lpReadBuf,
               lpWriteBuf;

      HFILE    hfile,
               hfileTarget;

      OFSTRUCT ofstr,
               ofstrTarget;


      long Read         (void);
      BOOL Write        (long Size = -1);
      BOOL Save         (long OffBegin, long OffEnd);
      long ReadBlock    (long Size);
      UINT SubstStr     (UINT BufSize, LPCSTR lpFind, LPCSTR lpNewText, BOOL WholeWords);
      UINT SubstVectStr (UINT BufSize, int nelem, LPCSTR far *lpFind, LPCSTR far *lpNewText, BOOL WholeWords);

   public:
      FileBlock         (LPCSTR Path, BOOL flOpen = TRUE);
     ~FileBlock         (void);

      BOOL FileOk       (void)  { return FileBlkOk;   }
      long GetLength    (void)  { return FileLength; }

      BOOL GetDOSDate   (UINT far *DOSDate, UINT far *DOSTime);

      BOOL Create       (void);
      BOOL Open         (void);
      BOOL OpenTarget   (void);
      void Close        (void);
      BOOL Seek         (long NewOffset);
      long ReadLine     (LPSTR BufLine, UINT BufSize);
      BOOL WriteLine    (LPCSTR BufLine);
      void Reset        (void);

      BOOL BeginInsert  (long OffsetEndFirstBlock = -1);
      BOOL EndInsert    (long OffsetBeginFinalBlock = -1);
      void AbortInsert  (void);

      BOOL BeginSave    (int Mode = FBLK_MODE_TXT);
      BOOL EndSave      (void);
      void AbortSave    (void) { AbortInsert(); }
                        
      BOOL Insert       (long Offset, void _huge *hpBlock, long BlockSize);
      BOOL Delete       (long OffsetBegin, long OffsetEnd);
      BOOL ChangeStr    (LPCSTR lpFind, LPCSTR lpNewText, BOOL WholeWords, int Mode);
      BOOL ChangeVectStr(int nelem, LPCSTR far *lpFind, LPCSTR far *lpNewText, BOOL WholeWords, int Mode);
};


#endif
