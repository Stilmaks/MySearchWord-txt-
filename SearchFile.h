#pragma once

#include <QtWidgets/QWidget>
#include "ui_SearchFile.h"
#include <QLineEdit>
#include <unordered_map>
#include <QString>
#include <vector>
#include <mutex>

class SearchFile : public QWidget
{
    Q_OBJECT

public:
    SearchFile(QWidget *parent = nullptr);
    ~SearchFile();
    void WindowsSearchDir();
    void loadNotes(QString& dir_path, QString& text);
    void addWidgets();
    void collectFiles(const QString& dir_path);
    void search(const std::vector<std::string>& file_vector,const QString& text, const int& count_min, const int& count_max);
private:
    Ui::SearchFileClass ui;
    std::unordered_map<QString, unsigned> file_path;
    std::vector<std::string> file_vector;
    std::mutex mtx;
};

