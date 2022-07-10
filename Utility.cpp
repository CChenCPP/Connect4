#include "Utility.h"

QString Parse::toQString(std::string& str)
{
    return QString::fromUtf8(str);
}
