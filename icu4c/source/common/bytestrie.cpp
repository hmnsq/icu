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
BytesTrie::read_मानम्(ध्रुव uint8_t *pos, int32_t leadByte) {
    int32_t value;
    जइ(leadByte<k_Min_Two_Byte_मानम्_Lead) {
        value=leadByte-k_Min_One_Byte_मानम्_Lead;
    } अण्णहा जइ(leadByte<k_Min_Three_Byte_मानम्_Lead) {
        value=((leadByte-k_Min_Two_Byte_मानम्_Lead)<<8)|*pos;
    } अण्णहा जइ(leadByte<k_Four_Byte_मानम्_Lead) {
        value=((leadByte-k_Min_Three_Byte_मानम्_Lead)<<16)|(pos[0]<<8)|pos[1];
    } अण्णहा जइ(leadByte==k_Four_Byte_मानम्_Lead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } अण्णहा {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    प्रत्यागम value;
}

ध्रुव uint8_t*
BytesTrie::jumpByDelta(ध्रुव uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    जइ(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } अण्णहा जइ(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } अण्णहा जइ(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } अण्णहा जइ(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } अण्णहा {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    प्रत्यागम pos+delta;
}

UStringTrieResult
BytesTrie::current() ध्रुव {
    ध्रुव uint8_t *pos=pos_;
    जइ(pos==nullptr) {
        प्रत्यागम USTRINGTRIE_NO_MATCH;
    } अण्णहा {
        int32_t ग्रन्थिः;
        प्रत्यागम (remainingMatchLength_<0 && (ग्रन्थिः=*pos)>=k_Min_मानम्_Lead) ?
                मानम्_Result(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
    }
}

UStringTrieResult
BytesTrie::branchNext(ध्रुव uint8_t *pos, int32_t length, int32_t inByte) {
    // Branch according to the current byte.
    जइ(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    जाव(length>k_Max_Branch_Linear_Sub_ग्रन्थिः_Length) {
        जइ(inByte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } अण्णहा {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few bytes.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    करेहि {
        जइ(inByte==*pos++) {
            UStringTrieResult result;
            int32_t ग्रन्थिः=*pos;
            U_ASSERT(ग्रन्थिः>=k_Min_मानम्_Lead);
            जइ(ग्रन्थिः&k_मानम्_Is_Final) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_मानम्;
            } अण्णहा {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                ग्रन्थिः>>=1;
                int32_t delta;
                जइ(ग्रन्थिः<k_Min_Two_Byte_मानम्_Lead) {
                    delta=ग्रन्थिः-k_Min_One_Byte_मानम्_Lead;
                } अण्णहा जइ(ग्रन्थिः<k_Min_Three_Byte_मानम्_Lead) {
                    delta=((ग्रन्थिः-k_Min_Two_Byte_मानम्_Lead)<<8)|*pos++;
                } अण्णहा जइ(ग्रन्थिः<k_Four_Byte_मानम्_Lead) {
                    delta=((ग्रन्थिः-k_Min_Three_Byte_मानम्_Lead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } अण्णहा जइ(ग्रन्थिः==k_Four_Byte_मानम्_Lead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } अण्णहा {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                ग्रन्थिः=*pos;
                result= ग्रन्थिः>=k_Min_मानम्_Lead ? मानम्_Result(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
            }
            pos_=pos;
            प्रत्यागम result;
        }
        --length;
        pos=skip_मानम्(pos);
    } जाव(length>1);
    जइ(inByte==*pos++) {
        pos_=pos;
        int32_t ग्रन्थिः=*pos;
        प्रत्यागम ग्रन्थिः>=k_Min_मानम्_Lead ? मानम्_Result(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
    } अण्णहा {
        stop();
        प्रत्यागम USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
BytesTrie::nextImpl(ध्रुव uint8_t *pos, int32_t inByte) {
    प्रति(;;) {
        int32_t ग्रन्थिः=*pos++;
        जइ(ग्रन्थिः<kMinLinearMatch) {
            प्रत्यागम branchNext(pos, ग्रन्थिः, inByte);
        } अण्णहा जइ(ग्रन्थिः<k_Min_मानम्_Lead) {
            // Match the first of length+1 bytes.
            int32_t length=ग्रन्थिः-kMinLinearMatch;  // Actual match length minus 1.
            जइ(inByte==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                प्रत्यागम (length<0 && (ग्रन्थिः=*pos)>=k_Min_मानम्_Lead) ?
                        मानम्_Result(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
            } अण्णहा {
                // No match.
                भङ्ग;
            }
        } अण्णहा जइ(ग्रन्थिः&k_मानम्_Is_Final) {
            // No further matching bytes.
            भङ्ग;
        } अण्णहा {
            // Skip intermediate value.
            pos=skip_मानम्(pos, ग्रन्थिः);
            // The next node must not also be a value node.
            U_ASSERT(*pos<k_Min_मानम्_Lead);
        }
    }
    stop();
    प्रत्यागम USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
BytesTrie::next(int32_t inByte) {
    ध्रुव uint8_t *pos=pos_;
    जइ(pos==nullptr) {
        प्रत्यागम USTRINGTRIE_NO_MATCH;
    }
    जइ(inByte<0) {
        inByte+=0x100;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    जइ(length>=0) {
        // Remaining part of a linear-match node.
        जइ(inByte==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t ग्रन्थिः;
            प्रत्यागम (length<0 && (ग्रन्थिः=*pos)>=k_Min_मानम्_Lead) ?
                    मानम्_Result(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
        } अण्णहा {
            stop();
            प्रत्यागम USTRINGTRIE_NO_MATCH;
        }
    }
    प्रत्यागम nextImpl(pos, inByte);
}

UStringTrieResult
BytesTrie::next(ध्रुव वर्ण *s, int32_t sLength) {
    जइ(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        प्रत्यागम current();
    }
    ध्रुव uint8_t *pos=pos_;
    जइ(pos==nullptr) {
        प्रत्यागम USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    प्रति(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t inByte;
        जइ(sLength<0) {
            प्रति(;;) {
                जइ((inByte=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t ग्रन्थिः;
                    प्रत्यागम (length<0 && (ग्रन्थिः=*pos)>=k_Min_मानम्_Lead) ?
                            मानम्_Result(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
                }
                जइ(length<0) {
                    remainingMatchLength_=length;
                    भङ्ग;
                }
                जइ(inByte!=*pos) {
                    stop();
                    प्रत्यागम USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } अण्णहा {
            प्रति(;;) {
                जइ(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t ग्रन्थिः;
                    प्रत्यागम (length<0 && (ग्रन्थिः=*pos)>=k_Min_मानम्_Lead) ?
                            मानम्_Result(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
                }
                inByte=*s++;
                --sLength;
                जइ(length<0) {
                    remainingMatchLength_=length;
                    भङ्ग;
                }
                जइ(inByte!=*pos) {
                    stop();
                    प्रत्यागम USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        प्रति(;;) {
            int32_t ग्रन्थिः=*pos++;
            जइ(ग्रन्थिः<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, ग्रन्थिः, inByte);
                जइ(result==USTRINGTRIE_NO_MATCH) {
                    प्रत्यागम USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input byte, if there is one.
                जइ(sLength<0) {
                    जइ((inByte=*s++)==0) {
                        प्रत्यागम result;
                    }
                } अण्णहा {
                    जइ(sLength==0) {
                        प्रत्यागम result;
                    }
                    inByte=*s++;
                    --sLength;
                }
                जइ(result==USTRINGTRIE_FINAL_मानम्) {
                    // No further matching bytes.
                    stop();
                    प्रत्यागम USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } अण्णहा जइ(ग्रन्थिः<k_Min_मानम्_Lead) {
                // Match length+1 bytes.
                length=ग्रन्थिः-kMinLinearMatch;  // Actual match length minus 1.
                जइ(inByte!=*pos) {
                    stop();
                    प्रत्यागम USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                भङ्ग;
            } अण्णहा जइ(ग्रन्थिः&k_मानम्_Is_Final) {
                // No further matching bytes.
                stop();
                प्रत्यागम USTRINGTRIE_NO_MATCH;
            } अण्णहा {
                // Skip intermediate value.
                pos=skip_मानम्(pos, ग्रन्थिः);
                // The next node must not also be a value node.
                U_ASSERT(*pos<k_Min_मानम्_Lead);
            }
        }
    }
}

ध्रुव uint8_t*
BytesTrie::find_Unique_मानम्_From_Branch(ध्रुव uint8_t* pos U_LIFETIME_BOUND,
                                     int32_t length,
                                     UBool have_Unique_मानम्,
                                     int32_t& unique_मानम्) {
    जाव(length>k_Max_Branch_Linear_Sub_ग्रन्थिः_Length) {
        ++pos;  // ignore the comparison byte
        जइ(nullptr==find_Unique_मानम्_From_Branch(jumpByDelta(pos), length>>1, have_Unique_मानम्, unique_मानम्)) {
            प्रत्यागम nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    करेहि {
        ++pos;  // ignore a comparison byte
        // handle its value
        int32_t ग्रन्थिः=*pos++;
        UBool isFinal = static_cast<UBool>(ग्रन्थिः & k_मानम्_Is_Final);
        int32_t value=read_मानम्(pos, ग्रन्थिः>>1);
        pos=skip_मानम्(pos, ग्रन्थिः);
        जइ(isFinal) {
            जइ(have_Unique_मानम्) {
                जइ(value!=unique_मानम्) {
                    प्रत्यागम nullptr;
                }
            } अण्णहा {
                unique_मानम्=value;
                have_Unique_मानम्=सच्च;
            }
        } अण्णहा {
            जइ(!find_Unique_मानम्(pos+value, have_Unique_मानम्, unique_मानम्)) {
                प्रत्यागम nullptr;
            }
            have_Unique_मानम्=सच्च;
        }
    } जाव(--length>1);
    प्रत्यागम pos+1;  // ignore the last comparison byte
}

UBool
BytesTrie::find_Unique_मानम्(ध्रुव uint8_t *pos, UBool have_Unique_मानम्, int32_t &unique_मानम्) {
    प्रति(;;) {
        int32_t ग्रन्थिः=*pos++;
        जइ(ग्रन्थिः<kMinLinearMatch) {
            जइ(ग्रन्थिः==0) {
                ग्रन्थिः=*pos++;
            }
            pos=find_Unique_मानम्_From_Branch(pos, ग्रन्थिः+1, have_Unique_मानम्, unique_मानम्);
            जइ(pos==nullptr) {
                प्रत्यागम मिच्छ;
            }
            have_Unique_मानम्=सच्च;
        } अण्णहा जइ(ग्रन्थिः<k_Min_मानम्_Lead) {
            // linear-match node
            pos+=ग्रन्थिः-kMinLinearMatch+1;  // Ignore the match bytes.
        } अण्णहा {
            UBool isFinal = static_cast<UBool>(ग्रन्थिः & k_मानम्_Is_Final);
            int32_t value=read_मानम्(pos, ग्रन्थिः>>1);
            जइ(have_Unique_मानम्) {
                जइ(value!=unique_मानम्) {
                    प्रत्यागम मिच्छ;
                }
            } अण्णहा {
                unique_मानम्=value;
                have_Unique_मानम्=सच्च;
            }
            जइ(isFinal) {
                प्रत्यागम सच्च;
            }
            pos=skip_मानम्(pos, ग्रन्थिः);
        }
    }
}

int32_t
BytesTrie::getNextBytes(ByteSink &out) ध्रुव {
    ध्रुव uint8_t *pos=pos_;
    जइ(pos==nullptr) {
        प्रत्यागम 0;
    }
    जइ(remainingMatchLength_>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        प्रत्यागम 1;
    }
    int32_t ग्रन्थिः=*pos++;
    जइ(ग्रन्थिः>=k_Min_मानम्_Lead) {
        जइ(ग्रन्थिः&k_मानम्_Is_Final) {
            प्रत्यागम 0;
        } अण्णहा {
            pos=skip_मानम्(pos, ग्रन्थिः);
            ग्रन्थिः=*pos++;
            U_ASSERT(ग्रन्थिः<k_Min_मानम्_Lead);
        }
    }
    जइ(ग्रन्थिः<kMinLinearMatch) {
        जइ(ग्रन्थिः==0) {
            ग्रन्थिः=*pos++;
        }
        getNextBranchBytes(pos, ++ग्रन्थिः, out);
        प्रत्यागम ग्रन्थिः;
    } अण्णहा {
        // First byte of the linear-match node.
        append(out, *pos);
        प्रत्यागम 1;
    }
}

शून्य
BytesTrie::getNextBranchBytes(ध्रुव uint8_t *pos, int32_t length, ByteSink &out) {
    जाव(length>k_Max_Branch_Linear_Sub_ग्रन्थिः_Length) {
        ++pos;  // ignore the comparison byte
        getNextBranchBytes(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    करेहि {
        append(out, *pos++);
        pos=skip_मानम्(pos);
    } जाव(--length>1);
    append(out, *pos);
}

शून्य
BytesTrie::append(ByteSink &out, पूर्णाङ्क c) {
    वर्ण ch = static_cast<वर्ण>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_END
