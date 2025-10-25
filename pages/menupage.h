#ifndef MENUPAGE_H
#define MENUPAGE_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel.h>

class MenuPage : public QWidget
{
    Q_OBJECT

public:
    explicit MenuPage(QWidget *parent = nullptr);
    void Reset();

private:
    QPushButton *newGameBtn;
    QPushButton *loadGameBtn;
    QPushButton *exitGameBtn;

    QColor color1, color2;
    double phase = 0.0;
    int really_exit = 0;

protected:
    void paintEvent(QPaintEvent *event) override;

signals:
    void NewGame();
    void LoadGame();
    void ExitGame();
};

#endif // MENUPAGE_H
