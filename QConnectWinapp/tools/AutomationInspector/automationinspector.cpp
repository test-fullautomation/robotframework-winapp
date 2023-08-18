/*
 * automationinspector.cpp
 *
 *  Created on: Jul 24, 2023
 *      Author: cuongnht
 */
#ifdef BUILD_ON_LINUX
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <pthread.h>
#include <X11/Xlib.h>
#include <jsoncpp/json/json.h>
#else
#include <json/json.h>
#endif
#include <iostream>
#include <cerrno>
#include <QTextStream>
#include "automationinspector.h"
#include "ui_automationinspector.h"

#include <sstream>
#include <QMessageBox>

#include <fstream>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialogButtonBox>

#include "elementtreemodel.h"
#include "../common/utilities.h"
#include "../common/constants.h"
#include "adddefinitiondialog.h"

const string LOG_FILE = "AutomationInspector.log";

void logInfor(const char* format, ...) {
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    std::ofstream outfile;
    outfile.open(LOG_FILE, std::ios::out | std::ios::app); // Open in append mode
    if (outfile.is_open()) {
        outfile << buffer << std::endl;
        outfile.close();
    }
}

PluginManager* AutomationInspector::pluginManager = NULL;

///
/// \brief AutomationInspector::AutomationInspector
/// \param parent
///
AutomationInspector::AutomationInspector(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AutomationInspector)
    , mainWindow(NULL)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/img/icons/automation-icon.png"));


    // Create the "Execute" action
    QAction *actionExecuteAUT = new QAction(QIcon(":/img/icons/execute-icon.png"), tr("Exec AUT"), this);
    ui->mainToolBar->addAction(actionExecuteAUT);

    // Connect the "Execute" action to the slot that handles the file open functionality
    connect(actionExecuteAUT, &QAction::triggered, this, &AutomationInspector::executeAUT);

    // Create the "Refresh" action
    QAction *actionRefresh = new QAction(QIcon(":/img/icons/refresh-icon.png"), tr("Refresh"), this);
    ui->mainToolBar->addAction(actionRefresh);

    // Connect the "Refresh" action to the slot that handles the file open functionality
    connect(actionRefresh, &QAction::triggered, this, &AutomationInspector::updateAUTInfor);

    ui->treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    eleTreeModel = new ElementTreeModel(this);
    connect(ui->treeView, &QTreeView::clicked, this, &AutomationInspector::EleTreeViewClicked);


    propsTableModel = new QStandardItemModel(this);
    ui->tableView->setModel(propsTableModel);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    QObject::connect(propsTableModel, &QStandardItemModel::dataChanged, [&](const QModelIndex &topLeft, const QModelIndex &bottomRight) {
        if (topLeft.column() == 0) {
            for (int row = topLeft.row(); row <= bottomRight.row(); ++row) {
                QModelIndex checkBoxIndex = topLeft.sibling(row, 0);
                QItemSelectionModel *selectionModel = ui->treeView->selectionModel();
                QModelIndexList selectedIndexes = selectionModel->selectedIndexes();
                const ViewerAutoObject *treeModelItem = static_cast<const ViewerAutoObject*>(selectedIndexes[0].internalPointer());
                string propName = propsTableModel->data(checkBoxIndex).toString().toStdString();
                treeModelItem->propertiesSelectedMap[propName] = checkBoxIndex.data(Qt::CheckStateRole).toBool();
            }
        }
    });

    definitionTableModel = new QStandardItemModel(this);
    ui->secondTableView->setModel(definitionTableModel);
    ui->secondTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    definitionTableModel->setColumnCount(2);

    // Create the "Add definition" action
    QAction *actionAddDefinition = new QAction(QIcon(":/img/icons/add-icon.png"), tr("Add definition"), this);
    ui->definitionToolBar->addAction(actionAddDefinition);

    // Connect the "Add definition" action to the slot that handles the file open functionality
    connect(actionAddDefinition, &QAction::triggered, this, &AutomationInspector::AddControlDefinition);

    // Create the "Remove definition" action
    QAction *actionRemoveDefinition = new QAction(QIcon(":/img/icons/remove-icon.png"), tr("Remove definition"), this);
    ui->definitionToolBar->addAction(actionRemoveDefinition);

    // Connect the "Remove definition" action to the slot that handles the file open functionality
    connect(actionRemoveDefinition, &QAction::triggered, this, &AutomationInspector::RemoveControlDefinition);

    // Create the "Clean definitions" action
    QAction *actionClearDefinitions = new QAction(QIcon(":/img/icons/clean-icon.png"), tr("Clear all definitions"), this);
    ui->definitionToolBar->addAction(actionClearDefinitions);

    // Connect the "Clean definitions" action to the slot that handles the file open functionality
    connect(actionClearDefinitions, &QAction::triggered, this, &AutomationInspector::ClearControlDefinitions);

    // Create the "Export definitions" action
    QAction *actionExportDefinitions = new QAction(QIcon(":/img/icons/export-icon.png"), tr("Export definitions"), this);
    ui->definitionToolBar->addAction(actionExportDefinitions);

    // Connect the "Export definitions" action to the slot that handles the file open functionality
    connect(actionExportDefinitions, &QAction::triggered, this, &AutomationInspector::ExportControlDefinitions);

    pluginManager = new PluginManager();
    pluginManager->loadPlugins("Plugins\\");

    toggleMouseAction = new QAction(QIcon(":/img/icons/target-icon.png"), tr("Inspect controls"), this);
    connect(toggleMouseAction, &QAction::triggered, this, &AutomationInspector::toggleMouseTracking);
    toggleMouseAction->setCheckable(true);
    ui->mainToolBar->addAction(toggleMouseAction);
    mouseTrackingEnabled = false;
    MousePositionTracker* tracker = new MousePositionTracker(this);
    connect(tracker, &MousePositionTracker::mouseMoved, this, &AutomationInspector::updateMousePosition);


    // Initialize the QProcess instance
    process = new QProcess(this);
}

