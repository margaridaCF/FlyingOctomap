#ifndef LTSTAR_H
#define LTSTAR_H


#include <octomap/OcTree.h>
#include <octomap/math/Vector3.h>
#include <unordered_set>
#include <map>
#include <ros/ros.h>
#include <sstream>
#include <vector>

#include <cmath>
#include <limits>

#include <resultSet.h>

namespace LazyThetaStarOctree{

	struct VectorComparatorOrder // for map
	{ 
		bool operator () (const octomath::Vector3 & lhs, const octomath::Vector3 & rhs) const 
		{ 
			float res = lhs.x() - rhs.x(); 
			if(res==0) 
			{ 
				res = lhs.y() - rhs.y(); 
			} 
			if(res==0) 
			{ 
				res = lhs.z() - rhs.z(); 
			} 
			// ROS_WARN_STREAM(   "Comparing "<< lhs << " with " << rhs << " ==> " << (res < 0)   );
			return res < 0; 
			// returns !0 if the first argument goes before the second argument in the strict weak ordering it defines, and false otherwise
		} 
	}; 

	struct VectorComparatorEqual // for unordered_map
	{ 
		bool operator () (const octomath::Vector3 & lhs, const octomath::Vector3 & rhs) const 
		{ 
			double scale = 0.0001;
			// ROS_WARN_STREAM(   std::setprecision(8) << "Distance from " << lhs << " and  " << rhs << " is " << lhs.distance(rhs) << " <= " << scale << " returning " << (lhs.distance(rhs) <= scale)   );
			return lhs.distance(rhs) <= scale;
			// returns !0 if the two container object keys passed as arguments are to be considered equal.
		} 
	};

	struct Vector3Hash
	{
	    std::size_t operator()(const octomath::Vector3 & v) const 
	    {
	    	int scale = 0.0001;
	        std::size_t hx = std::hash<float>{}( (int)(v.x() / scale) * scale );
	        std::size_t hy = std::hash<float>{}( (int)(v.y() / scale) * scale );
	        std::size_t hz = std::hash<float>{}( (int)(v.z() / scale) * scale );
            std::size_t return_value = ((hx 
               ^ (hy << 1)) >> 1)
               ^ (hz << 1);
            // std::cout << return_value << std::endl;
            return return_value;

	    }
	};



    void writeToFileWaypoint(octomath::Vector3 waypoint, double size, std::string waypointType)
    {
        std::ofstream pathWaypoints;
        pathWaypoints.open ("/home/mfaria/Margarida/20170802_lazyThetaStar/experimental data/waypoints_" + waypointType + ".csv", std::ofstream::out | std::ofstream::app);
        
        pathWaypoints << std::setprecision(5) << waypoint.x() << ", " << waypoint.y() << ", " << waypoint.z() << ", " << size << std::endl;
        
        pathWaypoints.close();
    }


