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
    सचे(leadByte<k_Min_Two_Byte_मानम्_Lead) {
        value=leadByte-k_Min_One_Byte_मानम्_Lead;
    } अञ्ञथा सचे(leadByte<k_Min_Three_Byte_मानम्_Lead) {
        value=((leadByte-k_Min_Two_Byte_मानम्_Lead)<<8)|*pos;
    } अञ्ञथा सचे(leadByte<k_Four_Byte_मानम्_Lead) {
        value=((leadByte-k_Min_Three_Byte_मानम्_Lead)<<16)|(pos[0]<<8)|pos[1];
    } अञ्ञथा सचे(leadByte==k_Four_Byte_मानम्_Lead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } अञ्ञथा {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    पटिनिवत्त value;
}

ध्रुव uint8_t*
BytesTrie::jumpByDelta(ध्रुव uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    सचे(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } अञ्ञथा सचे(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } अञ्ञथा सचे(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } अञ्ञथा सचे(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } अञ्ञथा {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    पटिनिवत्त pos+delta;
}

UStringTrieResult
BytesTrie::current() ध्रुव {
    ध्रुव uint8_t *pos=pos_;
    सचे(pos==nullptr) {
        पटिनिवत्त USTRINGTRIE_NO_MATCH;
    } अञ्ञथा {
        int32_t ग्रन्थिः;
        पटिनिवत्त (remainingMatchLength_<0 && (ग्रन्थिः=*pos)>=k_Min_मानम्_Lead) ?
                मानम्_Result(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
    }
}

UStringTrieResult
BytesTrie::branchNext(ध्रुव uint8_t *pos, int32_t length, int32_t inByte) {
    // Branch according to the current byte.
    सचे(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    याव(length>k_Max_Branch_Linear_Sub_ग्रन्थिः_Length) {
        सचे(inByte<*pos++) {
            length>>=1;
            pos=jumpByDelta(pos);
        } अञ्ञथा {
            length=length-(length>>1);
            pos=skipDelta(pos);
        }
    }
    // Drop down to linear search for the last few bytes.
    // length>=2 because the loop body above sees length>kMaxBranchLinearSubNodeLength>=3
    // and divides length by 2.
    करोहि {
        सचे(inByte==*pos++) {
            UStringTrieResult result;
            int32_t ग्रन्थिः=*pos;
            U_ASSERT(ग्रन्थिः>=k_Min_मानम्_Lead);
            सचे(ग्रन्थिः&k_मानम्_Is_Final) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_मानम्;
            } अञ्ञथा {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                ग्रन्थिः>>=1;
                int32_t delta;
                सचे(ग्रन्थिः<k_Min_Two_Byte_मानम्_Lead) {
                    delta=ग्रन्थिः-k_Min_One_Byte_मानम्_Lead;
                } अञ्ञथा सचे(ग्रन्थिः<k_Min_Three_Byte_मानम्_Lead) {
                    delta=((ग्रन्थिः-k_Min_Two_Byte_मानम्_Lead)<<8)|*pos++;
                } अञ्ञथा सचे(ग्रन्थिः<k_Four_Byte_मानम्_Lead) {
                    delta=((ग्रन्थिः-k_Min_Three_Byte_मानम्_Lead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } अञ्ञथा सचे(ग्रन्थिः==k_Four_Byte_मानम्_Lead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } अञ्ञथा {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                ग्रन्थिः=*pos;
                result= ग्रन्थिः>=k_Min_मानम्_Lead ? मानम्_Result(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
            }
            pos_=pos;
            पटिनिवत्त result;
        }
        --length;
        pos=skip_मानम्(pos);
    } याव(length>1);
    सचे(inByte==*pos++) {
        pos_=pos;
        int32_t ग्रन्थिः=*pos;
        पटिनिवत्त ग्रन्थिः>=k_Min_मानम्_Lead ? मानम्_Result(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
    } अञ्ञथा {
        stop();
        पटिनिवत्त USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
BytesTrie::nextImpl(ध्रुव uint8_t *pos, int32_t inByte) {
    अत्थाय(;;) {
        int32_t ग्रन्थिः=*pos++;
        सचे(ग्रन्थिः<kMinLinearMatch) {
            पटिनिवत्त branchNext(pos, ग्रन्थिः, inByte);
        } अञ्ञथा सचे(ग्रन्थिः<k_Min_मानम्_Lead) {
            // Match the first of length+1 bytes.
            int32_t length=ग्रन्थिः-kMinLinearMatch;  // Actual match length minus 1.
            सचे(inByte==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                पटिनिवत्त (length<0 && (ग्रन्थिः=*pos)>=k_Min_मानम्_Lead) ?
                        मानम्_Result(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
            } अञ्ञथा {
                // No match.
                भङ्ग;
            }
        } अञ्ञथा सचे(ग्रन्थिः&k_मानम्_Is_Final) {
            // No further matching bytes.
            भङ्ग;
        } अञ्ञथा {
            // Skip intermediate value.
            pos=skip_मानम्(pos, ग्रन्थिः);
            // The next node must not also be a value node.
            U_ASSERT(*pos<k_Min_मानम्_Lead);
        }
    }
    stop();
    पटिनिवत्त USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
BytesTrie::next(int32_t inByte) {
    ध्रुव uint8_t *pos=pos_;
    सचे(pos==nullptr) {
        पटिनिवत्त USTRINGTRIE_NO_MATCH;
    }
    सचे(inByte<0) {
        inByte+=0x100;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    सचे(length>=0) {
        // Remaining part of a linear-match node.
        सचे(inByte==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t ग्रन्थिः;
            पटिनिवत्त (length<0 && (ग्रन्थिः=*pos)>=k_Min_मानम्_Lead) ?
                    मानम्_Result(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
        } अञ्ञथा {
            stop();
            पटिनिवत्त USTRINGTRIE_NO_MATCH;
        }
    }
    पटिनिवत्त nextImpl(pos, inByte);
}

UStringTrieResult
BytesTrie::next(ध्रुव वर्ण *s, int32_t sLength) {
    सचे(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        पटिनिवत्त current();
    }
    ध्रुव uint8_t *pos=pos_;
    सचे(pos==nullptr) {
        पटिनिवत्त USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    अत्थाय(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t inByte;
        सचे(sLength<0) {
            अत्थाय(;;) {
                सचे((inByte=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t ग्रन्थिः;
                    पटिनिवत्त (length<0 && (ग्रन्थिः=*pos)>=k_Min_मानम्_Lead) ?
                            मानम्_Result(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
                }
                सचे(length<0) {
                    remainingMatchLength_=length;
                    भङ्ग;
                }
                सचे(inByte!=*pos) {
                    stop();
                    पटिनिवत्त USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } अञ्ञथा {
            अत्थाय(;;) {
                सचे(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t ग्रन्थिः;
                    पटिनिवत्त (length<0 && (ग्रन्थिः=*pos)>=k_Min_मानम्_Lead) ?
                            मानम्_Result(ग्रन्थिः) : USTRINGTRIE_NO_मानम्;
                }
                inByte=*s++;
                --sLength;
                सचे(length<0) {
                    remainingMatchLength_=length;
                    भङ्ग;
                }
                सचे(inByte!=*pos) {
                    stop();
                    पटिनिवत्त USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        अत्थाय(;;) {
            int32_t ग्रन्थिः=*pos++;
            सचे(ग्रन्थिः<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, ग्रन्थिः, inByte);
                सचे(result==USTRINGTRIE_NO_MATCH) {
                    पटिनिवत्त USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input byte, if there is one.
                सचे(sLength<0) {
                    सचे((inByte=*s++)==0) {
                        पटिनिवत्त result;
                    }
                } अञ्ञथा {
                    सचे(sLength==0) {
                        पटिनिवत्त result;
                    }
                    inByte=*s++;
                    --sLength;
                }
                सचे(result==USTRINGTRIE_FINAL_मानम्) {
                    // No further matching bytes.
                    stop();
                    पटिनिवत्त USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } अञ्ञथा सचे(ग्रन्थिः<k_Min_मानम्_Lead) {
                // Match length+1 bytes.
                length=ग्रन्थिः-kMinLinearMatch;  // Actual match length minus 1.
                सचे(inByte!=*pos) {
                    stop();
                    पटिनिवत्त USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                भङ्ग;
            } अञ्ञथा सचे(ग्रन्थिः&k_मानम्_Is_Final) {
                // No further matching bytes.
                stop();
                पटिनिवत्त USTRINGTRIE_NO_MATCH;
            } अञ्ञथा {
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
    याव(length>k_Max_Branch_Linear_Sub_ग्रन्थिः_Length) {
        ++pos;  // ignore the comparison byte
        सचे(nullptr==find_Unique_मानम्_From_Branch(jumpByDelta(pos), length>>1, have_Unique_मानम्, unique_मानम्)) {
            पटिनिवत्त nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    करोहि {
        ++pos;  // ignore a comparison byte
        // handle its value
        int32_t ग्रन्थिः=*pos++;
        UBool isFinal = static_cast<UBool>(ग्रन्थिः & k_मानम्_Is_Final);
        int32_t value=read_मानम्(pos, ग्रन्थिः>>1);
        pos=skip_मानम्(pos, ग्रन्थिः);
        सचे(isFinal) {
            सचे(have_Unique_मानम्) {
                सचे(value!=unique_मानम्) {
                    पटिनिवत्त nullptr;
                }
            } अञ्ञथा {
                unique_मानम्=value;
                have_Unique_मानम्=सच्च;
            }
        } अञ्ञथा {
            सचे(!find_Unique_मानम्(pos+value, have_Unique_मानम्, unique_मानम्)) {
                पटिनिवत्त nullptr;
            }
            have_Unique_मानम्=सच्च;
        }
    } याव(--length>1);
    पटिनिवत्त pos+1;  // ignore the last comparison byte
}

UBool
BytesTrie::find_Unique_मानम्(ध्रुव uint8_t *pos, UBool have_Unique_मानम्, int32_t &unique_मानम्) {
    अत्थाय(;;) {
        int32_t ग्रन्थिः=*pos++;
        सचे(ग्रन्थिः<kMinLinearMatch) {
            सचे(ग्रन्थिः==0) {
                ग्रन्थिः=*pos++;
            }
            pos=find_Unique_मानम्_From_Branch(pos, ग्रन्थिः+1, have_Unique_मानम्, unique_मानम्);
            सचे(pos==nullptr) {
                पटिनिवत्त मिच्छा;
            }
            have_Unique_मानम्=सच्च;
        } अञ्ञथा सचे(ग्रन्थिः<k_Min_मानम्_Lead) {
            // linear-match node
            pos+=ग्रन्थिः-kMinLinearMatch+1;  // Ignore the match bytes.
        } अञ्ञथा {
            UBool isFinal = static_cast<UBool>(ग्रन्थिः & k_मानम्_Is_Final);
            int32_t value=read_मानम्(pos, ग्रन्थिः>>1);
            सचे(have_Unique_मानम्) {
                सचे(value!=unique_मानम्) {
                    पटिनिवत्त मिच्छा;
                }
            } अञ्ञथा {
                unique_मानम्=value;
                have_Unique_मानम्=सच्च;
            }
            सचे(isFinal) {
                पटिनिवत्त सच्च;
            }
            pos=skip_मानम्(pos, ग्रन्थिः);
        }
    }
}

int32_t
BytesTrie::getNextBytes(ByteSink &out) ध्रुव {
    ध्रुव uint8_t *pos=pos_;
    सचे(pos==nullptr) {
        पटिनिवत्त 0;
    }
    सचे(remainingMatchLength_>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        पटिनिवत्त 1;
    }
    int32_t ग्रन्थिः=*pos++;
    सचे(ग्रन्थिः>=k_Min_मानम्_Lead) {
        सचे(ग्रन्थिः&k_मानम्_Is_Final) {
            पटिनिवत्त 0;
        } अञ्ञथा {
            pos=skip_मानम्(pos, ग्रन्थिः);
            ग्रन्थिः=*pos++;
            U_ASSERT(ग्रन्थिः<k_Min_मानम्_Lead);
        }
    }
    सचे(ग्रन्थिः<kMinLinearMatch) {
        सचे(ग्रन्थिः==0) {
            ग्रन्थिः=*pos++;
        }
        getNextBranchBytes(pos, ++ग्रन्थिः, out);
        पटिनिवत्त ग्रन्थिः;
    } अञ्ञथा {
        // First byte of the linear-match node.
        append(out, *pos);
        पटिनिवत्त 1;
    }
}

शून्य
BytesTrie::getNextBranchBytes(ध्रुव uint8_t *pos, int32_t length, ByteSink &out) {
    याव(length>k_Max_Branch_Linear_Sub_ग्रन्थिः_Length) {
        ++pos;  // ignore the comparison byte
        getNextBranchBytes(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    करोहि {
        append(out, *pos++);
        pos=skip_मानम्(pos);
    } याव(--length>1);
    append(out, *pos);
}

शून्य
BytesTrie::append(ByteSink &out, पूर्णाङ्क c) {
    वर्ण ch = static_cast<वर्ण>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_END
