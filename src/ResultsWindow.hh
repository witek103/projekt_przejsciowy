#ifndef _RESULTS_WINDOW_HH_
#define _RESULTS_WINDOW_HH_

#include <QDialog>
#include <QListWidget>
#include <QString>
#include <QVector>
#include <QTimer>
#include "qcustomplot.h"

#ifndef Q_MOC_RUN
   #include <gazebo/transport/transport.hh>
#endif

class ResultsWindow : public QDialog
{
   Q_OBJECT

   public:
      ResultsWindow();
      QCustomPlot *plot;
      QCustomPlot *plot2;
      QCPCurve *Spiral1;
      QListWidget *parametersList;
	QVector<double> x1; /**< vector przechowujący współrzędne x trasy */
	QVector<double> y1; /**< vector przechowujący współrzędne y trasy */
	QVector<double> t;  /**< vector przechowujący czas */

   private slots:
	void OnButtonPoseReg1();
	void OnButtonSave1();
	void OnButtonStop();
	
	void realtimeDataSlot();
	
            
   private:
      void Received(const boost::shared_ptr<const gazebo::msgs::Quaternion> &msg);
      void Received2(const boost::shared_ptr<const gazebo::msgs::Quaternion> &msg);
      void Received3(const boost::shared_ptr<const gazebo::msgs::Quaternion> &msg);
      QTimer dataTimer;
      gazebo::transport::NodePtr node;
      gazebo::transport::PublisherPtr publisher;
      
      gazebo::transport::SubscriberPtr subscriberRobot1;
      gazebo::transport::SubscriberPtr subscriberRobot2;
      gazebo::transport::SubscriberPtr subscriberRobot3;

};

#endif
