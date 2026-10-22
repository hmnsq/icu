// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#अन्तर्भाव "unicode/utypes.h"

#यदि U_ENABLE_TRACING

#अन्तर्भाव "restrace.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "utracimp.h"
#अन्तर्भाव "uresimp.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "util.h"

U_NAMESPACE_BEGIN

ResourceTracer::~ResourceTracer() = स्वभाव;

शून्य ResourceTracer::trace(ध्रुव वर्ण* resType) ध्रुव {
    U_ASSERT(fResB || fParent);
    UTRACE_प्रविष्टिः(UTRACE_UDATA_RESOURCE);
    UError_संहिता स्थितिः = U_ZERO_ERROR;

    CharString file_मार्गः;
    get_File_मार्गः(file_मार्गः, स्थितिः);

    CharString res_मार्गः;
    get_Res_मार्गः(res_मार्गः, स्थितिः);

    // The longest type ("intvector") is 9 chars
    ध्रुव वर्ण kSpaces[] = "         ";
    CharString format;
    format.append(kSpaces, परिमाण(kSpaces) - 1 - uprv_strlen(resType), स्थितिः);
    format.append("(%s) %s @ %s", स्थितिः);

    UTRACE_DATA3(UTRACE_VERBOSE,
        format.दत्त(),
        resType,
        file_मार्गः.दत्त(),
        res_मार्गः.दत्त());
    UTRACE_EXIT_स्थितिः(स्थितिः);
}

शून्य ResourceTracer::traceOpen() ध्रुव {
    U_ASSERT(fResB);
    UTRACE_प्रविष्टिः(UTRACE_UDATA_BUNDLE);
    UError_संहिता स्थितिः = U_ZERO_ERROR;

    CharString file_मार्गः;
    UTRACE_DATA1(UTRACE_VERBOSE, "%s", get_File_मार्गः(file_मार्गः, स्थितिः).दत्त());
    UTRACE_EXIT_स्थितिः(स्थितिः);
}

CharString& ResourceTracer::get_File_मार्गः(CharString& output, UError_संहिता& स्थितिः) ध्रुव {
    यदि (fResB) {
        // Note: if you get a segfault around here, check that ResourceTable and
        // ResourceArray instances outlive ResourceValue instances referring to
        // their contents:
        output.append(fResB->f_दत्त->f_मार्गः, स्थितिः);
        output.append('/', स्थितिः);
        output.append(fResB->f_दत्त->fName, स्थितिः);
        output.append(".res", स्थितिः);
    } अन्यथा {
        fParent->get_File_मार्गः(output, स्थितिः);
    }
    प्रत्यागम output;
}

CharString& ResourceTracer::get_Res_मार्गः(CharString& output, UError_संहिता& स्थितिः) ध्रुव {
    यदि (fResB) {
        output.append('/', स्थितिः);
        output.append(fResB->f_Res_मार्गः, स्थितिः);
        // removing the trailing /
        U_ASSERT(output[output.length()-1] == '/');
        output.truncate(output.length()-1);
    } अन्यथा {
        fParent->get_Res_मार्गः(output, स्थितिः);
    }
    यदि (f_कुञ्चिका) {
        output.append('/', स्थितिः);
        output.append(f_कुञ्चिका, स्थितिः);
    }
    यदि (f_सूचकांक != -1) {
        output.append('[', स्थितिः);
        UnicodeString सूचकांक_String;
        ICU_Utility::append_सङ्ख्या(सूचकांक_String, f_सूचकांक);
        output.appendInvariantChars(सूचकांक_String, स्थितिः);
        output.append(']', स्थितिः);
    }
    प्रत्यागम output;
}

शून्य FileTracer::traceOpen(ध्रुव वर्ण* मार्गः, ध्रुव वर्ण* type, ध्रुव वर्ण* name) {
    यदि (uprv_strcmp(type, "res") == 0) {
        traceOpenResFile(मार्गः, name);
    } अन्यथा {
        trace_Open_दत्त_File(मार्गः, type, name);
    }
}

शून्य FileTracer::trace_Open_दत्त_File(ध्रुव वर्ण* मार्गः, ध्रुव वर्ण* type, ध्रुव वर्ण* name) {
    UTRACE_प्रविष्टिः(UTRACE_UDATA_दत्त_FILE);
    UError_संहिता स्थितिः = U_ZERO_ERROR;

    CharString file_मार्गः;
    file_मार्गः.append(मार्गः, स्थितिः);
    file_मार्गः.append('/', स्थितिः);
    file_मार्गः.append(name, स्थितिः);
    file_मार्गः.append('.', स्थितिः);
    file_मार्गः.append(type, स्थितिः);

    UTRACE_DATA1(UTRACE_VERBOSE, "%s", file_मार्गः.दत्त());
    UTRACE_EXIT_स्थितिः(स्थितिः);
}

शून्य FileTracer::traceOpenResFile(ध्रुव वर्ण* मार्गः, ध्रुव वर्ण* name) {
    UTRACE_प्रविष्टिः(UTRACE_UDATA_RES_FILE);
    UError_संहिता स्थितिः = U_ZERO_ERROR;

    CharString file_मार्गः;
    file_मार्गः.append(मार्गः, स्थितिः);
    file_मार्गः.append('/', स्थितिः);
    file_मार्गः.append(name, स्थितिः);
    file_मार्गः.append(".res", स्थितिः);

    UTRACE_DATA1(UTRACE_VERBOSE, "%s", file_मार्गः.दत्त());
    UTRACE_EXIT_स्थितिः(स्थितिः);
}

U_NAMESPACE_अन्तः

#यदि_अन्तः // U_ENABLE_TRACING
