//
// Created by LEGION on 2025/12/10.
//

#include "utils.h"
#include <QLatin1StringView>
using namespace Qt::Literals::StringLiterals;

QString utils::toHtmlEscaped(const QString &text) {
    const auto pos = std::u16string_view(text).find_first_of(u"<>&\"'");
    if (pos == std::u16string_view::npos)
        return text;
    QString rich;
    const qsizetype len = text.size();
    rich.reserve(static_cast<qsizetype>(len * 1.1));
    rich += qToStringViewIgnoringNull(text).first(pos);
    for (auto ch : qToStringViewIgnoringNull(text).sliced(pos)) {
        if (ch == u'<')
            rich += "&lt;"_L1;
        else if (ch == u'>')
            rich += "&gt;"_L1;
        else if (ch == u'&')
            rich += "&amp;"_L1;
        else if (ch == u'"')
            rich += "&quot;"_L1;
        else if (ch == u'\'')
            rich += "&apos;"_L1;
        else
            rich += ch;
    }
    rich.squeeze();
    return rich;
}
