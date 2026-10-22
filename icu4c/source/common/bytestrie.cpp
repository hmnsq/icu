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

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/bytestream.h"
#অন্তর্ভাব "unicode/bytestrie.h"
#অন্তর্ভাব "unicode/uobject.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "uassert.h"

U_NAMESPACE_BEGIN

BytesTrie::~BytesTrie() {
    uprv_free(ownedArray_);
}

// lead byte already shifted right by 1.
int32_t
BytesTrie::read_মান(ধ্রুবক uint8_t *pos, int32_t leadByte) {
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
    ফেরত value;
}

ধ্রুবক uint8_t*
BytesTrie::jumpByDelta(ধ্রুবক uint8_t* pos U_LIFETIME_BOUND) {
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
    ফেরত pos+delta;
}

UString_Trie_ফলাফল
BytesTrie::current() ধ্রুবক {
    ধ্রুবক uint8_t *pos=pos_;
    যদি(pos==nullptr) {
        ফেরত USTRINGTRIE_NO_MATCH;
    } অন্যথা {
        int32_t গ্রন্থি;
        ফেরত (remaining_Match_দৈর্ঘ্য<0 && (গ্রন্থি=*pos)>=k_Min_মান_Lead) ?
                মান_ফলাফল(গ্রন্থি) : USTRINGTRIE_NO_মান;
    }
}

