#include <QApplication>
#include <QLabel>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QLabel l1 = QLabel();
    QLabel l2 = QLabel();

    l1.resize(300,500);
    l1.setText("HELLO");
    l1.setWindowTitle("Vertical");
    l1.show();

    l2.resize(500,300);
    l2.setText("WORLD!");
    l2.setWindowTitle("Horizontal");
    l2.show();
    return app.exec();
}