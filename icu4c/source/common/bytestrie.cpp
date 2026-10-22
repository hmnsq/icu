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
BytesTrie::read_मूल्य(स्थिरांक uint8_t *pos, int32_t leadByte) {
    int32_t value;
    जर(leadByte<k_Min_Two_Byte_मूल्य_Lead) {
        value=leadByte-k_Min_One_Byte_मूल्य_Lead;
    } अन्यथा जर(leadByte<k_Min_Three_Byte_मूल्य_Lead) {
        value=((leadByte-k_Min_Two_Byte_मूल्य_Lead)<<8)|*pos;
    } अन्यथा जर(leadByte<k_Four_Byte_मूल्य_Lead) {
        value=((leadByte-k_Min_Three_Byte_मूल्य_Lead)<<16)|(pos[0]<<8)|pos[1];
    } अन्यथा जर(leadByte==k_Four_Byte_मूल्य_Lead) {
        value=(pos[0]<<16)|(pos[1]<<8)|pos[2];
    } अन्यथा {
        value=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
    }
    परत value;
}

स्थिरांक uint8_t*
BytesTrie::jumpByDelta(स्थिरांक uint8_t* pos U_LIFETIME_BOUND) {
    int32_t delta=*pos++;
    जर(delta<kMinTwoByteDeltaLead) {
        // nothing to do
    } अन्यथा जर(delta<kMinThreeByteDeltaLead) {
        delta=((delta-kMinTwoByteDeltaLead)<<8)|*pos++;
    } अन्यथा जर(delta<kFourByteDeltaLead) {
        delta=((delta-kMinThreeByteDeltaLead)<<16)|(pos[0]<<8)|pos[1];
        pos+=2;
    } अन्यथा जर(delta==kFourByteDeltaLead) {
        delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
        pos+=3;
    } अन्यथा {
        delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
        pos+=4;
    }
    परत pos+delta;
}

UStringTrieResult
BytesTrie::current() स्थिरांक {
    स्थिरांक uint8_t *pos=pos_;
    जर(pos==nullptr) {
        परत USTRINGTRIE_NO_MATCH;
    } अन्यथा {
        int32_t गाठ;
        परत (remainingMatchLength_<0 && (गाठ=*pos)>=k_Min_मूल्य_Lead) ?
                मूल्य_Result(गाठ) : USTRINGTRIE_NO_मूल्य;
    }
}

