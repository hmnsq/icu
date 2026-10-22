// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#अन्तर्भाव <complex>
#अन्तर्भाव <utility>

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_BREAK_ITERATION

#अन्तर्भाव "brkeng.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "lstmbe.h"
#अन्तर्भाव "putilimp.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "ubrkimpl.h"
#अन्तर्भाव "uresimp.h"
#अन्तर्भाव "uvectr32.h"
#अन्तर्भाव "uvector.h"

#अन्तर्भाव "unicode/brkiter.h"
#अन्तर्भाव "unicode/resbund.h"
#अन्तर्भाव "unicode/ubrk.h"
#अन्तर्भाव "unicode/uniset.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/utf.h"

U_NAMESPACE_BEGIN

// Uncomment the following #define to debug.
// #define LSTM_DEBUG 1
// #define LSTM_VECTORIZER_DEBUG 1

/**
 * Interface for reading 1D array.
 */
वर्ग ReadArray1D {
सार्वजनिक:
    आभास ~ReadArray1D();
    आभास int32_t d1() ध्रुव = 0;
    आभास भिन्न get(int32_t i) ध्रुव = 0;

#यदि_परिभाषितम् LSTM_DEBUG
    शून्य print() ध्रुव {
        प्रारूप_लिख("\n[");
        प्रति (int32_t i = 0; i < d1(); i++) {
           प्रारूप_लिख("%0.8e ", get(i));
           यदि (i % 4 == 3) प्रारूप_लिख("\n");
        }
        प्रारूप_लिख("]\n");
    }
#यदि_अन्तः
};

ReadArray1D::~ReadArray1D()
{
}

/**
 * Interface for reading 2D array.
 */
वर्ग ReadArray2D {
सार्वजनिक:
    आभास ~ReadArray2D();
    आभास int32_t d1() ध्रुव = 0;
    आभास int32_t d2() ध्रुव = 0;
    आभास भिन्न get(int32_t i, int32_t j) ध्रुव = 0;
};

ReadArray2D::~ReadArray2D()
{
}

/**
 * A class to index a float array as a 1D Array without owning the pointer or
 * copy the data.
 */
वर्ग ConstArray1D : सार्वजनिक ReadArray1D {
सार्वजनिक:
    ConstArray1D() : दत्त_2(शून्यसूचकः), d1_(0) {}

    ConstArray1D(ध्रुव भिन्न* दत्त, int32_t d1) : दत्त_2(दत्त), d1_(d1) {}

    आभास ~ConstArray1D();

    // Init the object, the object does not own the data nor copy.
    // It is designed to directly use data from memory mapped resources.
    शून्य init(ध्रुव int32_t* दत्त, int32_t d1) {
        U_ASSERT(IEEE_754 == 1);
        दत्त_2 = reinterpret_cast<ध्रुव भिन्न*>(दत्त);
        d1_ = d1;
    }

    // ReadArray1D methods.
    आभास int32_t d1() ध्रुव override { प्रत्यागम d1_; }
    आभास भिन्न get(int32_t i) ध्रुव override {
        U_ASSERT(i < d1_);
        प्रत्यागम दत्त_2[i];
    }

गुप्त:
    ध्रुव भिन्न* दत्त_2;
    int32_t d1_;
};

ConstArray1D::~ConstArray1D()
{
}

/**
 * A class to index a float array as a 2D Array without owning the pointer or
 * copy the data.
 */
