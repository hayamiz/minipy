rm bench/python.txt
rm bench/tivi.txt

for i in {0..9}
do
    echo "Test $i: run"
    ./runpy.sh 2>&1 | cat >> bench/python.txt
    sleep 5
    ./runvm.sh 2>&1 | cat >> bench/tivi.txt
    sleep 5
    echo "Test $i: finished"
done

echo ", Python"
cat  bench/python.txt | bench/format.rb
echo
echo ", Tivi"
cat  bench/tivi.txt | bench/format.rb