UStringTrieResult
BytesTrie::branchNext(स्थिरांक uint8_t *pos, int32_t length, int32_t inByte) {
    // Branch according to the current byte.
    जर(length==0) {
        length=*pos++;
    }
    ++length;
    // The length of the branch is the number of bytes to select from.
    // The data structure encodes a binary search.
    पर्यंत(length>k_Max_Branch_Linear_Sub_गाठ_Length) {
        जर(inByte<*pos++) {
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
    करा {
        जर(inByte==*pos++) {
            UStringTrieResult result;
            int32_t गाठ=*pos;
            U_ASSERT(गाठ>=k_Min_मूल्य_Lead);
            जर(गाठ&k_मूल्य_Is_Final) {
                // Leave the final value for getValue() to read.
                result=USTRINGTRIE_FINAL_मूल्य;
            } अन्यथा {
                // Use the non-final value as the jump delta.
                ++pos;
                // int32_t delta=readValue(pos, node>>1);
                गाठ>>=1;
                int32_t delta;
                जर(गाठ<k_Min_Two_Byte_मूल्य_Lead) {
                    delta=गाठ-k_Min_One_Byte_मूल्य_Lead;
                } अन्यथा जर(गाठ<k_Min_Three_Byte_मूल्य_Lead) {
                    delta=((गाठ-k_Min_Two_Byte_मूल्य_Lead)<<8)|*pos++;
                } अन्यथा जर(गाठ<k_Four_Byte_मूल्य_Lead) {
                    delta=((गाठ-k_Min_Three_Byte_मूल्य_Lead)<<16)|(pos[0]<<8)|pos[1];
                    pos+=2;
                } अन्यथा जर(गाठ==k_Four_Byte_मूल्य_Lead) {
                    delta=(pos[0]<<16)|(pos[1]<<8)|pos[2];
                    pos+=3;
                } अन्यथा {
                    delta=(pos[0]<<24)|(pos[1]<<16)|(pos[2]<<8)|pos[3];
                    pos+=4;
                }
                // end readValue()
                pos+=delta;
                गाठ=*pos;
                result= गाठ>=k_Min_मूल्य_Lead ? मूल्य_Result(गाठ) : USTRINGTRIE_NO_मूल्य;
            }
            pos_=pos;
            परत result;
        }
        --length;
        pos=skip_मूल्य(pos);
    } पर्यंत(length>1);
    जर(inByte==*pos++) {
        pos_=pos;
        int32_t गाठ=*pos;
        परत गाठ>=k_Min_मूल्य_Lead ? मूल्य_Result(गाठ) : USTRINGTRIE_NO_मूल्य;
    } अन्यथा {
        stop();
        परत USTRINGTRIE_NO_MATCH;
    }
}

UStringTrieResult
BytesTrie::nextImpl(स्थिरांक uint8_t *pos, int32_t inByte) {
    साठी(;;) {
        int32_t गाठ=*pos++;
        जर(गाठ<kMinLinearMatch) {
            परत branchNext(pos, गाठ, inByte);
        } अन्यथा जर(गाठ<k_Min_मूल्य_Lead) {
            // Match the first of length+1 bytes.
            int32_t length=गाठ-kMinLinearMatch;  // Actual match length minus 1.
            जर(inByte==*pos++) {
                remainingMatchLength_=--length;
                pos_=pos;
                परत (length<0 && (गाठ=*pos)>=k_Min_मूल्य_Lead) ?
                        मूल्य_Result(गाठ) : USTRINGTRIE_NO_मूल्य;
            } अन्यथा {
                // No match.
                थांबा;
            }
        } अन्यथा जर(गाठ&k_मूल्य_Is_Final) {
            // No further matching bytes.
            थांबा;
        } अन्यथा {
            // Skip intermediate value.
            pos=skip_मूल्य(pos, गाठ);
            // The next node must not also be a value node.
            U_ASSERT(*pos<k_Min_मूल्य_Lead);
        }
    }
    stop();
    परत USTRINGTRIE_NO_MATCH;
}

UStringTrieResult
BytesTrie::next(int32_t inByte) {
    स्थिरांक uint8_t *pos=pos_;
    जर(pos==nullptr) {
        परत USTRINGTRIE_NO_MATCH;
    }
    जर(inByte<0) {
        inByte+=0x100;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    जर(length>=0) {
        // Remaining part of a linear-match node.
        जर(inByte==*pos++) {
            remainingMatchLength_=--length;
            pos_=pos;
            int32_t गाठ;
            परत (length<0 && (गाठ=*pos)>=k_Min_मूल्य_Lead) ?
                    मूल्य_Result(गाठ) : USTRINGTRIE_NO_मूल्य;
        } अन्यथा {
            stop();
            परत USTRINGTRIE_NO_MATCH;
        }
    }
    परत nextImpl(pos, inByte);
}

UStringTrieResult
BytesTrie::next(स्थिरांक अक्षर *s, int32_t sLength) {
    जर(sLength<0 ? *s==0 : sLength==0) {
        // Empty input.
        परत current();
    }
    स्थिरांक uint8_t *pos=pos_;
    जर(pos==nullptr) {
        परत USTRINGTRIE_NO_MATCH;
    }
    int32_t length=remainingMatchLength_;  // Actual remaining match length minus 1.
    साठी(;;) {
        // Fetch the next input byte, if there is one.
        // Continue a linear-match node without rechecking sLength<0.
        int32_t inByte;
        जर(sLength<0) {
            साठी(;;) {
                जर((inByte=*s++)==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t गाठ;
                    परत (length<0 && (गाठ=*pos)>=k_Min_मूल्य_Lead) ?
                            मूल्य_Result(गाठ) : USTRINGTRIE_NO_मूल्य;
                }
                जर(length<0) {
                    remainingMatchLength_=length;
                    थांबा;
                }
                जर(inByte!=*pos) {
                    stop();
                    परत USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        } अन्यथा {
            साठी(;;) {
                जर(sLength==0) {
                    remainingMatchLength_=length;
                    pos_=pos;
                    int32_t गाठ;
                    परत (length<0 && (गाठ=*pos)>=k_Min_मूल्य_Lead) ?
                            मूल्य_Result(गाठ) : USTRINGTRIE_NO_मूल्य;
                }
                inByte=*s++;
                --sLength;
                जर(length<0) {
                    remainingMatchLength_=length;
                    थांबा;
                }
                जर(inByte!=*pos) {
                    stop();
                    परत USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
            }
        }
        साठी(;;) {
            int32_t गाठ=*pos++;
            जर(गाठ<kMinLinearMatch) {
                UStringTrieResult result=branchNext(pos, गाठ, inByte);
                जर(result==USTRINGTRIE_NO_MATCH) {
                    परत USTRINGTRIE_NO_MATCH;
                }
                // Fetch the next input byte, if there is one.
                जर(sLength<0) {
                    जर((inByte=*s++)==0) {
                        परत result;
                    }
                } अन्यथा {
                    जर(sLength==0) {
                        परत result;
                    }
                    inByte=*s++;
                    --sLength;
                }
                जर(result==USTRINGTRIE_FINAL_मूल्य) {
                    // No further matching bytes.
                    stop();
                    परत USTRINGTRIE_NO_MATCH;
                }
                pos=pos_;  // branchNext() advanced pos and wrote it to pos_ .
            } अन्यथा जर(गाठ<k_Min_मूल्य_Lead) {
                // Match length+1 bytes.
                length=गाठ-kMinLinearMatch;  // Actual match length minus 1.
                जर(inByte!=*pos) {
                    stop();
                    परत USTRINGTRIE_NO_MATCH;
                }
                ++pos;
                --length;
                थांबा;
            } अन्यथा जर(गाठ&k_मूल्य_Is_Final) {
                // No further matching bytes.
                stop();
                परत USTRINGTRIE_NO_MATCH;
            } अन्यथा {
                // Skip intermediate value.
                pos=skip_मूल्य(pos, गाठ);
                // The next node must not also be a value node.
                U_ASSERT(*pos<k_Min_मूल्य_Lead);
            }
        }
    }
}

स्थिरांक uint8_t*
BytesTrie::find_Unique_मूल्य_From_Branch(स्थिरांक uint8_t* pos U_LIFETIME_BOUND,
                                     int32_t length,
                                     UBool have_Unique_मूल्य,
                                     int32_t& unique_मूल्य) {
    पर्यंत(length>k_Max_Branch_Linear_Sub_गाठ_Length) {
        ++pos;  // ignore the comparison byte
        जर(nullptr==find_Unique_मूल्य_From_Branch(jumpByDelta(pos), length>>1, have_Unique_मूल्य, unique_मूल्य)) {
            परत nullptr;
        }
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    करा {
        ++pos;  // ignore a comparison byte
        // handle its value
        int32_t गाठ=*pos++;
        UBool isFinal = static_cast<UBool>(गाठ & k_मूल्य_Is_Final);
        int32_t value=read_मूल्य(pos, गाठ>>1);
        pos=skip_मूल्य(pos, गाठ);
        जर(isFinal) {
            जर(have_Unique_मूल्य) {
                जर(value!=unique_मूल्य) {
                    परत nullptr;
                }
            } अन्यथा {
                unique_मूल्य=value;
                have_Unique_मूल्य=सत्य;
            }
        } अन्यथा {
            जर(!find_Unique_मूल्य(pos+value, have_Unique_मूल्य, unique_मूल्य)) {
                परत nullptr;
            }
            have_Unique_मूल्य=सत्य;
        }
    } पर्यंत(--length>1);
    परत pos+1;  // ignore the last comparison byte
}

UBool
BytesTrie::find_Unique_मूल्य(स्थिरांक uint8_t *pos, UBool have_Unique_मूल्य, int32_t &unique_मूल्य) {
    साठी(;;) {
        int32_t गाठ=*pos++;
        जर(गाठ<kMinLinearMatch) {
            जर(गाठ==0) {
                गाठ=*pos++;
            }
            pos=find_Unique_मूल्य_From_Branch(pos, गाठ+1, have_Unique_मूल्य, unique_मूल्य);
            जर(pos==nullptr) {
                परत असत्य;
            }
            have_Unique_मूल्य=सत्य;
        } अन्यथा जर(गाठ<k_Min_मूल्य_Lead) {
            // linear-match node
            pos+=गाठ-kMinLinearMatch+1;  // Ignore the match bytes.
        } अन्यथा {
            UBool isFinal = static_cast<UBool>(गाठ & k_मूल्य_Is_Final);
            int32_t value=read_मूल्य(pos, गाठ>>1);
            जर(have_Unique_मूल्य) {
                जर(value!=unique_मूल्य) {
                    परत असत्य;
                }
            } अन्यथा {
                unique_मूल्य=value;
                have_Unique_मूल्य=सत्य;
            }
            जर(isFinal) {
                परत सत्य;
            }
            pos=skip_मूल्य(pos, गाठ);
        }
    }
}

int32_t
BytesTrie::getNextBytes(ByteSink &out) स्थिरांक {
    स्थिरांक uint8_t *pos=pos_;
    जर(pos==nullptr) {
        परत 0;
    }
    जर(remainingMatchLength_>=0) {
        append(out, *pos);  // Next byte of a pending linear-match node.
        परत 1;
    }
    int32_t गाठ=*pos++;
    जर(गाठ>=k_Min_मूल्य_Lead) {
        जर(गाठ&k_मूल्य_Is_Final) {
            परत 0;
        } अन्यथा {
            pos=skip_मूल्य(pos, गाठ);
            गाठ=*pos++;
            U_ASSERT(गाठ<k_Min_मूल्य_Lead);
        }
    }
    जर(गाठ<kMinLinearMatch) {
        जर(गाठ==0) {
            गाठ=*pos++;
        }
        getNextBranchBytes(pos, ++गाठ, out);
        परत गाठ;
    } अन्यथा {
        // First byte of the linear-match node.
        append(out, *pos);
        परत 1;
    }
}

शून्य
BytesTrie::getNextBranchBytes(स्थिरांक uint8_t *pos, int32_t length, ByteSink &out) {
    पर्यंत(length>k_Max_Branch_Linear_Sub_गाठ_Length) {
        ++pos;  // ignore the comparison byte
        getNextBranchBytes(jumpByDelta(pos), length>>1, out);
        length=length-(length>>1);
        pos=skipDelta(pos);
    }
    करा {
        append(out, *pos++);
        pos=skip_मूल्य(pos);
    } पर्यंत(--length>1);
    append(out, *pos);
}

शून्य
BytesTrie::append(ByteSink &out, पूर्णांक c) {
    अक्षर ch = static_cast<अक्षर>(c);
    out.Append(&ch, 1);
}

U_NAMESPACE_END
