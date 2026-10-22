// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 1997-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  loclikely.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010feb25
*   created by: Markus W. Scherer
*
*   Code for miscellaneous locale-related resource bundle data access,
*   separated out from other .cpp files
*   that then do not depend on resource bundle code and this data.
*/

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/putil.h"
#అన్తర్భావ "unicode/uloc.h"
#అన్తర్భావ "unicode/ures.h"
#అన్తర్భావ "charstr.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "ulocimp.h"
#అన్తర్భావ "uresimp.h"

/*
 * Lookup a resource bundle table item with fallback on the table level.
 * Regular resource bundle lookups perform fallback to parent locale bundles
 * and eventually the root bundle, but only for top-level items.
 * This function takes the name of a top-level table and of an item in that table
 * and performs a lookup of both, falling back until a bundle contains a table
 * with this item.
 *
 * Note: Only the opening of entire bundles falls back through the default locale
 * before root. Once a bundle is open, item lookups do not go through the
 * default locale because that would result in a mix of languages that is
 * unpredictable to the programmer and most likely useless.
 */
U_CAPI స్థిరరాశి char16_t * U_EXPORT2
uloc_get_పట్టిక_String_With_Fallback(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *locale,
                              స్థిరరాశి అక్షరం *పట్టిక_కీలకం, స్థిరరాశి అక్షరం *sub_పట్టిక_కీలకం,
                              స్థిరరాశి అక్షరం *అంశం_కీలకం,
                              int32_t *p_నిడివి,
                              UError_సంకేతం *p_Error_సంకేతం)
{
    ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) { తిరిగి శూన్యసూచిక; }
/*    char localeBuffer[ULOC_FULLNAME_CAPACITY*4];*/
    స్థిరరాశి char16_t *అంశం=శూన్యసూచిక;
    UError_సంకేతం error_సంకేతం;

    /*
     * open the bundle for the current locale
     * this falls back through the locale's chain to root
     */
    error_సంకేతం=U_ZERO_ERROR;
    icu::Local_UResource_Bundle_సూచిక rb(ures_open(మార్గం, locale, &error_సంకేతం));

    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        /* total failure, not even root could be opened */
        *p_Error_సంకేతం=error_సంకేతం;
        తిరిగి శూన్యసూచిక;
    } లేకపోతే ఒకవేళ(error_సంకేతం==U_USING_DEFAULT_WARNING ||
                (error_సంకేతం==U_USING_FALLBACK_WARNING && *p_Error_సంకేతం!=U_USING_DEFAULT_WARNING)
    ) {
        /* set the "strongest" error code (success->fallback->default->failure) */
        *p_Error_సంకేతం=error_సంకేతం;
    }

    కొరకు(;;){
        icu::దొంతర_UResource_Bundle పట్టిక;
        icu::దొంతర_UResource_Bundle sub_పట్టిక;
        ures_get_By_కీలకం_With_Fallback(rb.getAlias(), పట్టిక_కీలకం, పట్టిక.getAlias(), &error_సంకేతం);

        ఒకవేళ (sub_పట్టిక_కీలకం != శూన్యసూచిక) {
            /*
            ures_getByKeyWithFallback(table.getAlias(), subTableKey, subTable.getAlias(), &errorCode);
            item = ures_getStringByKeyWithFallback(subTable.getAlias(), itemKey, pLength, &errorCode);
            if(U_FAILURE(errorCode)){
                *pErrorCode = errorCode;
            }
            
            break;*/
            
            ures_get_By_కీలకం_With_Fallback(పట్టిక.getAlias(), sub_పట్టిక_కీలకం, పట్టిక.getAlias(), &error_సంకేతం);
        }
        ఒకవేళ(U_SUCCESS(error_సంకేతం)){
            అంశం = ures_get_String_By_కీలకం_With_Fallback(పట్టిక.getAlias(), అంశం_కీలకం, p_నిడివి, &error_సంకేతం);
            ఒకవేళ(U_FAILURE(error_సంకేతం)){
                స్థిరరాశి అక్షరం* replacement = శూన్యసూచిక;
                *p_Error_సంకేతం = error_సంకేతం; /*save the errorCode*/
                error_సంకేతం = U_ZERO_ERROR;
                /* may be a deprecated code */
                ఒకవేళ(uprv_strcmp(పట్టిక_కీలకం, "Countries")==0){
                    replacement =  uloc_getCurrentCountryID(అంశం_కీలకం);
                }లేకపోతే ఒకవేళ(uprv_strcmp(పట్టిక_కీలకం, "Languages")==0){
                    replacement =  uloc_getCurrentLanguageID(అంశం_కీలకం);
                }
                /*pointer comparison is ok since uloc_getCurrentCountryID & uloc_getCurrentLanguageID return the key itself is replacement is not found*/
                ఒకవేళ(replacement!=శూన్యసూచిక && అంశం_కీలకం != replacement){
                    అంశం = ures_get_String_By_కీలకం_With_Fallback(పట్టిక.getAlias(), replacement, p_నిడివి, &error_సంకేతం);
                    ఒకవేళ(U_SUCCESS(error_సంకేతం)){
                        *p_Error_సంకేతం = error_సంకేతం;
                        ఆపు;
                    }
                }
            }లేకపోతే{
                ఆపు;
            }
        }
        
        ఒకవేళ(U_FAILURE(error_సంకేతం)){    

            /* still can't figure out ?.. try the fallback mechanism */
            int32_t len = 0;
            స్థిరరాశి char16_t* fallbackLocale =  శూన్యసూచిక;
            *p_Error_సంకేతం = error_సంకేతం;
            error_సంకేతం = U_ZERO_ERROR;

            fallbackLocale = ures_get_String_By_కీలకం_With_Fallback(పట్టిక.getAlias(), "Fallback", &len, &error_సంకేతం);
            ఒకవేళ(U_FAILURE(error_సంకేతం)){
               *p_Error_సంకేతం = error_సంకేతం;
                ఆపు;
            }

            icu::CharString explicitFallbackName;
            explicitFallbackName.appendInvariantChars(fallbackLocale, len, error_సంకేతం);

            /* guard against recursive fallback */
            ఒకవేళ (explicitFallbackName == locale) {
                *p_Error_సంకేతం = U_అంతర్గత_కార్యక్రమం_ERROR;
                ఆపు;
            }
            rb.adoptInstead(ures_open(మార్గం, explicitFallbackName.దత్తాంశం(), &error_సంకేతం));
            ఒకవేళ(U_FAILURE(error_సంకేతం)){
                *p_Error_సంకేతం = error_సంకేతం;
                ఆపు;
            }
            /* succeeded in opening the fallback bundle .. continue and try to fetch the item */
        }లేకపోతే{
            ఆపు;
        }
    }

    తిరిగి అంశం;
}

