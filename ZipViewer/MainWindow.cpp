#include "MainWindow.h"
#include "./ui_MainWindow.h"
#include <QTableView>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QHeaderView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
     ui(new Ui::MainWindow),
     tableView(new QTableView(this)),
     model(new ZipModel(this)),
     archivePathLabel(new QLabel(this))
{
    ui->setupUi(this);
    setWindowTitle("Zip viewer");
    QVBoxLayout *layout = new QVBoxLayout;
    QPushButton *loadButton = new QPushButton("Выберите zip-архив", this);
    connect(loadButton, &QPushButton::clicked, [this]() {
        QString archivePath = QFileDialog::getOpenFileName(this, "Открыть zip-архив", "", "ZIP Archives (*.zip)");
        if (!archivePath.isEmpty()) {
            model->loadArchive(archivePath);
            tableView->reset();
            // Обновление текста на метке с путем к архиву
            archivePathLabel->setText("Просматриваемый архив: " + archivePath);
        }
    });

    layout->addWidget(loadButton);
    layout->addWidget(tableView);
    layout->addWidget(archivePathLabel);  // Добавляем метку с путем в лейаут

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    tableView->setModel(model);
    model->setHeaderData(0, Qt::Horizontal, "Имя файла");
    model->setHeaderData(1, Qt::Horizontal, "Сжатый размер");
    model->setHeaderData(2, Qt::Horizontal, "Несжатый размер");
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->show();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete model;
    delete tableView;
}

