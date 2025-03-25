//==============================================================================================
// Originally written in 2016 by Peter Shirley <ptrshrl@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication
// along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==============================================================================================
#include "rtweekend.h"
#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"

#include <cstring>
#include <fstream>
#include <sstream>
#include <string>

int main(int argc, char * argv[]) {
  // Default path for sphere data
  std::string sphere_data_path = "sphere_data.txt";

  // Process command-line arguments
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--path") == 0) {
      if (i + 1 < argc) {
        sphere_data_path = argv[i + 1];
        i++;  // Skip the next argument (the path value)
      }
    }
  }

  hittable_list world;

  auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
  world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

  {
    std::ifstream infile(sphere_data_path);
    if (!infile.is_open()) {
      std::cerr << "Could not open file: " << sphere_data_path << std::endl;
      return 1;
    }
    std::string line;
    while (std::getline(infile, line)) {
      if (line.empty() || line[0] == '#') { continue; }
      std::istringstream iss(line);
      double x, y, z, radius;
      std::string material_type;
      if (!(iss >> x >> y >> z >> radius >> material_type)) {
        continue;  // Skip if line is malformed
      }
      shared_ptr<material> sphere_material;
      if (material_type == "lambertian") {
        double r, g, b;
        if (!(iss >> r >> g >> b)) { continue; }
        sphere_material = make_shared<lambertian>(color(r, g, b));
      } else if (material_type == "metal") {
        double r, g, b, fuzz;
        if (!(iss >> r >> g >> b >> fuzz)) { continue; }
        sphere_material = make_shared<metal>(color(r, g, b), fuzz);
      } else if (material_type == "dielectric") {
        double index;
        if (!(iss >> index)) { continue; }
        sphere_material = make_shared<dielectric>(index);
      }
      point3 center(x, y, z);
      world.add(make_shared<sphere>(center, radius, sphere_material));
    }
  }

  camera cam;

  cam.aspect_ratio      = 16.0 / 9.0;
  cam.image_width       = 800;
  cam.samples_per_pixel = 50;
  cam.max_depth         = 50;

  cam.vfov     = 20;
  cam.lookfrom = point3(13, 2, 3);
  cam.lookat   = point3(0, 0, 0);
  cam.vup      = vec3(0, 1, 0);

  cam.defocus_angle = 0.6;
  cam.focus_dist    = 10.0;

  cam.render(world);
}