वर्ग ConstArray2D : सार्वजनिक ReadArray2D {
सार्वजनिक:
    ConstArray2D() : दत्त_2(शून्यसूचकः), d1_(0), d2_(0) {}

    ConstArray2D(ध्रुव भिन्न* दत्त, int32_t d1, int32_t d2)
        : दत्त_2(दत्त), d1_(d1), d2_(d2) {}

    आभास ~ConstArray2D();

    // Init the object, the object does not own the data nor copy.
    // It is designed to directly use data from memory mapped resources.
    शून्य init(ध्रुव int32_t* दत्त, int32_t d1, int32_t d2) {
        U_ASSERT(IEEE_754 == 1);
        दत्त_2 = reinterpret_cast<ध्रुव भिन्न*>(दत्त);
        d1_ = d1;
        d2_ = d2;
    }

    // ReadArray2D methods.
    अन्तःस्थ int32_t d1() ध्रुव override { प्रत्यागम d1_; }
    अन्तःस्थ int32_t d2() ध्रुव override { प्रत्यागम d2_; }
    भिन्न get(int32_t i, int32_t j) ध्रुव override {
        U_ASSERT(i < d1_);
        U_ASSERT(j < d2_);
        प्रत्यागम दत्त_2[i * d2_ + j];
    }

    // Expose the ith row as a ConstArray1D
    अन्तःस्थ ConstArray1D row(int32_t i) ध्रुव {
        U_ASSERT(i < d1_);
        प्रत्यागम ConstArray1D(दत्त_2 + i * d2_, d2_);
    }

गुप्त:
    ध्रुव भिन्न* दत्त_2;
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
वर्ग Array1D : सार्वजनिक ReadArray1D {
सार्वजनिक:
    Array1D() : स्मृतिः_2(शून्यसूचकः), दत्त_2(शून्यसूचकः), d1_(0) {}
    Array1D(int32_t d1, UError_संहिता &स्थितिः)
        : स्मृतिः_2(uprv_malloc(d1 * परिमाण(भिन्न))),
          दत्त_2(static_cast<भिन्न*>(स्मृतिः_2)), d1_(d1) {
        यदि (U_SUCCESS(स्थितिः)) {
            यदि (स्मृतिः_2 == शून्यसूचकः) {
                स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
                प्रत्यागम;
            }
            clear();
        }
    }

    आभास ~Array1D();

    // A special constructor which does not own the memory but writeable
    // as a slice of an array.
    Array1D(भिन्न* दत्त, int32_t d1)
        : स्मृतिः_2(शून्यसूचकः), दत्त_2(दत्त), d1_(d1) {}

    // ReadArray1D methods.
    आभास int32_t d1() ध्रुव override { प्रत्यागम d1_; }
    आभास भिन्न get(int32_t i) ध्रुव override {
        U_ASSERT(i < d1_);
        प्रत्यागम दत्त_2[i];
    }

    // Return the index which point to the max data in the array.
    अन्तःस्थ int32_t max_सूचकांक() ध्रुव {
        int32_t सूचकांक = 0;
        भिन्न max = दत्त_2[0];
        प्रति (int32_t i = 1; i < d1_; i++) {
            यदि (दत्त_2[i] > max) {
                max = दत्त_2[i];
                सूचकांक = i;
            }
        }
        प्रत्यागम सूचकांक;
    }

    // Slice part of the array to a new one.
    अन्तःस्थ Array1D खण्डिका(int32_t from, int32_t size) ध्रुव {
        U_ASSERT(from >= 0);
        U_ASSERT(from < d1_);
        U_ASSERT(from + size <= d1_);
        प्रत्यागम Array1D(दत्त_2 + from, size);
    }

    // Add dot product of a 1D array and a 2D array into this one.
    अन्तःस्थ Array1D& सङ्कलय_Dot_गुणनफलम्(ध्रुव ReadArray1D& a, ध्रुव ReadArray2D& b) {
        U_ASSERT(a.d1() == b.d1());
        U_ASSERT(b.d2() == d1());
        प्रति (int32_t i = 0; i < d1(); i++) {
            प्रति (int32_t j = 0; j < a.d1(); j++) {
                दत्त_2[i] += a.get(j) * b.get(j, i);
            }
        }
        प्रत्यागम *एतत्;
    }

    // Hadamard Product the values of another array of the same size into this one.
    अन्तःस्थ Array1D& hadamard_गुणनफलम्(ध्रुव ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        प्रति (int32_t i = 0; i < d1(); i++) {
            दत्त_2[i] *= a.get(i);
        }
        प्रत्यागम *एतत्;
    }

    // Add the Hadamard Product of two arrays of the same size into this one.
    अन्तःस्थ Array1D& सङ्कलय_Hadamard_गुणनफलम्(ध्रुव ReadArray1D& a, ध्रुव ReadArray1D& b) {
        U_ASSERT(a.d1() == d1());
        U_ASSERT(b.d1() == d1());
        प्रति (int32_t i = 0; i < d1(); i++) {
            दत्त_2[i] += a.get(i) * b.get(i);
        }
        प्रत्यागम *एतत्;
    }

    // Add the values of another array of the same size into this one.
    अन्तःस्थ Array1D& सङ्कलय(ध्रुव ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        प्रति (int32_t i = 0; i < d1(); i++) {
            दत्त_2[i] += a.get(i);
        }
        प्रत्यागम *एतत्;
    }

    // Assign the values of another array of the same size into this one.
    अन्तःस्थ Array1D& assign(ध्रुव ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        प्रति (int32_t i = 0; i < d1(); i++) {
            दत्त_2[i] = a.get(i);
        }
        प्रत्यागम *एतत्;
    }

    // Apply tanh to all the elements in the array.
    अन्तःस्थ Array1D& tanh() {
        प्रत्यागम tanh(*एतत्);
    }

    // Apply tanh of a and store into this array.
    अन्तःस्थ Array1D& tanh(ध्रुव Array1D& a) {
        U_ASSERT(a.d1() == d1());
        प्रति (int32_t i = 0; i < d1_; i++) {
            दत्त_2[i] = std::tanh(a.get(i));
        }
        प्रत्यागम *एतत्;
    }

    // Apply sigmoid to all the elements in the array.
    अन्तःस्थ Array1D& sigmoid() {
        प्रति (int32_t i = 0; i < d1_; i++) {
            दत्त_2[i] = 1.0f/(1.0f + expf(-दत्त_2[i]));
        }
        प्रत्यागम *एतत्;
    }

    अन्तःस्थ Array1D& clear() {
        uprv_memset(दत्त_2, 0, d1_ * परिमाण(भिन्न));
        प्रत्यागम *एतत्;
    }

गुप्त:
    शून्य* स्मृतिः_2;
    भिन्न* दत्त_2;
    int32_t d1_;
};

Array1D::~Array1D()
{
    uprv_free(स्मृतिः_2);
}

वर्ग Array2D : सार्वजनिक ReadArray2D {
सार्वजनिक:
    Array2D() : स्मृतिः_2(शून्यसूचकः), दत्त_2(शून्यसूचकः), d1_(0), d2_(0) {}
    Array2D(int32_t d1, int32_t d2, UError_संहिता &स्थितिः)
        : स्मृतिः_2(uprv_malloc(d1 * d2 * परिमाण(भिन्न))),
          दत्त_2(static_cast<भिन्न*>(स्मृतिः_2)), d1_(d1), d2_(d2) {
        यदि (U_SUCCESS(स्थितिः)) {
            यदि (स्मृतिः_2 == शून्यसूचकः) {
                स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
                प्रत्यागम;
            }
            clear();
        }
    }
    आभास ~Array2D();

    // ReadArray2D methods.
    आभास int32_t d1() ध्रुव override { प्रत्यागम d1_; }
    आभास int32_t d2() ध्रुव override { प्रत्यागम d2_; }
    आभास भिन्न get(int32_t i, int32_t j) ध्रुव override {
        U_ASSERT(i < d1_);
        U_ASSERT(j < d2_);
        प्रत्यागम दत्त_2[i * d2_ + j];
    }

    अन्तःस्थ Array1D row(int32_t i) ध्रुव {
        U_ASSERT(i < d1_);
        प्रत्यागम Array1D(दत्त_2 + i * d2_, d2_);
    }

    अन्तःस्थ Array2D& clear() {
        uprv_memset(दत्त_2, 0, d1_ * d2_ * परिमाण(भिन्न));
        प्रत्यागम *एतत्;
    }

गुप्त:
    शून्य* स्मृतिः_2;
    भिन्न* दत्त_2;
    int32_t d1_;
    int32_t d2_;
};

Array2D::~Array2D()
{
    uprv_free(स्मृतिः_2);
}

प्रकारनिर्देश गणना {
    BEGIN,
    INSIDE,
    अन्तः_2,
    SINGLE
} LSTMClass;

प्रकारनिर्देश गणना {
    UNKNOWN,
    संहिता_POINTS,
    GRAPHEME_CLUSTER,
} EmbeddingType;

संरचना LSTMData : सार्वजनिक UMemory {
    LSTMData(UResourceBundle* rb, UError_संहिता &स्थितिः);
    ~LSTMData();
    UHashtable* fDict;
    EmbeddingType fType;
    ध्रुव char16_t* fName;
    ConstArray2D fEmbedding;
    ConstArray2D fForwardW;
    ConstArray2D fForwardU;
    ConstArray1D fForwardB;
    ConstArray2D fBackwardW;
    ConstArray2D fBackwardU;
    ConstArray1D fBackwardB;
    ConstArray2D fOutputW;
    ConstArray1D fOutputB;

गुप्त:
    UResourceBundle* fBundle;
};

LSTMData::LSTMData(UResourceBundle* rb, UError_संहिता &स्थितिः)
    : fDict(शून्यसूचकः), fType(UNKNOWN), fName(शून्यसूचकः),
      fBundle(rb)
{
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम;
    }
    यदि (IEEE_754 != 1) {
        स्थितिः = U_UNSUPPORTED_ERROR;
        प्रत्यागम;
    }
    LocalUResourceBundlePointer embeddings_res(
        ures_get_By_कुञ्चिका(rb, "embeddings", शून्यसूचकः, &स्थितिः));
    int32_t embedding_आकार = ures_getInt(embeddings_res.getAlias(), &स्थितिः);
    LocalUResourceBundlePointer hunits_res(
        ures_get_By_कुञ्चिका(rb, "hunits", शून्यसूचकः, &स्थितिः));
    यदि (U_FAILURE(स्थितिः)) प्रत्यागम;
    int32_t hunits = ures_getInt(hunits_res.getAlias(), &स्थितिः);
    ध्रुव char16_t* type = ures_get_String_By_कुञ्चिका(rb, "type", शून्यसूचकः, &स्थितिः);
    यदि (U_FAILURE(स्थितिः)) प्रत्यागम;
    यदि (u_strCompare(type, -1, u"codepoints", -1, असत्य) == 0) {
        fType = संहिता_POINTS;
    } अन्यथा यदि (u_strCompare(type, -1, u"graphclust", -1, असत्य) == 0) {
        fType = GRAPHEME_CLUSTER;
    }
    fName = ures_get_String_By_कुञ्चिका(rb, "model", शून्यसूचकः, &स्थितिः);
    LocalUResourceBundlePointer दत्त_Res(ures_get_By_कुञ्चिका(rb, "data", शून्यसूचकः, &स्थितिः));
    यदि (U_FAILURE(स्थितिः)) प्रत्यागम;
    int32_t दत्त_len = 0;
    ध्रुव int32_t* दत्त = ures_get_Int_सदिशम्(दत्त_Res.getAlias(), &दत्त_len, &स्थितिः);
    fDict = uhash_open(uhash_hashUChars, uhash_compareUChars, शून्यसूचकः, &स्थितिः);

    राशिः_UResource_Bundle राशिः_अस्थायी_Bundle;
    Resource_दत्त_मानम् value;
    ures_get_मानम्_With_Fallback(rb, "dict", राशिः_अस्थायी_Bundle.getAlias(), value, स्थितिः);
    ResourceArray stringArray = value.getArray(स्थितिः);
    int32_t अङ्कः_सूचकांक = stringArray.get_आकार();
    यदि (U_FAILURE(स्थितिः)) { प्रत्यागम; }

    // put dict into hash
    int32_t string_दीर्घता;
    प्रति (int32_t idx = 0; idx < अङ्कः_सूचकांक; idx++) {
        stringArray.get_मानम्(idx, value);
        ध्रुव char16_t* str = value.getString(string_दीर्घता, स्थितिः);
        uhash_putiAllowZero(fDict, (शून्य*)str, idx, &स्थितिः);
        यदि (U_FAILURE(स्थितिः)) प्रत्यागम;
#यदि_परिभाषितम् LSTM_VECTORIZER_DEBUG
        प्रारूप_लिख("Assign [");
        यावत् (*str != 0x0000) {
            प्रारूप_लिख("U+%04x ", *str);
            str++;
        }
        प्रारूप_लिख("] map to %d\n", idx-1);
#यदि_अन्तः
    }
    int32_t mat1_आकार = (अङ्कः_सूचकांक + 1) * embedding_आकार;
    int32_t mat2_आकार = embedding_आकार * 4 * hunits;
    int32_t mat3_आकार = hunits * 4 * hunits;
    int32_t mat4_आकार = 4 * hunits;
    int32_t mat5_आकार = mat2_आकार;
    int32_t mat6_आकार = mat3_आकार;
    int32_t mat7_आकार = mat4_आकार;
    int32_t mat8_आकार = 2 * hunits * 4;
#यदि U_DEBUG
    int32_t mat9_आकार = 4;
    U_ASSERT(दत्त_len == mat1_आकार + mat2_आकार + mat3_आकार + mat4_आकार + mat5_आकार +
        mat6_आकार + mat7_आकार + mat8_आकार + mat9_आकार);
#यदि_अन्तः

    fEmbedding.init(दत्त, (अङ्कः_सूचकांक + 1), embedding_आकार);
    दत्त += mat1_आकार;
    fForwardW.init(दत्त, embedding_आकार, 4 * hunits);
    दत्त += mat2_आकार;
    fForwardU.init(दत्त, hunits, 4 * hunits);
    दत्त += mat3_आकार;
    fForwardB.init(दत्त, 4 * hunits);
    दत्त += mat4_आकार;
    fBackwardW.init(दत्त, embedding_आकार, 4 * hunits);
    दत्त += mat5_आकार;
    fBackwardU.init(दत्त, hunits, 4 * hunits);
    दत्त += mat6_आकार;
    fBackwardB.init(दत्त, 4 * hunits);
    दत्त += mat7_आकार;
    fOutputW.init(दत्त, 2 * hunits, 4);
    दत्त += mat8_आकार;
    fOutputB.init(दत्त, 4);
}

