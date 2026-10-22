// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#అన్తర్భావ <complex>
#అన్తర్భావ <utility>

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION

#అన్తర్భావ "brkeng.h"
#అన్తర్భావ "charstr.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "lstmbe.h"
#అన్తర్భావ "putilimp.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "ubrkimpl.h"
#అన్తర్భావ "uresimp.h"
#అన్తర్భావ "uvectr32.h"
#అన్తర్భావ "uvector.h"

#అన్తర్భావ "unicode/brkiter.h"
#అన్తర్భావ "unicode/resbund.h"
#అన్తర్భావ "unicode/ubrk.h"
#అన్తర్భావ "unicode/uniset.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/utf.h"

U_NAMESPACE_BEGIN

// Uncomment the following #define to debug.
// #define LSTM_DEBUG 1
// #define LSTM_VECTORIZER_DEBUG 1

/**
 * Interface for reading 1D array.
 */
తరగతి ReadArray1D {
బహిరంగ:
    మాయా ~ReadArray1D();
    మాయా int32_t d1() స్థిరరాశి = 0;
    మాయా దశాంశం get(int32_t i) స్థిరరాశి = 0;

#నిర్వచితమైతే LSTM_DEBUG
    శూన్య print() స్థిరరాశి {
        రాయి("\n[");
        కొరకు (int32_t i = 0; i < d1(); i++) {
           రాయి("%0.8e ", get(i));
           ఒకవేళ (i % 4 == 3) రాయి("\n");
        }
        రాయి("]\n");
    }
#అయితే_ముగింపు
};

ReadArray1D::~ReadArray1D()
{
}

/**
 * Interface for reading 2D array.
 */
తరగతి ReadArray2D {
బహిరంగ:
    మాయా ~ReadArray2D();
    మాయా int32_t d1() స్థిరరాశి = 0;
    మాయా int32_t d2() స్థిరరాశి = 0;
    మాయా దశాంశం get(int32_t i, int32_t j) స్థిరరాశి = 0;
};

ReadArray2D::~ReadArray2D()
{
}

/**
 * A class to index a float array as a 1D Array without owning the pointer or
 * copy the data.
 */
తరగతి ConstArray1D : బహిరంగ ReadArray1D {
బహిరంగ:
    ConstArray1D() : దత్తాంశం_2(శూన్యసూచిక), d1_(0) {}

    ConstArray1D(స్థిరరాశి దశాంశం* దత్తాంశం, int32_t d1) : దత్తాంశం_2(దత్తాంశం), d1_(d1) {}

    మాయా ~ConstArray1D();

    // Init the object, the object does not own the data nor copy.
    // It is designed to directly use data from memory mapped resources.
    శూన్య init(స్థిరరాశి int32_t* దత్తాంశం, int32_t d1) {
        U_ASSERT(IEEE_754 == 1);
        దత్తాంశం_2 = reinterpret_cast<స్థిరరాశి దశాంశం*>(దత్తాంశం);
        d1_ = d1;
    }

    // ReadArray1D methods.
    మాయా int32_t d1() స్థిరరాశి override { తిరిగి d1_; }
    మాయా దశాంశం get(int32_t i) స్థిరరాశి override {
        U_ASSERT(i < d1_);
        తిరిగి దత్తాంశం_2[i];
    }

అంతరంగ:
    స్థిరరాశి దశాంశం* దత్తాంశం_2;
    int32_t d1_;
};

ConstArray1D::~ConstArray1D()
{
}

/**
 * A class to index a float array as a 2D Array without owning the pointer or
 * copy the data.
 */
తరగతి ConstArray2D : బహిరంగ ReadArray2D {
బహిరంగ:
    ConstArray2D() : దత్తాంశం_2(శూన్యసూచిక), d1_(0), d2_(0) {}

    ConstArray2D(స్థిరరాశి దశాంశం* దత్తాంశం, int32_t d1, int32_t d2)
        : దత్తాంశం_2(దత్తాంశం), d1_(d1), d2_(d2) {}

    మాయా ~ConstArray2D();

    // Init the object, the object does not own the data nor copy.
    // It is designed to directly use data from memory mapped resources.
    శూన్య init(స్థిరరాశి int32_t* దత్తాంశం, int32_t d1, int32_t d2) {
        U_ASSERT(IEEE_754 == 1);
        దత్తాంశం_2 = reinterpret_cast<స్థిరరాశి దశాంశం*>(దత్తాంశం);
        d1_ = d1;
        d2_ = d2;
    }

    // ReadArray2D methods.
    అంతఃవరుస int32_t d1() స్థిరరాశి override { తిరిగి d1_; }
    అంతఃవరుస int32_t d2() స్థిరరాశి override { తిరిగి d2_; }
    దశాంశం get(int32_t i, int32_t j) స్థిరరాశి override {
        U_ASSERT(i < d1_);
        U_ASSERT(j < d2_);
        తిరిగి దత్తాంశం_2[i * d2_ + j];
    }

    // Expose the ith row as a ConstArray1D
    అంతఃవరుస ConstArray1D అడ్డువరుస(int32_t i) స్థిరరాశి {
        U_ASSERT(i < d1_);
        తిరిగి ConstArray1D(దత్తాంశం_2 + i * d2_, d2_);
    }

అంతరంగ:
    స్థిరరాశి దశాంశం* దత్తాంశం_2;
    int32_t d1_;
    int32_t d2_;
};

