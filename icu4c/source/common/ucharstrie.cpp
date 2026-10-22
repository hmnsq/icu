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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/appendable.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucharstrie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"

U_NAMESPACE_BEGIN

UCharsTrie::~UCharsTrie() {
    uprv_free(ownedArray_);
}

UString_Trie_ਨਤੀਜਾ
UCharsTrie::current() ਅਚਲ {
    ਅਚਲ char16_t *pos=pos_;
    ਜੇ(pos==ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ USTRINGTRIE_NO_ਮੇਲ;
    } ਨਹੀਂਤਾਂ {
        int32_t ਗੰਢ;
        ਵਾਪਸ (ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ<0 && (ਗੰਢ=*pos)>=k_Min_ਮੁੱਲ_Lead) ?
                ਮੁੱਲ_ਨਤੀਜਾ(ਗੰਢ) : USTRINGTRIE_NO_ਮੁੱਲ;
    }
}

UString_Trie_ਨਤੀਜਾ
UCharsTrie::ਪਹਿਲਾ_For_ਕੋਡ_ਬਿੰਦੂ(UChar32 cp) {
    ਵਾਪਸ cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_ਅਗਲਾ(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_ਮੇਲ);
}

UString_Trie_ਨਤੀਜਾ
UCharsTrie::ਅਗਲਾ_For_ਕੋਡ_ਬਿੰਦੂ(UChar32 cp) {
    ਵਾਪਸ cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_ਅਗਲਾ(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_ਮੇਲ);
}

