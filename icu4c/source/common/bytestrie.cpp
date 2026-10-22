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

#شامل "unicode/utypes.h"
#شامل "unicode/bytestream.h"
#شامل "unicode/bytestrie.h"
#شامل "unicode/uobject.h"
#شامل "cmemory.h"
#شامل "uassert.h"

U_NAMESPACE_BEGIN

BytesTrie::~BytesTrie() {
    uprv_free(ownedArray_);
}

// lead byte already shifted right by 1.
int32_t
BytesTrie::read_مقدار(ثابت uint8_t *pos, int32_t leadByte) {
    int32_t value;
    اگر(leadByte<k_Min_Two_Byte_مقدار_Lead) {
        value=leadByte-k_Min_One_Byte_مقدار_Lead;
    } وگرنه اگر(leadByte<k_Min_Three_Byte_مقدار_Lead) {
        value=((leadByte-k_Min_Two_Byte_مقدار_Lead)<<8)|*pos;
    } وگرنه اگر(leadByte<k_Four_Byte_مقدار_Lead) {
        value=((leadByte-k_Min_Three_Byte_مقدار_Lead)<<16)|(pos[0]<<8)|pos[1];
    } وگرنه اگر(leadByte==k_Four_Byte_مقدار_Lead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } وگرنه {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    بازگشت value;
}

ثابت uint8_t*
BytesTrie::jumpByDelta(ثابت uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    اگر(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } وگرنه اگر(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } وگرنه اگر(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } وگرنه اگر(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } وگرنه {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    بازگشت pos+delta;
}

UStringTrieResult
BytesTrie::current() ثابت {
    ثابت uint8_t *pos=pos_;
    اگر(pos==nullptr) {
        بازگشت USTRINGTRIE_NO_MATCH;
    } وگرنه {
        int32_t گره;
        بازگشت (remainingMatchLength_<0 && (گره=*pos)>=k_Min_مقدار_Lead) ?
                مقدار_Result(گره) : USTRINGTRIE_NO_مقدار;
    }
}

UStringTrieResult
BytesTrie::branchNext(ثابت uint8_t *pos, int32_t length, int32_t inByte) {
    // Branch according to the current byte.
    اگر(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    وقتی(length>k_Max_Branch_Linear_Sub_گره_Length) {
        اگر(inByte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } وگرنه {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few bytes.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    انجامبده {
        اگر(inByte==*pos++) {
            UStringTrieResult result;
            int32_t گره=*pos;
            U_ASSERT(گره>=k_Min_مقدار_Lead);
            اگر(گره&k_مقدار_Is_Final) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_مقدار;
            } وگرنه {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                گره>>=1;
                int32_t delta;
                اگر(گره<k_Min_Two_Byte_مقدار_Lead) {
                    delta=گره-k_Min_One_Byte_مقدار_Lead;
                } وگرنه اگر(گره<k_Min_Three_Byte_مقدار_Lead) {
                    delta=((گره-k_Min_Two_Byte_مقدار_Lead)<<8)|*pos++;
                } وگرنه اگر(گره<k_Four_Byte_مقدار_Lead) {
                    delta=((گره-k_Min_Three_Byte_مقدار_Lead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } وگرنه اگر(گره==k_Four_Byte_مقدار_Lead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } وگرنه {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                گره=*pos;
                result= گره>=k_Min_مقدار_Lead ? مقدار_Result(گره) : USTRINGTRIE_NO_مقدار;
            }
            pos_=pos;
            بازگشت result;
        }
        --length;
        pos=skip_مقدار(pos);
    } وقتی(length>1);
    اگر(inByte==*pos++) {
        pos_=pos;
        int32_t گره=*pos;
        بازگشت گره>=k_Min_مقدار_Lead ? مقدار_Result(گره) : USTRINGTRIE_NO_مقدار;
    } وگرنه {
        stop();
        بازگشت USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
BytesTrie::nextImpl(ثابت uint8_t *pos, int32_t inByte) {
    برای(;;) {
        int32_t گره=*pos++;
        اگر(گره<kMinLinearMatch) {
            بازگشت branchNext(pos, گره, inByte);
        } وگرنه اگر(گره<k_Min_مقدار_Lead) {
            // Match the first of length+1 bytes.
            int32_t length=گره-kMinLinearMatch;  // Actual match length minus 1.
            اگر(inByte==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                بازگشت (length<0 && (گره=*pos)>=k_Min_مقدار_Lead) ?
                        مقدار_Result(گره) : USTRINGTRIE_NO_مقدار;
            } وگرنه {
                // No match.
                بشکن;
            }
        } وگرنه اگر(گره&k_مقدار_Is_Final) {
            // No further matching bytes.
            بشکن;
        } وگرنه {
            // Skip intermediate value.
            pos=skip_مقدار(pos, گره);
            // The next node must not also be a value node.
            U_ASSERT(*pos<k_Min_مقدار_Lead);
        }
    }
    stop();
    بازگشت USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
BytesTrie::next(int32_t inByte) {
    ثابت uint8_t *pos=pos_;
    اگر(pos==nullptr) {
        بازگشت USTRINGTRIE_NO_MATCH;
    }
    اگر(inByte<0) {
        inByte+=0x100;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    اگر(length>=0) {
        // Remaining part of a linear-match node.
        اگر(inByte==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t گره;
            بازگشت (length<0 && (گره=*pos)>=k_Min_مقدار_Lead) ?
                    مقدار_Result(گره) : USTRINGTRIE_NO_مقدار;
        } وگرنه {
            stop();
            بازگشت USTRINGTRIE_NO_MATCH;
        }
    }
    بازگشت nextImpl(pos, inByte);
}

UStringTrieResult
BytesTrie::next(ثابت نویسه *s, int32_t sLength) {
    اگر(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        بازگشت current();
    }
    ثابت uint8_t *pos=pos_;
    اگر(pos==nullptr) {
        بازگشت USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    برای(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t inByte;
        اگر(sLength<0) {
            برای(;;) {
                اگر((inByte=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t گره;
                    بازگشت (length<0 && (گره=*pos)>=k_Min_مقدار_Lead) ?
                            مقدار_Result(گره) : USTRINGTRIE_NO_مقدار;
                }
                اگر(length<0) {
                    remainingMatchLength_=length;
                    بشکن;
                }
                اگر(inByte!=*pos) {
                    stop();
                    بازگشت USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } وگرنه {
            برای(;;) {
                اگر(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t گره;
                    بازگشت (length<0 && (گره=*pos)>=k_Min_مقدار_Lead) ?
                            مقدار_Result(گره) : USTRINGTRIE_NO_مقدار;
                }
                inByte=*s++;
                --sLength;
                اگر(length<0) {
                    remainingMatchLength_=length;
                    بشکن;
                }
                اگر(inByte!=*pos) {
                    stop();
                    بازگشت USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        برای(;;) {
            int32_t گره=*pos++;
            اگر(گره<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, گره, inByte);
                اگر(result==USTRINGTRIE_NO_MATCH) {
                    بازگشت USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input byte, if there is one.
                اگر(sLength<0) {
                    اگر((inByte=*s++)==0) {
                        بازگشت result;
                    }
                } وگرنه {
                    اگر(sLength==0) {
                        بازگشت result;
                    }
                    inByte=*s++;
                    --sLength;
                }
                اگر(result==USTRINGTRIE_FINAL_مقدار) {
                    // No further matching bytes.
                    stop();
                    بازگشت USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } وگرنه اگر(گره<k_Min_مقدار_Lead) {
                // Match length+1 bytes.
                length=گره-kMinLinearMatch;  // Actual match length minus 1.
                اگر(inByte!=*pos) {
                    stop();
                    بازگشت USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                بشکن;
            } وگرنه اگر(گره&k_مقدار_Is_Final) {
                // No further matching bytes.
                stop();
                بازگشت USTRINGTRIE_NO_MATCH;
            } وگرنه {
                // Skip intermediate value.
                pos=skip_مقدار(pos, گره);
                // The next node must not also be a value node.
                U_ASSERT(*pos<k_Min_مقدار_Lead);
            }
        }
    }
}

ثابت uint8_t*
BytesTrie::find_Unique_مقدار_From_Branch(ثابت uint8_t* pos U_LIFETIME_BOUND,
                                     int32_t length,
                                     UBool have_Unique_مقدار,
                                     int32_t& unique_مقدار) {
    وقتی(length>k_Max_Branch_Linear_Sub_گره_Length) {
        ++pos;  // ignore the comparison byte
        اگر(nullptr==find_Unique_مقدار_From_Branch(jumpByDelta(pos), length>>1, have_Unique_مقدار, unique_مقدار)) {
            بازگشت nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    انجامبده {
        ++pos;  // ignore a comparison byte
        // handle its value
        int32_t گره=*pos++;
        UBool isFinal = static_cast<UBool>(گره & k_مقدار_Is_Final);
        int32_t value=read_مقدار(pos, گره>>1);
        pos=skip_مقدار(pos, گره);
        اگر(isFinal) {
            اگر(have_Unique_مقدار) {
                اگر(value!=unique_مقدار) {
                    بازگشت nullptr;
                }
            } وگرنه {
                unique_مقدار=value;
                have_Unique_مقدار=درست;
            }
        } وگرنه {
            اگر(!find_Unique_مقدار(pos+value, have_Unique_مقدار, unique_مقدار)) {
                بازگشت nullptr;
            }
            have_Unique_مقدار=درست;
        }
    } وقتی(--length>1);
    بازگشت pos+1;  // ignore the last comparison byte
}

UBool
BytesTrie::find_Unique_مقدار(ثابت uint8_t *pos, UBool have_Unique_مقدار, int32_t &unique_مقدار) {
    برای(;;) {
        int32_t گره=*pos++;
        اگر(گره<kMinLinearMatch) {
            اگر(گره==0) {
                گره=*pos++;
            }
            pos=find_Unique_مقدار_From_Branch(pos, گره+1, have_Unique_مقدار, unique_مقدار);
            اگر(pos==nullptr) {
                بازگشت نادرست;
            }
            have_Unique_مقدار=درست;
        } وگرنه اگر(گره<k_Min_مقدار_Lead) {
            // linear-match node
            pos+=گره-kMinLinearMatch+1;  // Ignore the match bytes.
        } وگرنه {
            UBool isFinal = static_cast<UBool>(گره & k_مقدار_Is_Final);
            int32_t value=read_مقدار(pos, گره>>1);
            اگر(have_Unique_مقدار) {
                اگر(value!=unique_مقدار) {
                    بازگشت نادرست;
                }
            } وگرنه {
                unique_مقدار=value;
                have_Unique_مقدار=درست;
            }
            اگر(isFinal) {
                بازگشت درست;
            }
            pos=skip_مقدار(pos, گره);
        }
    }
}

int32_t
BytesTrie::getNextBytes(ByteSink &out) ثابت {
    ثابت uint8_t *pos=pos_;
    اگر(pos==nullptr) {
        بازگشت 0;
    }
    اگر(remainingMatchLength_>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        بازگشت 1;
    }
    int32_t گره=*pos++;
    اگر(گره>=k_Min_مقدار_Lead) {
        اگر(گره&k_مقدار_Is_Final) {
            بازگشت 0;
        } وگرنه {
            pos=skip_مقدار(pos, گره);
            گره=*pos++;
            U_ASSERT(گره<k_Min_مقدار_Lead);
        }
    }
    اگر(گره<kMinLinearMatch) {
        اگر(گره==0) {
            گره=*pos++;
        }
        getNextBranchBytes(pos, ++گره, out);
        بازگشت گره;
    } وگرنه {
        // First byte of the linear-match node.
        append(out, *pos);
        بازگشت 1;
    }
}

تهی
BytesTrie::getNextBranchBytes(ثابت uint8_t *pos, int32_t length, ByteSink &out) {
    وقتی(length>k_Max_Branch_Linear_Sub_گره_Length) {
        ++pos;  // ignore the comparison byte
        getNextBranchBytes(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    انجامبده {
        append(out, *pos++);
        pos=skip_مقدار(pos);
    } وقتی(--length>1);
    append(out, *pos);
}

تهی
BytesTrie::append(ByteSink &out, صحیح c) {
    نویسه ch = static_cast<نویسه>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_END
