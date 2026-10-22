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
UCharsTrie::current() स्थिरांक {
    स्थिरांक char16_t *pos=pos_;
    जर(pos==nullptr) {
        परत USTRINGTRIE_NO_MATCH;
    } अन्यथा {
        int32_t गाठ;
        परत (remainingMatchLength_<0 && (गाठ=*pos)>=k_Min_मूल्य_Lead) ?
                मूल्य_Result(गाठ) : USTRINGTRIE_NO_मूल्य;
    }
}

UStringTrieResult
UCharsTrie::firstForCodePoint(UChar32 cp) {
    परत cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_NEXT(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::nextForCodePoint(UChar32 cp) {
    परत cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_NEXT(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::branchNext(स्थिरांक char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    जर(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    पर्यंत(length>k_Max_Branch_Linear_Sub_गाठ_Length) {
        जर(uchar<*pos++) {
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
    करा {
        जर(uchar==*pos++) {
            UStringTrieResult result;
            int32_t गाठ=*pos;
            जर(गाठ&k_मूल्य_Is_Final) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_मूल्य;
            } अन्यथा {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                जर(गाठ<k_Min_Two_Unit_मूल्य_Lead) {
                    delta=गाठ;
                } अन्यथा जर(गाठ<k_Three_Unit_मूल्य_Lead) {
                    delta=((गाठ-k_Min_Two_Unit_मूल्य_Lead)<<16)|*pos++;
                } अन्यथा {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                गाठ=*pos;
                result= गाठ>=k_Min_मूल्य_Lead ? मूल्य_Result(गाठ) : USTRINGTRIE_NO_मूल्य;
            }
            pos_=pos;
            परत result;
        }
        --length;
        pos=skip_मूल्य(pos);
    } पर्यंत(length>1);
    जर(uchar==*pos++) {
        pos_=pos;
        int32_t गाठ=*pos;
        परत गाठ>=k_Min_मूल्य_Lead ? मूल्य_Result(गाठ) : USTRINGTRIE_NO_मूल्य;
    } अन्यथा {
        stop();
        परत USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
UCharsTrie::nextImpl(स्थिरांक char16_t *pos, int32_t uchar) {
    int32_t गाठ=*pos++;
    साठी(;;) {
        जर(गाठ<kMinLinearMatch) {
            परत branchNext(pos, गाठ, uchar);
        } अन्यथा जर(गाठ<k_Min_मूल्य_Lead) {
            // Match the first of length+1 units.
            int32_t length=गाठ-kMinLinearMatch;  // Actual match length minus 1.
            जर(uchar==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                परत (length<0 && (गाठ=*pos)>=k_Min_मूल्य_Lead) ?
                        मूल्य_Result(गाठ) : USTRINGTRIE_NO_मूल्य;
            } अन्यथा {
                // No match.
                थांबा;
            }
        } अन्यथा जर(गाठ&k_मूल्य_Is_Final) {
            // No further matching units.
            थांबा;
        } अन्यथा {
            // Skip intermediate value.
            pos=skip_गाठ_मूल्य(pos, गाठ);
            गाठ&=k_गाठ_Type_Mask;
        }
    }
    stop();
    परत USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
UCharsTrie::next(int32_t uchar) {
    स्थिरांक char16_t *pos=pos_;
    जर(pos==nullptr) {
        परत USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    जर(length>=0) {
        // Remaining part of a linear-match node.
        जर(uchar==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t गाठ;
            परत (length<0 && (गाठ=*pos)>=k_Min_मूल्य_Lead) ?
                    मूल्य_Result(गाठ) : USTRINGTRIE_NO_मूल्य;
        } अन्यथा {
            stop();
            परत USTRINGTRIE_NO_MATCH;
        }
    }
    परत nextImpl(pos, uchar);
}

UStringTrieResult
UCharsTrie::next(ConstChar16Ptr ptr, int32_t sLength) {
    स्थिरांक char16_t *s=ptr;
    जर(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        परत current();
    }
    स्थिरांक char16_t *pos=pos_;
    जर(pos==nullptr) {
        परत USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    साठी(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        जर(sLength<0) {
            साठी(;;) {
                जर((uchar=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t गाठ;
                    परत (length<0 && (गाठ=*pos)>=k_Min_मूल्य_Lead) ?
                            मूल्य_Result(गाठ) : USTRINGTRIE_NO_मूल्य;
                }
                जर(length<0) {
                    remainingMatchLength_=length;
                    थांबा;
                }
                जर(uchar!=*pos) {
                    stop();
                    परत USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } अन्यथा {
            साठी(;;) {
                जर(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t गाठ;
                    परत (length<0 && (गाठ=*pos)>=k_Min_मूल्य_Lead) ?
                            मूल्य_Result(गाठ) : USTRINGTRIE_NO_मूल्य;
                }
                uchar=*s++;
                --sLength;
                जर(length<0) {
                    remainingMatchLength_=length;
                    थांबा;
                }
                जर(uchar!=*pos) {
                    stop();
                    परत USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        int32_t गाठ=*pos++;
        साठी(;;) {
            जर(गाठ<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, गाठ, uchar);
                जर(result==USTRINGTRIE_NO_MATCH) {
                    परत USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input unit, if there is one.
                जर(sLength<0) {
                    जर((uchar=*s++)==0) {
                        परत result;
                    }
                } अन्यथा {
                    जर(sLength==0) {
                        परत result;
                    }
                    uchar=*s++;
                    --sLength;
                }
                जर(result==USTRINGTRIE_FINAL_मूल्य) {
                    // No further matching units.
                    stop();
                    परत USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                गाठ=*pos++;
            } अन्यथा जर(गाठ<k_Min_मूल्य_Lead) {
                // Match length+1 units.
                length=गाठ-kMinLinearMatch;  // Actual match length minus 1.
                जर(uchar!=*pos) {
                    stop();
                    परत USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                थांबा;
            } अन्यथा जर(गाठ&k_मूल्य_Is_Final) {
                // No further matching units.
                stop();
                परत USTRINGTRIE_NO_MATCH;
            } अन्यथा {
                // Skip intermediate value.
                pos=skip_गाठ_मूल्य(pos, गाठ);
                गाठ&=k_गाठ_Type_Mask;
            }
        }
    }
}

स्थिरांक char16_t*
UCharsTrie::find_Unique_मूल्य_From_Branch(स्थिरांक char16_t* pos U_LIFETIME_BOUND,
                                      int32_t length,
                                      UBool have_Unique_मूल्य,
                                      int32_t& unique_मूल्य) {
    पर्यंत(length>k_Max_Branch_Linear_Sub_गाठ_Length) {
        ++pos;  // ignore the comparison unit
        जर(nullptr==find_Unique_मूल्य_From_Branch(jumpByDelta(pos), length>>1, have_Unique_मूल्य, unique_मूल्य)) {
            परत nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    करा {
        ++pos;  // ignore a comparison unit
        // handle its value
        int32_t गाठ=*pos++;
        UBool isFinal = static_cast<UBool>(गाठ >> 15);
        गाठ&=0x7fff;
        int32_t value=read_मूल्य(pos, गाठ);
        pos=skip_मूल्य(pos, गाठ);
        जर(isFinal) {
            जर(have_Unique_मूल्य) {
                जर(value!=unique_मूल्य) {
                    परत nullptr;
                }
            } अन्यथा {
                unique_मूल्य=value;
                have_Unique_मूल्य=सत्य;
            }
        } अन्यथा {
            जर(!find_Unique_मूल्य(pos+value, have_Unique_मूल्य, unique_मूल्य)) {
                परत nullptr;
            }
            have_Unique_मूल्य=सत्य;
        }
    } पर्यंत(--length>1);
    परत pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::find_Unique_मूल्य(स्थिरांक char16_t *pos, UBool have_Unique_मूल्य, int32_t &unique_मूल्य) {
    int32_t गाठ=*pos++;
    साठी(;;) {
        जर(गाठ<kMinLinearMatch) {
            जर(गाठ==0) {
                गाठ=*pos++;
            }
            pos=find_Unique_मूल्य_From_Branch(pos, गाठ+1, have_Unique_मूल्य, unique_मूल्य);
            जर(pos==nullptr) {
                परत असत्य;
            }
            have_Unique_मूल्य=सत्य;
            गाठ=*pos++;
        } अन्यथा जर(गाठ<k_Min_मूल्य_Lead) {
            // linear-match node
            pos+=गाठ-kMinLinearMatch+1;  // Ignore the match units.
            गाठ=*pos++;
        } अन्यथा {
            UBool isFinal = static_cast<UBool>(गाठ >> 15);
            int32_t value;
            जर(isFinal) {
                value=read_मूल्य(pos, गाठ&0x7fff);
            } अन्यथा {
                value=read_गाठ_मूल्य(pos, गाठ);
            }
            जर(have_Unique_मूल्य) {
                जर(value!=unique_मूल्य) {
                    परत असत्य;
                }
            } अन्यथा {
                unique_मूल्य=value;
                have_Unique_मूल्य=सत्य;
            }
            जर(isFinal) {
                परत सत्य;
            }
            pos=skip_गाठ_मूल्य(pos, गाठ);
            गाठ&=k_गाठ_Type_Mask;
        }
    }
}

int32_t
UCharsTrie::getNextUChars(Appendable &out) स्थिरांक {
    स्थिरांक char16_t *pos=pos_;
    जर(pos==nullptr) {
        परत 0;
    }
    जर(remainingMatchLength_>=0) {
        out.appendCodeUnit(*pos);  // Next unit of a pending linear-match node.
        परत 1;
    }
    int32_t गाठ=*pos++;
    जर(गाठ>=k_Min_मूल्य_Lead) {
        जर(गाठ&k_मूल्य_Is_Final) {
            परत 0;
        } अन्यथा {
            pos=skip_गाठ_मूल्य(pos, गाठ);
            गाठ&=k_गाठ_Type_Mask;
        }
    }
    जर(गाठ<kMinLinearMatch) {
        जर(गाठ==0) {
            गाठ=*pos++;
        }
        out.reserveAppendCapacity(++गाठ);
        getNextBranchUChars(pos, गाठ, out);
        परत गाठ;
    } अन्यथा {
        // First unit of the linear-match node.
        out.appendCodeUnit(*pos);
        परत 1;
    }
}

शून्य
UCharsTrie::getNextBranchUChars(स्थिरांक char16_t *pos, int32_t length, Appendable &out) {
    पर्यंत(length>k_Max_Branch_Linear_Sub_गाठ_Length) {
        ++pos;  // ignore the comparison unit
        getNextBranchUChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    करा {
        out.appendCodeUnit(*pos++);
        pos=skip_मूल्य(pos);
    } पर्यंत(--length>1);
    out.appendCodeUnit(*pos);
}

U_NAMESPACE_END