UString_Trie_ফলাফল
BytesTrie::branch_পরবর্তী(ধ্রুবক uint8_t *pos, int32_t length, int32_t inByte) {
    // Branch according to the current byte.
    যদি(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    যখন(length>k_Max_Branch_Linear_Sub_গ্রন্থি_দৈর্ঘ্য) {
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
    করো {
        যদি(inByte==*pos++) {
            UString_Trie_ফলাফল ফলাফল;
            int32_t গ্রন্থি=*pos;
            U_ASSERT(গ্রন্থি>=k_Min_মান_Lead);
            যদি(গ্রন্থি&k_মান_Is_Final) {
                // Leave the final value for getValue() to read.
                ফলাফল=USTRINGTRIE_FINAL_মান;
            } অন্যথা {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                গ্রন্থি>>=1;
                int32_t delta;
                যদি(গ্রন্থি<k_Min_Two_Byte_মান_Lead) {
                    delta=গ্রন্থি-k_Min_One_Byte_মান_Lead;
                } অন্যথা যদি(গ্রন্থি<k_Min_Three_Byte_মান_Lead) {
                    delta=((গ্রন্থি-k_Min_Two_Byte_মান_Lead)<<8)|*pos++;
                } অন্যথা যদি(গ্রন্থি<k_Four_Byte_মান_Lead) {
                    delta=((গ্রন্থি-k_Min_Three_Byte_মান_Lead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } অন্যথা যদি(গ্রন্থি==k_Four_Byte_মান_Lead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } অন্যথা {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
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
    যদি(inByte==*pos++) {
        pos_=pos;
        int32_t গ্রন্থি=*pos;
        ফেরত গ্রন্থি>=k_Min_মান_Lead ? মান_ফলাফল(গ্রন্থি) : USTRINGTRIE_NO_মান;
    } অন্যথা {
        stop();
        ফেরত USTRINGTRIE_NO_MATCH;
    }
}

UString_Trie_ফলাফল
BytesTrie::পরবর্তী_Impl(ধ্রুবক uint8_t *pos, int32_t inByte) {
    জন্য(;;) {
        int32_t গ্রন্থি=*pos++;
        যদি(গ্রন্থি<kMinLinearMatch) {
            ফেরত branch_পরবর্তী(pos, গ্রন্থি, inByte);
        } অন্যথা যদি(গ্রন্থি<k_Min_মান_Lead) {
            // Match the first of length+1 bytes.
            int32_t length=গ্রন্থি-kMinLinearMatch;  // Actual match length minus 1.
            যদি(inByte==*pos++) {
                remaining_Match_দৈর্ঘ্য=--length;
                pos_=pos;
                ফেরত (length<0 && (গ্রন্থি=*pos)>=k_Min_মান_Lead) ?
                        মান_ফলাফল(গ্রন্থি) : USTRINGTRIE_NO_মান;
            } অন্যথা {
                // No match.
                বিরতি;
            }
        } অন্যথা যদি(গ্রন্থি&k_মান_Is_Final) {
            // No further matching bytes.
            বিরতি;
        } অন্যথা {
            // Skip intermediate value.
            pos=skip_মান(pos, গ্রন্থি);
            // The next node must not also be a value node.
            U_ASSERT(*pos<k_Min_মান_Lead);
        }
    }
    stop();
    ফেরত USTRINGTRIE_NO_MATCH;
}

UString_Trie_ফলাফল
BytesTrie::next(int32_t inByte) {
    ধ্রুবক uint8_t *pos=pos_;
    যদি(pos==nullptr) {
        ফেরত USTRINGTRIE_NO_MATCH;
    }
    যদি(inByte<0) {
        inByte+=0x100;
    }
    int32_t length=remaining_Match_দৈর্ঘ্য;  // Actual remaining match length minus 1.
    যদি(length>=0) {
        // Remaining part of a linear-match node.
        যদি(inByte==*pos++) {
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
    ফেরত পরবর্তী_Impl(pos, inByte);
}

UString_Trie_ফলাফল
BytesTrie::next(ধ্রুবক অক্ষর *s, int32_t s_দৈর্ঘ্য) {
    যদি(s_দৈর্ঘ্য<0 ? *s==0 : s_দৈর্ঘ্য==0) {
        // Empty input.
        ফেরত current();
    }
    ধ্রুবক uint8_t *pos=pos_;
    যদি(pos==nullptr) {
        ফেরত USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remaining_Match_দৈর্ঘ্য;  // Actual remaining match length minus 1.
    জন্য(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t inByte;
        যদি(s_দৈর্ঘ্য<0) {
            জন্য(;;) {
                যদি((inByte=*s++)==0) {
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
                যদি(inByte!=*pos) {
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
                inByte=*s++;
                --s_দৈর্ঘ্য;
                যদি(length<0) {
                    remaining_Match_দৈর্ঘ্য=length;
                    বিরতি;
                }
                যদি(inByte!=*pos) {
                    stop();
                    ফেরত USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        জন্য(;;) {
            int32_t গ্রন্থি=*pos++;
            যদি(গ্রন্থি<kMinLinearMatch) {
                UString_Trie_ফলাফল ফলাফল=branch_পরবর্তী(pos, গ্রন্থি, inByte);
                যদি(ফলাফল==USTRINGTRIE_NO_MATCH) {
                    ফেরত USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input byte, if there is one.
                যদি(s_দৈর্ঘ্য<0) {
                    যদি((inByte=*s++)==0) {
                        ফেরত ফলাফল;
                    }
                } অন্যথা {
                    যদি(s_দৈর্ঘ্য==0) {
                        ফেরত ফলাফল;
                    }
                    inByte=*s++;
                    --s_দৈর্ঘ্য;
                }
                যদি(ফলাফল==USTRINGTRIE_FINAL_মান) {
                    // No further matching bytes.
                    stop();
                    ফেরত USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } অন্যথা যদি(গ্রন্থি<k_Min_মান_Lead) {
                // Match length+1 bytes.
                length=গ্রন্থি-kMinLinearMatch;  // Actual match length minus 1.
                যদি(inByte!=*pos) {
                    stop();
                    ফেরত USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                বিরতি;
            } অন্যথা যদি(গ্রন্থি&k_মান_Is_Final) {
                // No further matching bytes.
                stop();
                ফেরত USTRINGTRIE_NO_MATCH;
            } অন্যথা {
                // Skip intermediate value.
                pos=skip_মান(pos, গ্রন্থি);
                // The next node must not also be a value node.
                U_ASSERT(*pos<k_Min_মান_Lead);
            }
        }
    }
}

ধ্রুবক uint8_t*
BytesTrie::find_Unique_মান_From_Branch(ধ্রুবক uint8_t* pos U_LIFETIME_BOUND,
                                     int32_t length,
                                     UBool have_Unique_মান,
                                     int32_t& unique_মান) {
    যখন(length>k_Max_Branch_Linear_Sub_গ্রন্থি_দৈর্ঘ্য) {
        ++pos;  // ignore the comparison byte
        যদি(nullptr==find_Unique_মান_From_Branch(jumpByDelta(pos), length>>1, have_Unique_মান, unique_মান)) {
            ফেরত nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    করো {
        ++pos;  // ignore a comparison byte
        // handle its value
        int32_t গ্রন্থি=*pos++;
        UBool isFinal = static_cast<UBool>(গ্রন্থি & k_মান_Is_Final);
        int32_t value=read_মান(pos, গ্রন্থি>>1);
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
    ফেরত pos+1;  // ignore the last comparison byte
}

UBool
BytesTrie::find_Unique_মান(ধ্রুবক uint8_t *pos, UBool have_Unique_মান, int32_t &unique_মান) {
    জন্য(;;) {
        int32_t গ্রন্থি=*pos++;
        যদি(গ্রন্থি<kMinLinearMatch) {
            যদি(গ্রন্থি==0) {
                গ্রন্থি=*pos++;
            }
            pos=find_Unique_মান_From_Branch(pos, গ্রন্থি+1, have_Unique_মান, unique_মান);
            যদি(pos==nullptr) {
                ফেরত মিথ্যা;
            }
            have_Unique_মান=সত্য;
        } অন্যথা যদি(গ্রন্থি<k_Min_মান_Lead) {
            // linear-match node
            pos+=গ্রন্থি-kMinLinearMatch+1;  // Ignore the match bytes.
        } অন্যথা {
            UBool isFinal = static_cast<UBool>(গ্রন্থি & k_মান_Is_Final);
            int32_t value=read_মান(pos, গ্রন্থি>>1);
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
            pos=skip_মান(pos, গ্রন্থি);
        }
    }
}

int32_t
BytesTrie::get_পরবর্তী_Bytes(ByteSink &out) ধ্রুবক {
    ধ্রুবক uint8_t *pos=pos_;
    যদি(pos==nullptr) {
        ফেরত 0;
    }
    যদি(remaining_Match_দৈর্ঘ্য>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        ফেরত 1;
    }
    int32_t গ্রন্থি=*pos++;
    যদি(গ্রন্থি>=k_Min_মান_Lead) {
        যদি(গ্রন্থি&k_মান_Is_Final) {
            ফেরত 0;
        } অন্যথা {
            pos=skip_মান(pos, গ্রন্থি);
            গ্রন্থি=*pos++;
            U_ASSERT(গ্রন্থি<k_Min_মান_Lead);
        }
    }
    যদি(গ্রন্থি<kMinLinearMatch) {
        যদি(গ্রন্থি==0) {
            গ্রন্থি=*pos++;
        }
        get_পরবর্তী_Branch_Bytes(pos, ++গ্রন্থি, out);
        ফেরত গ্রন্থি;
    } অন্যথা {
        // First byte of the linear-match node.
        append(out, *pos);
        ফেরত 1;
    }
}

শূন্য
BytesTrie::get_পরবর্তী_Branch_Bytes(ধ্রুবক uint8_t *pos, int32_t length, ByteSink &out) {
    যখন(length>k_Max_Branch_Linear_Sub_গ্রন্থি_দৈর্ঘ্য) {
        ++pos;  // ignore the comparison byte
        get_পরবর্তী_Branch_Bytes(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    করো {
        append(out, *pos++);
        pos=skip_মান(pos);
    } যখন(--length>1);
    append(out, *pos);
}

শূন্য
BytesTrie::append(ByteSink &out, পূর্ণসংখ্যা c) {
    অক্ষর ch = static_cast<অক্ষর>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_END
