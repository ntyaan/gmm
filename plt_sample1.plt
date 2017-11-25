plot "normal_distribution.txt" title "data" , "centers.txt" title "centers" pt 7 lc rgb "red"
set key below
set size ratio -1
replot
set terminal gif
set output "gif/1.gif"
replot
set output
quit
