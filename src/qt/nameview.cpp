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
#include <iostream>
#include <fstream>
#include <string>

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
#include <QDateTime>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

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
   	nameModel->setHorizontalHeaderItem(2, new QStandardItem(tr("EffectTime")));
    nameModel->setHorizontalHeaderItem(3, new QStandardItem(tr("Status")));
    nameModel->setHorizontalHeaderItem(4, new QStandardItem(tr("BlockID")));
    nameModel->setHorizontalHeaderItem(5, new QStandardItem(tr("TxID")));
    nameView->horizontalHeader()->resizeSection(0, 200);
    nameView->horizontalHeader()->resizeSection(1, 270);
    nameView->horizontalHeader()->resizeSection(2, 160);
    nameView->horizontalHeader()->resizeSection(3, 85);
    nameView->horizontalHeader()->resizeSection(4, 80);
    nameView->horizontalHeader()->resizeSection(5, 150);
    
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
    
    //显示文件数据
    //读取文件
    ifstream ifile(fileName.string().c_str(),ios::in);
    if(!ifile)
		{
			LogPrintf("NameView ifile file error, %s can't open!\n", fileName.string().c_str());  
			return ;
		}
		
		//读取旧数据，判断是否重复，加入新数据,Block #745773
		//Name,Address,EffectTime,Status,BlockID,TxID
		//读取旧数据
		std::vector<std::string> namelist;
		std::vector<std::string> addresslist;
		std::vector<std::string> efftimelist;
		std::vector<std::string> statuslist;
		std::vector<std::string> blocklist;
		std::vector<std::string> txlist;
		int i=1;
		std::string temp;
		bool rf=true;
		while(rf)
		{
			rf=getline(ifile,temp);
			if (rf)
			{					
					//成功读取每个字段
					std::vector<std::string> vStr;
					boost::split(vStr,temp,boost::is_any_of(","), boost::token_compress_on);
					for( vector<string>::iterator it = vStr.begin(); it != vStr.end(); ++ it )
					{
						if (i==1)
							namelist.push_back(*it);
						if (i==2)
							addresslist.push_back(*it);
						if (i==3)
						{
							QDateTime dt ;
							dt.setTimeSpec(Qt::UTC);
							dt= QDateTime::fromTime_t(atoi(*it));
							QString strDate = dt.toString(Qt::SystemLocaleLongDate);
							efftimelist.push_back(strDate.toStdString());
						}
						if (i==4)
							statuslist.push_back(*it);
						if (i==5)
							blocklist.push_back(*it);
						if (i==6)
						{
							txlist.push_back(*it);
							i=0;
						}
						i++;
					}
			}
		}
		ifile.close();
		LogPrintf("NameView read nameview ok.\n");
		
		//显示数据
		nameModel->setRowCount(namelist.size()+1);
		for (i=0;i!=namelist.size();++i)
    { 
    	nameModel->setItem(i,0,new QStandardItem(QString(namelist.at(i).c_str())));
    	nameModel->setItem(i,1,new QStandardItem(QString(addresslist.at(i).c_str())));
    	nameModel->setItem(i,2,new QStandardItem(QString(efftimelist.at(i).c_str())));
    	nameModel->setItem(i,3,new QStandardItem(QString(statuslist.at(i).c_str())));
      nameModel->setItem(i,4,new QStandardItem(QString(blocklist.at(i).c_str())));
      nameModel->setItem(i,5,new QStandardItem(QString(txlist.at(i).c_str())));
    }
}
