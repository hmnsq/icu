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
#अन्तर्भाव "propname.h"
#अन्तर्भाव "unicode/uchar.h"
#अन्तर्भाव "unicode/udata.h"
#अन्तर्भाव "unicode/uscript.h"
#अन्तर्भाव "umutex.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "uarrsort.h"
#अन्तर्भाव "uinvchar.h"

#परिभाषय INCLUDED_FROM_PROPNAME_CPP
#अन्तर्भाव "propname_data.h"

U_CDECL_BEGIN

/**
 * Get the next non-ignorable ASCII character from a property name
 * and lowercases it.
 * @return ((advance count for the name)<<8)|character
 */
स्थिर अन्तःस्थ int32_t
getASCIIPropertyNameChar(ध्रुव वर्ण *name) {
    int32_t i;
    वर्ण c;

    /* Ignore delimiters '-', '_', and ASCII White_Space */
    प्रति(i=0;
        (c=name[i++])==0x2d || c==0x5f ||
        c==0x20 || (0x09<=c && c<=0x0d);
    ) {}

    यदि(c!=0) {
        प्रत्यागम (i << 8) | static_cast<uint8_t>(uprv_asciitolower(c));
    } अन्यथा {
        प्रत्यागम i<<8;
    }
}

/**
 * Get the next non-ignorable EBCDIC character from a property name
 * and lowercases it.
 * @return ((advance count for the name)<<8)|character
 */