///
/// \brief Destructor for AutomationInspector class.
///
AutomationInspector::~AutomationInspector()
{
    pluginManager->uninitializePlugin(m_sPlugin);
    pluginManager->unloadPlugins();
    delete pluginManager;
    delete ui;
    delete process;
}

///
/// \brief AutomationInspector::toggleMouseTracking
///
void AutomationInspector::toggleMouseTracking() {
    if(isStartedAUT)
    {
        mouseTrackingEnabled = !mouseTrackingEnabled;
        toggleMouseAction->setChecked(mouseTrackingEnabled);
    }
    else {
        QMessageBox::information(this, tr("Not start AUT"), tr("The AUT has not been started. Please start AUT before use Inspect function."));
        toggleMouseAction->setChecked(false);
    }

}

///
/// \brief AutomationInspector::findIndexRecursive
/// \param parentIndex
/// \param targetItem
/// \return
///
QModelIndex AutomationInspector::findIndexRecursive(const QModelIndex &parentIndex, AutoObject *targetItem) {
    int numRows = eleTreeModel->rowCount(parentIndex);
    ui->treeView->expand(parentIndex);
    for (int row = 0; row < numRows; ++row) {
        QModelIndex childIndex = eleTreeModel->index(row, 0, parentIndex);
        AutoObject *childItem = static_cast<AutoObject*>(childIndex.internalPointer());
        if (childItem == targetItem) {
            return childIndex;
        }


        QModelIndex recursiveResult = findIndexRecursive(childIndex, targetItem);
        if (recursiveResult.isValid()) {
            return recursiveResult;
        }
    }

    return QModelIndex();
}

