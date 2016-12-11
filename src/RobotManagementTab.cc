#include "RobotManagementTab.hh"

using namespace std;

RobotManagementTab::RobotManagementTab() :
    QWidget(),
    state(Start) {
	
    // Lewa czesc - ustawianie polozenia i orientacji
    verticalLayoutWidget = new QWidget ( this );
    verticalLayoutWidget->setObjectName ( "verticalLayoutWidget" );
    verticalLayoutWidget->setGeometry ( QRect ( QPoint(20, 50), QSize(150, 90)) );// 171, 141 ) );
    verticalLayout = new QVBoxLayout ( verticalLayoutWidget );
    verticalLayout->setSpacing ( 6 );
    verticalLayout->setContentsMargins ( 11, 11, 11, 11 );
    verticalLayout->setObjectName ( "verticalLayout" );
    verticalLayout->setContentsMargins ( 0, 0, 0, 0 );

    lineEditX = new QLineEdit ( verticalLayoutWidget );
    lineEditX->setObjectName ( "lineEditX" );
    lineEditX->setAlignment ( Qt::AlignCenter );
    lineEditX->setText ( QString() );
    lineEditX->setPlaceholderText ( "X" );
    verticalLayout->addWidget ( lineEditX );

    lineEditY = new QLineEdit ( verticalLayoutWidget );
    lineEditX->setObjectName ( "lineEditY" );
    lineEditY->setAlignment ( Qt::AlignCenter );
    lineEditY->setText ( QString() );
    lineEditY->setPlaceholderText ( "Y" );
    verticalLayout->addWidget ( lineEditY );

    lineEditOrient = new QLineEdit ( verticalLayoutWidget );
    lineEditOrient->setObjectName ( "lineEditOrient" );
    lineEditOrient->setAlignment ( Qt::AlignCenter );
    lineEditOrient->setText ( QString() );
    lineEditOrient->setPlaceholderText ( "Orientacja" );
    verticalLayout->addWidget ( lineEditOrient );

    verticalSpacer = new QSpacerItem ( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding );
    verticalLayout->addItem ( verticalSpacer );

    horizontalLayout = new QHBoxLayout ( verticalLayoutWidget );
    horizontalLayout->setSpacing ( 6 );
    horizontalLayout->setObjectName ( "horizontalLayout" );

    pushButtonUstaw = new QPushButton ( verticalLayoutWidget );
    pushButtonUstaw->setObjectName ( "pushButtonUstaw" );
    pushButtonUstaw->setText ( "Ustaw" );
    horizontalLayout->addWidget ( pushButtonUstaw );

    pushButtonReset = new QPushButton ( verticalLayoutWidget );
    pushButtonReset->setObjectName ( "pushButtonReset" );
    pushButtonReset->setText ( "Reset" );
    horizontalLayout->addWidget ( pushButtonReset );

    verticalLayout->addLayout ( horizontalLayout );

    // Nazwa topicu
    labelTopicName = new QLabel ( this );
    labelTopicName->setObjectName ( "labelTopicName" );
    labelTopicName->setText ( "Nazwa topicu" );
    labelTopicName->setGeometry ( QRect ( 210, 50, 121, 21 ) );

    // Przycisk start/stop
    pushButtonStartStop = new QPushButton ( this );
    pushButtonStartStop->setObjectName ( "pushButtonStartStop" );
    pushButtonStartStop->setText ( QString() );
    pushButtonStartStop->setGeometry ( QRect ( 220, 140, 81, 51 ) );
    icon_green.addFile ( "/root/projekt_przejsciowy/res/glossy-green-button.png", QSize(), QIcon::Normal, QIcon::Off ); 
    icon_red.addFile ( "/root/projekt_przejsciowy/res/glossy-red-button.png", QSize(), QIcon::Normal, QIcon::Off );
    pushButtonStartStop->setIcon ( icon_green ); 
    pushButtonStartStop->setIconSize ( QSize ( 50, 50 ) );

    // Sloty
    QMetaObject::connectSlotsByName ( this );

    // Transport wiadomosci
   //this->node = gazebo::transport::NodePtr ( new gazebo::transport::Node() );
   //this->node->Init();
   //this->publisher = this->node->Advertise<gazebo::msgs::Int> ( "~/buttons" ); // Topic okreslony w WorldControl.cc
   //this->subscriber = this->node->Subscribe( "~/buttons", &RobotManagementTab::receivedMsg, this );
    
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
    //TODO
}

void RobotManagementTab::on_pushButtonReset_clicked() {
    // TODO
}
