// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 2012,2014 International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*/

/**
 * \file
 * \brief C++: internal template EnumSet<>
 */

#నిర్వచితం_కాకపోతే ENUMSET_H
#నిర్వచించు ENUMSET_H

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

/* Can't use #ifndef U_HIDE_INTERNAL_API for the entire EnumSet class, needed in .h file declarations */
/**
 * enum bitset for boolean fields. Similar to Java EnumSet<>. 
 * Needs to range check. Used for private instance variables.
 * @internal
 * \cond
 */
మూస<typename T, uint32_t min_విలువ, uint32_t హద్దు_విలువ>
class Enum_సమితి {
బహిరంగ:
    అంతఃవరుస Enum_సమితి() : fBools(0) {}
    అంతఃవరుస Enum_సమితి(స్థిరరాశి Enum_సమితి<T,min_విలువ,హద్దు_విలువ>& other) : fBools(other.fBools) {}
    అంతఃవరుస ~Enum_సమితి() {}
#నిర్వచితం_కాకపోతే U_HIDE_అంతర్గత_API
    అంతఃవరుస శూన్య clear() { fBools=0; }
    అంతఃవరుస శూన్య కూడు(T గమ్యం_కూడు) { సమితి(గమ్యం_కూడు, 1); }
    అంతఃవరుస శూన్య remove(T గమ్యం_Remove) { సమితి(గమ్యం_Remove, 0); }
    అంతఃవరుస int32_t కలిగియుంది(T గమ్యం_Check) స్థిరరాశి { తిరిగి get(గమ్యం_Check); }
    అంతఃవరుస శూన్య సమితి(T గమ్యం_సమితి, int32_t v) { fBools=(fBools&(~flag(గమ్యం_సమితి)))|(v?(flag(గమ్యం_సమితి)):0); }
    అంతఃవరుస int32_t get(T గమ్యం_Check) స్థిరరాశి { తిరిగి (fBools & flag(గమ్యం_Check))?1:0; }
    అంతఃవరుస UBool isValidEnum(T గమ్యం_Check) స్థిరరాశి {  తిరిగి (గమ్యం_Check>=min_విలువ&&గమ్యం_Check<హద్దు_విలువ); }
    అంతఃవరుస UBool is_Valid_విలువ(int32_t v) స్థిరరాశి { తిరిగి (v==0||v==1); }
    అంతఃవరుస స్థిరరాశి Enum_సమితి<T, min_విలువ, హద్దు_విలువ>&
    క్రియాకారకం=(స్థిరరాశి Enum_సమితి<T, min_విలువ, హద్దు_విలువ>& other) U_LIFETIME_BOUND {
        fBools = other.fBools;
        తిరిగి *this;
    }
  
    అంతఃవరుస uint32_t getAll() స్థిరరాశి {
        తిరిగి fBools; 
    }
#అయితే_ముగింపు /* U_HIDE_INTERNAL_API */

అంతరంగ:
    అంతఃవరుస uint32_t flag(T గమ్యం_Check) స్థిరరాశి { తిరిగి (1<<(గమ్యం_Check-min_విలువ)); }
అంతరంగ:
    uint32_t fBools;
};

/** \endcond */

U_NAMESPACE_చివర

#అయితే_ముగింపు /* U_SHOW_CPLUSPLUS_API */
#అయితే_ముగింపు /* ENUMSET_H */
