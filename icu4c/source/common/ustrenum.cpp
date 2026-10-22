// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2014, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
* Author: Alan Liu
* Created: November 11 2002
* Since: ICU 2.4
**********************************************************************
*/
#అన్తర్భావ "utypeinfo.h"  // for 'typeid' to work 

#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/strenum.h"
#అన్తర్భావ "unicode/putil.h"
#అన్తర్భావ "uenumimp.h"
#అన్తర్భావ "ustrenum.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "uassert.h"

U_NAMESPACE_BEGIN
// StringEnumeration implementation ---------------------------------------- ***

StringEnumeration::StringEnumeration()
    : chars(chars_బఫరు), chars_సామర్థ్యం(పరిమాణం(chars_బఫరు)) {
}

StringEnumeration::~StringEnumeration() {
    ఒకవేళ (chars != శూన్యసూచిక && chars != chars_బఫరు) {
        uprv_free(chars);
    }
}

// StringEnumeration base class clone() default implementation, does not clone
StringEnumeration *
StringEnumeration::clone() స్థిరరాశి {
  తిరిగి శూన్యసూచిక;
}

స్థిరరాశి అక్షరం*
StringEnumeration::next(int32_t* ఫలితం_నిడివి, UError_సంకేతం& స్థితి_2) U_LIFETIME_BOUND {
    స్థిరరాశి UnicodeString *s=snext(స్థితి_2);
    ఒకవేళ(U_SUCCESS(స్థితి_2) && s!=శూన్యసూచిక) {
        unistr=*s;
        ensure_Chars_సామర్థ్యం(unistr.length()+1, స్థితి_2);
        ఒకవేళ(U_SUCCESS(స్థితి_2)) {
            ఒకవేళ(ఫలితం_నిడివి!=శూన్యసూచిక) {
                *ఫలితం_నిడివి=unistr.length();
            }
            unistr.extract(0, INT32_MAX, chars, chars_సామర్థ్యం, US_INV);
            తిరిగి chars;
        }
    }

    తిరిగి శూన్యసూచిక;
}

స్థిరరాశి char16_t*
StringEnumeration::unext(int32_t* ఫలితం_నిడివి, UError_సంకేతం& స్థితి_2) U_LIFETIME_BOUND {
    స్థిరరాశి UnicodeString *s=snext(స్థితి_2);
    ఒకవేళ(U_SUCCESS(స్థితి_2) && s!=శూన్యసూచిక) {
        unistr=*s;
        ఒకవేళ(ఫలితం_నిడివి!=శూన్యసూచిక) {
            *ఫలితం_నిడివి=unistr.length();
        }
        తిరిగి unistr.get_Terminated_బఫరు();
    }

    తిరిగి శూన్యసూచిక;
}

స్థిరరాశి UnicodeString*
StringEnumeration::snext(UError_సంకేతం& స్థితి_2) U_LIFETIME_BOUND {
    int32_t length;
    స్థిరరాశి అక్షరం *s=next(&length, స్థితి_2);
    తిరిగి సమితి_Chars(s, length, స్థితి_2);
}

శూన్య
StringEnumeration::ensure_Chars_సామర్థ్యం(int32_t capacity, UError_సంకేతం &స్థితి_2) {
    ఒకవేళ(U_SUCCESS(స్థితి_2) && capacity>chars_సామర్థ్యం) {
        ఒకవేళ(capacity<(chars_సామర్థ్యం+chars_సామర్థ్యం/2)) {
            // avoid allocation thrashing
            capacity=chars_సామర్థ్యం+chars_సామర్థ్యం/2;
        }
        ఒకవేళ(chars!=chars_బఫరు) {
            uprv_free(chars);
        }
        chars = static_cast<అక్షరం*>(uprv_malloc(capacity));
        ఒకవేళ(chars==శూన్యసూచిక) {
            chars=chars_బఫరు;
            chars_సామర్థ్యం=పరిమాణం(chars_బఫరు);
            స్థితి_2=U_జ్ఞాపకం_కేటాయింపు_ERROR;
        } లేకపోతే {
            chars_సామర్థ్యం=capacity;
        }
    }
}