ConstArray2D::~ConstArray2D()
{
}

/**
 * A class to allocate data as a writable 1D array.
 * This is the main class implement matrix operation.
 */
తరగతి Array1D : బహిరంగ ReadArray1D {
బహిరంగ:
    Array1D() : జ్ఞాపకం_2(శూన్యసూచిక), దత్తాంశం_2(శూన్యసూచిక), d1_(0) {}
    Array1D(int32_t d1, UError_సంకేతం &స్థితి_2)
        : జ్ఞాపకం_2(uprv_malloc(d1 * పరిమాణం(దశాంశం))),
          దత్తాంశం_2(static_cast<దశాంశం*>(జ్ఞాపకం_2)), d1_(d1) {
        ఒకవేళ (U_SUCCESS(స్థితి_2)) {
            ఒకవేళ (జ్ఞాపకం_2 == శూన్యసూచిక) {
                స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                తిరిగి;
            }
            clear();
        }
    }

    మాయా ~Array1D();

    // A special constructor which does not own the memory but writeable
    // as a slice of an array.
    Array1D(దశాంశం* దత్తాంశం, int32_t d1)
        : జ్ఞాపకం_2(శూన్యసూచిక), దత్తాంశం_2(దత్తాంశం), d1_(d1) {}

    // ReadArray1D methods.
    మాయా int32_t d1() స్థిరరాశి override { తిరిగి d1_; }
    మాయా దశాంశం get(int32_t i) స్థిరరాశి override {
        U_ASSERT(i < d1_);
        తిరిగి దత్తాంశం_2[i];
    }

    // Return the index which point to the max data in the array.
    అంతఃవరుస int32_t max_సూచీ() స్థిరరాశి {
        int32_t సూచీ = 0;
        దశాంశం max = దత్తాంశం_2[0];
        కొరకు (int32_t i = 1; i < d1_; i++) {
            ఒకవేళ (దత్తాంశం_2[i] > max) {
                max = దత్తాంశం_2[i];
                సూచీ = i;
            }
        }
        తిరిగి సూచీ;
    }

    // Slice part of the array to a new one.
    అంతఃవరుస Array1D ముక్క(int32_t from, int32_t size) స్థిరరాశి {
        U_ASSERT(from >= 0);
        U_ASSERT(from < d1_);
        U_ASSERT(from + size <= d1_);
        తిరిగి Array1D(దత్తాంశం_2 + from, size);
    }

    // Add dot product of a 1D array and a 2D array into this one.
    అంతఃవరుస Array1D& కూడు_Dot_లబ్ధం(స్థిరరాశి ReadArray1D& a, స్థిరరాశి ReadArray2D& b) {
        U_ASSERT(a.d1() == b.d1());
        U_ASSERT(b.d2() == d1());
        కొరకు (int32_t i = 0; i < d1(); i++) {
            కొరకు (int32_t j = 0; j < a.d1(); j++) {
                దత్తాంశం_2[i] += a.get(j) * b.get(j, i);
            }
        }
        తిరిగి *ఇది;
    }

    // Hadamard Product the values of another array of the same size into this one.
    అంతఃవరుస Array1D& hadamard_లబ్ధం(స్థిరరాశి ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        కొరకు (int32_t i = 0; i < d1(); i++) {
            దత్తాంశం_2[i] *= a.get(i);
        }
        తిరిగి *ఇది;
    }

    // Add the Hadamard Product of two arrays of the same size into this one.
    అంతఃవరుస Array1D& కూడు_Hadamard_లబ్ధం(స్థిరరాశి ReadArray1D& a, స్థిరరాశి ReadArray1D& b) {
        U_ASSERT(a.d1() == d1());
        U_ASSERT(b.d1() == d1());
        కొరకు (int32_t i = 0; i < d1(); i++) {
            దత్తాంశం_2[i] += a.get(i) * b.get(i);
        }
        తిరిగి *ఇది;
    }

    // Add the values of another array of the same size into this one.
    అంతఃవరుస Array1D& కూడు(స్థిరరాశి ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        కొరకు (int32_t i = 0; i < d1(); i++) {
            దత్తాంశం_2[i] += a.get(i);
        }
        తిరిగి *ఇది;
    }

    // Assign the values of another array of the same size into this one.
    అంతఃవరుస Array1D& assign(స్థిరరాశి ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        కొరకు (int32_t i = 0; i < d1(); i++) {
            దత్తాంశం_2[i] = a.get(i);
        }
        తిరిగి *ఇది;
    }

    // Apply tanh to all the elements in the array.
    అంతఃవరుస Array1D& tanh() {
        తిరిగి tanh(*ఇది);
    }

    // Apply tanh of a and store into this array.
    అంతఃవరుస Array1D& tanh(స్థిరరాశి Array1D& a) {
        U_ASSERT(a.d1() == d1());
        కొరకు (int32_t i = 0; i < d1_; i++) {
            దత్తాంశం_2[i] = std::tanh(a.get(i));
        }
        తిరిగి *ఇది;
    }

    // Apply sigmoid to all the elements in the array.
    అంతఃవరుస Array1D& sigmoid() {
        కొరకు (int32_t i = 0; i < d1_; i++) {
            దత్తాంశం_2[i] = 1.0f/(1.0f + expf(-దత్తాంశం_2[i]));
        }
        తిరిగి *ఇది;
    }

    అంతఃవరుస Array1D& clear() {
        uprv_memset(దత్తాంశం_2, 0, d1_ * పరిమాణం(దశాంశం));
        తిరిగి *ఇది;
    }

అంతరంగ:
    శూన్య* జ్ఞాపకం_2;
    దశాంశం* దత్తాంశం_2;
    int32_t d1_;
};