UString_Trie_ਨਤੀਜਾ
UCharsTrie::branch_ਅਗਲਾ(ਅਚਲ char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    ਜੇ(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    ਜਦੋਂ(length>k_Max_Branch_ਰੇਖਿਕ_Sub_ਗੰਢ_ਲੰਮਾਈ) {
        ਜੇ(uchar<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } ਨਹੀਂਤਾਂ {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few units.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    ਕਰੋ {
        ਜੇ(uchar==*pos++) {
            UString_Trie_ਨਤੀਜਾ ਨਤੀਜਾ;
            int32_t ਗੰਢ=*pos;
            ਜੇ(ਗੰਢ&k_ਮੁੱਲ_Is_Final) {
                // Leave the final value for getValue() to read.
                ਨਤੀਜਾ=USTRINGTRIE_FINAL_ਮੁੱਲ;
            } ਨਹੀਂਤਾਂ {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                ਜੇ(ਗੰਢ<k_Min_Two_Unit_ਮੁੱਲ_Lead) {
                    delta=ਗੰਢ;
                } ਨਹੀਂਤਾਂ ਜੇ(ਗੰਢ<k_Three_Unit_ਮੁੱਲ_Lead) {
                    delta=((ਗੰਢ-k_Min_Two_Unit_ਮੁੱਲ_Lead)<<16)|*pos++;
                } ਨਹੀਂਤਾਂ {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                ਗੰਢ=*pos;
                ਨਤੀਜਾ= ਗੰਢ>=k_Min_ਮੁੱਲ_Lead ? ਮੁੱਲ_ਨਤੀਜਾ(ਗੰਢ) : USTRINGTRIE_NO_ਮੁੱਲ;
            }
            pos_=pos;
            ਵਾਪਸ ਨਤੀਜਾ;
        }
        --length;
        pos=skip_ਮੁੱਲ(pos);
    } ਜਦੋਂ(length>1);
    ਜੇ(uchar==*pos++) {
        pos_=pos;
        int32_t ਗੰਢ=*pos;
        ਵਾਪਸ ਗੰਢ>=k_Min_ਮੁੱਲ_Lead ? ਮੁੱਲ_ਨਤੀਜਾ(ਗੰਢ) : USTRINGTRIE_NO_ਮੁੱਲ;
    } ਨਹੀਂਤਾਂ {
        stop();
        ਵਾਪਸ USTRINGTRIE_NO_ਮੇਲ;
    }
}

UString_Trie_ਨਤੀਜਾ
UCharsTrie::ਅਗਲਾ_Impl(ਅਚਲ char16_t *pos, int32_t uchar) {
    int32_t ਗੰਢ=*pos++;
    ਲਈ(;;) {
        ਜੇ(ਗੰਢ<k_Min_ਰੇਖਿਕ_ਮੇਲ) {
            ਵਾਪਸ branch_ਅਗਲਾ(pos, ਗੰਢ, uchar);
        } ਨਹੀਂਤਾਂ ਜੇ(ਗੰਢ<k_Min_ਮੁੱਲ_Lead) {
            // Match the first of length+1 units.
            int32_t length=ਗੰਢ-k_Min_ਰੇਖਿਕ_ਮੇਲ;  // Actual match length minus 1.
            ਜੇ(uchar==*pos++) {
                ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ=--length;
                pos_=pos;
                ਵਾਪਸ (length<0 && (ਗੰਢ=*pos)>=k_Min_ਮੁੱਲ_Lead) ?
                        ਮੁੱਲ_ਨਤੀਜਾ(ਗੰਢ) : USTRINGTRIE_NO_ਮੁੱਲ;
            } ਨਹੀਂਤਾਂ {
                // No match.
                ਰੋਕੋ;
            }
        } ਨਹੀਂਤਾਂ ਜੇ(ਗੰਢ&k_ਮੁੱਲ_Is_Final) {
            // No further matching units.
            ਰੋਕੋ;
        } ਨਹੀਂਤਾਂ {
            // Skip intermediate value.
            pos=skip_ਗੰਢ_ਮੁੱਲ(pos, ਗੰਢ);
            ਗੰਢ&=k_ਗੰਢ_Type_ਨਕਾਬ;
        }
    }
    stop();
    ਵਾਪਸ USTRINGTRIE_NO_ਮੇਲ;
}

UString_Trie_ਨਤੀਜਾ
UCharsTrie::next(int32_t uchar) {
    ਅਚਲ char16_t *pos=pos_;
    ਜੇ(pos==ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ USTRINGTRIE_NO_ਮੇਲ;
    }
    int32_t length=ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ;  // Actual remaining match length minus 1.
    ਜੇ(length>=0) {
        // Remaining part of a linear-match node.
        ਜੇ(uchar==*pos++) {
            ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ=--length;
            pos_=pos;
            int32_t ਗੰਢ;
            ਵਾਪਸ (length<0 && (ਗੰਢ=*pos)>=k_Min_ਮੁੱਲ_Lead) ?
                    ਮੁੱਲ_ਨਤੀਜਾ(ਗੰਢ) : USTRINGTRIE_NO_ਮੁੱਲ;
        } ਨਹੀਂਤਾਂ {
            stop();
            ਵਾਪਸ USTRINGTRIE_NO_ਮੇਲ;
        }
    }
    ਵਾਪਸ ਅਗਲਾ_Impl(pos, uchar);
}

UString_Trie_ਨਤੀਜਾ
UCharsTrie::next(ConstChar16Ptr ptr, int32_t s_ਲੰਮਾਈ) {
    ਅਚਲ char16_t *s=ptr;
    ਜੇ(s_ਲੰਮਾਈ<0 ? *s==0 : s_ਲੰਮਾਈ==0) {
        // Empty input.
        ਵਾਪਸ current();
    }
    ਅਚਲ char16_t *pos=pos_;
    ਜੇ(pos==ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ USTRINGTRIE_NO_ਮੇਲ;
    }
    int32_t length=ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ;  // Actual remaining match length minus 1.
    ਲਈ(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        ਜੇ(s_ਲੰਮਾਈ<0) {
            ਲਈ(;;) {
                ਜੇ((uchar=*s++)==0) {
                    ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ=length;
                    pos_=pos;
                    int32_t ਗੰਢ;
                    ਵਾਪਸ (length<0 && (ਗੰਢ=*pos)>=k_Min_ਮੁੱਲ_Lead) ?
                            ਮੁੱਲ_ਨਤੀਜਾ(ਗੰਢ) : USTRINGTRIE_NO_ਮੁੱਲ;
                }
                ਜੇ(length<0) {
                    ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ=length;
                    ਰੋਕੋ;
                }
                ਜੇ(uchar!=*pos) {
                    stop();
                    ਵਾਪਸ USTRINGTRIE_NO_ਮੇਲ;
                }
                ++pos;
                --length;
            }
        } ਨਹੀਂਤਾਂ {
            ਲਈ(;;) {
                ਜੇ(s_ਲੰਮਾਈ==0) {
                    ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ=length;
                    pos_=pos;
                    int32_t ਗੰਢ;
                    ਵਾਪਸ (length<0 && (ਗੰਢ=*pos)>=k_Min_ਮੁੱਲ_Lead) ?
                            ਮੁੱਲ_ਨਤੀਜਾ(ਗੰਢ) : USTRINGTRIE_NO_ਮੁੱਲ;
                }
                uchar=*s++;
                --s_ਲੰਮਾਈ;
                ਜੇ(length<0) {
                    ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ=length;
                    ਰੋਕੋ;
                }
                ਜੇ(uchar!=*pos) {
                    stop();
                    ਵਾਪਸ USTRINGTRIE_NO_ਮੇਲ;
                }
                ++pos;
                --length;
            }
        }
        int32_t ਗੰਢ=*pos++;
        ਲਈ(;;) {
            ਜੇ(ਗੰਢ<k_Min_ਰੇਖਿਕ_ਮੇਲ) {
                UString_Trie_ਨਤੀਜਾ ਨਤੀਜਾ=branch_ਅਗਲਾ(pos, ਗੰਢ, uchar);
                ਜੇ(ਨਤੀਜਾ==USTRINGTRIE_NO_ਮੇਲ) {
                    ਵਾਪਸ USTRINGTRIE_NO_ਮੇਲ;
                }
                // Fetch the next input unit, if there is one.
                ਜੇ(s_ਲੰਮਾਈ<0) {
                    ਜੇ((uchar=*s++)==0) {
                        ਵਾਪਸ ਨਤੀਜਾ;
                    }
                } ਨਹੀਂਤਾਂ {
                    ਜੇ(s_ਲੰਮਾਈ==0) {
                        ਵਾਪਸ ਨਤੀਜਾ;
                    }
                    uchar=*s++;
                    --s_ਲੰਮਾਈ;
                }
                ਜੇ(ਨਤੀਜਾ==USTRINGTRIE_FINAL_ਮੁੱਲ) {
                    // No further matching units.
                    stop();
                    ਵਾਪਸ USTRINGTRIE_NO_ਮੇਲ;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                ਗੰਢ=*pos++;
            } ਨਹੀਂਤਾਂ ਜੇ(ਗੰਢ<k_Min_ਮੁੱਲ_Lead) {
                // Match length+1 units.
                length=ਗੰਢ-k_Min_ਰੇਖਿਕ_ਮੇਲ;  // Actual match length minus 1.
                ਜੇ(uchar!=*pos) {
                    stop();
                    ਵਾਪਸ USTRINGTRIE_NO_ਮੇਲ;
                }
                ++pos;
                --length;
                ਰੋਕੋ;
            } ਨਹੀਂਤਾਂ ਜੇ(ਗੰਢ&k_ਮੁੱਲ_Is_Final) {
                // No further matching units.
                stop();
                ਵਾਪਸ USTRINGTRIE_NO_ਮੇਲ;
            } ਨਹੀਂਤਾਂ {
                // Skip intermediate value.
                pos=skip_ਗੰਢ_ਮੁੱਲ(pos, ਗੰਢ);
                ਗੰਢ&=k_ਗੰਢ_Type_ਨਕਾਬ;
            }
        }
    }
}

ਅਚਲ char16_t*
UCharsTrie::ਲੱਭੋ_Unique_ਮੁੱਲ_From_Branch(ਅਚਲ char16_t* pos U_LIFETIME_BOUND,
                                      int32_t length,
                                      UBool have_Unique_ਮੁੱਲ,
                                      int32_t& unique_ਮੁੱਲ) {
    ਜਦੋਂ(length>k_Max_Branch_ਰੇਖਿਕ_Sub_ਗੰਢ_ਲੰਮਾਈ) {
        ++pos;  // ignore the comparison unit
        ਜੇ(ਖਾਲੀ_ਸੂਚਕ==ਲੱਭੋ_Unique_ਮੁੱਲ_From_Branch(jumpByDelta(pos), length>>1, have_Unique_ਮੁੱਲ, unique_ਮੁੱਲ)) {
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    ਕਰੋ {
        ++pos;  // ignore a comparison unit
        // handle its value
        int32_t ਗੰਢ=*pos++;
        UBool isFinal = static_cast<UBool>(ਗੰਢ >> 15);
        ਗੰਢ&=0x7fff;
        int32_t value=read_ਮੁੱਲ(pos, ਗੰਢ);
        pos=skip_ਮੁੱਲ(pos, ਗੰਢ);
        ਜੇ(isFinal) {
            ਜੇ(have_Unique_ਮੁੱਲ) {
                ਜੇ(value!=unique_ਮੁੱਲ) {
                    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
                }
            } ਨਹੀਂਤਾਂ {
                unique_ਮੁੱਲ=value;
                have_Unique_ਮੁੱਲ=ਸੱਚ;
            }
        } ਨਹੀਂਤਾਂ {
            ਜੇ(!ਲੱਭੋ_Unique_ਮੁੱਲ(pos+value, have_Unique_ਮੁੱਲ, unique_ਮੁੱਲ)) {
                ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
            }
            have_Unique_ਮੁੱਲ=ਸੱਚ;
        }
    } ਜਦੋਂ(--length>1);
    ਵਾਪਸ pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::ਲੱਭੋ_Unique_ਮੁੱਲ(ਅਚਲ char16_t *pos, UBool have_Unique_ਮੁੱਲ, int32_t &unique_ਮੁੱਲ) {
    int32_t ਗੰਢ=*pos++;
    ਲਈ(;;) {
        ਜੇ(ਗੰਢ<k_Min_ਰੇਖਿਕ_ਮੇਲ) {
            ਜੇ(ਗੰਢ==0) {
                ਗੰਢ=*pos++;
            }
            pos=ਲੱਭੋ_Unique_ਮੁੱਲ_From_Branch(pos, ਗੰਢ+1, have_Unique_ਮੁੱਲ, unique_ਮੁੱਲ);
            ਜੇ(pos==ਖਾਲੀ_ਸੂਚਕ) {
                ਵਾਪਸ ਝੂਠ;
            }
            have_Unique_ਮੁੱਲ=ਸੱਚ;
            ਗੰਢ=*pos++;
        } ਨਹੀਂਤਾਂ ਜੇ(ਗੰਢ<k_Min_ਮੁੱਲ_Lead) {
            // linear-match node
            pos+=ਗੰਢ-k_Min_ਰੇਖਿਕ_ਮੇਲ+1;  // Ignore the match units.
            ਗੰਢ=*pos++;
        } ਨਹੀਂਤਾਂ {
            UBool isFinal = static_cast<UBool>(ਗੰਢ >> 15);
            int32_t value;
            ਜੇ(isFinal) {
                value=read_ਮੁੱਲ(pos, ਗੰਢ&0x7fff);
            } ਨਹੀਂਤਾਂ {
                value=read_ਗੰਢ_ਮੁੱਲ(pos, ਗੰਢ);
            }
            ਜੇ(have_Unique_ਮੁੱਲ) {
                ਜੇ(value!=unique_ਮੁੱਲ) {
                    ਵਾਪਸ ਝੂਠ;
                }
            } ਨਹੀਂਤਾਂ {
                unique_ਮੁੱਲ=value;
                have_Unique_ਮੁੱਲ=ਸੱਚ;
            }
            ਜੇ(isFinal) {
                ਵਾਪਸ ਸੱਚ;
            }
            pos=skip_ਗੰਢ_ਮੁੱਲ(pos, ਗੰਢ);
            ਗੰਢ&=k_ਗੰਢ_Type_ਨਕਾਬ;
        }
    }
}

int32_t
UCharsTrie::get_ਅਗਲਾ_UChars(Appendable &out) ਅਚਲ {
    ਅਚਲ char16_t *pos=pos_;
    ਜੇ(pos==ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ 0;
    }
    ਜੇ(ਬਚਿਆ_ਮੇਲ_ਲੰਮਾਈ>=0) {
        out.append_ਕੋਡ_Unit(*pos);  // Next unit of a pending linear-match node.
        ਵਾਪਸ 1;
    }
    int32_t ਗੰਢ=*pos++;
    ਜੇ(ਗੰਢ>=k_Min_ਮੁੱਲ_Lead) {
        ਜੇ(ਗੰਢ&k_ਮੁੱਲ_Is_Final) {
            ਵਾਪਸ 0;
        } ਨਹੀਂਤਾਂ {
            pos=skip_ਗੰਢ_ਮੁੱਲ(pos, ਗੰਢ);
            ਗੰਢ&=k_ਗੰਢ_Type_ਨਕਾਬ;
        }
    }
    ਜੇ(ਗੰਢ<k_Min_ਰੇਖਿਕ_ਮੇਲ) {
        ਜੇ(ਗੰਢ==0) {
            ਗੰਢ=*pos++;
        }
        out.reserve_Append_ਸਮਰੱਥਾ(++ਗੰਢ);
        get_ਅਗਲਾ_Branch_UChars(pos, ਗੰਢ, out);
        ਵਾਪਸ ਗੰਢ;
    } ਨਹੀਂਤਾਂ {
        // First unit of the linear-match node.
        out.append_ਕੋਡ_Unit(*pos);
        ਵਾਪਸ 1;
    }
}

ਖਾਲੀ
UCharsTrie::get_ਅਗਲਾ_Branch_UChars(ਅਚਲ char16_t *pos, int32_t length, Appendable &out) {
    ਜਦੋਂ(length>k_Max_Branch_ਰੇਖਿਕ_Sub_ਗੰਢ_ਲੰਮਾਈ) {
        ++pos;  // ignore the comparison unit
        get_ਅਗਲਾ_Branch_UChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    ਕਰੋ {
        out.append_ਕੋਡ_Unit(*pos++);
        pos=skip_ਮੁੱਲ(pos);
    } ਜਦੋਂ(--length>1);
    out.append_ਕੋਡ_Unit(*pos);
}

U_NAMESPACE_ਸਿਰਾ
