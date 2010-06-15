
#include "savedcalculations.h"

#define CALCULATION_NAME     "CalcName"
#define CALCULATION_DATETIME "CalcDateTime"
#define CALCULATION_RESULT   "CalcResult"
#define CALCULATION_INPUT    "CalcInput"
#define CALCULATION_INDEX    "CalcIndex"

#include <MWidgetCreator>
M_REGISTER_WIDGET( SavedCalculations )

SavedCalculationsDataModel::SavedCalculationsDataModel( QObject* parent )
				:QAbstractListModel(parent)
{
    theHeaderData << CALCULATION_NAME 
	    << CALCULATION_DATETIME 
	    << CALCULATION_RESULT 
	    << CALCULATION_INPUT
	    << CALCULATION_INDEX;
}

SavedCalculationsDataModel::~SavedCalculationsDataModel()
{
}

int SavedCalculationsDataModel::rowCount( const QModelIndex& parent ) const
{
    Q_UNUSED( parent );
    return theData.size();
}

int SavedCalculationsDataModel::columnCount ( const QModelIndex & parent ) const
{
    Q_UNUSED(parent);
    return theHeaderData.size();
}

QVariant SavedCalculationsDataModel::data ( const QModelIndex & index, int role ) const
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

QVariant SavedCalculationsDataModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    Q_UNUSED( orientation );

    if( role == Qt::DisplayRole )
    {
	if( section < theHeaderData.size() )
	{
	    return QVariant( theHeaderData.at( section ) );
	}
    }

    return QVariant();
}

