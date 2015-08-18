#include <iomanip>
#include <iostream>
#include <map>
#include <string>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#ifndef SERVER
#include <boost/program_options.hpp>
#endif // SERVER

#include "threecolours.h"

#ifndef SERVER
namespace po = boost::program_options;
#endif // SERVER

enum ExtiValue
{
   OK_END = 0,
   OK_HELP = 1,
   ERROR_NO_FILE = -1,
   ERROR_WRONG_OUTPUT_FORMAT = -2,
};

enum class OutputType
{
   JSON,
   XML,
   CSV,
};

int main(int argc, char * argv[])
{
   if (argc == 1) {
      std::cerr << "Usage: " << argv[0] << " FILE" << std::endl;

      return ExtiValue::ERROR_NO_FILE;
   }
   std::string filename = argv[1];
   int size = 100;
   int frame = 10;
   double bucketThreshold = 15;
   double foregroundThreshold = 80;
   double middlegroundThreshold = 45;
   bool show = false;
   std::string output = "json";

#ifndef SERVER
   po::options_description visible("Allowed options");
   visible.add_options()
      ("help,h", "produce help message")
      ("size,s", po::value< int >(& size)->default_value(size), "the image will be resized to this dimension before computing")
      ("frame,r", po::value< int >(& frame)->default_value(frame), "the size of the border")
      ("bth,t", po::value< double >(& bucketThreshold)->default_value(bucketThreshold), "bucket threshold")
      ("fth,f", po::value< double >(& foregroundThreshold)->default_value(foregroundThreshold), "foreground threshold")
      ("mth,m", po::value< double >(& middlegroundThreshold)->default_value(middlegroundThreshold), "middleground threshold")
      ("show,w", "show a result example")
      ("output,o", po::value< std::string >(& output)->default_value("json"), "output type (json|xml|csv)")
   ;

   po::options_description hidden("Hidden options");
   hidden.add_options()
      ("file,i", po::value< std::string >(& filename), "input file")
   ;

   po::options_description cmdline_options;
   cmdline_options.add(visible).add(hidden);

   po::variables_map vm;
   po::store(po::command_line_parser(argc, argv).options(cmdline_options).run(), vm);
   po::notify(vm);

   if (vm.count("help")) {
       std::cout << cmdline_options << std::endl;
       return ExtiValue::OK_HELP;
   }
   else if (filename == "")
   {
      std::cerr << "Usage: " << argv[0] << " [OPTIONS] FILE" << std::endl;

      return ExtiValue::ERROR_NO_FILE;
   }

   if (vm.count("show")) {
      show = true;
   }
#endif // SERVER

   tc::ThreeColours threeColours(filename, size, frame, bucketThreshold, foregroundThreshold, middlegroundThreshold);

   auto colours = threeColours.run(show);

   std::map< std::string, OutputType > outputTypes = {
      {"json", OutputType::JSON},
      {"xml", OutputType::XML},
      {"csv", OutputType::CSV}
   };

   boost::algorithm::to_lower(output);

   if (outputTypes.count(output) == 0)
   {
      std::cerr << "The option -o must be one of \"json\", \"xml\", \"csv\", " << output << " given" << std::endl;

      return ExtiValue::ERROR_WRONG_OUTPUT_FORMAT;
   }

   std::string outputFormat;
   switch (outputTypes.at(output))
   {
   case OutputType::JSON:
      outputFormat =
            "{"
               "\"foreground\":{\"r\":%1$i,\"g\":%2$i,\"b\":%3$i,\"hex\":\"%1$02x%2$02x%3$02x\"},"
               "\"middleground\":{\"r\":%4$i,\"g\":%5$i,\"b\":%6$i,\"hex\":\"%4$02x%5$02x%6$02x\"},"
               "\"background\":{\"r\":%7$i,\"g\":%8$i,\"b\":%9$i,\"hex\":\"%7$02x%8$02x%9$02x\"}"
            "}";
      break;
   case OutputType::XML:
      outputFormat =
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
            "<colours>"
               "<foreground>"
                  "<red>%1$i</red>"
                  "<green>%2$i</green>"
                  "<blue>%3$i</blue>"
                  "<hex>%1$02x%2$02x%3$02x</hex>"
               "</foreground>"
               "<middleground>"
                  "<red>%4$i</red>"
                  "<green>%5$i</green>"
                  "<blue>%6$i</blue>"
                  "<hex>%4$02x%5$02x%6$02x</hex>"
               "</middleground>"
               "<background>"
                  "<red>%7$i</red>"
                  "<green>%8$i</green>"
                  "<blue>%9$i</blue>"
                  "<hex>%7$02x%8$02x%9$02x</hex>"
               "</background>"
            "</colours>";
      break;
   case OutputType::CSV:
      outputFormat =
            "%1$i,%2$i,%3$i,%1$02x%2$02x%3$02x\n"
            "%4$i,%5$i,%6$i,%4$02x%5$02x%6$02x\n"
            "%7$i,%8$i,%9$i,%7$02x%8$02x%9$02x\n";
      break;
   }

   std::cout << boost::format(outputFormat)
      % (int)colours[0][2] % (int)colours[0][1] % (int)colours[0][0]
      % (int)colours[1][2] % (int)colours[1][1] % (int)colours[1][0]
      % (int)colours[2][2] % (int)colours[2][1] % (int)colours[2][0]
      << std::endl;

   return ExtiValue::OK_END;
}
