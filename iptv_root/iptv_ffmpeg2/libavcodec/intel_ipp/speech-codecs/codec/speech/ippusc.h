
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "../../core/usc/include/usc.h"

extern USC_Fxns USC_G711A_Fxns;
extern USC_Fxns USC_G711U_Fxns;
extern USC_Fxns USC_G723_Fxns;
extern USC_Fxns USC_G728_Fxns;
extern USC_Fxns USC_G729A_Fxns;

USC_Fxns* GetUSC_G711A_Fxns(void);
USC_Fxns* GetUSC_G711U_Fxns(void);USC_Fxns* GetUSC_G723_Fxns(void);
USC_Fxns* GetUSC_G728_Fxns(void);
USC_Fxns* GetUSC_G729A_Fxns(void);

#ifdef __cplusplus
}
#endif // __cplusplus
