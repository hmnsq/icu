// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *******************************************************************************
 *
 *   Copyright (C) 2003-2006, International Business Machines
 *   Corporation and others.  All Rights Reserved.
 *
 *******************************************************************************
 *   file name:  sprpimpl.h
 *   encoding:   UTF-8
 *   tab size:   8 (not used)
 *   indentation:4
 *
 *   created on: 2003feb1
 *   created by: Ram Viswanadha
 */

#నిర్వచితం_కాకపోతే SPRPIMPL_H
#నిర్వచించు SPRPIMPL_H

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_IDNA

#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/parseerr.h"
#అన్తర్భావ "unicode/usprep.h"
#అన్తర్భావ "unicode/udata.h"
#అన్తర్భావ "utrie.h"
#అన్తర్భావ "udataswp.h"
#అన్తర్భావ "ubidi_props.h"

#నిర్వచించు SPREP_దత్తాంశం_TYPE "spp"

గణన UStringPrepType{
    USPREP_UNASSIGNED           = 0x0000 ,
    USPREP_MAP                  = 0x0001 ,
    USPREP_PROHIBITED           = 0x0002 , 
    USPREP_DELETE               = 0x0003 ,
    USPREP_TYPE_హద్దు           = 0x0004  
};

రకనిర్వచనం గణన UStringPrepType UStringPrepType;

#నిర్వచితమైతే USPREP_TYPE_పేర్లు_ARRAY
స్థిర స్థిరరాశి అక్షరం* usprep_Type_పేర్లు[] ={
    "UNASSIGNED" ,          
    "MAP" , 
    "PROHIBITED" ,        
    "DELETE",
    "TYPE_LIMIT" 
};
#అయితే_ముగింపు

గణన{
    _SPREP_NORMALIZATION_ON = 0x0001,
    _SPREP_CHECK_BIDI_ON    = 0x0002
};

గణన{
    SPREP_TYPE_పరిమితి       = 0xFFF0,
    SPREP_MAX_సూచీ_విలువ      = 0x3FBF,   /*16139*/ 
    SPREP_MAX_సూచీ_శిఖరం_నిడివి = 0x0003
};

/* indexes[] value names */
గణన {
    SPREP_సూచీ_TRIE_కొలత                  = 0, /* number of bytes in StringPrep trie */
    SPREP_సూచీ_MAPPING_దత్తాంశం_కొలత          = 1, /* The array that contains the mapping   */
    SPREP_NORM_CORRECTNS_చివరి_UNI_సంస్కరణ  = 2, /* The index of Unicode version of last entry in NormalizationCorrections.txt */ 
    SPREP_ONE_UCHAR_MAPPING_సూచీ_ప్రారంభం    = 3, /* The starting index of 1 UChar mapping index in the mapping data array */
    SPREP_TWO_UCHARS_MAPPING_సూచీ_ప్రారంభం   = 4, /* The starting index of 2 UChars mapping index in the mapping data array */
    SPREP_THREE_UCHARS_MAPPING_సూచీ_ప్రారంభం = 5, /* The starting index of 3 UChars mapping index in the mapping data array */
    SPREP_FOUR_UCHARS_MAPPING_సూచీ_ప్రారంభం  = 6, /* The starting index of 4 UChars mapping index in the mapping data array */
    _SPREP_OPTIONS                          = 7, /* Bit set of options to turn on in the profile */
    SPREP_సూచీ_శిఖరం=16                          /* changing this requires a new formatVersion */
};

రకనిర్వచనం నిర్మాణం UString_Prep_కీలకం UString_Prep_కీలకం;


నిర్మాణం UString_Prep_కీలకం{
    అక్షరం* name;
    అక్షరం* మార్గం;
};

నిర్మాణం UStringPrepProfile{
    int32_t indexes[SPREP_సూచీ_శిఖరం];
    UTrie sprepTrie;
    స్థిరరాశి uint16_t* mapping_దత్తాంశం;
    UData_జ్ఞాపకం* sprep_దత్తాంశం;
    int32_t ref_లెక్క;
    UBool is_దత్తాంశం_Loaded;
    UBool doNFKC;
    UBool checkBiDi;
};

/**
 * Helper function for populating the UParseError struct
 * @internal
 */
U_CAPI శూన్య U_EXPORT2
uprv_syntaxError(స్థిరరాశి UChar* నియమాలు, 
                 int32_t pos,
                 int32_t నియమాలు_Len,
                 UParseError* parseError);


/**
 * Swap StringPrep .spp profile data. See udataswp.h.
 * @internal
 */
U_CAPI int32_t U_EXPORT2
usprep_మార్పిడి(స్థిరరాశి UDataSwapper *ds,
            స్థిరరాశి శూన్య *లోపల_దత్తాంశం, int32_t నిడివి, శూన్య *బహిర్గతం_దత్తాంశం,
            UError_సంకేతం *p_Error_సంకేతం);

#అయితే_ముగింపు /* #if !UCONFIG_NO_IDNA */

#అయితే_ముగింపు

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
