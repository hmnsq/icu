// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2014, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
* Author: Alan Liu
* Created: October 30 2002
* Since: ICU 2.4
* 2010nov19 Markus Scherer  Rewrite for formatVersion 2.
**********************************************************************
*/
#అన్తర్భావ "propname.h"
#అన్తర్భావ "unicode/uchar.h"
#అన్తర్భావ "unicode/udata.h"
#అన్తర్భావ "unicode/uscript.h"
#అన్తర్భావ "umutex.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "uarrsort.h"
#అన్తర్భావ "uinvchar.h"

#నిర్వచించు INCLUDED_FROM_PROPNAME_CPP
#అన్తర్భావ "propname_data.h"

U_CDECL_BEGIN

/**
 * Get the next non-ignorable ASCII character from a property name
 * and lowercases it.
 * @return ((advance count for the name)<<8)|character
 */
స్థిర అంతఃవరుస int32_t
getASCIIPropertyNameChar(స్థిరరాశి అక్షరం *name) {
    int32_t i;
    అక్షరం c;

    /* Ignore delimiters '-', '_', and ASCII White_Space */
    కొరకు(i=0;
        (c=name[i++])==0x2d || c==0x5f ||
        c==0x20 || (0x09<=c && c<=0x0d);
    ) {}

    ఒకవేళ(c!=0) {
        తిరిగి (i << 8) | static_cast<uint8_t>(uprv_asciitolower(c));
    } లేకపోతే {
        తిరిగి i<<8;
    }
}

/**
 * Get the next non-ignorable EBCDIC character from a property name
 * and lowercases it.
 * @return ((advance count for the name)<<8)|character
 */
స్థిర అంతఃవరుస int32_t
getEBCDICPropertyNameChar(స్థిరరాశి అక్షరం *name) {
    int32_t i;
    అక్షరం c;

    /* Ignore delimiters '-', '_', and EBCDIC White_Space */
    కొరకు(i=0;
        (c=name[i++])==0x60 || c==0x6d ||
        c==0x40 || c==0x05 || c==0x15 || c==0x25 || c==0x0b || c==0x0c || c==0x0d;
    ) {}

    ఒకవేళ(c!=0) {
        తిరిగి (i << 8) | static_cast<uint8_t>(uprv_ebcdictolower(c));
    } లేకపోతే {
        తిరిగి i<<8;
    }
}

/**
 * Unicode property names and property value names are compared "loosely".
 *
 * UCD.html 4.0.1 says:
 *   For all property names, property value names, and for property values for
 *   Enumerated, Binary, or Catalog properties, use the following
 *   loose matching rule:
 *
 *   LM3. Ignore case, whitespace, underscore ('_'), and hyphens.
 *
 * This function does just that, for (char *) name strings.
 * It is almost identical to ucnv_compareNames() but also ignores
 * C0 White_Space characters (U+0009..U+000d, and U+0085 on EBCDIC).
 *
 * @internal
 */

U_CAPI int32_t U_EXPORT2
uprv_compare_ASCIIProperty_పేర్లు(స్థిరరాశి అక్షరం *name1, స్థిరరాశి అక్షరం *name2) {
    int32_t rc, r1, r2;

    కొరకు(;;) {
        r1=getASCIIPropertyNameChar(name1);
        r2=getASCIIPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        ఒకవేళ(((r1|r2)&0xff)==0) {
            తిరిగి 0;
        }

        /* Compare the lowercased characters */
        ఒకవేళ(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            ఒకవేళ(rc!=0) {
                తిరిగి rc;
            }
        }

        name1+=r1>>8;
        name2+=r2>>8;
    }
}

U_CAPI int32_t U_EXPORT2
uprv_compare_EBCDICProperty_పేర్లు(స్థిరరాశి అక్షరం *name1, స్థిరరాశి అక్షరం *name2) {
    int32_t rc, r1, r2;

    కొరకు(;;) {
        r1=getEBCDICPropertyNameChar(name1);
        r2=getEBCDICPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        ఒకవేళ(((r1|r2)&0xff)==0) {
            తిరిగి 0;
        }

        /* Compare the lowercased characters */
        ఒకవేళ(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            ఒకవేళ(rc!=0) {
                తిరిగి rc;
            }
        }

        name1+=r1>>8;
        name2+=r2>>8;
    }
}

U_CDECL_చివర

U_NAMESPACE_BEGIN

