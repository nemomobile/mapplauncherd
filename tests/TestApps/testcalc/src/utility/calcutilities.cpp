#include "calcconstants.h"
#include "calcutilities.h"

CalcUtilities* CalcUtilities::utility = NULL;

CalcUtilities::CalcUtilities() {}

CalcUtilities* CalcUtilities::instance()
{
    if( utility == NULL )
	utility = new CalcUtilities();

    return utility;
}

void CalcUtilities::showNotificationBanner( const QString& banner, const QString& icon )
{
    MInfoBanner* notification = new MInfoBanner( MInfoBanner::Information );
//    notification->setIconID( icon );
    notification->setBodyText( "<font color=\"white\" size=\"1\">" + banner + "</font>" );

    QTimer* timer = new QTimer( notification );
    connect(
            timer,
            SIGNAL(timeout()),
            this,
            SLOT(notificationTimeout()) );
    timer->setSingleShot( true );
    timer->start( CALC_BANNER_SHORT_TIMER );
    notification->appear( MSceneWindow::DestroyWhenDone );
}

void CalcUtilities::notificationTimeout()
{
    QTimer* timer = qobject_cast<QTimer*>(sender());
    if( timer )
    {
        MInfoBanner* notification = qobject_cast<MInfoBanner*>(timer->parent());
        if( notification )
            notification->disappear();
    }
}

void CalcUtilities::convertToExponential( QString& numeral )
{
    QString backup = numeral;
    
    if( (numeral.startsWith( "-" )) && 
	   ( (!numeral.contains( "." ) && numeral.length() <= MAX_VALUE_LENGTH + 1) ||
	     (numeral.contains( "." ) && numeral.length() <= MAX_VALUE_LENGTH + 2) ) )
	// include the negative sign also while considering the maximum permissible length in non-scientific format
    {
	qDebug() << "don't convert just return back" << endl;
	return;
    }
    else if( !numeral.startsWith("-") &&
             numeral.contains( "." ) &&
	     numeral.length() <= MAX_VALUE_LENGTH+1 )
	return;

    std::cout.precision( MAX_VALUE_LENGTH );
    std::cout.flags( std::ios_base::scientific );
    std::ostringstream os( std::ostringstream::out );
    os.precision( MAX_VALUE_LENGTH );
    os << numeral.toDouble();
    numeral = os.str().data();

    // Flush the output string stream for future use again
    os.str( "" );

    if( numeral.contains( "e" ) )
    {
	QStringList parts = numeral.split( "e" );
	if( parts[1].toInt() < -MAX_VALUE_LENGTH )
	{
	    numeral = "0";
	    showNotificationBanner( TXT_CALC_NOTE_CALC_LIMIT, ICO_CALC_CROSS );
	    return;
	}
        int choplen = numeral.length() - numeral.indexOf("e");
        std::cout.precision( MAX_VALUE_LENGTH - choplen );
        std::cout.flags( std::ios_base::scientific );
        os.precision( MAX_VALUE_LENGTH - choplen );
        os << numeral.toDouble();
        numeral = os.str().data();
    }
    else if( ( numeral.startsWith( "0." )  && numeral.length() > MAX_VALUE_LENGTH+1) ||
	     ( numeral.startsWith( "-0." )  && numeral.length() > MAX_VALUE_LENGTH+2) )
    {
	// use the backup for truncation/roundoff
	int choplen = numeral.length() - MAX_VALUE_LENGTH + 1;

	// use the regular expression to chop additional numbers if there are leading zeros after 
	// decimal point
	QRegExp rx( "\\.0*" );
	int idx = rx.indexIn( numeral, 0 );
        qDebug() << "idx = " << idx << endl;
	int matchLength = rx.matchedLength();
	choplen += matchLength - 1;
	
	std::cout.precision( MAX_VALUE_LENGTH - choplen );
	os.precision( MAX_VALUE_LENGTH - choplen );
	
	std::cout.precision( numeral.length() - choplen );
	os.precision( numeral.length() - choplen );
	
        std::cout.flags( std::ios_base::scientific );
        os << backup.toDouble();
        numeral = os.str().data();
    }
}