Array1D::~Array1D()
{
    uprv_free(జ్ఞాపకం_2);
}

తరగతి Array2D : బహిరంగ ReadArray2D {
బహిరంగ:
    Array2D() : జ్ఞాపకం_2(శూన్యసూచిక), దత్తాంశం_2(శూన్యసూచిక), d1_(0), d2_(0) {}
    Array2D(int32_t d1, int32_t d2, UError_సంకేతం &స్థితి_2)
        : జ్ఞాపకం_2(uprv_malloc(d1 * d2 * పరిమాణం(దశాంశం))),
          దత్తాంశం_2(static_cast<దశాంశం*>(జ్ఞాపకం_2)), d1_(d1), d2_(d2) {
        ఒకవేళ (U_SUCCESS(స్థితి_2)) {
            ఒకవేళ (జ్ఞాపకం_2 == శూన్యసూచిక) {
                స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                తిరిగి;
            }
            clear();
        }
    }
    మాయా ~Array2D();

    // ReadArray2D methods.
    మాయా int32_t d1() స్థిరరాశి override { తిరిగి d1_; }
    మాయా int32_t d2() స్థిరరాశి override { తిరిగి d2_; }
    మాయా దశాంశం get(int32_t i, int32_t j) స్థిరరాశి override {
        U_ASSERT(i < d1_);
        U_ASSERT(j < d2_);
        తిరిగి దత్తాంశం_2[i * d2_ + j];
    }

    అంతఃవరుస Array1D అడ్డువరుస(int32_t i) స్థిరరాశి {
        U_ASSERT(i < d1_);
        తిరిగి Array1D(దత్తాంశం_2 + i * d2_, d2_);
    }

    అంతఃవరుస Array2D& clear() {
        uprv_memset(దత్తాంశం_2, 0, d1_ * d2_ * పరిమాణం(దశాంశం));
        తిరిగి *ఇది;
    }

అంతరంగ:
    శూన్య* జ్ఞాపకం_2;
    దశాంశం* దత్తాంశం_2;
    int32_t d1_;
    int32_t d2_;
};

Array2D::~Array2D()
{
    uprv_free(జ్ఞాపకం_2);
}

రకనిర్వచనం గణన {
    BEGIN,
    INSIDE,
    చివర_2,
    SINGLE
} LSTMClass;

రకనిర్వచనం గణన {
    UNKNOWN,
    సంకేతం_POINTS,
    GRAPHEME_CLUSTER,
} EmbeddingType;

నిర్మాణం LSTMData : బహిరంగ UMemory {
    LSTMData(UResourceBundle* rb, UError_సంకేతం &స్థితి_2);
    ~LSTMData();
    UHashtable* fDict;
    EmbeddingType fType;
    స్థిరరాశి char16_t* fName;
    ConstArray2D fEmbedding;
    ConstArray2D fForwardW;
    ConstArray2D fForwardU;
    ConstArray1D fForwardB;
    ConstArray2D fBackwardW;
    ConstArray2D fBackwardU;
    ConstArray1D fBackwardB;
    ConstArray2D fOutputW;
    ConstArray1D fOutputB;

అంతరంగ:
    UResourceBundle* fBundle;
};

