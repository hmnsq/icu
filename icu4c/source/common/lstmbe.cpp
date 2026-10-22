// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#অন্তর্ভাব <complex>
#অন্তর্ভাব <utility>

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_BREAK_ITERATION

#অন্তর্ভাব "brkeng.h"
#অন্তর্ভাব "charstr.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "lstmbe.h"
#অন্তর্ভাব "putilimp.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "ubrkimpl.h"
#অন্তর্ভাব "uresimp.h"
#অন্তর্ভাব "uvectr32.h"
#অন্তর্ভাব "uvector.h"

#অন্তর্ভাব "unicode/brkiter.h"
#অন্তর্ভাব "unicode/resbund.h"
#অন্তর্ভাব "unicode/ubrk.h"
#অন্তর্ভাব "unicode/uniset.h"
#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "unicode/utf.h"

U_NAMESPACE_BEGIN

// Uncomment the following #define to debug.
// #define LSTM_DEBUG 1
// #define LSTM_VECTORIZER_DEBUG 1

/**
 * Interface for reading 1D array.
 */
শ্রেণী ReadArray1D {
সর্বজনীন:
    আভাসী ~ReadArray1D();
    আভাসী int32_t d1() ধ্রুবক = 0;
    আভাসী দশমিক get(int32_t i) ধ্রুবক = 0;

#ifdef LSTM_DEBUG
    শূন্য print() ধ্রুবক {
        printf("\n[");
        জন্য (int32_t i = 0; i < d1(); i++) {
           printf("%0.8e ", get(i));
           যদি (i % 4 == 3) printf("\n");
        }
        printf("]\n");
    }
#endif
};

ReadArray1D::~ReadArray1D()
{
}

/**
 * Interface for reading 2D array.
 */
শ্রেণী ReadArray2D {
সর্বজনীন:
    আভাসী ~ReadArray2D();
    আভাসী int32_t d1() ধ্রুবক = 0;
    আভাসী int32_t d2() ধ্রুবক = 0;
    আভাসী দশমিক get(int32_t i, int32_t j) ধ্রুবক = 0;
};

ReadArray2D::~ReadArray2D()
{
}

/**
 * A class to index a float array as a 1D Array without owning the pointer or
 * copy the data.
 */
শ্রেণী ConstArray1D : সর্বজনীন ReadArray1D {
সর্বজনীন:
    ConstArray1D() : তথ্য_2(nullptr), d1_(0) {}

    ConstArray1D(ধ্রুবক দশমিক* তথ্য, int32_t d1) : তথ্য_2(তথ্য), d1_(d1) {}

    আভাসী ~ConstArray1D();

    // Init the object, the object does not own the data nor copy.
    // It is designed to directly use data from memory mapped resources.
    শূন্য init(ধ্রুবক int32_t* তথ্য, int32_t d1) {
        U_ASSERT(IEEE_754 == 1);
        তথ্য_2 = reinterpret_cast<ধ্রুবক দশমিক*>(তথ্য);
        d1_ = d1;
    }

    // ReadArray1D methods.
    আভাসী int32_t d1() ধ্রুবক override { ফেরত d1_; }
    আভাসী দশমিক get(int32_t i) ধ্রুবক override {
        U_ASSERT(i < d1_);
        ফেরত তথ্য_2[i];
    }

ব্যক্তিগত:
    ধ্রুবক দশমিক* তথ্য_2;
    int32_t d1_;
};

ConstArray1D::~ConstArray1D()
{
}

/**
 * A class to index a float array as a 2D Array without owning the pointer or
 * copy the data.
 */
