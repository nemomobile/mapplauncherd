#include "calcdomreader.h"

CalcDomReader::CalcDomReader( QFile& file, const QString& name ) 
            : QDomDocument( name ) 
{
    file.open( QIODevice::ReadOnly );
    this->setContent( &file );
    file.close();

    root = this->documentElement();

    if( root.hasChildNodes() )
    {
        itemList = root.childNodes();
	for( int i = 0; i < itemList.length(); i++ )
	{
	    QDomElement element = itemList.item(i).toElement();
	}
    }
}

bool CalcDomReader::readItem( int itemNumber, CalculationItem& calcItem )
{
    if( itemNumber >= 0 && itemNumber < itemList.count() )
    {
        QDomElement element = itemList.item(itemNumber).toElement();
	if(  element.tagName() == "calculation" )
	{
	    calcItem["name"] = element.attribute( "name", "" );
	    calcItem["datetime"] = element.attribute( "datetime", "" );
	    calcItem["result"] = element.attribute( "result", "" );
	    calcItem["input"] = element.attribute( "input", "" );
	    calcItem["index"] = element.attribute( "index", "" );
	    return true;
	}
    }

    return false;
}

int CalcDomReader::count() const
{
    return itemList.count();
}

QDomElement CalcDomReader::getRoot()
{
    return root;
}

void CalcDomReader::cloneRoot( QDomElement& r )
{
    r = root.cloneNode().toElement();
}