स्थिर अन्तःस्थ int32_t
getEBCDICPropertyNameChar(ध्रुव वर्ण *name) {
    int32_t i;
    वर्ण c;

    /* Ignore delimiters '-', '_', and EBCDIC White_Space */
    प्रति(i=0;
        (c=name[i++])==0x60 || c==0x6d ||
        c==0x40 || c==0x05 || c==0x15 || c==0x25 || c==0x0b || c==0x0c || c==0x0d;
    ) {}

    यदि(c!=0) {
        प्रत्यागम (i << 8) | static_cast<uint8_t>(uprv_ebcdictolower(c));
    } अन्यथा {
        प्रत्यागम i<<8;
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
uprv_compare_ASCIIProperty_नामानि(ध्रुव वर्ण *name1, ध्रुव वर्ण *name2) {
    int32_t rc, r1, r2;

    प्रति(;;) {
        r1=getASCIIPropertyNameChar(name1);
        r2=getASCIIPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        यदि(((r1|r2)&0xff)==0) {
            प्रत्यागम 0;
        }

        /* Compare the lowercased characters */
        यदि(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            यदि(rc!=0) {
                प्रत्यागम rc;
            }
        }

        name1+=r1>>8;
        name2+=r2>>8;
    }
}

U_CAPI int32_t U_EXPORT2
uprv_compare_EBCDICProperty_नामानि(ध्रुव वर्ण *name1, ध्रुव वर्ण *name2) {
    int32_t rc, r1, r2;

    प्रति(;;) {
        r1=getEBCDICPropertyNameChar(name1);
        r2=getEBCDICPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        यदि(((r1|r2)&0xff)==0) {
            प्रत्यागम 0;
        }

        /* Compare the lowercased characters */
        यदि(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            यदि(rc!=0) {
                प्रत्यागम rc;
            }
        }

        name1+=r1>>8;
        name2+=r2>>8;
    }
}

U_CDECL_अन्तः

U_NAMESPACE_BEGIN

int32_t Prop_Name_दत्त::findProperty(int32_t property) {
    int32_t i=1;  // valueMaps index, initially after numRanges
    प्रति(int32_t अङ्कः_Ranges=मानम्_Maps[0]; अङ्कः_Ranges>0; --अङ्कः_Ranges) {
        // Read and skip the start and limit of this range.
        int32_t start=मानम्_Maps[i];
        int32_t सीमा=मानम्_Maps[i+1];
        i+=2;
        यदि(property<start) {
            भङ्ग;
        }
        यदि(property<सीमा) {
            प्रत्यागम i+(property-start)*2;
        }
        i+=(सीमा-start)*2;  // Skip all entries for this range.
    }
    प्रत्यागम 0;
}

int32_t Prop_Name_दत्त::find_Property_मानम्_Name_गणः(int32_t मानम्_Map_सूचकांक, int32_t value) {
    यदि(मानम्_Map_सूचकांक==0) {
        प्रत्यागम 0;  // The property does not have named values.
    }
    ++मानम्_Map_सूचकांक;  // Skip the BytesTrie offset.
    int32_t अङ्कः_Ranges=मानम्_Maps[मानम्_Map_सूचकांक++];
    यदि(अङ्कः_Ranges<0x10) {
        // Ranges of values.
        प्रति(; अङ्कः_Ranges>0; --अङ्कः_Ranges) {
            // Read and skip the start and limit of this range.
            int32_t start=मानम्_Maps[मानम्_Map_सूचकांक];
            int32_t सीमा=मानम्_Maps[मानम्_Map_सूचकांक+1];
            मानम्_Map_सूचकांक+=2;
            यदि(value<start) {
                भङ्ग;
            }
            यदि(value<सीमा) {
                प्रत्यागम मानम्_Maps[मानम्_Map_सूचकांक+value-start];
            }
            मानम्_Map_सूचकांक+=सीमा-start;  // Skip all entries for this range.
        }
    } अन्यथा {
        // List of values.
        int32_t मानानि_Start=मानम्_Map_सूचकांक;
        int32_t name_गणः_Offsets_Start=मानम्_Map_सूचकांक+अङ्कः_Ranges-0x10;
        कुरु {
            int32_t v=मानम्_Maps[मानम्_Map_सूचकांक];
            यदि(value<v) {
                भङ्ग;
            }
            यदि(value==v) {
                प्रत्यागम मानम्_Maps[name_गणः_Offsets_Start+मानम्_Map_सूचकांक-मानानि_Start];
            }
        } यावत्(++मानम्_Map_सूचकांक<name_गणः_Offsets_Start);
    }
    प्रत्यागम 0;
}

ध्रुव वर्ण *Prop_Name_दत्त::getName(ध्रुव वर्ण *name_गणः, int32_t name_सूचकांक) {
    int32_t अङ्कः_नामानि=*name_गणः++;
    यदि(name_सूचकांक<0 || अङ्कः_नामानि<=name_सूचकांक) {
        प्रत्यागम शून्यसूचकः;
    }
    // Skip nameIndex names.
    प्रति(; name_सूचकांक>0; --name_सूचकांक) {
        name_गणः=uprv_strchr(name_गणः, 0)+1;
    }
    यदि(*name_गणः==0) {
        प्रत्यागम शून्यसूचकः;  // no name (Property[Value]Aliases.txt has "n/a")
    }
    प्रत्यागम name_गणः;
}

UBool Prop_Name_दत्त::अन्तर्भवति_Name(अष्टकानि_Trie &trie, ध्रुव वर्ण *name) {
    यदि(name==शून्यसूचकः) {
        प्रत्यागम असत्य;
    }
    UString_Trie_परिणाम परिणाम=USTRINGTRIE_NO_मानम्;
    वर्ण c;
    यावत्((c=*name++)!=0) {
        c=uprv_inv_Char_गन्तव्यम्_Lowercase_Ascii(c);
        // Ignore delimiters '-', '_', and ASCII White_Space.
        यदि(c==0x2d || c==0x5f || c==0x20 || (0x09<=c && c<=0x0d)) {
            अनुवर्त;
        }
        यदि(!USTRINGTRIE_HAS_अग्र(परिणाम)) {
            प्रत्यागम असत्य;
        }
        परिणाम = trie.next(static_cast<uint8_t>(c));
    }
    प्रत्यागम USTRINGTRIE_HAS_मानम्(परिणाम);
}

ध्रुव वर्ण *Prop_Name_दत्त::getPropertyName(int32_t property, int32_t nameChoice) {
    int32_t मानम्_Map_सूचकांक=findProperty(property);
    यदि(मानम्_Map_सूचकांक==0) {
        प्रत्यागम शून्यसूचकः;  // Not a known property.
    }
    प्रत्यागम getName(nameGroups+मानम्_Maps[मानम्_Map_सूचकांक], nameChoice);
}

ध्रुव वर्ण *Prop_Name_दत्त::get_Property_मानम्_Name(int32_t property, int32_t value, int32_t nameChoice) {
    int32_t मानम्_Map_सूचकांक=findProperty(property);
    यदि(मानम्_Map_सूचकांक==0) {
        प्रत्यागम शून्यसूचकः;  // Not a known property.
    }
    int32_t name_गणः_विस्थापनम्=find_Property_मानम्_Name_गणः(मानम्_Maps[मानम्_Map_सूचकांक+1], value);
    यदि(name_गणः_विस्थापनम्==0) {
        प्रत्यागम शून्यसूचकः;
    }
    प्रत्यागम getName(nameGroups+name_गणः_विस्थापनम्, nameChoice);
}

int32_t Prop_Name_दत्त::get_Property_Or_मानम्_Enum(int32_t अष्टकानि_Trie_विस्थापनम्, ध्रुव वर्ण *alias) {
    अष्टकानि_Trie trie(अष्टकानि_Tries+अष्टकानि_Trie_विस्थापनम्);
    यदि(अन्तर्भवति_Name(trie, alias)) {
        प्रत्यागम trie.get_मानम्();
    } अन्यथा {
        प्रत्यागम UCHAR_INVALID_संहिता;
    }
}

int32_t Prop_Name_दत्त::getPropertyEnum(ध्रुव वर्ण *alias) {
    प्रत्यागम get_Property_Or_मानम्_Enum(0, alias);
}

int32_t Prop_Name_दत्त::get_Property_मानम्_Enum(int32_t property, ध्रुव वर्ण *alias) {
    int32_t मानम्_Map_सूचकांक=findProperty(property);
    यदि(मानम्_Map_सूचकांक==0) {
        प्रत्यागम UCHAR_INVALID_संहिता;  // Not a known property.
    }
    मानम्_Map_सूचकांक=मानम्_Maps[मानम्_Map_सूचकांक+1];
    यदि(मानम्_Map_सूचकांक==0) {
        प्रत्यागम UCHAR_INVALID_संहिता;  // The property does not have named values.
    }
    // valueMapIndex is the start of the property's valueMap,
    // where the first word is the BytesTrie offset.
    प्रत्यागम get_Property_Or_मानम्_Enum(मानम्_Maps[मानम्_Map_सूचकांक], alias);
}
U_NAMESPACE_अन्तः

//----------------------------------------------------------------------
// Public API implementation

U_CAPI ध्रुव वर्ण* U_EXPORT2
u_getPropertyName(UProperty property,
                  UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    प्रत्यागम Prop_Name_दत्त::getPropertyName(property, nameChoice);
}

U_CAPI UProperty U_EXPORT2
u_getPropertyEnum(ध्रुव वर्ण* alias) {
    U_NAMESPACE_USE
    प्रत्यागम (UProperty)Prop_Name_दत्त::getPropertyEnum(alias);
}

U_CAPI ध्रुव वर्ण* U_EXPORT2
u_get_Property_मानम्_Name(UProperty property,
                       int32_t value,
                       UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    प्रत्यागम Prop_Name_दत्त::get_Property_मानम्_Name(property, value, nameChoice);
}

U_CAPI int32_t U_EXPORT2
u_get_Property_मानम्_Enum(UProperty property,
                       ध्रुव वर्ण* alias) {
    U_NAMESPACE_USE
    प्रत्यागम Prop_Name_दत्त::get_Property_मानम्_Enum(property, alias);
}

U_CAPI ध्रुव वर्ण*  U_EXPORT2
uscript_getName(UScript_संहिता script_संहिता){
    प्रत्यागम u_get_Property_मानम्_Name(UCHAR_SCRIPT, script_संहिता,
                                  U_LONG_PROPERTY_NAME);
}

U_CAPI ध्रुव वर्ण*  U_EXPORT2
uscript_getShortName(UScript_संहिता script_संहिता){
    प्रत्यागम u_get_Property_मानम्_Name(UCHAR_SCRIPT, script_संहिता,
                                  U_SHORT_PROPERTY_NAME);
}
