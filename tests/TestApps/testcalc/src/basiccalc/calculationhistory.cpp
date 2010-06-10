
#include "calculationhistory.h"

#define CALCULATION_DATETIME "CalcDateTime"
#define CALCULATION_RESULT   "CalcResult"
#define CALCULATION_INPUT    "CalcInput"
#define CALCULATION_INDEX    "CalcIndex"

#include <MWidgetCreator>
M_REGISTER_WIDGET( CalculationHistory )

CalculationHistoryDataModel::CalculationHistoryDataModel( QObject* parent )
				:QAbstractListModel(parent)
{
    theHeaderData << CALCULATION_DATETIME 
	    << CALCULATION_RESULT 
	    << CALCULATION_INPUT 
	    << CALCULATION_INDEX;
}

CalculationHistoryDataModel::~CalculationHistoryDataModel()
{
}

int CalculationHistoryDataModel::rowCount( const QModelIndex& parent ) const
{
    Q_UNUSED( parent );
    return theData.size();
}

int CalculationHistoryDataModel::columnCount ( const QModelIndex & parent ) const
{
    Q_UNUSED(parent);
    return theHeaderData.size();
}

QVariant CalculationHistoryDataModel::data ( const QModelIndex & index, int role ) const
{
    Q_UNUSED(index);
    Q_UNUSED(role);
    if(role == Qt::DisplayRole) 
    {
	if( index.isValid() && index.row() < theData.size() )
	{
	    QStringList rowData = theData[index.row()];
	    if( index.column() < rowData.size() )
		return QVariant( rowData[index.column()] );
	}
    }
    
    return QVariant();
}

QVariant CalculationHistoryDataModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    Q_UNUSED( orientation );

    if( role == Qt::DisplayRole )
	if( section < theHeaderData.size() )
	    return QVariant( theHeaderData.at( section ) );

    return QVariant();
}

bool CalculationHistoryDataModel::insertRows( int row, int count, const QModelIndex& parent )
{
    beginInsertRows( parent, row, row + count - 1 );
    QDir::setCurrent( QDir::homePath() + "/" + CALC_HIDDENDIR );
    QFile file( CALCULATOR_HISTORY );
    file.setPermissions( QFile::ReadOwner |
		            QFile::WriteOwner |
			    QFile::ReadGroup |
			    QFile::ReadOther );
    if( file.exists() && count > 0 )
    {
	CalcDomReader reader( file );
	for( int i = row; i < count; i++ )
	{
	    QStringList data;
	    if( !populateHeaderData( reader, row+count-i-1, data ) )
	    {
		endInsertRows();
		return false;
	    }
	    theData.insert( row+i, data );
	}
    }

    endInsertRows();
    return true;
}

bool CalculationHistoryDataModel::insertRow( int row, const QModelIndex& parent )
{
    beginInsertRows( parent, row, row );
    endInsertRows();
    QDir::setCurrent( QDir::homePath() + "/" + CALC_HIDDENDIR );
    QFile file( CALCULATOR_HISTORY );
    file.setPermissions( QFile::ReadOwner |
		            QFile::WriteOwner |
			    QFile::ReadGroup |
			    QFile::ReadOther );
    if( file.exists() )
    {
	CalcDomReader reader( file );
	QStringList data;
	if( !populateHeaderData( reader, row, data ) )
	{
	    endInsertRows();
	    return false;
	}
	theData.insert( row, data );
    }
    return true;
}

bool CalculationHistoryDataModel::populateHeaderData( CalcDomReader& reader, int row, QStringList& data )
{
    CalculationItem item;
    reader.readItem( row, item );
    data << item.value( "datetime" ) 
	    << item.value( "result" )
	    << item.value( "input" )
	    << item.value( "index" );
    return true;
}

bool CalculationHistoryDataModel::removeRow( int row, const QModelIndex& parent )
{
    beginRemoveRows( parent, row, row );
    theData.remove( row );
    endRemoveRows();
    return true;
}

bool CalculationHistoryDataModel::removeRows( int row, int count, const QModelIndex& parent )
{
    if( count < 1 )
	return false;
    beginRemoveRows( parent, row, row + count - 1 );

    for( int i = row; i < count; count-- )
	theData.remove( i );
    
    endRemoveRows();
    return true;
}