LSTMData::LSTMData(UResourceBundle* rb, UError_సంకేతం &స్థితి_2)
    : fDict(శూన్యసూచిక), fType(UNKNOWN), fName(శూన్యసూచిక),
      fBundle(rb)
{
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి;
    }
    ఒకవేళ (IEEE_754 != 1) {
        స్థితి_2 = U_UNSUPPORTED_ERROR;
        తిరిగి;
    }
    Local_UResource_Bundle_సూచిక embeddings_res(
        ures_get_By_కీలకం(rb, "embeddings", శూన్యసూచిక, &స్థితి_2));
    int32_t embedding_కొలత = ures_getInt(embeddings_res.getAlias(), &స్థితి_2);
    Local_UResource_Bundle_సూచిక hunits_res(
        ures_get_By_కీలకం(rb, "hunits", శూన్యసూచిక, &స్థితి_2));
    ఒకవేళ (U_FAILURE(స్థితి_2)) తిరిగి;
    int32_t hunits = ures_getInt(hunits_res.getAlias(), &స్థితి_2);
    స్థిరరాశి char16_t* type = ures_get_String_By_కీలకం(rb, "type", శూన్యసూచిక, &స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) తిరిగి;
    ఒకవేళ (u_strCompare(type, -1, u"codepoints", -1, అబద్ధం) == 0) {
        fType = సంకేతం_POINTS;
    } లేకపోతే ఒకవేళ (u_strCompare(type, -1, u"graphclust", -1, అబద్ధం) == 0) {
        fType = GRAPHEME_CLUSTER;
    }
    fName = ures_get_String_By_కీలకం(rb, "model", శూన్యసూచిక, &స్థితి_2);
    Local_UResource_Bundle_సూచిక దత్తాంశం_Res(ures_get_By_కీలకం(rb, "data", శూన్యసూచిక, &స్థితి_2));
    ఒకవేళ (U_FAILURE(స్థితి_2)) తిరిగి;
    int32_t దత్తాంశం_len = 0;
    స్థిరరాశి int32_t* దత్తాంశం = ures_get_Int_సదిశ(దత్తాంశం_Res.getAlias(), &దత్తాంశం_len, &స్థితి_2);
    fDict = uhash_open(uhash_hashUChars, uhash_compareUChars, శూన్యసూచిక, &స్థితి_2);

    దొంతర_UResource_Bundle దొంతర_తాత్కాలిక_Bundle;
    Resource_దత్తాంశం_విలువ value;
    ures_get_విలువ_With_Fallback(rb, "dict", దొంతర_తాత్కాలిక_Bundle.getAlias(), value, స్థితి_2);
    ResourceArray stringArray = value.getArray(స్థితి_2);
    int32_t అంకె_సూచీ = stringArray.get_కొలత();
    ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి; }

    // put dict into hash
    int32_t string_నిడివి;
    కొరకు (int32_t idx = 0; idx < అంకె_సూచీ; idx++) {
        stringArray.get_విలువ(idx, value);
        స్థిరరాశి char16_t* str = value.getString(string_నిడివి, స్థితి_2);
        uhash_putiAllowZero(fDict, (శూన్య*)str, idx, &స్థితి_2);
        ఒకవేళ (U_FAILURE(స్థితి_2)) తిరిగి;
#నిర్వచితమైతే LSTM_VECTORIZER_DEBUG
        రాయి("Assign [");
        అయితే (*str != 0x0000) {
            రాయి("U+%04x ", *str);
            str++;
        }
        రాయి("] map to %d\n", idx-1);
#అయితే_ముగింపు
    }
    int32_t mat1_కొలత = (అంకె_సూచీ + 1) * embedding_కొలత;
    int32_t mat2_కొలత = embedding_కొలత * 4 * hunits;
    int32_t mat3_కొలత = hunits * 4 * hunits;
    int32_t mat4_కొలత = 4 * hunits;
    int32_t mat5_కొలత = mat2_కొలత;
    int32_t mat6_కొలత = mat3_కొలత;
    int32_t mat7_కొలత = mat4_కొలత;
    int32_t mat8_కొలత = 2 * hunits * 4;
#ఒకవేళ U_DEBUG
    int32_t mat9_కొలత = 4;
    U_ASSERT(దత్తాంశం_len == mat1_కొలత + mat2_కొలత + mat3_కొలత + mat4_కొలత + mat5_కొలత +
        mat6_కొలత + mat7_కొలత + mat8_కొలత + mat9_కొలత);
#అయితే_ముగింపు

    fEmbedding.init(దత్తాంశం, (అంకె_సూచీ + 1), embedding_కొలత);
    దత్తాంశం += mat1_కొలత;
    fForwardW.init(దత్తాంశం, embedding_కొలత, 4 * hunits);
    దత్తాంశం += mat2_కొలత;
    fForwardU.init(దత్తాంశం, hunits, 4 * hunits);
    దత్తాంశం += mat3_కొలత;
    fForwardB.init(దత్తాంశం, 4 * hunits);
    దత్తాంశం += mat4_కొలత;
    fBackwardW.init(దత్తాంశం, embedding_కొలత, 4 * hunits);
    దత్తాంశం += mat5_కొలత;
    fBackwardU.init(దత్తాంశం, hunits, 4 * hunits);
    దత్తాంశం += mat6_కొలత;
    fBackwardB.init(దత్తాంశం, 4 * hunits);
    దత్తాంశం += mat7_కొలత;
    fOutputW.init(దత్తాంశం, 2 * hunits, 4);
    దత్తాంశం += mat8_కొలత;
    fOutputB.init(దత్తాంశం, 4);
}

LSTMData::~LSTMData() {
    uhash_close(fDict);
    ures_close(fBundle);
}