///
/// \brief Get the current position of mouse and show the properties of pointed element.
/// \param globalMousePos - Position of mouse on screen.
///
void AutomationInspector::updateMousePosition(const QPoint& globalMousePos)
{
    if(mouseTrackingEnabled && isStartedAUT)
    {
        int x = globalMousePos.x();
        int y = globalMousePos.y();
        std::cout<< "x:" << x << " y:" << y;
        QModelIndex rootIndex = ui->treeView->model()->index(0, 0);
        AutoObject *topLevelItem = static_cast<AutoObject*>(rootIndex.internalPointer());
        AutoObject* smallestEle = topLevelItem->FindSmallestChildContainingPoint(x,y);
        if (smallestEle != nullptr)
        {
             std::cout<< "x:" << x << " y:" << y;
             if(isDrawRect)
             {
  #ifdef BUILD_ON_LINUX
                 Utilities::drawHighlight(mDrawingRect.x, mDrawingRect.y, mDrawingRect.w, mDrawingRect.h, 2,  0xFF0000, GXclear);
  #else
                 Utilities::ClearRectangles((HWND)smallestEle->WindowId());
                 Sleep(INTERVAL_TIME);
  #endif
                 isDrawRect = false;

             }

             RECT eleRect;
             eleRect.left = smallestEle->X();
             eleRect.top = smallestEle->Y();
             eleRect.right = smallestEle->X() + smallestEle->Width();
             eleRect.bottom = smallestEle->Y() + smallestEle->Height();
             int result = Utilities::HighlightWindow((HWND)smallestEle->WindowId(), eleRect);
             if (result != -1) {
                isDrawRect = true;
                QModelIndex desiredIndex = findIndexRecursive(rootIndex, smallestEle);
                if (desiredIndex.isValid()) {
                    ui->treeView->expand(rootIndex);
                    ui->treeView->setCurrentIndex(desiredIndex);
                    QMetaObject::invokeMethod(ui->treeView, "clicked", Qt::DirectConnection, Q_ARG(QModelIndex, desiredIndex));
                }
             }
        }
    }

}

///
/// \brief Determine if a highlight rectangle was drawn.
///
bool AutomationInspector::isDrawRect = false;

///
/// \brief AutomationInspector::mDrawingRect
///
currectDrawingRect AutomationInspector::mDrawingRect;

///
/// \brief Update properties of a selected item to Properties Table's view.
/// \param showItem - Item to be shown properties.
///
void AutomationInspector::UpdatePropertiesTableItems(const ViewerAutoObject *showItem)
{
    int iRowIndex = 0;
    propsTableModel->clear();
    propsTableModel->setRowCount(PROPERTIES_MAP.size());
    propsTableModel->setColumnCount(2);
    for (auto it = PROPERTIES_MAP.begin(); it != PROPERTIES_MAP.end(); it++)
    {
        auto getPropFunc = it->second.first;
        if (getPropFunc != NULL)
        {
            QStandardItem* item0 = new QStandardItem(true);
            item0->setCheckable(true);
            map<string, bool> test = showItem->propertiesSelectedMap;
            item0->setCheckState(showItem->propertiesSelectedMap.at(it->first) ? Qt::Checked : Qt::Unchecked);
            item0->setText(QString::fromStdString(it->first));
            propsTableModel->setItem(iRowIndex, 0, item0);
            string value = getPropFunc(*showItem);
            propsTableModel->setData(propsTableModel->index(iRowIndex, 1), QString::fromStdString(value));
            iRowIndex++;
        }

    }
}


