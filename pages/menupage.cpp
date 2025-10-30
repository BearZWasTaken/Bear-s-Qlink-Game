#include "menupage.h"

#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QTimer>

MenuPage::MenuPage(QWidget *parent)
    : QWidget(parent)
{
    //// bkgrd color ////

    color1 = QColor("#fbc2eb");
    color2 = QColor("#a6c1ee");

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        phase += 1;
        if (phase >= 360.0) phase -= 360.0;

        QColor c1 = QColor::fromHsvF(fmod(phase / 360.0, 1.0), 0.4, 1.0);
        QColor c2 = QColor::fromHsvF(fmod((phase + 60.0) / 360.0, 1.0), 0.4, 1.0);

        color1 = c1.lighter(120);
        color2 = c2.lighter(120);

        update();
    });
    timer->start(50);


    //// main widget ////

    this->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, " "stop:0 #3a1c71, stop:1 #d76d77);");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(30);
    layout->setAlignment(Qt::AlignCenter);

    QLabel *title = new QLabel("🌸 Bear's QLink Game! 🌸", this);
    title->setStyleSheet(R"(
        color: white;
        font: bold 36pt "Comic Sans MS";
        text-align: center;
    )");
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    QString btnStyle = R"(
        QPushButton {
            color: white;
            background-color: rgba(0,0,0,0.5);
            border-radius: 15px;
            font: 20pt "Comic Sans MS";
            padding: 12px 30px;
        }
        QPushButton:hover {
            background-color: rgba(0,0,0,0.3);
        }
        QPushButton:pressed {
            background-color: rgba(0,0,0,0.1);
        }
    )";

    newGameBtn = new QPushButton("🎮 New Game");
    loadGameBtn = new QPushButton("📂 Load Game");
    exitGameBtn = new QPushButton("❌ Exit Game");

    newGameBtn->setStyleSheet(btnStyle);
    loadGameBtn->setStyleSheet(btnStyle);
    exitGameBtn->setStyleSheet(btnStyle);

    layout->addWidget(newGameBtn, 0, Qt::AlignCenter);
    layout->addWidget(loadGameBtn, 0, Qt::AlignCenter);
    layout->addWidget(exitGameBtn, 0, Qt::AlignCenter);

    connect(newGameBtn, &QPushButton::clicked, this, [this]() {
        emit NewGameSignal();
    });

    connect(loadGameBtn, &QPushButton::clicked, this, [this]() {
        emit LoadGameSignal();
    });

    connect(exitGameBtn, &QPushButton::clicked, this, [this]() {
        if (this->really_exit < 3)
        {
            this->really_exit++;
            QString text = "🥺";
            for (int i=0; i<this->really_exit; i++)
                text += " Really?";
            exitGameBtn->setText(text);
        }
        else
        {
            emit ExitGameSignal();
        }
    });
}

void MenuPage::Reset()
{
    this->really_exit = 0;
    exitGameBtn->setText("❌ Exit Game");
}

void MenuPage::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QLinearGradient grad(rect().topLeft(), rect().bottomRight());
    grad.setColorAt(0, color1);
    grad.setColorAt(1, color2);

    p.fillRect(rect(), grad);
}
