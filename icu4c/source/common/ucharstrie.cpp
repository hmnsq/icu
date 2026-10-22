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

UString_Trie_ফলাফল
UCharsTrie::current() ধ্রুবক {
    ধ্রুবক char16_t *pos=pos_;
    যদি(pos==nullptr) {
        ফেরত USTRINGTRIE_NO_MATCH;
    } অন্যথা {
        int32_t গ্রন্থি;
        ফেরত (remaining_Match_দৈর্ঘ্য<0 && (গ্রন্থি=*pos)>=k_Min_মান_Lead) ?
                মান_ফলাফল(গ্রন্থি) : USTRINGTRIE_NO_মান;
    }
}

UString_Trie_ফলাফল
UCharsTrie::firstForCodePoint(UChar32 cp) {
    ফেরত cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_পরবর্তী(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UString_Trie_ফলাফল
UCharsTrie::পরবর্তী_For_Code_Point(UChar32 cp) {
    ফেরত cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_পরবর্তী(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UString_Trie_ফলাফল
UCharsTrie::branch_পরবর্তী(ধ্রুবক char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    যদি(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    যখন(length>k_Max_Branch_Linear_Sub_গ্রন্থি_দৈর্ঘ্য) {
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
    করো {
        যদি(uchar==*pos++) {
            UString_Trie_ফলাফল ফলাফল;
            int32_t গ্রন্থি=*pos;
            যদি(গ্রন্থি&k_মান_Is_Final) {
                // Leave the final value for getValue() to read.
                ফলাফল=USTRINGTRIE_FINAL_মান;
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
                ফলাফল= গ্রন্থি>=k_Min_মান_Lead ? মান_ফলাফল(গ্রন্থি) : USTRINGTRIE_NO_মান;
            }
            pos_=pos;
            ফেরত ফলাফল;
        }
        --length;
        pos=skip_মান(pos);
    } যখন(length>1);
    যদি(uchar==*pos++) {
        pos_=pos;
        int32_t গ্রন্থি=*pos;
        ফেরত গ্রন্থি>=k_Min_মান_Lead ? মান_ফলাফল(গ্রন্থি) : USTRINGTRIE_NO_মান;
    } অন্যথা {
        stop();
        ফেরত USTRINGTRIE_NO_MATCH;
    }
}

UString_Trie_ফলাফল
UCharsTrie::পরবর্তী_Impl(ধ্রুবক char16_t *pos, int32_t uchar) {
    int32_t গ্রন্থি=*pos++;
    জন্য(;;) {
        যদি(গ্রন্থি<kMinLinearMatch) {
            ফেরত branch_পরবর্তী(pos, গ্রন্থি, uchar);
        } অন্যথা যদি(গ্রন্থি<k_Min_মান_Lead) {
            // Match the first of length+1 units.
            int32_t length=গ্রন্থি-kMinLinearMatch;  // Actual match length minus 1.
            যদি(uchar==*pos++) {
                remaining_Match_দৈর্ঘ্য=--length;
                pos_=pos;
                ফেরত (length<0 && (গ্রন্থি=*pos)>=k_Min_মান_Lead) ?
                        মান_ফলাফল(গ্রন্থি) : USTRINGTRIE_NO_মান;
            } অন্যথা {
                // No match.
                বিরতি;
            }
        } অন্যথা যদি(গ্রন্থি&k_মান_Is_Final) {
            // No further matching units.
            বিরতি;
        } অন্যথা {
            // Skip intermediate value.
            pos=skip_গ্রন্থি_মান(pos, গ্রন্থি);
            গ্রন্থি&=k_গ্রন্থি_Type_Mask;
        }
    }
    stop();
    ফেরত USTRINGTRIE_NO_MATCH;
}

UString_Trie_ফলাফল
UCharsTrie::next(int32_t uchar) {
    ধ্রুবক char16_t *pos=pos_;
    যদি(pos==nullptr) {
        ফেরত USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remaining_Match_দৈর্ঘ্য;  // Actual remaining match length minus 1.
    যদি(length>=0) {
        // Remaining part of a linear-match node.
        যদি(uchar==*pos++) {
            remaining_Match_দৈর্ঘ্য=--length;
            pos_=pos;
            int32_t গ্রন্থি;
            ফেরত (length<0 && (গ্রন্থি=*pos)>=k_Min_মান_Lead) ?
                    মান_ফলাফল(গ্রন্থি) : USTRINGTRIE_NO_মান;
        } অন্যথা {
            stop();
            ফেরত USTRINGTRIE_NO_MATCH;
        }
    }
    ফেরত পরবর্তী_Impl(pos, uchar);
}

UString_Trie_ফলাফল
UCharsTrie::next(ConstChar16Ptr ptr, int32_t s_দৈর্ঘ্য) {
    ধ্রুবক char16_t *s=ptr;
    যদি(s_দৈর্ঘ্য<0 ? *s==0 : s_দৈর্ঘ্য==0) {
        // Empty input.
        ফেরত current();
    }
    ধ্রুবক char16_t *pos=pos_;
    যদি(pos==nullptr) {
        ফেরত USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remaining_Match_দৈর্ঘ্য;  // Actual remaining match length minus 1.
    জন্য(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        যদি(s_দৈর্ঘ্য<0) {
            জন্য(;;) {
                যদি((uchar=*s++)==0) {
                    remaining_Match_দৈর্ঘ্য=length;
                    pos_=pos;
                    int32_t গ্রন্থি;
                    ফেরত (length<0 && (গ্রন্থি=*pos)>=k_Min_মান_Lead) ?
                            মান_ফলাফল(গ্রন্থি) : USTRINGTRIE_NO_মান;
                }
                যদি(length<0) {
                    remaining_Match_দৈর্ঘ্য=length;
                    বিরতি;
                }
                যদি(uchar!=*pos) {
                    stop();
                    ফেরত USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } অন্যথা {
            জন্য(;;) {
                যদি(s_দৈর্ঘ্য==0) {
                    remaining_Match_দৈর্ঘ্য=length;
                    pos_=pos;
                    int32_t গ্রন্থি;
                    ফেরত (length<0 && (গ্রন্থি=*pos)>=k_Min_মান_Lead) ?
                            মান_ফলাফল(গ্রন্থি) : USTRINGTRIE_NO_মান;
                }
                uchar=*s++;
                --s_দৈর্ঘ্য;
                যদি(length<0) {
                    remaining_Match_দৈর্ঘ্য=length;
                    বিরতি;
                }
                যদি(uchar!=*pos) {
                    stop();
                    ফেরত USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        int32_t গ্রন্থি=*pos++;
        জন্য(;;) {
            যদি(গ্রন্থি<kMinLinearMatch) {
                UString_Trie_ফলাফল ফলাফল=branch_পরবর্তী(pos, গ্রন্থি, uchar);
                যদি(ফলাফল==USTRINGTRIE_NO_MATCH) {
                    ফেরত USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input unit, if there is one.
                যদি(s_দৈর্ঘ্য<0) {
                    যদি((uchar=*s++)==0) {
                        ফেরত ফলাফল;
                    }
                } অন্যথা {
                    যদি(s_দৈর্ঘ্য==0) {
                        ফেরত ফলাফল;
                    }
                    uchar=*s++;
                    --s_দৈর্ঘ্য;
                }
                যদি(ফলাফল==USTRINGTRIE_FINAL_মান) {
                    // No further matching units.
                    stop();
                    ফেরত USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                গ্রন্থি=*pos++;
            } অন্যথা যদি(গ্রন্থি<k_Min_মান_Lead) {
                // Match length+1 units.
                length=গ্রন্থি-kMinLinearMatch;  // Actual match length minus 1.
                যদি(uchar!=*pos) {
                    stop();
                    ফেরত USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                বিরতি;
            } অন্যথা যদি(গ্রন্থি&k_মান_Is_Final) {
                // No further matching units.
                stop();
                ফেরত USTRINGTRIE_NO_MATCH;
            } অন্যথা {
                // Skip intermediate value.
                pos=skip_গ্রন্থি_মান(pos, গ্রন্থি);
                গ্রন্থি&=k_গ্রন্থি_Type_Mask;
            }
        }
    }
}

ধ্রুবক char16_t*
UCharsTrie::find_Unique_মান_From_Branch(ধ্রুবক char16_t* pos U_LIFETIME_BOUND,
                                      int32_t length,
                                      UBool have_Unique_মান,
                                      int32_t& unique_মান) {
    যখন(length>k_Max_Branch_Linear_Sub_গ্রন্থি_দৈর্ঘ্য) {
        ++pos;  // ignore the comparison unit
        যদি(nullptr==find_Unique_মান_From_Branch(jumpByDelta(pos), length>>1, have_Unique_মান, unique_মান)) {
            ফেরত nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    করো {
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
                    ফেরত nullptr;
                }
            } অন্যথা {
                unique_মান=value;
                have_Unique_মান=সত্য;
            }
        } অন্যথা {
            যদি(!find_Unique_মান(pos+value, have_Unique_মান, unique_মান)) {
                ফেরত nullptr;
            }
            have_Unique_মান=সত্য;
        }
    } যখন(--length>1);
    ফেরত pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::find_Unique_মান(ধ্রুবক char16_t *pos, UBool have_Unique_মান, int32_t &unique_মান) {
    int32_t গ্রন্থি=*pos++;
    জন্য(;;) {
        যদি(গ্রন্থি<kMinLinearMatch) {
            যদি(গ্রন্থি==0) {
                গ্রন্থি=*pos++;
            }
            pos=find_Unique_মান_From_Branch(pos, গ্রন্থি+1, have_Unique_মান, unique_মান);
            যদি(pos==nullptr) {
                ফেরত মিথ্যা;
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
                    ফেরত মিথ্যা;
                }
            } অন্যথা {
                unique_মান=value;
                have_Unique_মান=সত্য;
            }
            যদি(isFinal) {
                ফেরত সত্য;
            }
            pos=skip_গ্রন্থি_মান(pos, গ্রন্থি);
            গ্রন্থি&=k_গ্রন্থি_Type_Mask;
        }
    }
}

int32_t
UCharsTrie::get_পরবর্তী_UChars(Appendable &out) ধ্রুবক {
    ধ্রুবক char16_t *pos=pos_;
    যদি(pos==nullptr) {
        ফেরত 0;
    }
    যদি(remaining_Match_দৈর্ঘ্য>=0) {
        out.appendCodeUnit(*pos);  // Next unit of a pending linear-match node.
        ফেরত 1;
    }
    int32_t গ্রন্থি=*pos++;
    যদি(গ্রন্থি>=k_Min_মান_Lead) {
        যদি(গ্রন্থি&k_মান_Is_Final) {
            ফেরত 0;
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
        get_পরবর্তী_Branch_UChars(pos, গ্রন্থি, out);
        ফেরত গ্রন্থি;
    } অন্যথা {
        // First unit of the linear-match node.
        out.appendCodeUnit(*pos);
        ফেরত 1;
    }
}

শূন্য
UCharsTrie::get_পরবর্তী_Branch_UChars(ধ্রুবক char16_t *pos, int32_t length, Appendable &out) {
    যখন(length>k_Max_Branch_Linear_Sub_গ্রন্থি_দৈর্ঘ্য) {
        ++pos;  // ignore the comparison unit
        get_পরবর্তী_Branch_UChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    করো {
        out.appendCodeUnit(*pos++);
        pos=skip_মান(pos);
    } যখন(--length>1);
    out.appendCodeUnit(*pos);
}

U_NAMESPACE_END
