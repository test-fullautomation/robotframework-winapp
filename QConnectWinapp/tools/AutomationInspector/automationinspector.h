/*
 * automationinspector.h
 *
 *  Created on: Jul 24, 2023
 *      Author: cuongnht
 */
#ifndef AUTOMATIONINSPECTOR_H
#define AUTOMATIONINSPECTOR_H

#include <QApplication>
#include <QMainWindow>
#include <QAction>
#include <QFileDialog>
#include <QProcess>
#include <QStandardItemModel>
#include <QMouseEvent>
#include <QTimer>
#include <QCursor>
#include <QThread>
#include <QProgressBar>
#include <QVBoxLayout>
#include "elementtreemodel.h"
#include "pluginmanager.h"
#include "viewerautoobject.h"
#include "../common/MessageType.h"

QT_BEGIN_NAMESPACE
namespace Ui { class AutomationInspector; }
QT_END_NAMESPACE

struct currectDrawingRect
{
    int x;
    int y;
    unsigned int w;
    unsigned int h;
};

class ProgressWindow : public QDialog
{
    Q_OBJECT

public:
    ProgressWindow(QWidget *parent = nullptr) : QDialog(parent)
    {
        setWindowTitle("Progress");
        setFixedSize(500, 200); // Set a fixed size for the progress window
        QVBoxLayout *layout = new QVBoxLayout;

        progressBar = new QProgressBar(this);
        progressBar->setRange(0, 0);
//        progressBar->setMaximum(1); // Restore normal state
        layout->addWidget(progressBar);

        setLayout(layout);
    }

    void showEvent(QShowEvent *event) override
    {
        QDialog::showEvent(event);
//        progressBar->setValue(0); // Reset the progress bar
    }

private:
    QProgressBar *progressBar;
};

///
/// \brief The WorkerThread class
///
class WorkerThread : public QThread
{
    Q_OBJECT

public:
    WorkerThread(std::function<int()> functionToExecute, QObject *parent = nullptr)
        : QThread(parent), functionToExecute(functionToExecute)
    {
    }

    void run() override
    {
        emit started(); // Signal that the thread has started
        if (functionToExecute)
        {
            result = functionToExecute(); // Call the specified function
        }
        emit finished(result);
    }

signals:
    void started(); // Signal to indicate thread start
    void finished(const int iRes); // Signal to indicate thread start

private:
    std::function<int()> functionToExecute;
    int result;
};

///
/// \brief The MousePositionTracker class
///
class MousePositionTracker : public QObject
{
    Q_OBJECT
public:
    MousePositionTracker(QObject* parent = nullptr)
        : QObject(parent)
    {
        // Initialize the timer to track mouse movement
        mouseMoveTimer = new QTimer(this);
        connect(mouseMoveTimer, &QTimer::timeout, this, &MousePositionTracker::checkMousePosition);
        mouseMoveTimer->start(100); // Adjust the interval as needed
    }

signals:
    void mouseMoved(const QPoint& pos);

private slots:
    void checkMousePosition()
    {
        QPoint currentMousePos = QCursor::pos();
        if (currentMousePos != lastMousePos) {
            lastMousePos = currentMousePos;
            emit mouseMoved(currentMousePos);
        }
    }

private:
    QTimer* mouseMoveTimer;
    QPoint lastMousePos;
};

///
/// \brief The AutomationInspector class
///
class AutomationInspector : public QMainWindow
{
    Q_OBJECT

public:
    AutomationInspector(QWidget *parent = nullptr);
    ~AutomationInspector();

private:
    Ui::AutomationInspector *ui;
    QAction *toggleMouseAction;
    void executeAUT();
    void InitializeAgent();
    void UpdateAUTInforFinished(const int res);
    int updateAUTInfor();
    bool mouseTrackingEnabled;
    bool isStartedAUT;
    void ResetData();

    void AddControlDefinition();
    void RemoveControlDefinition();
    void ClearControlDefinitions();
    void ExportControlDefinitions();
    void toggleMouseTracking();
//    void updateMousePosition();
    void updateMousePosition(const QPoint& globalMousePos);


    QProcess *process;
    ElementTreeModel *eleTreeModel;
    QStandardItemModel *propsTableModel;
    QStandardItemModel *definitionTableModel;
    ViewerAutoObject* mainWindow;
    void EleTreeViewClicked(const QModelIndex &index);
    static bool isDrawRect;
    static currectDrawingRect mDrawingRect;
    static PluginManager *pluginManager;
    string m_sPlugin;
    static std::map<std::string, std::function<void(ViewerAutoObject&)>> propertiesMap;
    void UpdatePropertiesTableItems(const ViewerAutoObject *showItem);
    QModelIndex findIndexRecursive(const QModelIndex &parentIndex, AutoObject *targetItem);

protected:
    QProgressBar *progressBar;
    ProgressWindow *progressWindow = nullptr;
};
#endif // AUTOMATIONINSPECTOR_H
