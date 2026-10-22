#include "emoji_dialog.h"

EmojiDialog::EmojiDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("选择表情");
    setFixedSize(350, 300);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    emojiLayout = new QGridLayout();
    emojiLayout->setSpacing(10);
    mainLayout->addLayout(emojiLayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout();

    QPushButton *addBtn = new QPushButton("添加表情", this);
    addBtn->setStyleSheet("QPushButton { background-color: rgb(0, 148, 255); color: white; border: none; border-radius: 4px; padding: 6px 16px; }"
                         "QPushButton:hover { background-color: rgb(0, 130, 230); }");
    connect(addBtn, &QPushButton::clicked, this, &EmojiDialog::onAddEmoji);
    buttonLayout->addWidget(addBtn);

    QPushButton *removeBtn = new QPushButton("删除表情", this);
    removeBtn->setStyleSheet("QPushButton { background-color: rgb(220, 53, 69); color: white; border: none; border-radius: 4px; padding: 6px 16px; }"
                            "QPushButton:hover { background-color: rgb(200, 33, 49); }");
    connect(removeBtn, &QPushButton::clicked, this, &EmojiDialog::onRemoveEmoji);
    buttonLayout->addWidget(removeBtn);

    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);

    selectedBtn = nullptr;
    initEmojis();
    refreshEmojiButtons();
}

EmojiDialog::~EmojiDialog()
{
}

void EmojiDialog::initEmojis()
{
    emojiPaths.clear();
    emojiPaths << ":/res/18.png";
    emojiPaths << ":/res/19.png";
    emojiPaths << ":/res/20.png";
}

void EmojiDialog::refreshEmojiButtons()
{
    while (QLayoutItem *item = emojiLayout->takeAt(0))
    {
        delete item->widget();
        delete item;
    }

    for (int i = 0; i < emojiPaths.size(); ++i)
    {
        QPushButton *btn = new QPushButton(this);
        QPixmap pixmap(emojiPaths[i]);
        if (!pixmap.isNull())
        {
            btn->setIcon(QIcon(pixmap.scaled(48, 48, Qt::KeepAspectRatio)));
            btn->setIconSize(QSize(48, 48));
        }
        btn->setFixedSize(60, 60);
        btn->setStyleSheet("QPushButton { border: 1px solid #ddd; border-radius: 4px; }"
                          "QPushButton:hover { background-color: #f0f0f0; }");
        btn->setProperty("emojiPath", emojiPaths[i]);

        connect(btn, &QPushButton::clicked, this, &EmojiDialog::onEmojiClicked);

        int row = i / 4;
        int col = i % 4;
        emojiLayout->addWidget(btn, row, col);
    }
}

void EmojiDialog::onEmojiClicked()
{
    QPushButton *btn = qobject_cast<QPushButton *>(sender());
    if (!btn)
        return;

    QString emojiPath = btn->property("emojiPath").toString();

    if (selectedBtn == btn)
    {
        selectedBtn->setStyleSheet("QPushButton { border: 1px solid #ddd; border-radius: 4px; }"
                                  "QPushButton:hover { background-color: #f0f0f0; }");
        selectedBtn = nullptr;
        selectedEmojiPath.clear();
        emit emojiSelected(emojiPath);
        accept();
    }
    else
    {
        if (selectedBtn)
        {
            selectedBtn->setStyleSheet("QPushButton { border: 1px solid #ddd; border-radius: 4px; }"
                                      "QPushButton:hover { background-color: #f0f0f0; }");
        }

        btn->setStyleSheet("QPushButton { border: 2px solid rgb(0, 148, 255); border-radius: 4px; background-color: #e8f4fc; }"
                          "QPushButton:hover { background-color: #f0f0f0; }");
        selectedBtn = btn;
        selectedEmojiPath = emojiPath;
    }
}

void EmojiDialog::onAddEmoji()
{
    QString filePath = QFileDialog::getOpenFileName(this, "选择表情图片", "", "图片文件 (*.png *.jpg *.jpeg *.bmp)");
    if (!filePath.isEmpty())
    {
        emojiPaths.append(filePath);
        refreshEmojiButtons();
    }
}

void EmojiDialog::onRemoveEmoji()
{
    if (selectedBtn && !selectedEmojiPath.isEmpty())
    {
        int index = emojiPaths.indexOf(selectedEmojiPath);
        if (index >= 0)
        {
            emojiPaths.removeAt(index);
            selectedBtn = nullptr;
            selectedEmojiPath.clear();
            refreshEmojiButtons();
        }
    }
}