bool SavedCalculationsDataModel::insertRows( int row, int count, const QModelIndex& parent )
{
    beginInsertRows( parent, row, row + count - 1 );
    QDir::setCurrent( QDir::homePath() + "/" + CALC_HIDDENDIR );
    QFile file( CALCULATOR_SAVED );
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

bool SavedCalculationsDataModel::insertRow( int row, const QModelIndex& parent )
{
    beginInsertRows( parent, row, row );
    QDir::setCurrent( QDir::homePath() + "/" + CALC_HIDDENDIR );
    QFile file( CALCULATOR_SAVED );
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
    endInsertRows();
    return true;
}

bool SavedCalculationsDataModel::populateHeaderData( CalcDomReader& reader, int row, QStringList& data )
{
    CalculationItem item;
	 reader.readItem( row, item );
	data << item.value( "name" )
	    << item.value( "datetime" ) 
	    << item.value( "result" )
	    << item.value( "input" )
	    << item.value( "index" );
    return true;
}

bool SavedCalculationsDataModel::removeRow( int row, const QModelIndex& parent )
{
    beginRemoveRows( parent, row, row );
    theData.remove( row );
    endRemoveRows();
    return true;
}

bool SavedCalculationsDataModel::removeRows( int row, int count, const QModelIndex& parent )
{
    if( count < 1 )
	return false;
    beginRemoveRows( parent, row, row + count - 1 );

    for( int i = row; i < count; count -- )
	theData.remove( i );
    
    endRemoveRows();
    return true;
}

SavedCalculationsCellCreator::SavedCalculationsCellCreator(QObject * p):size(QSize(-1, -1) ),parent(p)
{
    MWidget* cell = new SavedCalculationItem;
    size = cell->effectiveSizeHint(Qt::PreferredSize);
    delete cell;
}

QSizeF SavedCalculationsCellCreator::cellSize() const
{
    return size;
}

MWidget* SavedCalculationsCellCreator::createCell( 
                                               const QModelIndex & index, 
					       MWidgetRecycler & recycler) const
{
    SavedCalculationItem* cell = dynamic_cast<SavedCalculationItem*>(recycler.take( "CalculationHistoryItem" ) );
    
    if( !cell )
    {
	cell = new SavedCalculationItem;
    }
    
    updateCell( index, cell );
    
    return cell;
}

void SavedCalculationsCellCreator::updateCell(
                                        const QModelIndex& index, 
					MWidget * cell) const
{
    SavedCalculationItem* item = qobject_cast<SavedCalculationItem*>(cell);
    item->setName( index.sibling( index.row(), 0 ).data().toString() );
    item->setTimeOfCalculation( index.sibling( index.row(), 1 ).data().toString() );
    item->setResult( index.sibling( index.row(), 2 ).data().toString() );
    item->setInput( index.sibling( index.row(), 3 ).data().toString() );
    item->setIndex( index.sibling( index.row(), 4 ).data().toString() );
    updateContentItemMode( index, item );
}

void SavedCalculationsCellCreator::updateContentItemMode( const QModelIndex& index, SavedCalculationItem* contentItem) const
{
    int row = index.row();
    bool thereIsNextRow= index.sibling(row + 1, 0).isValid();
    if( row == 0 && !thereIsNextRow )
        contentItem->setItemMode( SavedCalculationItem::Single );
    else if(row == 0)
        contentItem->setItemMode(SavedCalculationItem::SingleColumnTop);
    else if(thereIsNextRow)
        contentItem->setItemMode(SavedCalculationItem::SingleColumnCenter);
    else
        contentItem->setItemMode(SavedCalculationItem::SingleColumnBottom);
}

DeleteSavedItemsCellCreator::DeleteSavedItemsCellCreator( QObject* p )
	                        : SavedCalculationsCellCreator( p )
{
}

MWidget* DeleteSavedItemsCellCreator::createCell(
		                         const QModelIndex& index,
					 MWidgetRecycler& recycler ) const
{
    SavedCalculationItem* cell = dynamic_cast<SavedCalculationItem*>(recycler.take( "CalculationHistoryItem" ) );

    if( !cell )
    {
        cell = new SavedCalculationItem;
    }

    updateCell( index, cell );

    return cell;
}

SavedCalculations::SavedCalculations() 
                        : model( NULL ), 
			  confirmDialog(NULL),
			  renameText( NULL ),
			  renameDialog( NULL ),
			  renameItem( NULL ),
			  deleteSavedItems( NULL ),
			  centralWidgetLayout( NULL ),
			  list( NULL )
{
    utility = CalcUtilities::instance();
    setTitle( TXT_CALC_SAVED_TITLE );
    setPannable( true );
    model = new SavedCalculationsDataModel(this);
}

SavedCalculations::~SavedCalculations()
{
    delete renameDialog;
    delete model;
    delete confirmDialog;
}

void SavedCalculations::createContent()
{
    MApplicationPage::createContent();

    this->setObjectName( "calcpage" );

    MLabel* emptyHistory = new MLabel;
    MLayout* emptyHistoryLayout = new MLayout( emptyHistory );
    emptyHistoryLayoutPolicy = new MLinearLayoutPolicy( emptyHistoryLayout, Qt::Vertical );
    emptyHistory->setLayout( emptyHistoryLayout );

    MLabel* labelEmpty = new MLabel( TXT_CALC_SAVED_BLANK );
    MLabel* labelFiller = new MLabel( "" );

    labelEmpty->setAlignment( Qt::AlignHCenter );

    emptyHistoryLayoutPolicy->addItem( labelEmpty );
    emptyHistoryLayoutPolicy->addItem( labelFiller );

    emptyHistory->setObjectName( "emptyHistory" );
    labelEmpty->setObjectName( "emptyLabel" );
    
    list = new MList();
    list->setViewType( "fast" );
    list->setSelectionMode( MList::SingleSelection );
    list->setShowGroups(false);
    list->setCellCreator(new SavedCalculationsCellCreator(this));
    list->setItemModel(model);

    MAction* action = new MAction( TXT_CALC_SAVED_RENAME, list );
    action->setLocation( MAction::ObjectMenuLocation );
    list->addAction( action );
    QObject::connect( action, SIGNAL(triggered()), this, SLOT(initiateRenameCalculation()) );

    action = new MAction( TXT_CALC_DELETE, list );
    action->setLocation( MAction::ObjectMenuLocation );
    list->addAction( action );
    QObject::connect( action, SIGNAL(triggered()), this, SLOT(initiateDeleteCalculation()) );

    connect( this, SIGNAL( disappeared() ), list->selectionModel(), SLOT( clearSelection() ) );
    connect( list, SIGNAL( itemClicked(QModelIndex) ), this, SLOT( handleSavedCalculationItemClicked(QModelIndex) ) );
    connect( list, SIGNAL( itemLongTapped(QModelIndex) ), this, SLOT( itemLongTapped(QModelIndex) ) );
    
    deleteSavedItems = new DeleteSavedItems( model );
    connect(
	deleteSavedItems,
	SIGNAL( deleteItems( QModelIndexList& ) ),
	this,
	SLOT( initiateDeleteMultipleCalculations( QModelIndexList& ) ) );
    
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
    
    /****************************************************************************************
     * This block creates the MDialog for renaming calculation which can be reused later  *
     * The dialog is created once executed multiple number of times.                        *
     ****************************************************************************************/
    MWidget* centralWidget = new MWidget;
    QGraphicsLinearLayout* layout = new QGraphicsLinearLayout(Qt::Vertical );
    
    renameText = new MTextEdit(
                       MTextEditModel::SingleLine,
		       "",
		       centralWidget);

    renameText->setMaxLength( 30 );
    renameText->setInputMethodCorrectionEnabled( false );

     renameDialog = new MDialog( );
 	 renameDialog->setTitle(TXT_CALC_SAVED_RENAME_TITLE);
	 
	 renameButton = new MButton(TXT_CALC_RENAME_BUTTON);

	 renameDialog->addButton(renameButton->model());
 	 renameDialog->addButton(M::CancelButton);	

	 renameDialog->setCentralWidget( centralWidget );
	 renameDialog->setCloseButtonVisible(true);
 	 centralWidget->setLayout( layout );
  	 layout->addItem(renameText);
	
	connect(
			renameButton,
			SIGNAL(clicked()),
			this,
			SLOT(renameCalcAccepted()));

    connect(
            renameDialog,
            SIGNAL(rejected()),
            this,
            SLOT(renameCalcRejected()));

    
    confirmDialog = new MMessageBox( TXT_CALC_HISTORY_DEL_CONF, M::YesButton | M::NoButton );

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
}

void SavedCalculations::updateContent()
{
    if( !centralWidgetLayout )
	return;
    QDir::setCurrent( QDir::homePath() + "/" + CALC_HIDDENDIR );
    QFile file( CALCULATOR_SAVED );
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
}

void SavedCalculations::handleSavedCalculationItemClicked( QModelIndex index )
{
    emit this->itemClicked( index.sibling( index.row(), 2 ).data().toString() );
}

void SavedCalculations::handleItemSelected(bool clicked)
{
    Q_UNUSED(clicked );
}

void SavedCalculations::showDeleteCalculationsPage()
{
    deleteSavedItems->appear( KeepWhenDone );
}

void SavedCalculations::handleVisibilityChanged( bool visible )
{
    if( visible && isContentCreated() )
    {
	this->updateContent();
    }
}

void SavedCalculations::addItem( CalculationItem& calcItem )
{
    bool writeNewFile = false;
    QDir::setCurrent( QDir::homePath() );
    QDir dir( CALC_HIDDENDIR );
    QDir homeDir( QDir::homePath() );
    if( !dir.exists() )
        homeDir.mkdir( CALC_HIDDENDIR );
    QDir::setCurrent( QDir::homePath() + "/" + CALC_HIDDENDIR );
    QFile file( CALCULATOR_SAVED );
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
			calcItem["index"]=reader.count()+1;
            writer.addItem( calcItem );
            if( writer.saveToFile( file ) )
	    {
		emit this->itemSaved();
	    }
        }
        else
		{
			calcItem["index"]='0';
            writeNewFile = true;
		}
    }
    else
        writeNewFile = true;

    if( writeNewFile )
    {
        CalcDomWriter writer;
        writer.addItem( calcItem );
        writer.saveToFile( file );
        emit this->itemSaved();
    }
}