int32_t Prop_Name_దత్తాంశం::కనుగొను_Property(int32_t property) {
    int32_t i=1;  // valueMaps index, initially after numRanges
    కొరకు(int32_t అంకె_Ranges=విలువ_Maps[0]; అంకె_Ranges>0; --అంకె_Ranges) {
        // Read and skip the start and limit of this range.
        int32_t ప్రారంభం=విలువ_Maps[i];
        int32_t హద్దు=విలువ_Maps[i+1];
        i+=2;
        ఒకవేళ(property<ప్రారంభం) {
            ఆపు;
        }
        ఒకవేళ(property<హద్దు) {
            తిరిగి i+(property-ప్రారంభం)*2;
        }
        i+=(హద్దు-ప్రారంభం)*2;  // Skip all entries for this range.
    }
    తిరిగి 0;
}

int32_t Prop_Name_దత్తాంశం::కనుగొను_Property_విలువ_Name_సమూహం(int32_t విలువ_Map_సూచీ, int32_t value) {
    ఒకవేళ(విలువ_Map_సూచీ==0) {
        తిరిగి 0;  // The property does not have named values.
    }
    ++విలువ_Map_సూచీ;  // Skip the BytesTrie offset.
    int32_t అంకె_Ranges=విలువ_Maps[విలువ_Map_సూచీ++];
    ఒకవేళ(అంకె_Ranges<0x10) {
        // Ranges of values.
        కొరకు(; అంకె_Ranges>0; --అంకె_Ranges) {
            // Read and skip the start and limit of this range.
            int32_t ప్రారంభం=విలువ_Maps[విలువ_Map_సూచీ];
            int32_t హద్దు=విలువ_Maps[విలువ_Map_సూచీ+1];
            విలువ_Map_సూచీ+=2;
            ఒకవేళ(value<ప్రారంభం) {
                ఆపు;
            }
            ఒకవేళ(value<హద్దు) {
                తిరిగి విలువ_Maps[విలువ_Map_సూచీ+value-ప్రారంభం];
            }
            విలువ_Map_సూచీ+=హద్దు-ప్రారంభం;  // Skip all entries for this range.
        }
    } లేకపోతే {
        // List of values.
        int32_t విలువలు_ప్రారంభం=విలువ_Map_సూచీ;
        int32_t name_సమూహం_Offsets_ప్రారంభం=విలువ_Map_సూచీ+అంకె_Ranges-0x10;
        చేయి {
            int32_t v=విలువ_Maps[విలువ_Map_సూచీ];
            ఒకవేళ(value<v) {
                ఆపు;
            }
            ఒకవేళ(value==v) {
                తిరిగి విలువ_Maps[name_సమూహం_Offsets_ప్రారంభం+విలువ_Map_సూచీ-విలువలు_ప్రారంభం];
            }
        } అయితే(++విలువ_Map_సూచీ<name_సమూహం_Offsets_ప్రారంభం);
    }
    తిరిగి 0;
}

స్థిరరాశి అక్షరం *Prop_Name_దత్తాంశం::getName(స్థిరరాశి అక్షరం *name_సమూహం, int32_t name_సూచీ) {
    int32_t అంకె_పేర్లు=*name_సమూహం++;
    ఒకవేళ(name_సూచీ<0 || అంకె_పేర్లు<=name_సూచీ) {
        తిరిగి శూన్యసూచిక;
    }
    // Skip nameIndex names.
    కొరకు(; name_సూచీ>0; --name_సూచీ) {
        name_సమూహం=uprv_strchr(name_సమూహం, 0)+1;
    }
    ఒకవేళ(*name_సమూహం==0) {
        తిరిగి శూన్యసూచిక;  // no name (Property[Value]Aliases.txt has "n/a")
    }
    తిరిగి name_సమూహం;
}

UBool Prop_Name_దత్తాంశం::కలిగియుంది_Name(బైట్లు_Trie &trie, స్థిరరాశి అక్షరం *name) {
    ఒకవేళ(name==శూన్యసూచిక) {
        తిరిగి అబద్ధం;
    }
    UString_Trie_ఫలితం ఫలితం=USTRINGTRIE_NO_విలువ;
    అక్షరం c;
    అయితే((c=*name++)!=0) {
        c=uprv_inv_Char_గమ్యం_Lowercase_Ascii(c);
        // Ignore delimiters '-', '_', and ASCII White_Space.
        ఒకవేళ(c==0x2d || c==0x5f || c==0x20 || (0x09<=c && c<=0x0d)) {
            కొనసాగు;
        }
        ఒకవేళ(!USTRINGTRIE_HAS_తదుపరి(ఫలితం)) {
            తిరిగి అబద్ధం;
        }
        ఫలితం = trie.next(static_cast<uint8_t>(c));
    }
    తిరిగి USTRINGTRIE_HAS_విలువ(ఫలితం);
}

