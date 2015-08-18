/*
 * ThreeColours.cpp
 *
 *  Created on: 28 Sep 2014
 *      Author: netzach
 */

#include "threecolours.h"

#include <algorithm>
#include <cmath>
#include <sys/stat.h>
#include <utility>
#ifdef DEBUG
#include <iostream>
#include <boost/format.hpp>
#endif // DEBUG

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace tc {

template< typename P_ = int, typename T1_, typename T2_ >
double norm(T1_ v1, T2_ v2)
{
   return ::sqrt(
      ::pow((P_)v1[0] - (P_)v2[0], 2.0) +
      ::pow((P_)v1[1] - (P_)v2[1], 2.0) +
      ::pow((P_)v1[2] - (P_)v2[2], 2.0)
   );
}

template< typename P1_ = int, typename P2_, typename T1_, typename T2_ >
double norm(T1_ v1, T2_ v2, const std::vector< P2_ > & k)
{
   return ::sqrt(
      ::pow((P1_)v1[0] - (P1_)v2[0], 2.0) * k[0] +
      ::pow((P1_)v1[1] - (P1_)v2[1], 2.0) * k[1] +
      ::pow((P1_)v1[2] - (P1_)v2[2], 2.0) * k[2]
   );
}

}

using namespace tc;

ThreeColours::ThreeColours(
      const std::string & filename,
      int size, int frame,
      double bucketThreshold,
      double foregroundThreshold,
      double middlegroundThreshold
)
   : m_filename(filename)
   , m_size(size)
   , m_frame(frame)
   , m_knorm({2 / ::sqrt(6), 1. / 6, 1. / 6})
   , m_bucketThreshold(bucketThreshold)
   , m_foregroundThreshold(foregroundThreshold)
   , m_middlegroundThreshold(middlegroundThreshold)
{
}

auto ThreeColours::run(bool show) throw(std::runtime_error) -> colours_type
{
   auto && image = loadFile();

   auto && buckets = fillBuckets(image);

   auto && finalBuckets = processBuckets(buckets[0], buckets[1]);

   for (auto & bucket : finalBuckets)
   {
      cv::Mat mat(1, 1, image.type());
      mat.at< cv::Vec3b >(0, 0) = std::get< 1 >(bucket);
      cv::cvtColor(mat, mat, CV_YCrCb2BGR);
      std::get< 1 >(bucket) = mat.at< cv::Vec3b >(0, 0);
   }

   auto fCol = std::get< 1 >(finalBuckets[0]);
   auto mCol = std::get< 1 >(finalBuckets[1]);
   auto bCol = std::get< 1 >(finalBuckets[2]);

   if (show)
   {
      cv::Mat image2 = cv::imread(m_filename);
      cv::resize(image2, image2, cv::Size(400, 400), 0 ,0, cv::INTER_NEAREST);

      // add reflection
      cv::copyMakeBorder(image2, image2, 0, image2.rows, 0, 0, cv::BORDER_REFLECT);

      cv::copyMakeBorder(image2, image2, 50, 50, 550, 50, cv::BORDER_CONSTANT, cv::Scalar(bCol[0], bCol[1], bCol[2]));

      for (int x = 0; x < image2.cols; x++)
      {
         for (int y = 0; y < image2.rows; y++)
         {
            auto & p = image2.at< cv::Vec3b >(y, x);
            // reflection alpha
            if (x >= 550 and x < 950 and y >= 450 and y < 850)
            {
               p[0] = (int)bCol[0] * 3 / 4. + (int)p[0] / 4.;
               p[1] = (int)bCol[1] * 3 / 4. + (int)p[1] / 4.;
               p[2] = (int)bCol[2] * 3 / 4. + (int)p[2] / 4.;
            }
            // left gradient
            if (x >= 550 and x < 616)
            {
               double r = (x - 550) / 66.;
               p[0] = (int)bCol[0] * (1 - r) + (int)p[0] * r;
               p[1] = (int)bCol[1] * (1 - r) + (int)p[1] * r;
               p[2] = (int)bCol[2] * (1 - r) + (int)p[2] * r;
            }
            // right gradient
            if (x >= 884 and x < 950)
            {
               double r = (x - 884) / 66.;
               p[0] = (int)p[0] * (1 - r) + (int)bCol[0] * r;
               p[1] = (int)p[1] * (1 - r) + (int)bCol[1] * r;
               p[2] = (int)p[2] * (1 - r) + (int)bCol[2] * r;
            }
            // top gradient
            if (y >= 50 and y < 116)
            {
               double r = (y - 50) / 66.;
               p[0] = (int)bCol[0] * (1 - r) + (int)p[0] * r;
               p[1] = (int)bCol[1] * (1 - r) + (int)p[1] * r;
               p[2] = (int)bCol[2] * (1 - r) + (int)p[2] * r;
            }
            // bottom gradient
            if (y >= 450 and y < 850)
            {
               double r = (y - 450) / 400.;
               p[0] = (int)p[0] * (1 - r) + (int)bCol[0] * r;
               p[1] = (int)p[1] * (1 - r) + (int)bCol[1] * r;
               p[2] = (int)p[2] * (1 - r) + (int)bCol[2] * r;
            }
         }
      }

      cv::putText(image2, "Primary", cv::Point(75, 75), cv::FONT_HERSHEY_TRIPLEX, 1, cv::Scalar(fCol[0], fCol[1], fCol[2]), 3);
      cv::putText(image2, "Secondary", cv::Point(75, 175), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(mCol[0], mCol[1], mCol[2]), 2);

      cv::namedWindow("Example", CV_WINDOW_AUTOSIZE);
      cv::imshow("Example", image2);
      cv::waitKey();
      cv::destroyWindow("Example");
   }

   return {fCol, mCol, bCol};
}