int SavedCalculations::count() const
{
    if( model )
	return model->rowCount();
    return 0;
}

void SavedCalculations::itemLongTapped( QModelIndex index )
{
    longTappedIndex = index;
}


void SavedCalculations::initiateRenameCalculation()
{
    qDebug() << Q_FUNC_INFO << endl;
    renameText->setText( longTappedIndex.sibling( longTappedIndex.row(), 0 ).data().toString() );
    renameText->setPrompt( TXT_CALC_SAVE_DEF );
    renameDialog->appear();
}

void SavedCalculations::renameCalcAccepted()
{
    qDebug() << Q_FUNC_INFO << endl;
    
    if( longTappedIndex.isValid() && longTappedIndex.sibling( longTappedIndex.row(), 0 ).data().toString() != renameText->text() )
    {
	// this has to be renamed now
	// 1) update the xml file
        QDir::setCurrent( QDir::homePath() + "/" + CALC_HIDDENDIR );
	QFile file( CALCULATOR_SAVED );
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
	    writer.renameByIndex( 
                    longTappedIndex.sibling( longTappedIndex.row(), 4 ).data().toString(),
                    renameText->text()==""? renameText->prompt() : renameText->text() );
	    writer.saveToFile( file );
	}
	
	// 2) update the list view
	this->updateContent();
    }
}

void SavedCalculations::renameCalcRejected()
{
    qDebug() << Q_FUNC_INFO << endl;
}

void SavedCalculations::initiateDeleteCalculation()
{
    confirmDialog->setText( TXT_CALC_SAVED_DEL_CONF.arg( longTappedIndex.sibling( longTappedIndex.row(), 0 ).data().toString() ) );
    confirmDialog->appear();
}

void SavedCalculations::confirmDeleteAccept()
{
    qDebug() << Q_FUNC_INFO << endl;
    QDir::setCurrent( QDir::homePath() + "/" + CALC_HIDDENDIR );
    QFile file( CALCULATOR_SAVED );
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
	writer.removeByIndex( longTappedIndex.sibling( longTappedIndex.row(), 4 ).data().toString() );
	writer.saveToFile( file );
    }
    this->updateContent();
}

void SavedCalculations::confirmDeleteReject()
{
}

