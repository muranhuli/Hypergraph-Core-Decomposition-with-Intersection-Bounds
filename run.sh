# ActorMovies Flickr Genre Github IMDB Teams TVTropes Wikipedia Walmart
g++ main.cpp -o main -g -std=c++17 -O3 -fopenmp -pthread
./main ActorMovies

if [ -f "main" ]; then
    rm main
fi
if [ -d "main.dSYM" ]; then
    rm -r ./main.dSYM
fi