తరగతి Vectorizer : బహిరంగ UMemory {
బహిరంగ:
    Vectorizer(UHashtable* dict) : fDict(dict) {}
    మాయా ~Vectorizer();
    మాయా శూన్య vectorize(UText *పాఠ్యం, int32_t ప్రారంభం_Pos, int32_t చివర_Pos,
                           UVector32 &offsets, UVector32 &indices,
                           UError_సంకేతం &స్థితి_2) స్థిరరాశి = 0;
రక్షిత:
    int32_t string_గమ్యం_సూచీ(స్థిరరాశి char16_t* str) స్థిరరాశి {
        UBool దొరికింది = అబద్ధం;
        int32_t ret = uhash_geti_And_దొరికింది(fDict, (స్థిరరాశి శూన్య*)str, &దొరికింది);
        ఒకవేళ (!దొరికింది) {
            ret = fDict->count;
        }
#నిర్వచితమైతే LSTM_VECTORIZER_DEBUG
        రాయి("[");
        అయితే (*str != 0x0000) {
            రాయి("U+%04x ", *str);
            str++;
        }
        రాయి("] map to %d\n", ret);
#అయితే_ముగింపు
        తిరిగి ret;
    }

అంతరంగ:
    UHashtable* fDict;
};

Vectorizer::~Vectorizer()
{
}

తరగతి సంకేతం_Points_Vectorizer : బహిరంగ Vectorizer {
బహిరంగ:
    సంకేతం_Points_Vectorizer(UHashtable* dict) : Vectorizer(dict) {}
    మాయా ~సంకేతం_Points_Vectorizer();
    మాయా శూన్య vectorize(UText *పాఠ్యం, int32_t ప్రారంభం_Pos, int32_t చివర_Pos,
                           UVector32 &offsets, UVector32 &indices,
                           UError_సంకేతం &స్థితి_2) స్థిరరాశి override;
};

సంకేతం_Points_Vectorizer::~సంకేతం_Points_Vectorizer()
{
}

శూన్య సంకేతం_Points_Vectorizer::vectorize(
    UText *పాఠ్యం, int32_t ప్రారంభం_Pos, int32_t చివర_Pos,
    UVector32 &offsets, UVector32 &indices, UError_సంకేతం &స్థితి_2) స్థిరరాశి
{
    ఒకవేళ (offsets.ensure_సామర్థ్యం(చివర_Pos - ప్రారంభం_Pos, స్థితి_2) &&
            indices.ensure_సామర్థ్యం(చివర_Pos - ప్రారంభం_Pos, స్థితి_2)) {
        ఒకవేళ (U_FAILURE(స్థితి_2)) తిరిగి;
        utext_సమితి_Native_సూచీ(పాఠ్యం, ప్రారంభం_Pos);
        int32_t current;
        char16_t str[2] = {0, 0};
        అయితే (U_SUCCESS(స్థితి_2) &&
               (current = static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం))) < చివర_Pos) {
            // Since the LSTMBreakEngine is currently only accept chars in BMP,
            // we can ignore the possibility of hitting supplementary code
            // point.
            str[0] = static_cast<char16_t>(utext_next32(పాఠ్యం));
            U_ASSERT(!U_IS_SURROGATE(str[0]));
            offsets.కూడు_Element(current, స్థితి_2);
            indices.కూడు_Element(string_గమ్యం_సూచీ(str), స్థితి_2);
        }
    }
}

తరగతి GraphemeClusterVectorizer : బహిరంగ Vectorizer {
బహిరంగ:
    GraphemeClusterVectorizer(UHashtable* dict)
        : Vectorizer(dict)
    {
    }
    మాయా ~GraphemeClusterVectorizer();
    మాయా శూన్య vectorize(UText *పాఠ్యం, int32_t ప్రారంభం_Pos, int32_t చివర_Pos,
                           UVector32 &offsets, UVector32 &indices,
                           UError_సంకేతం &స్థితి_2) స్థిరరాశి override;
};

GraphemeClusterVectorizer::~GraphemeClusterVectorizer()
{
}

constexpr int32_t MAX_GRAPHEME_CLSTER_నిడివి = 10;