LSTMData::~LSTMData() {
    uhash_close(fDict);
    ures_close(fBundle);
}

वर्ग Vectorizer : सार्वजनिक UMemory {
सार्वजनिक:
    Vectorizer(UHashtable* dict) : fDict(dict) {}
    आभास ~Vectorizer();
    आभास शून्य vectorize(UText *text, int32_t startPos, int32_t अन्तः_Pos,
                           UVector32 &offsets, UVector32 &indices,
                           UError_संहिता &स्थितिः) ध्रुव = 0;
रक्षित:
    int32_t string_गन्तव्यम्_सूचकांक(ध्रुव char16_t* str) ध्रुव {
        UBool प्राप्तम् = असत्य;
        int32_t ret = uhash_geti_And_प्राप्तम्(fDict, (ध्रुव शून्य*)str, &प्राप्तम्);
        यदि (!प्राप्तम्) {
            ret = fDict->count;
        }
#यदि_परिभाषितम् LSTM_VECTORIZER_DEBUG
        प्रारूप_लिख("[");
        यावत् (*str != 0x0000) {
            प्रारूप_लिख("U+%04x ", *str);
            str++;
        }
        प्रारूप_लिख("] map to %d\n", ret);
#यदि_अन्तः
        प्रत्यागम ret;
    }

गुप्त:
    UHashtable* fDict;
};