నామాకాశ {

ULayoutType
_uloc_getOrientationHelper(స్థిరరాశి అక్షరం* localeId,
                           స్థిరరాశి అక్షరం* కీలకం,
                           UError_సంకేతం& స్థితి_2)
{
    ULayoutType ఫలితం = ULOC_LAYOUT_UNKNOWN;

    ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి ఫలితం; }

    ఒకవేళ (localeId == శూన్యసూచిక) {
        localeId = uloc_getDefault();
    }
    icu::CharString locale_బఫరు = ulocimp_canonicalize(localeId, స్థితి_2);

    ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి ఫలితం; }

    int32_t length = 0;
    స్థిరరాశి char16_t* స్థిరరాశి value =
        uloc_get_పట్టిక_String_With_Fallback(
            శూన్యసూచిక,
            locale_బఫరు.దత్తాంశం(),
            "layout",
            శూన్యసూచిక,
            కీలకం,
            &length,
            &స్థితి_2);

    ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి ఫలితం; }

    ఒకవేళ (length != 0) {
        ఎంపిక(value[0])
        {
        స్థితి 0x0062: /* 'b' */
            ఫలితం = ULOC_LAYOUT_BTT;
            ఆపు;
        స్థితి 0x006C: /* 'l' */
            ఫలితం = ULOC_LAYOUT_LTR;
            ఆపు;
        స్థితి 0x0072: /* 'r' */
            ఫలితం = ULOC_LAYOUT_RTL;
            ఆపు;
        స్థితి 0x0074: /* 't' */
            ఫలితం = ULOC_LAYOUT_TTB;
            ఆపు;
        అప్రమేయ:
            స్థితి_2 = U_అంతర్గత_కార్యక్రమం_ERROR;
            ఆపు;
        }
    }

    తిరిగి ఫలితం;
}

}  // namespace

U_CAPI ULayoutType U_EXPORT2
uloc_getCharacterOrientation(స్థిరరాశి అక్షరం* localeId,
                             UError_సంకేతం *స్థితి_2)
{
    తిరిగి _uloc_getOrientationHelper(localeId, "characters", *స్థితి_2);
}

/**
 * Get the layout line orientation for the specified locale.
 * 
 * @param localeID locale name
 * @param status Error status
 * @return an enum indicating the layout orientation for lines.
 */
U_CAPI ULayoutType U_EXPORT2
uloc_get_పంక్తి_Orientation(స్థిరరాశి అక్షరం* localeId,
                        UError_సంకేతం *స్థితి_2)
{
    తిరిగి _uloc_getOrientationHelper(localeId, "lines", *స్థితి_2);
}
