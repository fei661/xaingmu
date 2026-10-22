#ifndef EMOJI_DIALOG_H
#define EMOJI_DIALOG_H

#include <QDialog>
#include <QGridLayout>
#include <QPushButton>
#include <QPixmap>
#include <QVariant>
#include <QStringList>
#include <QFileDialog>
#include <QVBoxLayout>

class EmojiDialog : public QDialog
{
    Q_OBJECT

public:
    EmojiDialog(QWidget *parent = nullptr);
    ~EmojiDialog();

signals:
    void emojiSelected(QString emojiPath);

private slots:
    void onEmojiClicked();
    void onAddEmoji();
    void onRemoveEmoji();

private:
    void initEmojis();
    void refreshEmojiButtons();

    QGridLayout *emojiLayout;
    QStringList emojiPaths;
    QString selectedEmojiPath;
    QPushButton *selectedBtn;
};

#endif // EMOJI_DIALOG_H