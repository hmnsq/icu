// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#అన్తర్భావ <optional>
#అన్తర్భావ <string_view>
#అన్తర్భావ <utility>

#అన్తర్భావ "bytesinkutil.h"  // StringByteSink<CharString>
#అన్తర్భావ "charstr.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "fixedstring.h"
#అన్తర్భావ "ulocimp.h"
#అన్తర్భావ "unicode/localebuilder.h"
#అన్తర్భావ "unicode/locid.h"

నామాకాశ {

అంతఃవరుస తార్కిక UPRV_ISDIGIT(అక్షరం c) { తిరిగి c >= '0' && c <= '9'; }
అంతఃవరుస తార్కిక UPRV_ISALPHANUM(అక్షరం c) { తిరిగి uprv_isASCIILetter(c) || UPRV_ISDIGIT(c); }

constexpr స్థిరరాశి అక్షరం* k_Attribute_కీలకం = "attribute";

తార్కిక _isExtensionSubtags(అక్షరం కీలకం, స్థిరరాశి అక్షరం* s, int32_t len) {
    ఎంపిక (uprv_tolower(కీలకం)) {
        స్థితి 'u':
            తిరిగి ultag_isUnicodeExtensionSubtags(s, len);
        స్థితి 't':
            తిరిగి ultag_isTransformedExtensionSubtags(s, len);
        స్థితి 'x':
            తిరిగి ultag_is_Privateuse_విలువ_Subtags(s, len);
        అప్రమేయ:
            తిరిగి ultag_isExtensionSubtags(s, len);
    }
}

}  // namespace

U_NAMESPACE_BEGIN

LocaleBuilder::LocaleBuilder() : UObject(), స్థితి_3(U_ZERO_ERROR), language_(),
    script_(), region_(), variant_(శూన్యసూచిక), extensions_(శూన్యసూచిక)
{
    language_[0] = 0;
    script_[0] = 0;
    region_[0] = 0;
}

LocaleBuilder::~LocaleBuilder()
{
    తొలగించు variant_;
    తొలగించు extensions_;
}

LocaleBuilder& LocaleBuilder::సమితి_Locale(స్థిరరాశి Locale& locale) U_LIFETIME_BOUND
{
    clear();
    సమితి_Language(locale.getLanguage());
    సమితి_Script(locale.getScript());
    సమితి_Region(locale.getCountry());
    సమితి_Variant(locale.getVariant());
    extensions_ = locale.clone();
    ఒకవేళ (extensions_ == శూన్యసూచిక) {
        స్థితి_3 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
    }
    తిరిగి *ఇది;
}

LocaleBuilder& LocaleBuilder::సమితి_Language_ట్యాగ్(StringPiece ట్యాగ్) U_LIFETIME_BOUND
{
    Locale l = Locale::for_Language_ట్యాగ్(ట్యాగ్, స్థితి_3);
    ఒకవేళ (U_FAILURE(స్థితి_3)) { తిరిగి *ఇది; }
    // Because setLocale will reset status_ we need to return
    // first if we have error in forLanguageTag.
    సమితి_Locale(l);
    తిరిగి *ఇది;
}