std::string & ThreeColours::filename()
{
   return m_filename;
}

const std::string & ThreeColours::filename() const
{
   return m_filename;
}

int & ThreeColours::size()
{
   return m_size;
}

const int & ThreeColours::size() const
{
   return m_size;
}

int & ThreeColours::frame()
{
   return m_frame;
}

const int & ThreeColours::frame() const
{
   return m_frame;
}

double & ThreeColours::bucketThreshold()
{
   return m_bucketThreshold;
}

const double & ThreeColours::bucketThreshold() const
{
   return m_bucketThreshold;
}

double & ThreeColours::foregroundThreshold()
{
   return m_foregroundThreshold;
}

const double & ThreeColours::foregroundThreshold() const
{
   return m_foregroundThreshold;
}

double & ThreeColours::middlegroundThreshold()
{
   return m_middlegroundThreshold;
}

const double & ThreeColours::middlegroundThreshold() const
{
   return m_middlegroundThreshold;
}

cv::Mat ThreeColours::loadFile() const throw(std::runtime_error)
{
   struct stat buffer;
   if (stat(m_filename.c_str(), & buffer) != 0)
   {
      throw std::runtime_error("The file \"" + m_filename + "\" does not exists or could not be read.");
   }

   cv::Mat image = cv::imread(m_filename);
   cv::resize(image, image, cv::Size(m_size, m_size), 0 ,0, cv::INTER_NEAREST);
   cv::Mat image2(image.size(), image.type());
   cv::bilateralFilter(image, image2, 20, 40, 10);
   cv::cvtColor(image2, image2, CV_BGR2YCrCb);

   return image2;
}

