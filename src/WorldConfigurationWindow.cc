#include "WorldConfigurationWindow.hh"
#include <QPushButton>
#include <dirent.h>

using namespace std;
using namespace gazebo;

WorldConfigurationWindow::WorldConfigurationWindow() : QDialog()
{
   this->setWindowFlags(Qt::WindowStaysOnTopHint);

   laboratoryList = new QListWidget(this);
   laboratoryList->setGeometry(QRect(QPoint(10, 10),QSize(250, 150)));

   line = new QListWidget(this);
   line->setGeometry(QRect(QPoint(268, 0),QSize(4, 210)));
   
   
   DIR *dir;
   struct dirent *ent;
   if ((dir = opendir("/root/catkin_ws/src/projekt_przejsciowy/worlds")) != NULL)
   {
      while ((ent = readdir(dir)) != NULL)
      {
         string filename(ent->d_name);
         if (filename.find(".txt") != string::npos)
         {
            laboratoryList->addItem(QString::fromStdString(filename.substr(0, filename.size()-4)));
         }
      }
      closedir(dir);
   }
    
   robotListToAdd = new QListWidget(this);
   robotListToAdd->setGeometry(QRect(QPoint(280, 10),QSize(200, 150)));
   robotListToAdd->addItem("Robot Pioneer 1");
   robotListToAdd->addItem("Robot Pioneer 2");
   robotListToAdd->addItem("Robot Pioneer 3");

   addedRobotList = new QListWidget(this);
   addedRobotList->setGeometry(QRect(QPoint(550, 10),QSize(200, 150)));

   QPushButton *buttonLoad = new QPushButton("Wczytaj",this);
   buttonLoad->setGeometry(QRect(QPoint(10, 170),QSize(120, 30)));
   connect(buttonLoad, SIGNAL(clicked()), this, SLOT(OnButtonLoad()));
   
   QPushButton *buttonClear = new QPushButton("Wyczysc",this);
   buttonClear->setGeometry(QRect(QPoint(140, 170),QSize(120, 30)));
   connect(buttonClear, SIGNAL(clicked()), this, SLOT(OnButtonClear()));

   QPushButton *buttonAddRobot = new QPushButton("Dodaj",this);
   buttonAddRobot->setGeometry(QRect(QPoint(485, 55),QSize(60, 30)));
   connect(buttonAddRobot, SIGNAL(clicked()), this, SLOT(OnButtonAddRobot()));

   QPushButton *buttonDeleteRobot = new QPushButton("Schowaj",this);
   buttonDeleteRobot->setGeometry(QRect(QPoint(485, 95),QSize(60, 30)));
   //buttonDeleteRobot->setVisible(false);
   connect(buttonDeleteRobot, SIGNAL(clicked()), this, SLOT(OnButtonDeleteRobot()));

   this->adjustSize();
   this->setFixedSize(this->size());
   
   // Potrzebne dla transportu wiadomości
   this->node = transport::NodePtr(new transport::Node());
   this->node->Init();
   this->publisher = this->node->Advertise<msgs::Int>("~/buttons");
}

void WorldConfigurationWindow::OnButtonLoad()
{
   // Zapisywanie info o wczytanej sali i robotach (do przemyślenia)
   this->labName = laboratoryList->currentItem()->text();
   addedRobots.clear();
   for (int i = 0; i < addedRobotList->count(); ++i)
   {
      addedRobots.push_back(addedRobotList->item(i)->text());
   }
   
   // Numer w wiadomości to jej identyfikator (póki co)
   msgs::Int MyMsg;
   MyMsg.set_data(laboratoryList->currentRow());
   this->publisher->Publish(MyMsg);
}

void WorldConfigurationWindow::OnButtonClear()
{
   // Numer w wiadomości to jej identyfikator (póki co)
   msgs::Int MyMsg;
   MyMsg.set_data(99);
   this->publisher->Publish(MyMsg);
}

void WorldConfigurationWindow::OnButtonAddRobot()
{
   QListWidgetItem *item = robotListToAdd->takeItem(robotListToAdd->currentRow());
   addedRobotList->addItem(item);
   // Add
   gazebo::msgs::Int msg;
   string robot_name = item->text().toStdString();
   int robot_id = robot_name[robot_name.length()-1] - '0';
   msg.set_data( 100 + robot_id );  // switch in world_plugin.cc 
   this->publisher->Publish(msg); 
   emit addNewRobot(robot_id);
}

void WorldConfigurationWindow::OnButtonDeleteRobot()
{
   QListWidgetItem *item = addedRobotList->takeItem(addedRobotList->currentRow());
   if (item != nullptr) {
    robotListToAdd->addItem(item);
    // Hide
    gazebo::msgs::Int msg;
    string robot_name = item->text().toStdString();
    int robot_id = robot_name[robot_name.length()-1] - '0';
    msg.set_data( 200 + robot_id );  // switch in world_plugin.cc
    this->publisher->Publish(msg); 
    emit hideARobot(robot_id);
   }
}



