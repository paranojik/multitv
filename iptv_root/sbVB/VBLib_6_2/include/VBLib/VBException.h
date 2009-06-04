// VBException.h

// all entities in namespace br::com::sbVB::VBLib
namespace br {
namespace com {
namespace sbVB {
namespace VBLib {


class VBLIB_DLL_API VBException
{
	VBString m_message;
public:
	VBException(const char* message = "not defined");
	const char *getExplanation() const;
};

#define VB_CUSTOM_EXCEPTION(X) \
struct X : public br::com::sbVB::VBLib::VBException { \
  X(const char *m) : VBException(m) {}; \
};

// VB_CUSTOM_EXCEPTION(ABCControlException)

}}}} // close namespace ::br::com::sbVB::VBLib

