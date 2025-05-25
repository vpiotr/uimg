echo grid_chart.ppm
# cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -charts 4 -layout grid -out build/grid_chart.ppm
echo horizontal_chart.ppm
# cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -charts 3 -layout horizontal -out build/horizontal_chart.ppm
echo vertical_aa_chart.ppm
# cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -charts 2 -layout vertical -aa -out build/vertical_aa_chart.ppm

#cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -out build/chart3d_def_noaa.ppm
#cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -out build/chart3d_def_aa.ppm -aa

#cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -function sinc -out build/chart3d_one_sinc.ppm
#cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -function gaussian -out build/chart3d_one_gaussian.ppm
#cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -function ripple -out build/chart3d_one_ripple.ppm
#cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -function saddle -out build/chart3d_one_saddle.ppm

# chart3d
echo chart3d_def_aa.ppm
cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -aa -out build/chart3d_def_aa.ppm
echo chart3d_4_charts.ppm
cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -charts 4 -layout grid -notitle -out build/chart3d_4_charts.ppm

# chart3d_demo split functions
echo chart3d_demo_gaussian.ppm
cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d_demo_gaussian -out build/chart3d_demo_gaussian.ppm
echo chart3d_demo_ripple.ppm
cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d_demo_ripple -out build/chart3d_demo_ripple.ppm
echo chart3d_demo_saddle.ppm
cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d_demo_saddle -out build/chart3d_demo_saddle.ppm
echo chart3d_demo_sinc.ppm
cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d_demo_sinc -out build/chart3d_demo_sinc.ppm

