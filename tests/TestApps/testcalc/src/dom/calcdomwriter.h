#ifndef CALCDOMWRITER_H
#define CALCDOMWRITER_H

#include <QDomDocument>
#include <QTextStream>
#include <QFile>
#include <QDebug>
#include "calculationitem.h"

class CalcDomWriter : public QDomDocument
{
    public:
	CalcDomWriter( const QString& name = QString("CalcML" ) );
	CalcDomWriter( const QDomElement& oldRoot, const QString& name = QString("CalcML" ) );
	void addItem( const CalculationItem& );
	bool removeOldestItem();
	bool removeByIndex( const QString& index );
	bool renameByIndex( const QString& index, const QString& newname );
	bool existsCalculationItem( const QString& calcName );
	bool saveToFile( QFile& );
	int count() const;
    private:
	QDomElement calculationItemToNode( const CalculationItem& );
	QDomElement root;
};

#endif

