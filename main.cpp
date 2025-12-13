#include <SFML/Graphics.hpp>
#include <complex>
#include <chrono>
#include <iostream>
#include <vector>
#include <oneapi/tbb.h>
#include <fstream>
#include <algorithm>



using Complex = std::complex<double>;

// Structure to hold timing results
struct TimeResult {
    std::vector<int> grain_sizes;
    std::vector<double> times;
};

int mandelbrot(Complex const& c)
{
  int i = 0;
  auto z = c;
  for (; i != 256 && norm(z) < 4.; ++i) {
    z = z * z + c;
  }
  return i;
}

auto to_color(int k)
{
  return k < 256 ? sf::Color{static_cast<sf::Uint8>(10 * k), 0, 0}
                 : sf::Color::Black;
}


void parallel_loop(int rows_min, int rows_max, int cols_min, int cols_max, 
                   int grain_size, double delta_x, double delta_y, const Complex &top_left, sf::Image &auxiliary_image)
  // function to perform parallel computation of Mandelbrot set
{
  tbb::simple_partitioner partitioner{};
  tbb::parallel_for(
    // define ranges with grain size and lambda function to process each block, fragment contains the subimage processed by this task
    tbb::blocked_range2d<int>(rows_min, rows_max, grain_size, cols_min, cols_max, grain_size), [&](const tbb::blocked_range2d<int> &fragment)
      {
      for (int row = fragment.rows().begin(); row != fragment.rows().end(); ++row) // iterate over rows in the fragment
      {
        for (int column = fragment.cols().begin(); column != fragment.cols().end(); ++column) // iterate over columns in the fragment
        {
          auto k = mandelbrot(top_left + Complex{delta_x * column, delta_y * row}); // compute mandelbrot value
          auxiliary_image.setPixel(column, row, to_color(k)); // set pixel color, placeholder we are not using this image, just simulate how it affect1s performance
        }
      }
      },
      partitioner);
}


int main()
{
  // Image dimensions
  int const display_width{800};
  int const display_height{800};

  // Complex plane area to visualize
  Complex const top_left{-2.2, 1.5};
  Complex const lower_right{0.8, -1.5};
  auto const diff = lower_right - top_left;

  // Step sizes in the complex plane per pixel
  auto const delta_x = diff.real() / display_width;
  auto const delta_y = diff.imag() / display_height;

  // Create image
  sf::Image dummy_image;
  sf::Image image;
  image.create(display_width, display_height);
  dummy_image.create(display_width, display_height);

  // Timing results
  TimeResult time_result;

  // Grain size exploration loop
  for (int grain_size = 1; grain_size <= display_height; grain_size < 10 ? ++grain_size : grain_size += 10) {
    // Start timing
    auto start = std::chrono::steady_clock::now();
    // Perform parallel computation with current grain size
    parallel_loop(0, display_height, 0, display_width, grain_size, delta_x, delta_y, top_left, dummy_image);
    // End timing
    auto end = std::chrono::steady_clock::now();
    // Calculate elapsed time in microseconds
    std::chrono::duration<double> elapsed = end - start;

    // Store results
    time_result.grain_sizes.push_back(grain_size);
    time_result.times.push_back(elapsed.count());
    std::cout << "Grain size: " << grain_size << ", elapsed time: " << elapsed << "seconds" << std::endl;
  }

  // Finding minimum time and corresponding grain size
  auto min_it = std::min_element(time_result.times.begin(), time_result.times.end()); // get iterator to minimum time
  double min_time = *min_it; // get minimum time value
  int optimal_grain_size = time_result.grain_sizes[std::distance(time_result.times.begin(), min_it)]; // get corresponding grain size

  // Image generation with optimal grain size
  parallel_loop(0, display_height, 0, display_width, optimal_grain_size, delta_x, delta_y, top_left, image);


  // Save to file the timing results and the opimal values
    const std::string optimal_filename = "optimal_values.txt";
    std::ofstream optimal_file(optimal_filename);
    optimal_file << "Optimal grain size: " << optimal_grain_size << '\n';
    optimal_file << "Minimum time: " << min_time << '\n';
    optimal_file.close();

    const std::string filename = "timing_results.txt";
    std::ofstream file(filename);
    // Write header
    file << "grain_size time\n";
    for (size_t i = 0; i < time_result.grain_sizes.size() && i < time_result.times.size(); ++i) {
        file << time_result.grain_sizes[i] << ' ' << time_result.times[i] << '\n';
    }
    file.close();
  
  // Save the generated image
  image.saveToFile("OptimalMandelbrot.png");
}
