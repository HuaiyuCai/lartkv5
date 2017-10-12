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
/**
 * \file
 * \brief euclidean_cluster_extraction class declaration
 */

#ifndef _EUCLIDEAN_CLUSTER_H_
#define _EUCLIDEAN_CLUSTER_H_

// ROS
#include <ros/ros.h>
#include <sensor_msgs/PointCloud2.h>
#include <visualization_msgs/Marker.h>

// PCL
#include <pcl/conversions.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/surface/convex_hull.h>
#include <pcl/segmentation/extract_clusters.h>

// LAR
#include <points_from_volume/points_from_volume.h>
#include <pointcloud_segmentation/rviz_markers.h>

using namespace std;
using namespace pcl;

template<class T>
class euclidean_cluster_extraction
{
public:
    ///\brief class constructor
    euclidean_cluster_extraction()
    {
    };
    
    ///\brief class destructor
    ~euclidean_cluster_extraction()
    {
    };
    
    // VARIABLES
    ///\brief cluster tolerance for segmentation
    double cluster_tolerance;
    
    ///\brief minimum cluster points
    double min_cluster_size;
    
    ///\brief maximum cluster points
    double max_cluster_size;
    
    ///\brief pointer to publish markers
    ros::Publisher * pub_marker_ptr;
    
    ///\brief pointer to publish text markers
    ros::Publisher * pub_marker_text_ptr;
    
    // FUNCTIONS
    ///\brief callback function for pointcloud to cluster
    void callback_cloud (const sensor_msgs::PointCloud2Ptr& msg);
    
    
};

#include "euclidean_cluster.hpp"

#endif