#ifndef CALCDOMREADER_H
#define CALCDOMREADER_H

#include <QDomDocument>
#include <QTextStream>
#include <QFile>
#include <QDebug>
#include "calculationitem.h"

class CalcDomReader : public QDomDocument
{
    public:
	CalcDomReader( QFile& file, const QString& name = QString("CalcML" ) );
	bool readItem( int itemNumber, CalculationItem& calcItem );
	int count() const;
	QDomElement getRoot();
	void cloneRoot( QDomElement& );
    private:
	QDomElement  root;
	QDomNodeList itemList;
};

#endif

