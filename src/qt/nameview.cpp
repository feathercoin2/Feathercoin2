#include "nameview.h"

#include "addresstablemodel.h"
#include "bitcoinunits.h"
#include "csvmodelwriter.h"
#include "editaddressdialog.h"
#include "guiutil.h"
#include "optionsmodel.h"
#include "transactiondescdialog.h"
#include "transactionfilterproxy.h"
#include "transactionrecord.h"
#include "transactiontablemodel.h"
#include "walletmodel.h"

#include "ui_interface.h"

#include <stdint.h>
#include <stdio.h>

#include <QScrollBar>
#include <QComboBox>
#include <QDoubleValidator>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QTableView>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QMessageBox>
#include <QPoint>
#include <QMenu>
#include <QLabel>
#include <QDateTimeEdit>
#include <QFile>

#include <boost/filesystem.hpp> 

using namespace std;
using namespace boost;

NameView::NameView(QWidget *parent) :
    QWidget(parent), model(0), transactionProxyModel(0),
    nameView(0)
{
    // Build filter row
    setContentsMargins(0,0,0,0);

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setContentsMargins(0,0,0,0);
#ifdef Q_OS_MAC
    hlayout->setSpacing(5);
    hlayout->addSpacing(26);
#else
    hlayout->setSpacing(0);
    hlayout->addSpacing(23);
#endif

    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->setContentsMargins(0,0,0,0);
    vlayout->setSpacing(0);

    QTableView *view = new QTableView(this);
    vlayout->addLayout(hlayout);
    vlayout->addWidget(view);
    vlayout->setSpacing(0);
    // Always show scroll bar
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    view->setTabKeyNavigation(false);
    view->setContextMenuPolicy(Qt::CustomContextMenu);
    nameView = view;
}

void NameView::setModel(WalletModel *model)
{
    this->model = model;
    if(model)
    {
				nameModel = new QStandardItemModel(this);
				
				nameView->setModel(nameModel);
				nameView->setAlternatingRowColors(true);
				nameView->setSelectionBehavior(QAbstractItemView::SelectRows);
        nameView->setSelectionMode(QAbstractItemView::ExtendedSelection);
        nameView->setSortingEnabled(true);
        nameView->sortByColumn(0, Qt::DescendingOrder);
        nameView->verticalHeader()->hide();
        nameView->setShowGrid(false);
        
        showView();
    }
}

void NameView::exportClicked()
{

}

void NameView::showView()
{
	  nameModel->clear();
	  nameModel->setRowCount(5);
    nameModel->setHorizontalHeaderItem(0, new QStandardItem(tr("Name")));
    nameModel->setHorizontalHeaderItem(1, new QStandardItem(tr("Address")));
   	nameModel->setHorizontalHeaderItem(2, new QStandardItem(tr("NameHash")));
    nameModel->setHorizontalHeaderItem(3, new QStandardItem(tr("EffectTime")));
    nameModel->setHorizontalHeaderItem(4, new QStandardItem(tr("Status")));
    nameView->horizontalHeader()->resizeSection(0, 200);
    nameView->horizontalHeader()->resizeSection(1, 270);
    nameView->horizontalHeader()->resizeSection(2, 270);
    nameView->horizontalHeader()->resizeSection(3, 150);
    nameView->horizontalHeader()->resizeSection(4, 100);
    
    //读取文件nameview\NameView.dat
    QVector<QString> dbName;
    QVector<QString> dbAddress;
    QVector<QString> dbNameHash;
    QVector<QString> dbEffectTime;
    QVector<QString> dbStatus;
    
    boost::filesystem::path pathFile = GetDataDir() / "nameview";
    if(!boost::filesystem::exists(pathFile))
    {
    	boost::filesystem::create_directory(pathFile);  //目录不存在，创建 
    }
    boost::filesystem::path fileName = pathFile / "nameview.dat";
    QFile file(QString(fileName.string().c_str()));
    if(!file.open(QIODevice::ReadWrite))  //不存在就建立一个文件
		{
			QMessageBox::information(NULL, tr("NameView Message"), tr("%1 can't open!").arg(QString(fileName.string().c_str())), QMessageBox::Yes , QMessageBox::Yes);
		}
		file.close();
    
    //显示数据

}
