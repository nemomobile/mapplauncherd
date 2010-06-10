#include "calcdomwriter.h"

CalcDomWriter::CalcDomWriter( const QString& name ) 
            : QDomDocument( name ) 
{
    root = this->createElement( name );
    this->appendChild( root );
}

CalcDomWriter::CalcDomWriter( const QDomElement& oldRoot, const QString& name )
		: QDomDocument( name )
{
    // following line does a shallow copy
//    root = oldRoot;
    // this line does deep copy
    root = oldRoot.cloneNode().toElement();
    root.setTagName( name );
    this->appendChild( root );
}

int CalcDomWriter::count() const
{
    return root.childNodes().length();
}

void CalcDomWriter::addItem( const CalculationItem& calcItem )
{
    root.appendChild( calculationItemToNode( calcItem ) );
    if( root.hasChildNodes() )
    {
	QDomNodeList itemList = root.childNodes();
	for( int i = 0; i < itemList.length(); i++ )
	{
	    QDomElement element = itemList.item(i).toElement();
	}
    }
}

bool CalcDomWriter::removeOldestItem()
{
    if( root.hasChildNodes() )
    {
	root.removeChild( root.firstChild() );
	return true;
    }

    return false;
}

bool CalcDomWriter::removeByIndex( const QString& index )
{
    for(QDomNode node = root.firstChild(); !node.isNull(); node = node.nextSibling() )
    {
	if( node.toElement().attribute( "index" ) == index )
	{
	    root.removeChild(node);
	    return true;
	}
    }
    return false;
}

bool CalcDomWriter::renameByIndex( const QString& index, const QString& rename )
{
    for(QDomNode node = root.firstChild(); !node.isNull(); node = node.nextSibling() )
    {
	if( node.toElement().attribute( "index" ) == index )
	{
	    // now need to rename the calculation item here
	    node.toElement().setAttribute( "name", rename );
            return true;
	}
    }
    return false;
}

bool CalcDomWriter::existsCalculationItem( const QString& calcName )
{
    for(QDomNode node = root.firstChild(); !node.isNull(); node = node.nextSibling() )
	if( node.toElement().attribute( "name" ) == calcName )
            return true;
    return false;
}

QDomElement CalcDomWriter::calculationItemToNode( const CalculationItem& calcItem )
{
    QDomElement item = this->createElement( "calculation" );
    
    item.setAttribute( "name", calcItem.value( "name" ) );
    item.setAttribute( "datetime", calcItem.value( "datetime" ) );
    item.setAttribute( "result", calcItem.value( "result" ) );
    item.setAttribute( "input", calcItem.value( "input" ) );
    item.setAttribute( "index", calcItem.value( "index" ) );
    
    return item;
}

bool CalcDomWriter::saveToFile( QFile& file )
{
    if( !file.open( QIODevice::WriteOnly ) )
	return false;

    if( root.hasChildNodes() )
    {
	QDomNodeList itemList = root.childNodes();
	for( int i = 0; i < itemList.length(); i++ )
	{
	    QDomElement element = itemList.item(i).toElement();
	}
    }

    QTextStream ts( &file );
    ts << this->toString();

    file.close();

    return true;
}