స్థిరరాశి అక్షరం *Prop_Name_దత్తాంశం::getPropertyName(int32_t property, int32_t nameChoice) {
    int32_t విలువ_Map_సూచీ=కనుగొను_Property(property);
    ఒకవేళ(విలువ_Map_సూచీ==0) {
        తిరిగి శూన్యసూచిక;  // Not a known property.
    }
    తిరిగి getName(nameGroups+విలువ_Maps[విలువ_Map_సూచీ], nameChoice);
}

స్థిరరాశి అక్షరం *Prop_Name_దత్తాంశం::get_Property_విలువ_Name(int32_t property, int32_t value, int32_t nameChoice) {
    int32_t విలువ_Map_సూచీ=కనుగొను_Property(property);
    ఒకవేళ(విలువ_Map_సూచీ==0) {
        తిరిగి శూన్యసూచిక;  // Not a known property.
    }
    int32_t name_సమూహం_స్థానభ్రంశం=కనుగొను_Property_విలువ_Name_సమూహం(విలువ_Maps[విలువ_Map_సూచీ+1], value);
    ఒకవేళ(name_సమూహం_స్థానభ్రంశం==0) {
        తిరిగి శూన్యసూచిక;
    }
    తిరిగి getName(nameGroups+name_సమూహం_స్థానభ్రంశం, nameChoice);
}

int32_t Prop_Name_దత్తాంశం::get_Property_Or_విలువ_Enum(int32_t బైట్లు_Trie_స్థానభ్రంశం, స్థిరరాశి అక్షరం *alias) {
    బైట్లు_Trie trie(బైట్లు_Tries+బైట్లు_Trie_స్థానభ్రంశం);
    ఒకవేళ(కలిగియుంది_Name(trie, alias)) {
        తిరిగి trie.get_విలువ();
    } లేకపోతే {
        తిరిగి UCHAR_INVALID_సంకేతం;
    }
}

int32_t Prop_Name_దత్తాంశం::getPropertyEnum(స్థిరరాశి అక్షరం *alias) {
    తిరిగి get_Property_Or_విలువ_Enum(0, alias);
}

int32_t Prop_Name_దత్తాంశం::get_Property_విలువ_Enum(int32_t property, స్థిరరాశి అక్షరం *alias) {
    int32_t విలువ_Map_సూచీ=కనుగొను_Property(property);
    ఒకవేళ(విలువ_Map_సూచీ==0) {
        తిరిగి UCHAR_INVALID_సంకేతం;  // Not a known property.
    }
    విలువ_Map_సూచీ=విలువ_Maps[విలువ_Map_సూచీ+1];
    ఒకవేళ(విలువ_Map_సూచీ==0) {
        తిరిగి UCHAR_INVALID_సంకేతం;  // The property does not have named values.
    }
    // valueMapIndex is the start of the property's valueMap,
    // where the first word is the BytesTrie offset.
    తిరిగి get_Property_Or_విలువ_Enum(విలువ_Maps[విలువ_Map_సూచీ], alias);
}
U_NAMESPACE_చివర

//----------------------------------------------------------------------
// Public API implementation

U_CAPI స్థిరరాశి అక్షరం* U_EXPORT2
u_getPropertyName(UProperty property,
                  UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    తిరిగి Prop_Name_దత్తాంశం::getPropertyName(property, nameChoice);
}

U_CAPI UProperty U_EXPORT2
u_getPropertyEnum(స్థిరరాశి అక్షరం* alias) {
    U_NAMESPACE_USE
    తిరిగి (UProperty)Prop_Name_దత్తాంశం::getPropertyEnum(alias);
}

U_CAPI స్థిరరాశి అక్షరం* U_EXPORT2
u_get_Property_విలువ_Name(UProperty property,
                       int32_t value,
                       UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    తిరిగి Prop_Name_దత్తాంశం::get_Property_విలువ_Name(property, value, nameChoice);
}

U_CAPI int32_t U_EXPORT2
u_get_Property_విలువ_Enum(UProperty property,
                       స్థిరరాశి అక్షరం* alias) {
    U_NAMESPACE_USE
    తిరిగి Prop_Name_దత్తాంశం::get_Property_విలువ_Enum(property, alias);
}

U_CAPI స్థిరరాశి అక్షరం*  U_EXPORT2
uscript_getName(UScript_సంకేతం script_సంకేతం){
    తిరిగి u_get_Property_విలువ_Name(UCHAR_SCRIPT, script_సంకేతం,
                                  U_LONG_PROPERTY_NAME);
}

U_CAPI స్థిరరాశి అక్షరం*  U_EXPORT2
uscript_getShortName(UScript_సంకేతం script_సంకేతం){
    తిరిగి u_get_Property_విలువ_Name(UCHAR_SCRIPT, script_సంకేతం,
                                  U_SHORT_PROPERTY_NAME);
}