///
/// \brief Handle for clicking item on Elements treeview.
/// \param index - index of the clicked item.
///
void AutomationInspector::EleTreeViewClicked(const QModelIndex &index)
{
    if (!index.isValid() || !eleTreeModel || !propsTableModel)
           return;

       if (!index.parent().isValid()) // Top-level item clicked
       {
           const ViewerAutoObject *topLevelItem = static_cast<const ViewerAutoObject*>(index.internalPointer());
           // Update the tableView with the properties of the top-level item
           UpdatePropertiesTableItems(topLevelItem);

           if(isDrawRect)
           {
#ifdef BUILD_ON_LINUX
               Utilities::drawHighlight(mDrawingRect.x, mDrawingRect.y, mDrawingRect.w, mDrawingRect.h, 2,  0xFF0000, GXclear);
#else
               Utilities::ClearRectangles((HWND)topLevelItem->WindowId());
               Sleep(INTERVAL_TIME);
#endif
               isDrawRect = false;

           }

           int result = -1;
#ifdef BUILD_ON_LINUX
           result = Utilities::drawHighlight(   topLevelItem->X(),
                                                    topLevelItem->Y(),
                                                    topLevelItem->Width(),
                                                    topLevelItem->Height(),
                                                    2,
                                                    0xFF0000,
                                                    GXcopy);
#else
           RECT eleRect;
           eleRect.left = topLevelItem->X();
           eleRect.top = topLevelItem->Y();
           eleRect.right = topLevelItem->X() + topLevelItem->Width();
           eleRect.bottom = topLevelItem->Y() + topLevelItem->Height();
           result = Utilities::HighlightWindow((HWND)topLevelItem->WindowId(), eleRect);
#endif
           if (result != -1) {
               isDrawRect = true;
           }
       }
       else // Child item clicked
       {
           const ViewerAutoObject *parentItem = static_cast<const ViewerAutoObject*>(index.parent().internalPointer());
           if (parentItem && index.row() < parentItem->Children().size())
           {
               const ViewerAutoObject *childItem = (ViewerAutoObject*)parentItem->Children()[index.row()];
               // Update the tableView with the properties of the child item
               UpdatePropertiesTableItems(childItem);

               if(isDrawRect)
               {
#ifdef BUILD_ON_LINUX
                   Utilities::drawHighlight(mDrawingRect.x, mDrawingRect.y, mDrawingRect.w, mDrawingRect.h, 2,  0xFF0000, GXclear);
#else
                   Utilities::ClearRectangles((HWND)childItem->WindowId());
                   Sleep(INTERVAL_TIME);
#endif
                   isDrawRect = false;
               }

               int result = -1;
#ifdef BUILD_ON_LINUX
               result = Utilities::drawHighlight(   childItem->X(),
                                                        childItem->Y(),
                                                        childItem->Width(),
                                                        childItem->Height(),
                                                        2,
                                                        0xFF0000,
                                                        GXcopy);
#else
               RECT eleRect;
               eleRect.left = childItem->X();
               eleRect.top = childItem->Y();
               eleRect.right = childItem->X() + childItem->Width();
               eleRect.bottom = childItem->Y() + childItem->Height();
               result = Utilities::HighlightWindow((HWND)childItem->WindowId(), eleRect);
#endif
               if (result != -1) {
                   isDrawRect = true;
               }
           }
       }
}

///
/// \brief Execute the AUT for inspecting.
///
void AutomationInspector::executeAUT()
{
    QDialog dialog;
    QString selectedFilePath;
    QString selectedPlugin;
    dialog.setWindowTitle("Execute AUT");
    dialog.setMinimumWidth(300);

    QVBoxLayout *layout = new QVBoxLayout();

    // Textbox
    QLabel *label = new QLabel("AUT Path:");
    QLineEdit *lineEdit = new QLineEdit();
    layout->addWidget(label);
    layout->addWidget(lineEdit);

    // File Browser Button
    QPushButton *browseButton = new QPushButton("Browse");
    layout->addWidget(browseButton);



    // Connect the Browse button to open a file dialog
    QObject::connect(browseButton, &QPushButton::clicked, [&](){
        QString filePath = QFileDialog::getOpenFileName(&dialog, tr("Executable File"), QString(), tr("All Files (*);;Text Files (*.txt);;CSV Files (*.csv)"));
        if (!filePath.isEmpty()) {
            lineEdit->setText(filePath);
            selectedFilePath = filePath;
        }
    });

    // Combo Box
    QLabel *comboLabel = new QLabel("Select plugin:");
    QComboBox *comboBox = new QComboBox();
    for (string pluginName : pluginManager->supportedPlugins())
    {
        comboBox->addItem(QString::fromStdString(pluginName));
    }

    layout->addWidget(comboLabel);
    layout->addWidget(comboBox);

    selectedPlugin = comboBox->itemText(0);

    // Connect combo box's activated signal to store selected item
    QObject::connect(comboBox, QOverload<int>::of(&QComboBox::activated), [&](int index) {
        selectedPlugin = comboBox->itemText(index); // Store the selected combo box item
    });

    // Add layout to the dialog
    dialog.setLayout(layout);

    // Add OK and Cancel buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(buttonBox);

    // Connect the buttons to accept or reject the dialog
    QObject::connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    // Executing the dialog and waiting for it to close
    int result = dialog.exec();

    if (result == QDialog::Accepted)
    {
        m_sPlugin = selectedPlugin.toStdString();
        // Use selectedFilePath and selectedComboBoxItem as needed
        selectedFilePath = lineEdit->text();
        if (!selectedFilePath.isEmpty())
        {
#ifdef BUILD_ON_LINUX
            sleep(1);
#else
            Sleep(1000);
#endif

            pluginManager->initializePlugin(m_sPlugin);
            int iRes = pluginManager->launchAUT(m_sPlugin, selectedFilePath.toStdString());
            if(iRes != -1)
            {
                isStartedAUT = true;
                InitializeAgent();
            }

        }
    }
}

