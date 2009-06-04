/* /////////////////////////////////////////////////////////////////////////////
//
//                  INTEL CORPORATION PROPRIETARY INFORMATION
//     This software is supplied under the tems of a license agreement or
//     nondisclosure agreement with Intel Corporation and may not be copied
//     or disclosed except in accordance with the tems of that agreement.
//          Copyright(c) 2005-2007 Intel Corporation. All Rights Reserved.
//
//     Intel(R) Integrated Performance Primitives EC Sample
//
//  By downloading and installing this sample, you hereby agree that the
//  accompanying Materials are being provided to you under the terms and
//  conditions of the End User License Agreement for the Intel(R) Integrated
//  Performance Primitives product previously accepted by you. Please refer
//  to the file ippEULA.rtf or ippEULA.txt located in the root directory of your Intel(R) IPP
//  product installation for more information.
//
//
//  Purpose: anti-howler, tested for subband algorithm
//
*/

#include <stdio.h>
#include <stdlib.h>
#include <ipps.h>
#include <ippsc.h>
#include "ec_td_int.h"
/* Tone Disabler */
int toneDisabler(Ipp16s *r_in, Ipp16s *s_in,Ipp16s samplingRate,int frameSize,
                _td_int *td)
{
    int resr, ress;
    Ipp64s lr_in_pwr = 0, ls_in_pwr = 0;
    /* Detect 2100 Hz with periodical phase reversal */
    ippsToneDetect_EC_16s(r_in, frameSize, &resr, td->tdr);
    ippsToneDetect_EC_16s(s_in, frameSize, &ress, td->tds);

    /* Update receive-in signal and send-in signal powers */
    ippsDotProd_16s64s(r_in,r_in,frameSize,&lr_in_pwr);
    ippsDotProd_16s64s(s_in,s_in,frameSize,&ls_in_pwr);

    //ADDWEIGHTED_INT((*r_in_pwr), lr_in_pwr, td->td_coeff);
    //ADDWEIGHTED_INT((*s_in_pwr), ls_in_pwr, td->td_coeff);

    if ((td->td_ress) || (td->td_resr)) {
        if ((lr_in_pwr < td->td_thres) &&
            (ls_in_pwr < td->td_thres))
        {
            /* Restore previous mode if 250+-100ms signal of<=36dbm0*/
            (td->td_sil_time)=(Ipp16s)((td->td_sil_time)+(frameSize/(samplingRate/1000)));
            if ((td->td_sil_time)>=td->td_sil_thresh) {
                (td->td_resr) = (td->td_ress) = 0;
                return 0;
            }
        }
        return 1;
    } else{
        (td->td_sil_time)=0;
        if (resr || ress) {
            /* Zero coeffs, disable adaptation and NLP */
            if (resr) (td->td_resr) = 1;
            if (ress) (td->td_ress) = 1;
            return 1;
        }
    }

    return 0;
}

