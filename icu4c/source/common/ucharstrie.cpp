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

#समावेश "unicode/utypes.h"
#समावेश "unicode/appendable.h"
#समावेश "unicode/ucharstrie.h"
#समावेश "unicode/uobject.h"
#समावेश "unicode/utf16.h"
#समावेश "cmemory.h"
#समावेश "uassert.h"

U_NAMESPACE_BEGIN

UCharsTrie::~UCharsTrie() {
    uprv_free(ownedArray_);
}

UString_Trie_परिणाम
UCharsTrie::current() स्थिर {
    स्थिर char16_t *pos=pos_;
    अगर(pos==nullptr) {
        वापस USTRINGTRIE_NO_MATCH;
    } अन्यथा {
        int32_t गाँठ;
        वापस (remaining_Match_लंबाई<0 && (गाँठ=*pos)>=k_Min_मूल्य_Lead) ?
                मूल्य_परिणाम(गाँठ) : USTRINGTRIE_NO_मूल्य;
    }
}

UString_Trie_परिणाम
UCharsTrie::firstForCodePoint(UChar32 cp) {
    वापस cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_अग्र(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UString_Trie_परिणाम
UCharsTrie::अग्र_For_Code_Point(UChar32 cp) {
    वापस cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_अग्र(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UString_Trie_परिणाम
UCharsTrie::branch_अग्र(स्थिर char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    अगर(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    जबतक(length>k_Max_Branch_Linear_Sub_गाँठ_लंबाई) {
        अगर(uchar<*pos++) {
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
    करो {
        अगर(uchar==*pos++) {
            UString_Trie_परिणाम परिणाम;
            int32_t गाँठ=*pos;
            अगर(गाँठ&k_मूल्य_Is_Final) {
                // Leave the final value for getValue() to read.
                परिणाम=USTRINGTRIE_FINAL_मूल्य;
            } अन्यथा {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                अगर(गाँठ<k_Min_Two_Unit_मूल्य_Lead) {
                    delta=गाँठ;
                } अन्यथा अगर(गाँठ<k_Three_Unit_मूल्य_Lead) {
                    delta=((गाँठ-k_Min_Two_Unit_मूल्य_Lead)<<16)|*pos++;
                } अन्यथा {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                गाँठ=*pos;
                परिणाम= गाँठ>=k_Min_मूल्य_Lead ? मूल्य_परिणाम(गाँठ) : USTRINGTRIE_NO_मूल्य;
            }
            pos_=pos;
            वापस परिणाम;
        }
        --length;
        pos=skip_मूल्य(pos);
    } जबतक(length>1);
    अगर(uchar==*pos++) {
        pos_=pos;
        int32_t गाँठ=*pos;
        वापस गाँठ>=k_Min_मूल्य_Lead ? मूल्य_परिणाम(गाँठ) : USTRINGTRIE_NO_मूल्य;
    } अन्यथा {
        stop();
        वापस USTRINGTRIE_NO_MATCH;
    }
}

UString_Trie_परिणाम
UCharsTrie::अग्र_Impl(स्थिर char16_t *pos, int32_t uchar) {
    int32_t गाँठ=*pos++;
    क्रम(;;) {
        अगर(गाँठ<kMinLinearMatch) {
            वापस branch_अग्र(pos, गाँठ, uchar);
        } अन्यथा अगर(गाँठ<k_Min_मूल्य_Lead) {
            // Match the first of length+1 units.
            int32_t length=गाँठ-kMinLinearMatch;  // Actual match length minus 1.
            अगर(uchar==*pos++) {
                remaining_Match_लंबाई=--length;
                pos_=pos;
                वापस (length<0 && (गाँठ=*pos)>=k_Min_मूल्य_Lead) ?
                        मूल्य_परिणाम(गाँठ) : USTRINGTRIE_NO_मूल्य;
            } अन्यथा {
                // No match.
                अवरोध;
            }
        } अन्यथा अगर(गाँठ&k_मूल्य_Is_Final) {
            // No further matching units.
            अवरोध;
        } अन्यथा {
            // Skip intermediate value.
            pos=skip_गाँठ_मूल्य(pos, गाँठ);
            गाँठ&=k_गाँठ_Type_Mask;
        }
    }
    stop();
    वापस USTRINGTRIE_NO_MATCH;
}

UString_Trie_परिणाम
UCharsTrie::next(int32_t uchar) {
    स्थिर char16_t *pos=pos_;
    अगर(pos==nullptr) {
        वापस USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remaining_Match_लंबाई;  // Actual remaining match length minus 1.
    अगर(length>=0) {
        // Remaining part of a linear-match node.
        अगर(uchar==*pos++) {
            remaining_Match_लंबाई=--length;
            pos_=pos;
            int32_t गाँठ;
            वापस (length<0 && (गाँठ=*pos)>=k_Min_मूल्य_Lead) ?
                    मूल्य_परिणाम(गाँठ) : USTRINGTRIE_NO_मूल्य;
        } अन्यथा {
            stop();
            वापस USTRINGTRIE_NO_MATCH;
        }
    }
    वापस अग्र_Impl(pos, uchar);
}

UString_Trie_परिणाम
UCharsTrie::next(ConstChar16Ptr ptr, int32_t s_लंबाई) {
    स्थिर char16_t *s=ptr;
    अगर(s_लंबाई<0 ? *s==0 : s_लंबाई==0) {
        // Empty input.
        वापस current();
    }
    स्थिर char16_t *pos=pos_;
    अगर(pos==nullptr) {
        वापस USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remaining_Match_लंबाई;  // Actual remaining match length minus 1.
    क्रम(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        अगर(s_लंबाई<0) {
            क्रम(;;) {
                अगर((uchar=*s++)==0) {
                    remaining_Match_लंबाई=length;
                    pos_=pos;
                    int32_t गाँठ;
                    वापस (length<0 && (गाँठ=*pos)>=k_Min_मूल्य_Lead) ?
                            मूल्य_परिणाम(गाँठ) : USTRINGTRIE_NO_मूल्य;
                }
                अगर(length<0) {
                    remaining_Match_लंबाई=length;
                    अवरोध;
                }
                अगर(uchar!=*pos) {
                    stop();
                    वापस USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } अन्यथा {
            क्रम(;;) {
                अगर(s_लंबाई==0) {
                    remaining_Match_लंबाई=length;
                    pos_=pos;
                    int32_t गाँठ;
                    वापस (length<0 && (गाँठ=*pos)>=k_Min_मूल्य_Lead) ?
                            मूल्य_परिणाम(गाँठ) : USTRINGTRIE_NO_मूल्य;
                }
                uchar=*s++;
                --s_लंबाई;
                अगर(length<0) {
                    remaining_Match_लंबाई=length;
                    अवरोध;
                }
                अगर(uchar!=*pos) {
                    stop();
                    वापस USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        int32_t गाँठ=*pos++;
        क्रम(;;) {
            अगर(गाँठ<kMinLinearMatch) {
                UString_Trie_परिणाम परिणाम=branch_अग्र(pos, गाँठ, uchar);
                अगर(परिणाम==USTRINGTRIE_NO_MATCH) {
                    वापस USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input unit, if there is one.
                अगर(s_लंबाई<0) {
                    अगर((uchar=*s++)==0) {
                        वापस परिणाम;
                    }
                } अन्यथा {
                    अगर(s_लंबाई==0) {
                        वापस परिणाम;
                    }
                    uchar=*s++;
                    --s_लंबाई;
                }
                अगर(परिणाम==USTRINGTRIE_FINAL_मूल्य) {
                    // No further matching units.
                    stop();
                    वापस USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                गाँठ=*pos++;
            } अन्यथा अगर(गाँठ<k_Min_मूल्य_Lead) {
                // Match length+1 units.
                length=गाँठ-kMinLinearMatch;  // Actual match length minus 1.
                अगर(uchar!=*pos) {
                    stop();
                    वापस USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                अवरोध;
            } अन्यथा अगर(गाँठ&k_मूल्य_Is_Final) {
                // No further matching units.
                stop();
                वापस USTRINGTRIE_NO_MATCH;
            } अन्यथा {
                // Skip intermediate value.
                pos=skip_गाँठ_मूल्य(pos, गाँठ);
                गाँठ&=k_गाँठ_Type_Mask;
            }
        }
    }
}

स्थिर char16_t*
UCharsTrie::find_Unique_मूल्य_From_Branch(स्थिर char16_t* pos U_LIFETIME_BOUND,
                                      int32_t length,
                                      UBool have_Unique_मूल्य,
                                      int32_t& unique_मूल्य) {
    जबतक(length>k_Max_Branch_Linear_Sub_गाँठ_लंबाई) {
        ++pos;  // ignore the comparison unit
        अगर(nullptr==find_Unique_मूल्य_From_Branch(jumpByDelta(pos), length>>1, have_Unique_मूल्य, unique_मूल्य)) {
            वापस nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    करो {
        ++pos;  // ignore a comparison unit
        // handle its value
        int32_t गाँठ=*pos++;
        UBool isFinal = static_cast<UBool>(गाँठ >> 15);
        गाँठ&=0x7fff;
        int32_t value=read_मूल्य(pos, गाँठ);
        pos=skip_मूल्य(pos, गाँठ);
        अगर(isFinal) {
            अगर(have_Unique_मूल्य) {
                अगर(value!=unique_मूल्य) {
                    वापस nullptr;
                }
            } अन्यथा {
                unique_मूल्य=value;
                have_Unique_मूल्य=true;
            }
        } अन्यथा {
            अगर(!find_Unique_मूल्य(pos+value, have_Unique_मूल्य, unique_मूल्य)) {
                वापस nullptr;
            }
            have_Unique_मूल्य=true;
        }
    } जबतक(--length>1);
    वापस pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::find_Unique_मूल्य(स्थिर char16_t *pos, UBool have_Unique_मूल्य, int32_t &unique_मूल्य) {
    int32_t गाँठ=*pos++;
    क्रम(;;) {
        अगर(गाँठ<kMinLinearMatch) {
            अगर(गाँठ==0) {
                गाँठ=*pos++;
            }
            pos=find_Unique_मूल्य_From_Branch(pos, गाँठ+1, have_Unique_मूल्य, unique_मूल्य);
            अगर(pos==nullptr) {
                वापस false;
            }
            have_Unique_मूल्य=true;
            गाँठ=*pos++;
        } अन्यथा अगर(गाँठ<k_Min_मूल्य_Lead) {
            // linear-match node
            pos+=गाँठ-kMinLinearMatch+1;  // Ignore the match units.
            गाँठ=*pos++;
        } अन्यथा {
            UBool isFinal = static_cast<UBool>(गाँठ >> 15);
            int32_t value;
            अगर(isFinal) {
                value=read_मूल्य(pos, गाँठ&0x7fff);
            } अन्यथा {
                value=read_गाँठ_मूल्य(pos, गाँठ);
            }
            अगर(have_Unique_मूल्य) {
                अगर(value!=unique_मूल्य) {
                    वापस false;
                }
            } अन्यथा {
                unique_मूल्य=value;
                have_Unique_मूल्य=true;
            }
            अगर(isFinal) {
                वापस true;
            }
            pos=skip_गाँठ_मूल्य(pos, गाँठ);
            गाँठ&=k_गाँठ_Type_Mask;
        }
    }
}

int32_t
UCharsTrie::get_अग्र_UChars(Appendable &out) स्थिर {
    स्थिर char16_t *pos=pos_;
    अगर(pos==nullptr) {
        वापस 0;
    }
    अगर(remaining_Match_लंबाई>=0) {
        out.appendCodeUnit(*pos);  // Next unit of a pending linear-match node.
        वापस 1;
    }
    int32_t गाँठ=*pos++;
    अगर(गाँठ>=k_Min_मूल्य_Lead) {
        अगर(गाँठ&k_मूल्य_Is_Final) {
            वापस 0;
        } अन्यथा {
            pos=skip_गाँठ_मूल्य(pos, गाँठ);
            गाँठ&=k_गाँठ_Type_Mask;
        }
    }
    अगर(गाँठ<kMinLinearMatch) {
        अगर(गाँठ==0) {
            गाँठ=*pos++;
        }
        out.reserveAppendCapacity(++गाँठ);
        get_अग्र_Branch_UChars(pos, गाँठ, out);
        वापस गाँठ;
    } अन्यथा {
        // First unit of the linear-match node.
        out.appendCodeUnit(*pos);
        वापस 1;
    }
}

व्योम
UCharsTrie::get_अग्र_Branch_UChars(स्थिर char16_t *pos, int32_t length, Appendable &out) {
    जबतक(length>k_Max_Branch_Linear_Sub_गाँठ_लंबाई) {
        ++pos;  // ignore the comparison unit
        get_अग्र_Branch_UChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    करो {
        out.appendCodeUnit(*pos++);
        pos=skip_मूल्य(pos);
    } जबतक(--length>1);
    out.appendCodeUnit(*pos);
}

U_NAMESPACE_END