నామాకాశ {

శూన్య సమితి_క్షేత్రం(StringPiece input, అక్షరం* dest, UError_సంకేతం& error_సంకేతం,
              తార్కిక (*test)(స్థిరరాశి అక్షరం*, int32_t)) {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి; }
    ఒకవేళ (input.empty()) {
        dest[0] = '\0';
    } లేకపోతే ఒకవేళ (test(input.దత్తాంశం(), input.length())) {
        uprv_memcpy(dest, input.దత్తాంశం(), input.length());
        dest[input.length()] = '\0';
    } లేకపోతే {
        error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::సమితి_Language(StringPiece language) U_LIFETIME_BOUND
{
    సమితి_క్షేత్రం(language, language_, స్థితి_3, &ultag_isLanguageSubtag);
    తిరిగి *ఇది;
}

LocaleBuilder& LocaleBuilder::సమితి_Script(StringPiece script) U_LIFETIME_BOUND
{
    సమితి_క్షేత్రం(script, script_, స్థితి_3, &ultag_isScriptSubtag);
    తిరిగి *ఇది;
}

LocaleBuilder& LocaleBuilder::సమితి_Region(StringPiece region) U_LIFETIME_BOUND
{
    సమితి_క్షేత్రం(region, region_, స్థితి_3, &ultag_isRegionSubtag);
    తిరిగి *ఇది;
}

నామాకాశ {

శూన్య transform(అక్షరం* దత్తాంశం, int32_t len) {
    కొరకు (int32_t i = 0; i < len; i++, దత్తాంశం++) {
        ఒకవేళ (*దత్తాంశం == '_') {
            *దత్తాంశం = '-';
        } లేకపోతే {
            *దత్తాంశం = uprv_tolower(*దత్తాంశం);
        }
    }
}

}  // namespace

LocaleBuilder& LocaleBuilder::సమితి_Variant(StringPiece variant) U_LIFETIME_BOUND
{
    ఒకవేళ (U_FAILURE(స్థితి_3)) { తిరిగి *ఇది; }
    ఒకవేళ (variant.empty()) {
        తొలగించు variant_;
        variant_ = శూన్యసూచిక;
        తిరిగి *ఇది;
    }
    FixedString* new_variant = కొత్త FixedString(variant);
    ఒకవేళ (new_variant == శూన్యసూచిక || new_variant->is_ఖాళీలు()) {
        స్థితి_3 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి *ఇది;
    }
    transform(new_variant->getAlias(), variant.length());
    ఒకవేళ (!ultag_isVariantSubtags(new_variant->దత్తాంశం(), variant.length())) {
        తొలగించు new_variant;
        స్థితి_3 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి *ఇది;
    }
    తొలగించు variant_;
    variant_ = new_variant;
    తిరిగి *ఇది;
}

నామాకాశ {

తార్కిక
is_Keyword_విలువ(స్థిరరాశి అక్షరం* కీలకం, స్థిరరాశి అక్షరం* value, int32_t విలువ_len)
{
    ఒకవేళ (కీలకం[1] == '\0') {
        // one char key
        తిరిగి (UPRV_ISALPHANUM(uprv_tolower(కీలకం[0])) &&
                _isExtensionSubtags(కీలకం[0], value, విలువ_len));
    } లేకపోతే ఒకవేళ (uprv_strcmp(కీలకం, k_Attribute_కీలకం) == 0) {
        // unicode attributes
        తిరిగి ultag_is_Unicode_Locale_లక్షణాలు(value, విలువ_len);
    }
    // otherwise: unicode extension value
    // We need to convert from legacy key/value to unicode
    // key/value
    std::optional<std::string_view> unicode_locale_కీలకం = ulocimp_గమ్యం_Bcp_కీలకం_With_Fallback(కీలకం);
    std::optional<std::string_view> unicode_locale_type = ulocimp_గమ్యం_Bcp_Type_With_Fallback(కీలకం, value);

    తిరిగి unicode_locale_కీలకం.has_value() &&
           unicode_locale_type.has_value() &&
           ultag_is_Unicode_Locale_కీలకం(unicode_locale_కీలకం->దత్తాంశం(),
                                    static_cast<int32_t>(unicode_locale_కీలకం->size())) &&
           ultag_isUnicodeLocaleType(unicode_locale_type->దత్తాంశం(),
                                     static_cast<int32_t>(unicode_locale_type->size()));
}

శూన్య
_copyExtensions(స్థిరరాశి Locale& from, icu::StringEnumeration *keywords,
                Locale& గమ్యం, తార్కిక validate, UError_సంకేతం& error_సంకేతం)
{
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి; }
    Local_సూచిక<icu::StringEnumeration> ownedKeywords;
    ఒకవేళ (keywords == శూన్యసూచిక) {
        ownedKeywords.adoptInstead(from.createKeywords(error_సంకేతం));
        ఒకవేళ (U_FAILURE(error_సంకేతం) || ownedKeywords.isNull()) { తిరిగి; }
        keywords = ownedKeywords.getAlias();
    }
    స్థిరరాశి అక్షరం* కీలకం;
    అయితే ((కీలకం = keywords->next(శూన్యసూచిక, error_సంకేతం)) != శూన్యసూచిక) {
        స్వయంచాలక value = from.get_Keyword_విలువ<CharString>(కీలకం, error_సంకేతం);
        ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి; }
        ఒకవేళ (uprv_strcmp(కీలకం, k_Attribute_కీలకం) == 0) {
            transform(value.దత్తాంశం(), value.length());
        }
        ఒకవేళ (validate &&
            !is_Keyword_విలువ(కీలకం, value.దత్తాంశం(), value.length())) {
            error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
            తిరిగి;
        }
        గమ్యం.సమితి_Keyword_విలువ(కీలకం, value.దత్తాంశం(), error_సంకేతం);
        ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి; }
    }
}