auto ThreeColours::fillBuckets(const cv::Mat & image) const -> buckets_array_type
{
   buckets_type frameBuckets;
   buckets_type buckets;

   std::vector< std::array< int, 2> > pixels;
   for (int x = 0; x < image.size().width; x++)
   {
      for (int y = 0; y < image.size().height; y++)
      {
         pixels.push_back({x, y});
      }
   }

   for (auto pixel = pixels.begin(); pixel != pixels.end(); pixel = pixels.begin())
   {
      int x = (* pixel)[0];
      int y = (* pixel)[1];

      pixels.erase(pixel);

      auto p = image.at< cv::Vec3b >(y, x);

      bucket_type bucket;
      bucket_type frameBucket;

      if (x < m_frame or x > m_size - m_frame or y < m_frame or y > m_size - m_frame)
      {
         frameBucket.push_back(tuplet_type(x, y, p));
      }
      else
      {
         bucket.push_back(tuplet_type(x, y, p));
      }

      for (auto pixel1 = pixels.begin(); pixel1 != pixels.end(); )
      {
         int x1 = (* pixel1)[0];
         int y1 = (* pixel1)[1];

         auto p1 = image.at< cv::Vec3b >(y1, x1);

         if (norm(p, p1, m_knorm) < m_bucketThreshold)
         {
            pixels.erase(pixel1);

            bucket.push_back(tuplet_type(x1, y1, p1));
            if (x1 < m_frame or x1 > m_size - m_frame or y1 < m_frame or y1 > m_size - m_frame)
            {
               frameBucket.push_back(tuplet_type(x1, y1, p1));
            }
         }
         else
         {
            ++pixel1;
         }
      }

      if (bucket.size() > 5)
      {
         buckets.push_back(bucket_tuple_type(bucket, cv::Vec3b(0, 0, 0)));
      }
      if (frameBucket.size() > 20)
      {
         frameBuckets.push_back(bucket_tuple_type(frameBucket, cv::Vec3b(0, 0, 0)));
      }
   }

   return {frameBuckets, buckets};
}