CalculationHistoryCellCreator::CalculationHistoryCellCreator(QObject * p):size(QSize(-1, -1) ),parent(p)
{
    MWidget* cell = new CalculationHistoryItem;
    size = cell->effectiveSizeHint(Qt::PreferredSize);
    delete cell;
}

QSizeF CalculationHistoryCellCreator::cellSize() const
{
    return size;
}

MWidget* CalculationHistoryCellCreator::createCell( 
                                               const QModelIndex & index, 
					       MWidgetRecycler & recycler) const
{
    CalculationHistoryItem* cell = dynamic_cast<CalculationHistoryItem*>(recycler.take( "CalculationHistoryItem" ) );
    
    if( !cell )
    {
	cell = new CalculationHistoryItem;
	QObject::connect(
		cell,
		SIGNAL( clicked() ),
		parent,
		SLOT( handleHistoryItemClicked() ) );
	    
	MAction* action;
	action = new MAction( TXT_CALC_SAVE, cell );
	action->setLocation( MAction::ObjectMenuLocation );
	cell->addAction( action );
	QObject::connect(
	    action,
	    SIGNAL(triggered()),
	    parent,
	    SLOT(initiateSaveCalculation()) );
        
	action = new MAction( TXT_CALC_DELETE, cell );
	action->setLocation( MAction::ObjectMenuLocation );
	cell->addAction( action );

	QObject::connect(
	    action,
	    SIGNAL(triggered()),
	    parent,
	    SLOT(initiateDeleteCalculation()) );
    }
    
    updateCell( index, cell );
    
    return cell;
}

void CalculationHistoryCellCreator::updateCell(
                                        const QModelIndex& index, 
					MWidget * cell) const
{
    qDebug() << Q_FUNC_INFO << endl;
    CalculationHistoryItem* item = qobject_cast<CalculationHistoryItem*>(cell);
    item->setTimeOfCalculation( index.sibling( index.row(), 0 ).data().toString() );
    item->setResult( index.sibling( index.row(), 1 ).data().toString() );
    QString inputString = index.sibling( index.row(), 2 ).data().toString();
    if( inputString.contains( TXT_CALC_NOTE_CALC_LIMIT ) ||
	inputString.contains( TXT_CALC_NOTE_INDICATION ) )
    {
	qDebug() << "elide left for " << inputString << endl;
	item->setElide( Qt::ElideLeft );
    }
    else
    {
	qDebug() << "elide right for " << inputString << endl;
	item->setElide( Qt::ElideRight );
    }
    item->setInput( inputString );
    item->setIndex( index.sibling( index.row(), 3 ).data().toString() );
    updateContentItemMode(index, item );
}

void CalculationHistoryCellCreator::updateContentItemMode(const QModelIndex& index, CalculationHistoryItem* contentItem) const
{
    int row = index.row();
    bool thereIsNextRow= index.sibling(row + 1, 0).isValid();
    if( row == 0 && !thereIsNextRow )
        contentItem->setItemMode(CalculationHistoryItem::Single );
    else if(row == 0)
        contentItem->setItemMode(CalculationHistoryItem::SingleColumnTop);
    else if(thereIsNextRow)
        contentItem->setItemMode(CalculationHistoryItem::SingleColumnCenter);
    else 
        contentItem->setItemMode(CalculationHistoryItem::SingleColumnBottom);
}

DeleteHistoryItemsCellCreator::DeleteHistoryItemsCellCreator( QObject* p )
                                  : CalculationHistoryCellCreator( p )
{
}

MWidget* DeleteHistoryItemsCellCreator::createCell( 
                                const QModelIndex & index, 
				MWidgetRecycler & recycler) const
{
    CalculationHistoryItem* cell = dynamic_cast<CalculationHistoryItem*>(recycler.take( "CalculationHistoryItem" ) );
    
    if( !cell )
    {
	cell = new CalculationHistoryItem;
    }
    
    updateCell( index, cell );
    
    return cell;
}

