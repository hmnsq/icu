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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/appendable.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucharstrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

U_NAMESPACE_BEGIN

UCharsTrie::~UCharsTrie() {
    uprv_free(ownedArray_);
}

UString_Trie_ಫಲಿತಾಂಶ
UCharsTrie::current() ಸ್ಥಿರಾಂಕ {
    ಸ್ಥಿರಾಂಕ char16_t *pos=pos_;
    ಆದರೆ(pos==ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ USTRINGTRIE_NO_ಹೊಂದಿಕೆ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        int32_t ಗಂಟು;
        ಹಿಂತಿರುಗಿ (ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ<0 && (ಗಂಟು=*pos)>=k_Min_ಮೌಲ್ಯ_Lead) ?
                ಮೌಲ್ಯ_ಫಲಿತಾಂಶ(ಗಂಟು) : USTRINGTRIE_NO_ಮೌಲ್ಯ;
    }
}

UString_Trie_ಫಲಿತಾಂಶ
UCharsTrie::ಮೊದಲ_For_ಸಂಕೇತ_ಬಿಂದು(UChar32 cp) {
    ಹಿಂತಿರುಗಿ cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_ಮುಂದಿನ(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_ಹೊಂದಿಕೆ);
}

UString_Trie_ಫಲಿತಾಂಶ
UCharsTrie::ಮುಂದಿನ_For_ಸಂಕೇತ_ಬಿಂದು(UChar32 cp) {
    ಹಿಂತಿರುಗಿ cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_ಮುಂದಿನ(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_ಹೊಂದಿಕೆ);
}