///
/// \brief AutomationInspector::ResetData
///
void AutomationInspector::ResetData()
{
//    logInfor("setModel null");
    ui->treeView->setModel(nullptr);
//    logInfor("setModel null done");
    DELETE_POINTER(mainWindow);
}

///
/// \brief AutomationInspector::updateAUTInfor
/// \return
///
int AutomationInspector::updateAUTInfor()
{
    int iRes = 0;
    char *outInfo = new char[MAX_MSG_SIZE];
    memset(outInfo, 0, MAX_MSG_SIZE);
    pluginManager->getWindowInforString(m_sPlugin, outInfo);
    string jsonString = string(outInfo);
    if(Utilities::isFilePath(string(outInfo)))
    {
        jsonString = Utilities::readFileContent(string(outInfo));
    }

    mainWindow = (ViewerAutoObject*)ViewerAutoObject::deserializeTree(jsonString, NULL, (CreateInstanceByMapFunc)&ViewerAutoObject::createInstanceByMap);
    delete []outInfo;

    if (mainWindow)
    {
        eleTreeModel->setData(mainWindow);
        ui->treeView->setModel(eleTreeModel);
        iRes = 1;
    }
    return iRes;
}

///
/// \brief AutomationInspector::AddControlDefinition
///
void AutomationInspector::AddControlDefinition()
{
    QItemSelectionModel *selectionModel = ui->treeView->selectionModel();
    QModelIndexList selectedIndexes = selectionModel->selectedIndexes();
    const ViewerAutoObject *treeModelItem = static_cast<const ViewerAutoObject*>(selectedIndexes[0].internalPointer());
    Json::Value jsonRoot;

    for (auto it = treeModelItem->propertiesSelectedMap.begin(); it != treeModelItem->propertiesSelectedMap.end(); it++)
    {
        if (it->second == true)
        {
            jsonRoot[it->first] = PROPERTIES_MAP.at(it->first).first(*treeModelItem);
        }

    }

    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";
    std::string jsonString = Json::writeString(builder, jsonRoot);

    AddDefinitionDialog addDialog(this, jsonString);
    if (addDialog.exec() == QDialog::Accepted) {
        // Get the entered name and JSON from the dialog
        string name = addDialog.getName();
        string json = addDialog.getDefinition();

        // Do something with the name and JSON, e.g., add to tableview2
        QList<QStandardItem*> rowItems;
        QStandardItem* item0 = new QStandardItem(true);
        item0->setCheckable(true);
        item0->setCheckState(Qt::Unchecked);
        item0->setText(QString::fromStdString(name));
        QStandardItem* item1 = new QStandardItem(QString::fromStdString(json));
        rowItems << item0 << item1;
        definitionTableModel->appendRow(rowItems);
    }
}

