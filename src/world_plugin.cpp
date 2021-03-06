#include <gazebo/common/Plugin.hh>
#ifndef Q_MOC_RUN
   #include <gazebo/transport/transport.hh>
   #include <gazebo/physics/physics.hh>
#endif
#include <ros/ros.h>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <dirent.h>
#include <unistd.h>
#include "nav_msgs/Odometry.h"

using namespace std;

namespace gazebo
{
   class WorldPluginProject : public WorldPlugin
   {

   public:

      WorldPluginProject() : WorldPlugin()
      {
         model_counter = 0;
	
      }

      struct coordinates
      {
	double x;
	double y;
	double z;
	double t;
      };

     
      vector<coordinates> robotSimulationPose;
      const double stepTime = 0.2;
      double timeCounter = 0;
      int print_counter = 0;
      bool pionier_1_subscriber_not_induced = true;

      void Load(physics::WorldPtr _world, sdf::ElementPtr _sdf)
      {
         this->world = _world;
   
         // world->SetPaused(true);
   

         this->node = transport::NodePtr(new transport::Node());
         this->node->Init();
         this->subscriber = node->Subscribe("~/buttons", &WorldPluginProject::Received, this);
         this->publisher = this->node->Advertise<msgs::Request>("~/request");
         this->publisherRobotPose = this->node->Advertise<msgs::Quaternion>("~/robotPose");
         // Make sure the ROS node for Gazebo has already been initialized                                                                                    
         if ( !ros::isInitialized() )
         {
            ROS_FATAL_STREAM("A ROS node for Gazebo has not been initialized, unable to load plugin. "
               << "Load the Gazebo system plugin 'libgazebo_ros_api_plugin.so' in the gazebo_ros package)");
            return;
         }
      }



      void DeleteStaticModels()
      {
         string ground("ground_plane");
         string pioneer("pioneer");
         physics::Model_V modelList(world->GetModels());
         for (physics::Model_V::iterator it = modelList.begin(); it != modelList.end(); ++it)
         {
            string model_name((*it)->GetName());
            if (ground.compare(model_name) && (model_name).find(pioneer) == string::npos)
            {
               msgs::Request *MyMsg = msgs::CreateRequest("entity_delete", model_name);
               this->publisher->Publish(*MyMsg);
            }
         }
      }

      void LoadRoomByName( string room_name )
      {
         DeleteStaticModels();

         ifstream room_file, model_file;
         string sdf_string, name;
         double x, y, z, roll, pitch, yaw;
         math::Pose model_pos;

         room_file.open(room_name);
         while ( room_file >> name >> x >> y >> z >> roll >> pitch >> yaw )
         {
            ostringstream model_name, model_dir;
            sdf::SDF mySDF;
            sdf::ElementPtr element_ptr;
            model_dir << "/root/.gazebo/models/" << name << "/model.sdf";
            model_file.open( model_dir.str() );
            sdf_string.clear();
            model_file.seekg(0, ios::beg);
            sdf_string.assign( ( istreambuf_iterator<char>(model_file) ), ( istreambuf_iterator<char>() ) );
            mySDF.SetFromString(sdf_string);
            model_name << name << "_" << model_counter;
            ++model_counter;
            element_ptr = mySDF.Root()->GetElement("model");
            element_ptr->GetAttribute("name")->SetFromString( model_name.str() );
            model_pos.Set(x, y, z, roll, pitch, yaw);
            element_ptr->AddElement("pose")->Set(model_pos);
            world->InsertModelSDF(mySDF);
            model_file.close();
         }
         room_file.close();
      }

      void LoadRoomByIndex( int room_index )
      {
         DIR *dir;
         struct dirent *ent;
         if ((dir = opendir("/root/catkin_ws/src/projekt_przejsciowy/worlds")) != NULL)
         {
            int file_counter = 0;
            while ((ent = readdir(dir)) != NULL)
            {
               string filename(ent->d_name);
               if (filename.find(".txt") != string::npos)
               {
                  if ( file_counter == room_index )
                  {
                     ostringstream file;
                     file << "/root/catkin_ws/src/projekt_przejsciowy/worlds/" << filename;
                     LoadRoomByName( file.str() );
                     break;
                  }
                  ++file_counter;
               }
            }
            closedir(dir);
         }
      }


