#include "extend_display.h"

Extend_Display::Extend_Display() {}

QSize Extend_Display::size()
{
    QList<QScreen*> screens = QGuiApplication::screens();

    qDebug() << "Number of screens:" << screens.size();
    for (int i = 0; i < screens.size(); ++i) {
        QScreen* screen = screens.at(i);
        qDebug() << "Screen" << i;
        qDebug() << "  Name:" << screen->name();
        qDebug() << "  Geometry:" << screen->geometry();
        qDebug() << "  Logical DPI:" << screen->logicalDotsPerInch();
        qDebug() << "  Physical DPI:" << screen->physicalDotsPerInch();
        qDebug() << "  Refresh rate:" << screen->refreshRate();
    }
    QPoint farthestPt = farthestPoint(screens);
    return QSize(farthestPt.x(), farthestPt.y());
}

QPoint Extend_Display::farthestPoint(QList<QScreen *> screens)
{
    QPoint pt(0,0);
    for (int i = 0; i < screens.size(); ++i) {
        QScreen* screen = screens.at(i);
        QRect rect = screen->geometry();
        int farthest_x = rect.x() + rect.width();
        int farthest_y = rect.y() + rect.height();
        if(pt.x() < farthest_x){
            pt.setX(farthest_x);
        }
        if(pt.y() < farthest_y){
            pt.setY(farthest_y);
        }
    }

    return pt;
}
