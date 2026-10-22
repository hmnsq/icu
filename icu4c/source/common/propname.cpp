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
#समावेश "propname.h"
#समावेश "unicode/uchar.h"
#समावेश "unicode/udata.h"
#समावेश "unicode/uscript.h"
#समावेश "umutex.h"
#समावेश "cmemory.h"
#समावेश "cstring.h"
#समावेश "uarrsort.h"
#समावेश "uinvchar.h"

#घोषणा INCLUDED_FROM_PROPNAME_CPP
#समावेश "propname_data.h"

U_CDECL_BEGIN

/**
 * Get the next non-ignorable ASCII character from a property name
 * and lowercases it.
 * @return ((advance count for the name)<<8)|character
 */
अटल अंतरभूत int32_t
getASCIIPropertyNameChar(स्थिर अक्षर *name) {
    int32_t i;
    अक्षर c;

    /* Ignore delimiters '-', '_', and ASCII White_Space */
    क्रम(i=0;
        (c=name[i++])==0x2d || c==0x5f ||
        c==0x20 || (0x09<=c && c<=0x0d);
    ) {}

    अगर(c!=0) {
        वापस (i << 8) | static_cast<uint8_t>(uprv_asciitolower(c));
    } अन्यथा {
        वापस i<<8;
    }
}

/**
 * Get the next non-ignorable EBCDIC character from a property name
 * and lowercases it.
 * @return ((advance count for the name)<<8)|character
 */
