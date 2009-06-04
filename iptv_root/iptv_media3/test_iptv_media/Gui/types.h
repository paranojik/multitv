#ifndef TYPES_H
#define TYPES_H

struct Rect {

    Rect() {};
    Rect(ULONG _ulLeft, ULONG _ulTop, ULONG _ulRight, ULONG _ulBottom)
    {
        Left   = _ulLeft;
        Top    = _ulTop;
        Right  = _ulRight;
        Bottom = _ulBottom;
    }

    Rect(Rect & _rcRect)
    {
        Left   = _rcRect.Left;
        Top    = _rcRect.Top;
        Right  = _rcRect.Right;
        Bottom = _rcRect.Bottom; 
    }

    ULONG Left,
          Top, 
          Right,
          Bottom;          
};

#endif