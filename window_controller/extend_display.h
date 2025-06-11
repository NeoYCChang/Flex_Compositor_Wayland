#ifndef EXTEND_DISPLAY_H
#define EXTEND_DISPLAY_H
#include <QGuiApplication>
#include <QScreen>
#include <QDebug>
#include <QSize>

class Extend_Display
{
public:
    Extend_Display();

    QSize size();

private:
    QPoint farthestPoint(QList<QScreen*> screens);
};

#endif // EXTEND_DISPLAY_H
