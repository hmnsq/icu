// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#समावेश <complex>
#समावेश <utility>

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_BREAK_ITERATION

#समावेश "brkeng.h"
#समावेश "charstr.h"
#समावेश "cmemory.h"
#समावेश "lstmbe.h"
#समावेश "putilimp.h"
#समावेश "uassert.h"
#समावेश "ubrkimpl.h"
#समावेश "uresimp.h"
#समावेश "uvectr32.h"
#समावेश "uvector.h"

#समावेश "unicode/brkiter.h"
#समावेश "unicode/resbund.h"
#समावेश "unicode/ubrk.h"
#समावेश "unicode/uniset.h"
#समावेश "unicode/ustring.h"
#समावेश "unicode/utf.h"

U_NAMESPACE_BEGIN

// Uncomment the following #define to debug.
// #define LSTM_DEBUG 1
// #define LSTM_VECTORIZER_DEBUG 1

/**
 * Interface for reading 1D array.
 */
श्रेणी ReadArray1D {
खुला:
    भव ~ReadArray1D();
    भव int32_t d1() स्थिर = 0;
    भव भग्न लो(int32_t i) स्थिर = 0;

#अगर_घोषित LSTM_DEBUG
    व्योम print() स्थिर {
        म_लिखो("\n[");
        क्रम (int32_t i = 0; i < d1(); i++) {
           म_लिखो("%0.8e ", लो(i));
           अगर (i % 4 == 3) म_लिखो("\n");
        }
        म_लिखो("]\n");
    }
#पूर्ण_अगर
};

ReadArray1D::~ReadArray1D()
{
}

/**
 * Interface for reading 2D array.
 */
श्रेणी ReadArray2D {
खुला:
    भव ~ReadArray2D();
    भव int32_t d1() स्थिर = 0;
    भव int32_t d2() स्थिर = 0;
    भव भग्न लो(int32_t i, int32_t j) स्थिर = 0;
};

ReadArray2D::~ReadArray2D()
{
}

/**
 * A class to index a float array as a 1D Array without owning the pointer or
 * copy the data.
 */
श्रेणी ConstArray1D : खुला ReadArray1D {
खुला:
    ConstArray1D() : तथ्य_2(nullptr), d1_(0) {}

    ConstArray1D(स्थिर भग्न* तथ्य, int32_t d1) : तथ्य_2(तथ्य), d1_(d1) {}

    भव ~ConstArray1D();

    // Init the object, the object does not own the data nor copy.
    // It is designed to directly use data from memory mapped resources.
    व्योम init(स्थिर int32_t* तथ्य, int32_t d1) {
        U_ASSERT(IEEE_754 == 1);
        तथ्य_2 = reinterpret_cast<स्थिर भग्न*>(तथ्य);
        d1_ = d1;
    }

    // ReadArray1D methods.
    भव int32_t d1() स्थिर override { वापस d1_; }
    भव भग्न लो(int32_t i) स्थिर override {
        U_ASSERT(i < d1_);
        वापस तथ्य_2[i];
    }

निजी:
    स्थिर भग्न* तथ्य_2;
    int32_t d1_;
};

ConstArray1D::~ConstArray1D()
{
}

/**
 * A class to index a float array as a 2D Array without owning the pointer or
 * copy the data.
 */
