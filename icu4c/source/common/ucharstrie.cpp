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

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/appendable.h"
#অন্তর্ভাব "unicode/ucharstrie.h"
#অন্তর্ভাব "unicode/uobject.h"
#অন্তর্ভাব "unicode/utf16.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "uassert.h"

U_NAMESPACE_BEGIN

UCharsTrie::~UCharsTrie() {
    uprv_free(ownedArray_);
}

UStringTrieResult
UCharsTrie::current() ধ্রুব {
    ধ্রুব char16_t *pos=pos_;
    যদি(pos==nullptr) {
        প্রত্যাগম USTRINGTRIE_NO_MATCH;
    } অন্যথা {
        int32_t গ্রন্থি;
        প্রত্যাগম (remainingMatchLength_<0 && (গ্রন্থি=*pos)>=k_Min_মান_Lead) ?
                মান_Result(গ্রন্থি) : USTRINGTRIE_NO_মান;
    }
}

UStringTrieResult
UCharsTrie::firstForCodePoint(UChar32 cp) {
    প্রত্যাগম cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_NEXT(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::nextForCodePoint(UChar32 cp) {
    প্রত্যাগম cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_NEXT(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::branchNext(ধ্রুব char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    যদি(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    যাবত্(length>k_Max_Branch_Linear_Sub_গ্রন্থি_Length) {
        যদি(uchar<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } অন্যথা {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few units.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    কুরু {
        যদি(uchar==*pos++) {
            UStringTrieResult result;
            int32_t গ্রন্থি=*pos;
            যদি(গ্রন্থি&k_মান_Is_Final) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_মান;
            } অন্যথা {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                যদি(গ্রন্থি<k_Min_Two_Unit_মান_Lead) {
                    delta=গ্রন্থি;
                } অন্যথা যদি(গ্রন্থি<k_Three_Unit_মান_Lead) {
                    delta=((গ্রন্থি-k_Min_Two_Unit_মান_Lead)<<16)|*pos++;
                } অন্যথা {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                গ্রন্থি=*pos;
                result= গ্রন্থি>=k_Min_মান_Lead ? মান_Result(গ্রন্থি) : USTRINGTRIE_NO_মান;
            }
            pos_=pos;
            প্রত্যাগম result;
        }
        --length;
        pos=skip_মান(pos);
    } যাবত্(length>1);
    যদি(uchar==*pos++) {
        pos_=pos;
        int32_t গ্রন্থি=*pos;
        প্রত্যাগম গ্রন্থি>=k_Min_মান_Lead ? মান_Result(গ্রন্থি) : USTRINGTRIE_NO_মান;
    } অন্যথা {
        stop();
        প্রত্যাগম USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
UCharsTrie::nextImpl(ধ্রুব char16_t *pos, int32_t uchar) {
    int32_t গ্রন্থি=*pos++;
    প্রতি(;;) {
        যদি(গ্রন্থি<kMinLinearMatch) {
            প্রত্যাগম branchNext(pos, গ্রন্থি, uchar);
        } অন্যথা যদি(গ্রন্থি<k_Min_মান_Lead) {
            // Match the first of length+1 units.
            int32_t length=গ্রন্থি-kMinLinearMatch;  // Actual match length minus 1.
            যদি(uchar==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                প্রত্যাগম (length<0 && (গ্রন্থি=*pos)>=k_Min_মান_Lead) ?
                        মান_Result(গ্রন্থি) : USTRINGTRIE_NO_মান;
            } অন্যথা {
                // No match.
                ভঙ্গ;
            }
        } অন্যথা যদি(গ্রন্থি&k_মান_Is_Final) {
            // No further matching units.
            ভঙ্গ;
        } অন্যথা {
            // Skip intermediate value.
            pos=skip_গ্রন্থি_মান(pos, গ্রন্থি);
            গ্রন্থি&=k_গ্রন্থি_Type_Mask;
        }
    }
    stop();
    প্রত্যাগম USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
UCharsTrie::next(int32_t uchar) {
    ধ্রুব char16_t *pos=pos_;
    যদি(pos==nullptr) {
        প্রত্যাগম USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    যদি(length>=0) {
        // Remaining part of a linear-match node.
        যদি(uchar==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t গ্রন্থি;
            প্রত্যাগম (length<0 && (গ্রন্থি=*pos)>=k_Min_মান_Lead) ?
                    মান_Result(গ্রন্থি) : USTRINGTRIE_NO_মান;
        } অন্যথা {
            stop();
            প্রত্যাগম USTRINGTRIE_NO_MATCH;
        }
    }
    প্রত্যাগম nextImpl(pos, uchar);
}

UStringTrieResult
UCharsTrie::next(ConstChar16Ptr ptr, int32_t sLength) {
    ধ্রুব char16_t *s=ptr;
    যদি(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        প্রত্যাগম current();
    }
    ধ্রুব char16_t *pos=pos_;
    যদি(pos==nullptr) {
        প্রত্যাগম USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    প্রতি(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        যদি(sLength<0) {
            প্রতি(;;) {
                যদি((uchar=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t গ্রন্থি;
                    প্রত্যাগম (length<0 && (গ্রন্থি=*pos)>=k_Min_মান_Lead) ?
                            মান_Result(গ্রন্থি) : USTRINGTRIE_NO_মান;
                }
                যদি(length<0) {
                    remainingMatchLength_=length;
                    ভঙ্গ;
                }
                যদি(uchar!=*pos) {
                    stop();
                    প্রত্যাগম USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } অন্যথা {
            প্রতি(;;) {
                যদি(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t গ্রন্থি;
                    প্রত্যাগম (length<0 && (গ্রন্থি=*pos)>=k_Min_মান_Lead) ?
                            মান_Result(গ্রন্থি) : USTRINGTRIE_NO_মান;
                }
                uchar=*s++;
                --sLength;
                যদি(length<0) {
                    remainingMatchLength_=length;
                    ভঙ্গ;
                }
                যদি(uchar!=*pos) {
                    stop();
                    প্রত্যাগম USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        int32_t গ্রন্থি=*pos++;
        প্রতি(;;) {
            যদি(গ্রন্থি<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, গ্রন্থি, uchar);
                যদি(result==USTRINGTRIE_NO_MATCH) {
                    প্রত্যাগম USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input unit, if there is one.
                যদি(sLength<0) {
                    যদি((uchar=*s++)==0) {
                        প্রত্যাগম result;
                    }
                } অন্যথা {
                    যদি(sLength==0) {
                        প্রত্যাগম result;
                    }
                    uchar=*s++;
                    --sLength;
                }
                যদি(result==USTRINGTRIE_FINAL_মান) {
                    // No further matching units.
                    stop();
                    প্রত্যাগম USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                গ্রন্থি=*pos++;
            } অন্যথা যদি(গ্রন্থি<k_Min_মান_Lead) {
                // Match length+1 units.
                length=গ্রন্থি-kMinLinearMatch;  // Actual match length minus 1.
                যদি(uchar!=*pos) {
                    stop();
                    প্রত্যাগম USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                ভঙ্গ;
            } অন্যথা যদি(গ্রন্থি&k_মান_Is_Final) {
                // No further matching units.
                stop();
                প্রত্যাগম USTRINGTRIE_NO_MATCH;
            } অন্যথা {
                // Skip intermediate value.
                pos=skip_গ্রন্থি_মান(pos, গ্রন্থি);
                গ্রন্থি&=k_গ্রন্থি_Type_Mask;
            }
        }
    }
}

ধ্রুব char16_t*
UCharsTrie::find_Unique_মান_From_Branch(ধ্রুব char16_t* pos U_LIFETIME_BOUND,
                                      int32_t length,
                                      UBool have_Unique_মান,
                                      int32_t& unique_মান) {
    যাবত্(length>k_Max_Branch_Linear_Sub_গ্রন্থি_Length) {
        ++pos;  // ignore the comparison unit
        যদি(nullptr==find_Unique_মান_From_Branch(jumpByDelta(pos), length>>1, have_Unique_মান, unique_মান)) {
            প্রত্যাগম nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    কুরু {
        ++pos;  // ignore a comparison unit
        // handle its value
        int32_t গ্রন্থি=*pos++;
        UBool isFinal = static_cast<UBool>(গ্রন্থি >> 15);
        গ্রন্থি&=0x7fff;
        int32_t value=read_মান(pos, গ্রন্থি);
        pos=skip_মান(pos, গ্রন্থি);
        যদি(isFinal) {
            যদি(have_Unique_মান) {
                যদি(value!=unique_মান) {
                    প্রত্যাগম nullptr;
                }
            } অন্যথা {
                unique_মান=value;
                have_Unique_মান=সত্য;
            }
        } অন্যথা {
            যদি(!find_Unique_মান(pos+value, have_Unique_মান, unique_মান)) {
                প্রত্যাগম nullptr;
            }
            have_Unique_মান=সত্য;
        }
    } যাবত্(--length>1);
    প্রত্যাগম pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::find_Unique_মান(ধ্রুব char16_t *pos, UBool have_Unique_মান, int32_t &unique_মান) {
    int32_t গ্রন্থি=*pos++;
    প্রতি(;;) {
        যদি(গ্রন্থি<kMinLinearMatch) {
            যদি(গ্রন্থি==0) {
                গ্রন্থি=*pos++;
            }
            pos=find_Unique_মান_From_Branch(pos, গ্রন্থি+1, have_Unique_মান, unique_মান);
            যদি(pos==nullptr) {
                প্রত্যাগম অসত্য;
            }
            have_Unique_মান=সত্য;
            গ্রন্থি=*pos++;
        } অন্যথা যদি(গ্রন্থি<k_Min_মান_Lead) {
            // linear-match node
            pos+=গ্রন্থি-kMinLinearMatch+1;  // Ignore the match units.
            গ্রন্থি=*pos++;
        } অন্যথা {
            UBool isFinal = static_cast<UBool>(গ্রন্থি >> 15);
            int32_t value;
            যদি(isFinal) {
                value=read_মান(pos, গ্রন্থি&0x7fff);
            } অন্যথা {
                value=read_গ্রন্থি_মান(pos, গ্রন্থি);
            }
            যদি(have_Unique_মান) {
                যদি(value!=unique_মান) {
                    প্রত্যাগম অসত্য;
                }
            } অন্যথা {
                unique_মান=value;
                have_Unique_মান=সত্য;
            }
            যদি(isFinal) {
                প্রত্যাগম সত্য;
            }
            pos=skip_গ্রন্থি_মান(pos, গ্রন্থি);
            গ্রন্থি&=k_গ্রন্থি_Type_Mask;
        }
    }
}

int32_t
UCharsTrie::getNextUChars(Appendable &out) ধ্রুব {
    ধ্রুব char16_t *pos=pos_;
    যদি(pos==nullptr) {
        প্রত্যাগম 0;
    }
    যদি(remainingMatchLength_>=0) {
        out.appendCodeUnit(*pos);  // Next unit of a pending linear-match node.
        প্রত্যাগম 1;
    }
    int32_t গ্রন্থি=*pos++;
    যদি(গ্রন্থি>=k_Min_মান_Lead) {
        যদি(গ্রন্থি&k_মান_Is_Final) {
            প্রত্যাগম 0;
        } অন্যথা {
            pos=skip_গ্রন্থি_মান(pos, গ্রন্থি);
            গ্রন্থি&=k_গ্রন্থি_Type_Mask;
        }
    }
    যদি(গ্রন্থি<kMinLinearMatch) {
        যদি(গ্রন্থি==0) {
            গ্রন্থি=*pos++;
        }
        out.reserveAppendCapacity(++গ্রন্থি);
        getNextBranchUChars(pos, গ্রন্থি, out);
        প্রত্যাগম গ্রন্থি;
    } অন্যথা {
        // First unit of the linear-match node.
        out.appendCodeUnit(*pos);
        প্রত্যাগম 1;
    }
}

শূন্য
UCharsTrie::getNextBranchUChars(ধ্রুব char16_t *pos, int32_t length, Appendable &out) {
    যাবত্(length>k_Max_Branch_Linear_Sub_গ্রন্থি_Length) {
        ++pos;  // ignore the comparison unit
        getNextBranchUChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    কুরু {
        out.appendCodeUnit(*pos++);
        pos=skip_মান(pos);
    } যাবত্(--length>1);
    out.appendCodeUnit(*pos);
}

U_NAMESPACE_END