UnicodeString*
StringEnumeration::సమితి_Chars(స్థిరరాశి అక్షరం* s, int32_t length, UError_సంకేతం& స్థితి_2) U_LIFETIME_BOUND {
    ఒకవేళ(U_SUCCESS(స్థితి_2) && s!=శూన్యసూచిక) {
        ఒకవేళ(length<0) {
            length = static_cast<int32_t>(uprv_strlen(s));
        }

        char16_t *బఫరు=unistr.get_బఫరు(length+1);
        ఒకవేళ(బఫరు!=శూన్యసూచిక) {
            u_chars_గమ్యం_UChars(s, బఫరు, length);
            బఫరు[length]=0;
            unistr.release_బఫరు(length);
            తిరిగి &unistr;
        } లేకపోతే {
            స్థితి_2=U_జ్ఞాపకం_కేటాయింపు_ERROR;
        }
    }

    తిరిగి శూన్యసూచిక;
}
తార్కిక
StringEnumeration::క్రియాకారకం==(స్థిరరాశి StringEnumeration& that)స్థిరరాశి {
    తిరిగి typeid(*ఇది) == typeid(that); 
}

తార్కిక
StringEnumeration::క్రియాకారకం!=(స్థిరరాశి StringEnumeration& that)స్థిరరాశి {
    తిరిగి !క్రియాకారకం==(that);
}

// UStringEnumeration implementation --------------------------------------- ***

UStringEnumeration * U_EXPORT2
UStringEnumeration::fromUEnumeration(
        UEnumeration *uenum_గమ్యం_Adopt, UError_సంకేతం &స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        uenum_close(uenum_గమ్యం_Adopt);
        తిరిగి శూన్యసూచిక;
    }
    UStringEnumeration *ఫలితం = కొత్త UStringEnumeration(uenum_గమ్యం_Adopt);
    ఒకవేళ (ఫలితం == శూన్యసూచిక) {
        స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        uenum_close(uenum_గమ్యం_Adopt);
        తిరిగి శూన్యసూచిక;
    }
    తిరిగి ఫలితం;
}

UStringEnumeration::UStringEnumeration(UEnumeration* _uenum) :
    uenum(_uenum) {
    U_ASSERT(_uenum != 0);
}

UStringEnumeration::~UStringEnumeration() {
    uenum_close(uenum);
}

int32_t UStringEnumeration::count(UError_సంకేతం& స్థితి_2) స్థిరరాశి {
    తిరిగి uenum_లెక్క(uenum, &స్థితి_2);
}

స్థిరరాశి అక్షరం* UStringEnumeration::next(int32_t* ఫలితం_నిడివి, UError_సంకేతం& స్థితి_2) U_LIFETIME_BOUND {
    తిరిగి uenum_తదుపరి(uenum, ఫలితం_నిడివి, &స్థితి_2);
}

స్థిరరాశి UnicodeString* UStringEnumeration::snext(UError_సంకేతం& స్థితి_2) U_LIFETIME_BOUND {
    int32_t length;
    స్థిరరాశి char16_t* str = uenum_unext(uenum, &length, &స్థితి_2);
    ఒకవేళ (str == శూన్యసూచిక || U_FAILURE(స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }
    తిరిగి &unistr.సమితి_గమ్యం(str, length);
}

శూన్య UStringEnumeration::reset(UError_సంకేతం& స్థితి_2) {
    uenum_reset(uenum, &స్థితి_2);
}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UStringEnumeration)
U_NAMESPACE_చివర

// C wrapper --------------------------------------------------------------- ***

#నిర్వచించు THIS(en) ((icu::StringEnumeration*)(en->context))