श्रेणी ConstArray2D : खुला ReadArray2D {
खुला:
    ConstArray2D() : तथ्य_2(nullptr), d1_(0), d2_(0) {}

    ConstArray2D(स्थिर भग्न* तथ्य, int32_t d1, int32_t d2)
        : तथ्य_2(तथ्य), d1_(d1), d2_(d2) {}

    भव ~ConstArray2D();

    // Init the object, the object does not own the data nor copy.
    // It is designed to directly use data from memory mapped resources.
    व्योम init(स्थिर int32_t* तथ्य, int32_t d1, int32_t d2) {
        U_ASSERT(IEEE_754 == 1);
        तथ्य_2 = reinterpret_cast<स्थिर भग्न*>(तथ्य);
        d1_ = d1;
        d2_ = d2;
    }

    // ReadArray2D methods.
    अंतरभूत int32_t d1() स्थिर override { वापस d1_; }
    अंतरभूत int32_t d2() स्थिर override { वापस d2_; }
    भग्न लो(int32_t i, int32_t j) स्थिर override {
        U_ASSERT(i < d1_);
        U_ASSERT(j < d2_);
        वापस तथ्य_2[i * d2_ + j];
    }

    // Expose the ith row as a ConstArray1D
    अंतरभूत ConstArray1D row(int32_t i) स्थिर {
        U_ASSERT(i < d1_);
        वापस ConstArray1D(तथ्य_2 + i * d2_, d2_);
    }

निजी:
    स्थिर भग्न* तथ्य_2;
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
श्रेणी Array1D : खुला ReadArray1D {
खुला:
    Array1D() : memory_(nullptr), तथ्य_2(nullptr), d1_(0) {}
    Array1D(int32_t d1, UErrorCode &status)
        : memory_(uprv_malloc(d1 * माप(भग्न))),
          तथ्य_2(static_cast<भग्न*>(memory_)), d1_(d1) {
        अगर (U_SUCCESS(status)) {
            अगर (memory_ == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                वापस;
            }
            सफा();
        }
    }

    भव ~Array1D();

    // A special constructor which does not own the memory but writeable
    // as a slice of an array.
    Array1D(भग्न* तथ्य, int32_t d1)
        : memory_(nullptr), तथ्य_2(तथ्य), d1_(d1) {}

    // ReadArray1D methods.
    भव int32_t d1() स्थिर override { वापस d1_; }
    भव भग्न लो(int32_t i) स्थिर override {
        U_ASSERT(i < d1_);
        वापस तथ्य_2[i];
    }

    // Return the index which point to the max data in the array.
    अंतरभूत int32_t max_सूचकांक() स्थिर {
        int32_t सूचकांक = 0;
        भग्न max = तथ्य_2[0];
        क्रम (int32_t i = 1; i < d1_; i++) {
            अगर (तथ्य_2[i] > max) {
                max = तथ्य_2[i];
                सूचकांक = i;
            }
        }
        वापस सूचकांक;
    }

    // Slice part of the array to a new one.
    अंतरभूत Array1D slice(int32_t from, int32_t size) स्थिर {
        U_ASSERT(from >= 0);
        U_ASSERT(from < d1_);
        U_ASSERT(from + size <= d1_);
        वापस Array1D(तथ्य_2 + from, size);
    }

    // Add dot product of a 1D array and a 2D array into this one.
    अंतरभूत Array1D& addDotProduct(स्थिर ReadArray1D& a, स्थिर ReadArray2D& b) {
        U_ASSERT(a.d1() == b.d1());
        U_ASSERT(b.d2() == d1());
        क्रम (int32_t i = 0; i < d1(); i++) {
            क्रम (int32_t j = 0; j < a.d1(); j++) {
                तथ्य_2[i] += a.लो(j) * b.लो(j, i);
            }
        }
        वापस *यह;
    }

    // Hadamard Product the values of another array of the same size into this one.
    अंतरभूत Array1D& hadamardProduct(स्थिर ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        क्रम (int32_t i = 0; i < d1(); i++) {
            तथ्य_2[i] *= a.लो(i);
        }
        वापस *यह;
    }

    // Add the Hadamard Product of two arrays of the same size into this one.
    अंतरभूत Array1D& addHadamardProduct(स्थिर ReadArray1D& a, स्थिर ReadArray1D& b) {
        U_ASSERT(a.d1() == d1());
        U_ASSERT(b.d1() == d1());
        क्रम (int32_t i = 0; i < d1(); i++) {
            तथ्य_2[i] += a.लो(i) * b.लो(i);
        }
        वापस *यह;
    }

    // Add the values of another array of the same size into this one.
    अंतरभूत Array1D& add(स्थिर ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        क्रम (int32_t i = 0; i < d1(); i++) {
            तथ्य_2[i] += a.लो(i);
        }
        वापस *यह;
    }

    // Assign the values of another array of the same size into this one.
    अंतरभूत Array1D& assign(स्थिर ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        क्रम (int32_t i = 0; i < d1(); i++) {
            तथ्य_2[i] = a.लो(i);
        }
        वापस *यह;
    }

    // Apply tanh to all the elements in the array.
    अंतरभूत Array1D& tanh() {
        वापस tanh(*यह);
    }

    // Apply tanh of a and store into this array.
    अंतरभूत Array1D& tanh(स्थिर Array1D& a) {
        U_ASSERT(a.d1() == d1());
        क्रम (int32_t i = 0; i < d1_; i++) {
            तथ्य_2[i] = std::tanh(a.लो(i));
        }
        वापस *यह;
    }

    // Apply sigmoid to all the elements in the array.
    अंतरभूत Array1D& sigmoid() {
        क्रम (int32_t i = 0; i < d1_; i++) {
            तथ्य_2[i] = 1.0f/(1.0f + expf(-तथ्य_2[i]));
        }
        वापस *यह;
    }

    अंतरभूत Array1D& सफा() {
        uprv_memset(तथ्य_2, 0, d1_ * माप(भग्न));
        वापस *यह;
    }

निजी:
    व्योम* memory_;
    भग्न* तथ्य_2;
    int32_t d1_;
};

Array1D::~Array1D()
{
    uprv_free(memory_);
}

श्रेणी Array2D : खुला ReadArray2D {
खुला:
    Array2D() : memory_(nullptr), तथ्य_2(nullptr), d1_(0), d2_(0) {}
    Array2D(int32_t d1, int32_t d2, UErrorCode &status)
        : memory_(uprv_malloc(d1 * d2 * माप(भग्न))),
          तथ्य_2(static_cast<भग्न*>(memory_)), d1_(d1), d2_(d2) {
        अगर (U_SUCCESS(status)) {
            अगर (memory_ == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                वापस;
            }
            सफा();
        }
    }
    भव ~Array2D();

    // ReadArray2D methods.
    भव int32_t d1() स्थिर override { वापस d1_; }
    भव int32_t d2() स्थिर override { वापस d2_; }
    भव भग्न लो(int32_t i, int32_t j) स्थिर override {
        U_ASSERT(i < d1_);
        U_ASSERT(j < d2_);
        वापस तथ्य_2[i * d2_ + j];
    }

    अंतरभूत Array1D row(int32_t i) स्थिर {
        U_ASSERT(i < d1_);
        वापस Array1D(तथ्य_2 + i * d2_, d2_);
    }

    अंतरभूत Array2D& सफा() {
        uprv_memset(तथ्य_2, 0, d1_ * d2_ * माप(भग्न));
        वापस *यह;
    }

निजी:
    व्योम* memory_;
    भग्न* तथ्य_2;
    int32_t d1_;
    int32_t d2_;
};

Array2D::~Array2D()
{
    uprv_free(memory_);
}

प्रकार क्रमागत {
    BEGIN,
    INSIDE,
    END,
    SINGLE
} LSTMClass;

प्रकार क्रमागत {
    UNKNOWN,
    CODE_POINTS,
    GRAPHEME_CLUSTER,
} EmbeddingType;

काष्ठा LSTMData : खुला UMemory {
    LSTMData(UResourceBundle* rb, UErrorCode &status);
    ~LSTMData();
    UHashtable* fDict;
    EmbeddingType fType;
    स्थिर char16_t* fName;
    ConstArray2D fEmbedding;
    ConstArray2D fForwardW;
    ConstArray2D fForwardU;
    ConstArray1D fForwardB;
    ConstArray2D fBackwardW;
    ConstArray2D fBackwardU;
    ConstArray1D fBackwardB;
    ConstArray2D fOutputW;
    ConstArray1D fOutputB;

निजी:
    UResourceBundle* fBundle;
};

LSTMData::LSTMData(UResourceBundle* rb, UErrorCode &status)
    : fDict(nullptr), fType(UNKNOWN), fName(nullptr),
      fBundle(rb)
{
    अगर (U_FAILURE(status)) {
        वापस;
    }
    अगर (IEEE_754 != 1) {
        status = U_UNSUPPORTED_ERROR;
        वापस;
    }
    LocalUResourceBundlePointer embeddings_res(
        ures_get_By_कुंजी(rb, "embeddings", nullptr, &status));
    int32_t embedding_आकार = ures_getInt(embeddings_res.getAlias(), &status);
    LocalUResourceBundlePointer hunits_res(
        ures_get_By_कुंजी(rb, "hunits", nullptr, &status));
    अगर (U_FAILURE(status)) वापस;
    int32_t hunits = ures_getInt(hunits_res.getAlias(), &status);
    स्थिर char16_t* type = ures_get_String_By_कुंजी(rb, "type", nullptr, &status);
    अगर (U_FAILURE(status)) वापस;
    अगर (u_strCompare(type, -1, u"codepoints", -1, false) == 0) {
        fType = CODE_POINTS;
    } अन्यथा अगर (u_strCompare(type, -1, u"graphclust", -1, false) == 0) {
        fType = GRAPHEME_CLUSTER;
    }
    fName = ures_get_String_By_कुंजी(rb, "model", nullptr, &status);
    LocalUResourceBundlePointer तथ्य_Res(ures_get_By_कुंजी(rb, "data", nullptr, &status));
    अगर (U_FAILURE(status)) वापस;
    int32_t तथ्य_len = 0;
    स्थिर int32_t* तथ्य = ures_getIntVector(तथ्य_Res.getAlias(), &तथ्य_len, &status);
    fDict = uhash_open(uhash_hashUChars, uhash_compareUChars, nullptr, &status);

    ढेर_UResource_Bundle ढेर_अस्थायी_Bundle;
    Resource_तथ्य_मूल्य value;
    ures_get_मूल्य_With_Fallback(rb, "dict", ढेर_अस्थायी_Bundle.getAlias(), value, status);
    ResourceArray stringArray = value.getArray(status);
    int32_t अंक_सूचकांक = stringArray.get_आकार();
    अगर (U_FAILURE(status)) { वापस; }

    // put dict into hash
    int32_t string_लंबाई;
    क्रम (int32_t idx = 0; idx < अंक_सूचकांक; idx++) {
        stringArray.get_मूल्य(idx, value);
        स्थिर char16_t* str = value.getString(string_लंबाई, status);
        uhash_putiAllowZero(fDict, (व्योम*)str, idx, &status);
        अगर (U_FAILURE(status)) वापस;
#अगर_घोषित LSTM_VECTORIZER_DEBUG
        म_लिखो("Assign [");
        जबतक (*str != 0x0000) {
            म_लिखो("U+%04x ", *str);
            str++;
        }
        म_लिखो("] map to %d\n", idx-1);
#पूर्ण_अगर
    }
    int32_t mat1_आकार = (अंक_सूचकांक + 1) * embedding_आकार;
    int32_t mat2_आकार = embedding_आकार * 4 * hunits;
    int32_t mat3_आकार = hunits * 4 * hunits;
    int32_t mat4_आकार = 4 * hunits;
    int32_t mat5_आकार = mat2_आकार;
    int32_t mat6_आकार = mat3_आकार;
    int32_t mat7_आकार = mat4_आकार;
    int32_t mat8_आकार = 2 * hunits * 4;
#अगर U_DEBUG
    int32_t mat9_आकार = 4;
    U_ASSERT(तथ्य_len == mat1_आकार + mat2_आकार + mat3_आकार + mat4_आकार + mat5_आकार +
        mat6_आकार + mat7_आकार + mat8_आकार + mat9_आकार);
#पूर्ण_अगर

    fEmbedding.init(तथ्य, (अंक_सूचकांक + 1), embedding_आकार);
    तथ्य += mat1_आकार;
    fForwardW.init(तथ्य, embedding_आकार, 4 * hunits);
    तथ्य += mat2_आकार;
    fForwardU.init(तथ्य, hunits, 4 * hunits);
    तथ्य += mat3_आकार;
    fForwardB.init(तथ्य, 4 * hunits);
    तथ्य += mat4_आकार;
    fBackwardW.init(तथ्य, embedding_आकार, 4 * hunits);
    तथ्य += mat5_आकार;
    fBackwardU.init(तथ्य, hunits, 4 * hunits);
    तथ्य += mat6_आकार;
    fBackwardB.init(तथ्य, 4 * hunits);
    तथ्य += mat7_आकार;
    fOutputW.init(तथ्य, 2 * hunits, 4);
    तथ्य += mat8_आकार;
    fOutputB.init(तथ्य, 4);
}

LSTMData::~LSTMData() {
    uhash_close(fDict);
    ures_close(fBundle);
}

श्रेणी Vectorizer : खुला UMemory {
खुला:
    Vectorizer(UHashtable* dict) : fDict(dict) {}
    भव ~Vectorizer();
    भव व्योम vectorize(UText *text, int32_t startPos, int32_t endPos,
                           UVector32 &offsets, UVector32 &indices,
                           UErrorCode &status) स्थिर = 0;
रक्षित:
    int32_t string_To_सूचकांक(स्थिर char16_t* str) स्थिर {
        UBool found = false;
        int32_t ret = uhash_getiAndFound(fDict, (स्थिर व्योम*)str, &found);
        अगर (!found) {
            ret = fDict->count;
        }
#अगर_घोषित LSTM_VECTORIZER_DEBUG
        म_लिखो("[");
        जबतक (*str != 0x0000) {
            म_लिखो("U+%04x ", *str);
            str++;
        }
        म_लिखो("] map to %d\n", ret);
#पूर्ण_अगर
        वापस ret;
    }

निजी:
    UHashtable* fDict;
};

Vectorizer::~Vectorizer()
{
}

श्रेणी CodePointsVectorizer : खुला Vectorizer {
खुला:
    CodePointsVectorizer(UHashtable* dict) : Vectorizer(dict) {}
    भव ~CodePointsVectorizer();
    भव व्योम vectorize(UText *text, int32_t startPos, int32_t endPos,
                           UVector32 &offsets, UVector32 &indices,
                           UErrorCode &status) स्थिर override;
};

CodePointsVectorizer::~CodePointsVectorizer()
{
}

व्योम CodePointsVectorizer::vectorize(
    UText *text, int32_t startPos, int32_t endPos,
    UVector32 &offsets, UVector32 &indices, UErrorCode &status) स्थिर
{
    अगर (offsets.ensureCapacity(endPos - startPos, status) &&
            indices.ensureCapacity(endPos - startPos, status)) {
        अगर (U_FAILURE(status)) वापस;
        utext_set_Native_सूचकांक(text, startPos);
        int32_t current;
        char16_t str[2] = {0, 0};
        जबतक (U_SUCCESS(status) &&
               (current = static_cast<int32_t>(utext_get_Native_सूचकांक(text))) < endPos) {
            // Since the LSTMBreakEngine is currently only accept chars in BMP,
            // we can ignore the possibility of hitting supplementary code
            // point.
            str[0] = static_cast<char16_t>(utext_next32(text));
            U_ASSERT(!U_IS_SURROGATE(str[0]));
            offsets.addElement(current, status);
            indices.addElement(string_To_सूचकांक(str), status);
        }
    }
}

श्रेणी GraphemeClusterVectorizer : खुला Vectorizer {
खुला:
    GraphemeClusterVectorizer(UHashtable* dict)
        : Vectorizer(dict)
    {
    }
    भव ~GraphemeClusterVectorizer();
    भव व्योम vectorize(UText *text, int32_t startPos, int32_t endPos,
                           UVector32 &offsets, UVector32 &indices,
                           UErrorCode &status) स्थिर override;
};

GraphemeClusterVectorizer::~GraphemeClusterVectorizer()
{
}

constexpr int32_t MAX_GRAPHEME_CLSTER_लंबाई = 10;

व्योम GraphemeClusterVectorizer::vectorize(
    UText *text, int32_t startPos, int32_t endPos,
    UVector32 &offsets, UVector32 &indices, UErrorCode &status) स्थिर
{
    अगर (U_FAILURE(status)) वापस;
    अगर (!offsets.ensureCapacity(endPos - startPos, status) ||
            !indices.ensureCapacity(endPos - startPos, status)) {
        वापस;
    }
    अगर (U_FAILURE(status)) वापस;
    LocalPointer<BreakIterator> graphemeIter(BreakIterator::createCharacterInstance(Locale(), status));
    अगर (U_FAILURE(status)) वापस;
    graphemeIter->setText(text, status);
    अगर (U_FAILURE(status)) वापस;

    अगर (startPos != 0) {
        graphemeIter->preceding(startPos);
    }
    int32_t last = startPos;
    int32_t current = startPos;
    char16_t str[MAX_GRAPHEME_CLSTER_लंबाई];
    जबतक ((current = graphemeIter->next()) != BreakIterator::DONE) {
        अगर (current >= endPos) {
            अवरोध;
        }
        अगर (current > startPos) {
            utext_extract(text, last, current, str, MAX_GRAPHEME_CLSTER_लंबाई, &status);
            अगर (U_FAILURE(status)) वापस;
            offsets.addElement(last, status);
            indices.addElement(string_To_सूचकांक(str), status);
            अगर (U_FAILURE(status)) वापस;
        }
        last = current;
    }
    अगर (U_FAILURE(status) || last >= endPos) {
        वापस;
    }
    utext_extract(text, last, endPos, str, MAX_GRAPHEME_CLSTER_लंबाई, &status);
    अगर (U_SUCCESS(status)) {
        offsets.addElement(last, status);
        indices.addElement(string_To_सूचकांक(str), status);
    }
}

// Computing LSTM as stated in
// https://en.wikipedia.org/wiki/Long_short-term_memory#LSTM_with_a_forget_gate
// ifco is temp array allocate outside which does not need to be
// input/output value but could avoid unnecessary memory alloc/free if passing
// in.
व्योम compute(
    int32_t hunits,
    स्थिर ReadArray2D& W, स्थिर ReadArray2D& U, स्थिर ReadArray1D& b,
    स्थिर ReadArray1D& x, Array1D& h, Array1D& c,
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
अटल स्थिर int32_t MIN_WORD = 2;

// Minimum number of characters for two words
अटल स्थिर int32_t MIN_WORD_SPAN = MIN_WORD * 2;

int32_t
LSTMBreakEngine::divideUpDictionaryRange( UText *text,
                                                int32_t startPos,
                                                int32_t endPos,
                                                UVector32 &foundBreaks,
                                                UBool /* isPhraseBreaking */,
                                                UErrorCode& status) स्थिर {
    अगर (U_FAILURE(status)) वापस 0;
    int32_t begin_Found_Break_आकार = foundBreaks.size();
    utext_set_Native_सूचकांक(text, startPos);
    utext_moveIndex32(text, MIN_WORD_SPAN);
    अगर (utext_get_Native_सूचकांक(text) >= endPos) {
        वापस 0;       // Not enough characters for two words
    }
    utext_set_Native_सूचकांक(text, startPos);

    UVector32 offsets(status);
    UVector32 indices(status);
    अगर (U_FAILURE(status)) वापस 0;
    fVectorizer->vectorize(text, startPos, endPos, offsets, indices, status);
    अगर (U_FAILURE(status)) वापस 0;
    int32_t* offsetsBuf = offsets.get_बफर();
    int32_t* indicesBuf = indices.get_बफर();

    int32_t निवेश_seq_len = indices.size();
    int32_t hunits = f_तथ्य->fForwardU.d1();

    // ----- Begin of all the Array memory allocation needed for this function
    // Allocate temp array used inside compute()
    Array1D ifco(4 * hunits, status);

    Array1D c(hunits, status);
    Array1D logp(4, status);

    // TODO: limit size of hBackward. If input_seq_len is too big, we could
    // run out of memory.
    // Backward LSTM
    Array2D hBackward(निवेश_seq_len, hunits, status);

    // Allocate fbRow and slice the internal array in two.
    Array1D fbRow(2 * hunits, status);

    // ----- End of all the Array memory allocation needed for this function
    अगर (U_FAILURE(status)) वापस 0;

    // To save the needed memory usage, the following is different from the
    // Python or ICU4X implementation. We first perform the Backward LSTM
    // and then merge the iteration of the forward LSTM and the output layer
    // together because we only neetdto remember the h[t-1] for Forward LSTM.
    क्रम (int32_t i = निवेश_seq_len - 1; i >= 0; i--) {
        Array1D hRow = hBackward.row(i);
        अगर (i != निवेश_seq_len - 1) {
            hRow.assign(hBackward.row(i+1));
        }
#अगर_घोषित LSTM_DEBUG
        म_लिखो("hRow %d\n", i);
        hRow.print();
        म_लिखो("indicesBuf[%d] = %d\n", i, indicesBuf[i]);
        म_लिखो("fData->fEmbedding.row(indicesBuf[%d]):\n", i);
        f_तथ्य->fEmbedding.row(indicesBuf[i]).print();
#पूर्ण_अगर  // LSTM_DEBUG
        compute(hunits,
                f_तथ्य->fBackwardW, f_तथ्य->fBackwardU, f_तथ्य->fBackwardB,
                f_तथ्य->fEmbedding.row(indicesBuf[i]),
                hRow, c, ifco);
    }


    Array1D forwardRow = fbRow.slice(0, hunits);  // point to first half of data in fbRow.
    Array1D backwardRow = fbRow.slice(hunits, hunits);  // point to second half of data n fbRow.

    // The following iteration merge the forward LSTM and the output layer
    // together.
    c.सफा();  // reuse c since it is the same size.
    क्रम (int32_t i = 0; i < निवेश_seq_len; i++) {
#अगर_घोषित LSTM_DEBUG
        म_लिखो("forwardRow %d\n", i);
        forwardRow.print();
#पूर्ण_अगर  // LSTM_DEBUG
        // Forward LSTM
        // Calculate the result into forwardRow, which point to the data in the first half
        // of fbRow.
        compute(hunits,
                f_तथ्य->fForwardW, f_तथ्य->fForwardU, f_तथ्य->fForwardB,
                f_तथ्य->fEmbedding.row(indicesBuf[i]),
                forwardRow, c, ifco);

        // assign the data from hBackward.row(i) to second half of fbRowa.
        backwardRow.assign(hBackward.row(i));

        logp.assign(f_तथ्य->fOutputB).addDotProduct(fbRow, f_तथ्य->fOutputW);
#अगर_घोषित LSTM_DEBUG
        म_लिखो("backwardRow %d\n", i);
        backwardRow.print();
        म_लिखो("logp %d\n", i);
        logp.print();
#पूर्ण_अगर  // LSTM_DEBUG

        // current = argmax(logp)
        LSTMClass current = static_cast<LSTMClass>(logp.max_सूचकांक());
        // BIES logic.
        अगर (current == BEGIN || current == SINGLE) {
            अगर (i != 0) {
                foundBreaks.addElement(offsetsBuf[i], status);
                अगर (U_FAILURE(status)) वापस 0;
            }
        }
    }
    वापस foundBreaks.size() - begin_Found_Break_आकार;
}

Vectorizer* createVectorizer(स्थिर LSTMData* तथ्य, UErrorCode &status) {
    अगर (U_FAILURE(status)) {
        वापस nullptr;
    }
    चयन (तथ्य->fType) {
        हाल CODE_POINTS:
            वापस नया CodePointsVectorizer(तथ्य->fDict);
            अवरोध;
        हाल GRAPHEME_CLUSTER:
            वापस नया GraphemeClusterVectorizer(तथ्य->fDict);
            अवरोध;
        शेष:
            अवरोध;
    }
    UPRV_UNREACHABLE_EXIT;
}

LSTMBreakEngine::LSTMBreakEngine(स्थिर LSTMData* तथ्य, स्थिर UnicodeSet& set, UErrorCode &status)
    : DictionaryBreakEngine(), f_तथ्य(तथ्य), fVectorizer(createVectorizer(f_तथ्य, status))
{
    अगर (U_FAILURE(status)) {
      f_तथ्य = nullptr;  // If failure, we should not delete fData in destructor because the caller will do so.
      वापस;
    }
    setCharacters(set);
}

LSTMBreakEngine::~LSTMBreakEngine() {
    मिटाओ f_तथ्य;
    मिटाओ fVectorizer;
}

स्थिर char16_t* LSTMBreakEngine::name() स्थिर {
    वापस f_तथ्य->fName;
}

UnicodeString defaultLSTM(UScriptCode script, UErrorCode& status) {
    // open root from brkitr tree.
    UResourceBundle *b = ures_open(U_ICUDATA_BRKITR, "", &status);
    b = ures_get_By_कुंजी_With_Fallback(b, "lstm", b, &status);
    UnicodeString परिणाम = ures_get_Unicode_String_By_कुंजी(b, uscript_getShortName(script), &status);
    ures_close(b);
    वापस परिणाम;
}

U_CAPI स्थिर LSTMData* U_EXPORT2 CreateLSTMDataForScript(UScriptCode script, UErrorCode& status)
{
    अगर (script != USCRIPT_KHMER && script != USCRIPT_LAO && script != USCRIPT_MYANMAR && script != USCRIPT_THAI) {
        वापस nullptr;
    }
    UnicodeString name = defaultLSTM(script, status);
    अगर (U_FAILURE(status)) वापस nullptr;
    CharString namebuf;
    namebuf.appendInvariantChars(name, status).truncate(namebuf.last_सूचकांक_Of('.'));

    LocalUResourceBundlePointer rb(
        ures_openDirect(U_ICUDATA_BRKITR, namebuf.तथ्य(), &status));
    अगर (U_FAILURE(status)) वापस nullptr;

    वापस CreateLSTMData(rb.orphan(), status);
}

U_CAPI स्थिर LSTMData* U_EXPORT2 CreateLSTMData(UResourceBundle* rb, UErrorCode& status)
{
    अगर (U_FAILURE(status)) {
        वापस nullptr;
    }
    स्थिर LSTMData* परिणाम = नया LSTMData(rb, status);
    अगर (U_FAILURE(status)) {
        मिटाओ परिणाम;
        वापस nullptr;
    }
    वापस परिणाम;
}

U_CAPI स्थिर LanguageBreakEngine* U_EXPORT2
CreateLSTMBreakEngine(UScriptCode script, स्थिर LSTMData* तथ्य, UErrorCode& status)
{
    UnicodeString unicodeSetString;
    चयन(script) {
        हाल USCRIPT_THAI:
            unicodeSetString = UnicodeString(u"[[:Thai:]&[:LineBreak=SA:]]");
            अवरोध;
        हाल USCRIPT_MYANMAR:
            unicodeSetString = UnicodeString(u"[[:Mymr:]&[:LineBreak=SA:]]");
            अवरोध;
        शेष:
            मिटाओ तथ्य;
            वापस nullptr;
    }
    UnicodeSet unicodeSet;
    unicodeSet.applyPattern(unicodeSetString, status);
    स्थिर LanguageBreakEngine* engine = नया LSTMBreakEngine(तथ्य, unicodeSet, status);
    अगर (U_FAILURE(status) || engine == nullptr) {
        अगर (engine != nullptr) {
            मिटाओ engine;
        } अन्यथा {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        वापस nullptr;
    }
    वापस engine;
}

U_CAPI व्योम U_EXPORT2 DeleteLSTMData(स्थिर LSTMData* तथ्य)
{
    मिटाओ तथ्य;
}

U_CAPI स्थिर char16_t* U_EXPORT2 LSTMDataName(स्थिर LSTMData* तथ्य)
{
    वापस तथ्य->fName;
}

U_NAMESPACE_END

#पूर्ण_अगर /* #if !UCONFIG_NO_BREAK_ITERATION */
