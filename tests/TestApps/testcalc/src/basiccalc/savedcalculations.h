#ifndef SAVEDCALCULATIONS_H
#define SAVEDCALCULATIONS_H

#include <MApplicationPage>
#include <MLocale>
#include <MPopupList>
#include <MAction>
#include <MList>
#include <MButton>
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MLabel>
#include <MWidget>
#include <MCalendar>
#include <MWidgetRecycler>
#include <MMessageBox>
#include <MDialog>
#include <MInfoBanner>
#include <MAbstractCellCreator>
#include <MTextEdit>
#include <MMessageBox>

#include <QTimer>
#include <QDir>
#include <QStringList>
#include <QDateTime>
#include <QDebug>
#include <QGraphicsLinearLayout>
#include <QSettings>
#include <QAbstractListModel>

#include "calcconstants.h"
#include "calcutilities.h"
#include "calcdomreader.h"
#include "calcdomwriter.h"
#include "savedcalculationitem.h"

class SavedCalculationsDataModel : public QAbstractListModel
{
public:
    SavedCalculationsDataModel(QObject* parent);
    virtual ~SavedCalculationsDataModel();
    virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
    virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    QVariant headerData( int section, Qt::Orientation orientation, int role ) const;
    bool insertRow( int row, const QModelIndex& parent = QModelIndex() );
    bool insertRows( int row, int count, const QModelIndex& parent = QModelIndex() );
    bool removeRow( int row, const QModelIndex& parent = QModelIndex() );
    bool removeRows( int row, const QModelIndex & parent = QModelIndex() );
    bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
protected:
    bool populateHeaderData( CalcDomReader& reader, int row, QStringList& data );
private:
    QVector<QStringList> theData;
    QStringList theHeaderData;
};

class SavedCalculationsCellCreator : public MCellCreator
{
public:
    SavedCalculationsCellCreator( QObject* );
    void updateCell( const QModelIndex& index, MWidget* cell ) const;
    MWidget * createCell( const QModelIndex & index, MWidgetRecycler & recycler) const;
    void updateContentItemMode(const QModelIndex&, SavedCalculationItem*) const;
    QSizeF cellSize() const;
private:
    QSizeF size;
    QObject * parent;
};

class DeleteSavedItems : public MApplicationPage
{
    Q_OBJECT
public:
    DeleteSavedItems(SavedCalculationsDataModel* model );
    virtual ~DeleteSavedItems();
    void createContent();
    void retranslateUi();
signals:
    void deleteItems( QModelIndexList& );
public slots:
    void handleDeleteItems();
    void handleSelectAll();
    void confirmDeleteAccept();
    void confirmDeleteReject();
private:
    MList* dellist;
    MMessageBox* confirmDialog;
};



class SavedCalculations : public MApplicationPage
{
    Q_OBJECT
public:
    SavedCalculations();
    virtual ~SavedCalculations();
    void createContent();
    void updateContent();
    void addItem( CalculationItem& );
    int count() const;
    void showRenameDialog( const QString& );
    void retranslateUi();
    
signals:
    void itemClicked( const QString& );
    void savedCalculationsCleared();
    void itemAlreadyExists();
    void itemSaved();
    
public slots:
    void handleSavedCalculationItemClicked( QModelIndex );
    void handleItemSelected(bool);
    void confirmDeleteAccept();
    void confirmDeleteReject();
    void handleVisibilityChanged(bool);
    void initiateRenameCalculation();
    void initiateDeleteCalculation();

    void initiateDeleteMultipleCalculations(QModelIndexList&);

    void renameCalcAccepted();
    void renameCalcRejected();
    
    void showDeleteCalculationsPage();

    void itemLongTapped( QModelIndex );
    
private:

    SavedCalculationsDataModel*  model;
    MList* list;
    // needed for switching the layout policies of the central widget of this page
    // i.e. for showing a label when there is no calculation history
    // and showing list when there is calculation history
    MLayout* centralWidgetLayout;
    MLinearLayoutPolicy* centralEmptyPolicy;
    MLinearLayoutPolicy* centralListPolicy;
    MLinearLayoutPolicy* emptyHistoryLayoutPolicy;
    MMessageBox* confirmDialog;

    MDialog* renameDialog;
    MTextEdit* renameText;
	MButton* renameButton;

    SavedCalculationItem* renameItem;
    SavedCalculationItem* deleteCandidate;

    DeleteSavedItems* deleteSavedItems;

    CalcUtilities* utility;

    QModelIndex longTappedIndex;
};

class DeleteSavedItemsCellCreator : public SavedCalculationsCellCreator
{
public:
    DeleteSavedItemsCellCreator( QObject* );
    virtual MWidget * createCell( const QModelIndex & index, MWidgetRecycler & recycler) const;
};


#endif