Vectorizer::~Vectorizer()
{
}

वर्ग संहिता_Points_Vectorizer : सार्वजनिक Vectorizer {
सार्वजनिक:
    संहिता_Points_Vectorizer(UHashtable* dict) : Vectorizer(dict) {}
    आभास ~संहिता_Points_Vectorizer();
    आभास शून्य vectorize(UText *text, int32_t startPos, int32_t अन्तः_Pos,
                           UVector32 &offsets, UVector32 &indices,
                           UError_संहिता &स्थितिः) ध्रुव override;
};

संहिता_Points_Vectorizer::~संहिता_Points_Vectorizer()
{
}

शून्य संहिता_Points_Vectorizer::vectorize(
    UText *text, int32_t startPos, int32_t अन्तः_Pos,
    UVector32 &offsets, UVector32 &indices, UError_संहिता &स्थितिः) ध्रुव
{
    यदि (offsets.ensure_क्षमता(अन्तः_Pos - startPos, स्थितिः) &&
            indices.ensure_क्षमता(अन्तः_Pos - startPos, स्थितिः)) {
        यदि (U_FAILURE(स्थितिः)) प्रत्यागम;
        utext_समुच्चयः_Native_सूचकांक(text, startPos);
        int32_t current;
        char16_t str[2] = {0, 0};
        यावत् (U_SUCCESS(स्थितिः) &&
               (current = static_cast<int32_t>(utext_get_Native_सूचकांक(text))) < अन्तः_Pos) {
            // Since the LSTMBreakEngine is currently only accept chars in BMP,
            // we can ignore the possibility of hitting supplementary code
            // point.
            str[0] = static_cast<char16_t>(utext_next32(text));
            U_ASSERT(!U_IS_SURROGATE(str[0]));
            offsets.सङ्कलय_Element(current, स्थितिः);
            indices.सङ्कलय_Element(string_गन्तव्यम्_सूचकांक(str), स्थितिः);
        }
    }
}

