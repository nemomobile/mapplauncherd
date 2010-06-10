#include "calcretranslator.h"

#include <QDebug>

#include "mgconfitem.h"
#include <MLocale>

void CalcRetranslator::calcRetranslate()
{
    qDebug() << Q_FUNC_INFO << endl;
    MGConfItem languageItem( "/M/i18n/Language" );
    QString language = languageItem.value().toString();
    MLocale locale(language);
    locale.installTrCatalog( "essentials" );
    MLocale::setDefault( locale );
}