శూన్య GraphemeClusterVectorizer::vectorize(
    UText *పాఠ్యం, int32_t ప్రారంభం_Pos, int32_t చివర_Pos,
    UVector32 &offsets, UVector32 &indices, UError_సంకేతం &స్థితి_2) స్థిరరాశి
{
    ఒకవేళ (U_FAILURE(స్థితి_2)) తిరిగి;
    ఒకవేళ (!offsets.ensure_సామర్థ్యం(చివర_Pos - ప్రారంభం_Pos, స్థితి_2) ||
            !indices.ensure_సామర్థ్యం(చివర_Pos - ప్రారంభం_Pos, స్థితి_2)) {
        తిరిగి;
    }
    ఒకవేళ (U_FAILURE(స్థితి_2)) తిరిగి;
    Local_సూచిక<BreakIterator> graphemeIter(BreakIterator::createCharacterInstance(Locale(), స్థితి_2));
    ఒకవేళ (U_FAILURE(స్థితి_2)) తిరిగి;
    graphemeIter->సమితి_పాఠ్యం(పాఠ్యం, స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) తిరిగి;

    ఒకవేళ (ప్రారంభం_Pos != 0) {
        graphemeIter->preceding(ప్రారంభం_Pos);
    }
    int32_t చివరి = ప్రారంభం_Pos;
    int32_t current = ప్రారంభం_Pos;
    char16_t str[MAX_GRAPHEME_CLSTER_నిడివి];
    అయితే ((current = graphemeIter->next()) != BreakIterator::పూర్తి_2) {
        ఒకవేళ (current >= చివర_Pos) {
            ఆపు;
        }
        ఒకవేళ (current > ప్రారంభం_Pos) {
            utext_వెలికితీయి(పాఠ్యం, చివరి, current, str, MAX_GRAPHEME_CLSTER_నిడివి, &స్థితి_2);
            ఒకవేళ (U_FAILURE(స్థితి_2)) తిరిగి;
            offsets.కూడు_Element(చివరి, స్థితి_2);
            indices.కూడు_Element(string_గమ్యం_సూచీ(str), స్థితి_2);
            ఒకవేళ (U_FAILURE(స్థితి_2)) తిరిగి;
        }
        చివరి = current;
    }
    ఒకవేళ (U_FAILURE(స్థితి_2) || చివరి >= చివర_Pos) {
        తిరిగి;
    }
    utext_వెలికితీయి(పాఠ్యం, చివరి, చివర_Pos, str, MAX_GRAPHEME_CLSTER_నిడివి, &స్థితి_2);
    ఒకవేళ (U_SUCCESS(స్థితి_2)) {
        offsets.కూడు_Element(చివరి, స్థితి_2);
        indices.కూడు_Element(string_గమ్యం_సూచీ(str), స్థితి_2);
    }
}

// Computing LSTM as stated in
// https://en.wikipedia.org/wiki/Long_short-term_memory#LSTM_with_a_forget_gate
// ifco is temp array allocate outside which does not need to be
// input/output value but could avoid unnecessary memory alloc/free if passing
// in.
శూన్య compute(
    int32_t hunits,
    స్థిరరాశి ReadArray2D& W, స్థిరరాశి ReadArray2D& U, స్థిరరాశి ReadArray1D& b,
    స్థిరరాశి ReadArray1D& x, Array1D& h, Array1D& c,
    Array1D& ifco)
{
    // ifco = x * W + h * U + b
    ifco.assign(b)
        .కూడు_Dot_లబ్ధం(x, W)
        .కూడు_Dot_లబ్ధం(h, U);

    ifco.ముక్క(0*hunits, hunits).sigmoid();  // i: sigmod
    ifco.ముక్క(1*hunits, hunits).sigmoid(); // f: sigmoid
    ifco.ముక్క(2*hunits, hunits).tanh(); // c_: tanh
    ifco.ముక్క(3*hunits, hunits).sigmoid(); // o: sigmod

    c.hadamard_లబ్ధం(ifco.ముక్క(hunits, hunits))
        .కూడు_Hadamard_లబ్ధం(ifco.ముక్క(0, hunits), ifco.ముక్క(2*hunits, hunits));

    h.tanh(c)
        .hadamard_లబ్ధం(ifco.ముక్క(3*hunits, hunits));
}

// Minimum word size
స్థిర స్థిరరాశి int32_t MIN_మాట = 2;

// Minimum number of characters for two words
స్థిర స్థిరరాశి int32_t MIN_మాట_పరిధి = MIN_మాట * 2;

