/**
 * @file
 * @brief ptu control code. This is the actual code with messages for the ptu.
 */ 



#include <string>
#include <ros/ros.h>
#include <ptu46/ptu46_driver.h>
#include <sensor_msgs/JointState.h>

using namespace std;

namespace PTU46 {

/**
 * PTU46 ROS Package
 * Copyright (C) 2009 Erik Karulf (erik@cse.wustl.edu)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
class PTU46_Node {
    public:
        PTU46_Node(ros::NodeHandle& node_handle);
        ~PTU46_Node();

        // Service Control
        void Connect();
        bool ok() {
            return m_pantilt != NULL;
        }
        void Disconnect();

        // Service Execution
        void spinOnce();

        // Callback Methods
        void SetGoal(const sensor_msgs::JointState::ConstPtr& msg);

    protected:
        PTU46* m_pantilt;
        ros::NodeHandle m_node;
        ros::Publisher  m_joint_pub;
        ros::Subscriber m_joint_sub;
};

PTU46_Node::PTU46_Node(ros::NodeHandle& node_handle)
        :m_pantilt(NULL), m_node(node_handle) {
    // Empty Constructor
}

PTU46_Node::~PTU46_Node() {
    Disconnect();
}

/** Opens the connection to the PTU and sets appropriate parameters.
    Also manages subscriptions/publishers */
void PTU46_Node::Connect() {
    // If we are reconnecting, first make sure to disconnect
    if (ok()) {
        Disconnect();
    }

	
    // Publishers : Only publish the most recent reading
    m_joint_pub = m_node.advertise
                  <sensor_msgs::JointState>("state", 1);

    // Subscribers : Only subscribe to the most recent instructions
    m_joint_sub = m_node.subscribe
                  <sensor_msgs::JointState>("cmd", 1, &PTU46_Node::SetGoal, this);

// 	ros::spinOnce();
	
    // Query for serial configuration
    std::string port;
    m_node.param<std::string>("port", port, PTU46_DEFAULT_PORT);
    int baud;
    m_node.param("baud", baud, PTU46_DEFAULT_BAUD);

    // Connect to the PTU
    ROS_INFO("Attempting to connect to %s...", port.c_str());
    m_pantilt = new PTU46(port.c_str(), baud);
    ROS_ASSERT(m_pantilt != NULL);
    if (! m_pantilt->isOpen()) {
        ROS_ERROR("Could not connect to pan/tilt unit [%s]", port.c_str());
		Disconnect();
        return;
    }
    ROS_INFO("Connected!");

    m_node.setParam("min_tilt", m_pantilt->GetMin(PTU46_TILT));
    m_node.setParam("max_tilt", m_pantilt->GetMax(PTU46_TILT));
    m_node.setParam("min_tilt_speed", m_pantilt->GetMinSpeed(PTU46_TILT));
    m_node.setParam("max_tilt_speed", m_pantilt->GetMaxSpeed(PTU46_TILT));
    m_node.setParam("tilt_step", m_pantilt->GetResolution(PTU46_TILT));

    m_node.setParam("min_pan", m_pantilt->GetMin(PTU46_PAN));
    m_node.setParam("max_pan", m_pantilt->GetMax(PTU46_PAN));
    m_node.setParam("min_pan_speed", m_pantilt->GetMinSpeed(PTU46_PAN));
    m_node.setParam("max_pan_speed", m_pantilt->GetMaxSpeed(PTU46_PAN));
    m_node.setParam("pan_step", m_pantilt->GetResolution(PTU46_PAN));


    // Publishers : Only publish the most recent reading
    m_joint_pub = m_node.advertise
                  <sensor_msgs::JointState>("state", 1);

    // Subscribers : Only subscribe to the most recent instructions
    m_joint_sub = m_node.subscribe
                  <sensor_msgs::JointState>("cmd", 1, &PTU46_Node::SetGoal, this);

}

/** Disconnect */
void PTU46_Node::Disconnect() {
    if (m_pantilt != NULL) {
        delete m_pantilt;   // Closes the connection
        m_pantilt = NULL;   // Marks the service as disconnected
    }
}

/** Callback for getting new Goal JointState */
void PTU46_Node::SetGoal(const sensor_msgs::JointState::ConstPtr& msg) {
    if (! ok())
        return;

	bool set_pan_pos=false, set_tilt_pos=false, set_pan_speed=false, set_tilt_speed=false;
	double panpos, tiltpos, panspeed, tiltspeed;

	for (size_t i=0; i<msg->name.size(); i++)	
	{
		if (msg->name[i]==ros::names::remap("pan"))
		{
			if (msg->position.size()>i)
			{
				panpos=msg->position[i];	
				set_pan_pos = true;
			}

			if (msg->velocity.size()>i)
			{
				panspeed=msg->velocity[i];	
				set_pan_speed = true;
			}
		}	


		if (msg->name[i]==ros::names::remap("tilt"))
		{
			if (msg->position.size()>i)
			{
				tiltpos=msg->position[i];	
				set_tilt_pos = true;
			}

			if (msg->velocity.size()>i)
			{
				tiltspeed=msg->velocity[i];	
				set_tilt_speed = true;
			}
		}	

	}

	if (set_pan_pos)
	    m_pantilt->SetPosition(PTU46_PAN, panpos);

	if (set_tilt_pos)
	    m_pantilt->SetPosition(PTU46_TILT, tiltpos);

	if (set_pan_speed)
    m_pantilt->SetSpeed(PTU46_PAN, panspeed);

	if (set_tilt_speed)
		m_pantilt->SetSpeed(PTU46_TILT, tiltspeed);
}

/**
 * Publishes a joint_state message with position and speed.
 * Also sends out updated TF info.
 */
void PTU46_Node::spinOnce() {
    if (! ok())
        return;

    // Read Position & Speed
    double pan  = m_pantilt->GetPosition(PTU46_PAN);
    double tilt = m_pantilt->GetPosition(PTU46_TILT);

    double panspeed  = m_pantilt->GetSpeed(PTU46_PAN);
    double tiltspeed = m_pantilt->GetSpeed(PTU46_TILT);

    // Publish Position & Speed
    sensor_msgs::JointState joint_state;
    
    joint_state.header.stamp = ros::Time::now();
    joint_state.name.push_back(ros::names::remap("pan"));
    joint_state.position.push_back(pan);
    joint_state.velocity.push_back(panspeed);
    joint_state.name.push_back(ros::names::remap("tilt"));
    joint_state.position.push_back(tilt);
    joint_state.velocity.push_back(tiltspeed);
    m_joint_pub.publish(joint_state);

}

} // PTU46 namespace

int main(int argc, char** argv) {
    ros::init(argc, argv, "ptu");
    ros::NodeHandle n("~");

    // Connect to PTU
    PTU46::PTU46_Node ptu_node = PTU46::PTU46_Node(n);
    ptu_node.Connect();
    if (! ptu_node.ok())
        return -1;

    // Query for polling frequency
    int hz;
    n.param("hz", hz, PTU46_DEFAULT_HZ);
    ros::Rate loop_rate(hz);

    while (ros::ok() && ptu_node.ok()) {
        // Publish position & velocity
        ptu_node.spinOnce();

        // Process a round of subscription messages
        ros::spinOnce();

        // This will adjust as needed per iteration
        loop_rate.sleep();
    }

    if (! ptu_node.ok()) {
        ROS_ERROR("pan/tilt unit disconncted prematurely");
        return -1;
    }

    return 0;
}