UString_Trie_ಫಲಿತಾಂಶ
UCharsTrie::branch_ಮುಂದಿನ(ಸ್ಥಿರಾಂಕ char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    ಆದರೆ(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    ಆಗ(length>k_Max_Branch_ರೇಖೀಯ_Sub_ಗಂಟು_ಉದ್ದಳತೆ) {
        ಆದರೆ(uchar<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } ಇಲ್ಲದಿದ್ದರೆ {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few units.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    ಮಾಡು {
        ಆದರೆ(uchar==*pos++) {
            UString_Trie_ಫಲಿತಾಂಶ ಫಲಿತಾಂಶ;
            int32_t ಗಂಟು=*pos;
            ಆದರೆ(ಗಂಟು&k_ಮೌಲ್ಯ_Is_Final) {
                // Leave the final value for getValue() to read.
                ಫಲಿತಾಂಶ=USTRINGTRIE_FINAL_ಮೌಲ್ಯ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                ಆದರೆ(ಗಂಟು<k_Min_Two_Unit_ಮೌಲ್ಯ_Lead) {
                    delta=ಗಂಟು;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಗಂಟು<k_Three_Unit_ಮೌಲ್ಯ_Lead) {
                    delta=((ಗಂಟು-k_Min_Two_Unit_ಮೌಲ್ಯ_Lead)<<16)|*pos++;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                ಗಂಟು=*pos;
                ಫಲಿತಾಂಶ= ಗಂಟು>=k_Min_ಮೌಲ್ಯ_Lead ? ಮೌಲ್ಯ_ಫಲಿತಾಂಶ(ಗಂಟು) : USTRINGTRIE_NO_ಮೌಲ್ಯ;
            }
            pos_=pos;
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        }
        --length;
        pos=skip_ಮೌಲ್ಯ(pos);
    } ಆಗ(length>1);
    ಆದರೆ(uchar==*pos++) {
        pos_=pos;
        int32_t ಗಂಟು=*pos;
        ಹಿಂತಿರುಗಿ ಗಂಟು>=k_Min_ಮೌಲ್ಯ_Lead ? ಮೌಲ್ಯ_ಫಲಿತಾಂಶ(ಗಂಟು) : USTRINGTRIE_NO_ಮೌಲ್ಯ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        stop();
        ಹಿಂತಿರುಗಿ USTRINGTRIE_NO_ಹೊಂದಿಕೆ;
    }
}

UString_Trie_ಫಲಿತಾಂಶ
UCharsTrie::ಮುಂದಿನ_Impl(ಸ್ಥಿರಾಂಕ char16_t *pos, int32_t uchar) {
    int32_t ಗಂಟು=*pos++;
    ಗಾಗಿ(;;) {
        ಆದರೆ(ಗಂಟು<k_Min_ರೇಖೀಯ_ಹೊಂದಿಕೆ) {
            ಹಿಂತಿರುಗಿ branch_ಮುಂದಿನ(pos, ಗಂಟು, uchar);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಗಂಟು<k_Min_ಮೌಲ್ಯ_Lead) {
            // Match the first of length+1 units.
            int32_t length=ಗಂಟು-k_Min_ರೇಖೀಯ_ಹೊಂದಿಕೆ;  // Actual match length minus 1.
            ಆದರೆ(uchar==*pos++) {
                ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ=--length;
                pos_=pos;
                ಹಿಂತಿರುಗಿ (length<0 && (ಗಂಟು=*pos)>=k_Min_ಮೌಲ್ಯ_Lead) ?
                        ಮೌಲ್ಯ_ಫಲಿತಾಂಶ(ಗಂಟು) : USTRINGTRIE_NO_ಮೌಲ್ಯ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                // No match.
                ನಿಲ್ಲಿಸು;
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಗಂಟು&k_ಮೌಲ್ಯ_Is_Final) {
            // No further matching units.
            ನಿಲ್ಲಿಸು;
        } ಇಲ್ಲದಿದ್ದರೆ {
            // Skip intermediate value.
            pos=skip_ಗಂಟು_ಮೌಲ್ಯ(pos, ಗಂಟು);
            ಗಂಟು&=k_ಗಂಟು_Type_ಮುಖವಾಡ;
        }
    }
    stop();
    ಹಿಂತಿರುಗಿ USTRINGTRIE_NO_ಹೊಂದಿಕೆ;
}

UString_Trie_ಫಲಿತಾಂಶ
UCharsTrie::next(int32_t uchar) {
    ಸ್ಥಿರಾಂಕ char16_t *pos=pos_;
    ಆದರೆ(pos==ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ USTRINGTRIE_NO_ಹೊಂದಿಕೆ;
    }
    int32_t length=ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ;  // Actual remaining match length minus 1.
    ಆದರೆ(length>=0) {
        // Remaining part of a linear-match node.
        ಆದರೆ(uchar==*pos++) {
            ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ=--length;
            pos_=pos;
            int32_t ಗಂಟು;
            ಹಿಂತಿರುಗಿ (length<0 && (ಗಂಟು=*pos)>=k_Min_ಮೌಲ್ಯ_Lead) ?
                    ಮೌಲ್ಯ_ಫಲಿತಾಂಶ(ಗಂಟು) : USTRINGTRIE_NO_ಮೌಲ್ಯ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            stop();
            ಹಿಂತಿರುಗಿ USTRINGTRIE_NO_ಹೊಂದಿಕೆ;
        }
    }
    ಹಿಂತಿರುಗಿ ಮುಂದಿನ_Impl(pos, uchar);
}

UString_Trie_ಫಲಿತಾಂಶ
UCharsTrie::next(ConstChar16Ptr ptr, int32_t s_ಉದ್ದಳತೆ) {
    ಸ್ಥಿರಾಂಕ char16_t *s=ptr;
    ಆದರೆ(s_ಉದ್ದಳತೆ<0 ? *s==0 : s_ಉದ್ದಳತೆ==0) {
        // Empty input.
        ಹಿಂತಿರುಗಿ current();
    }
    ಸ್ಥಿರಾಂಕ char16_t *pos=pos_;
    ಆದರೆ(pos==ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ USTRINGTRIE_NO_ಹೊಂದಿಕೆ;
    }
    int32_t length=ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ;  // Actual remaining match length minus 1.
    ಗಾಗಿ(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        ಆದರೆ(s_ಉದ್ದಳತೆ<0) {
            ಗಾಗಿ(;;) {
                ಆದರೆ((uchar=*s++)==0) {
                    ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ=length;
                    pos_=pos;
                    int32_t ಗಂಟು;
                    ಹಿಂತಿರುಗಿ (length<0 && (ಗಂಟು=*pos)>=k_Min_ಮೌಲ್ಯ_Lead) ?
                            ಮೌಲ್ಯ_ಫಲಿತಾಂಶ(ಗಂಟು) : USTRINGTRIE_NO_ಮೌಲ್ಯ;
                }
                ಆದರೆ(length<0) {
                    ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ=length;
                    ನಿಲ್ಲಿಸು;
                }
                ಆದರೆ(uchar!=*pos) {
                    stop();
                    ಹಿಂತಿರುಗಿ USTRINGTRIE_NO_ಹೊಂದಿಕೆ;
                }
                ++pos;
                --length;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಗಾಗಿ(;;) {
                ಆದರೆ(s_ಉದ್ದಳತೆ==0) {
                    ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ=length;
                    pos_=pos;
                    int32_t ಗಂಟು;
                    ಹಿಂತಿರುಗಿ (length<0 && (ಗಂಟು=*pos)>=k_Min_ಮೌಲ್ಯ_Lead) ?
                            ಮೌಲ್ಯ_ಫಲಿತಾಂಶ(ಗಂಟು) : USTRINGTRIE_NO_ಮೌಲ್ಯ;
                }
                uchar=*s++;
                --s_ಉದ್ದಳತೆ;
                ಆದರೆ(length<0) {
                    ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ=length;
                    ನಿಲ್ಲಿಸು;
                }
                ಆದರೆ(uchar!=*pos) {
                    stop();
                    ಹಿಂತಿರುಗಿ USTRINGTRIE_NO_ಹೊಂದಿಕೆ;
                }
                ++pos;
                --length;
            }
        }
        int32_t ಗಂಟು=*pos++;
        ಗಾಗಿ(;;) {
            ಆದರೆ(ಗಂಟು<k_Min_ರೇಖೀಯ_ಹೊಂದಿಕೆ) {
                UString_Trie_ಫಲಿತಾಂಶ ಫಲಿತಾಂಶ=branch_ಮುಂದಿನ(pos, ಗಂಟು, uchar);
                ಆದರೆ(ಫಲಿತಾಂಶ==USTRINGTRIE_NO_ಹೊಂದಿಕೆ) {
                    ಹಿಂತಿರುಗಿ USTRINGTRIE_NO_ಹೊಂದಿಕೆ;
                }
                // Fetch the next input unit, if there is one.
                ಆದರೆ(s_ಉದ್ದಳತೆ<0) {
                    ಆದರೆ((uchar=*s++)==0) {
                        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಆದರೆ(s_ಉದ್ದಳತೆ==0) {
                        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
                    }
                    uchar=*s++;
                    --s_ಉದ್ದಳತೆ;
                }
                ಆದರೆ(ಫಲಿತಾಂಶ==USTRINGTRIE_FINAL_ಮೌಲ್ಯ) {
                    // No further matching units.
                    stop();
                    ಹಿಂತಿರುಗಿ USTRINGTRIE_NO_ಹೊಂದಿಕೆ;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                ಗಂಟು=*pos++;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಗಂಟು<k_Min_ಮೌಲ್ಯ_Lead) {
                // Match length+1 units.
                length=ಗಂಟು-k_Min_ರೇಖೀಯ_ಹೊಂದಿಕೆ;  // Actual match length minus 1.
                ಆದರೆ(uchar!=*pos) {
                    stop();
                    ಹಿಂತಿರುಗಿ USTRINGTRIE_NO_ಹೊಂದಿಕೆ;
                }
                ++pos;
                --length;
                ನಿಲ್ಲಿಸು;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಗಂಟು&k_ಮೌಲ್ಯ_Is_Final) {
                // No further matching units.
                stop();
                ಹಿಂತಿರುಗಿ USTRINGTRIE_NO_ಹೊಂದಿಕೆ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                // Skip intermediate value.
                pos=skip_ಗಂಟು_ಮೌಲ್ಯ(pos, ಗಂಟು);
                ಗಂಟು&=k_ಗಂಟು_Type_ಮುಖವಾಡ;
            }
        }
    }
}

ಸ್ಥಿರಾಂಕ char16_t*
UCharsTrie::ಕಂಡುಹಿಡಿ_Unique_ಮೌಲ್ಯ_From_Branch(ಸ್ಥಿರಾಂಕ char16_t* pos U_LIFETIME_BOUND,
                                      int32_t length,
                                      UBool have_Unique_ಮೌಲ್ಯ,
                                      int32_t& unique_ಮೌಲ್ಯ) {
    ಆಗ(length>k_Max_Branch_ರೇಖೀಯ_Sub_ಗಂಟು_ಉದ್ದಳತೆ) {
        ++pos;  // ignore the comparison unit
        ಆದರೆ(ಶೂನ್ಯಸೂಚಕ==ಕಂಡುಹಿಡಿ_Unique_ಮೌಲ್ಯ_From_Branch(jumpByDelta(pos), length>>1, have_Unique_ಮೌಲ್ಯ, unique_ಮೌಲ್ಯ)) {
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    ಮಾಡು {
        ++pos;  // ignore a comparison unit
        // handle its value
        int32_t ಗಂಟು=*pos++;
        UBool isFinal = static_cast<UBool>(ಗಂಟು >> 15);
        ಗಂಟು&=0x7fff;
        int32_t value=read_ಮೌಲ್ಯ(pos, ಗಂಟು);
        pos=skip_ಮೌಲ್ಯ(pos, ಗಂಟು);
        ಆದರೆ(isFinal) {
            ಆದರೆ(have_Unique_ಮೌಲ್ಯ) {
                ಆದರೆ(value!=unique_ಮೌಲ್ಯ) {
                    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                unique_ಮೌಲ್ಯ=value;
                have_Unique_ಮೌಲ್ಯ=ನಿಜ;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ(!ಕಂಡುಹಿಡಿ_Unique_ಮೌಲ್ಯ(pos+value, have_Unique_ಮೌಲ್ಯ, unique_ಮೌಲ್ಯ)) {
                ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
            }
            have_Unique_ಮೌಲ್ಯ=ನಿಜ;
        }
    } ಆಗ(--length>1);
    ಹಿಂತಿರುಗಿ pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::ಕಂಡುಹಿಡಿ_Unique_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ char16_t *pos, UBool have_Unique_ಮೌಲ್ಯ, int32_t &unique_ಮೌಲ್ಯ) {
    int32_t ಗಂಟು=*pos++;
    ಗಾಗಿ(;;) {
        ಆದರೆ(ಗಂಟು<k_Min_ರೇಖೀಯ_ಹೊಂದಿಕೆ) {
            ಆದರೆ(ಗಂಟು==0) {
                ಗಂಟು=*pos++;
            }
            pos=ಕಂಡುಹಿಡಿ_Unique_ಮೌಲ್ಯ_From_Branch(pos, ಗಂಟು+1, have_Unique_ಮೌಲ್ಯ, unique_ಮೌಲ್ಯ);
            ಆದರೆ(pos==ಶೂನ್ಯಸೂಚಕ) {
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
            have_Unique_ಮೌಲ್ಯ=ನಿಜ;
            ಗಂಟು=*pos++;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಗಂಟು<k_Min_ಮೌಲ್ಯ_Lead) {
            // linear-match node
            pos+=ಗಂಟು-k_Min_ರೇಖೀಯ_ಹೊಂದಿಕೆ+1;  // Ignore the match units.
            ಗಂಟು=*pos++;
        } ಇಲ್ಲದಿದ್ದರೆ {
            UBool isFinal = static_cast<UBool>(ಗಂಟು >> 15);
            int32_t value;
            ಆದರೆ(isFinal) {
                value=read_ಮೌಲ್ಯ(pos, ಗಂಟು&0x7fff);
            } ಇಲ್ಲದಿದ್ದರೆ {
                value=read_ಗಂಟು_ಮೌಲ್ಯ(pos, ಗಂಟು);
            }
            ಆದರೆ(have_Unique_ಮೌಲ್ಯ) {
                ಆದರೆ(value!=unique_ಮೌಲ್ಯ) {
                    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                unique_ಮೌಲ್ಯ=value;
                have_Unique_ಮೌಲ್ಯ=ನಿಜ;
            }
            ಆದರೆ(isFinal) {
                ಹಿಂತಿರುಗಿ ನಿಜ;
            }
            pos=skip_ಗಂಟು_ಮೌಲ್ಯ(pos, ಗಂಟು);
            ಗಂಟು&=k_ಗಂಟು_Type_ಮುಖವಾಡ;
        }
    }
}

int32_t
UCharsTrie::get_ಮುಂದಿನ_UChars(Appendable &out) ಸ್ಥಿರಾಂಕ {
    ಸ್ಥಿರಾಂಕ char16_t *pos=pos_;
    ಆದರೆ(pos==ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ>=0) {
        out.append_ಸಂಕೇತ_Unit(*pos);  // Next unit of a pending linear-match node.
        ಹಿಂತಿರುಗಿ 1;
    }
    int32_t ಗಂಟು=*pos++;
    ಆದರೆ(ಗಂಟು>=k_Min_ಮೌಲ್ಯ_Lead) {
        ಆದರೆ(ಗಂಟು&k_ಮೌಲ್ಯ_Is_Final) {
            ಹಿಂತಿರುಗಿ 0;
        } ಇಲ್ಲದಿದ್ದರೆ {
            pos=skip_ಗಂಟು_ಮೌಲ್ಯ(pos, ಗಂಟು);
            ಗಂಟು&=k_ಗಂಟು_Type_ಮುಖವಾಡ;
        }
    }
    ಆದರೆ(ಗಂಟು<k_Min_ರೇಖೀಯ_ಹೊಂದಿಕೆ) {
        ಆದರೆ(ಗಂಟು==0) {
            ಗಂಟು=*pos++;
        }
        out.reserve_Append_ಸಾಮರ್ಥ್ಯ(++ಗಂಟು);
        get_ಮುಂದಿನ_Branch_UChars(pos, ಗಂಟು, out);
        ಹಿಂತಿರುಗಿ ಗಂಟು;
    } ಇಲ್ಲದಿದ್ದರೆ {
        // First unit of the linear-match node.
        out.append_ಸಂಕೇತ_Unit(*pos);
        ಹಿಂತಿರುಗಿ 1;
    }
}

ಶೂನ್ಯ
UCharsTrie::get_ಮುಂದಿನ_Branch_UChars(ಸ್ಥಿರಾಂಕ char16_t *pos, int32_t length, Appendable &out) {
    ಆಗ(length>k_Max_Branch_ರೇಖೀಯ_Sub_ಗಂಟು_ಉದ್ದಳತೆ) {
        ++pos;  // ignore the comparison unit
        get_ಮುಂದಿನ_Branch_UChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    ಮಾಡು {
        out.append_ಸಂಕೇತ_Unit(*pos++);
        pos=skip_ಮೌಲ್ಯ(pos);
    } ಆಗ(--length>1);
    out.append_ಸಂಕೇತ_Unit(*pos);
}

U_NAMESPACE_ಕೊನೆ
