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

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/appendable.h"
#અન્તર્ભાવ "unicode/ucharstrie.h"
#અન્તર્ભાવ "unicode/uobject.h"
#અન્તર્ભાવ "unicode/utf16.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "uassert.h"

U_NAMESPACE_BEGIN

UCharsTrie::~UCharsTrie() {
    uprv_free(ownedArray_);
}

UStringTrieResult
UCharsTrie::current() અચલ {
    અચલ char16_t *pos=pos_;
    જો(pos==nullptr) {
        પાછા USTRINGTRIE_NO_MATCH;
    } નહીંતર {
        int32_t ગાંઠ;
        પાછા (remainingMatchLength_<0 && (ગાંઠ=*pos)>=k_Min_કિંમત_Lead) ?
                કિંમત_Result(ગાંઠ) : USTRINGTRIE_NO_કિંમત;
    }
}

UStringTrieResult
UCharsTrie::firstForCodePoint(UChar32 cp) {
    પાછા cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_NEXT(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::nextForCodePoint(UChar32 cp) {
    પાછા cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_NEXT(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::branchNext(અચલ char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    જો(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    જ્યારે(length>k_Max_Branch_Linear_Sub_ગાંઠ_Length) {
        જો(uchar<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } નહીંતર {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few units.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    કરો {
        જો(uchar==*pos++) {
            UStringTrieResult result;
            int32_t ગાંઠ=*pos;
            જો(ગાંઠ&k_કિંમત_Is_Final) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_કિંમત;
            } નહીંતર {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                જો(ગાંઠ<k_Min_Two_Unit_કિંમત_Lead) {
                    delta=ગાંઠ;
                } નહીંતર જો(ગાંઠ<k_Three_Unit_કિંમત_Lead) {
                    delta=((ગાંઠ-k_Min_Two_Unit_કિંમત_Lead)<<16)|*pos++;
                } નહીંતર {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                ગાંઠ=*pos;
                result= ગાંઠ>=k_Min_કિંમત_Lead ? કિંમત_Result(ગાંઠ) : USTRINGTRIE_NO_કિંમત;
            }
            pos_=pos;
            પાછા result;
        }
        --length;
        pos=skip_કિંમત(pos);
    } જ્યારે(length>1);
    જો(uchar==*pos++) {
        pos_=pos;
        int32_t ગાંઠ=*pos;
        પાછા ગાંઠ>=k_Min_કિંમત_Lead ? કિંમત_Result(ગાંઠ) : USTRINGTRIE_NO_કિંમત;
    } નહીંતર {
        stop();
        પાછા USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
UCharsTrie::nextImpl(અચલ char16_t *pos, int32_t uchar) {
    int32_t ગાંઠ=*pos++;
    માટે(;;) {
        જો(ગાંઠ<kMinLinearMatch) {
            પાછા branchNext(pos, ગાંઠ, uchar);
        } નહીંતર જો(ગાંઠ<k_Min_કિંમત_Lead) {
            // Match the first of length+1 units.
            int32_t length=ગાંઠ-kMinLinearMatch;  // Actual match length minus 1.
            જો(uchar==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                પાછા (length<0 && (ગાંઠ=*pos)>=k_Min_કિંમત_Lead) ?
                        કિંમત_Result(ગાંઠ) : USTRINGTRIE_NO_કિંમત;
            } નહીંતર {
                // No match.
                તોડો;
            }
        } નહીંતર જો(ગાંઠ&k_કિંમત_Is_Final) {
            // No further matching units.
            તોડો;
        } નહીંતર {
            // Skip intermediate value.
            pos=skip_ગાંઠ_કિંમત(pos, ગાંઠ);
            ગાંઠ&=k_ગાંઠ_Type_Mask;
        }
    }
    stop();
    પાછા USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
UCharsTrie::next(int32_t uchar) {
    અચલ char16_t *pos=pos_;
    જો(pos==nullptr) {
        પાછા USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    જો(length>=0) {
        // Remaining part of a linear-match node.
        જો(uchar==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t ગાંઠ;
            પાછા (length<0 && (ગાંઠ=*pos)>=k_Min_કિંમત_Lead) ?
                    કિંમત_Result(ગાંઠ) : USTRINGTRIE_NO_કિંમત;
        } નહીંતર {
            stop();
            પાછા USTRINGTRIE_NO_MATCH;
        }
    }
    પાછા nextImpl(pos, uchar);
}

UStringTrieResult
UCharsTrie::next(ConstChar16Ptr ptr, int32_t sLength) {
    અચલ char16_t *s=ptr;
    જો(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        પાછા current();
    }
    અચલ char16_t *pos=pos_;
    જો(pos==nullptr) {
        પાછા USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    માટે(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        જો(sLength<0) {
            માટે(;;) {
                જો((uchar=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t ગાંઠ;
                    પાછા (length<0 && (ગાંઠ=*pos)>=k_Min_કિંમત_Lead) ?
                            કિંમત_Result(ગાંઠ) : USTRINGTRIE_NO_કિંમત;
                }
                જો(length<0) {
                    remainingMatchLength_=length;
                    તોડો;
                }
                જો(uchar!=*pos) {
                    stop();
                    પાછા USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } નહીંતર {
            માટે(;;) {
                જો(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t ગાંઠ;
                    પાછા (length<0 && (ગાંઠ=*pos)>=k_Min_કિંમત_Lead) ?
                            કિંમત_Result(ગાંઠ) : USTRINGTRIE_NO_કિંમત;
                }
                uchar=*s++;
                --sLength;
                જો(length<0) {
                    remainingMatchLength_=length;
                    તોડો;
                }
                જો(uchar!=*pos) {
                    stop();
                    પાછા USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        int32_t ગાંઠ=*pos++;
        માટે(;;) {
            જો(ગાંઠ<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, ગાંઠ, uchar);
                જો(result==USTRINGTRIE_NO_MATCH) {
                    પાછા USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input unit, if there is one.
                જો(sLength<0) {
                    જો((uchar=*s++)==0) {
                        પાછા result;
                    }
                } નહીંતર {
                    જો(sLength==0) {
                        પાછા result;
                    }
                    uchar=*s++;
                    --sLength;
                }
                જો(result==USTRINGTRIE_FINAL_કિંમત) {
                    // No further matching units.
                    stop();
                    પાછા USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                ગાંઠ=*pos++;
            } નહીંતર જો(ગાંઠ<k_Min_કિંમત_Lead) {
                // Match length+1 units.
                length=ગાંઠ-kMinLinearMatch;  // Actual match length minus 1.
                જો(uchar!=*pos) {
                    stop();
                    પાછા USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                તોડો;
            } નહીંતર જો(ગાંઠ&k_કિંમત_Is_Final) {
                // No further matching units.
                stop();
                પાછા USTRINGTRIE_NO_MATCH;
            } નહીંતર {
                // Skip intermediate value.
                pos=skip_ગાંઠ_કિંમત(pos, ગાંઠ);
                ગાંઠ&=k_ગાંઠ_Type_Mask;
            }
        }
    }
}

અચલ char16_t*
UCharsTrie::find_Unique_કિંમત_From_Branch(અચલ char16_t* pos U_LIFETIME_BOUND,
                                      int32_t length,
                                      UBool have_Unique_કિંમત,
                                      int32_t& unique_કિંમત) {
    જ્યારે(length>k_Max_Branch_Linear_Sub_ગાંઠ_Length) {
        ++pos;  // ignore the comparison unit
        જો(nullptr==find_Unique_કિંમત_From_Branch(jumpByDelta(pos), length>>1, have_Unique_કિંમત, unique_કિંમત)) {
            પાછા nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    કરો {
        ++pos;  // ignore a comparison unit
        // handle its value
        int32_t ગાંઠ=*pos++;
        UBool isFinal = static_cast<UBool>(ગાંઠ >> 15);
        ગાંઠ&=0x7fff;
        int32_t value=read_કિંમત(pos, ગાંઠ);
        pos=skip_કિંમત(pos, ગાંઠ);
        જો(isFinal) {
            જો(have_Unique_કિંમત) {
                જો(value!=unique_કિંમત) {
                    પાછા nullptr;
                }
            } નહીંતર {
                unique_કિંમત=value;
                have_Unique_કિંમત=સાચું;
            }
        } નહીંતર {
            જો(!find_Unique_કિંમત(pos+value, have_Unique_કિંમત, unique_કિંમત)) {
                પાછા nullptr;
            }
            have_Unique_કિંમત=સાચું;
        }
    } જ્યારે(--length>1);
    પાછા pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::find_Unique_કિંમત(અચલ char16_t *pos, UBool have_Unique_કિંમત, int32_t &unique_કિંમત) {
    int32_t ગાંઠ=*pos++;
    માટે(;;) {
        જો(ગાંઠ<kMinLinearMatch) {
            જો(ગાંઠ==0) {
                ગાંઠ=*pos++;
            }
            pos=find_Unique_કિંમત_From_Branch(pos, ગાંઠ+1, have_Unique_કિંમત, unique_કિંમત);
            જો(pos==nullptr) {
                પાછા ખોટું;
            }
            have_Unique_કિંમત=સાચું;
            ગાંઠ=*pos++;
        } નહીંતર જો(ગાંઠ<k_Min_કિંમત_Lead) {
            // linear-match node
            pos+=ગાંઠ-kMinLinearMatch+1;  // Ignore the match units.
            ગાંઠ=*pos++;
        } નહીંતર {
            UBool isFinal = static_cast<UBool>(ગાંઠ >> 15);
            int32_t value;
            જો(isFinal) {
                value=read_કિંમત(pos, ગાંઠ&0x7fff);
            } નહીંતર {
                value=read_ગાંઠ_કિંમત(pos, ગાંઠ);
            }
            જો(have_Unique_કિંમત) {
                જો(value!=unique_કિંમત) {
                    પાછા ખોટું;
                }
            } નહીંતર {
                unique_કિંમત=value;
                have_Unique_કિંમત=સાચું;
            }
            જો(isFinal) {
                પાછા સાચું;
            }
            pos=skip_ગાંઠ_કિંમત(pos, ગાંઠ);
            ગાંઠ&=k_ગાંઠ_Type_Mask;
        }
    }
}

int32_t
UCharsTrie::getNextUChars(Appendable &out) અચલ {
    અચલ char16_t *pos=pos_;
    જો(pos==nullptr) {
        પાછા 0;
    }
    જો(remainingMatchLength_>=0) {
        out.appendCodeUnit(*pos);  // Next unit of a pending linear-match node.
        પાછા 1;
    }
    int32_t ગાંઠ=*pos++;
    જો(ગાંઠ>=k_Min_કિંમત_Lead) {
        જો(ગાંઠ&k_કિંમત_Is_Final) {
            પાછા 0;
        } નહીંતર {
            pos=skip_ગાંઠ_કિંમત(pos, ગાંઠ);
            ગાંઠ&=k_ગાંઠ_Type_Mask;
        }
    }
    જો(ગાંઠ<kMinLinearMatch) {
        જો(ગાંઠ==0) {
            ગાંઠ=*pos++;
        }
        out.reserveAppendCapacity(++ગાંઠ);
        getNextBranchUChars(pos, ગાંઠ, out);
        પાછા ગાંઠ;
    } નહીંતર {
        // First unit of the linear-match node.
        out.appendCodeUnit(*pos);
        પાછા 1;
    }
}

શૂન્ય
UCharsTrie::getNextBranchUChars(અચલ char16_t *pos, int32_t length, Appendable &out) {
    જ્યારે(length>k_Max_Branch_Linear_Sub_ગાંઠ_Length) {
        ++pos;  // ignore the comparison unit
        getNextBranchUChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    કરો {
        out.appendCodeUnit(*pos++);
        pos=skip_કિંમત(pos);
    } જ્યારે(--length>1);
    out.appendCodeUnit(*pos);
}

U_NAMESPACE_END
