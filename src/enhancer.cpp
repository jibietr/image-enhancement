#include <iostream>
#include <opencv2/opencv.hpp>
#include "image_enhancement.h"

void MyTimeOutput(const std::string &str, const clock_t &start_time, const clock_t &end_time)
{
    std::cout << str << (double)(end_time - start_time) / CLOCKS_PER_SEC << "s" << std::endl;
    return;
}

void MyTimeOutput(const std::string &str, const clock_t &time)
{
    std::cout << str << (double)(time) / CLOCKS_PER_SEC << "s" << std::endl;
    return;
}

char *getCmdOption(char **begin, char **end, const std::string &option)
{
    char **itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char **begin, char **end, const std::string &option)
{
    return std::find(begin, end, option) != end;
}

void help(char **argv)
{
    std::cout << "\n"
              << "Enhance images by removing black borders and improving image contrast\n"
              << "USAGE: " << argv[0] << " -i infile [options] -o . -m\n"
              << "  -i --input_file (string) Input file\n"
              << "  -o --out_dir (string) Output directory\n"
              << "  -c --contrast-algorithm (string) Contrast algorithm: LDR or IAGCWD\n"
              << std::endl;
}

int main(int argc, char **argv)
{

    // parse params
    if (cmdOptionExists(argv, argv + argc, "-h") or cmdOptionExists(argv, argv + argc, "--help"))
    {
        help(argv);
        return 0;
    }

    char *input_file;
    if (cmdOptionExists(argv, argv + argc, "-i"))
    {
        input_file = getCmdOption(argv, argv + argc, "-i");
    }
    else if (cmdOptionExists(argv, argv + argc, "--input_file"))
    {
        input_file = getCmdOption(argv, argv + argc, "--input_file");
    }
    else
    {
        std::cout << "Missing parameter input file" << std::endl;
        help(argv);
        return 0;
    }

    char *out_dir;
    if (cmdOptionExists(argv, argv + argc, "-o"))
    {
        out_dir = getCmdOption(argv, argv + argc, "-o");
    }
    else if (cmdOptionExists(argv, argv + argc, "--out_dir"))
    {
        out_dir = getCmdOption(argv, argv + argc, "--out_dir");
    }
    else
    {
        std::cout << "Missing parameter output directory" << std::endl;
        help(argv);
        return 0;
    }

    bool display_images = false;
    if (cmdOptionExists(argv, argv + argc, "-d") or cmdOptionExists(argv, argv + argc, "--debug"))
    {
        display_images = true;
    }
    bool verbose = false;
    if (cmdOptionExists(argv, argv + argc, "-v") or cmdOptionExists(argv, argv + argc, "--verbose"))
    {
        verbose = true;
    }

    // algorithm choice

    char *contrast_enhance_method;
    if (cmdOptionExists(argv, argv + argc, "-c"))
    {
        contrast_enhance_method = getCmdOption(argv, argv + argc, "-c");
    }
    else if (cmdOptionExists(argv, argv + argc, "--contrast-algorithm"))
    {
        contrast_enhance_method = getCmdOption(argv, argv + argc, "--contrast-algorithm");
    }
    else
    {
        std::cout << "Missing contrast enhancement method (LDR or IAGCWD)" << std::endl;
        help(argv);
        return 0;
    }

    // load image
    cv::Mat img = cv::imread(input_file, -1);

    if (img.empty())
    {
        std::cout << "Error loading input image: " + std::string(input_file) + " is not an image." << std::endl;
        return -1;
    }

    // remove borders
    cv::Mat img_no_borders;
    removeBordersFromImage(img, img_no_borders);

    // run one algorithm
    clock_t start_time, end_time;

    cv::Mat enhanced;

    std::string fname(input_file);
    std::size_t pos_extension = fname.rfind('.');
    std::string extension = fname.substr(pos_extension);
    std::size_t pos_slash = fname.rfind('/');

    std::string basename = fname.substr(pos_slash + 1, pos_extension - pos_slash - 1);
    std::string output_name;

    if (strncmp(contrast_enhance_method, "IAGCWD", 7) == 0)
    {
        start_time = clock();
        IAGCWD(img_no_borders, enhanced);
        output_name = std::string(out_dir) + "/" + basename + "_IAGCWD" + extension;
        end_time = clock();
    }
    else if (strncmp(contrast_enhance_method, "LDR", 3) == 0)
    {
        start_time = clock();
        LDR(img_no_borders, enhanced);
        end_time = clock();
        output_name = std::string(out_dir) + "/" + basename + "_LDR" + extension;
    }
    else
    {
        std::cout << "Contrast enhancement algorithm not implemented" << std::endl;
    }

    bool result = false;
    try
    {
        result = cv::imwrite(output_name, enhanced);
    }
    catch (const cv::Exception &ex)
    {
        std::cout << "Exception writing image" << std::endl;
    }
    if (result)
        std::cout << "Saved output file: " + output_name << std::endl;
    else
        std::cout << "ERROR. Cannot save file. Please, check that path exists: " + std::string(out_dir) << std::endl;

    return 0;
}