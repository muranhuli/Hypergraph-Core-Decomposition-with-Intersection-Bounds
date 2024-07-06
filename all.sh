#!/bin/bash
#g++ main.cpp -o main -g -std=c++17 -O3 -fopenmp -pthread
#dataset=( ActorMovies Flickr Genre Github IMDB Teams TVTropes Wikipedia Walmart )
#for datasetS in ${dataset[*]}
#do
#{
#    ./main $datasetS
#}&
#done
#wait
#if [ -f "main" ]; then
#    rm main
#fi
#!/bin/bash


# 可执行文件的路径
EXECUTABLE=./cmake-build-release/K_SCC_core

# 要处理的文件名列表 "IMDB"
FILES=("ActorMovies" "Genre" "Github" "Teams" "Wikipedia" "Wikipedia-0.2" "Wikipedia-0.4" "Wikipedia-0.6" "Wikipedia-0.8")
#FILES=("Wikipedia-0.2" "Wikipedia-0.4" "Wikipedia-0.6" "Wikipedia-0.8")

# 并行执行每个任务
for FILE in "${FILES[@]}"; do
    $EXECUTABLE $FILE &
done

# 等待所有后台任务完成
waith

echo "All tasks are completed."
