#include "widget.h"
#include "ui_widget.h"

#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    if(!initDataBase())
    {
        qDebug() << "open db failed!";
    }

    m_model = new QSqlTableModel(ui->tableView, m_db);
    m_model->setTable("test");
    m_model->select();
    m_model->setEditStrategy(QSqlTableModel::OnFieldChange);
    m_model->setHeaderData(0, Qt::Horizontal, "id");
    m_model->setHeaderData(1, Qt::Horizontal, "name");
    m_model->setHeaderData(2, Qt::Horizontal, "desc");

    ui->tableView->setModel(m_model);
    ui->tableView->verticalHeader()->setVisible(false);

    ui->tableView->setFocusPolicy(Qt::NoFocus);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->verticalHeader()->setDefaultSectionSize(30);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);


    connect(m_model, &QSqlTableModel::dataChanged, this, [this](){ m_model->submitAll();});
}

Widget::~Widget()
{
    delete ui;
}

bool Widget::initDataBase()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE", "SQLMODULE");
    m_db.setDatabaseName("sqlmode.db");

    if(!m_db.open())
    {
        qDebug() << m_db.lastError().text();
        return false;
    }

    QString sql = QString("SELECT COUNT(*) FROM sqlite_master WHERE type = 'table' AND name = 'test';");
    QSqlQuery query(m_db);
    if(!query.exec(sql))
    {
        qDebug() << "select table exists failed!" << query.lastError().text();
        return false;
    }

    int count = 0;
    if (query.next())
    {
        if (query.size() == 0)
        {
            qDebug() << "chackTableExists result is invalid!";
            return false;
        }
        count = query.value(0).toInt();
    }

    if(count == 0)
    {
        return query.exec("Create table test(`id` INT, `name` VARCHAR(20), `desc` TEXT);");
    }

    return true;
}


void Widget::on_pushButton_add_clicked()
{
    m_model->insertRow(m_model->rowCount());
}


void Widget::on_pushButton_del_clicked()
{
    QModelIndexList list = ui->tableView->selectionModel()->selectedRows(0);

    if(list.isEmpty()) return;

    int row = list.first().row();
    m_model->removeRow(row);
    m_model->select();
}