CalculationHistory::CalculationHistory() 
	             : model( NULL ), 
		       confirmDialog(NULL), 
		       deleteCandidate( NULL ),
		       deleteHistoryItems( NULL ),
		       centralWidgetLayout( NULL ),
		       list( NULL )
{
    utility = CalcUtilities::instance();
    setTitle( TXT_CALC_HISTORY_TITLE );
    setPannable( true );
    model = new CalculationHistoryDataModel(this);
}

CalculationHistory::~CalculationHistory()
{
    delete model;
    delete confirmDialog;
}

void CalculationHistory::createContent()
{
    MApplicationPage::createContent();

    this->setObjectName( "calcpage" );

    MLabel* emptyHistory = new MLabel;
    MLayout* emptyHistoryLayout = new MLayout( emptyHistory );
    emptyHistoryLayoutPolicy = new MLinearLayoutPolicy( emptyHistoryLayout, Qt::Vertical );
    emptyHistory->setLayout( emptyHistoryLayout );

    MLabel* labelEmpty = new MLabel( TXT_CALC_HISTORY_BLANK );
    MLabel* labelFiller = new MLabel( "" );

    labelEmpty->setAlignment( Qt::AlignHCenter );

    emptyHistoryLayoutPolicy->addItem( labelEmpty );
    emptyHistoryLayoutPolicy->addItem( labelFiller );

    emptyHistory->setObjectName( "emptyHistory" );
    labelEmpty->setObjectName( "emptyLabel" );

    list = new MList();
    list->setViewType( "fast" );
    list->setObjectName( "CalculationHistoryList" );
    list->setSelectionMode( MList::SingleSelection );
    list->setShowGroups(false);
    list->setCellCreator(new CalculationHistoryCellCreator(this));
    list->setItemModel(model);
    
    connect( this, SIGNAL( appeared() ), list->selectionModel(), SLOT( clearSelection() ) );
    
    deleteHistoryItems = new DeleteHistoryItems(model);
    connect( 
	deleteHistoryItems, 
	SIGNAL(deleteItems(QModelIndexList&)), 
	this,
	SLOT(initiateDeleteMultipleCalculations(QModelIndexList& )) );

    // Take the central widget and have two policies for it
    // 1) Empty layout policy: which will be applicable when there's no calculation history
    // 2) List layout Policy: which will be applicable when there are items in the calculation
    //    history to be displayed
    centralWidgetLayout = new MLayout( centralWidget() );
    centralEmptyPolicy = new MLinearLayoutPolicy( centralWidgetLayout, Qt::Vertical );
    centralListPolicy = new MLinearLayoutPolicy( centralWidgetLayout, Qt::Vertical );

    centralEmptyPolicy->addItem( emptyHistory );
    centralListPolicy->addItem( list );

    this->updateContent();
    
    /************************************************************************************
     * create the delete confirmation dialog here, it can be reused later whenever user *
     * tries deleting all the calculations from calculation history.                    *
     * **********************************************************************************/
    confirmDialog = new MMessageBox( TXT_CALC_HISTORY_DEL_CONF, M::YesButton | M::NoButton );

    connect(
            confirmDialog,
            SIGNAL(accepted()),
            this,
            SLOT(confirmClearAccept()));

    connect(
            confirmDialog,
            SIGNAL(rejected()),
            this,
            SLOT(confirmClearReject()));
}

void CalculationHistory::updateContent()
{
    qDebug() << Q_FUNC_INFO << endl;
    if( !centralWidgetLayout )
	return;
    QDir::setCurrent( QDir::homePath() + "/" + CALC_HIDDENDIR );
    QFile file( CALCULATOR_HISTORY );
    file.setPermissions( QFile::ReadOwner |
		            QFile::WriteOwner |
			    QFile::ReadGroup |
			    QFile::ReadOther );
    if( file.exists() )
    {
	CalcDomReader reader( file );
	
	if( model->rowCount() > 0 )
	{
	    model->removeRows( 0, model->rowCount() );
	}
	if( reader.count() > 0 )
	{
	    model->insertRows( model->rowCount(), reader.count() );
	    centralWidgetLayout->setPolicy( centralListPolicy );
	    this->setPannable( true );
	    
	    if( this->actions().count() == 0 )
	    {
                MAction* action = new MAction( TXT_CALC_DEL_LIST, this );
                action->setLocation( MAction::ApplicationMenuLocation );
                this->addAction( action );
                connect( action, SIGNAL(triggered()), this, SLOT( showDeleteCalculationsPage() ) );
	    }
	}
	else
	{
	    centralWidgetLayout->setPolicy( centralEmptyPolicy );
	    this->setPannable( false );
	    this->clearActions();
	}
    }
    else
    {
	centralWidgetLayout->setPolicy( centralEmptyPolicy );
	this->setPannable( false );
	this->clearActions();
    }

    list->selectionModel()->clearSelection();
}