void SavedCalculations::initiateDeleteMultipleCalculations( QModelIndexList& selectedItems )
{
    qDebug() << Q_FUNC_INFO << endl;
    deleteSavedItems->dismiss();

    QListIterator<QModelIndex> it( selectedItems );
    it.toBack();
    
    //foreach( QModelIndex index, selectedItems )
    while( it.hasPrevious() )
    {
	QModelIndex index = it.previous();
	qDebug() << "===========value=============" << endl;
	qDebug() << "name = " << index.sibling( index.row(), 0 ).data().toString() << endl;
	qDebug() << "time of calc = " << index.sibling( index.row(), 1 ).data().toString() << endl;
	qDebug() << "result = " << index.sibling( index.row(), 2 ).data().toString() << endl;
	qDebug() << "input = " << index.sibling( index.row(), 3 ).data().toString() << endl;
	qDebug() << "index = " << index.sibling( index.row(), 4 ).data().toString() << endl;
	qDebug() << "=============================" << endl;
	QString itemIndex = index.sibling( index.row(), 4 ).data().toString();
	
        QDir::setCurrent( QDir::homePath() + "/" + CALC_HIDDENDIR );
        QFile file( CALCULATOR_SAVED );
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
    
    this->updateContent();
}

void SavedCalculations::retranslateUi()
{
    setTitle( TXT_CALC_SAVED_TITLE );
    if( !isContentCreated() )
	return;

    this->updateContent();

    if( model->rowCount() > 0 )
	(this->actions()[0])->setText( TXT_CALC_DEL_LIST );

    ((MLabel*)emptyHistoryLayoutPolicy->itemAt(0))->setText( TXT_CALC_SAVED_BLANK );

    renameDialog->setTitle( TXT_CALC_SAVED_RENAME_TITLE );
    renameText->setPrompt( TXT_CALC_SAVE_DEF );
}

DeleteSavedItems::DeleteSavedItems( SavedCalculationsDataModel* model )
	             : confirmDialog( NULL )
{
    setTitle( TXT_CALC_SAVED_TITLE );
    setPannable( true );
    dellist = new MList();
    dellist->setViewType( "fast" );
    dellist->setObjectName( "CalculationHistoryList" );
    dellist->setSelectionMode( MList::MultiSelection );
    dellist->setShowGroups(false);
    dellist->setCellCreator(new DeleteSavedItemsCellCreator(this));
    dellist->setItemModel(model);
    connect( this, SIGNAL( appeared() ), dellist->selectionModel(), SLOT( clearSelection() ) );
}

DeleteSavedItems::~DeleteSavedItems() 
{
    delete confirmDialog;
}

void DeleteSavedItems::createContent()
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

void DeleteSavedItems::handleDeleteItems()
{
    QModelIndexList selectedItems = dellist->selectionModel()->selectedIndexes();

    if( selectedItems.size() > 0 )
    {
	qDebug() << Q_FUNC_INFO 
		<< ": selectedItems.size() = " << selectedItems.size() 
		<< ", rowcount = " << dellist->itemModel()->rowCount() << endl;
	if( selectedItems.size() == 1 )
	    confirmDialog->setText( TXT_CALC_DEL_ONE_CONF.arg( selectedItems.size() ) );
        else if( selectedItems.size() < dellist->itemModel()->rowCount() )
	    confirmDialog->setText( TXT_CALC_DEL_SEL_CONF.arg(selectedItems.size() ) );
	else if( selectedItems.size() == dellist->itemModel()->rowCount() )
	    confirmDialog->setText( TXT_CALC_DEL_ALL_CONF );

        confirmDialog->appear();
    }
}

void DeleteSavedItems::confirmDeleteAccept()
{
    qDebug() << Q_FUNC_INFO << endl;
    QModelIndexList selectedItems = dellist->selectionModel()->selectedIndexes();
    emit this->deleteItems( selectedItems );
}

void DeleteSavedItems::confirmDeleteReject()
{
}

void DeleteSavedItems::handleSelectAll()
{
    QModelIndex first = dellist->itemModel()->index( 0, 0 );
    QModelIndex last = dellist->itemModel()->index(dellist->itemModel()->rowCount()-1, 0 );
    QItemSelection selection( first, last );
    QItemSelectionModel* selectionModel = dellist->selectionModel();
    selectionModel->select( selection, QItemSelectionModel::Select);
}

void DeleteSavedItems::retranslateUi()
{
    setTitle( TXT_CALC_SAVED_TITLE );
    if( !isContentCreated() )
        return;
    if( this->actions().count() == 2 )
    {
        (this->actions()[0])->setText( TXT_CALC_SELECT_ALL );
        (this->actions()[1])->setText( TXT_CALC_UNSELECT_ALL );
    }
}


