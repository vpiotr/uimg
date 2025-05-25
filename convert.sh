convert /home/piotr/progs/oss_projects/uimg/build/chart3d_aa.ppm /home/piotr/progs/oss_projects/uimg/build/chart3d_aa.png
cp /home/piotr/progs/oss_projects/uimg/build/chart3d_aa.png /home/piotr/progs/oss_projects/uimg/images/chart3d.png

cd /home/piotr/progs/oss_projects/uimg/build &&
echo "Converting PPM to PNG..."
which convert || echo "ImageMagick's convert tool not found"
if command -v convert &> /dev/null; then
  convert grid_chart.ppm grid_chart.png
  convert horizontal_chart.ppm horizontal_chart.png
  convert gaussian_chart.ppm gaussian_chart.png
  convert vertical_aa_chart.ppm vertical_aa_chart.png
  echo "Conversion complete."
else
  echo "Using alternative method to display images..."
  # If convert isn't available, we can still do a basic check
  file grid_chart.ppm horizontal_chart.ppm gaussian_chart.ppm vertical_aa_chart.ppm
fi

ls -la *.png