अटल अंतरभूत int32_t
getEBCDICPropertyNameChar(स्थिर अक्षर *name) {
    int32_t i;
    अक्षर c;

    /* Ignore delimiters '-', '_', and EBCDIC White_Space */
    क्रम(i=0;
        (c=name[i++])==0x60 || c==0x6d ||
        c==0x40 || c==0x05 || c==0x15 || c==0x25 || c==0x0b || c==0x0c || c==0x0d;
    ) {}

    अगर(c!=0) {
        वापस (i << 8) | static_cast<uint8_t>(uprv_ebcdictolower(c));
    } अन्यथा {
        वापस i<<8;
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
uprv_compareASCIIPropertyNames(स्थिर अक्षर *name1, स्थिर अक्षर *name2) {
    int32_t rc, r1, r2;

    क्रम(;;) {
        r1=getASCIIPropertyNameChar(name1);
        r2=getASCIIPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        अगर(((r1|r2)&0xff)==0) {
            वापस 0;
        }

        /* Compare the lowercased characters */
        अगर(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            अगर(rc!=0) {
                वापस rc;
            }
        }

        name1+=r1>>8;
        name2+=r2>>8;
    }
}

U_CAPI int32_t U_EXPORT2
uprv_compareEBCDICPropertyNames(स्थिर अक्षर *name1, स्थिर अक्षर *name2) {
    int32_t rc, r1, r2;

    क्रम(;;) {
        r1=getEBCDICPropertyNameChar(name1);
        r2=getEBCDICPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        अगर(((r1|r2)&0xff)==0) {
            वापस 0;
        }

        /* Compare the lowercased characters */
        अगर(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            अगर(rc!=0) {
                वापस rc;
            }
        }

        name1+=r1>>8;
        name2+=r2>>8;
    }
}

U_CDECL_END

U_NAMESPACE_BEGIN

int32_t Prop_Name_तथ्य::findProperty(int32_t property) {
    int32_t i=1;  // valueMaps index, initially after numRanges
    क्रम(int32_t अंक_Ranges=मूल्य_Maps[0]; अंक_Ranges>0; --अंक_Ranges) {
        // Read and skip the start and limit of this range.
        int32_t start=मूल्य_Maps[i];
        int32_t limit=मूल्य_Maps[i+1];
        i+=2;
        अगर(property<start) {
            अवरोध;
        }
        अगर(property<limit) {
            वापस i+(property-start)*2;
        }
        i+=(limit-start)*2;  // Skip all entries for this range.
    }
    वापस 0;
}

int32_t Prop_Name_तथ्य::find_Property_मूल्य_Name_Group(int32_t मूल्य_Map_सूचकांक, int32_t value) {
    अगर(मूल्य_Map_सूचकांक==0) {
        वापस 0;  // The property does not have named values.
    }
    ++मूल्य_Map_सूचकांक;  // Skip the BytesTrie offset.
    int32_t अंक_Ranges=मूल्य_Maps[मूल्य_Map_सूचकांक++];
    अगर(अंक_Ranges<0x10) {
        // Ranges of values.
        क्रम(; अंक_Ranges>0; --अंक_Ranges) {
            // Read and skip the start and limit of this range.
            int32_t start=मूल्य_Maps[मूल्य_Map_सूचकांक];
            int32_t limit=मूल्य_Maps[मूल्य_Map_सूचकांक+1];
            मूल्य_Map_सूचकांक+=2;
            अगर(value<start) {
                अवरोध;
            }
            अगर(value<limit) {
                वापस मूल्य_Maps[मूल्य_Map_सूचकांक+value-start];
            }
            मूल्य_Map_सूचकांक+=limit-start;  // Skip all entries for this range.
        }
    } अन्यथा {
        // List of values.
        int32_t मूल्यावली_Start=मूल्य_Map_सूचकांक;
        int32_t nameGroupOffsetsStart=मूल्य_Map_सूचकांक+अंक_Ranges-0x10;
        करो {
            int32_t v=मूल्य_Maps[मूल्य_Map_सूचकांक];
            अगर(value<v) {
                अवरोध;
            }
            अगर(value==v) {
                वापस मूल्य_Maps[nameGroupOffsetsStart+मूल्य_Map_सूचकांक-मूल्यावली_Start];
            }
        } जबतक(++मूल्य_Map_सूचकांक<nameGroupOffsetsStart);
    }
    वापस 0;
}

स्थिर अक्षर *Prop_Name_तथ्य::getName(स्थिर अक्षर *nameGroup, int32_t name_सूचकांक) {
    int32_t अंक_Names=*nameGroup++;
    अगर(name_सूचकांक<0 || अंक_Names<=name_सूचकांक) {
        वापस nullptr;
    }
    // Skip nameIndex names.
    क्रम(; name_सूचकांक>0; --name_सूचकांक) {
        nameGroup=uprv_strchr(nameGroup, 0)+1;
    }
    अगर(*nameGroup==0) {
        वापस nullptr;  // no name (Property[Value]Aliases.txt has "n/a")
    }
    वापस nameGroup;
}

UBool Prop_Name_तथ्य::containsName(BytesTrie &trie, स्थिर अक्षर *name) {
    अगर(name==nullptr) {
        वापस false;
    }
    UString_Trie_परिणाम परिणाम=USTRINGTRIE_NO_मूल्य;
    अक्षर c;
    जबतक((c=*name++)!=0) {
        c=uprv_invCharToLowercaseAscii(c);
        // Ignore delimiters '-', '_', and ASCII White_Space.
        अगर(c==0x2d || c==0x5f || c==0x20 || (0x09<=c && c<=0x0d)) {
            जारी;
        }
        अगर(!USTRINGTRIE_HAS_अग्र(परिणाम)) {
            वापस false;
        }
        परिणाम = trie.next(static_cast<uint8_t>(c));
    }
    वापस USTRINGTRIE_HAS_मूल्य(परिणाम);
}

स्थिर अक्षर *Prop_Name_तथ्य::getPropertyName(int32_t property, int32_t nameChoice) {
    int32_t मूल्य_Map_सूचकांक=findProperty(property);
    अगर(मूल्य_Map_सूचकांक==0) {
        वापस nullptr;  // Not a known property.
    }
    वापस getName(nameGroups+मूल्य_Maps[मूल्य_Map_सूचकांक], nameChoice);
}

स्थिर अक्षर *Prop_Name_तथ्य::get_Property_मूल्य_Name(int32_t property, int32_t value, int32_t nameChoice) {
    int32_t मूल्य_Map_सूचकांक=findProperty(property);
    अगर(मूल्य_Map_सूचकांक==0) {
        वापस nullptr;  // Not a known property.
    }
    int32_t nameGroupOffset=find_Property_मूल्य_Name_Group(मूल्य_Maps[मूल्य_Map_सूचकांक+1], value);
    अगर(nameGroupOffset==0) {
        वापस nullptr;
    }
    वापस getName(nameGroups+nameGroupOffset, nameChoice);
}

int32_t Prop_Name_तथ्य::get_Property_Or_मूल्य_Enum(int32_t bytesTrieOffset, स्थिर अक्षर *alias) {
    BytesTrie trie(bytesTries+bytesTrieOffset);
    अगर(containsName(trie, alias)) {
        वापस trie.get_मूल्य();
    } अन्यथा {
        वापस UCHAR_INVALID_CODE;
    }
}

int32_t Prop_Name_तथ्य::getPropertyEnum(स्थिर अक्षर *alias) {
    वापस get_Property_Or_मूल्य_Enum(0, alias);
}

int32_t Prop_Name_तथ्य::get_Property_मूल्य_Enum(int32_t property, स्थिर अक्षर *alias) {
    int32_t मूल्य_Map_सूचकांक=findProperty(property);
    अगर(मूल्य_Map_सूचकांक==0) {
        वापस UCHAR_INVALID_CODE;  // Not a known property.
    }
    मूल्य_Map_सूचकांक=मूल्य_Maps[मूल्य_Map_सूचकांक+1];
    अगर(मूल्य_Map_सूचकांक==0) {
        वापस UCHAR_INVALID_CODE;  // The property does not have named values.
    }
    // valueMapIndex is the start of the property's valueMap,
    // where the first word is the BytesTrie offset.
    वापस get_Property_Or_मूल्य_Enum(मूल्य_Maps[मूल्य_Map_सूचकांक], alias);
}
U_NAMESPACE_END

//----------------------------------------------------------------------
// Public API implementation

U_CAPI स्थिर अक्षर* U_EXPORT2
u_getPropertyName(UProperty property,
                  UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    वापस Prop_Name_तथ्य::getPropertyName(property, nameChoice);
}

U_CAPI UProperty U_EXPORT2
u_getPropertyEnum(स्थिर अक्षर* alias) {
    U_NAMESPACE_USE
    वापस (UProperty)Prop_Name_तथ्य::getPropertyEnum(alias);
}

U_CAPI स्थिर अक्षर* U_EXPORT2
u_get_Property_मूल्य_Name(UProperty property,
                       int32_t value,
                       UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    वापस Prop_Name_तथ्य::get_Property_मूल्य_Name(property, value, nameChoice);
}

U_CAPI int32_t U_EXPORT2
u_get_Property_मूल्य_Enum(UProperty property,
                       स्थिर अक्षर* alias) {
    U_NAMESPACE_USE
    वापस Prop_Name_तथ्य::get_Property_मूल्य_Enum(property, alias);
}

U_CAPI स्थिर अक्षर*  U_EXPORT2
uscript_getName(UScriptCode scriptCode){
    वापस u_get_Property_मूल्य_Name(UCHAR_SCRIPT, scriptCode,
                                  U_LONG_PROPERTY_NAME);
}

U_CAPI स्थिर अक्षर*  U_EXPORT2
uscript_getShortName(UScriptCode scriptCode){
    वापस u_get_Property_मूल्य_Name(UCHAR_SCRIPT, scriptCode,
                                  U_SHORT_PROPERTY_NAME);
}