శూన్య
clear_UAttributes_And_కీలకం_Type(Locale& locale, UError_సంకేతం& error_సంకేతం)
{
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి; }
    // Clear Unicode attributes
    locale.సమితి_Keyword_విలువ(k_Attribute_కీలకం, "", error_సంకేతం);

    // Clear all Unicode keyword values
    Local_సూచిక<icu::StringEnumeration> iter(locale.createUnicodeKeywords(error_సంకేతం));
    ఒకవేళ (U_FAILURE(error_సంకేతం) || iter.isNull()) { తిరిగి; }
    స్థిరరాశి అక్షరం* కీలకం;
    అయితే ((కీలకం = iter->next(శూన్యసూచిక, error_సంకేతం)) != శూన్యసూచిక) {
        locale.సమితి_Unicode_Keyword_విలువ(కీలకం, శూన్యసూచిక, error_సంకేతం);
    }
}

శూన్య
సమితి_Unicode_Extensions(Locale& locale, స్థిరరాశి CharString& value, UError_సంకేతం& error_సంకేతం)
{
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి; }
    // Add the unicode extensions to extensions_
    CharString locale_str("und-u-", error_సంకేతం);
    locale_str.append(value, error_సంకేతం);
    _copyExtensions(
        Locale::for_Language_ట్యాగ్(locale_str.దత్తాంశం(), error_సంకేతం), శూన్యసూచిక,
        locale, అబద్ధం, error_సంకేతం);
}

}  // namespace

LocaleBuilder& LocaleBuilder::సమితి_Extension(అక్షరం కీలకం, StringPiece value) U_LIFETIME_BOUND
{
    ఒకవేళ (U_FAILURE(స్థితి_3)) { తిరిగి *ఇది; }
    ఒకవేళ (!UPRV_ISALPHANUM(కీలకం)) {
        స్థితి_3 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి *ఇది;
    }
    CharString విలువ_str(value, స్థితి_3);
    ఒకవేళ (U_FAILURE(స్థితి_3)) { తిరిగి *ఇది; }
    transform(విలువ_str.దత్తాంశం(), విలువ_str.length());
    ఒకవేళ (!విలువ_str.is_ఖాళీలు() &&
            !_isExtensionSubtags(కీలకం, విలువ_str.దత్తాంశం(), విలువ_str.length())) {
        స్థితి_3 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి *ఇది;
    }
    ఒకవేళ (extensions_ == శూన్యసూచిక) {
        extensions_ = Locale::get_మూలం().clone();
        ఒకవేళ (extensions_ == శూన్యసూచిక) {
            స్థితి_3 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి *ఇది;
        }
    }
    ఒకవేళ (uprv_tolower(కీలకం) != 'u') {
        // for t, x and others extension.
        extensions_->సమితి_Keyword_విలువ(StringPiece(&కీలకం, 1), విలువ_str.దత్తాంశం(),
                                     స్థితి_3);
        తిరిగి *ఇది;
    }
    clear_UAttributes_And_కీలకం_Type(*extensions_, స్థితి_3);
    ఒకవేళ (U_FAILURE(స్థితి_3)) { తిరిగి *ఇది; }
    ఒకవేళ (!value.empty()) {
        సమితి_Unicode_Extensions(*extensions_, విలువ_str, స్థితి_3);
    }
    తిరిగి *ఇది;
}