      void Received(const boost::shared_ptr<const msgs::Int> &msg)
      {
         if ( msg->data() < 20 )
         {
            LoadRoomByIndex(msg->data());
         }

         switch (msg->data())
         {


            case 99:
            {
               DeleteStaticModels();
               break;
            }
            case 101:case 102:case 103:case 104:case 105:case 106:case 107: // up to 7 robots
            {
               int robot_id = msg->data() % 100;
               ostringstream robot_name;
               robot_name << "pioneer_" << robot_id;
               bool already = false;
               // find if robot is already in the world
               physics::Model_V modelList(world->GetModels());
               for ( physics::Model_V::iterator it = modelList.begin(); it != modelList.end(); ++it )
               {
                  if ( robot_name.str().compare((*it)->GetName()) == 0 )
                  {
                     already = true;
                     ostringstream topicName;
                     topicName << "pioneer_" << robot_id;
                     float orient = 0;
                     float w = cos(orient/2), z = sin(orient/2);
                     string command = "rosservice call /gazebo/set_model_state '{model_state: { model_name: " + topicName.str() + ", pose: { position: { x: " + to_string(robot_id-3.0) + ", y: " + to_string(0) + ",z: 0 }, orientation: {x: 0, y: 0, z: "+ to_string(z) + ", w: " + to_string(w) +" } }, twist: { linear: {x: 0.0 , y: 0 ,z: 0 } , angular: { x: 0.0 , y: 0 , z: 0.0 } } , reference_frame: world } }'";
                     system(command.c_str());
                     //unsigned int microseconds = 4e6; // 2s
                     //usleep(microseconds);
                     break;
                  }
               } 
               // if there is no such a robot, add it
               if ( !already )
               {
                  ifstream file;
                  string sdf_string;
                  sdf::SDF mySDF;
                  sdf::ElementPtr model;
                  // Open sdf file and assign its content to a string
                  file.open("/root/.gazebo/models/pioneer2dx_with_sensors/model.sdf");
                  file.seekg(0, ios::end); 
                  sdf_string.reserve(file.tellg());
                  file.seekg(0, ios::beg);
                  sdf_string.assign((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
                  // Get model and change its name    
                  mySDF.SetFromString(sdf_string);
                  model = mySDF.Root()->GetElement( "model" );
                  robot_name.str("");
                  robot_name << "pioneer_" << robot_id;
                  model->GetAttribute( "name" )->SetFromString ( robot_name.str() );
                  // Add robot with a new name to the world at a given pose
                  math::Pose pose(robot_id-3.0, 0, 0, 0, 0, 0);
                  model->AddElement("pose")->Set(pose);
                  world->InsertModelSDF(mySDF);
                  file.close();
                  //world->InsertModelFile("model://pioneer2dx_with_sensors"); // insert model from .gazebo/models
               }   
               break;
            }
            case 201: case 202: case 203: case 204: case 205: case 206: case 207:
            {
                int robot_id = msg->data() % 200;
               ostringstream robot_name;
               robot_name << "pioneer_" << robot_id;
               //bool already = false;
               // find if robot is already in the world
               physics::Model_V modelList(world->GetModels());
               for ( physics::Model_V::iterator it = modelList.begin(); it != modelList.end(); ++it )
               {
                  if ( robot_name.str().compare((*it)->GetName()) == 0 )
                  {
                    float orient = 0;
                    float w = cos(orient/2), z = sin(orient/2);
                    float y = 3.5 + robot_id*0.5;
                    float x = 4;
                     ostringstream topicName;
                     topicName << "pioneer_" << robot_id;
                     string command = "rosservice call /gazebo/set_model_state '{model_state: { model_name: " + topicName.str() + ", pose: { position: { x: " + to_string(x) + ", y: " + to_string(y) + ",z: 0 }, orientation: {x: 0, y: 0, z: "+ to_string(z) + ", w: " + to_string(w) +" } }, twist: { linear: {x: 0.0 , y: 0 ,z: 0 } , angular: { x: 0.0 , y: 0 , z: 0.0 } } , reference_frame: world } }'";
                     system(command.c_str());
                  }
               } 
               break;
            }
/*
            case 1000:
            {
               ros::master::V_TopicInfo master_topics;
               ros::master::getTopics(master_topics);
               ROS_INFO("Hello World!");
               for ( ros::master::V_TopicInfo::iterator it = master_topics.begin(); it != master_topics.end(); it++ )
               {
                  const ros::master::TopicInfo& info = *it;
                  cout << "topic_" << it - master_topics.begin() << ": " << info.name << endl;
               }
               break;
            }
*/
         }
      }

   private:

      transport::NodePtr node;
      transport::SubscriberPtr subscriber;
      transport::PublisherPtr publisher;
      gazebo::transport::PublisherPtr publisherRobotPose;
      gazebo::transport::PublisherPtr publisherRobotPose2;
      gazebo::transport::PublisherPtr publisherRobotPose3;
      physics::WorldPtr world;
      int model_counter;
      ros::Subscriber rossub;
      ros::NodeHandle* rosnode;

   };

   GZ_REGISTER_WORLD_PLUGIN(WorldPluginProject)
}