void CalculationHistory::handleHistoryItemClicked( )
{
    CalculationHistoryItem *object = qobject_cast<CalculationHistoryItem*>(sender());
    // need to emit a signal here to return back to the main calculation view screen
    emit this->itemClicked( object->result() );
}

void CalculationHistory::initiateSaveCalculation()
{
    qDebug() << Q_FUNC_INFO << endl;
    CalculationHistoryItem* item = qobject_cast<CalculationHistoryItem*>(sender()->parent());
    if( item )
    {
	qDebug() << "Got 'im!" << endl;
	qDebug() << "input = " << item->input() << endl
		 << "result = " << item->result() << endl
		 << "datetime = " << item->timeOfCalculation()
	         << "index = " << item->index() << endl;
	qDebug() << "show name of calculation dialog" << endl;
	CalculationItem calcItem;
	calcItem["name"] = "";
	calcItem["datetime"] = item->timeOfCalculation();
	calcItem["input"] = item->input();
	calcItem["result"] = item->result();
	emit save(calcItem);
    }
}

void CalculationHistory::initiateDeleteCalculation()
{
    qDebug() << Q_FUNC_INFO << endl;
    CalculationHistoryItem* item = qobject_cast<CalculationHistoryItem*>(sender()->parent());
    deleteCandidate = item;

    MLocale locale;
    
    QString mystring;
    QString numeral = item->result();
    if( numeral.length() > MAX_VALUE_LENGTH )
	utility->convertToExponential( numeral );

    if( numeral.contains("e") )
    {
	QStringList splitText = numeral.split(  QChar('e') );
	qDebug() << "splitText[0]=" << splitText[0] << endl;
	qDebug() << "splitText[1]=" << splitText[1] << endl;
	mystring = locale.formatNumber( splitText[0].toDouble(), MAX_VALUE_LENGTH ) 
			+ "e" ;
	if( splitText[1].toDouble() > 0.0 )
	    mystring += QString("+");
	mystring += locale.formatNumber( splitText[1].toDouble(), MAX_VALUE_LENGTH );
    }
    else if( numeral.contains("E") )
    {
	QStringList splitText = numeral.split(  QChar('E') );
	qDebug() << "splitText[0]=" << splitText[0] << endl;
	qDebug() << "splitText[1]=" << splitText[1] << endl;
	mystring = locale.formatNumber( splitText[0].toDouble(), MAX_VALUE_LENGTH ) 
			+ "E" ;
	if( splitText[1].toDouble() > 0.0 )
	    mystring += QString("+");
	mystring += locale.formatNumber( splitText[1].toDouble(), MAX_VALUE_LENGTH );
    }
    else
	mystring = locale.formatNumber( numeral.toDouble(), MAX_VALUE_LENGTH );

    confirmDialog->setText( TXT_CALC_HISTORY_DEL_CONF.arg( mystring ) );
    confirmDialog->appear();
}