U_CDECL_BEGIN

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
స్థిర శూన్య U_CALLCONV
ustrenum_close(UEnumeration* en) {
    తొలగించు THIS(en);
    uprv_free(en);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
స్థిర int32_t U_CALLCONV
ustrenum_లెక్క(UEnumeration* en,
               UError_సంకేతం* ec)
{
    తిరిగి THIS(en)->count(*ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
స్థిర స్థిరరాశి char16_t* U_CALLCONV
ustrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
               int32_t* ఫలితం_నిడివి,
               UError_సంకేతం* ec)
{
    తిరిగి THIS(en)->unext(ఫలితం_నిడివి, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
స్థిర స్థిరరాశి అక్షరం* U_CALLCONV
ustrenum_తదుపరి(UEnumeration* en U_LIFETIME_BOUND,
              int32_t* ఫలితం_నిడివి,
              UError_సంకేతం* ec)
{
    తిరిగి THIS(en)->next(ఫలితం_నిడివి, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
స్థిర శూన్య U_CALLCONV
ustrenum_reset(UEnumeration* en,
               UError_సంకేతం* ec)
{
    THIS(en)->reset(*ec);
}

/**
 * Pseudo-vtable for UEnumeration wrapper around StringEnumeration.
 * The StringEnumeration pointer will be stored in 'context'.
 */
స్థిర స్థిరరాశి UEnumeration USTRENUM_VT = {
    శూన్యసూచిక,
    శూన్యసూచిక, // store StringEnumeration pointer here
    ustrenum_close,
    ustrenum_లెక్క,
    ustrenum_unext,
    ustrenum_తదుపరి,
    ustrenum_reset
};

U_CDECL_చివర

/**
 * Given a StringEnumeration, wrap it in a UEnumeration.  The
 * StringEnumeration is adopted; after this call, the caller must not
 * delete it (regardless of error status).
 */
U_CAPI UEnumeration* U_EXPORT2
uenum_openFromStringEnumeration(icu::StringEnumeration* adopted, UError_సంకేతం* ec) { 
    UEnumeration* ఫలితం = శూన్యసూచిక;
    ఒకవేళ (U_SUCCESS(*ec) && adopted != శూన్యసూచిక) {
        ఫలితం = (UEnumeration*) uprv_malloc(పరిమాణం(UEnumeration));
        ఒకవేళ (ఫలితం == శూన్యసూచిక) {
            *ec = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        } లేకపోతే {
            uprv_memcpy(ఫలితం, &USTRENUM_VT, పరిమాణం(USTRENUM_VT));
            ఫలితం->context = adopted;
        }
    }
    ఒకవేళ (ఫలితం == శూన్యసూచిక) {
        తొలగించు adopted;
    }
    తిరిగి ఫలితం;
}

// C wrapper --------------------------------------------------------------- ***

U_CDECL_BEGIN

రకనిర్వచనం నిర్మాణం UCharStringEnumeration {
    UEnumeration uenum;
    int32_t సూచీ, count;
} UCharStringEnumeration;

స్థిర శూన్య U_CALLCONV
ucharstrenum_close(UEnumeration* en) {
    uprv_free(en);
}

స్థిర int32_t U_CALLCONV
ucharstrenum_లెక్క(UEnumeration* en,
                   UError_సంకేతం* /*ec*/) {
    తిరిగి ((UCharStringEnumeration*)en)->count;
}

స్థిర స్థిరరాశి char16_t* U_CALLCONV
ucharstrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* ఫలితం_నిడివి,
                  UError_సంకేతం* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    ఒకవేళ (e->సూచీ >= e->count) {
        తిరిగి శూన్యసూచిక;
    }
    స్థిరరాశి char16_t* ఫలితం = ((స్థిరరాశి char16_t**)e->uenum.context)[e->సూచీ++];
    ఒకవేళ (ఫలితం_నిడివి) {
        *ఫలితం_నిడివి = u_strlen(ఫలితం);
    }
    తిరిగి ఫలితం;
}


స్థిర స్థిరరాశి అక్షరం* U_CALLCONV
ucharstrenum_తదుపరి(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* ఫలితం_నిడివి,
                  UError_సంకేతం* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    ఒకవేళ (e->సూచీ >= e->count) {
        తిరిగి శూన్యసూచిక;
    }
    స్థిరరాశి అక్షరం* ఫలితం = ((స్థిరరాశి అక్షరం**)e->uenum.context)[e->సూచీ++];
    ఒకవేళ (ఫలితం_నిడివి) {
        *ఫలితం_నిడివి = (int32_t)uprv_strlen(ఫలితం);
    }
    తిరిగి ఫలితం;
}

స్థిర శూన్య U_CALLCONV
ucharstrenum_reset(UEnumeration* en,
                   UError_సంకేతం* /*ec*/) {
    ((UCharStringEnumeration*)en)->సూచీ = 0;
}

స్థిర స్థిరరాశి UEnumeration UCHARSTRENUM_VT = {
    శూన్యసూచిక,
    శూన్యసూచిక, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_లెక్క,
    uenum_unextDefault,
    ucharstrenum_తదుపరి,
    ucharstrenum_reset
};

స్థిర స్థిరరాశి UEnumeration UCHARSTRENUM_U_VT = {
    శూన్యసూచిక,
    శూన్యసూచిక, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_లెక్క,
    ucharstrenum_unext,
    uenum_తదుపరి_Default,
    ucharstrenum_reset
};

U_CDECL_చివర

U_CAPI UEnumeration* U_EXPORT2
uenum_open_Char_అక్షరశ్రేణులు_Enumeration(U_LIFETIME_BOUND స్థిరరాశి అక్షరం* స్థిరరాశి అక్షరశ్రేణులు[],
                                 int32_t count,
                                 UError_సంకేతం* ec) {
    UCharStringEnumeration* ఫలితం = శూన్యసూచిక;
    ఒకవేళ (U_SUCCESS(*ec) && count >= 0 && (count == 0 || అక్షరశ్రేణులు != శూన్యసూచిక)) {
        ఫలితం = (UCharStringEnumeration*) uprv_malloc(పరిమాణం(UCharStringEnumeration));
        ఒకవేళ (ఫలితం == శూన్యసూచిక) {
            *ec = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        } లేకపోతే {
            U_ASSERT((అక్షరం*)ఫలితం==(అక్షరం*)(&ఫలితం->uenum));
            uprv_memcpy(ఫలితం, &UCHARSTRENUM_VT, పరిమాణం(UCHARSTRENUM_VT));
            ఫలితం->uenum.context = (శూన్య*)అక్షరశ్రేణులు;
            ఫలితం->సూచీ = 0;
            ఫలితం->count = count;
        }
    }
    తిరిగి (UEnumeration*) ఫలితం;
}

U_CAPI UEnumeration* U_EXPORT2
uenum_open_UChar_అక్షరశ్రేణులు_Enumeration(U_LIFETIME_BOUND స్థిరరాశి char16_t* స్థిరరాశి అక్షరశ్రేణులు[],
                                  int32_t count,
                                  UError_సంకేతం* ec) {
    UCharStringEnumeration* ఫలితం = శూన్యసూచిక;
    ఒకవేళ (U_SUCCESS(*ec) && count >= 0 && (count == 0 || అక్షరశ్రేణులు != శూన్యసూచిక)) {
        ఫలితం = (UCharStringEnumeration*) uprv_malloc(పరిమాణం(UCharStringEnumeration));
        ఒకవేళ (ఫలితం == శూన్యసూచిక) {
            *ec = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        } లేకపోతే {
            U_ASSERT((అక్షరం*)ఫలితం==(అక్షరం*)(&ఫలితం->uenum));
            uprv_memcpy(ఫలితం, &UCHARSTRENUM_U_VT, పరిమాణం(UCHARSTRENUM_U_VT));
            ఫలితం->uenum.context = (శూన్య*)అక్షరశ్రేణులు;
            ఫలితం->సూచీ = 0;
            ఫలితం->count = count;
        }
    }
    తిరిగి (UEnumeration*) ఫలితం;
}


// end C Wrapper
