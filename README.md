Three Colours
=============
Extracts three colors from an image: border background, middle and foreground.

## Configurations
There are three configurations:

### Debug
not optimized, somewhat more verbose, used only for development

### Release
usable from the shell, it has more parameters that could be fine tuned from command line
```
Usage: three_colours.exe [OPTIONS] FILE

Allowed options:
  -h [ --help ]               produce help message
  -s [ --size ] arg (=100)    the image will be resized to this dimension 
                              before computing
  -r [ --frame ] arg (=10)    the size of the border
  -t [ --bth ] arg (=15)      bucket threshold
  -f [ --fth ] arg (=80)      foreground threshold
  -m [ --mth ] arg (=45)      middleground threshold
  -w [ --show ]               show a result example
  -o [ --output ] arg (=json) output type (json|xml|csv)

Hidden options:
  -i [ --file ] arg     input file
```

### Server
the only inputt is the file name, the only output is a JSON array with the data
```
Usage: three_colours.exe FILE
```

## Compilation
For the compilation are are neede the following includes:
  - *[boost/algorithm](http://www.boost.org/doc/libs/1_56_0/libs/algorithm/doc/html/index.html)*
  - *[boost/format](http://www.boost.org/doc/libs/1_56_0/libs/format/)*
  - *[opencv2/core](http://docs.opencv.org/2.4.6/modules/core/doc/core.html)*
  - *[opencv2/imgproc](http://docs.opencv.org/2.4.6/modules/imgproc/doc/imgproc.html)*
  - *[opencv2/highgui](http://docs.opencv.org/2.4.6/modules/highgui/doc/highgui.html)*
  - *[boost/program_options](http://www.boost.org/doc/libs/1_56_0/doc/html/program_options.html)* (only in Debug and Release)

And the following libraries:
  - *opencv_core*
  - *opencv_imgproc*
  - *opencv_highgui*
  - *boost_program_options* (only in Debug and Release)

## Documentation
The main class is **`tc::ThreeColours`**, all the parameters passed in the constructor (with signature
**`tc::ThreeColours::ThreeColours(const std::string &, int, int, double, double, double)`**) are optional and can be configured later
with reference setters.

In order to work at least the filename must be set.

The the computation can be started with **`std::array< cv::Vec3b, 3 > tc::ThreeColours::run(bool)`**.
The result is an array of the three extracted colors in the format used by opencv (usually *BGR*).
