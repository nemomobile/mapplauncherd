#ifndef CALCULATIONHISTORY_H
#define CALCULATIONHISTORY_H

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
#include <MDialog>
#include <MMessageBox>
#include <MInfoBanner>
#include <MAbstractCellCreator>
#include <MMessageBox>
#include <MLocale>

#include <QTimer>
#include <QDir>
#include <QStringList>
#include <QDateTime>
#include <QDebug>
#include <QGraphicsLinearLayout>
#include <QAbstractListModel>

#include <iostream>
#include <sstream>

#include "calcconstants.h"
#include "calcutilities.h"
#include "calcdomreader.h"
#include "calcdomwriter.h"
#include "calculationhistoryitem.h"

using namespace std;

class CalculationHistoryDataModel : public QAbstractListModel //MWidgetListModel 
{
public:
    CalculationHistoryDataModel( QObject* parent );
    virtual ~CalculationHistoryDataModel();
    virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
    virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    QVariant headerData( int section, Qt::Orientation orientation, int role ) const;
    bool insertRow( int row, const QModelIndex& parent = QModelIndex() );
    bool insertRows( int row, int count, const QModelIndex& parent = QModelIndex() );
    bool removeRow( int row, const QModelIndex& parent = QModelIndex() );
    bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
protected:
    bool populateHeaderData( CalcDomReader& reader, int row, QStringList& data );
private:
    QVector<QStringList> theData;
    QStringList theHeaderData;
};

class CalculationHistoryCellCreator : public MCellCreator
{
public:
    CalculationHistoryCellCreator( QObject* );
    virtual void updateCell( const QModelIndex& index, MWidget* cell ) const;
    virtual MWidget * createCell( const QModelIndex & index, MWidgetRecycler & recycler) const;
    virtual void updateContentItemMode(const QModelIndex& index, CalculationHistoryItem* contentItem) const;
    virtual QSizeF cellSize() const;
private:
    QSizeF size;
    QObject * parent;
};

class DeleteHistoryItems : public MApplicationPage
{
    Q_OBJECT
public:
    DeleteHistoryItems(CalculationHistoryDataModel* model );
    virtual ~DeleteHistoryItems();
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

class CalculationHistory : public MApplicationPage
{
    Q_OBJECT
public:
    CalculationHistory();
    virtual ~CalculationHistory();
    void createContent();
    void updateContent();
    void addItem( CalculationItem& );
    void retranslateUi();
    
signals:
    void itemClicked( const QString& );
    void save(CalculationItem& );
public slots:
    void confirmClearAccept();
    void confirmClearReject();
    
    void handleHistoryItemClicked();
    void initiateSaveCalculation();
    void initiateDeleteCalculation();
    void initiateDeleteMultipleCalculations(QModelIndexList& );

    void showDeleteCalculationsPage();

private:

    CalculationHistoryDataModel*  model;
    MList* list;
    // needed for switching the layout policies of the central widget of this page
    // i.e. for showing a label when there is no calculation history
    // and showing list when there is calculation history
    MLayout* centralWidgetLayout;
    MLinearLayoutPolicy* centralEmptyPolicy;
    MLinearLayoutPolicy* centralListPolicy;
    MLinearLayoutPolicy* emptyHistoryLayoutPolicy;
    MMessageBox* confirmDialog;

    // delete multiple calculations
    DeleteHistoryItems* deleteHistoryItems;
    CalculationHistoryItem* deleteCandidate;
    
    CalcUtilities* utility;
};

class DeleteHistoryItemsCellCreator : public CalculationHistoryCellCreator
{
public:
    DeleteHistoryItemsCellCreator( QObject* );
    virtual MWidget * createCell( const QModelIndex & index, MWidgetRecycler & recycler) const;
};

#endif

