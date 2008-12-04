rm bench/python.txt
rm bench/tivi.txt

for i in $(seq 0 0)
do
    echo "Test $i: run"
    ./runpy.sh 2>&1 | tee -a bench/python.txt
    sleep 1
    ./runvm.sh 2>&1 | tee -a bench/tivi.txt
    sleep 1
    echo "Test $i: finished"
done

echo ", Python"
cat  bench/python.txt | bench/format.rb
echo
echo ", Tivi"
cat  bench/tivi.txt | bench/format.rb
