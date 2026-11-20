#include "SearchFile.h"
#include <qFileDialog>
#include <filesystem>
#include <fstream>
#include <QDebug>
#include <QMessageBox>
#include <thread>
#include <atomic>

SearchFile::SearchFile(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    connect(ui.pushButton, &QPushButton::clicked, this, &SearchFile::WindowsSearchDir);
    ui.progressBar->setRange(0, 100);
    ui.progressBar->setValue(0);
}

SearchFile::~SearchFile()
{
    
}

void SearchFile::WindowsSearchDir()
{
    file_path.clear();
    ui.listWidget->clear();
    ui.tableWidget->setRowCount(0);
    ui.progressBar->setValue(0);
   
    QString text = ui.lineEdit->text();

    ui.lineEdit->clear();
    QString dir_path = QFileDialog::getExistingDirectory(this, "Selected folder: ","", QFileDialog::ShowDirsOnly);
    
    if (dir_path.isEmpty()) {
        return;
    }
    try {

        loadNotes(dir_path, text); 

        if (!file_path.empty()) {
            addWidgets();
        }       
    }
    catch (...) {
        qDebug() << "Error open file";
    }
}


void SearchFile::loadNotes(QString& dir_path, QString& text)
{
    collectFiles(dir_path);
    std::vector<std::thread> threads;
  
    for (int i = 0; i < 4; ++i) {
        int count_min = i * file_vector.size()/4;
        int count_max = i==3 ? file_vector.size(): (i+1) * file_vector.size() / 4;
        threads.push_back(std::thread([&, count_min, count_max]() {
            search(file_vector, text, count_min, count_max);
            }));
    } 

    for (auto& el : threads) {
        el.join();
    }
    ui.progressBar->setValue(ui.progressBar->maximum());
}

void SearchFile::addWidgets()
{

    for (const auto& [path, count] : file_path) {
        std::filesystem::path p = path.toStdString();
        QString shortName = QString::fromStdString(p.filename().string());
        QListWidgetItem* item = new QListWidgetItem;
        item->setText(shortName);
        ui.listWidget->addItem(item);
        
    
        int row = ui.tableWidget->rowCount();
        ui.tableWidget->insertRow(row);

        ui.tableWidget->setItem(row, 0, new QTableWidgetItem(shortName));
        ui.tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(count)));
    }
    QMessageBox::information(
        this,
        "result",
        "count files: " +
        QString::number(file_path.size()) +
        "\ncount rows in table: " +
        QString::number(ui.tableWidget->rowCount()) +
        "\ncount element in list: " +
        QString::number(ui.listWidget->count())
    );
}

void SearchFile::collectFiles(const QString& dir_path)
{
    file_vector.clear();
    std::filesystem::path path = dir_path.toStdString();
    if (std::filesystem::exists(path) && std::filesystem::is_directory(path)) {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                file_vector.push_back(entry.path().string());
            }
        }
    }



}
void SearchFile::search(const std::vector<std::string>& file_vector,const QString& text,const int& count_min, const int& count_max)
{
 
    int counter = 0;
    for (int i = count_min; i < count_max; i++) {
        counter = 0;
        std::ifstream file(file_vector[i]);
        if (!file.is_open()) {
            throw 1;
        }
        std::string result;
        while (std::getline(file, result)) {
            auto pos = result.find(text.toStdString());
            while (pos != std::string::npos) {
                ++counter;
                pos = result.find(text.toStdString(), pos + 1);
            }
        }
        if (counter > 0) {

            mtx.lock();
            file_path.insert({ QString::fromStdString(file_vector[i]), counter });
            mtx.unlock();
        }

    }
 
}