void CalculationHistory::initiateDeleteMultipleCalculations(QModelIndexList& selectedItems )
{
    qDebug() << Q_FUNC_INFO << endl;

    QListIterator<QModelIndex> it( selectedItems );
    it.toBack();
    
    while( it.hasPrevious() )
    {
	QModelIndex index = it.previous();
	qDebug() << "===========value=============" << endl;
	qDebug() << "time of calc = " << index.sibling( index.row(), 0 ).data().toString() << endl;
	qDebug() << "result = " << index.sibling( index.row(), 1 ).data().toString() << endl;
	qDebug() << "input = " << index.sibling( index.row(), 2 ).data().toString() << endl;
	qDebug() << "index = " << index.sibling( index.row(), 3 ).data().toString() << endl;
	qDebug() << "=============================" << endl;
	QString itemIndex = index.sibling( index.row(), 3 ).data().toString();
	
        QDir::setCurrent( QDir::homePath() + "/" + CALC_HIDDENDIR );
        QFile file( CALCULATOR_HISTORY );
        file.setPermissions( QFile::ReadOwner |
		            QFile::WriteOwner |
			    QFile::ReadGroup |
			    QFile::ReadOther );
        if( file.exists() )
        {
            CalcDomReader reader( file );
            QDomElement root;
            reader.cloneRoot( root );
	    CalcDomWriter writer( root );
	    if( writer.removeByIndex( itemIndex ) )
		qDebug() << "item removed!" << endl;
	    else
		qDebug() << "item wasn't removed" << endl;
	    writer.saveToFile( file );
	    qDebug() << "=============================" << endl;
	}
    }
    deleteHistoryItems->dismiss();
    
    this->updateContent();
}

void CalculationHistory::showDeleteCalculationsPage()
{
    deleteHistoryItems->appear( KeepWhenDone );
}

void CalculationHistory::confirmClearAccept()
{
    CalculationHistoryItem* item = deleteCandidate;
    if( item )
    {
	qDebug() << "Got 'im!" << endl;
	qDebug() << "input = " << item->input() << endl
		 << "result = " << item->result() << endl
		 << "datetime = " << item->timeOfCalculation()
	         << "index = " << item->index() << endl;
        QDir::setCurrent( QDir::homePath() + "/" + CALC_HIDDENDIR );
        QFile file( CALCULATOR_HISTORY );
        file.setPermissions( QFile::ReadOwner |
		            QFile::WriteOwner |
			    QFile::ReadGroup |
			    QFile::ReadOther );
        if( file.exists() )
        {
            CalcDomReader reader( file );
            QDomElement root;
            reader.cloneRoot( root );
	    CalcDomWriter writer( root );
	    if( writer.removeByIndex( item->index() ) )
		qDebug() << "item removed!" << endl;
	    else
		qDebug() << "item wasn't removed" << endl;
	    writer.saveToFile( file );
	}

	this->updateContent();
    }
}

void CalculationHistory::confirmClearReject()
{
    deleteCandidate = NULL;
}

void CalculationHistory::addItem( CalculationItem& calcItem )
{
    bool writeNewFile = false;
    QDir::setCurrent( QDir::homePath() );
    QDir dir( CALC_HIDDENDIR );
    QDir homeDir( QDir::homePath() );
    if( !dir.exists() )
	homeDir.mkdir( CALC_HIDDENDIR );
    QDir::setCurrent( QDir::homePath() + "/" + CALC_HIDDENDIR );
    QFile file( CALCULATOR_HISTORY );
    file.setPermissions( QFile::ReadOwner |
		            QFile::WriteOwner |
			    QFile::ReadGroup |
			    QFile::ReadOther );
    if( file.exists() )
    {
        CalcDomReader reader( file );
        if( reader.count() > 0 )
        {
            QDomElement root;
            reader.cloneRoot( root );
            CalcDomWriter writer( root );
	    if( writer.count() == MAX_CALCULATION_HISTORY_ITEMS )
		writer.removeOldestItem();
            writer.addItem( calcItem );
            writer.saveToFile( file );
        }
        else
            writeNewFile = true;
    }
    else
        writeNewFile = true;

    if( writeNewFile )
    {
        CalcDomWriter writer;
        writer.addItem( calcItem );
        writer.saveToFile( file );
    }
}

void CalculationHistory::retranslateUi()
{
    setTitle( TXT_CALC_HISTORY_TITLE );
    if( !isContentCreated() )
	return;

    this->updateContent();

    if( model->rowCount() > 0 )
	(this->actions()[0])->setText( TXT_CALC_DEL_LIST );

    ((MLabel*)emptyHistoryLayoutPolicy->itemAt( 0 ))->setText( TXT_CALC_HISTORY_BLANK );
}

