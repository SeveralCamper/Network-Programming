TARGET_CLIENT := src/client.cpp
TARGET_SERVER := src/server.cpp

CC = g++ -Wall -Wextra

all: clean start

start:
	$(CC) -D _GNU_SOURCE src/server.cpp -o bin/server
	$(CC) -pthread src/client.cpp -o bin/client

clean:
	rm -rf bin/server bin/client bin/log.txt

