plot "< awk '{if($3>$4&&$3>$5){print $1, $2;}}' result.txt" notitle, "< awk '{if($4>$3&&$4>$5){print $1, $2;}}' result.txt" notitle, "< awk '{if($5>$3&&$5>$4){print $1, $2;}}' result.txt" notitle, "centers.txt" title "centers" pt 7 lc rgb "red"
set key below
set size ratio -1
set terminal gif
set output ite.".gif" 
replot
set output
quit
