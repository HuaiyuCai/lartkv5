/**************************************************************************************************
 Software License Agreement (BSD License)

 Copyright (c) 2011-2013, LAR toolkit developers - University of Aveiro - http://lars.mec.ua.pt
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, are permitted
 provided that the following conditions are met:

  *Redistributions of source code must retain the above copyright notice, this list of
   conditions and the following disclaimer.
  *Redistributions in binary form must reproduce the above copyright notice, this list of
   conditions and the following disclaimer in the documentation and/or other materials provided
   with the distribution.
  *Neither the name of the University of Aveiro nor the names of its contributors may be used to
   endorse or promote products derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************************************/
/** \brief A nodelet to implement point cloud filtering
 *  \file
 *  \author Tiago Talhada
 *  \date June 2012
 */

#include <pointcloud_segmentation/filtering.h>

int main(int argc, char **argv)
{
    // Set up ROS.
    ros::init(argc, argv, "listener");
    ros::NodeHandle n;
    
    // Variables from launch file
    string pointcloud_input;
    string pointcloud_output;
    string laser_input;
    double min_x, max_x, min_y, max_y;
    
    // Create a filtering class object and make the proper setup
    filtering<PointXYZRGB> * filt = new filtering<PointXYZRGB> ();
        
    // Initialize ros parameters
    ros::NodeHandle private_node_handle_("~");
    private_node_handle_.param("pointcloud_input", pointcloud_input, string("pointcloud_input"));
    private_node_handle_.param("pointcloud_output", pointcloud_output, string("pointcloud_output"));
    private_node_handle_.param("frame_id", filt->frame_id, string("frame_id"));
    
    private_node_handle_.param("voxel_size", filt->voxel_size, double(filt->voxel_size));
    private_node_handle_.param("min_x", min_x, double(min_x));
    private_node_handle_.param("max_x", max_x, double(max_x));
    private_node_handle_.param("min_y", min_y, double(min_y));
    private_node_handle_.param("max_y", max_y, double(max_y));
    private_node_handle_.param("min_z", filt->min_z, double(filt->min_z));
    private_node_handle_.param("max_z", filt->max_z, double(filt->max_z));
    
    // Set the convex hull properties
    PointXYZRGB point; point.z=0.0;
    point.x=min_x; point.y=max_y;
    filt->convex_hull.points.push_back(point);
    point.x=max_x; point.y=max_y;
    filt->convex_hull.points.push_back(point);
    point.x=max_x; point.y=min_y;
    filt->convex_hull.points.push_back(point);
    point.x=min_x; point.y=min_y;
    filt->convex_hull.points.push_back(point);
    filt->convex_hull.header.frame_id=filt->frame_id;
    
    
    // Subscribe a pointcloud message
    ros::Subscriber sub_cloud = n.subscribe(pointcloud_input.c_str(), 1000, &filtering<PointXYZRGB>::filter, filt);
        
    // Publish a pointcloud message
    ros::Publisher pub_message = n.advertise<sensor_msgs::PointCloud2>(pointcloud_output.c_str(), 10);
    
    // Pass a pub pointer to the class object
    filt->pub_ptr=&pub_message;
    
    // Pass a listener transform to class object
    tf::TransformListener listener_center_bumper;
    filt->transform_listener_ptr=&listener_center_bumper;
    
    // Main loop.
    while (n.ok())
    {
	ros::spinOnce();
    }
    
    return 0;
}