int32_t
LSTMBreakEngine::divide_పైకి_Dictionary_Range( UText *పాఠ్యం,
                                                int32_t ప్రారంభం_Pos,
                                                int32_t చివర_Pos,
                                                UVector32 &దొరికింది_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_సంకేతం& స్థితి_2) స్థిరరాశి {
    ఒకవేళ (U_FAILURE(స్థితి_2)) తిరిగి 0;
    int32_t begin_దొరికింది_Break_కొలత = దొరికింది_Breaks.size();
    utext_సమితి_Native_సూచీ(పాఠ్యం, ప్రారంభం_Pos);
    utext_moveIndex32(పాఠ్యం, MIN_మాట_పరిధి);
    ఒకవేళ (utext_get_Native_సూచీ(పాఠ్యం) >= చివర_Pos) {
        తిరిగి 0;       // Not enough characters for two words
    }
    utext_సమితి_Native_సూచీ(పాఠ్యం, ప్రారంభం_Pos);

    UVector32 offsets(స్థితి_2);
    UVector32 indices(స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) తిరిగి 0;
    fVectorizer->vectorize(పాఠ్యం, ప్రారంభం_Pos, చివర_Pos, offsets, indices, స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) తిరిగి 0;
    int32_t* offsetsBuf = offsets.get_బఫరు();
    int32_t* indicesBuf = indices.get_బఫరు();

    int32_t ఆదానం_seq_len = indices.size();
    int32_t hunits = f_దత్తాంశం->fForwardU.d1();

    // ----- Begin of all the Array memory allocation needed for this function
    // Allocate temp array used inside compute()
    Array1D ifco(4 * hunits, స్థితి_2);

    Array1D c(hunits, స్థితి_2);
    Array1D logp(4, స్థితి_2);

    // TODO: limit size of hBackward. If input_seq_len is too big, we could
    // run out of memory.
    // Backward LSTM
    Array2D hBackward(ఆదానం_seq_len, hunits, స్థితి_2);

    // Allocate fbRow and slice the internal array in two.
    Array1D fb_అడ్డువరుస(2 * hunits, స్థితి_2);

    // ----- End of all the Array memory allocation needed for this function
    ఒకవేళ (U_FAILURE(స్థితి_2)) తిరిగి 0;

    // To save the needed memory usage, the following is different from the
    // Python or ICU4X implementation. We first perform the Backward LSTM
    // and then merge the iteration of the forward LSTM and the output layer
    // together because we only neetdto remember the h[t-1] for Forward LSTM.
    కొరకు (int32_t i = ఆదానం_seq_len - 1; i >= 0; i--) {
        Array1D h_అడ్డువరుస = hBackward.అడ్డువరుస(i);
        ఒకవేళ (i != ఆదానం_seq_len - 1) {
            h_అడ్డువరుస.assign(hBackward.అడ్డువరుస(i+1));
        }
#నిర్వచితమైతే LSTM_DEBUG
        రాయి("hRow %d\n", i);
        h_అడ్డువరుస.print();
        రాయి("indicesBuf[%d] = %d\n", i, indicesBuf[i]);
        రాయి("fData->fEmbedding.row(indicesBuf[%d]):\n", i);
        f_దత్తాంశం->fEmbedding.అడ్డువరుస(indicesBuf[i]).print();
#అయితే_ముగింపు  // LSTM_DEBUG
        compute(hunits,
                f_దత్తాంశం->fBackwardW, f_దత్తాంశం->fBackwardU, f_దత్తాంశం->fBackwardB,
                f_దత్తాంశం->fEmbedding.అడ్డువరుస(indicesBuf[i]),
                h_అడ్డువరుస, c, ifco);
    }


    Array1D forward_అడ్డువరుస = fb_అడ్డువరుస.ముక్క(0, hunits);  // point to first half of data in fbRow.
    Array1D backward_అడ్డువరుస = fb_అడ్డువరుస.ముక్క(hunits, hunits);  // point to second half of data n fbRow.

    // The following iteration merge the forward LSTM and the output layer
    // together.
    c.clear();  // reuse c since it is the same size.
    కొరకు (int32_t i = 0; i < ఆదానం_seq_len; i++) {
#నిర్వచితమైతే LSTM_DEBUG
        రాయి("forwardRow %d\n", i);
        forward_అడ్డువరుస.print();
#అయితే_ముగింపు  // LSTM_DEBUG
        // Forward LSTM
        // Calculate the result into forwardRow, which point to the data in the first half
        // of fbRow.
        compute(hunits,
                f_దత్తాంశం->fForwardW, f_దత్తాంశం->fForwardU, f_దత్తాంశం->fForwardB,
                f_దత్తాంశం->fEmbedding.అడ్డువరుస(indicesBuf[i]),
                forward_అడ్డువరుస, c, ifco);

        // assign the data from hBackward.row(i) to second half of fbRowa.
        backward_అడ్డువరుస.assign(hBackward.అడ్డువరుస(i));

        logp.assign(f_దత్తాంశం->fOutputB).కూడు_Dot_లబ్ధం(fb_అడ్డువరుస, f_దత్తాంశం->fOutputW);
#నిర్వచితమైతే LSTM_DEBUG
        రాయి("backwardRow %d\n", i);
        backward_అడ్డువరుస.print();
        రాయి("logp %d\n", i);
        logp.print();
#అయితే_ముగింపు  // LSTM_DEBUG

        // current = argmax(logp)
        LSTMClass current = static_cast<LSTMClass>(logp.max_సూచీ());
        // BIES logic.
        ఒకవేళ (current == BEGIN || current == SINGLE) {
            ఒకవేళ (i != 0) {
                దొరికింది_Breaks.కూడు_Element(offsetsBuf[i], స్థితి_2);
                ఒకవేళ (U_FAILURE(స్థితి_2)) తిరిగి 0;
            }
        }
    }
    తిరిగి దొరికింది_Breaks.size() - begin_దొరికింది_Break_కొలత;
}

Vectorizer* createVectorizer(స్థిరరాశి LSTMData* దత్తాంశం, UError_సంకేతం &స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }
    ఎంపిక (దత్తాంశం->fType) {
        స్థితి సంకేతం_POINTS:
            తిరిగి కొత్త సంకేతం_Points_Vectorizer(దత్తాంశం->fDict);
            ఆపు;
        స్థితి GRAPHEME_CLUSTER:
            తిరిగి కొత్త GraphemeClusterVectorizer(దత్తాంశం->fDict);
            ఆపు;
        అప్రమేయ:
            ఆపు;
    }
    UPRV_UNREACHABLE_EXIT;
}