শ্রেণী ConstArray2D : সর্বজনীন ReadArray2D {
সর্বজনীন:
    ConstArray2D() : তথ্য_2(nullptr), d1_(0), d2_(0) {}

    ConstArray2D(ধ্রুবক দশমিক* তথ্য, int32_t d1, int32_t d2)
        : তথ্য_2(তথ্য), d1_(d1), d2_(d2) {}

    আভাসী ~ConstArray2D();

    // Init the object, the object does not own the data nor copy.
    // It is designed to directly use data from memory mapped resources.
    শূন্য init(ধ্রুবক int32_t* তথ্য, int32_t d1, int32_t d2) {
        U_ASSERT(IEEE_754 == 1);
        তথ্য_2 = reinterpret_cast<ধ্রুবক দশমিক*>(তথ্য);
        d1_ = d1;
        d2_ = d2;
    }

    // ReadArray2D methods.
    অন্তঃরেখ int32_t d1() ধ্রুবক override { ফেরত d1_; }
    অন্তঃরেখ int32_t d2() ধ্রুবক override { ফেরত d2_; }
    দশমিক get(int32_t i, int32_t j) ধ্রুবক override {
        U_ASSERT(i < d1_);
        U_ASSERT(j < d2_);
        ফেরত তথ্য_2[i * d2_ + j];
    }

    // Expose the ith row as a ConstArray1D
    অন্তঃরেখ ConstArray1D row(int32_t i) ধ্রুবক {
        U_ASSERT(i < d1_);
        ফেরত ConstArray1D(তথ্য_2 + i * d2_, d2_);
    }

ব্যক্তিগত:
    ধ্রুবক দশমিক* তথ্য_2;
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
শ্রেণী Array1D : সর্বজনীন ReadArray1D {
সর্বজনীন:
    Array1D() : memory_(nullptr), তথ্য_2(nullptr), d1_(0) {}
    Array1D(int32_t d1, UErrorCode &status)
        : memory_(uprv_malloc(d1 * আকার(দশমিক))),
          তথ্য_2(static_cast<দশমিক*>(memory_)), d1_(d1) {
        যদি (U_SUCCESS(status)) {
            যদি (memory_ == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                ফেরত;
            }
            clear();
        }
    }

    আভাসী ~Array1D();

    // A special constructor which does not own the memory but writeable
    // as a slice of an array.
    Array1D(দশমিক* তথ্য, int32_t d1)
        : memory_(nullptr), তথ্য_2(তথ্য), d1_(d1) {}

    // ReadArray1D methods.
    আভাসী int32_t d1() ধ্রুবক override { ফেরত d1_; }
    আভাসী দশমিক get(int32_t i) ধ্রুবক override {
        U_ASSERT(i < d1_);
        ফেরত তথ্য_2[i];
    }

    // Return the index which point to the max data in the array.
    অন্তঃরেখ int32_t max_সূচক() ধ্রুবক {
        int32_t সূচক = 0;
        দশমিক max = তথ্য_2[0];
        জন্য (int32_t i = 1; i < d1_; i++) {
            যদি (তথ্য_2[i] > max) {
                max = তথ্য_2[i];
                সূচক = i;
            }
        }
        ফেরত সূচক;
    }

    // Slice part of the array to a new one.
    অন্তঃরেখ Array1D slice(int32_t from, int32_t size) ধ্রুবক {
        U_ASSERT(from >= 0);
        U_ASSERT(from < d1_);
        U_ASSERT(from + size <= d1_);
        ফেরত Array1D(তথ্য_2 + from, size);
    }

    // Add dot product of a 1D array and a 2D array into this one.
    অন্তঃরেখ Array1D& addDotProduct(ধ্রুবক ReadArray1D& a, ধ্রুবক ReadArray2D& b) {
        U_ASSERT(a.d1() == b.d1());
        U_ASSERT(b.d2() == d1());
        জন্য (int32_t i = 0; i < d1(); i++) {
            জন্য (int32_t j = 0; j < a.d1(); j++) {
                তথ্য_2[i] += a.get(j) * b.get(j, i);
            }
        }
        ফেরত *এই;
    }

    // Hadamard Product the values of another array of the same size into this one.
    অন্তঃরেখ Array1D& hadamardProduct(ধ্রুবক ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        জন্য (int32_t i = 0; i < d1(); i++) {
            তথ্য_2[i] *= a.get(i);
        }
        ফেরত *এই;
    }

    // Add the Hadamard Product of two arrays of the same size into this one.
    অন্তঃরেখ Array1D& addHadamardProduct(ধ্রুবক ReadArray1D& a, ধ্রুবক ReadArray1D& b) {
        U_ASSERT(a.d1() == d1());
        U_ASSERT(b.d1() == d1());
        জন্য (int32_t i = 0; i < d1(); i++) {
            তথ্য_2[i] += a.get(i) * b.get(i);
        }
        ফেরত *এই;
    }

    // Add the values of another array of the same size into this one.
    অন্তঃরেখ Array1D& add(ধ্রুবক ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        জন্য (int32_t i = 0; i < d1(); i++) {
            তথ্য_2[i] += a.get(i);
        }
        ফেরত *এই;
    }

    // Assign the values of another array of the same size into this one.
    অন্তঃরেখ Array1D& assign(ধ্রুবক ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        জন্য (int32_t i = 0; i < d1(); i++) {
            তথ্য_2[i] = a.get(i);
        }
        ফেরত *এই;
    }

    // Apply tanh to all the elements in the array.
    অন্তঃরেখ Array1D& tanh() {
        ফেরত tanh(*এই);
    }

    // Apply tanh of a and store into this array.
    অন্তঃরেখ Array1D& tanh(ধ্রুবক Array1D& a) {
        U_ASSERT(a.d1() == d1());
        জন্য (int32_t i = 0; i < d1_; i++) {
            তথ্য_2[i] = std::tanh(a.get(i));
        }
        ফেরত *এই;
    }

    // Apply sigmoid to all the elements in the array.
    অন্তঃরেখ Array1D& sigmoid() {
        জন্য (int32_t i = 0; i < d1_; i++) {
            তথ্য_2[i] = 1.0f/(1.0f + expf(-তথ্য_2[i]));
        }
        ফেরত *এই;
    }

    অন্তঃরেখ Array1D& clear() {
        uprv_memset(তথ্য_2, 0, d1_ * আকার(দশমিক));
        ফেরত *এই;
    }

ব্যক্তিগত:
    শূন্য* memory_;
    দশমিক* তথ্য_2;
    int32_t d1_;
};

Array1D::~Array1D()
{
    uprv_free(memory_);
}

শ্রেণী Array2D : সর্বজনীন ReadArray2D {
সর্বজনীন:
    Array2D() : memory_(nullptr), তথ্য_2(nullptr), d1_(0), d2_(0) {}
    Array2D(int32_t d1, int32_t d2, UErrorCode &status)
        : memory_(uprv_malloc(d1 * d2 * আকার(দশমিক))),
          তথ্য_2(static_cast<দশমিক*>(memory_)), d1_(d1), d2_(d2) {
        যদি (U_SUCCESS(status)) {
            যদি (memory_ == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                ফেরত;
            }
            clear();
        }
    }
    আভাসী ~Array2D();

    // ReadArray2D methods.
    আভাসী int32_t d1() ধ্রুবক override { ফেরত d1_; }
    আভাসী int32_t d2() ধ্রুবক override { ফেরত d2_; }
    আভাসী দশমিক get(int32_t i, int32_t j) ধ্রুবক override {
        U_ASSERT(i < d1_);
        U_ASSERT(j < d2_);
        ফেরত তথ্য_2[i * d2_ + j];
    }

    অন্তঃরেখ Array1D row(int32_t i) ধ্রুবক {
        U_ASSERT(i < d1_);
        ফেরত Array1D(তথ্য_2 + i * d2_, d2_);
    }

    অন্তঃরেখ Array2D& clear() {
        uprv_memset(তথ্য_2, 0, d1_ * d2_ * আকার(দশমিক));
        ফেরত *এই;
    }

ব্যক্তিগত:
    শূন্য* memory_;
    দশমিক* তথ্য_2;
    int32_t d1_;
    int32_t d2_;
};

Array2D::~Array2D()
{
    uprv_free(memory_);
}

প্রকারলেখ গণনা {
    BEGIN,
    INSIDE,
    END,
    SINGLE
} LSTMClass;

প্রকারলেখ গণনা {
    UNKNOWN,
    CODE_POINTS,
    GRAPHEME_CLUSTER,
} EmbeddingType;

কাঠামো LSTMData : সর্বজনীন UMemory {
    LSTMData(UResourceBundle* rb, UErrorCode &status);
    ~LSTMData();
    UHashtable* fDict;
    EmbeddingType fType;
    ধ্রুবক char16_t* fName;
    ConstArray2D fEmbedding;
    ConstArray2D fForwardW;
    ConstArray2D fForwardU;
    ConstArray1D fForwardB;
    ConstArray2D fBackwardW;
    ConstArray2D fBackwardU;
    ConstArray1D fBackwardB;
    ConstArray2D fOutputW;
    ConstArray1D fOutputB;

ব্যক্তিগত:
    UResourceBundle* fBundle;
};

LSTMData::LSTMData(UResourceBundle* rb, UErrorCode &status)
    : fDict(nullptr), fType(UNKNOWN), fName(nullptr),
      fBundle(rb)
{
    যদি (U_FAILURE(status)) {
        ফেরত;
    }
    যদি (IEEE_754 != 1) {
        status = U_UNSUPPORTED_ERROR;
        ফেরত;
    }
    LocalUResourceBundlePointer embeddings_res(
        ures_get_By_চাবি(rb, "embeddings", nullptr, &status));
    int32_t embedding_আকার = ures_getInt(embeddings_res.getAlias(), &status);
    LocalUResourceBundlePointer hunits_res(
        ures_get_By_চাবি(rb, "hunits", nullptr, &status));
    যদি (U_FAILURE(status)) ফেরত;
    int32_t hunits = ures_getInt(hunits_res.getAlias(), &status);
    ধ্রুবক char16_t* type = ures_get_String_By_চাবি(rb, "type", nullptr, &status);
    যদি (U_FAILURE(status)) ফেরত;
    যদি (u_strCompare(type, -1, u"codepoints", -1, মিথ্যা) == 0) {
        fType = CODE_POINTS;
    } অন্যথা যদি (u_strCompare(type, -1, u"graphclust", -1, মিথ্যা) == 0) {
        fType = GRAPHEME_CLUSTER;
    }
    fName = ures_get_String_By_চাবি(rb, "model", nullptr, &status);
    LocalUResourceBundlePointer তথ্য_Res(ures_get_By_চাবি(rb, "data", nullptr, &status));
    যদি (U_FAILURE(status)) ফেরত;
    int32_t তথ্য_len = 0;
    ধ্রুবক int32_t* তথ্য = ures_getIntVector(তথ্য_Res.getAlias(), &তথ্য_len, &status);
    fDict = uhash_open(uhash_hashUChars, uhash_compareUChars, nullptr, &status);

    স্তূপ_UResource_Bundle স্তূপ_অস্থায়ী_Bundle;
    Resource_তথ্য_মান value;
    ures_get_মান_With_Fallback(rb, "dict", স্তূপ_অস্থায়ী_Bundle.getAlias(), value, status);
    ResourceArray stringArray = value.getArray(status);
    int32_t অঙ্ক_সূচক = stringArray.get_আকার();
    যদি (U_FAILURE(status)) { ফেরত; }

    // put dict into hash
    int32_t string_দৈর্ঘ্য;
    জন্য (int32_t idx = 0; idx < অঙ্ক_সূচক; idx++) {
        stringArray.get_মান(idx, value);
        ধ্রুবক char16_t* str = value.getString(string_দৈর্ঘ্য, status);
        uhash_putiAllowZero(fDict, (শূন্য*)str, idx, &status);
        যদি (U_FAILURE(status)) ফেরত;
#ifdef LSTM_VECTORIZER_DEBUG
        printf("Assign [");
        যখন (*str != 0x0000) {
            printf("U+%04x ", *str);
            str++;
        }
        printf("] map to %d\n", idx-1);
#endif
    }
    int32_t mat1_আকার = (অঙ্ক_সূচক + 1) * embedding_আকার;
    int32_t mat2_আকার = embedding_আকার * 4 * hunits;
    int32_t mat3_আকার = hunits * 4 * hunits;
    int32_t mat4_আকার = 4 * hunits;
    int32_t mat5_আকার = mat2_আকার;
    int32_t mat6_আকার = mat3_আকার;
    int32_t mat7_আকার = mat4_আকার;
    int32_t mat8_আকার = 2 * hunits * 4;
#যদি U_DEBUG
    int32_t mat9_আকার = 4;
    U_ASSERT(তথ্য_len == mat1_আকার + mat2_আকার + mat3_আকার + mat4_আকার + mat5_আকার +
        mat6_আকার + mat7_আকার + mat8_আকার + mat9_আকার);
#endif

    fEmbedding.init(তথ্য, (অঙ্ক_সূচক + 1), embedding_আকার);
    তথ্য += mat1_আকার;
    fForwardW.init(তথ্য, embedding_আকার, 4 * hunits);
    তথ্য += mat2_আকার;
    fForwardU.init(তথ্য, hunits, 4 * hunits);
    তথ্য += mat3_আকার;
    fForwardB.init(তথ্য, 4 * hunits);
    তথ্য += mat4_আকার;
    fBackwardW.init(তথ্য, embedding_আকার, 4 * hunits);
    তথ্য += mat5_আকার;
    fBackwardU.init(তথ্য, hunits, 4 * hunits);
    তথ্য += mat6_আকার;
    fBackwardB.init(তথ্য, 4 * hunits);
    তথ্য += mat7_আকার;
    fOutputW.init(তথ্য, 2 * hunits, 4);
    তথ্য += mat8_আকার;
    fOutputB.init(তথ্য, 4);
}

LSTMData::~LSTMData() {
    uhash_close(fDict);
    ures_close(fBundle);
}

শ্রেণী Vectorizer : সর্বজনীন UMemory {
সর্বজনীন:
    Vectorizer(UHashtable* dict) : fDict(dict) {}
    আভাসী ~Vectorizer();
    আভাসী শূন্য vectorize(UText *text, int32_t startPos, int32_t endPos,
                           UVector32 &offsets, UVector32 &indices,
                           UErrorCode &status) ধ্রুবক = 0;
সুরক্ষিত:
    int32_t string_To_সূচক(ধ্রুবক char16_t* str) ধ্রুবক {
        UBool found = মিথ্যা;
        int32_t ret = uhash_getiAndFound(fDict, (ধ্রুবক শূন্য*)str, &found);
        যদি (!found) {
            ret = fDict->count;
        }
#ifdef LSTM_VECTORIZER_DEBUG
        printf("[");
        যখন (*str != 0x0000) {
            printf("U+%04x ", *str);
            str++;
        }
        printf("] map to %d\n", ret);
#endif
        ফেরত ret;
    }

ব্যক্তিগত:
    UHashtable* fDict;
};

Vectorizer::~Vectorizer()
{
}

শ্রেণী CodePointsVectorizer : সর্বজনীন Vectorizer {
সর্বজনীন:
    CodePointsVectorizer(UHashtable* dict) : Vectorizer(dict) {}
    আভাসী ~CodePointsVectorizer();
    আভাসী শূন্য vectorize(UText *text, int32_t startPos, int32_t endPos,
                           UVector32 &offsets, UVector32 &indices,
                           UErrorCode &status) ধ্রুবক override;
};

CodePointsVectorizer::~CodePointsVectorizer()
{
}

শূন্য CodePointsVectorizer::vectorize(
    UText *text, int32_t startPos, int32_t endPos,
    UVector32 &offsets, UVector32 &indices, UErrorCode &status) ধ্রুবক
{
    যদি (offsets.ensureCapacity(endPos - startPos, status) &&
            indices.ensureCapacity(endPos - startPos, status)) {
        যদি (U_FAILURE(status)) ফেরত;
        utext_set_Native_সূচক(text, startPos);
        int32_t current;
        char16_t str[2] = {0, 0};
        যখন (U_SUCCESS(status) &&
               (current = static_cast<int32_t>(utext_get_Native_সূচক(text))) < endPos) {
            // Since the LSTMBreakEngine is currently only accept chars in BMP,
            // we can ignore the possibility of hitting supplementary code
            // point.
            str[0] = static_cast<char16_t>(utext_next32(text));
            U_ASSERT(!U_IS_SURROGATE(str[0]));
            offsets.addElement(current, status);
            indices.addElement(string_To_সূচক(str), status);
        }
    }
}

শ্রেণী GraphemeClusterVectorizer : সর্বজনীন Vectorizer {
সর্বজনীন:
    GraphemeClusterVectorizer(UHashtable* dict)
        : Vectorizer(dict)
    {
    }
    আভাসী ~GraphemeClusterVectorizer();
    আভাসী শূন্য vectorize(UText *text, int32_t startPos, int32_t endPos,
                           UVector32 &offsets, UVector32 &indices,
                           UErrorCode &status) ধ্রুবক override;
};

GraphemeClusterVectorizer::~GraphemeClusterVectorizer()
{
}

constexpr int32_t MAX_GRAPHEME_CLSTER_দৈর্ঘ্য = 10;

শূন্য GraphemeClusterVectorizer::vectorize(
    UText *text, int32_t startPos, int32_t endPos,
    UVector32 &offsets, UVector32 &indices, UErrorCode &status) ধ্রুবক
{
    যদি (U_FAILURE(status)) ফেরত;
    যদি (!offsets.ensureCapacity(endPos - startPos, status) ||
            !indices.ensureCapacity(endPos - startPos, status)) {
        ফেরত;
    }
    যদি (U_FAILURE(status)) ফেরত;
    LocalPointer<BreakIterator> graphemeIter(BreakIterator::createCharacterInstance(Locale(), status));
    যদি (U_FAILURE(status)) ফেরত;
    graphemeIter->setText(text, status);
    যদি (U_FAILURE(status)) ফেরত;

    যদি (startPos != 0) {
        graphemeIter->preceding(startPos);
    }
    int32_t last = startPos;
    int32_t current = startPos;
    char16_t str[MAX_GRAPHEME_CLSTER_দৈর্ঘ্য];
    যখন ((current = graphemeIter->next()) != BreakIterator::DONE) {
        যদি (current >= endPos) {
            বিরতি;
        }
        যদি (current > startPos) {
            utext_extract(text, last, current, str, MAX_GRAPHEME_CLSTER_দৈর্ঘ্য, &status);
            যদি (U_FAILURE(status)) ফেরত;
            offsets.addElement(last, status);
            indices.addElement(string_To_সূচক(str), status);
            যদি (U_FAILURE(status)) ফেরত;
        }
        last = current;
    }
    যদি (U_FAILURE(status) || last >= endPos) {
        ফেরত;
    }
    utext_extract(text, last, endPos, str, MAX_GRAPHEME_CLSTER_দৈর্ঘ্য, &status);
    যদি (U_SUCCESS(status)) {
        offsets.addElement(last, status);
        indices.addElement(string_To_সূচক(str), status);
    }
}

// Computing LSTM as stated in
// https://en.wikipedia.org/wiki/Long_short-term_memory#LSTM_with_a_forget_gate
// ifco is temp array allocate outside which does not need to be
// input/output value but could avoid unnecessary memory alloc/free if passing
// in.
শূন্য compute(
    int32_t hunits,
    ধ্রুবক ReadArray2D& W, ধ্রুবক ReadArray2D& U, ধ্রুবক ReadArray1D& b,
    ধ্রুবক ReadArray1D& x, Array1D& h, Array1D& c,
    Array1D& ifco)
{
    // ifco = x * W + h * U + b
    ifco.assign(b)
        .addDotProduct(x, W)
        .addDotProduct(h, U);

    ifco.slice(0*hunits, hunits).sigmoid();  // i: sigmod
    ifco.slice(1*hunits, hunits).sigmoid(); // f: sigmoid
    ifco.slice(2*hunits, hunits).tanh(); // c_: tanh
    ifco.slice(3*hunits, hunits).sigmoid(); // o: sigmod

    c.hadamardProduct(ifco.slice(hunits, hunits))
        .addHadamardProduct(ifco.slice(0, hunits), ifco.slice(2*hunits, hunits));

    h.tanh(c)
        .hadamardProduct(ifco.slice(3*hunits, hunits));
}

// Minimum word size
স্থির ধ্রুবক int32_t MIN_WORD = 2;

// Minimum number of characters for two words
স্থির ধ্রুবক int32_t MIN_WORD_SPAN = MIN_WORD * 2;

int32_t
LSTMBreakEngine::divideUpDictionaryRange( UText *text,
                                                int32_t startPos,
                                                int32_t endPos,
                                                UVector32 &foundBreaks,
                                                UBool /* isPhraseBreaking */,
                                                UErrorCode& status) ধ্রুবক {
    যদি (U_FAILURE(status)) ফেরত 0;
    int32_t begin_Found_Break_আকার = foundBreaks.size();
    utext_set_Native_সূচক(text, startPos);
    utext_moveIndex32(text, MIN_WORD_SPAN);
    যদি (utext_get_Native_সূচক(text) >= endPos) {
        ফেরত 0;       // Not enough characters for two words
    }
    utext_set_Native_সূচক(text, startPos);

    UVector32 offsets(status);
    UVector32 indices(status);
    যদি (U_FAILURE(status)) ফেরত 0;
    fVectorizer->vectorize(text, startPos, endPos, offsets, indices, status);
    যদি (U_FAILURE(status)) ফেরত 0;
    int32_t* offsetsBuf = offsets.get_বাফার();
    int32_t* indicesBuf = indices.get_বাফার();

    int32_t প্রবেশ_seq_len = indices.size();
    int32_t hunits = f_তথ্য->fForwardU.d1();

    // ----- Begin of all the Array memory allocation needed for this function
    // Allocate temp array used inside compute()
    Array1D ifco(4 * hunits, status);

    Array1D c(hunits, status);
    Array1D logp(4, status);

    // TODO: limit size of hBackward. If input_seq_len is too big, we could
    // run out of memory.
    // Backward LSTM
    Array2D hBackward(প্রবেশ_seq_len, hunits, status);

    // Allocate fbRow and slice the internal array in two.
    Array1D fbRow(2 * hunits, status);

    // ----- End of all the Array memory allocation needed for this function
    যদি (U_FAILURE(status)) ফেরত 0;

    // To save the needed memory usage, the following is different from the
    // Python or ICU4X implementation. We first perform the Backward LSTM
    // and then merge the iteration of the forward LSTM and the output layer
    // together because we only neetdto remember the h[t-1] for Forward LSTM.
    জন্য (int32_t i = প্রবেশ_seq_len - 1; i >= 0; i--) {
        Array1D hRow = hBackward.row(i);
        যদি (i != প্রবেশ_seq_len - 1) {
            hRow.assign(hBackward.row(i+1));
        }
#ifdef LSTM_DEBUG
        printf("hRow %d\n", i);
        hRow.print();
        printf("indicesBuf[%d] = %d\n", i, indicesBuf[i]);
        printf("fData->fEmbedding.row(indicesBuf[%d]):\n", i);
        f_তথ্য->fEmbedding.row(indicesBuf[i]).print();
#endif  // LSTM_DEBUG
        compute(hunits,
                f_তথ্য->fBackwardW, f_তথ্য->fBackwardU, f_তথ্য->fBackwardB,
                f_তথ্য->fEmbedding.row(indicesBuf[i]),
                hRow, c, ifco);
    }


    Array1D forwardRow = fbRow.slice(0, hunits);  // point to first half of data in fbRow.
    Array1D backwardRow = fbRow.slice(hunits, hunits);  // point to second half of data n fbRow.

    // The following iteration merge the forward LSTM and the output layer
    // together.
    c.clear();  // reuse c since it is the same size.
    জন্য (int32_t i = 0; i < প্রবেশ_seq_len; i++) {
#ifdef LSTM_DEBUG
        printf("forwardRow %d\n", i);
        forwardRow.print();
#endif  // LSTM_DEBUG
        // Forward LSTM
        // Calculate the result into forwardRow, which point to the data in the first half
        // of fbRow.
        compute(hunits,
                f_তথ্য->fForwardW, f_তথ্য->fForwardU, f_তথ্য->fForwardB,
                f_তথ্য->fEmbedding.row(indicesBuf[i]),
                forwardRow, c, ifco);

        // assign the data from hBackward.row(i) to second half of fbRowa.
        backwardRow.assign(hBackward.row(i));

        logp.assign(f_তথ্য->fOutputB).addDotProduct(fbRow, f_তথ্য->fOutputW);
#ifdef LSTM_DEBUG
        printf("backwardRow %d\n", i);
        backwardRow.print();
        printf("logp %d\n", i);
        logp.print();
#endif  // LSTM_DEBUG

        // current = argmax(logp)
        LSTMClass current = static_cast<LSTMClass>(logp.max_সূচক());
        // BIES logic.
        যদি (current == BEGIN || current == SINGLE) {
            যদি (i != 0) {
                foundBreaks.addElement(offsetsBuf[i], status);
                যদি (U_FAILURE(status)) ফেরত 0;
            }
        }
    }
    ফেরত foundBreaks.size() - begin_Found_Break_আকার;
}

Vectorizer* createVectorizer(ধ্রুবক LSTMData* তথ্য, UErrorCode &status) {
    যদি (U_FAILURE(status)) {
        ফেরত nullptr;
    }
    নির্বাচন (তথ্য->fType) {
        ক্ষেত্র CODE_POINTS:
            ফেরত নতুন CodePointsVectorizer(তথ্য->fDict);
            বিরতি;
        ক্ষেত্র GRAPHEME_CLUSTER:
            ফেরত নতুন GraphemeClusterVectorizer(তথ্য->fDict);
            বিরতি;
        পূর্বনির্ধারিত:
            বিরতি;
    }
    UPRV_UNREACHABLE_EXIT;
}

LSTMBreakEngine::LSTMBreakEngine(ধ্রুবক LSTMData* তথ্য, ধ্রুবক UnicodeSet& set, UErrorCode &status)
    : DictionaryBreakEngine(), f_তথ্য(তথ্য), fVectorizer(createVectorizer(f_তথ্য, status))
{
    যদি (U_FAILURE(status)) {
      f_তথ্য = nullptr;  // If failure, we should not delete fData in destructor because the caller will do so.
      ফেরত;
    }
    setCharacters(set);
}

LSTMBreakEngine::~LSTMBreakEngine() {
    delete f_তথ্য;
    delete fVectorizer;
}

ধ্রুবক char16_t* LSTMBreakEngine::name() ধ্রুবক {
    ফেরত f_তথ্য->fName;
}

UnicodeString defaultLSTM(UScriptCode script, UErrorCode& status) {
    // open root from brkitr tree.
    UResourceBundle *b = ures_open(U_ICUDATA_BRKITR, "", &status);
    b = ures_get_By_চাবি_With_Fallback(b, "lstm", b, &status);
    UnicodeString ফলাফল = ures_get_Unicode_String_By_চাবি(b, uscript_getShortName(script), &status);
    ures_close(b);
    ফেরত ফলাফল;
}

U_CAPI ধ্রুবক LSTMData* U_EXPORT2 CreateLSTMDataForScript(UScriptCode script, UErrorCode& status)
{
    যদি (script != USCRIPT_KHMER && script != USCRIPT_LAO && script != USCRIPT_MYANMAR && script != USCRIPT_THAI) {
        ফেরত nullptr;
    }
    UnicodeString name = defaultLSTM(script, status);
    যদি (U_FAILURE(status)) ফেরত nullptr;
    CharString namebuf;
    namebuf.appendInvariantChars(name, status).truncate(namebuf.last_সূচক_Of('.'));

    LocalUResourceBundlePointer rb(
        ures_openDirect(U_ICUDATA_BRKITR, namebuf.তথ্য(), &status));
    যদি (U_FAILURE(status)) ফেরত nullptr;

    ফেরত CreateLSTMData(rb.orphan(), status);
}

U_CAPI ধ্রুবক LSTMData* U_EXPORT2 CreateLSTMData(UResourceBundle* rb, UErrorCode& status)
{
    যদি (U_FAILURE(status)) {
        ফেরত nullptr;
    }
    ধ্রুবক LSTMData* ফলাফল = নতুন LSTMData(rb, status);
    যদি (U_FAILURE(status)) {
        delete ফলাফল;
        ফেরত nullptr;
    }
    ফেরত ফলাফল;
}

U_CAPI ধ্রুবক LanguageBreakEngine* U_EXPORT2
CreateLSTMBreakEngine(UScriptCode script, ধ্রুবক LSTMData* তথ্য, UErrorCode& status)
{
    UnicodeString unicodeSetString;
    নির্বাচন(script) {
        ক্ষেত্র USCRIPT_THAI:
            unicodeSetString = UnicodeString(u"[[:Thai:]&[:LineBreak=SA:]]");
            বিরতি;
        ক্ষেত্র USCRIPT_MYANMAR:
            unicodeSetString = UnicodeString(u"[[:Mymr:]&[:LineBreak=SA:]]");
            বিরতি;
        পূর্বনির্ধারিত:
            delete তথ্য;
            ফেরত nullptr;
    }
    UnicodeSet unicodeSet;
    unicodeSet.applyPattern(unicodeSetString, status);
    ধ্রুবক LanguageBreakEngine* engine = নতুন LSTMBreakEngine(তথ্য, unicodeSet, status);
    যদি (U_FAILURE(status) || engine == nullptr) {
        যদি (engine != nullptr) {
            delete engine;
        } অন্যথা {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        ফেরত nullptr;
    }
    ফেরত engine;
}

U_CAPI শূন্য U_EXPORT2 DeleteLSTMData(ধ্রুবক LSTMData* তথ্য)
{
    delete তথ্য;
}

U_CAPI ধ্রুবক char16_t* U_EXPORT2 LSTMDataName(ধ্রুবক LSTMData* তথ্য)
{
    ফেরত তথ্য->fName;
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
