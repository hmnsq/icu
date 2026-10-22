// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 2009-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*/

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/icudataver.h"
#అన్తర్భావ "unicode/ures.h"
#అన్తర్భావ "uresimp.h" /* for ures_getVersionByKey */

U_CAPI శూన్య U_EXPORT2 u_get_దత్తాంశం_సంస్కరణ(UVersionInfo దత్తాంశం_సంస్కరణ_Fillin, UError_సంకేతం *స్థితి_2) {
    UResourceBundle *icudatares = శూన్యసూచిక;
    
    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
        తిరిగి;
    }
    
    ఒకవేళ (దత్తాంశం_సంస్కరణ_Fillin != శూన్యసూచిక) {
        icudatares = ures_open_ప్రత్యక్ష(శూన్యసూచిక, U_ICU_సంస్కరణ_BUNDLE , స్థితి_2);
        ఒకవేళ (U_SUCCESS(*స్థితి_2)) {
            ures_get_సంస్కరణ_By_కీలకం(icudatares, U_ICU_దత్తాంశం_కీలకం, దత్తాంశం_సంస్కరణ_Fillin, స్థితి_2);
        }
        ures_close(icudatares);
    }
}
