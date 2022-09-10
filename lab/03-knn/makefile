build:
	gcc -Wall -Wno-unused-result -g classifier.c knn.o -o classifier -lm -pthread -lpthread -lraylib -lGL -ldl -lrt -lX11

start:
	./classifier -k 10 -d ./csv/ex_3000-size_4-classes_8-features_train.csv -t ./csv/ex_3000-size_4-classes_8-features_test.csv -n 1000