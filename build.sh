mkdir -p /home/piotr/progs/oss_projects/uimg/build && cd /home/piotr/progs/oss_projects/uimg/build && cmake .. && make clean && make -j$(nproc)
cd ..
cmake --build build
