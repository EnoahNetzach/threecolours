#include <iomanip>
#include <iostream>
#include <map>
#include <string>

#include <boost/algorithm/string.hpp>
#ifndef SERVER
#include <boost/program_options.hpp>
#endif // SERVER

#include "threecolours.h"

#ifndef SERVER
namespace po = boost::program_options;
#endif // SERVER

int main(int argc, char * argv[])
{
   if (argc == 1) {
      return -1;
   }
   std::string filename = argv[1];
   int size = 100;
   int frame = 10;
   double bucketThreshold = 15;
   double processThreshold = 150;
   bool show = false;
   std::string output = "json";

#ifndef SERVER
   po::options_description visible("Allowed options");
   visible.add_options()
      ("help,h", "produce help message")
      ("size,s", po::value< int >(& size)->default_value(100), "")
      ("frame,f", po::value< int >(& frame)->default_value(10), "")
      ("bthreshold,t", po::value< double >(& bucketThreshold)->default_value(15), "bucket threshold")
      ("sthreshold,p", po::value< double >(& processThreshold)->default_value(150), "process threshold")
      ("show,w", po::value< bool >(& show)->default_value(false), "")
      ("output,o", po::value< std::string >(& output)->default_value("xml"), "output type (json|xml|txt)")
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
       return 1;
   }
   else if (filename == "")
   {
      std::cout << "Usage: " << argv[0] << " [OPTIONS] FILE" << std::endl;
      return -1;
   }
#endif // SERVER

   tc::ThreeColours threeColours(filename, size, frame, bucketThreshold, processThreshold);

   auto colours = threeColours.run(show);

   std::map< std::string, int > outputTypes = {
      {"json", 0},
      {"xml", 1},
      {"txt", 2}
   };

   boost::algorithm::to_lower(output);

   if (outputTypes.count(output) == 0)
   {
      std::cout << "The option -o must be one of \"json\", \"xml\", \"txt\", " << output << " given" << std::endl;
      return -2;
   }
   switch (outputTypes.at(output))
   {
   default:
   case 0: // json
      std::cout << std::setfill('0')
                << "{\"foreground\":"
                << "{\"r\":" << (int)colours[0][2] << ",\"g\":" << (int)colours[0][1] << ",\"b\":" << (int)colours[0][0]
                << ",\"hex\":\"" << std::hex
                << std::setw(2) << (int)colours[0][2]
                << std::setw(2) << (int)colours[0][1]
                << std::setw(2) << (int)colours[0][0]
                << "\"},\"middleground\":{"
                << std::dec
                << "\"r\":" << (int)colours[1][2] << ",\"g\":" << (int)colours[1][1] << ",\"b\":" << (int)colours[1][0]
                << ",\"hex\":\"" << std::hex
                << std::setw(2) << (int)colours[1][2]
                << std::setw(2) << (int)colours[1][1]
                << std::setw(2) << (int)colours[1][0]
                << "\"},\"background\":{"
                << std::dec
                << "\"r\":" << (int)colours[2][2] << ",\"g\":" << (int)colours[2][1] << ",\"b\":" << (int)colours[2][0]
                << ",\"hex\":\"" << std::hex
                << std::setw(2) << (int)colours[2][2]
                << std::setw(2) << (int)colours[2][1]
                << std::setw(2) << (int)colours[2][0]
                << "\"}}" << std::endl;
      break;
   case 1: // xml
      std::cout << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<colours>"
                << "\t<foreground>\n"
                << "\t\t<red>" << (int)colours[0][2] << "</red>\n"
                << "\t\t<green>" << (int)colours[0][1] << "</green>\n"
                << "\t\t<blue>" << (int)colours[0][0] << "</blue>\n"
                << "\t</foreground>\n"
                << "\t<middleground>\n"
                << "\t\t<red>" << (int)colours[1][2] << "</red>\n"
                << "\t\t<green>" << (int)colours[1][1] << "</green>\n"
                << "\t\t<blue>" << (int)colours[1][0] << "</blue>\n"
                << "\t</middleground>\n"
                << "\t<background>\n"
                << "\t\t<red>" << (int)colours[2][2] << "</red>\n"
                << "\t\t<green>" << (int)colours[2][1] << "</green>\n"
                << "\t\t<blue>" << (int)colours[2][0] << "</blue>\n"
                << "\t</background>\n</colours>"
                << std::endl;
      break;
   case 2: // txt
      std::cout << (int)colours[0][2] << "," << (int)colours[0][1] << "," << (int)colours[0][0] << ";"
                << (int)colours[1][2] << "," << (int)colours[1][1] << "," << (int)colours[1][0] << ";"
                << (int)colours[2][2] << "," << (int)colours[2][1] << "," << (int)colours[2][0]
                << std::endl;
      break;
   }

	return 0;
}