LocaleBuilder& LocaleBuilder::సమితి_Unicode_Locale_Keyword(StringPiece కీలకం,
                                                      StringPiece type) U_LIFETIME_BOUND
{
    ఒకవేళ (U_FAILURE(స్థితి_3)) { తిరిగి *ఇది; }
    ఒకవేళ (!ultag_is_Unicode_Locale_కీలకం(కీలకం.దత్తాంశం(), కీలకం.length()) ||
            (!type.empty() &&
                 !ultag_isUnicodeLocaleType(type.దత్తాంశం(), type.length()))) {
      స్థితి_3 = U_ILLEGAL_వాదం_ERROR;
      తిరిగి *ఇది;
    }
    ఒకవేళ (extensions_ == శూన్యసూచిక) {
        extensions_ = Locale::get_మూలం().clone();
        ఒకవేళ (extensions_ == శూన్యసూచిక) {
            స్థితి_3 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి *ఇది;
        }
    }
    extensions_->సమితి_Unicode_Keyword_విలువ(కీలకం, type, స్థితి_3);
    తిరిగి *ఇది;
}

LocaleBuilder& LocaleBuilder::కూడు_Unicode_Locale_Attribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString విలువ_str(value, స్థితి_3);
    ఒకవేళ (U_FAILURE(స్థితి_3)) { తిరిగి *ఇది; }
    transform(విలువ_str.దత్తాంశం(), విలువ_str.length());
    ఒకవేళ (!ultag_isUnicodeLocaleAttribute(విలువ_str.దత్తాంశం(), విలువ_str.length())) {
        స్థితి_3 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి *ఇది;
    }
    ఒకవేళ (extensions_ == శూన్యసూచిక) {
        extensions_ = Locale::get_మూలం().clone();
        ఒకవేళ (extensions_ == శూన్యసూచిక) {
            స్థితి_3 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి *ఇది;
        }
        extensions_->సమితి_Keyword_విలువ(k_Attribute_కీలకం, విలువ_str.దత్తాంశం(), స్థితి_3);
        తిరిగి *ఇది;
    }

    UError_సంకేతం local_Error_సంకేతం = U_ZERO_ERROR;
    స్వయంచాలక లక్షణాలు = extensions_->get_Keyword_విలువ<CharString>(k_Attribute_కీలకం, local_Error_సంకేతం);
    ఒకవేళ (U_FAILURE(local_Error_సంకేతం)) {
        CharString new_లక్షణాలు(విలువ_str.దత్తాంశం(), స్థితి_3);
        // No attributes, set the attribute.
        extensions_->సమితి_Keyword_విలువ(k_Attribute_కీలకం, new_లక్షణాలు.దత్తాంశం(), స్థితి_3);
        తిరిగి *ఇది;
    }

    transform(లక్షణాలు.దత్తాంశం(),లక్షణాలు.length());
    స్థిరరాశి అక్షరం* ప్రారంభం = లక్షణాలు.దత్తాంశం();
    స్థిరరాశి అక్షరం* హద్దు = లక్షణాలు.దత్తాంశం() + లక్షణాలు.length();
    CharString new_లక్షణాలు;
    తార్కిక inserted = అబద్ధం;
    అయితే (ప్రారంభం < హద్దు) {
        ఒకవేళ (!inserted) {
            పూర్ణాంకం cmp = uprv_strcmp(ప్రారంభం, విలువ_str.దత్తాంశం());
            ఒకవేళ (cmp == 0) { తిరిగి *ఇది; }  // Found it in attributes: Just return
            ఒకవేళ (cmp > 0) {
                ఒకవేళ (!new_లక్షణాలు.is_ఖాళీలు()) new_లక్షణాలు.append('_', స్థితి_3);
                new_లక్షణాలు.append(విలువ_str.దత్తాంశం(), స్థితి_3);
                inserted = నిజం;
            }
        }
        ఒకవేళ (!new_లక్షణాలు.is_ఖాళీలు()) {
            new_లక్షణాలు.append('_', స్థితి_3);
        }
        new_లక్షణాలు.append(ప్రారంభం, స్థితి_3);
        ప్రారంభం += uprv_strlen(ప్రారంభం) + 1;
    }
    ఒకవేళ (!inserted) {
        ఒకవేళ (!new_లక్షణాలు.is_ఖాళీలు()) {
            new_లక్షణాలు.append('_', స్థితి_3);
        }
        new_లక్షణాలు.append(విలువ_str.దత్తాంశం(), స్థితి_3);
    }
    // Not yet in the attributes, set the attribute.
    extensions_->సమితి_Keyword_విలువ(k_Attribute_కీలకం, new_లక్షణాలు.దత్తాంశం(), స్థితి_3);
    తిరిగి *ఇది;
}