DeleteHistoryItems::DeleteHistoryItems( CalculationHistoryDataModel* model )
	                      : confirmDialog( NULL )
{
    setTitle( TXT_CALC_HISTORY_TITLE );
    setPannable( true );
    dellist = new MList();
    dellist->setViewType( "fast" );
    dellist->setObjectName( "CalculationHistoryList" );
    dellist->setSelectionMode( MList::MultiSelection );
    dellist->setShowGroups(false);
    dellist->setCellCreator(new DeleteHistoryItemsCellCreator(this));
    dellist->setItemModel(model);
    connect( this, SIGNAL( appeared() ), dellist->selectionModel(), SLOT( clearSelection() ) );
}

DeleteHistoryItems::~DeleteHistoryItems() 
{
    delete confirmDialog;
}

void DeleteHistoryItems::createContent()
{
    MAction* action;
    action = new MAction( TXT_CALC_SELECT_ALL, this );
    action->setLocation( MAction::ApplicationMenuLocation );
    this->addAction( action );
    connect( action, SIGNAL( triggered() ), this, SLOT( handleSelectAll() ) );
    
    action = new MAction( TXT_CALC_UNSELECT_ALL, this );
    action->setLocation( MAction::ApplicationMenuLocation );
    this->addAction( action );
    connect( action, SIGNAL( triggered() ), dellist->selectionModel(), SLOT( clearSelection() ) );
    
    MLayout* delLayout = new MLayout( centralWidget() );
    MLinearLayoutPolicy* delPolicy = new MLinearLayoutPolicy( delLayout, Qt::Vertical );
    
    delPolicy->addItem( dellist );

    action = new MAction( this );
    action->setIconID( ICO_CALC_DELETE );
    action->setLocation( MAction::ToolBarLocation );
    this->addAction( action );
    connect( action, SIGNAL( triggered() ), this, SLOT( handleDeleteItems() ) );
    
    confirmDialog = new MMessageBox( TXT_CALC_DEL_SEL_CONF, M::YesButton | M::NoButton );

    connect(
            confirmDialog,
            SIGNAL(accepted()),
            this,
            SLOT(confirmDeleteAccept()));

    connect(
            confirmDialog,
            SIGNAL(rejected()),
            this,
            SLOT(confirmDeleteReject()));

    qDebug() << Q_FUNC_INFO << endl;
    qDebug() << "selected indices count = " << dellist->selectionModel()->selectedIndexes().size() << endl;
    dellist->selectionModel()->clear();
}

void DeleteHistoryItems::handleDeleteItems()
{
    qDebug() << Q_FUNC_INFO << endl;
    
    QModelIndexList selectedItems = dellist->selectionModel()->selectedIndexes();

    if( selectedItems.size() > 0 )
    {
	if( selectedItems.size() == 1 )
	    confirmDialog->setText( TXT_CALC_DEL_ONE_CONF.arg( selectedItems.size() ) );
	else if( selectedItems.size() < dellist->itemModel()->rowCount() )
            confirmDialog->setText( TXT_CALC_DEL_SEL_CONF.arg(selectedItems.size() ) );
	else if( selectedItems.size() == dellist->itemModel()->rowCount() )
            confirmDialog->setText( TXT_CALC_DEL_ALL_CONF );

        confirmDialog->appear();
    }
}

void DeleteHistoryItems::confirmDeleteAccept()
{
    qDebug() << Q_FUNC_INFO << endl;
    QModelIndexList selectedItems = dellist->selectionModel()->selectedIndexes();
    emit this->deleteItems( selectedItems );
}

void DeleteHistoryItems::confirmDeleteReject()
{
    qDebug() << Q_FUNC_INFO << endl;
}

void DeleteHistoryItems::handleSelectAll()
{
    QModelIndex first = dellist->itemModel()->index( 0, 0 );
    QModelIndex last = dellist->itemModel()->index(dellist->itemModel()->rowCount()-1, 0 );
    QItemSelection selection( first, last );
    QItemSelectionModel* selectionModel = dellist->selectionModel();
    selectionModel->select( selection, QItemSelectionModel::Select);
}

void DeleteHistoryItems::retranslateUi()
{
    setTitle( TXT_CALC_HISTORY_TITLE );
    if( !isContentCreated() )
	return;

    if( this->actions().count() == 2 )
    {
	(this->actions()[0])->setText( TXT_CALC_SELECT_ALL );
	(this->actions()[1])->setText( TXT_CALC_UNSELECT_ALL );
    }
}

