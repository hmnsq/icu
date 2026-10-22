// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2010-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  bytestrie.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010sep25
*   created by: Markus W. Scherer
*/

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/bytestream.h"
#ଅନ୍ତର୍ଭାଵ "unicode/bytestrie.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uobject.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"

U_NAMESPACE_BEGIN

BytesTrie::~BytesTrie() {
    uprv_free(ownedArray_);
}

// lead byte already shifted right by 1.
int32_t
BytesTrie::read_ମୂଲ୍ୟ(ଅଚଳ uint8_t *pos, int32_t leadByte) {
    int32_t value;
    ଯଦି(leadByte<k_Min_Two_Byte_ମୂଲ୍ୟ_Lead) {
        value=leadByte-k_Min_One_Byte_ମୂଲ୍ୟ_Lead;
    } ଅନ୍ୟଥା ଯଦି(leadByte<k_Min_Three_Byte_ମୂଲ୍ୟ_Lead) {
        value=((leadByte-k_Min_Two_Byte_ମୂଲ୍ୟ_Lead)<<8)|*pos;
    } ଅନ୍ୟଥା ଯଦି(leadByte<k_Four_Byte_ମୂଲ୍ୟ_Lead) {
        value=((leadByte-k_Min_Three_Byte_ମୂଲ୍ୟ_Lead)<<16)|(pos[0]<<8)|pos[1];
    } ଅନ୍ୟଥା ଯଦି(leadByte==k_Four_Byte_ମୂଲ୍ୟ_Lead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } ଅନ୍ୟଥା {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    ଫେରନ୍ତୁ value;
}

ଅଚଳ uint8_t*
BytesTrie::jumpByDelta(ଅଚଳ uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    ଯଦି(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } ଅନ୍ୟଥା ଯଦି(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } ଅନ୍ୟଥା ଯଦି(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } ଅନ୍ୟଥା ଯଦି(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } ଅନ୍ୟଥା {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    ଫେରନ୍ତୁ pos+delta;
}

UStringTrieResult
BytesTrie::current() ଅଚଳ {
    ଅଚଳ uint8_t *pos=pos_;
    ଯଦି(pos==nullptr) {
        ଫେରନ୍ତୁ USTRINGTRIE_NO_MATCH;
    } ଅନ୍ୟଥା {
        int32_t ଗଣ୍ଠି;
        ଫେରନ୍ତୁ (remainingMatchLength_<0 && (ଗଣ୍ଠି=*pos)>=k_Min_ମୂଲ୍ୟ_Lead) ?
                ମୂଲ୍ୟ_Result(ଗଣ୍ଠି) : USTRINGTRIE_NO_ମୂଲ୍ୟ;
    }
}

UStringTrieResult
BytesTrie::branchNext(ଅଚଳ uint8_t *pos, int32_t length, int32_t inByte) {
    // Branch according to the current byte.
    ଯଦି(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    ଯେତେବେଳେ(length>k_Max_Branch_Linear_Sub_ଗଣ୍ଠି_Length) {
        ଯଦି(inByte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } ଅନ୍ୟଥା {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few bytes.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    କର {
        ଯଦି(inByte==*pos++) {
            UStringTrieResult result;
            int32_t ଗଣ୍ଠି=*pos;
            U_ASSERT(ଗଣ୍ଠି>=k_Min_ମୂଲ୍ୟ_Lead);
            ଯଦି(ଗଣ୍ଠି&k_ମୂଲ୍ୟ_Is_Final) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_ମୂଲ୍ୟ;
            } ଅନ୍ୟଥା {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                ଗଣ୍ଠି>>=1;
                int32_t delta;
                ଯଦି(ଗଣ୍ଠି<k_Min_Two_Byte_ମୂଲ୍ୟ_Lead) {
                    delta=ଗଣ୍ଠି-k_Min_One_Byte_ମୂଲ୍ୟ_Lead;
                } ଅନ୍ୟଥା ଯଦି(ଗଣ୍ଠି<k_Min_Three_Byte_ମୂଲ୍ୟ_Lead) {
                    delta=((ଗଣ୍ଠି-k_Min_Two_Byte_ମୂଲ୍ୟ_Lead)<<8)|*pos++;
                } ଅନ୍ୟଥା ଯଦି(ଗଣ୍ଠି<k_Four_Byte_ମୂଲ୍ୟ_Lead) {
                    delta=((ଗଣ୍ଠି-k_Min_Three_Byte_ମୂଲ୍ୟ_Lead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } ଅନ୍ୟଥା ଯଦି(ଗଣ୍ଠି==k_Four_Byte_ମୂଲ୍ୟ_Lead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } ଅନ୍ୟଥା {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                ଗଣ୍ଠି=*pos;
                result= ଗଣ୍ଠି>=k_Min_ମୂଲ୍ୟ_Lead ? ମୂଲ୍ୟ_Result(ଗଣ୍ଠି) : USTRINGTRIE_NO_ମୂଲ୍ୟ;
            }
            pos_=pos;
            ଫେରନ୍ତୁ result;
        }
        --length;
        pos=skip_ମୂଲ୍ୟ(pos);
    } ଯେତେବେଳେ(length>1);
    ଯଦି(inByte==*pos++) {
        pos_=pos;
        int32_t ଗଣ୍ଠି=*pos;
        ଫେରନ୍ତୁ ଗଣ୍ଠି>=k_Min_ମୂଲ୍ୟ_Lead ? ମୂଲ୍ୟ_Result(ଗଣ୍ଠି) : USTRINGTRIE_NO_ମୂଲ୍ୟ;
    } ଅନ୍ୟଥା {
        stop();
        ଫେରନ୍ତୁ USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
BytesTrie::nextImpl(ଅଚଳ uint8_t *pos, int32_t inByte) {
    ପାଇଁ(;;) {
        int32_t ଗଣ୍ଠି=*pos++;
        ଯଦି(ଗଣ୍ଠି<kMinLinearMatch) {
            ଫେରନ୍ତୁ branchNext(pos, ଗଣ୍ଠି, inByte);
        } ଅନ୍ୟଥା ଯଦି(ଗଣ୍ଠି<k_Min_ମୂଲ୍ୟ_Lead) {
            // Match the first of length+1 bytes.
            int32_t length=ଗଣ୍ଠି-kMinLinearMatch;  // Actual match length minus 1.
            ଯଦି(inByte==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                ଫେରନ୍ତୁ (length<0 && (ଗଣ୍ଠି=*pos)>=k_Min_ମୂଲ୍ୟ_Lead) ?
                        ମୂଲ୍ୟ_Result(ଗଣ୍ଠି) : USTRINGTRIE_NO_ମୂଲ୍ୟ;
            } ଅନ୍ୟଥା {
                // No match.
                ଭାଙ୍ଗ;
            }
        } ଅନ୍ୟଥା ଯଦି(ଗଣ୍ଠି&k_ମୂଲ୍ୟ_Is_Final) {
            // No further matching bytes.
            ଭାଙ୍ଗ;
        } ଅନ୍ୟଥା {
            // Skip intermediate value.
            pos=skip_ମୂଲ୍ୟ(pos, ଗଣ୍ଠି);
            // The next node must not also be a value node.
            U_ASSERT(*pos<k_Min_ମୂଲ୍ୟ_Lead);
        }
    }
    stop();
    ଫେରନ୍ତୁ USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
BytesTrie::next(int32_t inByte) {
    ଅଚଳ uint8_t *pos=pos_;
    ଯଦି(pos==nullptr) {
        ଫେରନ୍ତୁ USTRINGTRIE_NO_MATCH;
    }
    ଯଦି(inByte<0) {
        inByte+=0x100;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ଯଦି(length>=0) {
        // Remaining part of a linear-match node.
        ଯଦି(inByte==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t ଗଣ୍ଠି;
            ଫେରନ୍ତୁ (length<0 && (ଗଣ୍ଠି=*pos)>=k_Min_ମୂଲ୍ୟ_Lead) ?
                    ମୂଲ୍ୟ_Result(ଗଣ୍ଠି) : USTRINGTRIE_NO_ମୂଲ୍ୟ;
        } ଅନ୍ୟଥା {
            stop();
            ଫେରନ୍ତୁ USTRINGTRIE_NO_MATCH;
        }
    }
    ଫେରନ୍ତୁ nextImpl(pos, inByte);
}

UStringTrieResult
BytesTrie::next(ଅଚଳ ଅକ୍ଷର *s, int32_t sLength) {
    ଯଦି(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        ଫେରନ୍ତୁ current();
    }
    ଅଚଳ uint8_t *pos=pos_;
    ଯଦି(pos==nullptr) {
        ଫେରନ୍ତୁ USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    ପାଇଁ(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t inByte;
        ଯଦି(sLength<0) {
            ପାଇଁ(;;) {
                ଯଦି((inByte=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t ଗଣ୍ଠି;
                    ଫେରନ୍ତୁ (length<0 && (ଗଣ୍ଠି=*pos)>=k_Min_ମୂଲ୍ୟ_Lead) ?
                            ମୂଲ୍ୟ_Result(ଗଣ୍ଠି) : USTRINGTRIE_NO_ମୂଲ୍ୟ;
                }
                ଯଦି(length<0) {
                    remainingMatchLength_=length;
                    ଭାଙ୍ଗ;
                }
                ଯଦି(inByte!=*pos) {
                    stop();
                    ଫେରନ୍ତୁ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } ଅନ୍ୟଥା {
            ପାଇଁ(;;) {
                ଯଦି(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t ଗଣ୍ଠି;
                    ଫେରନ୍ତୁ (length<0 && (ଗଣ୍ଠି=*pos)>=k_Min_ମୂଲ୍ୟ_Lead) ?
                            ମୂଲ୍ୟ_Result(ଗଣ୍ଠି) : USTRINGTRIE_NO_ମୂଲ୍ୟ;
                }
                inByte=*s++;
                --sLength;
                ଯଦି(length<0) {
                    remainingMatchLength_=length;
                    ଭାଙ୍ଗ;
                }
                ଯଦି(inByte!=*pos) {
                    stop();
                    ଫେରନ୍ତୁ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        ପାଇଁ(;;) {
            int32_t ଗଣ୍ଠି=*pos++;
            ଯଦି(ଗଣ୍ଠି<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, ଗଣ୍ଠି, inByte);
                ଯଦି(result==USTRINGTRIE_NO_MATCH) {
                    ଫେରନ୍ତୁ USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input byte, if there is one.
                ଯଦି(sLength<0) {
                    ଯଦି((inByte=*s++)==0) {
                        ଫେରନ୍ତୁ result;
                    }
                } ଅନ୍ୟଥା {
                    ଯଦି(sLength==0) {
                        ଫେରନ୍ତୁ result;
                    }
                    inByte=*s++;
                    --sLength;
                }
                ଯଦି(result==USTRINGTRIE_FINAL_ମୂଲ୍ୟ) {
                    // No further matching bytes.
                    stop();
                    ଫେରନ୍ତୁ USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } ଅନ୍ୟଥା ଯଦି(ଗଣ୍ଠି<k_Min_ମୂଲ୍ୟ_Lead) {
                // Match length+1 bytes.
                length=ଗଣ୍ଠି-kMinLinearMatch;  // Actual match length minus 1.
                ଯଦି(inByte!=*pos) {
                    stop();
                    ଫେରନ୍ତୁ USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                ଭାଙ୍ଗ;
            } ଅନ୍ୟଥା ଯଦି(ଗଣ୍ଠି&k_ମୂଲ୍ୟ_Is_Final) {
                // No further matching bytes.
                stop();
                ଫେରନ୍ତୁ USTRINGTRIE_NO_MATCH;
            } ଅନ୍ୟଥା {
                // Skip intermediate value.
                pos=skip_ମୂଲ୍ୟ(pos, ଗଣ୍ଠି);
                // The next node must not also be a value node.
                U_ASSERT(*pos<k_Min_ମୂଲ୍ୟ_Lead);
            }
        }
    }
}

ଅଚଳ uint8_t*
BytesTrie::find_Unique_ମୂଲ୍ୟ_From_Branch(ଅଚଳ uint8_t* pos U_LIFETIME_BOUND,
                                     int32_t length,
                                     UBool have_Unique_ମୂଲ୍ୟ,
                                     int32_t& unique_ମୂଲ୍ୟ) {
    ଯେତେବେଳେ(length>k_Max_Branch_Linear_Sub_ଗଣ୍ଠି_Length) {
        ++pos;  // ignore the comparison byte
        ଯଦି(nullptr==find_Unique_ମୂଲ୍ୟ_From_Branch(jumpByDelta(pos), length>>1, have_Unique_ମୂଲ୍ୟ, unique_ମୂଲ୍ୟ)) {
            ଫେରନ୍ତୁ nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    କର {
        ++pos;  // ignore a comparison byte
        // handle its value
        int32_t ଗଣ୍ଠି=*pos++;
        UBool isFinal = static_cast<UBool>(ଗଣ୍ଠି & k_ମୂଲ୍ୟ_Is_Final);
        int32_t value=read_ମୂଲ୍ୟ(pos, ଗଣ୍ଠି>>1);
        pos=skip_ମୂଲ୍ୟ(pos, ଗଣ୍ଠି);
        ଯଦି(isFinal) {
            ଯଦି(have_Unique_ମୂଲ୍ୟ) {
                ଯଦି(value!=unique_ମୂଲ୍ୟ) {
                    ଫେରନ୍ତୁ nullptr;
                }
            } ଅନ୍ୟଥା {
                unique_ମୂଲ୍ୟ=value;
                have_Unique_ମୂଲ୍ୟ=ସତ୍ୟ;
            }
        } ଅନ୍ୟଥା {
            ଯଦି(!find_Unique_ମୂଲ୍ୟ(pos+value, have_Unique_ମୂଲ୍ୟ, unique_ମୂଲ୍ୟ)) {
                ଫେରନ୍ତୁ nullptr;
            }
            have_Unique_ମୂଲ୍ୟ=ସତ୍ୟ;
        }
    } ଯେତେବେଳେ(--length>1);
    ଫେରନ୍ତୁ pos+1;  // ignore the last comparison byte
}

UBool
BytesTrie::find_Unique_ମୂଲ୍ୟ(ଅଚଳ uint8_t *pos, UBool have_Unique_ମୂଲ୍ୟ, int32_t &unique_ମୂଲ୍ୟ) {
    ପାଇଁ(;;) {
        int32_t ଗଣ୍ଠି=*pos++;
        ଯଦି(ଗଣ୍ଠି<kMinLinearMatch) {
            ଯଦି(ଗଣ୍ଠି==0) {
                ଗଣ୍ଠି=*pos++;
            }
            pos=find_Unique_ମୂଲ୍ୟ_From_Branch(pos, ଗଣ୍ଠି+1, have_Unique_ମୂଲ୍ୟ, unique_ମୂଲ୍ୟ);
            ଯଦି(pos==nullptr) {
                ଫେରନ୍ତୁ ମିଥ୍ୟା;
            }
            have_Unique_ମୂଲ୍ୟ=ସତ୍ୟ;
        } ଅନ୍ୟଥା ଯଦି(ଗଣ୍ଠି<k_Min_ମୂଲ୍ୟ_Lead) {
            // linear-match node
            pos+=ଗଣ୍ଠି-kMinLinearMatch+1;  // Ignore the match bytes.
        } ଅନ୍ୟଥା {
            UBool isFinal = static_cast<UBool>(ଗଣ୍ଠି & k_ମୂଲ୍ୟ_Is_Final);
            int32_t value=read_ମୂଲ୍ୟ(pos, ଗଣ୍ଠି>>1);
            ଯଦି(have_Unique_ମୂଲ୍ୟ) {
                ଯଦି(value!=unique_ମୂଲ୍ୟ) {
                    ଫେରନ୍ତୁ ମିଥ୍ୟା;
                }
            } ଅନ୍ୟଥା {
                unique_ମୂଲ୍ୟ=value;
                have_Unique_ମୂଲ୍ୟ=ସତ୍ୟ;
            }
            ଯଦି(isFinal) {
                ଫେରନ୍ତୁ ସତ୍ୟ;
            }
            pos=skip_ମୂଲ୍ୟ(pos, ଗଣ୍ଠି);
        }
    }
}

int32_t
BytesTrie::getNextBytes(ByteSink &out) ଅଚଳ {
    ଅଚଳ uint8_t *pos=pos_;
    ଯଦି(pos==nullptr) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି(remainingMatchLength_>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        ଫେରନ୍ତୁ 1;
    }
    int32_t ଗଣ୍ଠି=*pos++;
    ଯଦି(ଗଣ୍ଠି>=k_Min_ମୂଲ୍ୟ_Lead) {
        ଯଦି(ଗଣ୍ଠି&k_ମୂଲ୍ୟ_Is_Final) {
            ଫେରନ୍ତୁ 0;
        } ଅନ୍ୟଥା {
            pos=skip_ମୂଲ୍ୟ(pos, ଗଣ୍ଠି);
            ଗଣ୍ଠି=*pos++;
            U_ASSERT(ଗଣ୍ଠି<k_Min_ମୂଲ୍ୟ_Lead);
        }
    }
    ଯଦି(ଗଣ୍ଠି<kMinLinearMatch) {
        ଯଦି(ଗଣ୍ଠି==0) {
            ଗଣ୍ଠି=*pos++;
        }
        getNextBranchBytes(pos, ++ଗଣ୍ଠି, out);
        ଫେରନ୍ତୁ ଗଣ୍ଠି;
    } ଅନ୍ୟଥା {
        // First byte of the linear-match node.
        append(out, *pos);
        ଫେରନ୍ତୁ 1;
    }
}

ଶୂନ୍ୟ
BytesTrie::getNextBranchBytes(ଅଚଳ uint8_t *pos, int32_t length, ByteSink &out) {
    ଯେତେବେଳେ(length>k_Max_Branch_Linear_Sub_ଗଣ୍ଠି_Length) {
        ++pos;  // ignore the comparison byte
        getNextBranchBytes(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    କର {
        append(out, *pos++);
        pos=skip_ମୂଲ୍ୟ(pos);
    } ଯେତେବେଳେ(--length>1);
    append(out, *pos);
}

ଶୂନ୍ୟ
BytesTrie::append(ByteSink &out, ପୂର୍ଣ୍ଣସଂଖ୍ୟା c) {
    ଅକ୍ଷର ch = static_cast<ଅକ୍ଷର>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_END
