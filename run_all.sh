cd /home/piotr/progs/oss_projects/uimg/build && ./text_demo -font ../fonts/courR12.bdf -out text_demo_output_v3.ppm
cd /home/piotr/progs/oss_projects/uimg/build && make filter_demo && ./filter_demo && convert filter_demo.ppm ../images/filter_demo_output.png
cd /home/piotr/progs/oss_projects/uimg/build && make sphere_3d && ./sphere_3d
cd /home/piotr/progs/oss_projects/uimg/build && cp sphere_3d.ppm /home/piotr/progs/oss_projects/uimg/images/ && cd /home/piotr/progs/oss_projects/uimg/images && convert sphere_3d.ppm sphere_3d.png
cd /home/piotr/progs/oss_projects/uimg/build && cmake .. && make 2d_line_chart_demo
cd /home/piotr/progs/oss_projects/uimg/build && ./2d_line_chart_demo -font ../fonts/courR12.bdf -charts 2 -thickness 1 -out 2d_line_chart_aa.ppm -aa
cd /home/piotr/progs/oss_projects/uimg/build && ./2d_line_chart_demo -font ../fonts/courR12.bdf -charts 2 -thickness 1 -out 2d_line_chart_noaa.ppm

cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -charts 4 -layout grid -out build/grid_chart.ppm
cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -charts 3 -layout horizontal -out build/horizontal_chart.ppm
cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -charts 2 -layout vertical -aa -out build/vertical_aa_chart.ppm

cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -out build/chart3d_def_noaa.ppm
cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -out build/chart3d_def_aa.ppm -aa

cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -function sinc -out build/chart3d_one_sinc.ppm
cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -function gaussian -out build/chart3d_one_gaussian.ppm
cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -function ripple -out build/chart3d_one_ripple.ppm
cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -function saddle -out build/chart3d_one_saddle.ppm

cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d_demo_sinc -out build/chart3d_demo_sinc.ppm
cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d_demo_gaussian -out build/chart3d_demo_gaussian.ppm
cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d_demo_ripple -out build/chart3d_demo_ripple.ppm
cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d_demo_saddle -out build/chart3d_demo_saddle.ppm
