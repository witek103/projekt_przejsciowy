#include "RobotManagementTab.hh"
#include <cstdlib>
#include <ros/ros.h>
#include <cmath>
#include <string>

using namespace std;

RobotManagementTab::RobotManagementTab(string topicName) :
    QWidget(),
    state(Start) {
	
    // Lewa czesc - ustawianie polozenia i orientacji
    verticalLayoutWidget = new QWidget ( this );
    verticalLayoutWidget->setObjectName ( "verticalLayoutWidget" );
    verticalLayoutWidget->setGeometry ( QRect ( QPoint(20, 20), QSize(150, 90)) );
    verticalLayout = new QVBoxLayout;
    verticalLayout->setSpacing ( 6 );
    verticalLayout->setContentsMargins ( 11, 11, 11, 11 );
    verticalLayout->setObjectName ( "verticalLayout" );
    verticalLayout->setContentsMargins ( 0, 0, 0, 0 );

    lineEditX = new QLineEdit;
    lineEditX->setObjectName ( "lineEditX" );
    lineEditX->setAlignment ( Qt::AlignCenter );
    lineEditX->setText ( QString() );
    lineEditX->setPlaceholderText ( "X" );
	lineEditX->setStyleSheet("background-color : #32CD32; color : #006400;");
    verticalLayout->addWidget ( lineEditX );

    lineEditY = new QLineEdit;
    lineEditX->setObjectName ( "lineEditY" );
    lineEditY->setAlignment ( Qt::AlignCenter );
    lineEditY->setText ( QString() );
    lineEditY->setPlaceholderText ( "Y" );
	lineEditY->setStyleSheet("background-color : #32CD32; color : #006400;");
    verticalLayout->addWidget ( lineEditY );

    lineEditOrient = new QLineEdit;
    lineEditOrient->setObjectName ( "lineEditOrient" );
    lineEditOrient->setAlignment ( Qt::AlignCenter );
    lineEditOrient->setText ( QString() );
    lineEditOrient->setPlaceholderText ( "Orientacja" );
	lineEditOrient->setStyleSheet("background-color : #32CD32; color : #006400;");
    verticalLayout->addWidget ( lineEditOrient );

    verticalSpacer = new QSpacerItem ( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding );
    verticalLayout->addItem ( verticalSpacer );

    horizontalLayout = new QHBoxLayout;
    horizontalLayout->setSpacing ( 6 );
    horizontalLayout->setObjectName ( "horizontalLayout" );

    pushButtonUstaw = new QPushButton;
    pushButtonUstaw->setObjectName ( "pushButtonUstaw" );
    pushButtonUstaw->setText ( "Ustaw" );
    horizontalLayout->addWidget ( pushButtonUstaw );

    pushButtonReset = new QPushButton;
    pushButtonReset->setObjectName ( "pushButtonReset" );
    pushButtonReset->setText ( "Reset" );
    horizontalLayout->addWidget ( pushButtonReset );

    verticalLayout->addLayout ( horizontalLayout );

    verticalLayoutWidget->setLayout(verticalLayout);

    // Nazwa topicu
    labelTopicName = new QLabel ( this );
    labelTopicName->setObjectName ( "labelTopicName" );
    labelTopicName->setText ( QString::fromStdString(topicName) );
    labelTopicName->setGeometry ( QRect ( 210, 20, 120, 20 ) );

    // Przycisk start/stop
    pushButtonStartStop = new QPushButton ( this );
    pushButtonStartStop->setObjectName ( "pushButtonStartStop" );
    pushButtonStartStop->setText ( QString() );
    pushButtonStartStop->setGeometry ( QRect ( 210, 50, 50, 50 ) );
    icon_green.addFile ( "/root/catkin_ws/src/projekt_przejsciowy/res/glossy-green-button.png", QSize(), QIcon::Normal, QIcon::Off ); 
    icon_red.addFile ( "/root/catkin_ws/src/projekt_przejsciowy/res/glossy-red-button.png", QSize(), QIcon::Normal, QIcon::Off );
    pushButtonStartStop->setIcon ( icon_green ); 
    pushButtonStartStop->setIconSize ( QSize ( 50, 50 ) );

    // Sloty
    QMetaObject::connectSlotsByName ( this );

    // Transport wiadomosci
   this->node = gazebo::transport::NodePtr ( new gazebo::transport::Node() );
   this->node->Init();
   this->publisher = this->node->Advertise<gazebo::msgs::Int> ( "~/buttons" ); // Topic okreslony w WorldControl.cc
}

void RobotManagementTab::on_pushButtonStartStop_clicked() {
    if ( state == Start ) {
        state = Stop;
        pushButtonStartStop->setIcon ( icon_red );
        pushButtonStartStop->setIconSize ( QSize ( 40, 40 ) );
    }
    else {
        state = Start;
        pushButtonStartStop->setIcon ( icon_green );
        pushButtonStartStop->setIconSize ( QSize ( 50, 50 ) );
    }
}

void RobotManagementTab::on_pushButtonUstaw_clicked() {
    bool correct;
	bool wrong = false;
	lineEditX->setStyleSheet("background-color : #32CD32; color : #006400;");
	lineEditY->setStyleSheet("background-color : #32CD32; color : #006400;");
	lineEditOrient->setStyleSheet("background-color : #32CD32; color : #006400;");

    lineEditX->text().toFloat(&correct);
    if(!correct)
    {
	ROS_INFO("Wrong X-coordinate!!");
	lineEditX->setStyleSheet("background-color : #FF6347; color : #800000;");
	wrong = true;
    }

    lineEditY->text().toFloat(&correct);
    if(!correct)
    {
	ROS_INFO("Wrong Y-coordinate!!");
	lineEditY->setStyleSheet("background-color : #FF6347; color : #800000;");
	wrong = true;
    }      

    float orient = lineEditOrient->text().toFloat(&correct);
    if(!correct)
    {
	ROS_INFO("Wrong Orientation!!");
	lineEditOrient->setStyleSheet("background-color : #FF6347; color : #800000;");
	wrong = true;
    }
	if(wrong) return;

    float w = cos(orient/2), z = sin(orient/2);
    string topicName = labelTopicName->text().toStdString();   

    string command = "rosservice call /gazebo/set_model_state '{model_state: { model_name: " + topicName + ", pose: { position: { x: " + lineEditX->text().toStdString() + ", y: " + lineEditY->text().toStdString() + ",z: 0 }, orientation: {x: 0, y: 0, z: "+ to_string(z) + ", w: " + to_string(w) +" } }, twist: { linear: {x: 0.0 , y: 0 ,z: 0 } , angular: { x: 0.0 , y: 0 , z: 0.0 } } , reference_frame: world } }'";
    system(command.c_str());
}

void RobotManagementTab::on_pushButtonReset_clicked() {
    string topicName = labelTopicName->text().toStdString();   
    string command = "rosservice call /gazebo/set_model_state '{model_state: { model_name: "+ topicName + ", pose: { position: { x: 0, y: 0 ,z: 0 }, orientation: {x: 0, y: 0, z: 0, w: 1 } }, twist: { linear: {x: 0.0 , y: 0 ,z: 0 } , angular: { x: 0.0 , y: 0 , z: 0.0 } } , reference_frame: world } }'";
    system(command.c_str());
}