वर्ग GraphemeClusterVectorizer : सार्वजनिक Vectorizer {
सार्वजनिक:
    GraphemeClusterVectorizer(UHashtable* dict)
        : Vectorizer(dict)
    {
    }
    आभास ~GraphemeClusterVectorizer();
    आभास शून्य vectorize(UText *text, int32_t startPos, int32_t अन्तः_Pos,
                           UVector32 &offsets, UVector32 &indices,
                           UError_संहिता &स्थितिः) ध्रुव override;
};

GraphemeClusterVectorizer::~GraphemeClusterVectorizer()
{
}

constexpr int32_t MAX_GRAPHEME_CLSTER_दीर्घता = 10;

शून्य GraphemeClusterVectorizer::vectorize(
    UText *text, int32_t startPos, int32_t अन्तः_Pos,
    UVector32 &offsets, UVector32 &indices, UError_संहिता &स्थितिः) ध्रुव
{
    यदि (U_FAILURE(स्थितिः)) प्रत्यागम;
    यदि (!offsets.ensure_क्षमता(अन्तः_Pos - startPos, स्थितिः) ||
            !indices.ensure_क्षमता(अन्तः_Pos - startPos, स्थितिः)) {
        प्रत्यागम;
    }
    यदि (U_FAILURE(स्थितिः)) प्रत्यागम;
    LocalPointer<BreakIterator> graphemeIter(BreakIterator::createCharacterInstance(Locale(), स्थितिः));
    यदि (U_FAILURE(स्थितिः)) प्रत्यागम;
    graphemeIter->समुच्चयः_Text(text, स्थितिः);
    यदि (U_FAILURE(स्थितिः)) प्रत्यागम;

    यदि (startPos != 0) {
        graphemeIter->preceding(startPos);
    }
    int32_t चरमः = startPos;
    int32_t current = startPos;
    char16_t str[MAX_GRAPHEME_CLSTER_दीर्घता];
    यावत् ((current = graphemeIter->next()) != BreakIterator::समाप्तम्_2) {
        यदि (current >= अन्तः_Pos) {
            भङ्ग;
        }
        यदि (current > startPos) {
            utext_extract(text, चरमः, current, str, MAX_GRAPHEME_CLSTER_दीर्घता, &स्थितिः);
            यदि (U_FAILURE(स्थितिः)) प्रत्यागम;
            offsets.सङ्कलय_Element(चरमः, स्थितिः);
            indices.सङ्कलय_Element(string_गन्तव्यम्_सूचकांक(str), स्थितिः);
            यदि (U_FAILURE(स्थितिः)) प्रत्यागम;
        }
        चरमः = current;
    }
    यदि (U_FAILURE(स्थितिः) || चरमः >= अन्तः_Pos) {
        प्रत्यागम;
    }
    utext_extract(text, चरमः, अन्तः_Pos, str, MAX_GRAPHEME_CLSTER_दीर्घता, &स्थितिः);
    यदि (U_SUCCESS(स्थितिः)) {
        offsets.सङ्कलय_Element(चरमः, स्थितिः);
        indices.सङ्कलय_Element(string_गन्तव्यम्_सूचकांक(str), स्थितिः);
    }
}