LSTMBreakEngine::LSTMBreakEngine(స్థిరరాశి LSTMData* దత్తాంశం, స్థిరరాశి Unicode_సమితి& సమితి, UError_సంకేతం &స్థితి_2)
    : DictionaryBreakEngine(), f_దత్తాంశం(దత్తాంశం), fVectorizer(createVectorizer(f_దత్తాంశం, స్థితి_2))
{
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
      f_దత్తాంశం = శూన్యసూచిక;  // If failure, we should not delete fData in destructor because the caller will do so.
      తిరిగి;
    }
    సమితి_Characters(సమితి);
}

LSTMBreakEngine::~LSTMBreakEngine() {
    తొలగించు f_దత్తాంశం;
    తొలగించు fVectorizer;
}

స్థిరరాశి char16_t* LSTMBreakEngine::name() స్థిరరాశి {
    తిరిగి f_దత్తాంశం->fName;
}

UnicodeString defaultLSTM(UScript_సంకేతం script, UError_సంకేతం& స్థితి_2) {
    // open root from brkitr tree.
    UResourceBundle *b = ures_open(U_ICUDATA_BRKITR, "", &స్థితి_2);
    b = ures_get_By_కీలకం_With_Fallback(b, "lstm", b, &స్థితి_2);
    UnicodeString ఫలితం = ures_get_Unicode_String_By_కీలకం(b, uscript_getShortName(script), &స్థితి_2);
    ures_close(b);
    తిరిగి ఫలితం;
}

U_CAPI స్థిరరాశి LSTMData* U_EXPORT2 CreateLSTMDataForScript(UScript_సంకేతం script, UError_సంకేతం& స్థితి_2)
{
    ఒకవేళ (script != USCRIPT_KHMER && script != USCRIPT_LAO && script != USCRIPT_MYANMAR && script != USCRIPT_THAI) {
        తిరిగి శూన్యసూచిక;
    }
    UnicodeString name = defaultLSTM(script, స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) తిరిగి శూన్యసూచిక;
    CharString namebuf;
    namebuf.appendInvariantChars(name, స్థితి_2).truncate(namebuf.చివరి_సూచీ_యొక్క('.'));

    Local_UResource_Bundle_సూచిక rb(
        ures_open_ప్రత్యక్ష(U_ICUDATA_BRKITR, namebuf.దత్తాంశం(), &స్థితి_2));
    ఒకవేళ (U_FAILURE(స్థితి_2)) తిరిగి శూన్యసూచిక;

    తిరిగి CreateLSTMData(rb.orphan(), స్థితి_2);
}

U_CAPI స్థిరరాశి LSTMData* U_EXPORT2 CreateLSTMData(UResourceBundle* rb, UError_సంకేతం& స్థితి_2)
{
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }
    స్థిరరాశి LSTMData* ఫలితం = కొత్త LSTMData(rb, స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తొలగించు ఫలితం;
        తిరిగి శూన్యసూచిక;
    }
    తిరిగి ఫలితం;
}

U_CAPI స్థిరరాశి LanguageBreakEngine* U_EXPORT2
CreateLSTMBreakEngine(UScript_సంకేతం script, స్థిరరాశి LSTMData* దత్తాంశం, UError_సంకేతం& స్థితి_2)
{
    UnicodeString unicode_సమితి_String;
    ఎంపిక(script) {
        స్థితి USCRIPT_THAI:
            unicode_సమితి_String = UnicodeString(u"[[:Thai:]&[:LineBreak=SA:]]");
            ఆపు;
        స్థితి USCRIPT_MYANMAR:
            unicode_సమితి_String = UnicodeString(u"[[:Mymr:]&[:LineBreak=SA:]]");
            ఆపు;
        అప్రమేయ:
            తొలగించు దత్తాంశం;
            తిరిగి శూన్యసూచిక;
    }
    Unicode_సమితి unicode_సమితి;
    unicode_సమితి.apply_నమూనా(unicode_సమితి_String, స్థితి_2);
    స్థిరరాశి LanguageBreakEngine* engine = కొత్త LSTMBreakEngine(దత్తాంశం, unicode_సమితి, స్థితి_2);
    ఒకవేళ (U_FAILURE(స్థితి_2) || engine == శూన్యసూచిక) {
        ఒకవేళ (engine != శూన్యసూచిక) {
            తొలగించు engine;
        } లేకపోతే {
            స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        }
        తిరిగి శూన్యసూచిక;
    }
    తిరిగి engine;
}

U_CAPI శూన్య U_EXPORT2 DeleteLSTMData(స్థిరరాశి LSTMData* దత్తాంశం)
{
    తొలగించు దత్తాంశం;
}

U_CAPI స్థిరరాశి char16_t* U_EXPORT2 LSTMDataName(స్థిరరాశి LSTMData* దత్తాంశం)
{
    తిరిగి దత్తాంశం->fName;
}

U_NAMESPACE_చివర

#అయితే_ముగింపు /* #if !UCONFIG_NO_BREAK_ITERATION */
