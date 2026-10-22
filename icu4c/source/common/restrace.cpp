// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#অন্তর্ভাব "unicode/utypes.h"

#যদি U_ENABLE_TRACING

#অন্তর্ভাব "restrace.h"
#অন্তর্ভাব "charstr.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "utracimp.h"
#অন্তর্ভাব "uresimp.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "util.h"

U_NAMESPACE_BEGIN

ResourceTracer::~ResourceTracer() = স্বভাব;

শূন্য ResourceTracer::trace(ধ্রুব বর্ণ* resType) ধ্রুব {
    U_ASSERT(fResB || fParent);
    UTRACE_ENTRY(UTRACE_UDATA_RESOURCE);
    UErrorCode status = U_ZERO_ERROR;

    CharString filePath;
    getFilePath(filePath, status);

    CharString resPath;
    getResPath(resPath, status);

    // The longest type ("intvector") is 9 chars
    ধ্রুব বর্ণ kSpaces[] = "         ";
    CharString format;
    format.append(kSpaces, পরিমাণ(kSpaces) - 1 - uprv_strlen(resType), status);
    format.append("(%s) %s @ %s", status);

    UTRACE_DATA3(UTRACE_VERBOSE,
        format.data(),
        resType,
        filePath.data(),
        resPath.data());
    UTRACE_EXIT_STATUS(status);
}

শূন্য ResourceTracer::traceOpen() ধ্রুব {
    U_ASSERT(fResB);
    UTRACE_ENTRY(UTRACE_UDATA_BUNDLE);
    UErrorCode status = U_ZERO_ERROR;

    CharString filePath;
    UTRACE_DATA1(UTRACE_VERBOSE, "%s", getFilePath(filePath, status).data());
    UTRACE_EXIT_STATUS(status);
}

CharString& ResourceTracer::getFilePath(CharString& output, UErrorCode& status) ধ্রুব {
    যদি (fResB) {
        // Note: if you get a segfault around here, check that ResourceTable and
        // ResourceArray instances outlive ResourceValue instances referring to
        // their contents:
        output.append(fResB->fData->fPath, status);
        output.append('/', status);
        output.append(fResB->fData->fName, status);
        output.append(".res", status);
    } অন্যথা {
        fParent->getFilePath(output, status);
    }
    প্রত্যাগম output;
}

CharString& ResourceTracer::getResPath(CharString& output, UErrorCode& status) ধ্রুব {
    যদি (fResB) {
        output.append('/', status);
        output.append(fResB->fResPath, status);
        // removing the trailing /
        U_ASSERT(output[output.length()-1] == '/');
        output.truncate(output.length()-1);
    } অন্যথা {
        fParent->getResPath(output, status);
    }
    যদি (f_চাবি) {
        output.append('/', status);
        output.append(f_চাবি, status);
    }
    যদি (fIndex != -1) {
        output.append('[', status);
        UnicodeString indexString;
        ICU_Utility::append_সংখ্যা(indexString, fIndex);
        output.appendInvariantChars(indexString, status);
        output.append(']', status);
    }
    প্রত্যাগম output;
}

শূন্য FileTracer::traceOpen(ধ্রুব বর্ণ* path, ধ্রুব বর্ণ* type, ধ্রুব বর্ণ* name) {
    যদি (uprv_strcmp(type, "res") == 0) {
        traceOpenResFile(path, name);
    } অন্যথা {
        traceOpenDataFile(path, type, name);
    }
}

শূন্য FileTracer::traceOpenDataFile(ধ্রুব বর্ণ* path, ধ্রুব বর্ণ* type, ধ্রুব বর্ণ* name) {
    UTRACE_ENTRY(UTRACE_UDATA_DATA_FILE);
    UErrorCode status = U_ZERO_ERROR;

    CharString filePath;
    filePath.append(path, status);
    filePath.append('/', status);
    filePath.append(name, status);
    filePath.append('.', status);
    filePath.append(type, status);

    UTRACE_DATA1(UTRACE_VERBOSE, "%s", filePath.data());
    UTRACE_EXIT_STATUS(status);
}

শূন্য FileTracer::traceOpenResFile(ধ্রুব বর্ণ* path, ধ্রুব বর্ণ* name) {
    UTRACE_ENTRY(UTRACE_UDATA_RES_FILE);
    UErrorCode status = U_ZERO_ERROR;

    CharString filePath;
    filePath.append(path, status);
    filePath.append('/', status);
    filePath.append(name, status);
    filePath.append(".res", status);

    UTRACE_DATA1(UTRACE_VERBOSE, "%s", filePath.data());
    UTRACE_EXIT_STATUS(status);
}

U_NAMESPACE_END

#endif // U_ENABLE_TRACING