// Computing LSTM as stated in
// https://en.wikipedia.org/wiki/Long_short-term_memory#LSTM_with_a_forget_gate
// ifco is temp array allocate outside which does not need to be
// input/output value but could avoid unnecessary memory alloc/free if passing
// in.
शून्य compute(
    int32_t hunits,
    ध्रुव ReadArray2D& W, ध्रुव ReadArray2D& U, ध्रुव ReadArray1D& b,
    ध्रुव ReadArray1D& x, Array1D& h, Array1D& c,
    Array1D& ifco)
{
    // ifco = x * W + h * U + b
    ifco.assign(b)
        .सङ्कलय_Dot_गुणनफलम्(x, W)
        .सङ्कलय_Dot_गुणनफलम्(h, U);

    ifco.खण्डिका(0*hunits, hunits).sigmoid();  // i: sigmod
    ifco.खण्डिका(1*hunits, hunits).sigmoid(); // f: sigmoid
    ifco.खण्डिका(2*hunits, hunits).tanh(); // c_: tanh
    ifco.खण्डिका(3*hunits, hunits).sigmoid(); // o: sigmod

    c.hadamard_गुणनफलम्(ifco.खण्डिका(hunits, hunits))
        .सङ्कलय_Hadamard_गुणनफलम्(ifco.खण्डिका(0, hunits), ifco.खण्डिका(2*hunits, hunits));

    h.tanh(c)
        .hadamard_गुणनफलम्(ifco.खण्डिका(3*hunits, hunits));
}

// Minimum word size
स्थिर ध्रुव int32_t MIN_WORD = 2;

// Minimum number of characters for two words
स्थिर ध्रुव int32_t MIN_WORD_विस्तृतिः = MIN_WORD * 2;

