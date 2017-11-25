plot "normal_distribution.txt" title "data"
set key below
set size ratio -1
replot
set terminal gif
set output "gif/0.gif"
replot
set output
quit
