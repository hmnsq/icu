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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/bytestream.h"
#अन्तर्भाव "unicode/bytestrie.h"
#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "uassert.h"

U_NAMESPACE_BEGIN

BytesTrie::~BytesTrie() {
    uprv_free(ownedArray_);
}

// lead byte already shifted right by 1.
int32_t
BytesTrie::read_मूल्य(ध्रुव uint8_t *pos, int32_t leadByte) {
    int32_t value;
    यदि(leadByte<k_Min_Two_Byte_मूल्य_Lead) {
        value=leadByte-k_Min_One_Byte_मूल्य_Lead;
    } अन्यथा यदि(leadByte<k_Min_Three_Byte_मूल्य_Lead) {
        value=((leadByte-k_Min_Two_Byte_मूल्य_Lead)<<8)|*pos;
    } अन्यथा यदि(leadByte<k_Four_Byte_मूल्य_Lead) {
        value=((leadByte-k_Min_Three_Byte_मूल्य_Lead)<<16)|(pos[0]<<8)|pos[1];
    } अन्यथा यदि(leadByte==k_Four_Byte_मूल्य_Lead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } अन्यथा {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    प्रत्यागम value;
}

ध्रुव uint8_t*
BytesTrie::jumpByDelta(ध्रुव uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    यदि(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } अन्यथा यदि(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } अन्यथा यदि(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } अन्यथा यदि(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } अन्यथा {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    प्रत्यागम pos+delta;
}

UStringTrieResult
BytesTrie::current() ध्रुव {
    ध्रुव uint8_t *pos=pos_;
    यदि(pos==nullptr) {
        प्रत्यागम USTRINGTRIE_NO_MATCH;
    } अन्यथा {
        int32_t गाँठ;
        प्रत्यागम (remainingMatchLength_<0 && (गाँठ=*pos)>=k_Min_मूल्य_Lead) ?
                मूल्य_Result(गाँठ) : USTRINGTRIE_NO_मूल्य;
    }
}

UStringTrieResult
BytesTrie::branchNext(ध्रुव uint8_t *pos, int32_t length, int32_t inByte) {
    // Branch according to the current byte.
    यदि(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    यावत्(length>k_Max_Branch_Linear_Sub_गाँठ_Length) {
        यदि(inByte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } अन्यथा {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few bytes.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    कुरु {
        यदि(inByte==*pos++) {
            UStringTrieResult result;
            int32_t गाँठ=*pos;
            U_ASSERT(गाँठ>=k_Min_मूल्य_Lead);
            यदि(गाँठ&k_मूल्य_Is_Final) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_मूल्य;
            } अन्यथा {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                गाँठ>>=1;
                int32_t delta;
                यदि(गाँठ<k_Min_Two_Byte_मूल्य_Lead) {
                    delta=गाँठ-k_Min_One_Byte_मूल्य_Lead;
                } अन्यथा यदि(गाँठ<k_Min_Three_Byte_मूल्य_Lead) {
                    delta=((गाँठ-k_Min_Two_Byte_मूल्य_Lead)<<8)|*pos++;
                } अन्यथा यदि(गाँठ<k_Four_Byte_मूल्य_Lead) {
                    delta=((गाँठ-k_Min_Three_Byte_मूल्य_Lead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } अन्यथा यदि(गाँठ==k_Four_Byte_मूल्य_Lead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } अन्यथा {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                गाँठ=*pos;
                result= गाँठ>=k_Min_मूल्य_Lead ? मूल्य_Result(गाँठ) : USTRINGTRIE_NO_मूल्य;
            }
            pos_=pos;
            प्रत्यागम result;
        }
        --length;
        pos=skip_मूल्य(pos);
    } यावत्(length>1);
    यदि(inByte==*pos++) {
        pos_=pos;
        int32_t गाँठ=*pos;
        प्रत्यागम गाँठ>=k_Min_मूल्य_Lead ? मूल्य_Result(गाँठ) : USTRINGTRIE_NO_मूल्य;
    } अन्यथा {
        stop();
        प्रत्यागम USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
BytesTrie::nextImpl(ध्रुव uint8_t *pos, int32_t inByte) {
    प्रति(;;) {
        int32_t गाँठ=*pos++;
        यदि(गाँठ<kMinLinearMatch) {
            प्रत्यागम branchNext(pos, गाँठ, inByte);
        } अन्यथा यदि(गाँठ<k_Min_मूल्य_Lead) {
            // Match the first of length+1 bytes.
            int32_t length=गाँठ-kMinLinearMatch;  // Actual match length minus 1.
            यदि(inByte==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                प्रत्यागम (length<0 && (गाँठ=*pos)>=k_Min_मूल्य_Lead) ?
                        मूल्य_Result(गाँठ) : USTRINGTRIE_NO_मूल्य;
            } अन्यथा {
                // No match.
                भङ्ग;
            }
        } अन्यथा यदि(गाँठ&k_मूल्य_Is_Final) {
            // No further matching bytes.
            भङ्ग;
        } अन्यथा {
            // Skip intermediate value.
            pos=skip_मूल्य(pos, गाँठ);
            // The next node must not also be a value node.
            U_ASSERT(*pos<k_Min_मूल्य_Lead);
        }
    }
    stop();
    प्रत्यागम USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
BytesTrie::next(int32_t inByte) {
    ध्रुव uint8_t *pos=pos_;
    यदि(pos==nullptr) {
        प्रत्यागम USTRINGTRIE_NO_MATCH;
    }
    यदि(inByte<0) {
        inByte+=0x100;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    यदि(length>=0) {
        // Remaining part of a linear-match node.
        यदि(inByte==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t गाँठ;
            प्रत्यागम (length<0 && (गाँठ=*pos)>=k_Min_मूल्य_Lead) ?
                    मूल्य_Result(गाँठ) : USTRINGTRIE_NO_मूल्य;
        } अन्यथा {
            stop();
            प्रत्यागम USTRINGTRIE_NO_MATCH;
        }
    }
    प्रत्यागम nextImpl(pos, inByte);
}

UStringTrieResult
BytesTrie::next(ध्रुव वर्ण *s, int32_t sLength) {
    यदि(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        प्रत्यागम current();
    }
    ध्रुव uint8_t *pos=pos_;
    यदि(pos==nullptr) {
        प्रत्यागम USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    प्रति(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t inByte;
        यदि(sLength<0) {
            प्रति(;;) {
                यदि((inByte=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t गाँठ;
                    प्रत्यागम (length<0 && (गाँठ=*pos)>=k_Min_मूल्य_Lead) ?
                            मूल्य_Result(गाँठ) : USTRINGTRIE_NO_मूल्य;
                }
                यदि(length<0) {
                    remainingMatchLength_=length;
                    भङ्ग;
                }
                यदि(inByte!=*pos) {
                    stop();
                    प्रत्यागम USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } अन्यथा {
            प्रति(;;) {
                यदि(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t गाँठ;
                    प्रत्यागम (length<0 && (गाँठ=*pos)>=k_Min_मूल्य_Lead) ?
                            मूल्य_Result(गाँठ) : USTRINGTRIE_NO_मूल्य;
                }
                inByte=*s++;
                --sLength;
                यदि(length<0) {
                    remainingMatchLength_=length;
                    भङ्ग;
                }
                यदि(inByte!=*pos) {
                    stop();
                    प्रत्यागम USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        प्रति(;;) {
            int32_t गाँठ=*pos++;
            यदि(गाँठ<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, गाँठ, inByte);
                यदि(result==USTRINGTRIE_NO_MATCH) {
                    प्रत्यागम USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input byte, if there is one.
                यदि(sLength<0) {
                    यदि((inByte=*s++)==0) {
                        प्रत्यागम result;
                    }
                } अन्यथा {
                    यदि(sLength==0) {
                        प्रत्यागम result;
                    }
                    inByte=*s++;
                    --sLength;
                }
                यदि(result==USTRINGTRIE_FINAL_मूल्य) {
                    // No further matching bytes.
                    stop();
                    प्रत्यागम USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } अन्यथा यदि(गाँठ<k_Min_मूल्य_Lead) {
                // Match length+1 bytes.
                length=गाँठ-kMinLinearMatch;  // Actual match length minus 1.
                यदि(inByte!=*pos) {
                    stop();
                    प्रत्यागम USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                भङ्ग;
            } अन्यथा यदि(गाँठ&k_मूल्य_Is_Final) {
                // No further matching bytes.
                stop();
                प्रत्यागम USTRINGTRIE_NO_MATCH;
            } अन्यथा {
                // Skip intermediate value.
                pos=skip_मूल्य(pos, गाँठ);
                // The next node must not also be a value node.
                U_ASSERT(*pos<k_Min_मूल्य_Lead);
            }
        }
    }
}

ध्रुव uint8_t*
BytesTrie::find_Unique_मूल्य_From_Branch(ध्रुव uint8_t* pos U_LIFETIME_BOUND,
                                     int32_t length,
                                     UBool have_Unique_मूल्य,
                                     int32_t& unique_मूल्य) {
    यावत्(length>k_Max_Branch_Linear_Sub_गाँठ_Length) {
        ++pos;  // ignore the comparison byte
        यदि(nullptr==find_Unique_मूल्य_From_Branch(jumpByDelta(pos), length>>1, have_Unique_मूल्य, unique_मूल्य)) {
            प्रत्यागम nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    कुरु {
        ++pos;  // ignore a comparison byte
        // handle its value
        int32_t गाँठ=*pos++;
        UBool isFinal = static_cast<UBool>(गाँठ & k_मूल्य_Is_Final);
        int32_t value=read_मूल्य(pos, गाँठ>>1);
        pos=skip_मूल्य(pos, गाँठ);
        यदि(isFinal) {
            यदि(have_Unique_मूल्य) {
                यदि(value!=unique_मूल्य) {
                    प्रत्यागम nullptr;
                }
            } अन्यथा {
                unique_मूल्य=value;
                have_Unique_मूल्य=सत्य;
            }
        } अन्यथा {
            यदि(!find_Unique_मूल्य(pos+value, have_Unique_मूल्य, unique_मूल्य)) {
                प्रत्यागम nullptr;
            }
            have_Unique_मूल्य=सत्य;
        }
    } यावत्(--length>1);
    प्रत्यागम pos+1;  // ignore the last comparison byte
}

UBool
BytesTrie::find_Unique_मूल्य(ध्रुव uint8_t *pos, UBool have_Unique_मूल्य, int32_t &unique_मूल्य) {
    प्रति(;;) {
        int32_t गाँठ=*pos++;
        यदि(गाँठ<kMinLinearMatch) {
            यदि(गाँठ==0) {
                गाँठ=*pos++;
            }
            pos=find_Unique_मूल्य_From_Branch(pos, गाँठ+1, have_Unique_मूल्य, unique_मूल्य);
            यदि(pos==nullptr) {
                प्रत्यागम असत्य;
            }
            have_Unique_मूल्य=सत्य;
        } अन्यथा यदि(गाँठ<k_Min_मूल्य_Lead) {
            // linear-match node
            pos+=गाँठ-kMinLinearMatch+1;  // Ignore the match bytes.
        } अन्यथा {
            UBool isFinal = static_cast<UBool>(गाँठ & k_मूल्य_Is_Final);
            int32_t value=read_मूल्य(pos, गाँठ>>1);
            यदि(have_Unique_मूल्य) {
                यदि(value!=unique_मूल्य) {
                    प्रत्यागम असत्य;
                }
            } अन्यथा {
                unique_मूल्य=value;
                have_Unique_मूल्य=सत्य;
            }
            यदि(isFinal) {
                प्रत्यागम सत्य;
            }
            pos=skip_मूल्य(pos, गाँठ);
        }
    }
}

int32_t
BytesTrie::getNextBytes(ByteSink &out) ध्रुव {
    ध्रुव uint8_t *pos=pos_;
    यदि(pos==nullptr) {
        प्रत्यागम 0;
    }
    यदि(remainingMatchLength_>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        प्रत्यागम 1;
    }
    int32_t गाँठ=*pos++;
    यदि(गाँठ>=k_Min_मूल्य_Lead) {
        यदि(गाँठ&k_मूल्य_Is_Final) {
            प्रत्यागम 0;
        } अन्यथा {
            pos=skip_मूल्य(pos, गाँठ);
            गाँठ=*pos++;
            U_ASSERT(गाँठ<k_Min_मूल्य_Lead);
        }
    }
    यदि(गाँठ<kMinLinearMatch) {
        यदि(गाँठ==0) {
            गाँठ=*pos++;
        }
        getNextBranchBytes(pos, ++गाँठ, out);
        प्रत्यागम गाँठ;
    } अन्यथा {
        // First byte of the linear-match node.
        append(out, *pos);
        प्रत्यागम 1;
    }
}

शून्य
BytesTrie::getNextBranchBytes(ध्रुव uint8_t *pos, int32_t length, ByteSink &out) {
    यावत्(length>k_Max_Branch_Linear_Sub_गाँठ_Length) {
        ++pos;  // ignore the comparison byte
        getNextBranchBytes(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    कुरु {
        append(out, *pos++);
        pos=skip_मूल्य(pos);
    } यावत्(--length>1);
    append(out, *pos);
}

शून्य
BytesTrie::append(ByteSink &out, पूर्णाङ्क c) {
    वर्ण ch = static_cast<वर्ण>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_END