int32_t
LSTMBreakEngine::divide_ऊर्ध्वम्_Dictionary_Range( UText *text,
                                                int32_t startPos,
                                                int32_t अन्तः_Pos,
                                                UVector32 &प्राप्तम्_Breaks,
                                                UBool /* isPhraseBreaking */,
                                                UError_संहिता& स्थितिः) ध्रुव {
    यदि (U_FAILURE(स्थितिः)) प्रत्यागम 0;
    int32_t begin_प्राप्तम्_Break_आकार = प्राप्तम्_Breaks.size();
    utext_समुच्चयः_Native_सूचकांक(text, startPos);
    utext_moveIndex32(text, MIN_WORD_विस्तृतिः);
    यदि (utext_get_Native_सूचकांक(text) >= अन्तः_Pos) {
        प्रत्यागम 0;       // Not enough characters for two words
    }
    utext_समुच्चयः_Native_सूचकांक(text, startPos);

    UVector32 offsets(स्थितिः);
    UVector32 indices(स्थितिः);
    यदि (U_FAILURE(स्थितिः)) प्रत्यागम 0;
    fVectorizer->vectorize(text, startPos, अन्तः_Pos, offsets, indices, स्थितिः);
    यदि (U_FAILURE(स्थितिः)) प्रत्यागम 0;
    int32_t* offsetsBuf = offsets.get_बफर();
    int32_t* indicesBuf = indices.get_बफर();

    int32_t निवेशः_seq_len = indices.size();
    int32_t hunits = f_दत्त->fForwardU.d1();

    // ----- Begin of all the Array memory allocation needed for this function
    // Allocate temp array used inside compute()
    Array1D ifco(4 * hunits, स्थितिः);

    Array1D c(hunits, स्थितिः);
    Array1D logp(4, स्थितिः);

    // TODO: limit size of hBackward. If input_seq_len is too big, we could
    // run out of memory.
    // Backward LSTM
    Array2D hBackward(निवेशः_seq_len, hunits, स्थितिः);

    // Allocate fbRow and slice the internal array in two.
    Array1D fbRow(2 * hunits, स्थितिः);

    // ----- End of all the Array memory allocation needed for this function
    यदि (U_FAILURE(स्थितिः)) प्रत्यागम 0;

    // To save the needed memory usage, the following is different from the
    // Python or ICU4X implementation. We first perform the Backward LSTM
    // and then merge the iteration of the forward LSTM and the output layer
    // together because we only neetdto remember the h[t-1] for Forward LSTM.
    प्रति (int32_t i = निवेशः_seq_len - 1; i >= 0; i--) {
        Array1D hRow = hBackward.row(i);
        यदि (i != निवेशः_seq_len - 1) {
            hRow.assign(hBackward.row(i+1));
        }
#यदि_परिभाषितम् LSTM_DEBUG
        प्रारूप_लिख("hRow %d\n", i);
        hRow.print();
        प्रारूप_लिख("indicesBuf[%d] = %d\n", i, indicesBuf[i]);
        प्रारूप_लिख("fData->fEmbedding.row(indicesBuf[%d]):\n", i);
        f_दत्त->fEmbedding.row(indicesBuf[i]).print();
#यदि_अन्तः  // LSTM_DEBUG
        compute(hunits,
                f_दत्त->fBackwardW, f_दत्त->fBackwardU, f_दत्त->fBackwardB,
                f_दत्त->fEmbedding.row(indicesBuf[i]),
                hRow, c, ifco);
    }


    Array1D forwardRow = fbRow.खण्डिका(0, hunits);  // point to first half of data in fbRow.
    Array1D backwardRow = fbRow.खण्डिका(hunits, hunits);  // point to second half of data n fbRow.

    // The following iteration merge the forward LSTM and the output layer
    // together.
    c.clear();  // reuse c since it is the same size.
    प्रति (int32_t i = 0; i < निवेशः_seq_len; i++) {
#यदि_परिभाषितम् LSTM_DEBUG
        प्रारूप_लिख("forwardRow %d\n", i);
        forwardRow.print();
#यदि_अन्तः  // LSTM_DEBUG
        // Forward LSTM
        // Calculate the result into forwardRow, which point to the data in the first half
        // of fbRow.
        compute(hunits,
                f_दत्त->fForwardW, f_दत्त->fForwardU, f_दत्त->fForwardB,
                f_दत्त->fEmbedding.row(indicesBuf[i]),
                forwardRow, c, ifco);

        // assign the data from hBackward.row(i) to second half of fbRowa.
        backwardRow.assign(hBackward.row(i));

        logp.assign(f_दत्त->fOutputB).सङ्कलय_Dot_गुणनफलम्(fbRow, f_दत्त->fOutputW);
#यदि_परिभाषितम् LSTM_DEBUG
        प्रारूप_लिख("backwardRow %d\n", i);
        backwardRow.print();
        प्रारूप_लिख("logp %d\n", i);
        logp.print();
#यदि_अन्तः  // LSTM_DEBUG

        // current = argmax(logp)
        LSTMClass current = static_cast<LSTMClass>(logp.max_सूचकांक());
        // BIES logic.
        यदि (current == BEGIN || current == SINGLE) {
            यदि (i != 0) {
                प्राप्तम्_Breaks.सङ्कलय_Element(offsetsBuf[i], स्थितिः);
                यदि (U_FAILURE(स्थितिः)) प्रत्यागम 0;
            }
        }
    }
    प्रत्यागम प्राप्तम्_Breaks.size() - begin_प्राप्तम्_Break_आकार;
}

Vectorizer* createVectorizer(ध्रुव LSTMData* दत्त, UError_संहिता &स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }
    विकल्प (दत्त->fType) {
        पक्ष संहिता_POINTS:
            प्रत्यागम नूतन संहिता_Points_Vectorizer(दत्त->fDict);
            भङ्ग;
        पक्ष GRAPHEME_CLUSTER:
            प्रत्यागम नूतन GraphemeClusterVectorizer(दत्त->fDict);
            भङ्ग;
        स्वभाव:
            भङ्ग;
    }
    UPRV_UNREACHABLE_EXIT;
}