LocaleBuilder& LocaleBuilder::removeUnicodeLocaleAttribute(StringPiece value) U_LIFETIME_BOUND
{
    CharString విలువ_str(value, స్థితి_3);
    ఒకవేళ (U_FAILURE(స్థితి_3)) { తిరిగి *ఇది; }
    transform(విలువ_str.దత్తాంశం(), విలువ_str.length());
    ఒకవేళ (!ultag_isUnicodeLocaleAttribute(విలువ_str.దత్తాంశం(), విలువ_str.length())) {
        స్థితి_3 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి *ఇది;
    }
    ఒకవేళ (extensions_ == శూన్యసూచిక) { తిరిగి *ఇది; }
    UError_సంకేతం local_Error_సంకేతం = U_ZERO_ERROR;
    స్వయంచాలక లక్షణాలు = extensions_->get_Keyword_విలువ<CharString>(k_Attribute_కీలకం, local_Error_సంకేతం);
    // get failure, just return
    ఒకవేళ (U_FAILURE(local_Error_సంకేతం)) { తిరిగి *ఇది; }
    // Do not have any attributes, just return.
    ఒకవేళ (లక్షణాలు.is_ఖాళీలు()) { తిరిగి *ఇది; }

    అక్షరం* p = లక్షణాలు.దత్తాంశం();
    // Replace null terminiator in place for _ and - so later
    // we can use uprv_strcmp to compare.
    కొరకు (int32_t i = 0; i < లక్షణాలు.length(); i++, p++) {
        *p = (*p == '_' || *p == '-') ? '\0' : uprv_tolower(*p);
    }

    స్థిరరాశి అక్షరం* ప్రారంభం = లక్షణాలు.దత్తాంశం();
    స్థిరరాశి అక్షరం* హద్దు = లక్షణాలు.దత్తాంశం() + లక్షణాలు.length();
    CharString new_లక్షణాలు;
    తార్కిక దొరికింది = అబద్ధం;
    అయితే (ప్రారంభం < హద్దు) {
        ఒకవేళ (uprv_strcmp(ప్రారంభం, విలువ_str.దత్తాంశం()) == 0) {
            దొరికింది = నిజం;
        } లేకపోతే {
            ఒకవేళ (!new_లక్షణాలు.is_ఖాళీలు()) {
                new_లక్షణాలు.append('_', స్థితి_3);
            }
            new_లక్షణాలు.append(ప్రారంభం, స్థితి_3);
        }
        ప్రారంభం += uprv_strlen(ప్రారంభం) + 1;
    }
    // Found the value in attributes, set the attribute.
    ఒకవేళ (దొరికింది) {
        extensions_->సమితి_Keyword_విలువ(k_Attribute_కీలకం, new_లక్షణాలు.దత్తాంశం(), స్థితి_3);
    }
    తిరిగి *ఇది;
}

