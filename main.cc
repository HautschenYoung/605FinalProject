#include <iostream>
#include <fstream>
#include <time.h>
#include <sys/resource.h>
#include "sphere.h"
#include "hitable_list.h"
#include "float.h"
#include "camera.h"
#include "material.h"
#include <atomic>
#include <thread>
#include <vector>
#include <random>

#define MAXFLOAT FLT_MAX

vec3 color(const ray& r, hitable *world, int depth) {
    hit_record rec;
    if (world->hit(r, 0.001, MAXFLOAT, rec)) {
        ray scattered;
        vec3 attenuation;
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
             return attenuation*color(scattered, world, depth+1);
        }
        else {
            return vec3(0,0,0);
        }
    }
    else {
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5*(unit_direction.y() + 1.0);
        return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
    }
}


hitable *random_scene() {
    int n = 500;
    hitable **list = new hitable*[n+1];
    list[0] =  new sphere(vec3(0,-1000,0), 1000, new lambertian(vec3(0.5, 0.5, 0.5)));
    int i = 1;
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            float choose_mat = drand48();
            vec3 center(a+0.9*drand48(),0.2,b+0.9*drand48());
            if ((center-vec3(4,0.2,0)).length() > 0.9) {
                if (choose_mat < 0.8) {  // diffuse
                    list[i++] = new sphere(center, 0.2, new lambertian(vec3(drand48()*drand48(), drand48()*drand48(), drand48()*drand48())));
                }
                else if (choose_mat < 0.95) { // metal
                    list[i++] = new sphere(center, 0.2,
                            new metal(vec3(0.5*(1 + drand48()), 0.5*(1 + drand48()), 0.5*(1 + drand48())),  0.5*drand48()));
                }
                else {  // glass
                    list[i++] = new sphere(center, 0.2, new dielectric(1.5));
                }
            }
        }
    }

    list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
    list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
    list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));

    return new hitable_list(list,i);
}

int main(int argc, char** argv) {
    int spp = 0;
    if (argc == 1) {
        spp = 10;
    }
    else {
        spp = atoi(argv[1]);
    }

    int nx = 1200;
    int ny = 800;
    int ns = spp;

    std::cout << "Rendering a " << nx << "x" << ny << " image with " << ns << " samples per pixel ";
    std::vector<vec3> framebuffer(nx * ny);
    // std::cout << "P3\n" << nx << " " << ny << "\n255\n";
    hitable *list[5];
    float R = cos(M_PI/4);
    list[0] = new sphere(vec3(0,0,-1), 0.5, new lambertian(vec3(0.1, 0.2, 0.5)));
    list[1] = new sphere(vec3(0,-100.5,-1), 100, new lambertian(vec3(0.8, 0.8, 0.0)));
    list[2] = new sphere(vec3(1,0,-1), 0.5, new metal(vec3(0.8, 0.6, 0.2), 0.0));
    list[3] = new sphere(vec3(-1,0,-1), 0.5, new dielectric(1.5));
    list[4] = new sphere(vec3(-1,0,-1), -0.45, new dielectric(1.5));
    hitable *world = new hitable_list(list,5);
    world = random_scene();

    vec3 lookfrom(13,2,3);
    vec3 lookat(0,0,0);
    float dist_to_focus = 10.0;
    float aperture = 0.1;

    camera cam(lookfrom, lookat, vec3(0,1,0), 20, float(nx)/float(ny), aperture, dist_to_focus);

    // int num_threads = std::thread::hardware_concurrency();
    // std::cout << "Number of threads: " << num_threads << std::endl;
    // std::thread threads[num_threads];
    // int rows_per_thread = ny / num_threads;
    // auto renderRow = [&](uint32_t start_row, uint32_t end_row) {
    //     for (int j = start_row; j < end_row; j++) {
    //         for (int i = 0; i < nx; i++) {
    //             vec3 col(0, 0, 0);
    //             for (int s=0; s < ns; s++) {
    //                 std::mt19937 rng(std::random_device{}());
    //                 std::uniform_real_distribution<float> dist(0.0, 1.0);

    //                 float u = float(i + dist(rng)) / float(nx);
    //                 float v = float(j + dist(rng)) / float(ny);
    //                 // float u = float(i + drand48()) / float(nx);
    //                 // float v = float(j + drand48()) / float(ny);
    //                 ray r = cam.get_ray(u, v);
    //                 vec3 p = r.point_at_parameter(2.0);
    //                 col += color(r, world,0);
    //             }
    //             col /= float(ns);
    //             col = vec3( sqrt(col[0]), sqrt(col[1]), sqrt(col[2]) );
    //             // Store the color in the framebuffer
    //             framebuffer[j * nx + i] = col;
    //             // // Output the color to the console
    //             // std::cout << ir << " " << ig << " " << ib << "\n";
    //         }
    //     }
    // };
    // for(int i=0;i<num_threads - 1;i++){
    //     threads[i] = std::thread(renderRow,i*rows_per_thread,(i+1)*rows_per_thread);
    // }
    // threads[num_threads - 1] = std::thread(renderRow,(num_threads-1)*rows_per_thread,ny);
    // for(int i=0;i<num_threads;i++){
    //     threads[i].join();
    // }
    // // Output the framebuffer to the console
    // for (int j = ny - 1; j >= 0; j--) {
    //     for (int i = 0; i < nx; i++) {
    //         vec3 col = framebuffer[j * nx + i];
    //         int ir = int(255.99*col[0]);
    //         int ig = int(255.99*col[1]);
    //         int ib = int(255.99*col[2]);
    //         std::cout << ir << " " << ig << " " << ib << "\n";
    //     }
    // }
    
    // compute CPU time
    clock_t start, end;
    start = clock();

    for (int j = ny-1; j >= 0; j--) {
        for (int i = 0; i < nx; i++) {
            vec3 col(0, 0, 0);
            for (int s=0; s < ns; s++) {
                float u = float(i + drand48()) / float(nx);
                float v = float(j + drand48()) / float(ny);
                ray r = cam.get_ray(u, v);
                vec3 p = r.point_at_parameter(2.0);
                col += color(r, world,0);
            }
            col /= float(ns);
            col = vec3( sqrt(col[0]), sqrt(col[1]), sqrt(col[2]) );
            // Store the color in the framebuffer
            framebuffer[j * nx + i] = col;
            // int ir = int(255.99*col[0]);
            // int ig = int(255.99*col[1]);
            // int ib = int(255.99*col[2]);
            // std::cout << ir << " " << ig << " " << ib << "\n";
        }
    }
    end = clock();
    double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    std::cout << "CPU time: " << cpu_time_used << " seconds" << std::endl;

    // measure CPU memory usage
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    std::cerr << "CPU Memory Usage: " << usage.ru_maxrss / 1024.0 << " MB.\n";

    // store framebuffer to file
    std::ofstream outfile("single.ppm");
    outfile << "P3\n" << nx << " " << ny << "\n255\n";
    for (int j = ny-1; j >= 0; j--) {
        for (int i = 0; i < nx; i++) {
            vec3 col = framebuffer[j * nx + i];
            int ir = int(255.99*col[0]);
            int ig = int(255.99*col[1]);
            int ib = int(255.99*col[2]);
            outfile << ir << " " << ig << " " << ib << "\n";
        }
    }
    outfile.close();

    return 0;
}