	// ln 26 heuristics is g(s') + h(s')
	// g(s') has been previouly defined at ln 32 as g(parent(s)) + c(parent(s), s') 
	// h is straight line distance between goal and s vertex
	// h(s') is then s' distance to goal
	// heuristics is = found distance (parent(s)) + straight line distance (parent(s), s') + distance to goal (s')
	float calculateH (float distanceFromInitialPoint, float lineDistanceToFinalPoint) 
	{
		// ROS_WARN_STREAM("distanceFromInitialPoint + lineDistanceToFinalPoint <=> " << distanceFromInitialPoint << " + " << lineDistanceToFinalPoint);
		return distanceFromInitialPoint + lineDistanceToFinalPoint;
	}
	class ThetaStarNode
	{
	public:
		ThetaStarNode()
			: cell_size(0),
				distanceFromInitialPoint(std::numeric_limits<float>::max()), 
				lineDistanceToFinalPoint(0)
		 {
		 	// std::cout << total_ << " ThetaStarNode. Creating 1. " << *coordinates << std::endl; 
		 	parentNode = NULL;
		 	++total_;
		 	//std::cout << total_ << " ThetaStarNode " << std::endl; 
		 }
		ThetaStarNode(std::shared_ptr<octomath::Vector3> coordinates, double cell_size)
			: coordinates(coordinates), cell_size(cell_size),
				distanceFromInitialPoint(std::numeric_limits<float>::max()), 
				lineDistanceToFinalPoint(std::numeric_limits<float>::max())
		 {
		 	// std::cout << total_ << " ThetaStarNode. Creating 2. " << *coordinates << std::endl; 
		 	parentNode = NULL;
		 	++total_;
		 	//std::cout << total_ << " ThetaStarNode " << std::endl; 
		 }
		ThetaStarNode(std::shared_ptr<octomath::Vector3> coordinates, double cell_size,
			float distanceFromInitialPoint, float lineDistanceToFinalPoint)
			: coordinates(coordinates), cell_size(cell_size),
				distanceFromInitialPoint(distanceFromInitialPoint), 
				lineDistanceToFinalPoint(lineDistanceToFinalPoint)
		 {
		 	// std::cout << total_ << " ThetaStarNode. Creating 3. " << *coordinates << std::endl; 
		 	parentNode = NULL;
		 	++total_;
		 	//std::cout << total_ << " ThetaStarNode " << std::endl; 
		 }
		 // ThetaStarNode(ThetaStarNode const& original)
			// : cell_size(original.cell_size),
			// 	distanceFromInitialPoint(original.distanceFromInitialPoint), 
			// 	lineDistanceToFinalPoint(original.lineDistanceToFinalPoint),
			// 	parentNode(original.parentNode),
			// 	coordinates(original.coordinates)
		 // {
		 // 	++total_;
		 // 	//std::cout << total_ << " ThetaStarNode " << std::endl; 
		 // }
		 ~ThetaStarNode()
		 {
		 	if(total_>0) --total_;
		 	// std::cout << total_ << " ThetaStarNode. Another one bites the dust. " << *coordinates << std::endl; 
		 }


        ///  Display  and  <<
        std::string displayString() const
        {
          std::ostringstream oss;
          oss << parentNode;
          return "(" + std::to_string(coordinates->x()) + "; "
          			+ std::to_string(coordinates->y()) + "; "
          			+ std::to_string(coordinates->z()) + " )" 
          		+ " @ "+ std::to_string(cell_size) 
          		+ "; g = " + std::to_string(distanceFromInitialPoint)
          		+ "; to final = " + std::to_string(lineDistanceToFinalPoint)
          		+ "; parent is " + oss.str();
        }
        std::ostream& displayString(std::ostream& stream_out) const
        {
          stream_out << displayString() ;
          stream_out.precision(7);
          return stream_out;
        }
        bool hasSameCoordinates (std::shared_ptr<ThetaStarNode> other_node, double tolerance_marging) const
        {
        	double distance = coordinates->distance( *(other_node->coordinates));
        	return distance < tolerance_marging;
        }
		static size_t OustandingObjects() {return total_;}
		float calculateH_ () const;


		// TODO this should never change!!!
		std::shared_ptr<octomath::Vector3> const coordinates;	// always coordinates of center of cell
		double cell_size;
		float distanceFromInitialPoint;  // g
		float lineDistanceToFinalPoint; 
		std::shared_ptr<ThetaStarNode> parentNode;
	private:
		static size_t total_;
	};

	size_t ThetaStarNode::total_ = 0;

	std::ostream& operator<<(std::ostream& s, const ThetaStarNode& c){
		return c.displayString(s);
	};


	float ThetaStarNode::calculateH_ () const
	{
		// This strage way of organising the code is to use the emplace to insert in map
		return calculateH (distanceFromInitialPoint, lineDistanceToFinalPoint);
	}
}

#endif // LTSTAR_H