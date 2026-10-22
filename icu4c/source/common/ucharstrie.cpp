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

UStringTrieResult
UCharsTrie::current() ध्रुव {
    ध्रुव char16_t *pos=pos_;
    जइ(pos==nullptr) {
        प्रत्यागम USTRINGTRIE_NO_MATCH;
    } अण्णहा {
        int32_t ग्रन्थिः;
        प्रत्यागम (remainingMatchLength_<0 && (ग्रन्थिः=*pos)>=k_Min_मानम्_Lead) ?
                मानम्_Result(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
    }
}

UStringTrieResult
UCharsTrie::firstForCodePoint(UChar32 cp) {
    प्रत्यागम cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_NEXT(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::nextForCodePoint(UChar32 cp) {
    प्रत्यागम cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_NEXT(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::branchNext(ध्रुव char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    जइ(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    जाव(length>k_Max_Branch_Linear_Sub_ग्रन्थिः_Length) {
        जइ(uchar<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } अण्णहा {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few units.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    करेहि {
        जइ(uchar==*pos++) {
            UStringTrieResult result;
            int32_t ग्रन्थिः=*pos;
            जइ(ग्रन्थिः&k_मानम्_Is_Final) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_मानम्;
            } अण्णहा {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                जइ(ग्रन्थिः<k_Min_Two_Unit_मानम्_Lead) {
                    delta=ग्रन्थिः;
                } अण्णहा जइ(ग्रन्थिः<k_Three_Unit_मानम्_Lead) {
                    delta=((ग्रन्थिः-k_Min_Two_Unit_मानम्_Lead)<<16)|*pos++;
                } अण्णहा {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                ग्रन्थिः=*pos;
                result= ग्रन्थिः>=k_Min_मानम्_Lead ? मानम्_Result(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
            }
            pos_=pos;
            प्रत्यागम result;
        }
        --length;
        pos=skip_मानम्(pos);
    } जाव(length>1);
    जइ(uchar==*pos++) {
        pos_=pos;
        int32_t ग्रन्थिः=*pos;
        प्रत्यागम ग्रन्थिः>=k_Min_मानम्_Lead ? मानम्_Result(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
    } अण्णहा {
        stop();
        प्रत्यागम USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
UCharsTrie::nextImpl(ध्रुव char16_t *pos, int32_t uchar) {
    int32_t ग्रन्थिः=*pos++;
    प्रति(;;) {
        जइ(ग्रन्थिः<kMinLinearMatch) {
            प्रत्यागम branchNext(pos, ग्रन्थिः, uchar);
        } अण्णहा जइ(ग्रन्थिः<k_Min_मानम्_Lead) {
            // Match the first of length+1 units.
            int32_t length=ग्रन्थिः-kMinLinearMatch;  // Actual match length minus 1.
            जइ(uchar==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                प्रत्यागम (length<0 && (ग्रन्थिः=*pos)>=k_Min_मानम्_Lead) ?
                        मानम्_Result(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
            } अण्णहा {
                // No match.
                भङ्ग;
            }
        } अण्णहा जइ(ग्रन्थिः&k_मानम्_Is_Final) {
            // No further matching units.
            भङ्ग;
        } अण्णहा {
            // Skip intermediate value.
            pos=skip_ग्रन्थिः_मानम्(pos, ग्रन्थिः);
            ग्रन्थिः&=k_ग्रन्थिः_Type_Mask;
        }
    }
    stop();
    प्रत्यागम USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
UCharsTrie::next(int32_t uchar) {
    ध्रुव char16_t *pos=pos_;
    जइ(pos==nullptr) {
        प्रत्यागम USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    जइ(length>=0) {
        // Remaining part of a linear-match node.
        जइ(uchar==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t ग्रन्थिः;
            प्रत्यागम (length<0 && (ग्रन्थिः=*pos)>=k_Min_मानम्_Lead) ?
                    मानम्_Result(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
        } अण्णहा {
            stop();
            प्रत्यागम USTRINGTRIE_NO_MATCH;
        }
    }
    प्रत्यागम nextImpl(pos, uchar);
}

UStringTrieResult
UCharsTrie::next(ConstChar16Ptr ptr, int32_t sLength) {
    ध्रुव char16_t *s=ptr;
    जइ(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        प्रत्यागम current();
    }
    ध्रुव char16_t *pos=pos_;
    जइ(pos==nullptr) {
        प्रत्यागम USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    प्रति(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        जइ(sLength<0) {
            प्रति(;;) {
                जइ((uchar=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t ग्रन्थिः;
                    प्रत्यागम (length<0 && (ग्रन्थिः=*pos)>=k_Min_मानम्_Lead) ?
                            मानम्_Result(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
                }
                जइ(length<0) {
                    remainingMatchLength_=length;
                    भङ्ग;
                }
                जइ(uchar!=*pos) {
                    stop();
                    प्रत्यागम USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } अण्णहा {
            प्रति(;;) {
                जइ(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t ग्रन्थिः;
                    प्रत्यागम (length<0 && (ग्रन्थिः=*pos)>=k_Min_मानम्_Lead) ?
                            मानम्_Result(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
                }
                uchar=*s++;
                --sLength;
                जइ(length<0) {
                    remainingMatchLength_=length;
                    भङ्ग;
                }
                जइ(uchar!=*pos) {
                    stop();
                    प्रत्यागम USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        int32_t ग्रन्थिः=*pos++;
        प्रति(;;) {
            जइ(ग्रन्थिः<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, ग्रन्थिः, uchar);
                जइ(result==USTRINGTRIE_NO_MATCH) {
                    प्रत्यागम USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input unit, if there is one.
                जइ(sLength<0) {
                    जइ((uchar=*s++)==0) {
                        प्रत्यागम result;
                    }
                } अण्णहा {
                    जइ(sLength==0) {
                        प्रत्यागम result;
                    }
                    uchar=*s++;
                    --sLength;
                }
                जइ(result==USTRINGTRIE_FINAL_मानम्) {
                    // No further matching units.
                    stop();
                    प्रत्यागम USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                ग्रन्थिः=*pos++;
            } अण्णहा जइ(ग्रन्थिः<k_Min_मानम्_Lead) {
                // Match length+1 units.
                length=ग्रन्थिः-kMinLinearMatch;  // Actual match length minus 1.
                जइ(uchar!=*pos) {
                    stop();
                    प्रत्यागम USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                भङ्ग;
            } अण्णहा जइ(ग्रन्थिः&k_मानम्_Is_Final) {
                // No further matching units.
                stop();
                प्रत्यागम USTRINGTRIE_NO_MATCH;
            } अण्णहा {
                // Skip intermediate value.
                pos=skip_ग्रन्थिः_मानम्(pos, ग्रन्थिः);
                ग्रन्थिः&=k_ग्रन्थिः_Type_Mask;
            }
        }
    }
}

ध्रुव char16_t*
UCharsTrie::find_Unique_मानम्_From_Branch(ध्रुव char16_t* pos U_LIFETIME_BOUND,
                                      int32_t length,
                                      UBool have_Unique_मानम्,
                                      int32_t& unique_मानम्) {
    जाव(length>k_Max_Branch_Linear_Sub_ग्रन्थिः_Length) {
        ++pos;  // ignore the comparison unit
        जइ(nullptr==find_Unique_मानम्_From_Branch(jumpByDelta(pos), length>>1, have_Unique_मानम्, unique_मानम्)) {
            प्रत्यागम nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    करेहि {
        ++pos;  // ignore a comparison unit
        // handle its value
        int32_t ग्रन्थिः=*pos++;
        UBool isFinal = static_cast<UBool>(ग्रन्थिः >> 15);
        ग्रन्थिः&=0x7fff;
        int32_t value=read_मानम्(pos, ग्रन्थिः);
        pos=skip_मानम्(pos, ग्रन्थिः);
        जइ(isFinal) {
            जइ(have_Unique_मानम्) {
                जइ(value!=unique_मानम्) {
                    प्रत्यागम nullptr;
                }
            } अण्णहा {
                unique_मानम्=value;
                have_Unique_मानम्=सच्च;
            }
        } अण्णहा {
            जइ(!find_Unique_मानम्(pos+value, have_Unique_मानम्, unique_मानम्)) {
                प्रत्यागम nullptr;
            }
            have_Unique_मानम्=सच्च;
        }
    } जाव(--length>1);
    प्रत्यागम pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::find_Unique_मानम्(ध्रुव char16_t *pos, UBool have_Unique_मानम्, int32_t &unique_मानम्) {
    int32_t ग्रन्थिः=*pos++;
    प्रति(;;) {
        जइ(ग्रन्थिः<kMinLinearMatch) {
            जइ(ग्रन्थिः==0) {
                ग्रन्थिः=*pos++;
            }
            pos=find_Unique_मानम्_From_Branch(pos, ग्रन्थिः+1, have_Unique_मानम्, unique_मानम्);
            जइ(pos==nullptr) {
                प्रत्यागम मिच्छ;
            }
            have_Unique_मानम्=सच्च;
            ग्रन्थिः=*pos++;
        } अण्णहा जइ(ग्रन्थिः<k_Min_मानम्_Lead) {
            // linear-match node
            pos+=ग्रन्थिः-kMinLinearMatch+1;  // Ignore the match units.
            ग्रन्थिः=*pos++;
        } अण्णहा {
            UBool isFinal = static_cast<UBool>(ग्रन्थिः >> 15);
            int32_t value;
            जइ(isFinal) {
                value=read_मानम्(pos, ग्रन्थिः&0x7fff);
            } अण्णहा {
                value=read_ग्रन्थिः_मानम्(pos, ग्रन्थिः);
            }
            जइ(have_Unique_मानम्) {
                जइ(value!=unique_मानम्) {
                    प्रत्यागम मिच्छ;
                }
            } अण्णहा {
                unique_मानम्=value;
                have_Unique_मानम्=सच्च;
            }
            जइ(isFinal) {
                प्रत्यागम सच्च;
            }
            pos=skip_ग्रन्थिः_मानम्(pos, ग्रन्थिः);
            ग्रन्थिः&=k_ग्रन्थिः_Type_Mask;
        }
    }
}

int32_t
UCharsTrie::getNextUChars(Appendable &out) ध्रुव {
    ध्रुव char16_t *pos=pos_;
    जइ(pos==nullptr) {
        प्रत्यागम 0;
    }
    जइ(remainingMatchLength_>=0) {
        out.appendCodeUnit(*pos);  // Next unit of a pending linear-match node.
        प्रत्यागम 1;
    }
    int32_t ग्रन्थिः=*pos++;
    जइ(ग्रन्थिः>=k_Min_मानम्_Lead) {
        जइ(ग्रन्थिः&k_मानम्_Is_Final) {
            प्रत्यागम 0;
        } अण्णहा {
            pos=skip_ग्रन्थिः_मानम्(pos, ग्रन्थिः);
            ग्रन्थिः&=k_ग्रन्थिः_Type_Mask;
        }
    }
    जइ(ग्रन्थिः<kMinLinearMatch) {
        जइ(ग्रन्थिः==0) {
            ग्रन्थिः=*pos++;
        }
        out.reserveAppendCapacity(++ग्रन्थिः);
        getNextBranchUChars(pos, ग्रन्थिः, out);
        प्रत्यागम ग्रन्थिः;
    } अण्णहा {
        // First unit of the linear-match node.
        out.appendCodeUnit(*pos);
        प्रत्यागम 1;
    }
}

शून्य
UCharsTrie::getNextBranchUChars(ध्रुव char16_t *pos, int32_t length, Appendable &out) {
    जाव(length>k_Max_Branch_Linear_Sub_ग्रन्थिः_Length) {
        ++pos;  // ignore the comparison unit
        getNextBranchUChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    करेहि {
        out.appendCodeUnit(*pos++);
        pos=skip_मानम्(pos);
    } जाव(--length>1);
    out.appendCodeUnit(*pos);
}

U_NAMESPACE_END
