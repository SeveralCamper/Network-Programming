TARGET_CLIENT := src/client.cpp
TARGET_SERVER := src/server.cpp

CC = g++ -Wall -Wextra

all: clean start

start:
	$(CC) -D _GNU_SOURCE src/server.cpp -o server
	$(CC) -pthread src/client.cpp -o client

clean:
	rm -rf server client log.txt

