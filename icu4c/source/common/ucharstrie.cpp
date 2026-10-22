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

#include "unicode/utypes.h"
#include "unicode/appendable.h"
#include "unicode/ucharstrie.h"
#include "unicode/uobject.h"
#include "unicode/utf16.h"
#include "cmemory.h"
#include "uassert.h"

U_NAMESPACE_BEGIN

UCharsTrie::~UCharsTrie() {
    uprv_free(ownedArray_);
}

UStringTrieResult
UCharsTrie::current() const {
    const char16_t *pos=pos_;
    if(pos==nullptr) {
        return USTRINGTRIE_NO_MATCH;
    } else {
        int32_t बुहोल;
        return (remainingMatchLength_<0 && (बुहोल=*pos)>=k_Min_हालागा_Lead) ?
                हालागा_Result(बुहोल) : USTRINGTRIE_NO_हालागा;
    }
}

UStringTrieResult
UCharsTrie::firstForCodePoint(UChar32 cp) {
    return cp<=0xffff ?
        first(cp) :
        (USTRINGTRIE_HAS_NEXT(first(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::nextForCodePoint(UChar32 cp) {
    return cp<=0xffff ?
        next(cp) :
        (USTRINGTRIE_HAS_NEXT(next(U16_LEAD(cp))) ?
            next(U16_TRAIL(cp)) :
            USTRINGTRIE_NO_MATCH);
}

UStringTrieResult
UCharsTrie::branchNext(const char16_t *pos, int32_t length, int32_t uchar) {
    // Branch according to the current unit.
    if(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of units to select from.
    // The data structure encodes a binary search.
    while(length>k_Max_Branch_Linear_Sub_बुहोल_Length) {
        if(uchar<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } else {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few units.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    do {
        if(uchar==*pos++) {
            UStringTrieResult result;
            int32_t बुहोल=*pos;
            if(बुहोल&k_हालागा_Is_Final) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_हालागा;
            } else {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node);
                int32_t delta;
                if(बुहोल<k_Min_Two_Unit_हालागा_Lead) {
                    delta=बुहोल;
                } else if(बुहोल<k_Three_Unit_हालागा_Lead) {
                    delta=((बुहोल-k_Min_Two_Unit_हालागा_Lead)<<16)|*pos++;
                } else {
                    delta=(pos[0]<<16)|pos[1];
                    pos+=2;
                }
                // end readValue()
                pos+=delta;
                बुहोल=*pos;
                result= बुहोल>=k_Min_हालागा_Lead ? हालागा_Result(बुहोल) : USTRINGTRIE_NO_हालागा;
            }
            pos_=pos;
            return result;
        }
        --length;
        pos=skip_हालागा(pos);
    } while(length>1);
    if(uchar==*pos++) {
        pos_=pos;
        int32_t बुहोल=*pos;
        return बुहोल>=k_Min_हालागा_Lead ? हालागा_Result(बुहोल) : USTRINGTRIE_NO_हालागा;
    } else {
        stop();
        return USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
UCharsTrie::nextImpl(const char16_t *pos, int32_t uchar) {
    int32_t बुहोल=*pos++;
    for(;;) {
        if(बुहोल<kMinLinearMatch) {
            return branchNext(pos, बुहोल, uchar);
        } else if(बुहोल<k_Min_हालागा_Lead) {
            // Match the first of length+1 units.
            int32_t length=बुहोल-kMinLinearMatch;  // Actual match length minus 1.
            if(uchar==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                return (length<0 && (बुहोल=*pos)>=k_Min_हालागा_Lead) ?
                        हालागा_Result(बुहोल) : USTRINGTRIE_NO_हालागा;
            } else {
                // No match.
                break;
            }
        } else if(बुहोल&k_हालागा_Is_Final) {
            // No further matching units.
            break;
        } else {
            // Skip intermediate value.
            pos=skip_बुहोल_हालागा(pos, बुहोल);
            बुहोल&=k_बुहोल_Type_Mask;
        }
    }
    stop();
    return USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
UCharsTrie::next(int32_t uchar) {
    const char16_t *pos=pos_;
    if(pos==nullptr) {
        return USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    if(length>=0) {
        // Remaining part of a linear-match node.
        if(uchar==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t बुहोल;
            return (length<0 && (बुहोल=*pos)>=k_Min_हालागा_Lead) ?
                    हालागा_Result(बुहोल) : USTRINGTRIE_NO_हालागा;
        } else {
            stop();
            return USTRINGTRIE_NO_MATCH;
        }
    }
    return nextImpl(pos, uchar);
}

UStringTrieResult
UCharsTrie::next(ConstChar16Ptr ptr, int32_t sLength) {
    const char16_t *s=ptr;
    if(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        return current();
    }
    const char16_t *pos=pos_;
    if(pos==nullptr) {
        return USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    for(;;) {
        // Fetch the next input unit, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t uchar;
        if(sLength<0) {
            for(;;) {
                if((uchar=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t बुहोल;
                    return (length<0 && (बुहोल=*pos)>=k_Min_हालागा_Lead) ?
                            हालागा_Result(बुहोल) : USTRINGTRIE_NO_हालागा;
                }
                if(length<0) {
                    remainingMatchLength_=length;
                    break;
                }
                if(uchar!=*pos) {
                    stop();
                    return USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } else {
            for(;;) {
                if(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t बुहोल;
                    return (length<0 && (बुहोल=*pos)>=k_Min_हालागा_Lead) ?
                            हालागा_Result(बुहोल) : USTRINGTRIE_NO_हालागा;
                }
                uchar=*s++;
                --sLength;
                if(length<0) {
                    remainingMatchLength_=length;
                    break;
                }
                if(uchar!=*pos) {
                    stop();
                    return USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        int32_t बुहोल=*pos++;
        for(;;) {
            if(बुहोल<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, बुहोल, uchar);
                if(result==USTRINGTRIE_NO_MATCH) {
                    return USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input unit, if there is one.
                if(sLength<0) {
                    if((uchar=*s++)==0) {
                        return result;
                    }
                } else {
                    if(sLength==0) {
                        return result;
                    }
                    uchar=*s++;
                    --sLength;
                }
                if(result==USTRINGTRIE_FINAL_हालागा) {
                    // No further matching units.
                    stop();
                    return USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
                बुहोल=*pos++;
            } else if(बुहोल<k_Min_हालागा_Lead) {
                // Match length+1 units.
                length=बुहोल-kMinLinearMatch;  // Actual match length minus 1.
                if(uchar!=*pos) {
                    stop();
                    return USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                break;
            } else if(बुहोल&k_हालागा_Is_Final) {
                // No further matching units.
                stop();
                return USTRINGTRIE_NO_MATCH;
            } else {
                // Skip intermediate value.
                pos=skip_बुहोल_हालागा(pos, बुहोल);
                बुहोल&=k_बुहोल_Type_Mask;
            }
        }
    }
}

const char16_t*
UCharsTrie::find_Unique_हालागा_From_Branch(const char16_t* pos U_LIFETIME_BOUND,
                                      int32_t length,
                                      UBool have_Unique_हालागा,
                                      int32_t& unique_हालागा) {
    while(length>k_Max_Branch_Linear_Sub_बुहोल_Length) {
        ++pos;  // ignore the comparison unit
        if(nullptr==find_Unique_हालागा_From_Branch(jumpByDelta(pos), length>>1, have_Unique_हालागा, unique_हालागा)) {
            return nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        ++pos;  // ignore a comparison unit
        // handle its value
        int32_t बुहोल=*pos++;
        UBool isFinal = static_cast<UBool>(बुहोल >> 15);
        बुहोल&=0x7fff;
        int32_t value=read_हालागा(pos, बुहोल);
        pos=skip_हालागा(pos, बुहोल);
        if(isFinal) {
            if(have_Unique_हालागा) {
                if(value!=unique_हालागा) {
                    return nullptr;
                }
            } else {
                unique_हालागा=value;
                have_Unique_हालागा=true;
            }
        } else {
            if(!find_Unique_हालागा(pos+value, have_Unique_हालागा, unique_हालागा)) {
                return nullptr;
            }
            have_Unique_हालागा=true;
        }
    } while(--length>1);
    return pos+1;  // ignore the last comparison unit
}

UBool
UCharsTrie::find_Unique_हालागा(const char16_t *pos, UBool have_Unique_हालागा, int32_t &unique_हालागा) {
    int32_t बुहोल=*pos++;
    for(;;) {
        if(बुहोल<kMinLinearMatch) {
            if(बुहोल==0) {
                बुहोल=*pos++;
            }
            pos=find_Unique_हालागा_From_Branch(pos, बुहोल+1, have_Unique_हालागा, unique_हालागा);
            if(pos==nullptr) {
                return false;
            }
            have_Unique_हालागा=true;
            बुहोल=*pos++;
        } else if(बुहोल<k_Min_हालागा_Lead) {
            // linear-match node
            pos+=बुहोल-kMinLinearMatch+1;  // Ignore the match units.
            बुहोल=*pos++;
        } else {
            UBool isFinal = static_cast<UBool>(बुहोल >> 15);
            int32_t value;
            if(isFinal) {
                value=read_हालागा(pos, बुहोल&0x7fff);
            } else {
                value=read_बुहोल_हालागा(pos, बुहोल);
            }
            if(have_Unique_हालागा) {
                if(value!=unique_हालागा) {
                    return false;
                }
            } else {
                unique_हालागा=value;
                have_Unique_हालागा=true;
            }
            if(isFinal) {
                return true;
            }
            pos=skip_बुहोल_हालागा(pos, बुहोल);
            बुहोल&=k_बुहोल_Type_Mask;
        }
    }
}

int32_t
UCharsTrie::getNextUChars(Appendable &out) const {
    const char16_t *pos=pos_;
    if(pos==nullptr) {
        return 0;
    }
    if(remainingMatchLength_>=0) {
        out.appendCodeUnit(*pos);  // Next unit of a pending linear-match node.
        return 1;
    }
    int32_t बुहोल=*pos++;
    if(बुहोल>=k_Min_हालागा_Lead) {
        if(बुहोल&k_हालागा_Is_Final) {
            return 0;
        } else {
            pos=skip_बुहोल_हालागा(pos, बुहोल);
            बुहोल&=k_बुहोल_Type_Mask;
        }
    }
    if(बुहोल<kMinLinearMatch) {
        if(बुहोल==0) {
            बुहोल=*pos++;
        }
        out.reserveAppendCapacity(++बुहोल);
        getNextBranchUChars(pos, बुहोल, out);
        return बुहोल;
    } else {
        // First unit of the linear-match node.
        out.appendCodeUnit(*pos);
        return 1;
    }
}

void
UCharsTrie::getNextBranchUChars(const char16_t *pos, int32_t length, Appendable &out) {
    while(length>k_Max_Branch_Linear_Sub_बुहोल_Length) {
        ++pos;  // ignore the comparison unit
        getNextBranchUChars(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    do {
        out.appendCodeUnit(*pos++);
        pos=skip_हालागा(pos);
    } while(--length>1);
    out.appendCodeUnit(*pos);
}

U_NAMESPACE_END
