// Copyright (c) 2013-2014 The Feathercoin developers
#ifndef NAMEVIEW_H
#define NAMEVIEW_H

#include "guiutil.h"

#include <QWidget>

class WalletModel;
class TransactionFilterProxy;

QT_BEGIN_NAMESPACE
class QTableView;
class QStandardItemModel;
class QComboBox;
class QLineEdit;
class QModelIndex;
class QMenu;
class QFrame;
class QDateTimeEdit;
class QLabel;
QT_END_NAMESPACE

/** Widget showing the transaction list for a wallet, including a filter row.
    Using the filter row, the user can view or export a subset of the transactions.
  */
class NameView : public QWidget
{
    Q_OBJECT

public:
    explicit NameView(QWidget *parent = 0);

    void setModel(WalletModel *model);


private:
    WalletModel *model;
    TransactionFilterProxy *transactionProxyModel;
    QTableView *nameView;
    QStandardItemModel *nameModel;

private slots:
    void showView();

signals:
    //void doubleClicked(const QModelIndex&);
    /**  Fired when a message should be reported to the user */
    void message(const QString &title, const QString &message, unsigned int style);

public slots:
    void exportClicked();

};

#endif // NAMEVIEW_H