LocaleBuilder& LocaleBuilder::clear() U_LIFETIME_BOUND
{
    స్థితి_3 = U_ZERO_ERROR;
    language_[0] = 0;
    script_[0] = 0;
    region_[0] = 0;
    తొలగించు variant_;
    variant_ = శూన్యసూచిక;
    clearExtensions();
    తిరిగి *ఇది;
}

LocaleBuilder& LocaleBuilder::clearExtensions() U_LIFETIME_BOUND
{
    తొలగించు extensions_;
    extensions_ = శూన్యసూచిక;
    తిరిగి *ఇది;
}

Locale makeBogusLocale() {
  Locale bogus;
  bogus.సమితి_గమ్యం_Bogus();
  తిరిగి bogus;
}

శూన్య LocaleBuilder::copyExtensionsFrom(స్థిరరాశి Locale& src, UError_సంకేతం& error_సంకేతం)
{
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి; }
    Local_సూచిక<icu::StringEnumeration> keywords(src.createKeywords(error_సంకేతం));
    ఒకవేళ (U_FAILURE(error_సంకేతం) || keywords.isNull() || keywords->count(error_సంకేతం) == 0) {
        // Error, or no extensions to copy.
        తిరిగి;
    }
    ఒకవేళ (extensions_ == శూన్యసూచిక) {
        extensions_ = Locale::get_మూలం().clone();
        ఒకవేళ (extensions_ == శూన్యసూచిక) {
            స్థితి_3 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి;
        }
    }
    _copyExtensions(src, keywords.getAlias(), *extensions_, అబద్ధం, error_సంకేతం);
}

Locale LocaleBuilder::build(UError_సంకేతం& error_సంకేతం)
{
    ఒకవేళ (U_FAILURE(error_సంకేతం)) {
        తిరిగి makeBogusLocale();
    }
    ఒకవేళ (U_FAILURE(స్థితి_3)) {
        error_సంకేతం = స్థితి_3;
        తిరిగి makeBogusLocale();
    }
    CharString locale_str(language_, error_సంకేతం);
    ఒకవేళ (uprv_strlen(script_) > 0) {
        locale_str.append('-', error_సంకేతం).append(StringPiece(script_), error_సంకేతం);
    }
    ఒకవేళ (uprv_strlen(region_) > 0) {
        locale_str.append('-', error_సంకేతం).append(StringPiece(region_), error_సంకేతం);
    }
    ఒకవేళ (variant_ != శూన్యసూచిక) {
        locale_str.append('-', error_సంకేతం).append(StringPiece(variant_->దత్తాంశం()), error_సంకేతం);
    }
    ఒకవేళ (U_FAILURE(error_సంకేతం)) {
        తిరిగి makeBogusLocale();
    }
    Locale లబ్ధం(locale_str.దత్తాంశం());
    ఒకవేళ (extensions_ != శూన్యసూచిక) {
        _copyExtensions(*extensions_, శూన్యసూచిక, లబ్ధం, నిజం, error_సంకేతం);
    }
    ఒకవేళ (U_FAILURE(error_సంకేతం)) {
        తిరిగి makeBogusLocale();
    }
    తిరిగి లబ్ధం;
}

UBool LocaleBuilder::copy_Error_గమ్యం(UError_సంకేతం &బహిర్గతం_Error_సంకేతం) స్థిరరాశి {
    ఒకవేళ (U_FAILURE(బహిర్గతం_Error_సంకేతం)) {
        // Do not overwrite the older error code
        తిరిగి నిజం;
    }
    బహిర్గతం_Error_సంకేతం = స్థితి_3;
    తిరిగి U_FAILURE(బహిర్గతం_Error_సంకేతం);
}

U_NAMESPACE_చివర
