/******************************************************************************
 *  MIT License
 *
 *  Copyright (c) 2019 Abhinav Modi 

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *******************************************************************************/

/**@file talker.cpp
 * @brief Source file for Publisher node for the ROS tutorial
 *
 * Detailed description follows here.
 * @author     : Abhinav Modi
 * @created on : Oct 27, 2019
 */

#include <tf/transform_broadcaster.h>
#include <sstream>
#include "ros/ros.h"
#include "std_msgs/String.h"
#include "beginner_tutorials/modify_string.h"

#define PI 3.14

/**
 * Initialize default string
 */
extern std::string stringMsg = "Base string msg";

/**
 * @brief callback funciton for the service to modify string
 * @param req - request object for the service
 * @param resp - response object for the service
 * @return bool - to indicate success/failure of callback function
 */
bool modifyString(beginner_tutorials::modify_string::Request &req,  // NOLINT
                  beginner_tutorials::modify_string::Response &resp) { // NOLINT
  stringMsg = req.input;
  resp.output = req.input;

  // Display warning when string is updated using the services
  ROS_WARN_STREAM("The base output string has been updated");
  return true;
}

/**
 * This tutorial demonstrates how to send messages across the ROS system.
 */
int main(int argc, char **argv) {
  /**
   * The ros::init() function needs to see argc and argv so that it can perform
   * any ROS arguments and name remapping that were provided at the command line.
   * For programmatic remappings you can use a different version of init() which takes
   * remappings directly, but for most command-line programs, passing argc and argv is
   * the easiest way to do it.  The third argument to init() is the name of the node.
   *
   * You must call one of the versions of ros::init() before using any other
   * part of the ROS system.
   */
  ros::init(argc, argv, "talker");

  static tf::TransformBroadcaster br;
  tf::Transform transform;

  /**
   * Default publishing rate
   */
  int rate = 10;

  /**
   * Check if rate argument is passsed
   */
  if (argc == 2) {
    rate = atoi(argv[1]);
    std::cout << "rate " << rate <<"\n";
    ROS_DEBUG_STREAM("Input rate is: " << rate);          // Debug logger level

    if (rate < 0) {
      rate = 1;
      ROS_ERROR_STREAM("Invalid rate value");             // Error logger level
    }
  } else {
    ROS_WARN_STREAM("Using default publishing rate");     // Warning message
  }

  /**
   * NodeHandle is the main access point to communications with the ROS system.
   * The first NodeHandle constructed will fully initialize this node, and the last
   * NodeHandle destructed will close down the node.
   */
  ros::NodeHandle n;

  /**
   * The advertise() function is how you tell ROS that you want to
   * publish on a given topic name. This invokes a call to the ROS
   * master node, which keeps a registry of who is publishing and who
   * is subscribing. After this advertise() call is made, the master
   * node will notify anyone who is trying to subscribe to this topic name,
   * and they will in turn negotiate a peer-to-peer connection with this
   * node.  advertise() returns a Publisher object which allows you to
   * publish messages on that topic through a call to publish().  Once
   * all copies of the returned Publisher object are destroyed, the topic
   * will be automatically unadvertised.
   *
   * The second parameter to advertise() is the size of the message queue
   * used for publishing messages.  If messages are published more quickly
   * than we can send them, the number here specifies how many messages to
   * buffer up before throwing some away.
   */
  ros::Publisher chatter_pub = n.advertise<std_msgs::String>("chatter", 1000);

  /**
   * Advertise modify string service with the corresponding callback
   */
  auto server = n.advertiseService("modify_string", modifyString);

  ros::Rate loop_rate(rate);
  ROS_INFO_STREAM("Setting publishing rate");

  /**
   * A count of how many messages we have sent. This is used to create
   * a unique string for each message.
   */
  int count = 0;
  while (ros::ok()) {
    /**
     * This is a message object. You stuff it with data, and then publish it.
     */
    std_msgs::String msg;

    std::stringstream ss;
    ss << stringMsg << count;
    msg.data = ss.str();

    ROS_INFO_STREAM(msg.data.c_str());
    ROS_DEBUG_STREAM("Input rate is: " << rate);

    /**
     * The publish() function is how you send messages. The parameter
     * is the message object. The type of this object must agree with the type
     * given as a template parameter to the advertise<>() call, as was done
     * in the constructor above.
     */
    chatter_pub.publish(msg);

    /**
     * set transform
     */
    transform.setOrigin(tf::Vector3(1.0, 2.0, 3.0));
    tf::Quaternion q;
    q.setRPY(PI, PI/2, 2);
    transform.setRotation(q);
    br.sendTransform(tf::StampedTransform(transform,
                                          ros::Time::now(), "world", "talk"));

    ros::spinOnce();

    loop_rate.sleep();
    ++count;
  }

  /**
   * Fatal message if ROS node is not running
   */
  if (!ros::ok()) {
    ROS_FATAL_STREAM("ROS node is not running");
  }

  return 0;
}
