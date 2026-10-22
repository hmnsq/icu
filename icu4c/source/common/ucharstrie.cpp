// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2010-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  ucharstrie.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010nov14
*   created by: Markus W. Scherer
*/

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/appendable.h"
#अन्तर्भाव "unicode/ucharstrie.h"
#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "uassert.h"

U_NAMESPACE_BEGIN

UCharsTrie::~UCharsTrie() {
    uprv_free(ownedArray_);
}

UString_Trie_परिणाम
UCharsTrie::current() ध्रुव {
    ध्रुव char16_t *pos=pos_;
    यदि(pos==शून्यसूचकः) {
        प्रत्यागम USTRINGTRIE_NO_मेलनम्;
    } अन्यथा {
        int32_t ग्रन्थिः;
        प्रत्यागम (remaining_मेलनम्_दीर्घता<0 && (ग्रन्थिः=*pos)>=k_Min_मानम्_Lead) ?
                मानम्_परिणाम(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
    }
}

UString_Trie_परिणाम
UCharsTrie::first_For_संहिता_Point(UChar32 cp) {
    प्रत्यागम cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_अग्र(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_मेलनम्);
}

UString_Trie_परिणाम
UCharsTrie::अग्र_For_संहिता_Point(UChar32 cp) {
    प्रत्यागम cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_अग्र(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_मेलनम्);
}

UString_Trie_परिणाम
UCharsTrie::branch_अग्र(ध्रुव char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    यदि(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    यावत्(length>k_Max_Branch_रैखिकम्_Sub_ग्रन्थिः_दीर्घता) {
        यदि(uchar<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } अन्यथा {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few units.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    कुरु {
        यदि(uchar==*pos++) {
            UString_Trie_परिणाम परिणाम;
            int32_t ग्रन्थिः=*pos;
            यदि(ग्रन्थिः&k_मानम्_Is_Final) {
                // Leave the final value for getValue() to read.
                परिणाम=USTRINGTRIE_FINAL_मानम्;
            } अन्यथा {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                यदि(ग्रन्थिः<k_Min_Two_Unit_मानम्_Lead) {
                    delta=ग्रन्थिः;
                } अन्यथा यदि(ग्रन्थिः<k_Three_Unit_मानम्_Lead) {
                    delta=((ग्रन्थिः-k_Min_Two_Unit_मानम्_Lead)<<16)|*pos++;
                } अन्यथा {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                ग्रन्थिः=*pos;
                परिणाम= ग्रन्थिः>=k_Min_मानम्_Lead ? मानम्_परिणाम(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
            }
            pos_=pos;
            प्रत्यागम परिणाम;
        }
        --length;
        pos=skip_मानम्(pos);
    } यावत्(length>1);
    यदि(uchar==*pos++) {
        pos_=pos;
        int32_t ग्रन्थिः=*pos;
        प्रत्यागम ग्रन्थिः>=k_Min_मानम्_Lead ? मानम्_परिणाम(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
    } अन्यथा {
        stop();
        प्रत्यागम USTRINGTRIE_NO_मेलनम्;
    }
}

UString_Trie_परिणाम
UCharsTrie::अग्र_Impl(ध्रुव char16_t *pos, int32_t uchar) {
    int32_t ग्रन्थिः=*pos++;
    प्रति(;;) {
        यदि(ग्रन्थिः<k_Min_रैखिकम्_मेलनम्) {
            प्रत्यागम branch_अग्र(pos, ग्रन्थिः, uchar);
        } अन्यथा यदि(ग्रन्थिः<k_Min_मानम्_Lead) {
            // Match the first of length+1 units.
            int32_t length=ग्रन्थिः-k_Min_रैखिकम्_मेलनम्;  // Actual match length minus 1.
            यदि(uchar==*pos++) {
                remaining_मेलनम्_दीर्घता=--length;
                pos_=pos;
                प्रत्यागम (length<0 && (ग्रन्थिः=*pos)>=k_Min_मानम्_Lead) ?
                        मानम्_परिणाम(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
            } अन्यथा {
                // No match.
                भङ्ग;
            }
        } अन्यथा यदि(ग्रन्थिः&k_मानम्_Is_Final) {
            // No further matching units.
            भङ्ग;
        } अन्यथा {
            // Skip intermediate value.
            pos=skip_ग्रन्थिः_मानम्(pos, ग्रन्थिः);
            ग्रन्थिः&=k_ग्रन्थिः_Type_आवरणम्;
        }
    }
    stop();
    प्रत्यागम USTRINGTRIE_NO_मेलनम्;
}

UString_Trie_परिणाम
UCharsTrie::next(int32_t uchar) {
    ध्रुव char16_t *pos=pos_;
    यदि(pos==शून्यसूचकः) {
        प्रत्यागम USTRINGTRIE_NO_मेलनम्;
    }
    int32_t length=remaining_मेलनम्_दीर्घता;  // Actual remaining match length minus 1.
    यदि(length>=0) {
        // Remaining part of a linear-match node.
        यदि(uchar==*pos++) {
            remaining_मेलनम्_दीर्घता=--length;
            pos_=pos;
            int32_t ग्रन्थिः;
            प्रत्यागम (length<0 && (ग्रन्थिः=*pos)>=k_Min_मानम्_Lead) ?
                    मानम्_परिणाम(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
        } अन्यथा {
            stop();
            प्रत्यागम USTRINGTRIE_NO_मेलनम्;
        }
    }
    प्रत्यागम अग्र_Impl(pos, uchar);
}

UString_Trie_परिणाम
UCharsTrie::next(ConstChar16Ptr ptr, int32_t s_दीर्घता) {
    ध्रुव char16_t *s=ptr;
    यदि(s_दीर्घता<0 ? *s==0 : s_दीर्घता==0) {
        // Empty input.
        प्रत्यागम current();
    }
    ध्रुव char16_t *pos=pos_;
    यदि(pos==शून्यसूचकः) {
        प्रत्यागम USTRINGTRIE_NO_मेलनम्;
    }
    int32_t length=remaining_मेलनम्_दीर्घता;  // Actual remaining match length minus 1.
    प्रति(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        यदि(s_दीर्घता<0) {
            प्रति(;;) {
                यदि((uchar=*s++)==0) {
                    remaining_मेलनम्_दीर्घता=length;
                    pos_=pos;
                    int32_t ग्रन्थिः;
                    प्रत्यागम (length<0 && (ग्रन्थिः=*pos)>=k_Min_मानम्_Lead) ?
                            मानम्_परिणाम(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
                }
                यदि(length<0) {
                    remaining_मेलनम्_दीर्घता=length;
                    भङ्ग;
                }
                यदि(uchar!=*pos) {
                    stop();
                    प्रत्यागम USTRINGTRIE_NO_मेलनम्;
                }
                ++pos;
                --length;
            }
        } अन्यथा {
            प्रति(;;) {
                यदि(s_दीर्घता==0) {
                    remaining_मेलनम्_दीर्घता=length;
                    pos_=pos;
                    int32_t ग्रन्थिः;
                    प्रत्यागम (length<0 && (ग्रन्थिः=*pos)>=k_Min_मानम्_Lead) ?
                            मानम्_परिणाम(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
                }
                uchar=*s++;
                --s_दीर्घता;
                यदि(length<0) {
                    remaining_मेलनम्_दीर्घता=length;
                    भङ्ग;
                }
                यदि(uchar!=*pos) {
                    stop();
                    प्रत्यागम USTRINGTRIE_NO_मेलनम्;
                }
                ++pos;
                --length;
            }
        }
        int32_t ग्रन्थिः=*pos++;
        प्रति(;;) {
            यदि(ग्रन्थिः<k_Min_रैखिकम्_मेलनम्) {
                UString_Trie_परिणाम परिणाम=branch_अग्र(pos, ग्रन्थिः, uchar);
                यदि(परिणाम==USTRINGTRIE_NO_मेलनम्) {
                    प्रत्यागम USTRINGTRIE_NO_मेलनम्;
                }
                // Fetch the next input unit, if there is one.
                यदि(s_दीर्घता<0) {
                    यदि((uchar=*s++)==0) {
                        प्रत्यागम परिणाम;
                    }
                } अन्यथा {
                    यदि(s_दीर्घता==0) {
                        प्रत्यागम परिणाम;
                    }
                    uchar=*s++;
                    --s_दीर्घता;
                }
                यदि(परिणाम==USTRINGTRIE_FINAL_मानम्) {
                    // No further matching units.
                    stop();
                    प्रत्यागम USTRINGTRIE_NO_मेलनम्;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                ग्रन्थिः=*pos++;
            } अन्यथा यदि(ग्रन्थिः<k_Min_मानम्_Lead) {
                // Match length+1 units.
                length=ग्रन्थिः-k_Min_रैखिकम्_मेलनम्;  // Actual match length minus 1.
                यदि(uchar!=*pos) {
                    stop();
                    प्रत्यागम USTRINGTRIE_NO_मेलनम्;
                }
                ++pos;
                --length;
                भङ्ग;
            } अन्यथा यदि(ग्रन्थिः&k_मानम्_Is_Final) {
                // No further matching units.
                stop();
                प्रत्यागम USTRINGTRIE_NO_मेलनम्;
            } अन्यथा {
                // Skip intermediate value.
                pos=skip_ग्रन्थिः_मानम्(pos, ग्रन्थिः);
                ग्रन्थिः&=k_ग्रन्थिः_Type_आवरणम्;
            }
        }
    }
}

ध्रुव char16_t*
UCharsTrie::find_Unique_मानम्_From_Branch(ध्रुव char16_t* pos U_LIFETIME_BOUND,
                                      int32_t length,
                                      UBool have_Unique_मानम्,
                                      int32_t& unique_मानम्) {
    यावत्(length>k_Max_Branch_रैखिकम्_Sub_ग्रन्थिः_दीर्घता) {
        ++pos;  // ignore the comparison unit
        यदि(शून्यसूचकः==find_Unique_मानम्_From_Branch(jumpByDelta(pos), length>>1, have_Unique_मानम्, unique_मानम्)) {
            प्रत्यागम शून्यसूचकः;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    कुरु {
        ++pos;  // ignore a comparison unit
        // handle its value
        int32_t ग्रन्थिः=*pos++;
        UBool isFinal = static_cast<UBool>(ग्रन्थिः >> 15);
        ग्रन्थिः&=0x7fff;
        int32_t value=read_मानम्(pos, ग्रन्थिः);
        pos=skip_मानम्(pos, ग्रन्थिः);
        यदि(isFinal) {
            यदि(have_Unique_मानम्) {
                यदि(value!=unique_मानम्) {
                    प्रत्यागम शून्यसूचकः;
                }
            } अन्यथा {
                unique_मानम्=value;
                have_Unique_मानम्=सत्य;
            }
        } अन्यथा {
            यदि(!find_Unique_मानम्(pos+value, have_Unique_मानम्, unique_मानम्)) {
                प्रत्यागम शून्यसूचकः;
            }
            have_Unique_मानम्=सत्य;
        }
    } यावत्(--length>1);
    प्रत्यागम pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::find_Unique_मानम्(ध्रुव char16_t *pos, UBool have_Unique_मानम्, int32_t &unique_मानम्) {
    int32_t ग्रन्थिः=*pos++;
    प्रति(;;) {
        यदि(ग्रन्थिः<k_Min_रैखिकम्_मेलनम्) {
            यदि(ग्रन्थिः==0) {
                ग्रन्थिः=*pos++;
            }
            pos=find_Unique_मानम्_From_Branch(pos, ग्रन्थिः+1, have_Unique_मानम्, unique_मानम्);
            यदि(pos==शून्यसूचकः) {
                प्रत्यागम असत्य;
            }
            have_Unique_मानम्=सत्य;
            ग्रन्थिः=*pos++;
        } अन्यथा यदि(ग्रन्थिः<k_Min_मानम्_Lead) {
            // linear-match node
            pos+=ग्रन्थिः-k_Min_रैखिकम्_मेलनम्+1;  // Ignore the match units.
            ग्रन्थिः=*pos++;
        } अन्यथा {
            UBool isFinal = static_cast<UBool>(ग्रन्थिः >> 15);
            int32_t value;
            यदि(isFinal) {
                value=read_मानम्(pos, ग्रन्थिः&0x7fff);
            } अन्यथा {
                value=read_ग्रन्थिः_मानम्(pos, ग्रन्थिः);
            }
            यदि(have_Unique_मानम्) {
                यदि(value!=unique_मानम्) {
                    प्रत्यागम असत्य;
                }
            } अन्यथा {
                unique_मानम्=value;
                have_Unique_मानम्=सत्य;
            }
            यदि(isFinal) {
                प्रत्यागम सत्य;
            }
            pos=skip_ग्रन्थिः_मानम्(pos, ग्रन्थिः);
            ग्रन्थिः&=k_ग्रन्थिः_Type_आवरणम्;
        }
    }
}

int32_t
UCharsTrie::get_अग्र_UChars(Appendable &out) ध्रुव {
    ध्रुव char16_t *pos=pos_;
    यदि(pos==शून्यसूचकः) {
        प्रत्यागम 0;
    }
    यदि(remaining_मेलनम्_दीर्घता>=0) {
        out.append_संहिता_Unit(*pos);  // Next unit of a pending linear-match node.
        प्रत्यागम 1;
    }
    int32_t ग्रन्थिः=*pos++;
    यदि(ग्रन्थिः>=k_Min_मानम्_Lead) {
        यदि(ग्रन्थिः&k_मानम्_Is_Final) {
            प्रत्यागम 0;
        } अन्यथा {
            pos=skip_ग्रन्थिः_मानम्(pos, ग्रन्थिः);
            ग्रन्थिः&=k_ग्रन्थिः_Type_आवरणम्;
        }
    }
    यदि(ग्रन्थिः<k_Min_रैखिकम्_मेलनम्) {
        यदि(ग्रन्थिः==0) {
            ग्रन्थिः=*pos++;
        }
        out.reserve_Append_क्षमता(++ग्रन्थिः);
        get_अग्र_Branch_UChars(pos, ग्रन्थिः, out);
        प्रत्यागम ग्रन्थिः;
    } अन्यथा {
        // First unit of the linear-match node.
        out.append_संहिता_Unit(*pos);
        प्रत्यागम 1;
    }
}

शून्य
UCharsTrie::get_अग्र_Branch_UChars(ध्रुव char16_t *pos, int32_t length, Appendable &out) {
    यावत्(length>k_Max_Branch_रैखिकम्_Sub_ग्रन्थिः_दीर्घता) {
        ++pos;  // ignore the comparison unit
        get_अग्र_Branch_UChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    कुरु {
        out.append_संहिता_Unit(*pos++);
        pos=skip_मानम्(pos);
    } यावत्(--length>1);
    out.append_संहिता_Unit(*pos);
}

U_NAMESPACE_अन्तः