LSTMBreakEngine::LSTMBreakEngine(ध्रुव LSTMData* दत्त, ध्रुव Unicode_समुच्चयः& समुच्चयः, UError_संहिता &स्थितिः)
    : DictionaryBreakEngine(), f_दत्त(दत्त), fVectorizer(createVectorizer(f_दत्त, स्थितिः))
{
    यदि (U_FAILURE(स्थितिः)) {
      f_दत्त = शून्यसूचकः;  // If failure, we should not delete fData in destructor because the caller will do so.
      प्रत्यागम;
    }
    समुच्चयः_Characters(समुच्चयः);
}

LSTMBreakEngine::~LSTMBreakEngine() {
    विलोपय f_दत्त;
    विलोपय fVectorizer;
}

ध्रुव char16_t* LSTMBreakEngine::name() ध्रुव {
    प्रत्यागम f_दत्त->fName;
}

UnicodeString defaultLSTM(UScript_संहिता script, UError_संहिता& स्थितिः) {
    // open root from brkitr tree.
    UResourceBundle *b = ures_open(U_ICUDATA_BRKITR, "", &स्थितिः);
    b = ures_get_By_कुञ्चिका_With_Fallback(b, "lstm", b, &स्थितिः);
    UnicodeString परिणाम = ures_get_Unicode_String_By_कुञ्चिका(b, uscript_getShortName(script), &स्थितिः);
    ures_close(b);
    प्रत्यागम परिणाम;
}

U_CAPI ध्रुव LSTMData* U_EXPORT2 CreateLSTMDataForScript(UScript_संहिता script, UError_संहिता& स्थितिः)
{
    यदि (script != USCRIPT_KHMER && script != USCRIPT_LAO && script != USCRIPT_MYANMAR && script != USCRIPT_THAI) {
        प्रत्यागम शून्यसूचकः;
    }
    UnicodeString name = defaultLSTM(script, स्थितिः);
    यदि (U_FAILURE(स्थितिः)) प्रत्यागम शून्यसूचकः;
    CharString namebuf;
    namebuf.appendInvariantChars(name, स्थितिः).truncate(namebuf.चरमः_सूचकांक_सम्बन्धि('.'));

    LocalUResourceBundlePointer rb(
        ures_open_प्रत्यक्षम्(U_ICUDATA_BRKITR, namebuf.दत्त(), &स्थितिः));
    यदि (U_FAILURE(स्थितिः)) प्रत्यागम शून्यसूचकः;

    प्रत्यागम CreateLSTMData(rb.orphan(), स्थितिः);
}

U_CAPI ध्रुव LSTMData* U_EXPORT2 CreateLSTMData(UResourceBundle* rb, UError_संहिता& स्थितिः)
{
    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }
    ध्रुव LSTMData* परिणाम = नूतन LSTMData(rb, स्थितिः);
    यदि (U_FAILURE(स्थितिः)) {
        विलोपय परिणाम;
        प्रत्यागम शून्यसूचकः;
    }
    प्रत्यागम परिणाम;
}

U_CAPI ध्रुव LanguageBreakEngine* U_EXPORT2
CreateLSTMBreakEngine(UScript_संहिता script, ध्रुव LSTMData* दत्त, UError_संहिता& स्थितिः)
{
    UnicodeString unicode_समुच्चयः_String;
    विकल्प(script) {
        पक्ष USCRIPT_THAI:
            unicode_समुच्चयः_String = UnicodeString(u"[[:Thai:]&[:LineBreak=SA:]]");
            भङ्ग;
        पक्ष USCRIPT_MYANMAR:
            unicode_समुच्चयः_String = UnicodeString(u"[[:Mymr:]&[:LineBreak=SA:]]");
            भङ्ग;
        स्वभाव:
            विलोपय दत्त;
            प्रत्यागम शून्यसूचकः;
    }
    Unicode_समुच्चयः unicode_समुच्चयः;
    unicode_समुच्चयः.apply_आदर्शः(unicode_समुच्चयः_String, स्थितिः);
    ध्रुव LanguageBreakEngine* engine = नूतन LSTMBreakEngine(दत्त, unicode_समुच्चयः, स्थितिः);
    यदि (U_FAILURE(स्थितिः) || engine == शून्यसूचकः) {
        यदि (engine != शून्यसूचकः) {
            विलोपय engine;
        } अन्यथा {
            स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        }
        प्रत्यागम शून्यसूचकः;
    }
    प्रत्यागम engine;
}

U_CAPI शून्य U_EXPORT2 DeleteLSTMData(ध्रुव LSTMData* दत्त)
{
    विलोपय दत्त;
}

U_CAPI ध्रुव char16_t* U_EXPORT2 LSTMDataName(ध्रुव LSTMData* दत्त)
{
    प्रत्यागम दत्त->fName;
}

U_NAMESPACE_अन्तः

#यदि_अन्तः /* #if !UCONFIG_NO_BREAK_ITERATION */
