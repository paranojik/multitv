// VBGenMath.h

// all entities in namespace br::com::sbVB::VBLib
namespace br {
namespace com {
namespace sbVB {
namespace VBLib {


////////////////////////////////////////////////////////////////////
// global functions for math

extern VBLIB_DLL_API double eps;

// return true absulute difference (x-y) < eps
template <typename T>
bool nearEqual(const T & x, const T & y)
{ 
	return ::fabs(x-y) < eps; 
};

/*===============================================================*/
// the default implementation of functions
template <typename T, typename REAL = double>
struct VBNumTrait
{
	// absolute value
	static T abs(const T & x)
	{
		return ::fabs(x);
	}

	// return true if x is near equal to y
	static bool nearEqualTrait(const T & x, const T & y) 
	{
		return nearEqual(x,y);
	}

	// return true if x is near zero
	static bool nearZero(const T & x) 
	{
		return nearEqual<REAL>(x,0);
	}

	// true if x > y
	static bool greater(const T & x, const T & y) 
	{
		return x > y;
	}

	// true if x >= y
	static bool greaterEqual(const T & x, const T & y) 
	{
		return x >= y;
	}

	// return the int cast of x
	static int getInt(const T & x)
	{
		return (int)x;
	}

};

// implementation for VBComplex_F=complex<float>
template <>
struct VBNumTrait<VBComplex_F>
{
    static T abs(const T & x)
	{
		REAL abs = sqrt(x.real()*x.real()+x.imag()*x.imag());
		return T(abs,0);    
	}
    static bool nearEqualTrait(const T & x, const T & y) // near equal
	{
		return nearEqual(x.real(),y.real()) && nearEqual(x.imag(),y.imag());
	}
	static bool nearZero(const T & x) 
	{
		REAL absX = sqrt(x.real()*x.real()+x.imag()*x.imag());
		return nearEqual<REAL>(absX,0);
	}
	static bool greater(const T & x, const T & y) // true if x > y
	{
		REAL absX = sqrt(x.real()*x.real()+x.imag()*x.imag());
		REAL absY = sqrt(y.real()*y.real()+y.imag()*y.imag());
		return absX > absY;
	}
	static bool greaterEqual(const T & x, const T & y) // true if x >= y
	{
		REAL absX = sqrt(x.real()*x.real()+x.imag()*x.imag());
		REAL absY = sqrt(y.real()*y.real()+y.imag()*y.imag());
		return absX >= absY;
	}
	static int getInt(const T & x)
	{
		REAL absX = sqrt(x.real()*x.real()+x.imag()*x.imag());
		return (int)absX;
	}
};

// implementation for VBComplex_D=complex<double>
template <>
struct VBNumTrait<VBComplex_D>
{
    static T abs(const T & x)
	{
		REAL abs = sqrt(x.real()*x.real()+x.imag()*x.imag());
		return T(abs,0);    
	}
    static bool nearEqualTrait(const T & x, const T & y) 
	{
		return nearEqual(x.real(),y.real()) && nearEqual(x.imag(),y.imag());
	}
	static bool nearZero(const T & x) 
	{
		REAL absX = sqrt(x.real()*x.real()+x.imag()*x.imag());
		return nearEqual<REAL>(absX,0);
	}
	static bool greater(const T & x, const T & y) 
	{
		REAL absX = sqrt(x.real()*x.real()+x.imag()*x.imag());
		REAL absY = sqrt(y.real()*y.real()+y.imag()*y.imag());
		return absX > absY;
	}
	static bool greaterEqual(const T & x, const T & y) 
	{
		REAL absX = sqrt(x.real()*x.real()+x.imag()*x.imag());
		REAL absY = sqrt(y.real()*y.real()+y.imag()*y.imag());
		return absX >= absY;
	}
	static int getInt(const T & x)
	{
		REAL absX = sqrt(x.real()*x.real()+x.imag()*x.imag());
		return (int)absX;
	}
};

// implementation for VBComplex_LD=complex<long double>
template <>
struct VBNumTrait<VBComplex_LD>
{
    static T abs(const T & x)
	{
		REAL abs = sqrt(x.real()*x.real()+x.imag()*x.imag());
		return T(abs,0);    
	}
    static bool nearEqualTrait(const T & x, const T & y) // near equal
	{
		return nearEqual(x.real(),y.real()) && nearEqual(x.imag(),y.imag());
	}
	static bool nearZero(const T & x) 
	{
		REAL absX = sqrt(x.real()*x.real()+x.imag()*x.imag());
		return nearEqual<REAL>(absX,0);
	}
	static bool greater(const T & x, const T & y) // true if x > y
	{
		REAL absX = sqrt(x.real()*x.real()+x.imag()*x.imag());
		REAL absY = sqrt(y.real()*y.real()+y.imag()*y.imag());
		return absX > absY;
	}
	static bool greaterEqual(const T & x, const T & y) // true if x >= y
	{
		REAL absX = sqrt(x.real()*x.real()+x.imag()*x.imag());
		REAL absY = sqrt(y.real()*y.real()+y.imag()*y.imag());
		return absX >= absY;
	}
	static int getInt(const T & x)
	{
		REAL absX = sqrt(x.real()*x.real()+x.imag()*x.imag());
		return (int)absX;
	}
};

////////////////////////////////////////////////////////////////////
// global functions based on VBNumTrait

// generic absolute value (good for real and complex)
template<typename T>
T genAbs(const T & x)
{
	return VBNumTrait<T>::abs(x);
};

// generic near equal (good for real and complex)
template<typename T>
bool genNe(const T & x,const T & y)
{
	return VBNumTrait<T>::nearEqualTrait(x,y);
};

// generic greater (good for real and complex)
template<typename T>
bool genGreater(const T & x,const T & y)
{
	return VBNumTrait<T>::greater(x,y);
};

// generic greater (good for real and complex)
template<typename T>
bool genGreaterEqual(const T & x,const T & y)
{
	return VBNumTrait<T>::greaterEqual(x,y);
};

// generic greater (good for real and complex)
template<typename T>
int genGetInt(const T & x)
{
	return VBNumTrait<T>::getInt(x);
};

// generic greater (good for real and complex)
template<typename T>
bool nearZero(const T & x)
{
	return VBNumTrait<T>::nearZero(x);
};


}}}} // close namespace ::br::com::sbVB::VBLib
