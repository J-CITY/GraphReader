cd ./report;
rm ./report.pdf;
cd ./core/ && nuweb ./main.w && cd ..;
mv ./core/main.cpp ./core/Graph.* ../;
pdflatex ./report.tex;
cd ./core/ && nuweb ./main.w && cd ..;
mv ./core/main.cpp ./core/Graph.* ../;
cd ..;
cmake . && make;
