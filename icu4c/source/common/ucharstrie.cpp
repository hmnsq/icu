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
    सचे(pos==nullptr) {
        पटिनिवत्त USTRINGTRIE_NO_MATCH;
    } अञ्ञथा {
        int32_t ग्रन्थिः;
        पटिनिवत्त (remainingMatchLength_<0 && (ग्रन्थिः=*pos)>=k_Min_मानम्_Lead) ?
                मानम्_Result(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
    }
}

UStringTrieResult
UCharsTrie::firstForCodePoint(UChar32 cp) {
    पटिनिवत्त cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_NEXT(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::nextForCodePoint(UChar32 cp) {
    पटिनिवत्त cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_NEXT(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::branchNext(ध्रुव char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    सचे(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    याव(length>k_Max_Branch_Linear_Sub_ग्रन्थिः_Length) {
        सचे(uchar<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } अञ्ञथा {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few units.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    करोहि {
        सचे(uchar==*pos++) {
            UStringTrieResult result;
            int32_t ग्रन्थिः=*pos;
            सचे(ग्रन्थिः&k_मानम्_Is_Final) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_मानम्;
            } अञ्ञथा {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                सचे(ग्रन्थिः<k_Min_Two_Unit_मानम्_Lead) {
                    delta=ग्रन्थिः;
                } अञ्ञथा सचे(ग्रन्थिः<k_Three_Unit_मानम्_Lead) {
                    delta=((ग्रन्थिः-k_Min_Two_Unit_मानम्_Lead)<<16)|*pos++;
                } अञ्ञथा {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                ग्रन्थिः=*pos;
                result= ग्रन्थिः>=k_Min_मानम्_Lead ? मानम्_Result(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
            }
            pos_=pos;
            पटिनिवत्त result;
        }
        --length;
        pos=skip_मानम्(pos);
    } याव(length>1);
    सचे(uchar==*pos++) {
        pos_=pos;
        int32_t ग्रन्थिः=*pos;
        पटिनिवत्त ग्रन्थिः>=k_Min_मानम्_Lead ? मानम्_Result(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
    } अञ्ञथा {
        stop();
        पटिनिवत्त USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
UCharsTrie::nextImpl(ध्रुव char16_t *pos, int32_t uchar) {
    int32_t ग्रन्थिः=*pos++;
    अत्थाय(;;) {
        सचे(ग्रन्थिः<kMinLinearMatch) {
            पटिनिवत्त branchNext(pos, ग्रन्थिः, uchar);
        } अञ्ञथा सचे(ग्रन्थिः<k_Min_मानम्_Lead) {
            // Match the first of length+1 units.
            int32_t length=ग्रन्थिः-kMinLinearMatch;  // Actual match length minus 1.
            सचे(uchar==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                पटिनिवत्त (length<0 && (ग्रन्थिः=*pos)>=k_Min_मानम्_Lead) ?
                        मानम्_Result(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
            } अञ्ञथा {
                // No match.
                भङ्ग;
            }
        } अञ्ञथा सचे(ग्रन्थिः&k_मानम्_Is_Final) {
            // No further matching units.
            भङ्ग;
        } अञ्ञथा {
            // Skip intermediate value.
            pos=skip_ग्रन्थिः_मानम्(pos, ग्रन्थिः);
            ग्रन्थिः&=k_ग्रन्थिः_Type_Mask;
        }
    }
    stop();
    पटिनिवत्त USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
UCharsTrie::next(int32_t uchar) {
    ध्रुव char16_t *pos=pos_;
    सचे(pos==nullptr) {
        पटिनिवत्त USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    सचे(length>=0) {
        // Remaining part of a linear-match node.
        सचे(uchar==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t ग्रन्थिः;
            पटिनिवत्त (length<0 && (ग्रन्थिः=*pos)>=k_Min_मानम्_Lead) ?
                    मानम्_Result(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
        } अञ्ञथा {
            stop();
            पटिनिवत्त USTRINGTRIE_NO_MATCH;
        }
    }
    पटिनिवत्त nextImpl(pos, uchar);
}

UStringTrieResult
UCharsTrie::next(ConstChar16Ptr ptr, int32_t sLength) {
    ध्रुव char16_t *s=ptr;
    सचे(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        पटिनिवत्त current();
    }
    ध्रुव char16_t *pos=pos_;
    सचे(pos==nullptr) {
        पटिनिवत्त USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    अत्थाय(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        सचे(sLength<0) {
            अत्थाय(;;) {
                सचे((uchar=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t ग्रन्थिः;
                    पटिनिवत्त (length<0 && (ग्रन्थिः=*pos)>=k_Min_मानम्_Lead) ?
                            मानम्_Result(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
                }
                सचे(length<0) {
                    remainingMatchLength_=length;
                    भङ्ग;
                }
                सचे(uchar!=*pos) {
                    stop();
                    पटिनिवत्त USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } अञ्ञथा {
            अत्थाय(;;) {
                सचे(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t ग्रन्थिः;
                    पटिनिवत्त (length<0 && (ग्रन्थिः=*pos)>=k_Min_मानम्_Lead) ?
                            मानम्_Result(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
                }
                uchar=*s++;
                --sLength;
                सचे(length<0) {
                    remainingMatchLength_=length;
                    भङ्ग;
                }
                सचे(uchar!=*pos) {
                    stop();
                    पटिनिवत्त USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        int32_t ग्रन्थिः=*pos++;
        अत्थाय(;;) {
            सचे(ग्रन्थिः<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, ग्रन्थिः, uchar);
                सचे(result==USTRINGTRIE_NO_MATCH) {
                    पटिनिवत्त USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input unit, if there is one.
                सचे(sLength<0) {
                    सचे((uchar=*s++)==0) {
                        पटिनिवत्त result;
                    }
                } अञ्ञथा {
                    सचे(sLength==0) {
                        पटिनिवत्त result;
                    }
                    uchar=*s++;
                    --sLength;
                }
                सचे(result==USTRINGTRIE_FINAL_मानम्) {
                    // No further matching units.
                    stop();
                    पटिनिवत्त USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                ग्रन्थिः=*pos++;
            } अञ्ञथा सचे(ग्रन्थिः<k_Min_मानम्_Lead) {
                // Match length+1 units.
                length=ग्रन्थिः-kMinLinearMatch;  // Actual match length minus 1.
                सचे(uchar!=*pos) {
                    stop();
                    पटिनिवत्त USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                भङ्ग;
            } अञ्ञथा सचे(ग्रन्थिः&k_मानम्_Is_Final) {
                // No further matching units.
                stop();
                पटिनिवत्त USTRINGTRIE_NO_MATCH;
            } अञ्ञथा {
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
    याव(length>k_Max_Branch_Linear_Sub_ग्रन्थिः_Length) {
        ++pos;  // ignore the comparison unit
        सचे(nullptr==find_Unique_मानम्_From_Branch(jumpByDelta(pos), length>>1, have_Unique_मानम्, unique_मानम्)) {
            पटिनिवत्त nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    करोहि {
        ++pos;  // ignore a comparison unit
        // handle its value
        int32_t ग्रन्थिः=*pos++;
        UBool isFinal = static_cast<UBool>(ग्रन्थिः >> 15);
        ग्रन्थिः&=0x7fff;
        int32_t value=read_मानम्(pos, ग्रन्थिः);
        pos=skip_मानम्(pos, ग्रन्थिः);
        सचे(isFinal) {
            सचे(have_Unique_मानम्) {
                सचे(value!=unique_मानम्) {
                    पटिनिवत्त nullptr;
                }
            } अञ्ञथा {
                unique_मानम्=value;
                have_Unique_मानम्=सच्च;
            }
        } अञ्ञथा {
            सचे(!find_Unique_मानम्(pos+value, have_Unique_मानम्, unique_मानम्)) {
                पटिनिवत्त nullptr;
            }
            have_Unique_मानम्=सच्च;
        }
    } याव(--length>1);
    पटिनिवत्त pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::find_Unique_मानम्(ध्रुव char16_t *pos, UBool have_Unique_मानम्, int32_t &unique_मानम्) {
    int32_t ग्रन्थिः=*pos++;
    अत्थाय(;;) {
        सचे(ग्रन्थिः<kMinLinearMatch) {
            सचे(ग्रन्थिः==0) {
                ग्रन्थिः=*pos++;
            }
            pos=find_Unique_मानम्_From_Branch(pos, ग्रन्थिः+1, have_Unique_मानम्, unique_मानम्);
            सचे(pos==nullptr) {
                पटिनिवत्त मिच्छा;
            }
            have_Unique_मानम्=सच्च;
            ग्रन्थिः=*pos++;
        } अञ्ञथा सचे(ग्रन्थिः<k_Min_मानम्_Lead) {
            // linear-match node
            pos+=ग्रन्थिः-kMinLinearMatch+1;  // Ignore the match units.
            ग्रन्थिः=*pos++;
        } अञ्ञथा {
            UBool isFinal = static_cast<UBool>(ग्रन्थिः >> 15);
            int32_t value;
            सचे(isFinal) {
                value=read_मानम्(pos, ग्रन्थिः&0x7fff);
            } अञ्ञथा {
                value=read_ग्रन्थिः_मानम्(pos, ग्रन्थिः);
            }
            सचे(have_Unique_मानम्) {
                सचे(value!=unique_मानम्) {
                    पटिनिवत्त मिच्छा;
                }
            } अञ्ञथा {
                unique_मानम्=value;
                have_Unique_मानम्=सच्च;
            }
            सचे(isFinal) {
                पटिनिवत्त सच्च;
            }
            pos=skip_ग्रन्थिः_मानम्(pos, ग्रन्थिः);
            ग्रन्थिः&=k_ग्रन्थिः_Type_Mask;
        }
    }
}

int32_t
UCharsTrie::getNextUChars(Appendable &out) ध्रुव {
    ध्रुव char16_t *pos=pos_;
    सचे(pos==nullptr) {
        पटिनिवत्त 0;
    }
    सचे(remainingMatchLength_>=0) {
        out.appendCodeUnit(*pos);  // Next unit of a pending linear-match node.
        पटिनिवत्त 1;
    }
    int32_t ग्रन्थिः=*pos++;
    सचे(ग्रन्थिः>=k_Min_मानम्_Lead) {
        सचे(ग्रन्थिः&k_मानम्_Is_Final) {
            पटिनिवत्त 0;
        } अञ्ञथा {
            pos=skip_ग्रन्थिः_मानम्(pos, ग्रन्थिः);
            ग्रन्थिः&=k_ग्रन्थिः_Type_Mask;
        }
    }
    सचे(ग्रन्थिः<kMinLinearMatch) {
        सचे(ग्रन्थिः==0) {
            ग्रन्थिः=*pos++;
        }
        out.reserveAppendCapacity(++ग्रन्थिः);
        getNextBranchUChars(pos, ग्रन्थिः, out);
        पटिनिवत्त ग्रन्थिः;
    } अञ्ञथा {
        // First unit of the linear-match node.
        out.appendCodeUnit(*pos);
        पटिनिवत्त 1;
    }
}

शून्य
UCharsTrie::getNextBranchUChars(ध्रुव char16_t *pos, int32_t length, Appendable &out) {
    याव(length>k_Max_Branch_Linear_Sub_ग्रन्थिः_Length) {
        ++pos;  // ignore the comparison unit
        getNextBranchUChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    करोहि {
        out.appendCodeUnit(*pos++);
        pos=skip_मानम्(pos);
    } याव(--length>1);
    out.appendCodeUnit(*pos);
}

U_NAMESPACE_END