///
/// \brief AutomationInspector::RemoveControlDefinition
///
void AutomationInspector::RemoveControlDefinition()
{
    QDialog dialog;
    QSet<int> selectedRows;

    //  Get all selected definitions
    for (int row = 0; row < definitionTableModel->rowCount(); ++row)
    {
        QModelIndex index = definitionTableModel->index(row, 0);
        bool isChecked = definitionTableModel->data(index, Qt::CheckStateRole) == Qt::Checked;
        if (isChecked) {
             selectedRows.insert(index.row());
        }
    }

    for (int row : selectedRows) {
        definitionTableModel->removeRow(row);
    }
}

///
/// \brief AutomationInspector::ClearControlDefinitions
///
void AutomationInspector::ClearControlDefinitions()
{
    definitionTableModel->clear();
}

///
/// \brief AutomationInspector::ExportControlDefinitions
///
void AutomationInspector::ExportControlDefinitions()
{
    QDialog dialog;
    QString content = "*** Variables ***\n";

    //  Get all selected definitions
    for (int row = 0; row < definitionTableModel->rowCount(); ++row)
    {
        QModelIndex index = definitionTableModel->index(row, 0);
        bool isChecked = definitionTableModel->data(index, Qt::CheckStateRole) == Qt::Checked;
        if (isChecked) {
            QString name = definitionTableModel->data(definitionTableModel->index(row, 0)).toString();
            QString definition = definitionTableModel->data(definitionTableModel->index(row, 1)).toString();
            Json::CharReaderBuilder builder;
            Json::Value jsonRoot;
            JSONCPP_STRING err;
            std::istringstream iss(definition.toStdString());

            if (!Json::parseFromStream(builder, iss, &jsonRoot, &err)) {
                std::cerr << "Error parsing JSON: " << err << std::endl;
            }
            else if (!jsonRoot.isObject()) {
                std::cerr << "Invalid JSON format: Root should be an object." << std::endl;
            }
            else
            {
                QString attrDef = "";
                for (const auto& key : jsonRoot.getMemberNames()) {
                    const Json::Value& value = jsonRoot[key];
                    attrDef += QString("%1=%2\t").arg(QString::fromStdString(key),
                                                      QString::fromStdString(value.asString()));
                }

                content += QString("&{%1}    %2\n").arg(name, attrDef);
            }



        }
    }

    // Save resource fille with selected definitions
    QString filePath = QFileDialog::getSaveFileName(&dialog, "Save File", "", "Resource Files (*.resource);;All Files (*)");
    if (!filePath.isEmpty()) {
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << content;
            file.close();
            QMessageBox::information(this, tr("File Saved"), tr("The resource file has been saved successfully."));
        }
    }
}

///
/// \brief AutomationInspector::InitializeAgent
///
void AutomationInspector::InitializeAgent()
{
    if (!progressWindow) {
        progressWindow = new ProgressWindow(this);
    }
//    logInfor("progress bar show");
    // Show the progress window
    progressWindow->show();
    QApplication::processEvents(); // Update the GUI
//    logInfor("Update UI");
    ResetData();
//    logInfor("rESET DATA");
    auto updateAUTInforFunc = std::bind(&AutomationInspector::updateAUTInfor, this);
    WorkerThread *workerThread = new WorkerThread(updateAUTInforFunc, this);
//    connect(workerThread, &WorkerThread::started, progressWindow, &ProgressWindow::show);

    // Connect other signals and slots as needed
    connect(workerThread, &WorkerThread::finished, this, &AutomationInspector::UpdateAUTInforFinished);

    workerThread->start();

}

///
/// \brief AutomationInspector::UpdateAUTInforFinished
/// \param iRes
///
void AutomationInspector::UpdateAUTInforFinished(const int iRes)
{
    if (iRes)
    {
        QMessageBox::information(this, tr("AUT loaded"), tr("The AUT started successfully!!!"));
    }

    progressWindow->close();
    delete progressWindow;
}
