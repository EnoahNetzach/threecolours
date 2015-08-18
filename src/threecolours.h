/*
 * ThreeColours.h
 *
 *  Created on: 28 Sep 2014
 *      Author: netzach
 */

#ifndef THREECOLOURS_H_
#define THREECOLOURS_H_

#include <array>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

#include <opencv2/core/core.hpp>

namespace tc
{

class ThreeColours
{
public:
   typedef std::array< cv::Vec3b, 3 > colours_type;
   typedef std::tuple< int, int, cv::Vec3b > tuplet_type;
   typedef std::vector< tuplet_type > bucket_type;
   typedef std::tuple< bucket_type, cv::Vec3b > bucket_tuple_type;
   typedef std::vector< bucket_tuple_type > buckets_type;
   typedef std::array< buckets_type, 2 > buckets_array_type;

   ThreeColours(const std::string & filename = "", int size = 100,
                int frame = 10, double bucketThreshold = 15,
                double foregroundThreshold = 80,
                double middlegroundThreshold = 45);

   colours_type run(bool show = false) throw (std::runtime_error);

   std::string & filename();
   const std::string & filename() const;
   int & size();
   const int & size() const;
   int & frame();
   const int & frame() const;
   double & bucketThreshold();
   const double & bucketThreshold() const;
   double & foregroundThreshold();
   const double & foregroundThreshold() const;
   double & middlegroundThreshold();
   const double & middlegroundThreshold() const;

protected:
   cv::Mat loadFile() const throw (std::runtime_error);
   buckets_array_type fillBuckets(const cv::Mat & image) const;
   buckets_type processBuckets(buckets_type frameBuckets,
                               buckets_type buckets) const;

private:
   std::string m_filename;
   int m_size;
   int m_frame;
   std::vector< double > m_knorm;
   double m_bucketThreshold;
   double m_foregroundThreshold;
   double m_middlegroundThreshold;
};

}

#endif // THREECOLOURS_H_