auto ThreeColours::processBuckets(buckets_type frameBuckets, buckets_type buckets) const -> buckets_type
{
   bucket_tuple_type backgroundBucket;
   bucket_tuple_type foregroundBucket;
   bucket_tuple_type middlegroundBucket;

   for (auto & frameBucket : frameBuckets)
   {
      double pixel0 = (int)std::get< 1 >(frameBucket)[0];
      double pixel1 = (int)std::get< 1 >(frameBucket)[1];
      double pixel2 = (int)std::get< 1 >(frameBucket)[2];
      for (auto tuple : std::get< 0 >(frameBucket))
      {
         pixel0 += std::get< 2 >(tuple)[0];
         pixel1 += std::get< 2 >(tuple)[1];
         pixel2 += std::get< 2 >(tuple)[2];
      }
      std::get< 1 >(frameBucket)[0] = pixel0 / std::get< 0 >(frameBucket).size();
      std::get< 1 >(frameBucket)[1] = pixel1 / std::get< 0 >(frameBucket).size();
      std::get< 1 >(frameBucket)[2] = pixel2 / std::get< 0 >(frameBucket).size();
   }
   std::sort(frameBuckets.begin(), frameBuckets.end(), [](const bucket_tuple_type & b1, const bucket_tuple_type & b2) -> bool
   {
      bool order;
      if (std::get< 0 >(b1).size() == std::get< 0 >(b2).size())
      {
         order = (int)std::get< 1 >(b1)[0] > (int)std::get< 1 >(b2)[0];
      }
      else
      {
         order = std::get< 0 >(b1).size() > std::get< 0 >(b2).size();
      }

      return order;
   });

   backgroundBucket = frameBuckets[0];
   frameBuckets.clear();

   for (auto & bucket : buckets)
   {
      double pixel0 = (int)std::get< 1 >(bucket)[0];
      double pixel1 = (int)std::get< 1 >(bucket)[1];
      double pixel2 = (int)std::get< 1 >(bucket)[2];
      for (auto tuple : std::get< 0 >(bucket))
      {
         pixel0 += std::get< 2 >(tuple)[0];
         pixel1 += std::get< 2 >(tuple)[1];
         pixel2 += std::get< 2 >(tuple)[2];
      }
      std::get< 1 >(bucket)[0] = pixel0 / std::get< 0 >(bucket).size();
      std::get< 1 >(bucket)[1] = pixel1 / std::get< 0 >(bucket).size();
      std::get< 1 >(bucket)[2] = pixel2 / std::get< 0 >(bucket).size();
   }
   std::sort(buckets.begin(), buckets.end(), [this, backgroundBucket](const bucket_tuple_type & b1, const bucket_tuple_type & b2) -> bool
   {
      bool order;
      auto n1 = norm(std::get< 1 >(backgroundBucket), std::get< 1 >(b1), m_knorm);
      auto n2 = norm(std::get< 1 >(backgroundBucket), std::get< 1 >(b2), m_knorm);
      if ((n1 > m_foregroundThreshold) == (n2 > m_foregroundThreshold))
      {
//         order = n1 > n2;
         order = std::get< 0 >(b1).size() > std::get< 0 >(b2).size();
      }
      else
      {
         order = n1 > m_foregroundThreshold;
      }

      return order;
   });

   foregroundBucket = buckets[0];
   buckets.erase(buckets.begin());

   if (buckets.size() > 0)
   {
      std::sort(buckets.begin(), buckets.end(), [this, backgroundBucket](const bucket_tuple_type & b1, const bucket_tuple_type & b2)
      {
         bool order;
         auto n1 = norm(std::get< 1 >(backgroundBucket), std::get< 1 >(b1), m_knorm);
         auto n2 = norm(std::get< 1 >(backgroundBucket), std::get< 1 >(b2), m_knorm);
         if ((n1 > m_middlegroundThreshold) == (n2 > m_middlegroundThreshold))
         {
            order = n1 > n2;
//            order = std::get< 0 >(b1).size() > std::get< 0 >(b2).size();
         }
         else
         {
            order = n1 > m_middlegroundThreshold;
         }

         return order;
         return norm(std::get< 1 >(backgroundBucket), std::get< 1 >(b1)) > norm(std::get< 1 >(backgroundBucket), std::get< 1 >(b2));
      });
      middlegroundBucket = buckets[0];

      if (norm(std::get< 1 >(backgroundBucket), std::get< 1 >(middlegroundBucket), m_knorm)
          > norm(std::get< 1 >(backgroundBucket), std::get< 1 >(foregroundBucket), m_knorm))
      {
         std::swap(foregroundBucket, middlegroundBucket);
      }

      double previousVal = 0;
      while (norm(std::get< 1 >(backgroundBucket), std::get< 1 >(middlegroundBucket), m_knorm) < m_middlegroundThreshold)
      {
         double val = ((int)std::get< 1 >(middlegroundBucket)[0] - (int)std::get< 1 >(backgroundBucket)[0]);
         if (val != 0 && val != previousVal)
         {
#ifdef DEBUG
            std::cout << boost::format("val: %d\nmg: {%i, %i, %i} (%d / %d)\nbg: {%i, %i, %i}")
            % val
            % (int)std::get< 1 >(middlegroundBucket)[0]
            % (int)std::get< 1 >(middlegroundBucket)[1]
            % (int)std::get< 1 >(middlegroundBucket)[2]
            % norm(std::get< 1 >(backgroundBucket), std::get< 1 >(middlegroundBucket), m_knorm)
            % m_middlegroundThreshold
            % (int)std::get< 1 >(backgroundBucket)[0]
            % (int)std::get< 1 >(backgroundBucket)[1]
            % (int)std::get< 1 >(backgroundBucket)[2]
            << std::endl;
#endif // DEBUG
            std::get< 1 >(middlegroundBucket)[0] += 1 / val;
         }
         else
         {
#ifdef DEBUG
            std::cout << "mg = fg" << std::endl;
#endif // DEBUG
            middlegroundBucket = foregroundBucket;
            break;
         }
         previousVal = val;
      }
   }
   else
   {
      middlegroundBucket = foregroundBucket;
   }

   return {foregroundBucket, middlegroundBucket, backgroundBucket};
}
