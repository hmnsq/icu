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

#অন্তর্ভাৱ "unicode/utypes.h"
#অন্তর্ভাৱ "unicode/bytestream.h"
#অন্তর্ভাৱ "unicode/bytestrie.h"
#অন্তর্ভাৱ "unicode/uobject.h"
#অন্তর্ভাৱ "cmemory.h"
#অন্তর্ভাৱ "uassert.h"

U_NAMESPACE_BEGIN

BytesTrie::~BytesTrie() {
    uprv_free(ownedArray_);
}

// lead byte already shifted right by 1.
int32_t
BytesTrie::read_মান(ধ্রুৱ uint8_t *pos, int32_t leadByte) {
    int32_t value;
    যদি(leadByte<k_Min_Two_Byte_মান_Lead) {
        value=leadByte-k_Min_One_Byte_মান_Lead;
    } অন্যথা যদি(leadByte<k_Min_Three_Byte_মান_Lead) {
        value=((leadByte-k_Min_Two_Byte_মান_Lead)<<8)|*pos;
    } অন্যথা যদি(leadByte<k_Four_Byte_মান_Lead) {
        value=((leadByte-k_Min_Three_Byte_মান_Lead)<<16)|(pos[0]<<8)|pos[1];
    } অন্যথা যদি(leadByte==k_Four_Byte_মান_Lead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } অন্যথা {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    ঘূৰাই value;
}

ধ্রুৱ uint8_t*
BytesTrie::jumpByDelta(ধ্রুৱ uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    যদি(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } অন্যথা যদি(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } অন্যথা যদি(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } অন্যথা যদি(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } অন্যথা {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    ঘূৰাই pos+delta;
}

UStringTrieResult
BytesTrie::current() ধ্রুৱ {
    ধ্রুৱ uint8_t *pos=pos_;
    যদি(pos==nullptr) {
        ঘূৰাই USTRINGTRIE_NO_MATCH;
    } অন্যথা {
        int32_t গ্ৰন্থি;
        ঘূৰাই (remainingMatchLength_<0 && (গ্ৰন্থি=*pos)>=k_Min_মান_Lead) ?
                মান_Result(গ্ৰন্থি) : USTRINGTRIE_NO_মান;
    }
}

UStringTrieResult
BytesTrie::branchNext(ধ্রুৱ uint8_t *pos, int32_t length, int32_t inByte) {
    // Branch according to the current byte.
    যদি(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    যেতিয়ালৈ(length>k_Max_Branch_Linear_Sub_গ্ৰন্থি_Length) {
        যদি(inByte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } অন্যথা {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few bytes.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    কৰা {
        যদি(inByte==*pos++) {
            UStringTrieResult result;
            int32_t গ্ৰন্থি=*pos;
            U_ASSERT(গ্ৰন্থি>=k_Min_মান_Lead);
            যদি(গ্ৰন্থি&k_মান_Is_Final) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_মান;
            } অন্যথা {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                গ্ৰন্থি>>=1;
                int32_t delta;
                যদি(গ্ৰন্থি<k_Min_Two_Byte_মান_Lead) {
                    delta=গ্ৰন্থি-k_Min_One_Byte_মান_Lead;
                } অন্যথা যদি(গ্ৰন্থি<k_Min_Three_Byte_মান_Lead) {
                    delta=((গ্ৰন্থি-k_Min_Two_Byte_মান_Lead)<<8)|*pos++;
                } অন্যথা যদি(গ্ৰন্থি<k_Four_Byte_মান_Lead) {
                    delta=((গ্ৰন্থি-k_Min_Three_Byte_মান_Lead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } অন্যথা যদি(গ্ৰন্থি==k_Four_Byte_মান_Lead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } অন্যথা {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                গ্ৰন্থি=*pos;
                result= গ্ৰন্থি>=k_Min_মান_Lead ? মান_Result(গ্ৰন্থি) : USTRINGTRIE_NO_মান;
            }
            pos_=pos;
            ঘূৰাই result;
        }
        --length;
        pos=skip_মান(pos);
    } যেতিয়ালৈ(length>1);
    যদি(inByte==*pos++) {
        pos_=pos;
        int32_t গ্ৰন্থি=*pos;
        ঘূৰাই গ্ৰন্থি>=k_Min_মান_Lead ? মান_Result(গ্ৰন্থি) : USTRINGTRIE_NO_মান;
    } অন্যথা {
        stop();
        ঘূৰাই USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
BytesTrie::nextImpl(ধ্রুৱ uint8_t *pos, int32_t inByte) {
    বাবে(;;) {
        int32_t গ্ৰন্থি=*pos++;
        যদি(গ্ৰন্থি<kMinLinearMatch) {
            ঘূৰাই branchNext(pos, গ্ৰন্থি, inByte);
        } অন্যথা যদি(গ্ৰন্থি<k_Min_মান_Lead) {
            // Match the first of length+1 bytes.
            int32_t length=গ্ৰন্থি-kMinLinearMatch;  // Actual match length minus 1.
            যদি(inByte==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                ঘূৰাই (length<0 && (গ্ৰন্থি=*pos)>=k_Min_মান_Lead) ?
                        মান_Result(গ্ৰন্থি) : USTRINGTRIE_NO_মান;
            } অন্যথা {
                // No match.
                ভঙ্গ;
            }
        } অন্যথা যদি(গ্ৰন্থি&k_মান_Is_Final) {
            // No further matching bytes.
            ভঙ্গ;
        } অন্যথা {
            // Skip intermediate value.
            pos=skip_মান(pos, গ্ৰন্থি);
            // The next node must not also be a value node.
            U_ASSERT(*pos<k_Min_মান_Lead);
        }
    }
    stop();
    ঘূৰাই USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
BytesTrie::next(int32_t inByte) {
    ধ্রুৱ uint8_t *pos=pos_;
    যদি(pos==nullptr) {
        ঘূৰাই USTRINGTRIE_NO_MATCH;
    }
    যদি(inByte<0) {
        inByte+=0x100;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    যদি(length>=0) {
        // Remaining part of a linear-match node.
        যদি(inByte==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t গ্ৰন্থি;
            ঘূৰাই (length<0 && (গ্ৰন্থি=*pos)>=k_Min_মান_Lead) ?
                    মান_Result(গ্ৰন্থি) : USTRINGTRIE_NO_মান;
        } অন্যথা {
            stop();
            ঘূৰাই USTRINGTRIE_NO_MATCH;
        }
    }
    ঘূৰাই nextImpl(pos, inByte);
}

UStringTrieResult
BytesTrie::next(ধ্রুৱ ৱর্ণ *s, int32_t sLength) {
    যদি(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        ঘূৰাই current();
    }
    ধ্রুৱ uint8_t *pos=pos_;
    যদি(pos==nullptr) {
        ঘূৰাই USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    বাবে(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t inByte;
        যদি(sLength<0) {
            বাবে(;;) {
                যদি((inByte=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t গ্ৰন্থি;
                    ঘূৰাই (length<0 && (গ্ৰন্থি=*pos)>=k_Min_মান_Lead) ?
                            মান_Result(গ্ৰন্থি) : USTRINGTRIE_NO_মান;
                }
                যদি(length<0) {
                    remainingMatchLength_=length;
                    ভঙ্গ;
                }
                যদি(inByte!=*pos) {
                    stop();
                    ঘূৰাই USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } অন্যথা {
            বাবে(;;) {
                যদি(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t গ্ৰন্থি;
                    ঘূৰাই (length<0 && (গ্ৰন্থি=*pos)>=k_Min_মান_Lead) ?
                            মান_Result(গ্ৰন্থি) : USTRINGTRIE_NO_মান;
                }
                inByte=*s++;
                --sLength;
                যদি(length<0) {
                    remainingMatchLength_=length;
                    ভঙ্গ;
                }
                যদি(inByte!=*pos) {
                    stop();
                    ঘূৰাই USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        বাবে(;;) {
            int32_t গ্ৰন্থি=*pos++;
            যদি(গ্ৰন্থি<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, গ্ৰন্থি, inByte);
                যদি(result==USTRINGTRIE_NO_MATCH) {
                    ঘূৰাই USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input byte, if there is one.
                যদি(sLength<0) {
                    যদি((inByte=*s++)==0) {
                        ঘূৰাই result;
                    }
                } অন্যথা {
                    যদি(sLength==0) {
                        ঘূৰাই result;
                    }
                    inByte=*s++;
                    --sLength;
                }
                যদি(result==USTRINGTRIE_FINAL_মান) {
                    // No further matching bytes.
                    stop();
                    ঘূৰাই USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } অন্যথা যদি(গ্ৰন্থি<k_Min_মান_Lead) {
                // Match length+1 bytes.
                length=গ্ৰন্থি-kMinLinearMatch;  // Actual match length minus 1.
                যদি(inByte!=*pos) {
                    stop();
                    ঘূৰাই USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                ভঙ্গ;
            } অন্যথা যদি(গ্ৰন্থি&k_মান_Is_Final) {
                // No further matching bytes.
                stop();
                ঘূৰাই USTRINGTRIE_NO_MATCH;
            } অন্যথা {
                // Skip intermediate value.
                pos=skip_মান(pos, গ্ৰন্থি);
                // The next node must not also be a value node.
                U_ASSERT(*pos<k_Min_মান_Lead);
            }
        }
    }
}

ধ্রুৱ uint8_t*
BytesTrie::find_Unique_মান_From_Branch(ধ্রুৱ uint8_t* pos U_LIFETIME_BOUND,
                                     int32_t length,
                                     UBool have_Unique_মান,
                                     int32_t& unique_মান) {
    যেতিয়ালৈ(length>k_Max_Branch_Linear_Sub_গ্ৰন্থি_Length) {
        ++pos;  // ignore the comparison byte
        যদি(nullptr==find_Unique_মান_From_Branch(jumpByDelta(pos), length>>1, have_Unique_মান, unique_মান)) {
            ঘূৰাই nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    কৰা {
        ++pos;  // ignore a comparison byte
        // handle its value
        int32_t গ্ৰন্থি=*pos++;
        UBool isFinal = static_cast<UBool>(গ্ৰন্থি & k_মান_Is_Final);
        int32_t value=read_মান(pos, গ্ৰন্থি>>1);
        pos=skip_মান(pos, গ্ৰন্থি);
        যদি(isFinal) {
            যদি(have_Unique_মান) {
                যদি(value!=unique_মান) {
                    ঘূৰাই nullptr;
                }
            } অন্যথা {
                unique_মান=value;
                have_Unique_মান=সঁচা;
            }
        } অন্যথা {
            যদি(!find_Unique_মান(pos+value, have_Unique_মান, unique_মান)) {
                ঘূৰাই nullptr;
            }
            have_Unique_মান=সঁচা;
        }
    } যেতিয়ালৈ(--length>1);
    ঘূৰাই pos+1;  // ignore the last comparison byte
}

UBool
BytesTrie::find_Unique_মান(ধ্রুৱ uint8_t *pos, UBool have_Unique_মান, int32_t &unique_মান) {
    বাবে(;;) {
        int32_t গ্ৰন্থি=*pos++;
        যদি(গ্ৰন্থি<kMinLinearMatch) {
            যদি(গ্ৰন্থি==0) {
                গ্ৰন্থি=*pos++;
            }
            pos=find_Unique_মান_From_Branch(pos, গ্ৰন্থি+1, have_Unique_মান, unique_মান);
            যদি(pos==nullptr) {
                ঘূৰাই মিছা;
            }
            have_Unique_মান=সঁচা;
        } অন্যথা যদি(গ্ৰন্থি<k_Min_মান_Lead) {
            // linear-match node
            pos+=গ্ৰন্থি-kMinLinearMatch+1;  // Ignore the match bytes.
        } অন্যথা {
            UBool isFinal = static_cast<UBool>(গ্ৰন্থি & k_মান_Is_Final);
            int32_t value=read_মান(pos, গ্ৰন্থি>>1);
            যদি(have_Unique_মান) {
                যদি(value!=unique_মান) {
                    ঘূৰাই মিছা;
                }
            } অন্যথা {
                unique_মান=value;
                have_Unique_মান=সঁচা;
            }
            যদি(isFinal) {
                ঘূৰাই সঁচা;
            }
            pos=skip_মান(pos, গ্ৰন্থি);
        }
    }
}

int32_t
BytesTrie::getNextBytes(ByteSink &out) ধ্রুৱ {
    ধ্রুৱ uint8_t *pos=pos_;
    যদি(pos==nullptr) {
        ঘূৰাই 0;
    }
    যদি(remainingMatchLength_>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        ঘূৰাই 1;
    }
    int32_t গ্ৰন্থি=*pos++;
    যদি(গ্ৰন্থি>=k_Min_মান_Lead) {
        যদি(গ্ৰন্থি&k_মান_Is_Final) {
            ঘূৰাই 0;
        } অন্যথা {
            pos=skip_মান(pos, গ্ৰন্থি);
            গ্ৰন্থি=*pos++;
            U_ASSERT(গ্ৰন্থি<k_Min_মান_Lead);
        }
    }
    যদি(গ্ৰন্থি<kMinLinearMatch) {
        যদি(গ্ৰন্থি==0) {
            গ্ৰন্থি=*pos++;
        }
        getNextBranchBytes(pos, ++গ্ৰন্থি, out);
        ঘূৰাই গ্ৰন্থি;
    } অন্যথা {
        // First byte of the linear-match node.
        append(out, *pos);
        ঘূৰাই 1;
    }
}

শূন্য
BytesTrie::getNextBranchBytes(ধ্রুৱ uint8_t *pos, int32_t length, ByteSink &out) {
    যেতিয়ালৈ(length>k_Max_Branch_Linear_Sub_গ্ৰন্থি_Length) {
        ++pos;  // ignore the comparison byte
        getNextBranchBytes(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    কৰা {
        append(out, *pos++);
        pos=skip_মান(pos);
    } যেতিয়ালৈ(--length>1);
    append(out, *pos);
}

শূন্য
BytesTrie::append(ByteSink &out, পূর্ণাঙ্ক c) {
    ৱর্ণ ch = static_cast<ৱর্ণ>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_END
