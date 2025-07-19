if [ ! -d "./output" ]; then
  echo "Running CMAKE"
  mkdir output
  cmake . -B ./output
fi

./compileShaders.sh && cmake --build ./output && ./output/SokolTest
